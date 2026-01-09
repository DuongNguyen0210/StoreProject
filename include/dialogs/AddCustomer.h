#ifndef ADDCUSTOMER_H
#define ADDCUSTOMER_H

#include <QDialog>

namespace Ui {
class AddCustomerDialog;
}

class Store;
class Customer;

class AddCustomerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddCustomerDialog(Store* store, QWidget *parent = nullptr);
    ~AddCustomerDialog();

    Customer* getNewCustomer() const { return m_newCustomer; }

private slots:
    void accept() override;

private:
    Ui::AddCustomerDialog *ui;
    Store* m_store;
    Customer* m_newCustomer;

    bool validateInput();
    void showError(const QString& message);
    void hideError();
};

#endif
