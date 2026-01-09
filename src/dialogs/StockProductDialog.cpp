#include "dialogs/StockProductDialog.h"
#include "ui_StockProductDialog.h"
#include "models/Product.h"
#include "models/Food.h"
#include "models/Beverage.h"
#include "models/HouseholdItem.h"
#include <QHeaderView>

StockProductDialog::StockProductDialog(Store* store, QWidget *parent)
    : QDialog(parent), ui(new Ui::StockProductDialog), m_store(store)
{
    ui->setupUi(this);
    setWindowTitle("Chi Tiết Tồn Kho Sản Phẩm");
    setupTables();
    loadProductData();
}

StockProductDialog::~StockProductDialog()
{
    delete ui;
}

void StockProductDialog::setupTables()
{
    m_outOfStockModel = new QStandardItemModel(this);
    m_criticalLowModel = new QStandardItemModel(this);
    m_lowModel = new QStandardItemModel(this);
    m_adequateModel = new QStandardItemModel(this);
    QList<QStandardItemModel*> models = {m_outOfStockModel, m_criticalLowModel, m_lowModel, m_adequateModel};

    for (auto* model : models)
    {
        model->setColumnCount(6);
        model->setHeaderData(0, Qt::Horizontal, "Mã SP");
        model->setHeaderData(1, Qt::Horizontal, "Tên SP");
        model->setHeaderData(2, Qt::Horizontal, "Loại");
        model->setHeaderData(3, Qt::Horizontal, "Số Lượng");
        model->setHeaderData(4, Qt::Horizontal, "Giá Nhập");
        model->setHeaderData(5, Qt::Horizontal, "Giá Bán");
    }
    
    ui->tableOutOfStock->setModel(m_outOfStockModel);
    ui->tableCriticalLow->setModel(m_criticalLowModel);
    ui->tableLow->setModel(m_lowModel);
    ui->tableAdequate->setModel(m_adequateModel);
    
    QTableView* tables[] =
    {
        ui->tableOutOfStock,
        ui->tableCriticalLow,
        ui->tableLow,
        ui->tableAdequate
    };
    
    for (QTableView* table : tables)
    {
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setAlternatingRowColors(true);

        QHeaderView* header = table->horizontalHeader();
        header->setStretchLastSection(true);
        header->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
}

void StockProductDialog::loadProductData()
{
    m_outOfStockModel->removeRows(0, m_outOfStockModel->rowCount());
    m_criticalLowModel->removeRows(0, m_criticalLowModel->rowCount());
    m_lowModel->removeRows(0, m_lowModel->rowCount());
    m_adequateModel->removeRows(0, m_adequateModel->rowCount());
    
    int outOfStockCount = 0;
    int criticalLowCount = 0;
    int lowCount = 0;
    int adequateCount = 0;
    
    m_store->forEachProduct([&](const QString&, Product* p){
        if (!p || !p->getIsActive())
            return;
        
        int qty = p->getQuantity();
        QString type;
        
        if (dynamic_cast<Food*>(p))
            type = "Thực phẩm";
        else if (dynamic_cast<Beverage*>(p))
            type = "Đồ uống";
        else if (dynamic_cast<HouseholdItem*>(p))
            type = "Đồ gia dụng";
        else
            type = "Khác";
        
        QList<QStandardItem*> row;
        row << new QStandardItem(p->getId());
        row << new QStandardItem(p->getName());
        row << new QStandardItem(type);
        
        QStandardItem* qtyItem = new QStandardItem(QString::number(qty));
        qtyItem->setTextAlignment(Qt::AlignCenter);
        row << qtyItem;
        
        row << new QStandardItem(QString::number(p->getImportPrice(), 'f', 0) + " đ");
        row << new QStandardItem(QString::number(p->getBasePrice(), 'f', 0) + " đ");
        
        if (qty == 0)
        {
            for (auto* item : std::as_const(row))
                item->setForeground(QBrush(QColor(0xEF, 0x44, 0x44))); // Đỏ
            m_outOfStockModel->appendRow(row);
            outOfStockCount++;
        }
        else if (qty >= 1 && qty <= 5)
        {
            for (auto* item : std::as_const(row))
                item->setForeground(QBrush(QColor(0xF5, 0x9E, 0x0B))); // Cam
            m_criticalLowModel->appendRow(row);
            criticalLowCount++;
        }
        else if (qty >= 6 && qty <= 10)
        {
            for (auto* item : std::as_const(row))
                item->setForeground(QBrush(QColor(0xEA, 0xB3, 0x08))); // Vàng
            m_lowModel->appendRow(row);
            lowCount++;
        }
        else
        {
            for (auto* item : std::as_const(row))
                item->setForeground(QBrush(QColor(0x10, 0xB9, 0x81))); // Lục
            m_adequateModel->appendRow(row);
            adequateCount++;
        }
    });
    
    ui->tabWidget->setTabText(0, QString("Hết Hàng (%1)").arg(outOfStockCount));
    ui->tabWidget->setTabText(1, QString("Gần Hết (1-5) (%1)").arg(criticalLowCount));
    ui->tabWidget->setTabText(2, QString("Sắp Hết (6-10) (%1)").arg(lowCount));
    ui->tabWidget->setTabText(3, QString("Đủ Hàng (>10) (%1)").arg(adequateCount));
}
