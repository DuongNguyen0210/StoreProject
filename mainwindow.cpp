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
#include <QLabel>
#include <algorithm>

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
#include "ManageInventory.h"


MainWindow::MainWindow(User* user, Store* storePtr, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), modelTable(nullptr), curTableProduct(0), store(storePtr), currentUser(user), currentSortCriteria(SORT_DEFAULT)
{
    ui->setupUi(this);
    setupTable();
    setupHoaDonTable();
    setupSortComboBox();
    ui->txtSearchCustomer->setVisible(false);
    ui->btnDungDiem->setVisible(false);
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
    connect(ui->tableViewOrder, &QTableView::doubleClicked, this, &MainWindow::onEditSanPhamDoubleClicked);
    connect(ui->txtSearchCustomer, &QLineEdit::returnPressed, this, &MainWindow::onTimKhachPressed);
    connect(ui->txtSearchPhoneCustomer, &QLineEdit::returnPressed, this, &MainWindow::onTimKhachPressed);
    connect(ui->btnDungDiem, &QPushButton::clicked, this, &MainWindow::onDungDiemClicked);
    connect(ui->ExportOrder, &QPushButton::clicked, this, &MainWindow::onXuatHoaDonClicked);
    connect(ui->btnBack, &QPushButton::clicked, this, &MainWindow::onQuayLaiClicked);
    connect(ui->btnCash, &QPushButton::clicked, this, &MainWindow::onThanhToanTienMatClicked);
    connect(ui->btnCard, &QPushButton::clicked, this, &MainWindow::onThanhToanTheClicked);
    connect(ui->btnExport, &QPushButton::clicked, this, &MainWindow::onThanhToanClicked);

    loadAndSortProducts(0);
    currentBill = new Bill(nullptr, "", currentUser);

    ui->stackedWidgeOrder->setCurrentIndex(curTableProduct);

    applyPermissions();
    QHeaderView* header = ui->tableViewProduct->horizontalHeader();
    header->setMaximumSectionSize(500);
    header->setMinimumSectionSize(100);
    header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Interactive);
    header->resizeSection(1, 300);
    for (int i = 2; i <= 6; i++) {
        header->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    header->setSectionResizeMode(7, QHeaderView::ResizeToContents);
}

void MainWindow::setupSortComboBox()
{
    ui->sortComboBox->setMinimumHeight(45);
    ui->sortComboBox->setMinimumWidth(200);

    ui->sortComboBox->addItem("Mặc định", SORT_DEFAULT);
    ui->sortComboBox->addItem("Giá: Tăng dần", SORT_PRICE_ASC);
    ui->sortComboBox->addItem("Giá: Giảm dần", SORT_PRICE_DESC);
    ui->sortComboBox->addItem("Số lượng: Tăng dần", SORT_QUANTITY_ASC);
    ui->sortComboBox->addItem("Số lượng: Giảm dần", SORT_QUANTITY_DESC);
    ui->sortComboBox->addItem("Hạn SD: Gần nhất", SORT_EXPIRY_ASC);
    ui->sortComboBox->addItem("Hạn SD: Xa nhất", SORT_EXPIRY_DESC);
    ui->sortComboBox->addItem("Thể tích: Tăng dần", SORT_VOLUME_ASC);
    ui->sortComboBox->addItem("Thể tích: Giảm dần", SORT_VOLUME_DESC);
    ui->sortComboBox->addItem("Bảo hành: Tăng dần", SORT_WARRANTY_ASC);
    ui->sortComboBox->addItem("Bảo hành: Giảm dần", SORT_WARRANTY_DESC);

    connect(ui->sortComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSortCriteriaChanged);
}

void MainWindow::onSortCriteriaChanged(int index)
{
    currentSortCriteria = static_cast<SortCriteria>(ui->sortComboBox->itemData(index).toInt());
    loadAndSortProducts(curTableProduct);
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
    if(currentBill == nullptr)
        return;

    bool isBillEmpty = currentBill->getItems().empty();
    if (isBillEmpty)
    {
        // Chỉ cần gọi reset giao diện
        // Hàm resetHoaDon() sẽ không trả hàng vì không có item nào trong bill.
        resetHoaDon();
        updateHoaDonView();

        // Cập nhật lại giao diện chung
        ui->txtSearchPhoneCustomer->clear();
        ui->lblTenKhach->setText("Khách Lẻ");

        loadAndSortProducts(curTableProduct);
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận hủy",
                                  "Bạn có chắc muốn hủy hóa đơn này?\nTất cả sản phẩm sẽ được trả về kho.",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        resetHoaDon();
        updateHoaDonView();
        ui->txtSearchPhoneCustomer->clear();
        ui->lblTenKhach->setText("Khách Lẻ");

        loadAndSortProducts(curTableProduct);
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

// Trong file mainwindow.cpp

void MainWindow::updateHoaDonView()
{
    modelHoaDon->removeRows(0, modelHoaDon->rowCount());

    if(currentBill)
    {
        // 1. Cập nhật bảng hàng hóa (Giữ nguyên)
        const auto& items = currentBill->getItems();
        for (const BillItem& item : items)
        {
            QList<QStandardItem*> row;

            // Lấy item Tên Sản Phẩm
            QStandardItem* nameItem = new QStandardItem(item.getProduct()->getName());
            // KHÓA CHỈNH SỬA: Bỏ cờ cho phép chỉnh sửa
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable); // <--- QUAN TRỌNG
            row << nameItem;

            // Lấy item Số Lượng
            QStandardItem* qtyItem = new QStandardItem(QString::number(item.getQuantity()));
            // KHÓA CHỈNH SỬA
            qtyItem->setFlags(qtyItem->flags() & ~Qt::ItemIsEditable); // <--- QUAN TRỌNG
            row << qtyItem;

            // Lấy item Thành Tiền
            QStandardItem* totalItem = new QStandardItem(QString::number(item.getLineTotal(), 'f', 0));
            // KHÓA CHỈNH SỬA
            totalItem->setFlags(totalItem->flags() & ~Qt::ItemIsEditable); // <--- QUAN TRỌNG
            row << totalItem;

            modelHoaDon->appendRow(row);
        }

        double subTotal = currentBill->getSubTotal();
        double finalTotal = currentBill->getTotal();

        // 2. Cập nhật thông tin khách và Nút dùng điểm
        Customer* c = currentBill->getCustomer();
        if(c)
        {
            ui->lblTenKhach->setText(c->getName());
            ui->lblDiemKhach->setText(QString("Điểm Tích Lũy: %1").arg(c->getPoints()));

            ui->btnDungDiem->setVisible(true); // Hiện nút

            // === LOGIC MỚI CHO NÚT DÙNG ĐIỂM ===
            if (currentBill->getCheck())
            {
                // TRƯỜNG HỢP 1: ĐÃ ÁP DỤNG GIẢM GIÁ -> HIỆN NÚT HỦY
                ui->btnDungDiem->setEnabled(true); // Vẫn cho bấm để hủy
                ui->btnDungDiem->setText(QString("Hủy giảm giá (Trả lại %1 điểm)")
                                             .arg(currentBill->getPointsUsed()));
                // Có thể set style đỏ để cảnh báo đây là nút hủy
                ui->btnDungDiem->setStyleSheet("QPushButton { color: red; border-color: red; }");
            }
            else
            {
                // TRƯỜNG HỢP 2: CHƯA ÁP DỤNG -> TÍNH TOÁN XEM CÓ ĐƯỢC DÙNG KHÔNG
                // Reset style về mặc định (Xanh)
                ui->btnDungDiem->setStyleSheet("");

                int currentPoints = c->getPoints();
                double maxAllowedDiscount = subTotal - 1000.0;

                if (currentPoints >= 10 && maxAllowedDiscount > 0)
                {
                    double pointsValue = currentPoints * 100.0;
                    double actualDiscount = (pointsValue > maxAllowedDiscount) ? maxAllowedDiscount : pointsValue;
                    int ptsToUse = qRound(actualDiscount / 100.0);
                    double priceAfterDiscount = subTotal - (ptsToUse * 100.0);

                    ui->btnDungDiem->setEnabled(true);
                    ui->btnDungDiem->setText(QString("Dùng %1 điểm (Còn: %2 đ)")
                                                 .arg(ptsToUse)
                                                 .arg(QString::number(priceAfterDiscount, 'f', 0)));
                }
                else
                {
                    ui->btnDungDiem->setEnabled(false);
                    if (subTotal <= 1000)
                        ui->btnDungDiem->setText("Hóa đơn quá thấp");
                    else
                        ui->btnDungDiem->setText("Không đủ điểm (Min 10)");
                }
            }
        }
        else
        {
            ui->btnDungDiem->setVisible(false); // Không khách -> Ẩn
        }

        // 3. Cập nhật tổng tiền
        ui->TotalBefore->setText(QString("Tổng tiền ban đầu: %1 đ")
                                     .arg(QString::number(subTotal, 'f', 0)));

        ui->TotalAfter->setText(QString("Tổng tiền thanh toán: %1 đ")
                                    .arg(QString::number(finalTotal, 'f', 0)));
    }
}

void MainWindow::updateLastBillView()
{
    modelLastBill->removeRows(0, modelLastBill->rowCount());
    const auto& items = currentBill->getItems();
    for (const BillItem& item : items)
    {
        QList<QStandardItem*> row;
        // Cột 1: Tên SP
        QStandardItem* nameItem = new QStandardItem(item.getProduct()->getName());
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable); // KHÓA CHỈNH SỬA
        row << nameItem;

        // Cột 2: Số lượng
        QStandardItem* qtyItem = new QStandardItem(QString::number(item.getQuantity()));
        qtyItem->setFlags(qtyItem->flags() & ~Qt::ItemIsEditable); // KHÓA CHỈNH SỬA
        row << qtyItem;

        // Cột 3: Thành tiền
        QStandardItem* totalItem = new QStandardItem(QString::number(item.getLineTotal(), 'f', 0));
        totalItem->setFlags(totalItem->flags() & ~Qt::ItemIsEditable); // KHÓA CHỈNH SỬA
        row << totalItem;

        modelLastBill->appendRow(row);
    }

    double subTotal = currentBill->getSubTotal();
    double finalTotal = currentBill->getTotal();
    ui->TotalBefore_2->setText(QString("Tổng tiền ban đầu: %1").arg(QString::number(subTotal, 'f', 0)));
    ui->TotalAfter_2->setText(QString("Tổng tiền thanh toán: %1").arg(QString::number(finalTotal, 'f', 0)));
}

void MainWindow::resetHoaDon()
{
    ui->txtSearchCustomer->clear();
    ui->txtSearchPhoneCustomer->clear();
    ui->btnDungDiem->setEnabled(false);
    ui->stackedWidgeOrder->setCurrentIndex(0);
    ui->lblTenKhach->setText("Khách Lẻ");
    ui->lblDiemKhach->setText("");
    ui->TotalBefore->setText("");
    ui->TotalAfter->setText("");

    // ✅ TRẢ HÀNG VỀ KHO khi hủy đơn
    const std::vector<BillItem>& billitems = currentBill->getItems();
    for(const BillItem& item : billitems)
    {
        // 🛡️ FIX: Dùng ID thay vì Name (tránh trả nhầm sản phẩm cùng tên)
        Product* p = store->findProductById(item.getProduct()->getId());
        if(p) {
            p->setQuantity(p->getQuantity() + item.getQuantity());
        }
    }

    Customer* c = currentBill->getCustomer();

    // Lấy đúng số điểm đã trừ lúc nãy từ Bill::getPointsUsed()
    int used = currentBill->getPointsUsed();
    if (c && currentBill->getCheck() && used > 0) {
        c->setPoints(c->getPoints() + used);
    }

    delete currentBill;
    currentBill = new Bill(nullptr, "", currentUser);

    ui->btnDungDiem->setVisible(false);
    ui->btnDungDiem->setEnabled(false);
    ui->btnDungDiem->setText("Dùng điểm tích lũy");

    ui->lblTenKhach->setStyleSheet("");
    ui->lblDiemKhach->setStyleSheet("");
}

void MainWindow::loadAndSortProducts(int typeFilter)
{
    std::vector<Product*> products;

    store->forEachProduct([&](const QString&, Product* p) {
        if (!p) return;
        
        // ✅ FILTER: Only show active products with stock > 0
        if (!p->getIsActive()) return;
        if (p->getQuantity() <= 0) return;

        bool shouldInclude = false;
        Food* f = dynamic_cast<Food*>(p);
        Beverage* b = dynamic_cast<Beverage*>(p);
        HouseholdItem* h = dynamic_cast<HouseholdItem*>(p);

        if (typeFilter == 0) {
            shouldInclude = true;
        } else if (typeFilter == 1 && f) {
            shouldInclude = true;
        } else if (typeFilter == 2 && b) {
            shouldInclude = true;
        } else if (typeFilter == 3 && h) {
            shouldInclude = true;
        }

        if (shouldInclude) {
            products.push_back(p);
        }
    });

    applySortingAndFiltering(products);

    modelTable->removeRows(0, modelTable->rowCount());

    for (Product* p : products) {
        if (!p) continue;

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
    }
}

void MainWindow::applySortingAndFiltering(std::vector<Product*>& products)
{
    switch (currentSortCriteria) {
    case SORT_DEFAULT:
        std::sort(products.begin(), products.end(), compareNameAsc);
        break;

    case SORT_PRICE_ASC:
        std::sort(products.begin(), products.end(), comparePriceAsc);
        break;

    case SORT_PRICE_DESC:
        std::sort(products.begin(), products.end(), comparePriceDesc);
        break;

    case SORT_QUANTITY_ASC:
        std::sort(products.begin(), products.end(), compareQuantityAsc);
        break;

    case SORT_QUANTITY_DESC:
        std::sort(products.begin(), products.end(), compareQuantityDesc);
        break;

    case SORT_EXPIRY_ASC:
    case SORT_EXPIRY_DESC:
        products.erase(
            std::remove_if(products.begin(), products.end(), [](Product* p) {
                return dynamic_cast<HouseholdItem*>(p);
            }),
            products.end()
            );
        if(currentSortCriteria == SORT_EXPIRY_ASC)
            std::sort(products.begin(), products.end(), compareExpiryAsc);
        else
            std::sort(products.begin(), products.end(), compareExpiryDesc);
        break;
    case SORT_VOLUME_ASC:
    case SORT_VOLUME_DESC:
        products.erase(
            std::remove_if(products.begin(), products.end(), [](Product* p) {
                return dynamic_cast<Beverage*>(p) == nullptr;
            }),
            products.end()
            );
        if (currentSortCriteria == SORT_VOLUME_ASC)
            std::sort(products.begin(), products.end(), compareVolumeAsc);
        else
            std::sort(products.begin(), products.end(), compareVolumeDesc);
        break;

    case SORT_WARRANTY_ASC:
    case SORT_WARRANTY_DESC:
        products.erase(
            std::remove_if(products.begin(), products.end(), [](Product* p) {
                return dynamic_cast<HouseholdItem*>(p) == nullptr;
            }),
            products.end()
            );
        if (currentSortCriteria == SORT_WARRANTY_ASC)
            std::sort(products.begin(), products.end(), compareWarrantyAsc);
        else
            std::sort(products.begin(), products.end(), compareWarrantyDesc);
        break;
    }
}
bool MainWindow::compareNameAsc(Product* a, Product* b)
{
    if (!a || !b) return false;
    // So sánh tên không phân biệt hoa thường (localeAwareCompare hoặc compare)
    return QString::localeAwareCompare(a->getName(), b->getName()) < 0;
}

bool MainWindow::comparePriceAsc(Product* a, Product* b)
{
    if (!a || !b) return false;
    return a->calcFinalPrice() < b->calcFinalPrice();
}

bool MainWindow::comparePriceDesc(Product* a, Product* b)
{
    if (!a || !b) return false;
    return a->calcFinalPrice() > b->calcFinalPrice();
}

bool MainWindow::compareQuantityAsc(Product* a, Product* b)
{
    if (!a || !b) return false;
    return a->getQuantity() < b->getQuantity();
}

bool MainWindow::compareQuantityDesc(Product* a, Product* b)
{
    if (!a || !b) return false;
    return a->getQuantity() > b->getQuantity();
}

bool MainWindow::compareExpiryAsc(Product* a, Product* b)
{
    if (!a || !b) return false;

    QDate d1 = getProductDate(a);
    QDate d2 = getProductDate(b);

    if (!d1.isValid() && !d2.isValid()) return false;
    if (!d1.isValid()) return false;
    if (!d2.isValid()) return false;

    return d1 < d2;
}

bool MainWindow::compareExpiryDesc(Product* a, Product* b)
{
    if (!a || !b) return false;

    QDate d1 = getProductDate(a);
    QDate d2 = getProductDate(b);

    if (!d1.isValid() && !d2.isValid()) return false;
    if (!d1.isValid()) return false;
    if (!d2.isValid()) return true;

    return d1 > d2;
}

bool MainWindow::compareVolumeAsc(Product* a, Product* b)
{
    if (!a || !b) return false;

    Beverage* b1 = dynamic_cast<Beverage*>(a);
    Beverage* b2 = dynamic_cast<Beverage*>(b);

    if (!b1 || !b2) return false;

    return b1->getVolume() < b2->getVolume();
}

bool MainWindow::compareVolumeDesc(Product* a, Product* b)
{
    if (!a || !b) return false;

    Beverage* b1 = dynamic_cast<Beverage*>(a);
    Beverage* b2 = dynamic_cast<Beverage*>(b);

    if (!b1 || !b2) return false;

    return b1->getVolume() > b2->getVolume();
}

bool MainWindow::compareWarrantyAsc(Product* a, Product* b)
{
    if (!a || !b) return false;

    HouseholdItem* h1 = dynamic_cast<HouseholdItem*>(a);
    HouseholdItem* h2 = dynamic_cast<HouseholdItem*>(b);

    if (!h1 || !h2) return false;

    return h1->getWarrantyMonths() < h2->getWarrantyMonths();
}

bool MainWindow::compareWarrantyDesc(Product* a, Product* b)
{
    if (!a || !b) return false;

    HouseholdItem* h1 = dynamic_cast<HouseholdItem*>(a);
    HouseholdItem* h2 = dynamic_cast<HouseholdItem*>(b);

    if (!h1 || !h2) return false;

    return h1->getWarrantyMonths() > h2->getWarrantyMonths();
}

QDate MainWindow::getProductDate(Product* p)
{
    if (!p) return QDate();

    QString dateStr;
    if (auto f = dynamic_cast<Food*>(p))
        dateStr = f->getExpiryDate();
    else if (auto b = dynamic_cast<Beverage*>(p))
        dateStr = b->getExpiryDate();

    if (dateStr.isEmpty()) return QDate();
    return QDate::fromString(dateStr, "dd/MM/yyyy");
}

void MainWindow::loadProductsFromStore(int typeFilter)
{
    loadAndSortProducts(typeFilter);
}

void MainWindow::loadProductsFromStoreWithKeyWord(const QString &keyword)
{
    QString kw = keyword.trimmed();
    if (kw.isEmpty())
    {
        loadAndSortProducts(0);
        return;
    }

    modelTable->removeRows(0, modelTable->rowCount());

    store->forEachProductByName(kw, [&](const QString&, Product* p) {
        if (!p) return;

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
    loadAndSortProducts(0);
    curTableProduct = 0;
}

void MainWindow::on_DoAn_clicked()
{
    loadAndSortProducts(1);
    curTableProduct = 1;
}

void MainWindow::on_ThucUong_clicked()
{
    loadAndSortProducts(2);
    curTableProduct = 2;
}

void MainWindow::on_DoGiaDung_clicked()
{
    loadAndSortProducts(3);
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
        QString("Nhập số lượng cho %1:").arg(p->getName()), 1, 1, maxStock, 1, &ok);

    if(ok)
    {
        if(currentBill == nullptr)
            currentBill = new Bill(nullptr, "", currentUser);
        currentBill->addItem(p, quantityToAdd);
        ui->stackedWidgeOrder->setCurrentIndex(0);
        loadAndSortProducts(curTableProduct);
        updateHoaDonView();
    }
}

void MainWindow::onEditSanPhamDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;

    // 1. Xác định sản phẩm đang chọn
    QString name = modelHoaDon->item(index.row(), 0)->text();
    Product* p = store->findProductByName(name);

    if (!p) return;

    // 2. Tìm số lượng hiện tại đang có trong hóa đơn
    int currentBillQty = 0;
    const auto& items = currentBill->getItems();
    for(const auto& item : items) {
        if(item.getProduct()->getId() == p->getId()) {
            currentBillQty = item.getQuantity();
            break;
        }
    }

    // 3. Tính toán số lượng tối đa có thể nhập
    // Max = Số lượng đang giữ trong hóa đơn + Số lượng còn lại trong kho
    int maxAllowed = currentBillQty + p->getQuantity();

    // 4. Hiển thị hộp thoại nhập số lượng
    bool ok;
    int newQty = QInputDialog::getInt(
        this,
        "Cập nhật số lượng",
        QString("Nhập số lượng mới cho %1:\n(Nhập 0 để trả hàng)").arg(p->getName()),
        currentBillQty, // Giá trị mặc định là số lượng hiện tại
        0,              // Min = 0 (để cho phép trả hàng)
        maxAllowed,     // Max = Tổng kho có thể đáp ứng
        1,
        &ok
        );

    if (ok)
    {
        if (newQty == 0)
        {
            // === TRƯỜNG HỢP 1: Nhập 0 -> Xóa sản phẩm (Trả hàng) ===
            currentBill->removeItem(p);
        }
        else if (newQty != currentBillQty)
        {
            // === TRƯỜNG HỢP 2: Cập nhật số lượng ===

            // Cách xử lý an toàn nhất với cấu trúc class hiện tại:
            // Bước 1: Xóa sản phẩm khỏi hóa đơn (để hoàn trả toàn bộ số lượng về kho)
            currentBill->removeItem(p);

            // Bước 2: Thêm lại sản phẩm với số lượng mới (trừ kho theo số lượng mới)
            currentBill->addItem(p, newQty);
        }

        // 5. Cập nhật lại giao diện
        updateHoaDonView();              // Cập nhật bảng hóa đơn
        loadAndSortProducts(curTableProduct); // Cập nhật số lượng tồn kho bên danh sách sản phẩm
    }

}

void MainWindow::onTimKhachPressed()
{
    QString phone = ui->txtSearchPhoneCustomer->text().trimmed();
    auto resetErrorDisplay = [this]() {
        // Chỉ reset nếu nhãn đang hiển thị lỗi (kiểm tra bằng màu đỏ)
        if (ui->lblTenKhach->styleSheet().contains("red")) {
            ui->lblTenKhach->setText("Khách Lẻ");
            ui->lblTenKhach->setStyleSheet(""); // Xóa style sheet
            ui->lblDiemKhach->setText("");
        }
    };
    if (phone.isEmpty())
    {
        return;
    }

    bool errorOccurred = false;

    // Kiểm tra ký tự số
    for (QChar c : std::as_const(phone))
    {
        if (!c.isDigit())
        {
            ui->lblTenKhach->setText("Số điện thoại chỉ được chứa chữ số!");
            errorOccurred = true;
            break;
        }
    }

    if (!errorOccurred && (phone.length() < 10 || phone.length() > 11))
    {
        ui->lblTenKhach->setText("Số điện thoại phải có 10-11 chữ số!");
        errorOccurred = true;
    }

    if (!errorOccurred && !phone.startsWith('0'))
    {
        ui->lblTenKhach->setText("Số điện thoại phải bắt đầu bằng số 0!");
        errorOccurred = true;
    }

    if (errorOccurred)
    {
        // Áp dụng style lỗi cho nhãn hiển thị
        ui->lblTenKhach->setStyleSheet("color: red; font-weight: bold;");
        ui->lblDiemKhach->setText("");

        QTimer::singleShot(3500, this, resetErrorDisplay);
        return;
    }

    Customer* c = store->findCustomerByPhone(phone);

    if (c != nullptr)
    {
        if (currentBill == nullptr) currentBill = new Bill(nullptr, "", currentUser);
        currentBill->setCustomer(c);
        ui->lblTenKhach->setStyleSheet("");
        ui->lblTenKhach->setText(c->getName());
        ui->lblTenKhach->setStyleSheet("color: #0284C7; font-weight: 600;");
        ui->lblDiemKhach->setText(QString("Điểm Tích Lũy: %1").arg(c->getPoints()));
        updateHoaDonView();
    }
    else
    {
        // === KHÁCH MỚI HOẶC KHÔNG TÌM THẤY ===
        if (currentBill) currentBill->setCustomer(nullptr);

        // Xóa thông tin trên giao diện
        ui->lblTenKhach->setText("Chưa có dữ liệu! Vui lòng thêm ở mục 'Khách Hàng'.");
        ui->lblTenKhach->setStyleSheet("color: #DC2626; font-weight: bold;");
        ui->lblDiemKhach->setText("");

        // Ẩn nút dùng điểm ngay lập tức
        ui->btnDungDiem->setVisible(false);
        QTimer::singleShot(3500, this, resetErrorDisplay);
    }
}


void MainWindow::onDungDiemClicked()
{
    if (currentBill->getCheck())
    {
        currentBill->removePointsDiscount();
        updateHoaDonView();

        QMessageBox::information(this, "Đã hủy", "Đã hủy áp dụng giảm giá và hoàn trả điểm cho khách.");
    }
    else
    {
        bool success = currentBill->applyPointsDiscount(10);
        if (success)
        {
            updateHoaDonView();
            QMessageBox::information(this, "Thành công", QString("Đã dùng %1 điểm để giảm giá.").arg(currentBill->getPointsUsed()));
        }
        else
        {
            QMessageBox::warning(this, "Lỗi", "Không thể áp dụng điểm (Số dư không đủ hoặc hóa đơn quá thấp).");
        }
    }
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
    // REMOVED: currentBill = new Bill(nullptr, "", currentUser);
    // Reason: finalizeThanhToan already creates a new Bill, this was causing duplicate ID generation
    updateHoaDonView();
    loadAndSortProducts(curTableProduct);
}

void MainWindow::finalizeThanhToan(const QString& paymentMethod)
{
    // Mục đích: Đảm bảo an toàn dữ liệu, đề phòng kho bị âm trước khi chốt đơn
    const std::vector<BillItem>& billItems = currentBill->getItems();
    for (const BillItem& item : billItems)
    {
        Product* p = item.getProduct();
        int available = p->getQuantity();

        // Nếu kho bị âm (nghĩa là đã trừ quá tay ở bước trước đó)
        if (available < 0)
        {
            QMessageBox::critical(this, "Lỗi Nghiêm Trọng",
                                  QString("Sản phẩm '%1' có tồn kho bất thường (%2). Hủy giao dịch để bảo toàn dữ liệu!")
                                      .arg(p->getName()).arg(available));

            // Rollback: Trả lại số lượng hàng đã trừ vào kho
            for (const BillItem& rollbackItem : billItems)
            {
                Product* rp = rollbackItem.getProduct();
                if(rp) {
                    rp->setQuantity(rp->getQuantity() + rollbackItem.getQuantity());
                }
            }

            // Cập nhật lại giao diện kho hàng để người dùng thấy số lượng đúng
            loadAndSortProducts(curTableProduct);

            return; // DỪNG GIAO DỊCH NGAY LẬP TỨC
        }
    }

    double finalTotal = currentBill->getTotal();

    store->addRevenue(finalTotal);

    // Xử lý cộng điểm tích lũy
    Customer* c = currentBill->getCustomer();
    int pointsAdded = 0;
    if (c != nullptr)
    {
        // LOGIC MỚI: 1.000đ chi tiêu = 1 điểm
        double finalTotal = currentBill->getTotal();
        pointsAdded = qRound(finalTotal / 1000.0);

        if (pointsAdded> 0)
            c->addPoints(pointsAdded);

    }

    store->addBillToHistory(currentBill);

    // Tạo thông báo chi tiết
    QString msg = QString("Thanh toán thành công!\n\n" "Tổng tiền: %1 đ\n" "Hình thức: %2").arg(QString::number(finalTotal, 'f', 0)).arg(paymentMethod);


    // Nếu có khách hàng thì báo thêm về điểm
    if (c != nullptr && pointsAdded > 0)
    {
        msg += QString("\n--------------------\n" "Đã cộng: +%1 điểm\n" "Tổng điểm hiện tại: %2 điểm").arg(pointsAdded).arg(c->getPoints());
    }

    QMessageBox::information(this, "Hoàn tất giao dịch", msg);

    // --- Reset giao diện để sẵn sàng cho đơn mới ---
    ui->stackedWidgeOrder->setCurrentIndex(0); // Quay về màn hình bán hàng

    // Tạo hóa đơn mới (rỗng)
    currentBill = new Bill(nullptr, "", currentUser);

    // Cập nhật lại view hóa đơn (trống)
    updateHoaDonView();

    // QUAN TRỌNG: Tải lại danh sách sản phẩm để cập nhật số lượng tồn kho mới lên bảng
    loadAndSortProducts(curTableProduct);

    // Xóa trắng thông tin khách hàng trên giao diện
    ui->txtSearchCustomer->clear();
    ui->txtSearchCustomer->setVisible(false); // Ẩn ô nhập tên đi cho gọn
    ui->txtSearchPhoneCustomer->clear();
    ui->lblTenKhach->setText("Khách Lẻ");
    ui->lblDiemKhach->setText("");
    ui->btnDungDiem->setEnabled(false);
}

void MainWindow::on_ThemHang_clicked()
{
    AddProductToStore dialog(this);
    int result = dialog.exec();

    if (result == QDialog::Accepted)
    {
        QString type = dialog.getProductType();
        QString name = dialog.getName();
        double price = dialog.getPrice();              // Giá bán (tự động tính)
        double importPrice = dialog.getImportPrice();  // Giá gốc
        double profitMargin = dialog.getProfitMargin();// % Lợi nhuận
        int quantity = dialog.getQuantity();

        if (type == "Đồ ăn")
        {
            QString expiry = dialog.getExpiryDate();
            QString id = store->generateFoodId();
            Product* p = new Food(id, name, price, quantity, expiry);
            p->setImportPrice(importPrice);
            p->setProfitMargin(profitMargin);
            store->addProduct(p);
        }
        else if (type == "Thức uống")
        {
            QString expiry = dialog.getExpiryDate();
            double volume = dialog.getVolume();
            QString id = store->generateBeverageId();
            Product* p = new Beverage(id, name, price, quantity, expiry, volume);
            p->setImportPrice(importPrice);
            p->setProfitMargin(profitMargin);
            store->addProduct(p);
        }
        else if (type == "Đồ gia dụng")
        {
            int warranty = dialog.getWarranty();
            QString id = store->generateHouseholdId();
            Product* p = new HouseholdItem(id, name, price, quantity, warranty);
            p->setImportPrice(importPrice);
            p->setProfitMargin(profitMargin);
            store->addProduct(p);
        }
        loadAndSortProducts(curTableProduct);
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


void MainWindow::on_QuanLyKho_clicked()
{
    ManageInventory dialog(store, this);
    dialog.exec();
    loadAndSortProducts(curTableProduct);
}

