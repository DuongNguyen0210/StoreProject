#include "dialogs/StockProductDialog.h"
#include "ui_StockProductDialog.h"
#include "models/Product.h"
#include "models/Food.h"
#include "models/Beverage.h"
#include "models/HouseholdItem.h"
#include <QHeaderView>

StockProductDialog::StockProductDialog(Store* store, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StockProductDialog)
    , m_store(store)
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
    // Create models for each tab
    m_outOfStockModel = new QStandardItemModel(this);
    m_criticalLowModel = new QStandardItemModel(this);
    m_lowModel = new QStandardItemModel(this);
    m_adequateModel = new QStandardItemModel(this);
    
    // Set column headers
    QStringList headers;
    headers << "Mã SP" << "Tên SP" << "Loại" << "Số Lượng" << "Giá Nhập" << "Giá Bán";
    
    m_outOfStockModel->setHorizontalHeaderLabels(headers);
    m_criticalLowModel->setHorizontalHeaderLabels(headers);
    m_lowModel->setHorizontalHeaderLabels(headers);
    m_adequateModel->setHorizontalHeaderLabels(headers);
    
    // Set models to table views
    ui->tableOutOfStock->setModel(m_outOfStockModel);
    ui->tableCriticalLow->setModel(m_criticalLowModel);
    ui->tableLow->setModel(m_lowModel);
    ui->tableAdequate->setModel(m_adequateModel);
    
    // Configure table views
    QTableView* tables[] = {
        ui->tableOutOfStock,
        ui->tableCriticalLow,
        ui->tableLow,
        ui->tableAdequate
    };
    
    for (QTableView* table : tables) {
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setAlternatingRowColors(true);
        table->horizontalHeader()->setStretchLastSection(true);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
}

void StockProductDialog::loadProductData()
{
    // Clear existing data
    m_outOfStockModel->removeRows(0, m_outOfStockModel->rowCount());
    m_criticalLowModel->removeRows(0, m_criticalLowModel->rowCount());
    m_lowModel->removeRows(0, m_lowModel->rowCount());
    m_adequateModel->removeRows(0, m_adequateModel->rowCount());
    
    // Counters for tab labels
    int outOfStockCount = 0;
    int criticalLowCount = 0;
    int lowCount = 0;
    int adequateCount = 0;
    
    // Iterate through all products
    m_store->forEachProduct([&](const QString&, Product* p) {
        if (!p || !p->getIsActive()) return;
        
        int qty = p->getQuantity();
        QString type;
        
        // Determine product type
        if (dynamic_cast<Food*>(p)) {
            type = "Thực phẩm";
        } else if (dynamic_cast<Beverage*>(p)) {
            type = "Đồ uống";
        } else if (dynamic_cast<HouseholdItem*>(p)) {
            type = "Đồ gia dụng";
        } else {
            type = "Khác";
        }
        
        // Create row items
        QList<QStandardItem*> row;
        row << new QStandardItem(p->getId());
        row << new QStandardItem(p->getName());
        row << new QStandardItem(type);
        
        QStandardItem* qtyItem = new QStandardItem(QString::number(qty));
        qtyItem->setTextAlignment(Qt::AlignCenter);
        row << qtyItem;
        
        row << new QStandardItem(QString::number(p->getImportPrice(), 'f', 0) + " đ");
        row << new QStandardItem(QString::number(p->getBasePrice(), 'f', 0) + " đ");
        
        // Add to appropriate model based on quantity
        if (qty == 0) {
            // Apply red color for out of stock
            for (auto* item : row) {
                item->setForeground(QBrush(QColor("#EF4444")));
            }
            m_outOfStockModel->appendRow(row);
            outOfStockCount++;
        } else if (qty >= 1 && qty <= 5) {
            // Apply orange color for critical low
            for (auto* item : row) {
                item->setForeground(QBrush(QColor("#F59E0B")));
            }
            m_criticalLowModel->appendRow(row);
            criticalLowCount++;
        } else if (qty >= 6 && qty <= 10) {
            // Apply yellow color for low
            for (auto* item : row) {
                item->setForeground(QBrush(QColor("#EAB308")));
            }
            m_lowModel->appendRow(row);
            lowCount++;
        } else {
            // Apply green color for adequate
            for (auto* item : row) {
                item->setForeground(QBrush(QColor("#10B981")));
            }
            m_adequateModel->appendRow(row);
            adequateCount++;
        }
    });
    
    // Update tab labels with counts
    ui->tabWidget->setTabText(0, QString("Hết Hàng (%1)").arg(outOfStockCount));
    ui->tabWidget->setTabText(1, QString("Gần Hết (1-5) (%1)").arg(criticalLowCount));
    ui->tabWidget->setTabText(2, QString("Sắp Hết (6-10) (%1)").arg(lowCount));
    ui->tabWidget->setTabText(3, QString("Đủ Hàng (>10) (%1)").arg(adequateCount));
}

void StockProductDialog::populateTable(QStandardItemModel* model, int minQty, int maxQty)
{
    // This function is kept for potential future use but not currently needed
    // as loadProductData() handles all population
}
