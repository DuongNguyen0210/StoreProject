#include "ThongKe.h"
#include "User.h"
#include "Customer.h"
#include "ui_ThongKe.h"
#include "BillDetailDialog.h"
#include "Food.h"
#include "Beverage.h"
#include <QString>
#include <QHeaderView>
#include <QDateTime>
#include <QDate>
#include <QMap>
#include <QtCharts>

ThongKe::ThongKe(Store* store, QWidget *parent)
    : QDialog(parent), ui(new Ui::ThongKe), m_store(store)
{
    ui->setupUi(this);
    setupTable();
    loadBillHistory();

    setupDashboard();

    connect(ui->billHistoryTable, &QTableView::doubleClicked,
            this, &ThongKe::onBillDoubleClicked);
}

ThongKe::~ThongKe()
{
    delete ui;
}

void ThongKe::setupTable()
{
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(6);
    m_model->setHeaderData(0, Qt::Horizontal, "Mã HĐ");
    m_model->setHeaderData(1, Qt::Horizontal, "Tên Khách");
    m_model->setHeaderData(2, Qt::Horizontal, "Ngày Tạo");
    m_model->setHeaderData(3, Qt::Horizontal, "Giờ Tạo");
    m_model->setHeaderData(4, Qt::Horizontal, "Tổng Tiền");
    m_model->setHeaderData(5, Qt::Horizontal, "Nhân Viên");

    ui->billHistoryTable->setModel(m_model);
    ui->billHistoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->billHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->billHistoryTable->horizontalHeader()->setStretchLastSection(true);
    ui->billHistoryTable->setAlternatingRowColors(true);
}

void ThongKe::loadBillHistory()
{
    m_model->removeRows(0, m_model->rowCount());

    const auto& history = m_store->getBillHistory();

    for (const Bill* bill : history)
    {
        if (!bill) continue;

        QList<QStandardItem*> row;
        row << new QStandardItem(bill->getId());

        QString customerName = "Khách lẻ";
        if (bill->getCustomer())
            customerName = bill->getCustomer()->getName();
        row << new QStandardItem(customerName);

        QString dateStr = bill->getCreatedDate().toString("dd/MM/yyyy");
        row << new QStandardItem(dateStr);

        QString timeStr = bill->getCreatedDate().toString("HH:mm:ss");
        row << new QStandardItem(timeStr);

        QString totalStr = QString::number(bill->getTotal(), 'f', 0) + " đ";
        row << new QStandardItem(totalStr);

        User * u = bill->getCreatedBy();
        if(u)
            row << new QStandardItem(u->getName());
        else
            row << new QStandardItem("N/A");

        m_model->appendRow(row);
    }
}

void ThongKe::setupDashboard()
{
    // Cập nhật KPI Cards
    updateKPICards();

    // Tạo các biểu đồ
    // ComboBox chọn thời gian
    ui->cboRevenueTimeframe->clear();
    ui->cboRevenueTimeframe->addItem("1 Tuần");
    ui->cboRevenueTimeframe->addItem("1 Tháng");
    ui->cboRevenueTimeframe->addItem("1 Năm");
    ui->cboRevenueTimeframe->setCurrentIndex(1); // Mặc định 1 Tháng

    connect(ui->cboRevenueTimeframe, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ThongKe::onRevenueTimeframeChanged);

    createRevenueChart();
    createTop5ProductsChart();
    createWarningsChart();
}

void ThongKe::updateKPICards()
{
    // Tổng doanh thu
    double totalRevenue = m_store->getTotalRevenue();
    ui->lblRevenue->setText(QString("%1 đ")
                                .arg(QString::number(totalRevenue, 'f', 0)));

    // Lợi nhuận ước tính
    double totalProfit = calculateTotalProfit();
    ui->lblProfit->setText(QString("%1 đ")
                               .arg(QString::number(totalProfit, 'f', 0)));

    // Số hóa đơn
    int billCount = m_store->getBillHistory().size();
    ui->lblBillCount->setText(QString::number(billCount));
}

void ThongKe::onRevenueTimeframeChanged(int index)
{
    createRevenueChart();
}

void ThongKe::createRevenueChart()
{
    int timeframeIndex = ui->cboRevenueTimeframe->currentIndex();
    
    QDate today = QDate::currentDate();
    QDate startDate;
    QString format;
    bool isMonthly = false;

    // Xác định khoảng thời gian
    if (timeframeIndex == 0) // 1 Tuần
    {
        startDate = today.addDays(-6);
        format = "dd/MM";
    }
    else if (timeframeIndex == 2) // 1 Năm
    {
        startDate = today.addMonths(-11);
        startDate = QDate(startDate.year(), startDate.month(), 1); // Đầu tháng
        format = "MM/yyyy";
        isMonthly = true;
    }
    else // 1 Tháng (Default)
    {
        startDate = today.addDays(-29);
        format = "dd/MM";
    }

    // Lấy dữ liệu
    QMap<QDate, double> revenueData;
    const std::vector<Bill*>& history = m_store->getBillHistory();

    for (const Bill* bill : history)
    {
        if (!bill) continue;
        QDate date = bill->getCreatedDate().date();
        
        if (date >= startDate && date <= today)
        {
            if (isMonthly)
            {
                // Gom nhóm theo tháng (ngày đầu tháng)
                QDate monthKey(date.year(), date.month(), 1);
                revenueData[monthKey] += bill->getTotal(); // ✅ Fixed: getTotal() instead of getTotalAmount()
            }
            else
            {
                // Gom nhóm theo ngày
                revenueData[date] += bill->getTotal(); // ✅ Fixed: getTotal() instead of getTotalAmount()
            }
        }
    }

    QLineSeries *series = new QLineSeries();
    series->setName("Doanh thu");

    double maxVal = 0;

    // Tạo các điểm dữ liệu liên tục
    if (isMonthly)
    {
        QDate current = startDate;
        while (current <= today || (current.year() == today.year() && current.month() == today.month()))
        {
            double val = revenueData.value(current, 0.0);
            QDateTime dt(current, QTime(0, 0));
            series->append(dt.toMSecsSinceEpoch(), val);
            if (val > maxVal) maxVal = val;
            current = current.addMonths(1);
        }
    }
    else
    {
        for (QDate date = startDate; date <= today; date = date.addDays(1))
        {
            double val = revenueData.value(date, 0.0);
            QDateTime dt(date, QTime(0, 0));
            series->append(dt.toMSecsSinceEpoch(), val);
            if (val > maxVal) maxVal = val;
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Trục X - Thời gian
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setFormat(format);
    axisX->setTitleText("Thời gian");
    axisX->setTickCount(timeframeIndex == 0 ? 7 : (timeframeIndex == 1 ? 6 : 12)); 
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Trục Y - Tiền
    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Doanh thu (đ)");
    axisY->setLabelFormat("%i");
    axisY->setRange(0, maxVal > 0 ? maxVal * 1.1 : 1000000); // Thêm 10% đỉnh
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Màu sắc hiện đại - Teal gradient
    QLinearGradient gradient(0, 0, 0, 1);
    gradient.setColorAt(0.0, QColor("#14B8A6")); // Teal-500
    gradient.setColorAt(1.0, QColor("#0D9488")); // Teal-600
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    series->setPen(QPen(QBrush(gradient), 3));

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->chartRevenue->setChart(chart);
    ui->chartRevenue->setRenderHint(QPainter::Antialiasing);
}

void ThongKe::createTop5ProductsChart()
{
    // Tính toán số lượng bán của từng sản phẩm
    QMap<QString, int> productSales;

    const auto& history = m_store->getBillHistory();
    for (const Bill* bill : history)
    {
        if (!bill) continue;

        const auto& items = bill->getItems();
        for (const BillItem& item : items)
        {
            Product* p = item.getProduct();
            if (p)
            {
                productSales[p->getName()] += item.getQuantity();
            }
        }
    }

    // ✅ SỬA: Kiểm tra nếu không có dữ liệu
    if (productSales.isEmpty())
    {
        QChart *chart = new QChart();
        chart->setTitle("Chưa có dữ liệu bán hàng");
        ui->chartTop5->setChart(chart);
        ui->chartTop5->setRenderHint(QPainter::Antialiasing);
        return;
    }

    // Sắp xếp và lấy top 5
    QList<QPair<QString, int>> sortedProducts;
    for (auto it = productSales.begin(); it != productSales.end(); ++it)
    {
        sortedProducts.append(qMakePair(it.key(), it.value()));
    }

    std::sort(sortedProducts.begin(), sortedProducts.end(),
              [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
                  return a.second > b.second;
              });

    // Lấy top 5
    int top = qMin(5, sortedProducts.size());

    QBarSeries *series = new QBarSeries();
    QBarSet *set = new QBarSet("Số lượng");

    QStringList categories;
    for (int i = 0; i < top; ++i)
    {
        *set << sortedProducts[i].second;
        categories << sortedProducts[i].first;
    }

    // Gradient cho bar
    QLinearGradient gradient(0, 0, 0, 1);
    gradient.setColorAt(0.0, QColor("#10B981")); // Emerald-500
    gradient.setColorAt(1.0, QColor("#059669")); // Emerald-600
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    set->setBrush(gradient);

    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Số lượng");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);

    // ✅ SỬA: Không xóa widget, chỉ set chart vào QChartView hiện có
    ui->chartTop5->setChart(chart);
    ui->chartTop5->setRenderHint(QPainter::Antialiasing);
}

void ThongKe::createWarningsChart()
{
    // Count products by stock status
    int outOfStock = 0;        // Quantity = 0
    int criticalLow = 0;       // 1-5
    int low = 0;               // 6-10
    int adequate = 0;          // > 10
    
    m_store->forEachProduct([&](const QString&, Product* p) {
        if (!p || !p->getIsActive()) return;
        
        int qty = p->getQuantity();
        
        if (qty == 0) {
            outOfStock++;
        }
        else if (qty >= 1 && qty <= 5) {
            criticalLow++;
        }
        else if (qty >= 6 && qty <= 10) {
            low++;
        }
        else {
            adequate++;
        }
    });
    
    // Create pie chart
    QPieSeries *series = new QPieSeries();
    
    if (outOfStock > 0) {
        QPieSlice *slice = series->append(QString("❌ Hết hàng (%1)").arg(outOfStock), outOfStock);
        slice->setBrush(QColor("#EF4444"));  // Red
        slice->setLabelVisible(true);
    }
    
    if (criticalLow > 0) {
        QPieSlice *slice = series->append(QString("⚠️ Gần hết (1-5) (%1)").arg(criticalLow), criticalLow);
        slice->setBrush(QColor("#F59E0B"));  // Orange
        slice->setLabelVisible(true);
    }
    
    if (low > 0) {
        QPieSlice *slice = series->append(QString("⚡ Sắp hết (6-10) (%1)").arg(low), low);
        slice->setBrush(QColor("#EAB308"));  // Yellow
        slice->setLabelVisible(true);
    }
    
    if (adequate > 0) {
        QPieSlice *slice = series->append(QString("✅ Đủ hàng (>10) (%1)").arg(adequate), adequate);
        slice->setBrush(QColor("#10B981"));  // Green
        slice->setLabelVisible(true);
    }
    
    // If no products
    if (outOfStock == 0 && criticalLow == 0 && low == 0 && adequate == 0) {
        QPieSlice *slice = series->append("Chưa có sản phẩm", 1);
        slice->setBrush(QColor("#9CA3AF"));
    }
    
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);
    
    ui->chartWarnings->setChart(chart);
    ui->chartWarnings->setRenderHint(QPainter::Antialiasing);
}

double ThongKe::calculateTotalProfit()
{
    // Tính tổng lợi nhuận từ các hóa đơn
    double totalProfit = 0.0;

    const auto& history = m_store->getBillHistory();
    for (const Bill* bill : history)
    {
        if (!bill) continue;

        const auto& items = bill->getItems();
        for (const BillItem& item : items)
        {
            Product* p = item.getProduct();
            if (p)
            {
                // Lợi nhuận = (Giá bán - Giá gốc) * Số lượng
                double profit = (item.getUnitPrice() - p->getImportPrice()) * item.getQuantity();
                totalProfit += profit;
            }
        }
    }

    return totalProfit;
}

void ThongKe::onBillDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString billId = m_model->item(index.row(), 0)->text();

    const auto& history = m_store->getBillHistory();
    Bill* selectedBill = nullptr;

    for (Bill* bill : history)
    {
        if (bill && bill->getId() == billId)
        {
            selectedBill = bill;
            break;
        }
    }

    if (selectedBill)
    {
        BillDetailDialog detailDialog(selectedBill, this);
        detailDialog.exec();
    }
}
