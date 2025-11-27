#ifndef MANAGEINVENTORY_H
#define MANAGEINVENTORY_H

#include <QDialog>
#include <QStandardItemModel>
#include "Store.h"

namespace Ui {
class ManageInventory;
}

class ManageInventory : public QDialog
{
    Q_OBJECT

public:
    explicit ManageInventory(Store* store, QWidget *parent = nullptr);
    ~ManageInventory();

private slots:
    void onProductDoubleClicked(const QModelIndex &index);
    void onDeleteProductClicked();
    void onRefreshClicked();
    void onSearchTextChanged(const QString &text);
    void onFilterChanged(int index);

private:
    Ui::ManageInventory *ui;
    Store* m_store;
    QStandardItemModel* m_model;

    void setupTable();
    void loadProducts();
    void loadProductsFiltered(int filterType, const QString& searchText = "");
    Product* getProductFromRow(int row);
};

#endif
