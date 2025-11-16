#ifndef ADDCUSTOMERTOSTORE_H
#define ADDCUSTOMERTOSTORE_H

#include <QDialog>
#include <QStandardItemModel>
#include "Store.h"

namespace Ui {
class CustomerDialog;
}

class CustomerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomerDialog(Store* store, QWidget *parent = nullptr);
    ~CustomerDialog();

private slots:
    void on_btnAddCustomer_clicked();

private:
    void setupTable();
    void loadCustomers();

    Ui::CustomerDialog *ui;
    Store* m_store;
    QStandardItemModel* m_model;
};

#endif
