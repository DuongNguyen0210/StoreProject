#include "ThongKe.h"
#include "User.h"
#include "Customer.h"
#include "ui_ThongKe.h"
#include "BillDetailDialog.h"
#include <QString>
#include <QHeaderView>

ThongKe::ThongKe(Store* store, QWidget *parent)
    : QDialog(parent), ui(new Ui::ThongKe), m_store(store)
{
    ui->setupUi(this);
    setupTable();
    loadBillHistory();

    connect(ui->billHistoryTable, &QTableView::doubleClicked,
            this, &ThongKe::onBillDoubleClicked);
}

ThongKe::~ThongKe()
{
    delete ui;
}

void ThongKe::setupTable()
{
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(6);
    m_model->setHeaderData(0, Qt::Horizontal, "Mã HĐ");
    m_model->setHeaderData(1, Qt::Horizontal, "Tên Khách");
    m_model->setHeaderData(2, Qt::Horizontal, "Ngày Tạo");
    m_model->setHeaderData(3, Qt::Horizontal, "Giờ Tạo");
    m_model->setHeaderData(4, Qt::Horizontal, "Tổng Tiền");
    m_model->setHeaderData(5, Qt::Horizontal, "Nhân Viên");

    ui->billHistoryTable->setModel(m_model);
    ui->billHistoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->billHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->billHistoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->billHistoryTable->setAlternatingRowColors(true);
}

void ThongKe::loadBillHistory()
{
    m_model->removeRows(0, m_model->rowCount());

    const auto& history = m_store->getBillHistory();

    for (const Bill* bill : history)
    {
        if (!bill) continue;

        QList<QStandardItem*> row;

        row << new QStandardItem(bill->getId());

        QString customerName = "Khách lẻ";
        if (bill->getCustomer())
            customerName = bill->getCustomer()->getName();
        row << new QStandardItem(customerName);

        QString dateStr = bill->getCreatedDate().toString("dd/MM/yyyy");
        row << new QStandardItem(dateStr);

        QString timeStr = bill->getCreatedDate().toString("HH:mm:ss");
        row << new QStandardItem(timeStr);

        QString totalStr = QString::number(bill->getTotal(), 'f', 0) + " đ";
        row << new QStandardItem(totalStr);

        User * u = bill->getCreatedBy();
        if(u)
            row << new QStandardItem(u->getName());
        else
            row << new QStandardItem("N/A");

        m_model->appendRow(row);
    }

    double totalRevenue = m_store->getTotalRevenue();
    ui->lblRevenue->setText(QString("Tổng doanh thu: %1 đ")
                                .arg(QString::number(totalRevenue, 'f', 0)));
}

void ThongKe::onBillDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString billId = m_model->item(index.row(), 0)->text();

    const auto& history = m_store->getBillHistory();
    Bill* selectedBill = nullptr;

    for (Bill* bill : history)
    {
        if (bill && bill->getId() == billId)
        {
            selectedBill = bill;
            break;
        }
    }

    if (selectedBill)
    {
        BillDetailDialog detailDialog(selectedBill, this);
        detailDialog.exec();
    }
}
