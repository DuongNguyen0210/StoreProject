#include "AddCustomerToStore.h"
#include "ui_AddCustomerToStore.h"
#include "Customer.h"
#include "Exceptions.h"
#include <QMessageBox>
#include <QHeaderView>
//Mai anh kể cho
CustomerDialog::CustomerDialog(Store* store, QWidget *parent)
    :QDialog(parent), ui(new Ui::CustomerDialog), m_store(store)
{
    ui->setupUi(this);
    ui->label_Message->setVisible(false);
    ui->label_Message->setText("");

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

    ui->label_Message->setVisible(false);

    if (name.isEmpty() || phone.isEmpty())
    {
        ui->label_Message->setText("Vui lòng nhập đầy đủ thông tin!");
        ui->label_Message->setVisible(true);
        return;
    }

    if (name.length() < 2)
    {
        ui->label_Message->setText("Tên khách hàng phải có ít nhất 2 ký tự!");
        ui->label_Message->setVisible(true);
        return;
    }

    bool hasDigit = false;
    for (QChar c : std::as_const(name))
    {
        if (c.isDigit())
        {
            hasDigit = true;
            break;
        }
    }
    if (hasDigit)
    {
        ui->label_Message->setText("Tên khách hàng không được chứa số!");
        ui->label_Message->setVisible(true);
        return;
    }

    bool isValidPhone = true;
    for (QChar c : std::as_const(phone))
    {
        if (!c.isDigit())
        {
            isValidPhone = false;
            break;
        }
    }

    if (!isValidPhone)
    {
        ui->label_Message->setText("Số điện thoại chỉ được chứa chữ số!");
        ui->label_Message->setVisible(true);
        return;
    }

    if (phone.length() < 10 || phone.length() > 11)
    {
        ui->label_Message->setText("Số điện thoại phải có 10-11 chữ số!");
        ui->label_Message->setVisible(true);
        return;
    }

    if (!phone.startsWith('0'))
    {
        ui->label_Message->setText("Số điện thoại phải bắt đầu bằng số 0!");
        ui->label_Message->setVisible(true);
        return;
    }

    if (m_store->findCustomerByPhone(phone))
    {
        ui->label_Message->setText("Số điện thoại này đã tồn tại trong hệ thống!");
        ui->label_Message->setVisible(true);
        return;
    }

    try
    {
        Customer* c = new Customer("", name, phone, 0);
        m_store->addCustomer(c);

        ui->label_Message->setText("✓ Đã thêm khách hàng thành công!");
        ui->label_Message->setStyleSheet("color: #28a745; font-weight: bold; background-color: #D4EDDA; border: 1px solid #C3E6CB;");
        ui->label_Message->setVisible(true);

        loadCustomers();

        ui->txtName->clear();
        ui->txtPhone->clear();
        ui->txtName->setFocus();

        QTimer::singleShot(3000, this, [this]() {
            ui->label_Message->setText("");
            ui->label_Message->setStyleSheet("");
            ui->label_Message->setVisible(false);
        });
    }
    catch (const std::exception& e)
    {
        ui->label_Message->setText(QString("Lỗi: %1").arg(e.what()));
        ui->label_Message->setVisible(true);
    }
}
