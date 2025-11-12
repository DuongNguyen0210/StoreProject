#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

#include "Store.h"
#include "Food.h"
#include "Beverage.h"
#include "HouseholdItem.h"
#include "Bill.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ToanBo_clicked();
    void on_DoAn_clicked();
    void on_ThucUong_clicked();
    void on_DoGiaDung_clicked();
    void on_BtnSearch_clicked();
    void on_btnMenu_clicked();
    void on_btnOrder_clicked();

    void onSanPhamDoubleClicked(const QModelIndex &index);
    void onRemoveSanPhamDoubleClicked(const QModelIndex &index);
    void onTimKhachPressed();
    void onDungDiemClicked();
    void onXuatHoaDonClicked();
    void onQuayLaiClicked();
    void onThanhToanTienMatClicked();
    void onThanhToanTheClicked();

private:
    void setupTable();
    void loadProductsFromStore(int typeFilter);
    void loadProductsFromStoreWithKeyWord(const QString &keyword);

    void setupHoaDonTable();
    void updateHoaDonView();
    void resetHoaDon();
    void finalizeThanhToan(const QString& paymentMethod);

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    int curTableProduct;
    Store store;
    Bill *currentBill;
    QStandardItemModel *modelHoaDon;
};

#endif
