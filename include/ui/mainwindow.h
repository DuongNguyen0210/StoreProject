#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QComboBox>
#include <QTimer>

#include "core/Store.h"
#include "models/Food.h"
#include "models/Beverage.h"
#include "models/HouseholdItem.h"
#include "models/Bill.h"
#include "models/User.h"

#include "dialogs/AddProductToStore.h"
#include "ui/ThongKe.h"
#include "dialogs/ManageCustomer.h"
#include "dialogs/ManageInventory.h"

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
    Store* store;
    Bill *currentBill;
    QStandardItemModel *modelHoaDon;
    QStandardItemModel *modelLastBill;
    User* currentUser;

    QComboBox* sortComboBox;

    enum SortCriteria {
        SORT_DEFAULT,
        SORT_PRICE_ASC,
        SORT_PRICE_DESC,
        SORT_QUANTITY_ASC,
        SORT_QUANTITY_DESC,
        SORT_EXPIRY_ASC,
        SORT_EXPIRY_DESC,
        SORT_VOLUME_ASC,
        SORT_VOLUME_DESC,
        SORT_WARRANTY_ASC,
        SORT_WARRANTY_DESC
    };

    SortCriteria currentSortCriteria;

public:
    MainWindow(User* user, Store* store, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_ToanBo_clicked();
    void on_DoAn_clicked();
    void on_ThucUong_clicked();
    void on_DoGiaDung_clicked();
    void on_BtnSearch_clicked();
    //void on_SearchText_changed(const QString& text);

    void onAddSanPham(const QModelIndex &index);
    void onEditSanPhamDoubleClicked(const QModelIndex &index);
    void onTimKhachPressed();
    void onXuatHoaDonClicked();
    void onQuayLaiClicked();
    void onThanhToanTienMatClicked();
    void onThanhToanTheClicked();
    void onThanhToanClicked();

    void on_ThemHang_clicked();
    void on_ThongKe_clicked();
    void on_KhachHang_clicked();
    void on_QuanLyKho_clicked();

    void onToggleMenuClicked();
    void onCancelOrderClicked();

    void onSortCriteriaChanged(int index);

private:
    void setupTable();
    void setupHoaDonTable();
    void setupLastBill();
    void setupSortComboBox();

    void updateHoaDonView();
    void updateLastBillView();

    void loadProductsFromStore(int typeFilter);
    void loadProductsFromStoreWithKeyWord(const QString &keyword);

    void loadAndSortProducts(int typeFilter);
    void applySortingAndFiltering(std::vector<Product*>& products);

    static bool compareNameAsc(Product* a, Product* b);
    static bool comparePriceAsc(Product* a, Product* b);
    static bool comparePriceDesc(Product* a, Product* b);
    static bool compareQuantityAsc(Product* a, Product* b);
    static bool compareQuantityDesc(Product* a, Product* b);
    static bool compareExpiryAsc(Product* a, Product* b);
    static bool compareExpiryDesc(Product* a, Product* b);
    static bool compareVolumeAsc(Product* a, Product* b);
    static bool compareVolumeDesc(Product* a, Product* b);
    static bool compareWarrantyAsc(Product* a, Product* b);
    static bool compareWarrantyDesc(Product* a, Product* b);

    static QDate getProductDate(Product* p);

    void resetHoaDon();
    void finalizeThanhToan(const QString& paymentMethod);

    void applyPermissions();
};

#endif
