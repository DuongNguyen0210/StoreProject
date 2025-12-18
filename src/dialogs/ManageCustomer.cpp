#include "dialogs/ManageCustomer.h"
#include "ui_ManageCustomer.h"
#include "dialogs/AddCustomer.h"
#include "models/Customer.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <algorithm>

CustomerDialog::CustomerDialog(Store* store, QWidget *parent)
    :QDialog(parent), ui(new Ui::CustomerDialog), m_store(store),
    m_currentSearchText(""), m_currentTierFilter(""), m_currentSortIndex(0)
{
    ui->setupUi(this);

    setupTable();
    loadCustomers();
    updateStatistics();

    connect(ui->searchBar, &QLineEdit::textChanged, this, &CustomerDialog::on_searchBar_textChanged);
    connect(ui->filterTier, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CustomerDialog::on_filterTier_currentIndexChanged);
    connect(ui->sortBy, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CustomerDialog::on_sortBy_currentIndexChanged);
}

CustomerDialog::~CustomerDialog()
{
    delete ui;
}

void CustomerDialog::setupTable()
{
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(5);
    m_model->setHeaderData(0, Qt::Horizontal, "ID");
    m_model->setHeaderData(1, Qt::Horizontal, "Tên Khách Hàng");
    m_model->setHeaderData(2, Qt::Horizontal, "Số Điện Thoại");
    m_model->setHeaderData(3, Qt::Horizontal, "Điểm");
    m_model->setHeaderData(4, Qt::Horizontal, "Hạng");

    ui->tableCustomers->setModel(m_model);
    ui->tableCustomers->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableCustomers->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // Connect itemChanged signal for inline editing
    connect(m_model, &QStandardItemModel::itemChanged, this, &CustomerDialog::onCustomerItemChanged);
    
    // Setup header resize modes similar to mainwindow.cpp
    QHeaderView* header = ui->tableCustomers->horizontalHeader();
    header->setStretchLastSection(false);
    
    // ID: Fixed width (auto-resize to content)
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    
    // Name: Interactive with min/max limits (150-400px)
    header->setSectionResizeMode(1, QHeaderView::Interactive);
    header->setMinimumSectionSize(150);  // Min width for name
    header->setMaximumSectionSize(400);  // Max width for name
    header->resizeSection(1, 250);       // Initial width: 250px
    
    // Phone, Points, Tier: Stretch to fill remaining space
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::Stretch);
    header->setSectionResizeMode(4, QHeaderView::Stretch);
}

void CustomerDialog::loadCustomers()
{
    m_model->removeRows(0, m_model->rowCount());

    // Get current search text
    QString search = m_currentSearchText.trimmed().toLower();

    // Collect all customers
    std::vector<Customer*> customers;
    m_store->forEachCustomer([&customers](const QString&, Customer* c) {
        if (c) customers.push_back(c);
    });

    // Apply filters
    std::vector<Customer*> filteredCustomers;
    for (Customer* c : customers) {
        // Apply tier filter
        if (!m_currentTierFilter.isEmpty()) {
            if (c->getTier() != m_currentTierFilter) {
                continue;
            }
        }

        // Apply search filter (search in name, ID, and phone)
        if (!search.isEmpty()) {
            QString name = c->getName().toLower();
            QString id = c->getId().toLower();
            QString phone = c->getPhone().toLower();
            if (!name.contains(search) && !id.contains(search) && !phone.contains(search)) {
                continue;
            }
        }

        filteredCustomers.push_back(c);
    }

    // Apply sorting
    switch (m_currentSortIndex) {
        case 0: // Highest Points
            std::sort(filteredCustomers.begin(), filteredCustomers.end(),
                [](Customer* a, Customer* b) { return a->getPoints() > b->getPoints(); });
            break;
        case 1: // Lowest Points
            std::sort(filteredCustomers.begin(), filteredCustomers.end(),
                [](Customer* a, Customer* b) { return a->getPoints() < b->getPoints(); });
            break;
        case 2: // Name A-Z
            std::sort(filteredCustomers.begin(), filteredCustomers.end(),
                [](Customer* a, Customer* b) { return a->getName() < b->getName(); });
            break;
        case 3: // Name Z-A
            std::sort(filteredCustomers.begin(), filteredCustomers.end(),
                [](Customer* a, Customer* b) { return a->getName() > b->getName(); });
            break;
    }

    // Add to table
    for (Customer* c : filteredCustomers) {
        QList<QStandardItem*> row;
        
        // ID (column 0) - Read-only
        QStandardItem* idItem = new QStandardItem(c->getId());
        idItem->setEditable(false);
        row << idItem;
        
        // Name (column 1) - Editable
        QStandardItem* nameItem = new QStandardItem(c->getName());
        nameItem->setEditable(true);
        row << nameItem;
        
        // Phone (column 2) - Editable
        QStandardItem* phoneItem = new QStandardItem(c->getPhone());
        phoneItem->setEditable(true);
        row << phoneItem;
        
        // Points (column 3) - Read-only
        QStandardItem* pointsItem = new QStandardItem(QString::number(c->getPoints()));
        pointsItem->setEditable(false);
        row << pointsItem;
        
        // Tier with color (column 4) - Read-only
        QString tier = c->getTier();
        QStandardItem* tierItem = new QStandardItem(tier);
        tierItem->setEditable(false);
        QString color = getTierColor(tier);
        tierItem->setForeground(QBrush(QColor(color)));
        QFont font = tierItem->font();
        font.setBold(true);
        tierItem->setFont(font);
        row << tierItem;

        m_model->appendRow(row);
    }

    // Update footer
    int total = filteredCustomers.size();
    int showing = qMin(total, 5);
    ui->labelShowing->setText(QString("Hiển thị 1 đến %1 của %2 kết quả").arg(showing).arg(total));
}

void CustomerDialog::updateStatistics()
{
    int totalMembers = 0;
    int diamondCount = 0;
    int goldCount = 0;
    int silverCount = 0;
    int bronzeCount = 0;

    m_store->forEachCustomer([&](const QString&, Customer* c) {
        if (!c) return;
        totalMembers++;
        QString tier = c->getTier();
        if (tier == "Diamond") diamondCount++;
        else if (tier == "Gold") goldCount++;
        else if (tier == "Silver") silverCount++;
        else if (tier == "Bronze") bronzeCount++;
    });

    ui->labelTotalMembers->setText(QString::number(totalMembers));
    ui->labelDiamondCount->setText(QString::number(diamondCount));
    ui->labelGoldCount->setText(QString::number(goldCount));
    ui->labelSilverCount->setText(QString::number(silverCount));
    ui->labelBronzeCount->setText(QString::number(bronzeCount));
}

void CustomerDialog::applyFiltersAndSort()
{
    loadCustomers();
    updateStatistics();
}

int CustomerDialog::getTierCount(const QString& tier)
{
    int count = 0;
    m_store->forEachCustomer([&](const QString&, Customer* c) {
        if (c && c->getTier() == tier) count++;
    });
    return count;
}

QString CustomerDialog::getTierColor(const QString& tier)
{
    if (tier == "Diamond")
        return "#4A90E2";
    if (tier == "Gold")
        return "#F5A623";
    if (tier == "Silver")
        return "#9B9B9B";
    return "#CD7F32";
}

void CustomerDialog::on_btnAddNewMember_clicked()
{
    AddCustomerDialog dialog(m_store, this);
    
    if (dialog.exec() == QDialog::Accepted)
    {
        QMessageBox::information(this, "Thành công", "Đã thêm khách hàng thành công!");
        loadCustomers();
        updateStatistics();
    }
}



void CustomerDialog::on_searchBar_textChanged(const QString& text)
{
    m_currentSearchText = text;
    applyFiltersAndSort();
}

void CustomerDialog::on_filterTier_currentIndexChanged(int index)
{
    switch (index) {
        case 0: m_currentTierFilter = ""; break;
        case 1: m_currentTierFilter = "Diamond"; break;
        case 2: m_currentTierFilter = "Gold"; break;
        case 3: m_currentTierFilter = "Silver"; break;
        case 4: m_currentTierFilter = "Bronze"; break;
    }
    applyFiltersAndSort();
}

void CustomerDialog::on_sortBy_currentIndexChanged(int index)
{
    m_currentSortIndex = index;
    applyFiltersAndSort();
}

void CustomerDialog::onCustomerItemChanged(QStandardItem* item)
{
    if (!item) return;
    
    int row = item->row();
    int column = item->column();
    
    // Only handle Name (column 1) and Phone (column 2) edits
    if (column != 1 && column != 2) return;
    
    // Disconnect to prevent recursive signals
    disconnect(m_model, &QStandardItemModel::itemChanged, this, &CustomerDialog::onCustomerItemChanged);
    
    // Get customer ID from column 0
    QString customerId = m_model->item(row, 0)->text();
    Customer* customer = m_store->findCustomerById(customerId);
    
    if (!customer) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy khách hàng!");
        loadCustomers();
        connect(m_model, &QStandardItemModel::itemChanged, this, &CustomerDialog::onCustomerItemChanged);
        return;
    }
    
    QString newValue = item->text().trimmed();
    
    if (column == 1)
    {
        if (newValue.isEmpty() || newValue.length() < 2)
        {
            QMessageBox::warning(this, "Lỗi", "Tên khách hàng phải có ít nhất 2 ký tự!");
            loadCustomers();
            connect(m_model, &QStandardItemModel::itemChanged, this, &CustomerDialog::onCustomerItemChanged);
            return;
        }
        
        for (QChar c : std::as_const(newValue))
            if (c.isDigit())
            {
                QMessageBox::warning(this, "Lỗi", "Tên khách hàng không được chứa số!");
                loadCustomers();
                connect(m_model, &QStandardItemModel::itemChanged, this, &CustomerDialog::onCustomerItemChanged);
                return;
            }
        
        customer->setName(newValue);
        QMessageBox::information(this, "Thành công", QString("Đã cập nhật tên thành '%1'").arg(newValue));
    }
    else if (column == 2)
    {
        for (QChar c : std::as_const(newValue))
            if (!c.isDigit())
            {
                QMessageBox::warning(this, "Lỗi", "Số điện thoại chỉ được chứa chữ số!");
                loadCustomers();
                connect(m_model, &QStandardItemModel::itemChanged, this, &CustomerDialog::onCustomerItemChanged);
                return;
            }
        
        if (newValue.length() < 10 || newValue.length() > 11)
        {
            QMessageBox::warning(this, "Lỗi", "Số điện thoại phải có 10-11 chữ số!");
            loadCustomers();
            connect(m_model, &QStandardItemModel::itemChanged, this, &CustomerDialog::onCustomerItemChanged);
            return;
        }
        
        if (!newValue.startsWith('0'))
        {
            QMessageBox::warning(this, "Lỗi", "Số điện thoại phải bắt đầu bằng số 0!");
            loadCustomers();
            connect(m_model, &QStandardItemModel::itemChanged, this, &CustomerDialog::onCustomerItemChanged);
            return;
        }
        
        Customer* existingCustomer = m_store->findCustomerByPhone(newValue);
        if (existingCustomer && existingCustomer->getId() != customerId)
        {
            QMessageBox::warning(this, "Lỗi", "Số điện thoại này đã tồn tại!");
            loadCustomers();
            connect(m_model, &QStandardItemModel::itemChanged, this, &CustomerDialog::onCustomerItemChanged);
            return;
        }
        
        customer->setPhone(newValue);
        QMessageBox::information(this, "Thành công", QString("Đã cập nhật SĐT thành '%1'").arg(newValue));
    }
    
    // Reconnect signal
    connect(m_model, &QStandardItemModel::itemChanged, this, &CustomerDialog::onCustomerItemChanged);
}
