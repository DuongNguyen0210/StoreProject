#include "ui/ThongKe.h"
#include "models/User.h"
#include "models/Customer.h"
#include "ui_ThongKe.h"
#include "dialogs/billdetaildialog.h"
#include "dialogs/StockProductDialog.h"
#include <QString>
#include <QHeaderView>
#include <QDateTime>
#include <QDate>
#include <QMap>
#include <QtCharts>

ThongKe::ThongKe(Store* store, QWidget *parent) : QDialog(parent), ui(new Ui::ThongKe), m_store(store)
{
    ui->setupUi(this);

    setupTable();
    loadBillHistory();
    setupDashboard();

    connect(ui->billHistoryTable, &QTableView::doubleClicked, this, &ThongKe::onBillDoubleClicked);
    connect(ui->btnStockDetails, &QPushButton::clicked, this, &ThongKe::showStockDetails);
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

    for (auto it = history.rbegin(); it != history.rend(); ++it)
    {
        const Bill* bill = *it;
        if (!bill)
            continue;

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
    updateKPICards();

    ui->cboRevenueTimeframe->clear();
    ui->cboRevenueTimeframe->addItem("1 Tuần");
    ui->cboRevenueTimeframe->addItem("1 Tháng");
    ui->cboRevenueTimeframe->addItem("1 Năm");
    ui->cboRevenueTimeframe->setCurrentIndex(1);

    connect(ui->cboRevenueTimeframe, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ThongKe::onRevenueTimeframeChanged);

    createRevenueChart();
    createTop5ProductsChart();
    createWarningsChart();
}

void ThongKe::updateKPICards()
{
    double totalRevenue = m_store->getTotalRevenue();
    ui->lblRevenue->setText(QString("%1 đ").arg(QString::number(totalRevenue, 'f', 0)));

    double totalProfit = calculateTotalProfit();
    ui->lblProfit->setText(QString("%1 đ").arg(QString::number(totalProfit, 'f', 0)));

    double totalCost = calculateTotalCost();

    double profitMargin = 0.0;
    if (totalCost > 0)
        profitMargin = (totalProfit / totalCost) * 100.0;

    QString marginText = QString("Tỉ lệ lời: %1%").arg(QString::number(profitMargin, 'f', 1));
    ui->lblProfitMargin->setText(marginText);
    int billCount = m_store->getBillHistory().size();
    ui->lblBillCount->setText(QString::number(billCount));
}

void ThongKe::onRevenueTimeframeChanged()
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

    if (timeframeIndex == 0)
    {
        startDate = today.addDays(-6);
        format = "dd/MM";
    }
    else if(timeframeIndex == 1)
    {
        startDate = today.addDays(-29);
        format = "dd/MM";
    }
    else
    {
        startDate = today.addMonths(-11);
        startDate = QDate(startDate.year(), startDate.month(), 1);
        format = "MM/yyyy";
        isMonthly = true;
    }

    QMap<QDate, double> revenueData;
    const std::vector<Bill*>& history = m_store->getBillHistory();

    for (const Bill* bill : history)
    {
        QDate date = bill->getCreatedDate().date();

        if (date >= startDate && date <= today)
        {
            if (isMonthly)
            {
                QDate monthKey(date.year(), date.month(), 1);
                revenueData[monthKey] += bill->getTotal();
            }
            else
            {
                revenueData[date] += bill->getTotal();
            }
        }
    }

    QLineSeries *series = new QLineSeries();
    series->setName("Doanh thu");

    double maxVal = 0;

    if (isMonthly)
    {
        QDate current = startDate;
        while (current <= today || (current.year() == today.year() && current.month() == today.month()))
        {
            double val = revenueData.value(current, 0.0);
            QDateTime dt(current, QTime(0, 0));
            series->append(dt.toMSecsSinceEpoch(), val);
            if (val > maxVal)
                maxVal = val;
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
            if (val > maxVal)
                maxVal = val;
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
    axisX->setTickCount(timeframeIndex == 0 ? 7 : (timeframeIndex == 1 ? 15 : 12));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Trục Y - Tiền
    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Doanh thu (đ)");
    axisY->setLabelFormat("%i");
    axisY->setRange(0, maxVal > 0 ? maxVal * 1.1 : 1000000);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QLinearGradient gradient(0, 0, 0, 1);
    gradient.setColorAt(0.0, QColor(0x14, 0xB8, 0xA6));
    gradient.setColorAt(1.0, QColor(0x0D, 0x94, 0x88));
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    series->setPen(QPen(QBrush(gradient), 3));

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->chartRevenue->setChart(chart);
    ui->chartRevenue->setRenderHint(QPainter::Antialiasing);
}

void ThongKe::createTop5ProductsChart()
{
    QMap<QString, int> productSales;

    const auto& history = m_store->getBillHistory();
    for (const Bill* bill : history)
    {
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

    if (productSales.isEmpty())
    {
        QChart *chart = new QChart();
        chart->setTitle("Chưa có dữ liệu bán hàng");
        ui->chartTop5->setChart(chart);
        ui->chartTop5->setRenderHint(QPainter::Antialiasing);
        return;
    }

    QList<QPair<QString, int>> sortedProducts;
    for (auto it = productSales.begin(); it != productSales.end(); ++it)
        sortedProducts.append(qMakePair(it.key(), it.value()));

    std::sort(sortedProducts.begin(), sortedProducts.end(),
        [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
          return a.second > b.second;
        });

    int top = qMin(5, sortedProducts.size());

    QBarSeries *series = new QBarSeries();
    QBarSet *set = new QBarSet("Số lượng");

    QStringList categories;
    for (int i = 0; i < top; ++i)
    {
        *set << sortedProducts[i].second;
        categories << sortedProducts[i].first;
    }

    QLinearGradient gradient(0, 0, 0, 1);
    gradient.setColorAt(0.0, QColor(0x10, 0xB9, 0x81));
    gradient.setColorAt(1.0, QColor(0x05, 0x96, 0x69));
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

    ui->chartTop5->setChart(chart);
    ui->chartTop5->setRenderHint(QPainter::Antialiasing);
}

void ThongKe::createWarningsChart()
{
    int outOfStock = 0;
    int criticalLow = 0;
    int low = 0;
    int adequate = 0;

    m_store->forEachProduct([&](const QString&, Product* p) {
        if (!p->getIsActive()) return;
        int qty = p->getQuantity();

        if (qty == 0)
            outOfStock++;
        else if (qty >= 1 && qty <= 5)
            criticalLow++;
        else if (qty >= 6 && qty <= 10)
            low++;
        else
            adequate++;
    });

    ui->lblCountGreen->setText(QString::number(adequate));
    ui->lblCountYellow->setText(QString::number(low));
    ui->lblCountOrange->setText(QString::number(criticalLow));
    ui->lblCountRed->setText(QString::number(outOfStock));

    QPieSeries *series = new QPieSeries();

    if (outOfStock > 0) {
        QPieSlice *slice = series->append(QString("Hết hàng (%1)").arg(outOfStock), outOfStock);
        slice->setBrush(QColor(0xEF, 0x44, 0x44));  // Đỏ (#EF4444)
    }

    if (criticalLow > 0) {
        QPieSlice *slice = series->append(QString("Gần hết (%1)").arg(criticalLow), criticalLow);
        slice->setBrush(QColor(0xF5, 0x9E, 0x0B));  // Cam (#F59E0B)
    }

    if (low > 0) {
        QPieSlice *slice = series->append(QString("Sắp hết (%1)").arg(low), low);
        slice->setBrush(QColor(0xEA, 0xB3, 0x08));  // Vàng (#EAB308)
    }

    if (adequate > 0) {
        QPieSlice *slice = series->append(QString("Đủ hàng (%1)").arg(adequate), adequate);
        slice->setBrush(QColor(0x10, 0xB9, 0x81));  // Lục (#10B981)
    }

    if (outOfStock == 0 && criticalLow == 0 && low == 0 && adequate == 0)
        series->append("Chưa có sản phẩm", 1);

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
    double totalProfit = 0.0;

    const auto& history = m_store->getBillHistory();
    for (const Bill* bill : history)
    {
        if (!bill) continue;

        const auto& items = bill->getItems();
        double tierDiscount = bill->getTierDiscountPercent();

        for (const BillItem& item : items)
        {
            Product* p = item.getProduct();
            if (p)
            {
                double actualUnitPrice = item.getUnitPrice() * (1.0 - tierDiscount / 100.0);
                double profit = (actualUnitPrice - item.getImportPrice()) * item.getQuantity();
                totalProfit += profit;
            }
        }
    }

    return totalProfit;
}

double ThongKe::calculateTotalCost()
{
    double totalCost = 0.0;
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
                double cost = item.getImportPrice() * item.getQuantity();
                totalCost += cost;
            }
        }
    }

    return totalCost;
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

void ThongKe::showStockDetails()
{
    StockProductDialog stockDialog(m_store, this);
    stockDialog.exec();
}

