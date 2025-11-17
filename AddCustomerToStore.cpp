#include "AddCustomerToStore.h"
#include "ui_AddCustomerToStore.h"
#include "Customer.h"
#include "Exceptions.h"
#include <QMessageBox>
#include <QHeaderView>

CustomerDialog::CustomerDialog(Store* store, QWidget *parent)
    :QDialog(parent), ui(new Ui::CustomerDialog), m_store(store)
{
    ui->setupUi(this);

    setupTable();
    loadCustomers();
}

CustomerDialog::~CustomerDialog()
{
    delete ui;
}

void CustomerDialog::setupTable()
{
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(4);
    m_model->setHeaderData(0, Qt::Horizontal, "ID");
    m_model->setHeaderData(1, Qt::Horizontal, "Tên Khách Hàng");
    m_model->setHeaderData(2, Qt::Horizontal, "Số Điện Thoại");
    m_model->setHeaderData(3, Qt::Horizontal, "Điểm Tích Lũy");

    ui->tableCustomers->setModel(m_model);
    ui->tableCustomers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableCustomers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void CustomerDialog::loadCustomers()
{
    m_model->removeRows(0, m_model->rowCount());

    m_store->forEachCustomer([this](const QString&, Customer* c) {
        if (!c) return;
        QList<QStandardItem*> row;
        row << new QStandardItem(c->getId());
        row << new QStandardItem(c->getName());
        row << new QStandardItem(c->getPhone());
        row << new QStandardItem(QString::number(c->getPoints()));
        m_model->appendRow(row);
    });
}

void CustomerDialog::on_btnAddCustomer_clicked()
{
    QString name = ui->txtName->text().trimmed();
    QString phone = ui->txtPhone->text().trimmed();

    if (name.isEmpty() || phone.isEmpty())
    {
        ui->label_3->setText(QString("Vui lòng nhập đầy đủ thông tin!"));
        return;
    }

    if (m_store->findCustomerByPhone(phone))
    {
        ui->label_3->setText(QString("Số điện thoại này đã tồn tại!"));
        return;
    }

    Customer* c = new Customer("", name, phone, 0);
    m_store->addCustomer(c);
    ui->label_3->setText(QString("Đã thêm khách hàng thành công!"));
    loadCustomers();
    ui->txtName->clear();
    ui->txtPhone->clear();
}
