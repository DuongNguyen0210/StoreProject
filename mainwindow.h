#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QGridLayout>

#include "Store.h"
#include "Food.h"
#include "Beverage.h"
#include "HouseholdItem.h"
#include "Bill.h"

#include "addProductToStore.h"
#include "ThongKe.h"
#include "AddCustomerToStore.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    int curTableProduct;
    Store store;
    Bill *currentBill;
    QStandardItemModel *modelHoaDon;
    QStandardItemModel *modelLastBill;

    QGridLayout* productsLayout = nullptr;
    static constexpr int PRODUCT_COLUMNS = 4;
    void clearProductsGrid();
    void addProductCard(Product* p);
    QString getProductTypeName(Product* p) const;

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

    void onAddSanPham(Product *p);
    void onRemoveSanPhamDoubleClicked(const QModelIndex &index);
    void onTimKhachPressed();
    void onDungDiemClicked();
    void onXuatHoaDonClicked();
    void onQuayLaiClicked();
    void onThanhToanTienMatClicked();
    void onThanhToanTheClicked();
    void onThanhToanClicked();

    void on_ThemHang_clicked();
    void on_ThongKe_clicked();
    void on_KhachHang_clicked();

private:
    void setupTable();
    void loadProductsFromStore(int typeFilter);
    void loadProductsFromStoreWithKeyWord(const QString &keyword);

    void setupHoaDonTable();
    void updateHoaDonView();
    void resetHoaDon();
    void finalizeThanhToan(const QString& paymentMethod);

    void setupLastBill();
    void updateLastBillView();
};

#endif
