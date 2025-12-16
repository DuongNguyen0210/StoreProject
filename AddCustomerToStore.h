#ifndef ADDCUSTOMERTOSTORE_H
#define ADDCUSTOMERTOSTORE_H

#include <QDialog>
#include <QStandardItemModel>
#include <QTimer>
#include "Store.h"

namespace Ui
{
class CustomerDialog;
}

class CustomerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomerDialog(Store* store, QWidget *parent = nullptr);
    ~CustomerDialog();

private slots:
    void onCustomerItemChanged(QStandardItem* item);
    void on_btnAddNewMember_clicked();
    void on_searchBar_textChanged(const QString& text);
    void on_filterTier_currentIndexChanged(int index);
    void on_sortBy_currentIndexChanged(int index);

private:
    void setupTable();
    void loadCustomers();
    void updateStatistics();
    void applyFiltersAndSort();
    int getTierCount(const QString& tier);
    QString getTierColor(const QString& tier);

    Ui::CustomerDialog *ui;
    Store* m_store;
    QStandardItemModel* m_model;
    QString m_currentSearchText;
    QString m_currentTierFilter;
    int m_currentSortIndex;
};

#endif
