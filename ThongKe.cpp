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
    // Thống kê hàng tồn kho: Tốt / Thấp / Hết hạn sắp tới
    int goodStock = 0;
    int lowStock = 0;
    int nearExpiry = 0;

    QDate today = QDate::currentDate();
    QDate warningDate = today.addDays(7); // Cảnh báo trong vòng 7 ngày

    m_store->forEachProduct([&](const QString&, Product* p) {
        if (!p) return;

        int qty = p->getQuantity();

        // Kiểm tra hạn sử dụng
        Food* f = dynamic_cast<Food*>(p);
        Beverage* b = dynamic_cast<Beverage*>(p);

        bool isExpiringSoon = false;
        if (f)
        {
            QDate expiry = QDate::fromString(f->getExpiryDate(), "dd/MM/yyyy");
            if (expiry.isValid() && expiry <= warningDate && expiry >= today)
                isExpiringSoon = true;
        }
        else if (b)
        {
            QDate expiry = QDate::fromString(b->getExpiryDate(), "dd/MM/yyyy");
            if (expiry.isValid() && expiry <= warningDate && expiry >= today)
                isExpiringSoon = true;
        }

        if (isExpiringSoon)
            nearExpiry++;
        else if (qty < 10)
            lowStock++;
        else
            goodStock++;
    });

    // ✅ SỬA: Kiểm tra nếu không có sản phẩm nào
    if (goodStock == 0 && lowStock == 0 && nearExpiry == 0)
    {
        QChart *chart = new QChart();
        chart->setTitle("Chưa có dữ liệu sản phẩm");
        ui->chartWarnings->setChart(chart);
        ui->chartWarnings->setRenderHint(QPainter::Antialiasing);
        return;
    }

    // Tạo biểu đồ tròn (Donut Chart)
    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.4); // Donut style

    if (goodStock > 0) {
        QPieSlice *sliceGood = series->append("Tốt", goodStock);
        sliceGood->setBrush(QColor("#10B981")); // Emerald - Tốt
        sliceGood->setLabelVisible(true);
        sliceGood->setLabelColor(Qt::black);
        sliceGood->setLabel(QString("%1 (%2)").arg("Tốt").arg(goodStock));
    }

    if (lowStock > 0) {
        QPieSlice *sliceLow = series->append("Sắp hết", lowStock);
        sliceLow->setBrush(QColor("#F59E0B")); // Amber - Cảnh báo
        sliceLow->setLabelVisible(true);
        sliceLow->setLabelColor(Qt::black);
        sliceLow->setLabel(QString("%1 (%2)").arg("Sắp hết").arg(lowStock));
    }

    if (nearExpiry > 0) {
        QPieSlice *sliceExpiry = series->append("Gần hết hạn", nearExpiry);
        sliceExpiry->setBrush(QColor("#EF4444")); // Rose - Nguy hiểm
        sliceExpiry->setLabelVisible(true);
        sliceExpiry->setLabelColor(Qt::black);
        sliceExpiry->setLabel(QString("%1 (%2)").arg("Gần hết hạn").arg(nearExpiry));
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // ✅ SỬA: Không xóa widget, chỉ set chart vào QChartView hiện có
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
