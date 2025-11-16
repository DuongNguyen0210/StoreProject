#ifndef ADDPRODUCTTOSTORE_H
#define ADDPRODUCTTOSTORE_H

#include <QDialog>
#include "Store.h"

namespace Ui {
class AddStockDialog;
}

class AddStockDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddStockDialog(Store* store, QWidget *parent = nullptr);
    ~AddStockDialog();

    QString getSelectedProductId() const;
    int getQuantity() const;

private:
    Ui::AddStockDialog *ui;
    Store* m_store;
};

#endif
