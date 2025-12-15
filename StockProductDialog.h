#ifndef STOCKPRODUCTDIALOG_H
#define STOCKPRODUCTDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "Store.h"

namespace Ui {
class StockProductDialog;
}

class StockProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StockProductDialog(Store* store, QWidget *parent = nullptr);
    ~StockProductDialog();

private:
    Ui::StockProductDialog *ui;
    Store* m_store;
    
    // Models for each tab
    QStandardItemModel* m_outOfStockModel;
    QStandardItemModel* m_criticalLowModel;
    QStandardItemModel* m_lowModel;
    QStandardItemModel* m_adequateModel;
    
    // Setup functions
    void setupTables();
    void loadProductData();
    void populateTable(QStandardItemModel* model, int minQty, int maxQty);
};

#endif // STOCKPRODUCTDIALOG_H
