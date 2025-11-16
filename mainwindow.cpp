#include "mainwindow.h"
#include "ui_mainwindow.h"

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

#include "addProductToStore.h"
#include "ThongKe.h"
#include "AddCustomerToStore.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), model(nullptr), curTableProduct(0)
{
    ui->setupUi(this);

    productsLayout = new QGridLayout(ui->scrollProducts->widget());
    productsLayout->setContentsMargins(12, 12, 12, 12);
    productsLayout->setHorizontalSpacing(16);
    productsLayout->setVerticalSpacing(16);

    store.addProduct(new Food("", "Bánh mì", 10000, 50, "banhmi.png", "01/01/2026"));
    store.addProduct(new Food("", "Xúc xích", 12000, 40, "xucxich.png", "15/01/2026"));
    store.addProduct(new Beverage("", "Coca-Cola", 15000, 30, "coca.png", 330));
    store.addProduct(new HouseholdItem("", "Nước rửa chén", 30000, 20, "nuocruachen.png", 12));
    store.addProduct(new Food("", "Kẹo Cao Su", 50000, 10, "keocaosu.png", "15/01/2026"));

    store.addCustomer(new Customer("C1", "Khách Vip", "0905123456", 110000));
    store.addCustomer(new Customer("C2", "Khách Thường", "0905654321", 10000));

    ui->dockMenu->hide();
    ui->dockOrder->hide();
    loadProductsFromStore(0);
    connect(ui->SearchText, &QLineEdit::returnPressed, this, &MainWindow::on_BtnSearch_clicked);


    currentBill = new Bill(nullptr);
    setupHoaDonTable();

    ui->stackedWidgeOrder->setCurrentIndex(curTableProduct);

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
    //qDebug() << "size of items: " << items.size();
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


QString MainWindow::getProductTypeName(Product* p) const
{
    if (dynamic_cast<Food*>(p)) return "Đồ ăn";
    if (dynamic_cast<Beverage*>(p)) return "Thức uống";
    if (dynamic_cast<HouseholdItem*>(p)) return "Đồ gia dụng";
    return "Khác";
}

void MainWindow::clearProductsGrid()
{
    if (!productsLayout) return;

    QLayoutItem* item = nullptr;
    // xóa hết widget (card) cũ khỏi grid
    while ((item = productsLayout->takeAt(0)) != nullptr)
    {
        if (QWidget* w = item->widget())
            w->deleteLater();
        delete item;
    }
}

static QString getProductImagePath(Product* p)
{
    if (!p) return {};

    QString fileName = p->getImageFile();
    qDebug() << "Loading image for" << p->getName() << "-> FileName:" << fileName;

    if (fileName.isEmpty())
        return {};
    return ":/icons/" + fileName;
}
void MainWindow::addProductCard(Product* p)
{
    if (!p || !productsLayout) return;

    const int index = productsLayout->count();
    const int row = index / PRODUCT_COLUMNS;
    const int col = index % PRODUCT_COLUMNS;

    QFrame* card = new QFrame(ui->scrollProducts->widget());
    card->setObjectName("productCard");
    card->setFixedSize(220, 260);
    card->setStyleSheet(
        "QFrame#productCard {"
        "  border-radius: 16px;"
        "  background: #ffffff;"
        "}"
        "QLabel#productName { color: #222222; }"
        "QLabel#productPrice { color: #222222; }"
        "QLabel#productType { color: #888888; font-size: 11px; }"
        "QPushButton#btnAddProduct {"
        "  border-radius: 14px;"
        "  padding: 4px 12px;"
        "  background: #0078ff;"
        "  color: white;"
        "  border: none;"
        "}"
        "QPushButton#btnAddProduct:hover {"
        "  background: #005ccc;"
        "}"
        );

    QVBoxLayout* v = new QVBoxLayout(card);
    v->setContentsMargins(10, 10, 10, 10);
    v->setSpacing(6);

    QLabel* imageLabel = new QLabel(card);
    imageLabel->setMinimumHeight(110);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet(
        "background: #f5f5f5;"
        "border-radius: 12px;"
        "color: #aaaaaa;"
        );

    QString imgPath = getProductImagePath(p);
    if (!imgPath.isEmpty())
    {
        QPixmap pix(imgPath);
        if (!pix.isNull())
        {
            QSize targetSize(200, 110);
            QPixmap scaledPix = pix.scaled(targetSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            imageLabel->setPixmap(scaledPix);
        }
        else
            imageLabel->setText("Ảnh");
    }
    else
        imageLabel->setText("Ảnh");

    QLabel* nameLabel = new QLabel(p->getName(), card);
    nameLabel->setObjectName("productName");
    nameLabel->setWordWrap(true);
    QFont nameFont = nameLabel->font();
    nameFont.setPointSize(nameFont.pointSize() + 1);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);

    QLabel* typeLabel = new QLabel(getProductTypeName(p), card);
    typeLabel->setObjectName("productType");

    QLabel* stockLabel = new QLabel(QString("Còn lại: %1").arg(p->getQuantity()), card);
    stockLabel->setObjectName("productStock");
    stockLabel->setStyleSheet("color: #008800; font-size: 10px; font-weight: bold;");

    QLabel* priceLabel = new QLabel(QString::number(p->calcFinalPrice()) + " đ", card);
    priceLabel->setObjectName("productPrice");
    QFont priceFont = priceLabel->font();
    priceFont.setBold(true);
    priceLabel->setFont(priceFont);

    QHBoxLayout* bottomRow = new QHBoxLayout();
    bottomRow->addWidget(priceLabel);
    bottomRow->addStretch();

    QPushButton* btnAdd = new QPushButton("+ Thêm", card);
    btnAdd->setObjectName("btnAddProduct");
    btnAdd->setCursor(Qt::PointingHandCursor);
    bottomRow->addWidget(btnAdd);

    v->addWidget(imageLabel);
    v->addSpacing(6);
    v->addWidget(nameLabel);
    v->addWidget(typeLabel);
    v->addWidget(stockLabel);
    v->addStretch();
    v->addLayout(bottomRow);

    productsLayout->addWidget(card, row, col);

    connect(btnAdd, &QPushButton::clicked, this, [this, p]() {this->onAddSanPham(p);});
}



void MainWindow::loadProductsFromStore(int typeFilter)
{
    clearProductsGrid();
    store.forEachProduct([&](const QString&, Product* p)
                        {
                            if (!p)
                                return;
                            bool ok = false;
                            Food* f = dynamic_cast<Food*>(p);
                            Beverage* b = dynamic_cast<Beverage*>(p);
                            HouseholdItem* h = dynamic_cast<HouseholdItem*>(p);
                            if (typeFilter == 0) ok = true;
                            else if (typeFilter == 1 && f)
                               ok = true;
                            else if (typeFilter == 2 && b)
                               ok = true;
                            else if (typeFilter == 3 && h)
                               ok = true;
                            if (!ok) return;

                            addProductCard(p);
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
    clearProductsGrid();

    store.forEachProductByName(kw,
                              [&](const QString&, Product* p)
                              {
                                if (!p)
                                   return;
                                addProductCard(p);
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

void MainWindow::onAddSanPham(Product *p)
{
    auto v2 = ui->stackedWidgeOrder->currentIndex();
    if(v2 == 1)
        return;

    int maxStock = p->getQuantity();
    if (maxStock <= 0)
    {
        QMessageBox::warning(this, "Hết hàng", "Sản phẩm này đã hết hàng.");
        return;
    }

    bool ok;
    int quantityToAdd = QInputDialog::getInt(this, "Nhập số lượng", QString("%1:").arg(p->getName()), 1, 1, maxStock, 1, &ok);
    if(ok)
    {
        currentBill->addItem(p, quantityToAdd);
        ui->dockOrder->show();
        ui->stackedWidgeOrder->setCurrentIndex(0);
        loadProductsFromStore(curTableProduct);
        updateHoaDonView();
    }
}

void MainWindow::onRemoveSanPhamDoubleClicked(const QModelIndex &index)
{
    QString name = modelHoaDon->item(index.row(), 0)->text();
    Product* p = store.findProductByName(name);
    currentBill->removeItem(p);
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

void MainWindow::on_ThemHang_clicked()
{
    AddStockDialog dialog(&store, this);

    int result = dialog.exec();

    if (result == QDialog::Accepted)
    {
        QString productId = dialog.getSelectedProductId();
        int quantityToAdd = dialog.getQuantity();

        Product* p = store.findProductById(productId);
        if (p)
        {
            p->setQuantity(p->getQuantity() + quantityToAdd);
            loadProductsFromStore(curTableProduct);
        }
    }
}

void MainWindow::on_ThongKe_clicked()
{
    double totalRevenue = store.getTotalRevenue();
    ThongKe dialog(totalRevenue, this);
    dialog.exec();
}

void MainWindow::on_KhachHang_clicked()
{
    CustomerDialog dialog(&store, this);
    dialog.exec();
}

