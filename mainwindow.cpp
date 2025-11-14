#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHeaderView>
#include <QStandardItem>
#include <QVariant>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QDebug>

#include <qstring.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtableview.h>

#include "BillItem.h"
#include "Exceptions.h"
#include "Payment.h"
#include "Customer.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), model(nullptr), curTableProduct(0)
{
    ui->setupUi(this);

    setupTable();
    store.addProduct(new Food("", "Bánh mì", 10000, 50, "01/01/2026"));
    store.addProduct(new Food("", "Xúc xích", 12000, 40, "15/01/2026"));
    store.addProduct(new Beverage("", "Coca-Cola", 15000, 30, 330));
    store.addProduct(new HouseholdItem("", "Nước rửa chén", 30000, 20, 12));

    store.addCustomer(new Customer("C1", "Khách Vip", "0905123456", 110000));
    store.addCustomer(new Customer("C2", "Khách Thường", "0905654321", 10000));

    ui->dockMenu->hide();
    ui->dockOrder->hide();
    loadProductsFromStore(0);
    connect(ui->SearchText, &QLineEdit::returnPressed, this, &MainWindow::on_BtnSearch_clicked);


    currentBill = new Bill(nullptr);
    setupHoaDonTable();

    ui->stackedWidgeOrder->setCurrentIndex(curTableProduct);

    connect(ui->tableView, &QTableView::doubleClicked, this, &MainWindow::onSanPhamDoubleClicked);
    connect(ui->tableViewOrder, &QTableView::doubleClicked, this, &MainWindow::onRemoveSanPhamDoubleClicked);

    connect(ui->txtSearchCustomer, &QLineEdit::returnPressed, this, &MainWindow::onTimKhachPressed);
    connect(ui->btnDungDiem, &QPushButton::clicked, this, &MainWindow::onDungDiemClicked);
    connect(ui->ExportOrder, &QPushButton::clicked, this, &MainWindow::onXuatHoaDonClicked);

    connect(ui->btnBack, &QPushButton::clicked, this, &MainWindow::onQuayLaiClicked);
    connect(ui->btnCash, &QPushButton::clicked, this, &MainWindow::onThanhToanTienMatClicked);
    connect(ui->btnCard, &QPushButton::clicked, this, &MainWindow::onThanhToanTheClicked);
    connect(ui->btnExport, &QPushButton::clicked, this, &MainWindow::onThanhToanClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete currentBill;
}

void MainWindow::setupHoaDonTable()
{
    modelHoaDon = new QStandardItemModel(this);
    modelHoaDon->setColumnCount(3);
    modelHoaDon->setHeaderData(0, Qt::Horizontal, "Tên SP");
    modelHoaDon->setHeaderData(1, Qt::Horizontal, "SL");
    modelHoaDon->setHeaderData(2, Qt::Horizontal, "Thành tiền");
    ui->tableViewOrder->setModel(modelHoaDon);
    ui->tableViewOrder->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::setupLastBill()
{
    modelLastBill = new QStandardItemModel(this);
    modelLastBill->setColumnCount(3);
    modelLastBill->setHeaderData(0, Qt::Horizontal, "Tên SP");
    modelLastBill->setHeaderData(1, Qt::Horizontal, "SL");
    modelLastBill->setHeaderData(2, Qt::Horizontal, "Thành tiền");
    ui->tableLastBill->setModel(modelLastBill);
    ui->tableLastBill->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::updateHoaDonView()
{
    modelHoaDon->removeRows(0, modelHoaDon->rowCount());

    const auto& items = currentBill->getItems();
    for (const BillItem& item : items)
    {
        QList<QStandardItem*> row;
        row << new QStandardItem(item.getProduct()->getName());
        row << new QStandardItem(QString::number(item.getQuantity()));
        row << new QStandardItem(QString::number(item.getLineTotal()));
        modelHoaDon->appendRow(row);
    }

    double subTotal = currentBill->getSubTotal();
    double finalTotal = currentBill->getTotal();
    if(currentBill->getCustomer())
    {
        ui->lblTenKhach->setText(currentBill->getCustomer()->getName());
        ui->lblDiemKhach->setText(QString("Điểm Tích Lũy: %1").arg(currentBill->getCustomer()->getPoints()));
    }
    ui->TotalBefore->setText(QString("Tổng tiền ban đầu: %1").arg(subTotal));
    ui->TotalAfter->setText(QString("Tổng tiền thanh toán: %1").arg(finalTotal));
}

void MainWindow::updateLastBillView()
{
    modelLastBill->removeRows(0, modelLastBill->rowCount());
    const auto& items = currentBill->getItems();
    qDebug() << "size of items: " << items.size();
    for (const BillItem& item : items)
    {
        QList<QStandardItem*> row;
        row << new QStandardItem(item.getProduct()->getName());
        row << new QStandardItem(QString::number(item.getQuantity()));
        row << new QStandardItem(QString::number(item.getLineTotal()));
        modelLastBill->appendRow(row);
    }

    double subTotal = currentBill->getSubTotal();
    double finalTotal = currentBill->getTotal();
    ui->TotalBefore_2->setText(QString("Tổng tiền ban đầu: %1").arg(subTotal));
    ui->TotalAfter_2->setText(QString("Tổng tiền thanh toán: %1").arg(finalTotal));
}

void MainWindow::resetHoaDon()
{
    ui->txtSearchCustomer->clear();
    ui->btnDungDiem->setEnabled(false);
    ui->stackedWidgeOrder->setCurrentIndex(0);
    ui->lblTenKhach->setText("Khách lẻ");
    ui->lblDiemKhach->setText(nullptr);
    ui->TotalBefore->setText(nullptr);
    ui->TotalAfter->setText(nullptr);
    std::vector<BillItem> billitems = currentBill->getItems();
    for(size_t i = 0; i < billitems.size(); i++)
    {
        Product* p = store.findProductByName(billitems[i].getProduct()->getName());
        p->setQuantity(p->getQuantity() + billitems[i].getQuantity());
    }
    Customer *c = currentBill->getCustomer();
    if(c && currentBill->getCheck())
        c->setPoints(c->getPoints() + 100000);

    delete currentBill;
    currentBill = new Bill(nullptr);
    updateHoaDonView();
}


void MainWindow::setupTable()
{
    model = new QStandardItemModel(this);
    model->setColumnCount(8);
    model->setHeaderData(0, Qt::Horizontal, "Mã");
    model->setHeaderData(1, Qt::Horizontal, "Tên sản phẩm");
    model->setHeaderData(2, Qt::Horizontal, "Loại");
    model->setHeaderData(3, Qt::Horizontal, "Giá");
    model->setHeaderData(4, Qt::Horizontal, "Số lượng");
    model->setHeaderData(5, Qt::Horizontal, "Thể tích");
    model->setHeaderData(6, Qt::Horizontal, "Hạn sử dụng");
    model->setHeaderData(7, Qt::Horizontal, "Thời hạn bảo hành (tháng)");
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::loadProductsFromStore(int typeFilter)
{
    model->removeRows(0, model->rowCount());
    store.forEachProduct([&](const QString&, Product* p)
                         {
                             if (!p) return;
                             bool ok = false;
                             Food* f = dynamic_cast<Food*>(p);
                             Beverage* b = dynamic_cast<Beverage*>(p);
                             HouseholdItem* h = dynamic_cast<HouseholdItem*>(p);
                             if (typeFilter == 0) ok = true;
                             else if (typeFilter == 1 && f) ok = true;
                             else if (typeFilter == 2 && b) ok = true;
                             else if (typeFilter == 3 && h) ok = true;
                             if (!ok) return;

                             QList<QStandardItem*> row;
                             row << new QStandardItem(p->getId());
                             row << new QStandardItem(p->getName());
                             QString typeName;
                             if (f) typeName = "Đồ ăn";
                             else if (b) typeName = "Thức uống";
                             else if (h) typeName = "Đồ gia dụng";
                             else typeName = "Khác";
                             row << new QStandardItem(typeName);
                             row << new QStandardItem(QString::number(p->calcFinalPrice()));
                             row << new QStandardItem(QString::number(p->getQuantity()));
                             row << new QStandardItem(b ? QString::number(b->getVolume()) : "");
                             row << new QStandardItem(f ? f->getExpiryDate() : "");
                             row << new QStandardItem(h ? QString::number(h->getWarrantyMonths()) : "");
                             model->appendRow(row);
                         });
}

void MainWindow::loadProductsFromStoreWithKeyWord(const QString &keyword)
{
    QString kw = keyword.trimmed();
    if (kw.isEmpty())
    {
        loadProductsFromStore(0);
        return;
    }
    model->removeRows(0, model->rowCount());
    store.forEachProductByKey(kw,
                              [&](const QString&, Product* p)
                              {
                                  if (!p) return;
                                  Food* f = dynamic_cast<Food*>(p);
                                  Beverage* b = dynamic_cast<Beverage*>(p);
                                  HouseholdItem* h = dynamic_cast<HouseholdItem*>(p);
                                  QList<QStandardItem*> row;
                                  row << new QStandardItem(p->getId());
                                  row << new QStandardItem(p->getName());
                                  QString typeName;
                                  if (f) typeName = "Đồ ăn";
                                  else if (b) typeName = "Thức uống";
                                  else if (h) typeName = "Đồ gia dụng";
                                  else typeName = "Khác";
                                  row << new QStandardItem(typeName);
                                  row << new QStandardItem(QString::number(p->getQuantity()));
                                  row << new QStandardItem(QString::number(p->calcFinalPrice()));
                                  row << new QStandardItem(b ? QString::number(b->getVolume()) : "");
                                  row << new QStandardItem(f ? f->getExpiryDate() : "");
                                  row << new QStandardItem(h ? QString::number(h->getWarrantyMonths()) : "");
                                  model->appendRow(row);
                              });
}

void MainWindow::on_ToanBo_clicked()
{
    ui->dockMenu->hide();
    loadProductsFromStore(0);
    curTableProduct = 0;
}
void MainWindow::on_DoAn_clicked()
{
    ui->dockMenu->hide();
    loadProductsFromStore(1);
    curTableProduct = 1;
}
void MainWindow::on_ThucUong_clicked()
{
    ui->dockMenu->hide();
    loadProductsFromStore(2);
    curTableProduct = 2;
}
void MainWindow::on_DoGiaDung_clicked()
{
    ui->dockMenu->hide();
    loadProductsFromStore(3);
    curTableProduct = 3;
}
void MainWindow::on_BtnSearch_clicked()
{
    ui->dockMenu->hide();
    loadProductsFromStoreWithKeyWord(ui->SearchText->text());
}
void MainWindow::on_btnMenu_clicked()
{
    bool v = ui->dockMenu->isVisible();
    ui->dockMenu->setVisible(!v);
}
void MainWindow::on_btnOrder_clicked()
{
    ui->dockMenu->hide();
    bool v = ui->dockOrder->isVisible();
    ui->dockOrder->setVisible(!v);
    ui->stackedWidgeOrder->setCurrentIndex(0);
    resetHoaDon();
    loadProductsFromStore(curTableProduct);
    updateHoaDonView();
}

void MainWindow::onSanPhamDoubleClicked(const QModelIndex &index)
{
    bool v1 = ui->dockOrder->isVisible();
    auto v2 = ui->stackedWidgeOrder->currentIndex();
    if(!v1)
        return;
    if(v2 == 1)
        return;
    QString id = model->item(index.row(), 0)->text();
    Product* p = store.findProductById(id);
    if (!p)
        return;
    try
    {
        currentBill->addItem(p, 1);
        loadProductsFromStore(curTableProduct);
    }
    catch (const OutOfStockException& e)
    {
        QMessageBox::warning(this, "Hết hàng", e.what());
        return;
    }
    updateHoaDonView();
}

void MainWindow::onRemoveSanPhamDoubleClicked(const QModelIndex &index)
{
    QString name = modelHoaDon->item(index.row(), 0)->text();
    Product* p = store.findProductByName(name);
    currentBill->removeItem(p, 1);
    loadProductsFromStore(curTableProduct);
    updateHoaDonView();

}

void MainWindow::onTimKhachPressed()
{
    QString key = ui->txtSearchCustomer->text().trimmed();
    if (key.isEmpty())
        return;

    Customer* c = store.findCustomerByName(key);
    if (c == nullptr)
        c = store.findCustomerByPhone(key);

    if (c)
    {
        currentBill->setCustomer(c);
        ui->lblTenKhach->setText(c->getName());
        ui->lblDiemKhach->setText(QString("Điểm Tích Lũy: %1").arg(c->getPoints()));
        if (c->getPoints() >= 100000)
            ui->btnDungDiem->setEnabled(true);
        else
            ui->btnDungDiem->setEnabled(false);
    }
    else
    {
        currentBill->setCustomer(nullptr);
        ui->lblTenKhach->setText("Không tìm thấy!");
        ui->btnDungDiem->setEnabled(false);
    }
}

void MainWindow::onDungDiemClicked()
{
    bool success = currentBill->applyPointsDiscount(100000);
    currentBill->setCheck(true);
    if (success)
    {
        updateHoaDonView();
        ui->btnDungDiem->setEnabled(false);
        QMessageBox::information(this, "Thành công", "Đã áp dụng giảm giá 2% và trừ 100.000 điểm.");
    }
    else
        QMessageBox::warning(this, "Lỗi", "Không đủ điểm hoặc đã áp dụng giảm giá.");
}

void MainWindow::onXuatHoaDonClicked()
{
    if (currentBill->getItems().empty())
    {
        QMessageBox::warning(this, "Hóa đơn trống", "Vui lòng chọn sản phẩm.");
        return;
    }
    ui->stackedWidgeOrder->setCurrentIndex(1);
    setupLastBill();
    updateLastBillView();
}

void MainWindow::onQuayLaiClicked()
{
    ui->stackedWidgeOrder->setCurrentIndex(0);
}


void MainWindow::onThanhToanTienMatClicked()
{
    currentBill->setPayment(new CashPayment());
    QMessageBox::information(this, "Thông Báo: ", "Đã Chọn phương thức thanh toán là tiền mặt");
}

void MainWindow::onThanhToanTheClicked()
{
    currentBill->setPayment(new CardPayment());
    QMessageBox::information(this, "Thông Báo: ", "Đã Chọn phương thức thanh toán là quẹt thẻ");
}

void MainWindow::onThanhToanClicked()
{
    if(currentBill->getPayment() == nullptr)
    {
        QMessageBox::information(this, "Thông Báo: ", "Vui lòng chọn phương thức thanh toán trước khi thanh toán");
        return;
    }
    finalizeThanhToan(currentBill->getPayment()->getMethodName());
    ui->stackedWidgeOrder->setCurrentIndex(0);
    resetHoaDon();
    updateHoaDonView();
}

void MainWindow::finalizeThanhToan(const QString& paymentMethod)
{
    double finalTotal = currentBill->getTotal();
    store.addRevenue(finalTotal);

    Customer* c = currentBill->getCustomer();
    if (c != nullptr)
    {
        int pointsToAdd = static_cast<int>(finalTotal * 0.10);
        c->addPoints(pointsToAdd);
    }
    delete currentBill;
    currentBill = new Bill(nullptr);
    QMessageBox::information(this, "Thành công", QString("Đã thanh toán %1 đồng bằng %2").arg(finalTotal).arg(paymentMethod));
}
