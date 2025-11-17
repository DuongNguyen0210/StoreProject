#ifndef THONGKE_H
#define THONGKE_H

#include <QDialog>
#include <QStandardItemModel>
#include "Store.h"

namespace Ui
{
class ThongKe;
}

class ThongKe : public QDialog
{
    Q_OBJECT

public:
    explicit ThongKe(Store* store, QWidget *parent = nullptr);
    ~ThongKe();

private slots:
    void onBillDoubleClicked(const QModelIndex &index);

private:
    Ui::ThongKe *ui;
    Store* m_store;
    QStandardItemModel* m_model;

    void setupTable();
    void loadBillHistory();
};

#endif
