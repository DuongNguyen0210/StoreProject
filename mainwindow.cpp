#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Manager.h"
#include "Cashier.h"

#include <QHeaderView>
#include <QStandardItem>
#include <QVariant>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QDebug>
#include <QInputDialog>

#include <qstring.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtableview.h>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QSize>

#include "BillItem.h"
#include "Exceptions.h"
#include "Payment.h"
#include "Customer.h"
#include "HashTable.h"

#include "AddProductToStore.h"
#include "ThongKe.h"
#include "AddCustomerToStore.h"

MainWindow::MainWindow(User* user, Store* storePtr, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), modelTable(nullptr),
    curTableProduct(0), store(storePtr), currentUser(user)
{
    ui->setupUi(this);
    setupTable();
    setupHoaDonTable();
    ui->frameMenu->hide();

    connect(ui->ToanBo, &QPushButton::clicked, this, [this]() {
        ui->ToanBo->setChecked(true);
        ui->DoAn->setChecked(false);
        ui->ThucUong->setChecked(false);
        ui->DoGiaDung->setChecked(false);
        on_ToanBo_clicked();
    });

    connect(ui->DoAn, &QPushButton::clicked, this, [this]() {
        ui->ToanBo->setChecked(false);
        ui->DoAn->setChecked(true);
        ui->ThucUong->setChecked(false);
        ui->DoGiaDung->setChecked(false);
        on_DoAn_clicked();
    });

    connect(ui->ThucUong, &QPushButton::clicked, this, [this]() {
        ui->ToanBo->setChecked(false);
        ui->DoAn->setChecked(false);
        ui->ThucUong->setChecked(true);
        ui->DoGiaDung->setChecked(false);
        on_ThucUong_clicked();
    });

    connect(ui->DoGiaDung, &QPushButton::clicked, this, [this]() {
        ui->ToanBo->setChecked(false);
        ui->DoAn->setChecked(false);
        ui->ThucUong->setChecked(false);
        ui->DoGiaDung->setChecked(true);
        on_DoGiaDung_clicked();
    });

    connect(ui->btnToggleMenu, &QPushButton::clicked, this, &MainWindow::onToggleMenuClicked);
    connect(ui->btnCancelOrder, &QPushButton::clicked, this, &MainWindow::onCancelOrderClicked);
    connect(ui->tableViewProduct, &QTableView::doubleClicked, this, &MainWindow::onAddSanPham);
    connect(ui->SearchText, &QLineEdit::returnPressed, this, &MainWindow::on_BtnSearch_clicked);
    connect(ui->tableViewOrder, &QTableView::doubleClicked, this, &MainWindow::onRemoveSanPhamDoubleClicked);
    connect(ui->txtSearchCustomer, &QLineEdit::returnPressed, this, &MainWindow::onTimKhachPressed);
    connect(ui->txtSearchPhoneCustomer, &QLineEdit::returnPressed, this, &MainWindow::onTimKhachPressed);
    connect(ui->btnDungDiem, &QPushButton::clicked, this, &MainWindow::onDungDiemClicked);
    connect(ui->ExportOrder, &QPushButton::clicked, this, &MainWindow::onXuatHoaDonClicked);
    connect(ui->btnBack, &QPushButton::clicked, this, &MainWindow::onQuayLaiClicked);
    connect(ui->btnCash, &QPushButton::clicked, this, &MainWindow::onThanhToanTienMatClicked);
    connect(ui->btnCard, &QPushButton::clicked, this, &MainWindow::onThanhToanTheClicked);
    connect(ui->btnExport, &QPushButton::clicked, this, &MainWindow::onThanhToanClicked);

    loadProductsFromStore(0);

    currentBill = new Bill(nullptr, "", currentUser);

    ui->stackedWidgeOrder->setCurrentIndex(curTableProduct);

    applyPermissions();
    QHeaderView* header = ui->tableViewProduct->horizontalHeader();
    header->setMaximumSectionSize(500);
    header->setMinimumSectionSize(100);
    header->setStretchLastSection(true);
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Interactive);
    header->resizeSection(1, 300);
    for (int i = 2; i <= 6; i++) {
        header->setSectionResizeMode(i, QHeaderView::Stretch);
    }

}

void MainWindow::applyPermissions()
{
    bool isAdmin = (dynamic_cast<Manager*>(currentUser) != nullptr);
    bool isCashier = (dynamic_cast<Cashier*>(currentUser) != nullptr);
    ui->ThemHang->setVisible(isAdmin);
    ui->ThongKe->setVisible(isAdmin || isCashier);
    ui->KhachHang->setVisible(isAdmin || isCashier);

    QString userInfo = QString("%1 (%2)").arg(currentUser->getName()).arg(isAdmin ? "Quản lý" : "Nhân viên");
    ui->lblStoreName->setText(QString("CỬA HÀNG TẠP HÓA\n%1").arg(userInfo));
}


void MainWindow::onToggleMenuClicked()
{
    bool isVisible = ui->frameMenu->isVisible();
    ui->frameMenu->setVisible(!isVisible);
}

void MainWindow::onCancelOrderClicked()
{
    if (currentBill->getCustomer() == nullptr &&  currentBill->getItems().empty())
    {
        ui->txtSearchCustomer->clear();
        ui->txtSearchPhoneCustomer->clear();
        ui->lblTenKhach->setText("Khách Lẻ");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận hủy",
                                  "Bạn có chắc muốn hủy hóa đơn này?\nTất cả sản phẩm sẽ được trả về kho.",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        resetHoaDon();
        QMessageBox::information(this, "Thành công", "Đã hủy hóa đơn và trả hàng về kho.");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete currentBill;
}

void MainWindow::setupTable()
{
    modelTable = new QStandardItemModel(this);
    modelTable->setColumnCount(8);
    modelTable->setHeaderData(0, Qt::Horizontal, "Mã sản phẩm");
    modelTable->setHeaderData(1, Qt::Horizontal, "Tên sản phẩm");
    modelTable->setHeaderData(2, Qt::Horizontal, "Loại sản phẩm");
    modelTable->setHeaderData(3, Qt::Horizontal, "Giá sản phẩm");
    modelTable->setHeaderData(4, Qt::Horizontal, "Số lượng");
    modelTable->setHeaderData(5, Qt::Horizontal, "Thể tích");
    modelTable->setHeaderData(6, Qt::Horizontal, "Hạn sử dụng");
    modelTable->setHeaderData(7, Qt::Horizontal, "Thời hạn bảo hành");
    ui->tableViewProduct->setModel(modelTable);
    ui->tableViewProduct->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewProduct->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewProduct->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
    ui->txtSearchPhoneCustomer->clear();
    ui->btnDungDiem->setEnabled(false);
    ui->stackedWidgeOrder->setCurrentIndex(0);
    ui->lblTenKhach->setText("Khách lẻ");
    ui->lblDiemKhach->setText("");
    ui->TotalBefore->setText("");
    ui->TotalAfter->setText("");

    const std::vector<BillItem>& billitems = currentBill->getItems();
    for(const BillItem& item : billitems)
    {
        Product* p = store->findProductByName(item.getProduct()->getName());
        if(p) {
            p->setQuantity(p->getQuantity() + item.getQuantity());
        }
    }

    Customer* c = currentBill->getCustomer();
    if(c && currentBill->getCheck())
        c->setPoints(c->getPoints() + 100000);

    delete currentBill;
    currentBill = nullptr;

    currentBill = new Bill(nullptr, "", currentUser);

    updateHoaDonView();
}

void MainWindow::loadProductsFromStore(int typeFilter)
{
    modelTable->removeRows(0, modelTable->rowCount());
    store->forEachProduct([&](const QString&, Product* p)
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

                            if(f)
                            row << new QStandardItem(f->getExpiryDate());
                            else if(b)
                            row << new QStandardItem(b->getExpiryDate());
                            else
                            row << new QStandardItem("");
                            row << new QStandardItem(h ? QString::number(h->getWarrantyMonths()) : "");
                            modelTable->appendRow(row);
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
    modelTable->removeRows(0, modelTable->rowCount());
    qDebug() << kw << '\n';
    store->forEachProductByName(kw, [&](const QString&, Product* p)
                                {
                                    if (!p)
                                       return;
                                    Food* f = dynamic_cast<Food*>(p);
                                    Beverage* b = dynamic_cast<Beverage*>(p);
                                    HouseholdItem* h = dynamic_cast<HouseholdItem*>(p);
                                    QList<QStandardItem*> row;
                                    row << new QStandardItem(p->getId());
                                    row << new QStandardItem(p->getName());
                                    QString typeName;
                                    if (f) typeName = "Đồ ăn";
                                    else if(b) typeName = "Thức uống";
                                    else if (h) typeName = "Đồ gia dụng";
                                    row << new QStandardItem(typeName);
                                    row << new QStandardItem(QString::number(p->calcFinalPrice()));
                                    row << new QStandardItem(QString::number(p->getQuantity()));
                                    row << new QStandardItem(b ? QString::number(b->getVolume()) : "");
                                    if(f)
                                       row << new QStandardItem(f->getExpiryDate());
                                    else if(b)
                                       row << new QStandardItem(b->getExpiryDate());
                                    else
                                       row << new QStandardItem("");
                                    row << new QStandardItem(h ? QString::number(h->getWarrantyMonths()) : "");
                                    modelTable->appendRow(row);
                                });
}


void MainWindow::on_ToanBo_clicked()
{
    loadProductsFromStore(0);
    curTableProduct = 0;
}

void MainWindow::on_DoAn_clicked()
{
    loadProductsFromStore(1);
    curTableProduct = 1;
}

void MainWindow::on_ThucUong_clicked()
{
    loadProductsFromStore(2);
    curTableProduct = 2;
}

void MainWindow::on_DoGiaDung_clicked()
{
    loadProductsFromStore(3);
    curTableProduct = 3;
}

void MainWindow::on_BtnSearch_clicked()
{
    loadProductsFromStoreWithKeyWord(ui->SearchText->text());
}

void MainWindow::onAddSanPham(const QModelIndex &index)
{
    auto v2 = ui->stackedWidgeOrder->currentIndex();
    if(v2 == 1)
        return;

    if(!index.isValid()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn sản phẩm hợp lệ.");
        return;
    }

    QString Id = modelTable->item(index.row(), 0)->text();
    Product* p = store->findProductById(Id);

    if(!p) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy sản phẩm.");
        return;
    }

    int maxStock = p->getQuantity();
    if (maxStock <= 0)
    {
        QMessageBox::warning(this, "Hết hàng", "Sản phẩm này đã hết hàng.");
        return;
    }

    bool ok;
    int quantityToAdd = QInputDialog::getInt(
        this,
        "Nhập số lượng",
        QString("Nhập số lượng cho %1:").arg(p->getName()),
        1, 1, maxStock, 1, &ok
        );

    if(ok)
    {
        try {
            currentBill->addItem(p, quantityToAdd);
            ui->stackedWidgeOrder->setCurrentIndex(0);
            loadProductsFromStore(curTableProduct);
            updateHoaDonView();
        } catch(const std::exception& e) {
            QMessageBox::critical(this, "Lỗi", QString("Không thể thêm sản phẩm: %1").arg(e.what()));
        }
    }
}

void MainWindow::onRemoveSanPhamDoubleClicked(const QModelIndex &index)
{
    QString name = modelHoaDon->item(index.row(), 0)->text();
    Product* p = store->findProductByName(name);
    currentBill->removeItem(p);
    loadProductsFromStore(curTableProduct);
    updateHoaDonView();
}

void MainWindow::onTimKhachPressed()
{
    QString phone = ui->txtSearchPhoneCustomer->text().trimmed();
    QString name = ui->txtSearchCustomer->text().trimmed();
    if (phone.isEmpty() || name.isEmpty())
        return;

    Customer* c = store->findCustomerByPhone(phone);
    if (c == nullptr)
    {
        currentBill->setCustomer(nullptr);
        ui->lblTenKhach->setText("Không tìm thấy!");
        ui->lblTenKhach->setStyleSheet("color: red;");
        ui->lblDiemKhach->setText("");
        ui->btnDungDiem->setEnabled(false);
        return;
    }

    if (c->getName().toLower() != name.toLower())
    {
        currentBill->setCustomer(nullptr);
        ui->lblTenKhach->setText("Thông tin không khớp!");
        ui->lblTenKhach->setStyleSheet("color: red;");
        ui->lblDiemKhach->setText("");
        ui->btnDungDiem->setEnabled(false);
        return;
    }

    currentBill->setCustomer(c);
    ui->lblTenKhach->setText(c->getName());
    ui->lblDiemKhach->setText(QString("Điểm Tích Lũy: %1").arg(c->getPoints()));

    if (c->getPoints() >= 100000)
        ui->btnDungDiem->setEnabled(true);
    else
        ui->btnDungDiem->setEnabled(false);
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
    store->addRevenue(finalTotal);

    Customer* c = currentBill->getCustomer();
    if (c != nullptr)
    {
        int pointsToAdd = static_cast<int>(finalTotal * 0.10);
        c->addPoints(pointsToAdd);
    }

    store->addBillToHistory(currentBill);
    currentBill = new Bill(nullptr, "", currentUser);

    QMessageBox::information(this, "Thành công",
                             QString("Đã thanh toán %1 đồng bằng %2").arg(finalTotal).arg(paymentMethod));
}

void MainWindow::on_ThemHang_clicked()
{
    AddProductToStore dialog(this);
    int result = dialog.exec();

    if (result == QDialog::Accepted)
    {
        QString type = dialog.getProductType();
        QString name = dialog.getName();
        double price = dialog.getPrice();
        int quantity = dialog.getQuantity();

        if (type == "Đồ ăn")
        {
            QString expiry = dialog.getExpiryDate();
            store->addProduct(new Food("", name, price, quantity, expiry));
        }
        else if (type == "Thức uống")
        {
            QString expiry = dialog.getExpiryDate();
            double volume = dialog.getVolume();
            store->addProduct(new Beverage("", name, price, quantity, expiry, volume));
        }
        else if (type == "Đồ gia dụng")
        {
            int warranty = dialog.getWarranty();
            store->addProduct(new HouseholdItem("", name, price, quantity, warranty));
        }
        loadProductsFromStore(curTableProduct);
    }
}

void MainWindow::on_ThongKe_clicked()
{
    ThongKe dialog(store, this);
    dialog.exec();
}

void MainWindow::on_KhachHang_clicked()
{
    CustomerDialog dialog(store, this);
    dialog.exec();
}


QDate MainWindow::getProductDate(Product* p)
{
    QString dateStr;
    if (auto f = dynamic_cast<Food*>(p)) dateStr = f->getExpiryDate();
    else if (auto b = dynamic_cast<Beverage*>(p)) dateStr = b->getExpiryDate();

    if (dateStr.isEmpty()) return QDate();
    return QDate::fromString(dateStr, "dd/MM/yyyy");
}

bool MainWindow::comparePriceAsc(Product* a, Product* b)
{
    return a->calcFinalPrice() < b->calcFinalPrice();
}

bool MainWindow::comparePriceDesc(Product* a, Product* b)
{
    return a->calcFinalPrice() > b->calcFinalPrice();
}

bool MainWindow::compareExpiry(Product* a, Product* b)
{
    QDate d1 = getProductDate(a);
    QDate d2 = getProductDate(b);

    if (!d1.isValid() && !d2.isValid()) return compareDefault(a, b);
    if (!d1.isValid()) return false;
    if (!d2.isValid()) return true;

    return d1 < d2;
}

bool MainWindow::compareDefault(Product* a, Product* b)
{
    return QString::compare(a->getId(), b->getId(), Qt::CaseInsensitive) < 0;
}

