#ifndef STOCKPRODUCTDIALOG_H
#define STOCKPRODUCTDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "core/Store.h"

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
    
    QStandardItemModel* m_outOfStockModel;
    QStandardItemModel* m_criticalLowModel;
    QStandardItemModel* m_lowModel;
    QStandardItemModel* m_adequateModel;
    
    void setupTables();
    void loadProductData();
};

#endif // STOCKPRODUCTDIALOG_H
