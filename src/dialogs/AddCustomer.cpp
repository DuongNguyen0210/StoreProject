#include "dialogs/AddCustomer.h"
#include "ui_AddCustomer.h"
#include "core/Store.h"
#include "models/Customer.h"

AddCustomerDialog::AddCustomerDialog(Store* store, QWidget *parent)
    : QDialog(parent), ui(new Ui::AddCustomerDialog), m_store(store), m_newCustomer(nullptr)
{
    ui->setupUi(this);
}

AddCustomerDialog::~AddCustomerDialog()
{
    delete ui;
}

void AddCustomerDialog::accept()
{
    if (validateInput())
    {
        QString name = ui->txtName->text().trimmed();
        QString phone = ui->txtPhone->text().trimmed();
        m_newCustomer = new Customer("", name, phone, 0);
        m_store->addCustomer(m_newCustomer);
        
        QDialog::accept();
    }
}

bool AddCustomerDialog::validateInput()
{
    QString name = ui->txtName->text().trimmed();
    QString phone = ui->txtPhone->text().trimmed();
    
    if (name.isEmpty() || phone.isEmpty())
    {
        showError("Vui lòng nhập đầy đủ tên và số điện thoại!");
        return false;
    }
    
    if (name.length() < 2)
    {
        showError("Tên khách hàng phải có ít nhất 2 ký tự!");
        return false;
    }
    
    for (QChar c : std::as_const(name))
    {
        if (c.isDigit())
        {
            showError("Tên khách hàng không được chứa số!");
            return false;
        }
        if (!c.isLetter() && !c.isSpace())
        {
            showError("Tên khách hàng không được chứa ký tự đặc biệt!");
            return false;
        }
    }
    
    for (QChar c : std::as_const(phone))
        if (!c.isDigit())
        {
            showError("Số điện thoại chỉ được chứa chữ số!");
            return false;
        }
    
    if (phone.length() < 10 || phone.length() > 11)
    {
        showError("Số điện thoại phải có 10-11 chữ số!");
        return false;
    }
    
    if (!phone.startsWith('0'))
    {
        showError("Số điện thoại phải bắt đầu bằng số 0!");
        return false;
    }
    
    if (m_store->findCustomerByPhone(phone))
    {
        showError("Số điện thoại này đã tồn tại trong hệ thống!");
        return false;
    }
    
    hideError();
    return true;
}

void AddCustomerDialog::showError(const QString& message)
{
    ui->lblError->setText(message);
    ui->lblError->setVisible(true);
}

void AddCustomerDialog::hideError()
{
    ui->lblError->setVisible(false);
}
