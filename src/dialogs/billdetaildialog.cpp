#include "dialogs/billdetaildialog.h"
#include "ui_BillDetailDialog.h"
#include "models/Bill.h"
#include "models/User.h"
#include "models/Customer.h"
#include <QHeaderView>

BillDetailDialog::BillDetailDialog(Bill* bill, QWidget *parent) : QDialog(parent), ui(new Ui::BillDetailDialog), bill(bill)
{
    ui->setupUi(this);
    setupTable();
    loadBillDetails();
}

BillDetailDialog::~BillDetailDialog()
{
    delete ui;
}

void BillDetailDialog::setupTable()
{
    model = new QStandardItemModel(this);
    model->setColumnCount(4);
    model->setHeaderData(0, Qt::Horizontal, "Tên Sản Phẩm");
    model->setHeaderData(1, Qt::Horizontal, "Số Lượng");
    model->setHeaderData(2, Qt::Horizontal, "Đơn Giá");
    model->setHeaderData(3, Qt::Horizontal, "Thành Tiền");

    ui->tableItems->setModel(model);
    ui->tableItems->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableItems->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableItems->setAlternatingRowColors(true);
}

void BillDetailDialog::loadBillDetails()
{
    if (!bill)
        return;

    ui->lblBillId->setText(QString("Mã hóa đơn: %1").arg(bill->getId()));

    QString customerName = "Khách Lẻ";
    if (bill->getCustomer())
        customerName = bill->getCustomer()->getName();
    ui->lblCustomer->setText(QString("Khách hàng: %1").arg(customerName));

    QString dateTime = bill->getCreatedDate().toString("dd/MM/yyyy HH:mm:ss");
    ui->lblDateTime->setText(QString("Ngày giờ: %1").arg(dateTime));

    QString employee = "N/A";
    if (bill->getCreatedBy())
        employee = bill->getCreatedBy()->getName();
    ui->lblEmployee->setText(QString("Nhân viên: %1").arg(employee));

    QString paymentMethod = "Chưa thanh toán";
    if (bill->getPayment())
        paymentMethod = bill->getPayment()->getMethodName();
    ui->lblPaymentMethod->setText(QString("Phương thức: %1").arg(paymentMethod));

    model->removeRows(0, model->rowCount());
    const auto& items = bill->getItems();

    for (const BillItem& item : items)
    {
        QList<QStandardItem*> row;
        row << new QStandardItem(item.getProduct()->getName());
        row << new QStandardItem(QString::number(item.getQuantity()));
        row << new QStandardItem(QString::number(item.getUnitPrice(), 'f', 0) + " đ");
        row << new QStandardItem(QString::number(item.getLineTotal(), 'f', 0) + " đ");
        model->appendRow(row);
    }

    double subTotal = bill->getSubTotal();
    double total = bill->getTotal();

    ui->lblSubTotal->setText(QString("Tổng tiền: %1 đ").arg(QString::number(subTotal, 'f', 0)));

    if (subTotal != total)
    {
        double discount = subTotal - total;
        ui->lblDiscount->setText(QString("Giảm giá: -%1 đ").arg(QString::number(discount, 'f', 0)));
        ui->lblDiscount->setVisible(true);
    }
    else
    {
        ui->lblDiscount->setVisible(false);
    }

    ui->lblTotal->setText(QString("Thành tiền: %1 đ").arg(QString::number(total, 'f', 0)));
}
