#include "BillDetailDialog.h"
#include "ui_BillDetailDialog.h"
#include "Bill.h"
#include "User.h"
#include "Customer.h"
#include <QHeaderView>

BillDetailDialog::BillDetailDialog(Bill* bill, QWidget *parent)
    : QDialog(parent), ui(new Ui::BillDetailDialog), m_bill(bill)
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
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(4);
    m_model->setHeaderData(0, Qt::Horizontal, "Tên Sản Phẩm");
    m_model->setHeaderData(1, Qt::Horizontal, "Số Lượng");
    m_model->setHeaderData(2, Qt::Horizontal, "Đơn Giá");
    m_model->setHeaderData(3, Qt::Horizontal, "Thành Tiền");

    ui->tableItems->setModel(m_model);
    ui->tableItems->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableItems->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableItems->setAlternatingRowColors(true);
}

void BillDetailDialog::loadBillDetails()
{
    if (!m_bill) return;

    // Hiển thị thông tin hóa đơn
    ui->lblBillId->setText(QString("Mã hóa đơn: %1").arg(m_bill->getId()));

    QString customerName = "Khách Lẻ";
    if (m_bill->getCustomer())
        customerName = m_bill->getCustomer()->getName();
    ui->lblCustomer->setText(QString("Khách hàng: %1").arg(customerName));

    QString dateTime = m_bill->getCreatedDate().toString("dd/MM/yyyy HH:mm:ss");
    ui->lblDateTime->setText(QString("Ngày giờ: %1").arg(dateTime));

    QString employee = "N/A";
    if (m_bill->getCreatedBy())
        employee = m_bill->getCreatedBy()->getName();
    ui->lblEmployee->setText(QString("Nhân viên: %1").arg(employee));

    QString paymentMethod = "Chưa thanh toán";
    if (m_bill->getPayment())
        paymentMethod = m_bill->getPayment()->getMethodName();
    ui->lblPaymentMethod->setText(QString("Phương thức: %1").arg(paymentMethod));

    // Hiển thị các sản phẩm
    m_model->removeRows(0, m_model->rowCount());
    const auto& items = m_bill->getItems();

    for (const BillItem& item : items)
    {
        QList<QStandardItem*> row;
        row << new QStandardItem(item.getProduct()->getName());
        row << new QStandardItem(QString::number(item.getQuantity()));
        row << new QStandardItem(QString::number(item.getUnitPrice(), 'f', 0) + " đ");
        row << new QStandardItem(QString::number(item.getLineTotal(), 'f', 0) + " đ");
        m_model->appendRow(row);
    }

    // Hiển thị tổng tiền
    double subTotal = m_bill->getSubTotal();
    double total = m_bill->getTotal();

    ui->lblSubTotal->setText(QString("Tổng tiền: %1 đ")
                                 .arg(QString::number(subTotal, 'f', 0)));

    if (subTotal != total)
    {
        double discount = subTotal - total;
        ui->lblDiscount->setText(QString("Giảm giá: -%1 đ")
                                     .arg(QString::number(discount, 'f', 0)));
        ui->lblDiscount->setVisible(true);
    }
    else
    {
        ui->lblDiscount->setVisible(false);
    }

    ui->lblTotal->setText(QString("Thành tiền: %1 đ")
                              .arg(QString::number(total, 'f', 0)));
}
