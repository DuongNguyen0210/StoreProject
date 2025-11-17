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
#include "User.h"

#include "AddProductToStore.h"
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
    QStandardItemModel *modelTable;
    int curTableProduct;
    Store* store;  // Thay đổi thành pointer
    Bill *currentBill;
    QStandardItemModel *modelHoaDon;
    QStandardItemModel *modelLastBill;
    User* currentUser;  // Thêm user hiện tại (Task 5)

public:
    MainWindow(User* user, Store* store, QWidget *parent = nullptr);  // Cập nhật constructor
    ~MainWindow();

private slots:
    void on_ToanBo_clicked();
    void on_DoAn_clicked();
    void on_ThucUong_clicked();
    void on_DoGiaDung_clicked();
    void on_BtnSearch_clicked();

    void onAddSanPham(const QModelIndex &index);
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

    void onToggleMenuClicked();
    void onCancelOrderClicked();

private:
    void setupTable();
    void setupHoaDonTable();
    void setupLastBill();

    void updateHoaDonView();
    void updateLastBillView();

    void loadProductsFromStore(int typeFilter);
    void loadProductsFromStoreWithKeyWord(const QString &keyword);

    void resetHoaDon();
    void finalizeThanhToan(const QString& paymentMethod);

    void applyPermissions();  // Thêm hàm phân quyền (Task 5)
};

#endif
