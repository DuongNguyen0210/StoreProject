#include "AddCustomerToStore.h"
#include "ui_AddCustomerToStore.h"
#include "Customer.h"
#include "Exceptions.h"
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
    ui->tableCustomers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableCustomers->setSelectionBehavior(QAbstractItemView::SelectRows);
    
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
        
        // ID (column 0)
        row << new QStandardItem(c->getId());
        
        // Name (column 1)
        row << new QStandardItem(c->getName());
        
        // Phone (column 2)
        row << new QStandardItem(c->getPhone());
        
        // Points (column 3)
        row << new QStandardItem(QString::number(c->getPoints()));
        
        // Tier with color
        QString tier = c->getTier();
        QStandardItem* tierItem = new QStandardItem(tier);
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
    if (tier == "Diamond") return "#4A90E2"; // Blue
    if (tier == "Gold") return "#F5A623";    // Gold
    if (tier == "Silver") return "#9B9B9B";  // Silver
    return "#CD7F32";                         // Bronze
}

void CustomerDialog::on_btnAddNewMember_clicked()
{
    QDialog addDialog(this);
    addDialog.setWindowTitle("Thêm Khách Hàng Mới");
    addDialog.setMinimumWidth(400);
    
    QVBoxLayout* layout = new QVBoxLayout(&addDialog);
    QFormLayout* formLayout = new QFormLayout();
    
    QLineEdit* txtName = new QLineEdit();
    QLineEdit* txtPhone = new QLineEdit();
    
    QLabel* lblMessage = new QLabel();
    lblMessage->setStyleSheet("color: red; font-weight: bold;");
    lblMessage->setVisible(false);
    
    formLayout->addRow("Tên khách hàng:", txtName);
    formLayout->addRow("Số điện thoại:", txtPhone);
    
    layout->addLayout(formLayout);
    layout->addWidget(lblMessage);
    
    QPushButton* btnAdd = new QPushButton("Thêm");
    btnAdd->setStyleSheet("background-color: #4A90E2; color: white; font-weight: bold; padding: 8px; border-radius: 5px;");
    layout->addWidget(btnAdd);
    
    connect(btnAdd, &QPushButton::clicked, [&]() {
        QString name = txtName->text().trimmed();
        QString phone = txtPhone->text().trimmed();
        
        lblMessage->setVisible(false);
        
        // Validation
        if (name.isEmpty() || phone.isEmpty()) {
            lblMessage->setText("Vui lòng nhập đầy đủ tên và số điện thoại!");
            lblMessage->setVisible(true);
            return;
        }
        
        if (name.length() < 2) {
            lblMessage->setText("Tên khách hàng phải có ít nhất 2 ký tự!");
            lblMessage->setVisible(true);
            return;
        }
        
        bool hasDigit = false;
        for (QChar c : name) {
            if (c.isDigit()) {
                hasDigit = true;
                break;
            }
        }
        if (hasDigit) {
            lblMessage->setText("Tên khách hàng không được chứa số!");
            lblMessage->setVisible(true);
            return;
        }
        
        bool isValidPhone = true;
        for (QChar c : phone) {
            if (!c.isDigit()) {
                isValidPhone = false;
                break;
            }
        }
        
        if (!isValidPhone) {
            lblMessage->setText("Số điện thoại chỉ được chứa chữ số!");
            lblMessage->setVisible(true);
            return;
        }
        
        if (phone.length() < 10 || phone.length() > 11) {
            lblMessage->setText("Số điện thoại phải có 10-11 chữ số!");
            lblMessage->setVisible(true);
            return;
        }
        
        if (!phone.startsWith('0')) {
            lblMessage->setText("Số điện thoại phải bắt đầu bằng số 0!");
            lblMessage->setVisible(true);
            return;
        }
        
        if (m_store->findCustomerByPhone(phone)) {
            lblMessage->setText("Số điện thoại này đã tồn tại trong hệ thống!");
            lblMessage->setVisible(true);
            return;
        }
        
        try {
            Customer* c = new Customer("", name, phone, "", 0);
            m_store->addCustomer(c);
            
            QMessageBox::information(&addDialog, "Thành công", "Đã thêm khách hàng thành công!");
            addDialog.accept();
            
            loadCustomers();
            updateStatistics();
        }
        catch (const std::exception& e) {
            lblMessage->setText(QString("Lỗi: %1").arg(e.what()));
            lblMessage->setVisible(true);
        }
    });
    
    addDialog.exec();
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
