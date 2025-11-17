#ifndef BILLDETAILDIALOG_H
#define BILLDETAILDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "Bill.h"

namespace Ui {
class BillDetailDialog;
}

class BillDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BillDetailDialog(Bill* bill, QWidget *parent = nullptr);
    ~BillDetailDialog();

private:
    Ui::BillDetailDialog *ui;
    Bill* m_bill;
    QStandardItemModel* m_model;

    void setupTable();
    void loadBillDetails();
};

#endif
