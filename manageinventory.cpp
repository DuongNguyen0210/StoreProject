#include "ManageInventory.h"
#include "ui_ManageInventory.h"
#include "EditProductDialog.h"
#include "Food.h"
#include "Beverage.h"
#include "HouseholdItem.h"
#include "Exceptions.h"
#include <QMessageBox>
#include <QHeaderView>

ManageInventory::ManageInventory(Store* store, QWidget *parent)
    : QDialog(parent), ui(new Ui::ManageInventory), m_store(store)
{
    ui->setupUi(this);
    setupTable();
    loadProducts();

    connect(ui->tableProducts, &QTableView::doubleClicked, this, &ManageInventory::onProductDoubleClicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &ManageInventory::onDeleteProductClicked);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &ManageInventory::onRefreshClicked);
    connect(ui->txtSearch, &QLineEdit::textChanged, this, &ManageInventory::onSearchTextChanged);
    connect(ui->cmbFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ManageInventory::onFilterChanged);
}

ManageInventory::~ManageInventory()
{
    delete ui;
}

void ManageInventory::setupTable()
{
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(8);
    m_model->setHeaderData(0, Qt::Horizontal, "Mã SP");
    m_model->setHeaderData(1, Qt::Horizontal, "Tên Sản Phẩm");
    m_model->setHeaderData(2, Qt::Horizontal, "Loại");
    m_model->setHeaderData(3, Qt::Horizontal, "Giá (đ)");
    m_model->setHeaderData(4, Qt::Horizontal, "Số Lượng");
    m_model->setHeaderData(5, Qt::Horizontal, "Thể Tích (ml)");
    m_model->setHeaderData(6, Qt::Horizontal, "Hạn SD");
    m_model->setHeaderData(7, Qt::Horizontal, "Bảo Hành (tháng)");

    ui->tableProducts->setModel(m_model);
    ui->tableProducts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableProducts->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableProducts->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableProducts->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableProducts->setAlternatingRowColors(true);
}

void ManageInventory::loadProducts()
{
    loadProductsFiltered(0, "");
}

void ManageInventory::loadProductsFiltered(int filterType, const QString& searchText)
{
    m_model->removeRows(0, m_model->rowCount());

    QString search = searchText.trimmed().toLower();

    m_store->forEachProduct([&](const QString&, Product* p) {
        if (!p) return;

        Food* f = dynamic_cast<Food*>(p);
        Beverage* b = dynamic_cast<Beverage*>(p);
        HouseholdItem* h = dynamic_cast<HouseholdItem*>(p);

        // Lọc theo loại
        bool typeMatch = false;
        if (filterType == 0) typeMatch = true;
        else if (filterType == 1 && f) typeMatch = true;
        else if (filterType == 2 && b) typeMatch = true;
        else if (filterType == 3 && h) typeMatch = true;

        if (!typeMatch) return;

        // Lọc theo tìm kiếm
        if (!search.isEmpty()) {
            QString name = p->getName().toLower();
            QString id = p->getId().toLower();
            if (!name.contains(search) && !id.contains(search))
                return;
        }

        QList<QStandardItem*> row;
        row << new QStandardItem(p->getId());
        row << new QStandardItem(p->getName());

        QString typeName;
        if (f) typeName = "Đồ ăn";
        else if (b) typeName = "Thức uống";
        else if (h) typeName = "Đồ gia dụng";
        row << new QStandardItem(typeName);

        row << new QStandardItem(QString::number(p->getBasePrice(), 'f', 0));
        row << new QStandardItem(QString::number(p->getQuantity()));
        row << new QStandardItem(b ? QString::number(b->getVolume(), 'f', 0) : "");

        QString expiryDate = "";
        if (f) expiryDate = f->getExpiryDate();
        else if (b) expiryDate = b->getExpiryDate();
        row << new QStandardItem(expiryDate);

        row << new QStandardItem(h ? QString::number(h->getWarrantyMonths()) : "");

        m_model->appendRow(row);
    });

    ui->lblTotal->setText(QString("Tổng số sản phẩm: %1").arg(m_model->rowCount()));
}

Product* ManageInventory::getProductFromRow(int row)
{
    if (row < 0 || row >= m_model->rowCount())
        return nullptr;

    QString productId = m_model->item(row, 0)->text();
    return m_store->findProductById(productId);
}

void ManageInventory::onProductDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;

    Product* p = getProductFromRow(index.row());
    if (!p) return;

    EditProductDialog dialog(p, this);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            // Cập nhật thông tin sản phẩm
            QString newName = dialog.getName();
            double newPrice = dialog.getPrice();
            int newQuantity = dialog.getQuantity();

            if (!newName.isEmpty()) {
                p->setName(newName);
            }

            p->setBasePrice(newPrice);
            p->setQuantity(newQuantity);

            // Cập nhật các trường đặc biệt theo loại sản phẩm
            if (Food* f = dynamic_cast<Food*>(p)) {
                QString newExpiry = dialog.getExpiryDate();
                try {
                    Validation::validateDateDDMMYYYY(newExpiry);
                    f->setExpiryDate(newExpiry);
                } catch (const InvalidDateException& e) {
                    QMessageBox::warning(this, "Cảnh báo",
                                         QString("Ngày không hợp lệ: %1").arg(e.what()));
                }
            }
            else if (Beverage* b = dynamic_cast<Beverage*>(p)) {
                QString newExpiry = dialog.getExpiryDate();
                try {
                    Validation::validateDateDDMMYYYY(newExpiry);
                    b->setExpiryDate(newExpiry);
                } catch (const InvalidDateException& e) {
                    QMessageBox::warning(this, "Cảnh báo",
                                         QString("Ngày không hợp lệ: %1").arg(e.what()));
                }
                b->setVolume(dialog.getVolume());
            }
            else if (HouseholdItem* h = dynamic_cast<HouseholdItem*>(p)) {
                h->setWarrantyMonths(dialog.getWarranty());
            }

            QMessageBox::information(this, "Thành công",
                                     "Đã cập nhật thông tin sản phẩm thành công!");

            loadProductsFiltered(ui->cmbFilter->currentIndex(), ui->txtSearch->text());

        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Lỗi",
                                  QString("Không thể cập nhật: %1").arg(e.what()));
        }
    }
}

void ManageInventory::onDeleteProductClicked()
{
    QModelIndex index = ui->tableProducts->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn sản phẩm cần xóa!");
        return;
    }

    Product* p = getProductFromRow(index.row());
    if (!p) return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận xóa",
                                  QString("Bạn có chắc muốn xóa sản phẩm '%1'?\n\n"
                                          "⚠️ Cảnh báo: Hành động này không thể hoàn tác!\n"
                                          "Sản phẩm sẽ bị xóa hoàn toàn khỏi hệ thống.")
                                      .arg(p->getName()),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QString productName = p->getName();

        QMessageBox::information(this, "Thông báo",
                                 QString("Đã xóa sản phẩm '%1', đùa thôi chưa có chức năng xóa <(')\n\n") .arg(productName));

        loadProductsFiltered(ui->cmbFilter->currentIndex(), ui->txtSearch->text());
    }
}

void ManageInventory::onRefreshClicked()
{
    loadProductsFiltered(ui->cmbFilter->currentIndex(), ui->txtSearch->text());
    QMessageBox::information(this, "Thông báo", "Đã làm mới dữ liệu!");
}

void ManageInventory::onSearchTextChanged(const QString &text)
{
    loadProductsFiltered(ui->cmbFilter->currentIndex(), text);
}

void ManageInventory::onFilterChanged(int index)
{
    loadProductsFiltered(index, ui->txtSearch->text());
}
