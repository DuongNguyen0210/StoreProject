#include "ThongKe.h"
#include "ui_ThongKe.h"
#include <QString>
#include <QDialogButtonBox>

ThongKe::ThongKe(double revenue, QWidget *parent)
    : QDialog(parent), ui(new Ui::ThongKe)
{
    ui->setupUi(this);

    QString revenueText = QString::number(revenue, 'f', 0) + " đ";

    ui->lblRevenue->setText(QString("Tổng tiền hàng bán được trong hôm này là: ") + revenueText);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ThongKe::reject);
}

ThongKe::~ThongKe()
{
    delete ui;
}
