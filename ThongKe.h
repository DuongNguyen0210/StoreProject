#ifndef THONGKE_H
#define THONGKE_H

#include <QDialog>
#include <QStandardItemModel>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
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
    void onRevenueTimeframeChanged(int index);

private:
    Ui::ThongKe *ui;
    Store* m_store;
    QStandardItemModel* m_model;

    // Hàm thiết lập giao diện cũ (Table)
    void setupTable();
    void loadBillHistory();

    // Hàm thiết lập Dashboard mới
    void setupDashboard();
    void updateKPICards();

    // Hàm tạo các biểu đồ
    void createRevenueChart();
    void createTop5ProductsChart();
    void createWarningsChart();

    // Hàm tính toán
    double calculateTotalProfit();
};

#endif
