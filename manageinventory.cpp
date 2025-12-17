#include "ManageInventory.h"
#include "ui_ManageInventory.h"
#include "EditProductDialog.h"
#include "Food.h"
#include "Beverage.h"
#include "HouseholdItem.h"
#include <QMessageBox>
#include <QHeaderView>

ManageInventory::ManageInventory(Store* store, QWidget *parent) : QDialog(parent), ui(new Ui::ManageInventory), store(store)
{
    ui->setupUi(this);
    setupTable();
    loadProductsFiltered(0, "");

    connect(ui->tableProducts, &QTableView::doubleClicked, this, &ManageInventory::onProductDoubleClicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &ManageInventory::onDeleteProductClicked);
    connect(ui->txtSearch, &QLineEdit::textChanged, this, &ManageInventory::onSearchTextChanged);
    connect(ui->cmbFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ManageInventory::onFilterChanged);
}

ManageInventory::~ManageInventory()
{
    delete ui;
}

void ManageInventory::setupTable()
{
    model = new QStandardItemModel(this);
    model->setColumnCount(8);
    model->setHeaderData(0, Qt::Horizontal, "Mã SP");
    model->setHeaderData(1, Qt::Horizontal, "Tên Sản Phẩm");
    model->setHeaderData(2, Qt::Horizontal, "Loại");
    model->setHeaderData(3, Qt::Horizontal, "Giá (đ)");
    model->setHeaderData(4, Qt::Horizontal, "Số Lượng");
    model->setHeaderData(5, Qt::Horizontal, "Thể Tích (ml)");
    model->setHeaderData(6, Qt::Horizontal, "Hạn SD");
    model->setHeaderData(7, Qt::Horizontal, "Bảo Hành (tháng)");

    ui->tableProducts->setModel(model);
    ui->tableProducts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableProducts->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableProducts->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableProducts->setAlternatingRowColors(true);
    
    QHeaderView* header = ui->tableProducts->horizontalHeader();
    
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Interactive);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::Stretch);
    header->setSectionResizeMode(4, QHeaderView::Stretch);
    header->setSectionResizeMode(5, QHeaderView::Stretch);
    header->setSectionResizeMode(6, QHeaderView::Stretch);
    header->setSectionResizeMode(7, QHeaderView::Stretch);

    header->setMinimumSectionSize(150);
    header->setMaximumSectionSize(500);
    header->resizeSection(1, 250);
}

void ManageInventory::loadProductsFiltered(int filterType, const QString& searchText)
{
    model->removeRows(0, model->rowCount());

    QString search = searchText.trimmed().toLower();

    store->forEachProduct([&](const QString&, Product* p) {
        if (!p)
            return;
        if (!p->getIsActive())
            return;

        Food* f = dynamic_cast<Food*>(p);
        Beverage* b = dynamic_cast<Beverage*>(p);
        HouseholdItem* h = dynamic_cast<HouseholdItem*>(p);

        bool typeMatch = false;
        if (filterType == 0) typeMatch = true;
        else if (filterType == 1 && f) typeMatch = true;
        else if (filterType == 2 && b) typeMatch = true;
        else if (filterType == 3 && h) typeMatch = true;

        if (!typeMatch)
            return;

        if (!search.isEmpty())
        {
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

        model->appendRow(row);
    });

    ui->lblTotal->setText(QString("Tổng số sản phẩm: %1").arg(model->rowCount()));
}

Product* ManageInventory::getProductFromRow(int row)
{
    QString productId = model->item(row, 0)->text();
    return store->findProductById(productId);
}

void ManageInventory::onProductDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;

    Product* p = getProductFromRow(index.row());
    if (!p) return;

    EditProductDialog dialog(p, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString newName = dialog.getName();
        double newPrice = dialog.getPrice();
        int newQuantity = dialog.getQuantity();

        if (!newName.isEmpty())
            p->setName(newName);

        p->setImportPrice(dialog.getImportPrice());
        p->setProfitMargin(dialog.getProfitMargin());
        p->setBasePrice(newPrice);
        p->setQuantity(newQuantity);

        if (Food* f = dynamic_cast<Food*>(p))
        {
            QString newExpiry = dialog.getExpiryDate();
            f->setExpiryDate(newExpiry);
        }
        else if (Beverage* b = dynamic_cast<Beverage*>(p))
        {
            QString newExpiry = dialog.getExpiryDate();
            b->setExpiryDate(newExpiry);
            b->setVolume(dialog.getVolume());
        }
        else if (HouseholdItem* h = dynamic_cast<HouseholdItem*>(p))
            h->setWarrantyMonths(dialog.getWarranty());

        QMessageBox::information(this, "Thành công", "Đã cập nhật thông tin sản phẩm thành công!");
        loadProductsFiltered(ui->cmbFilter->currentIndex(), ui->txtSearch->text());
    }
}

void ManageInventory::onDeleteProductClicked()
{
    QModelIndex index = ui->tableProducts->currentIndex();
    if (!index.isValid())
    {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn sản phẩm cần xóa!");
        return;
    }

    Product* p = getProductFromRow(index.row());
    if (!p) return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận xóa", QString("Bạn có chắc muốn xóa sản phẩm '%1'?\n\n") .arg(p->getName()), QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        QString productId = p->getId();
        QString productName = p->getName();
        store->softDeleteProduct(productId);
        QMessageBox::information(this, "Thành công", QString("Đã xóa sản phẩm '%1' thành công!\n\n").arg(productName));
        loadProductsFiltered(ui->cmbFilter->currentIndex(), ui->txtSearch->text());
    }
}

void ManageInventory::onSearchTextChanged(const QString &text)
{
    loadProductsFiltered(ui->cmbFilter->currentIndex(), text);
}

void ManageInventory::onFilterChanged(int index)
{
    loadProductsFiltered(index, ui->txtSearch->text());
}
