#include "AddCustomerToStore.h"
#include "ui_AddCustomerToStore.h"
#include "Customer.h"    // Cần để tạo khách hàng mới
#include "Exceptions.h"  // Cần để bắt lỗi trùng
#include <QMessageBox>
#include <QHeaderView>

CustomerDialog::CustomerDialog(Store* store, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomerDialog),
    m_store(store)
{
    ui->setupUi(this);

    // Kết nối nút bấm với slot [quan trọng hơn cả auto-connect]

    setupTable();
    loadCustomers();
}

CustomerDialog::~CustomerDialog()
{
    delete ui;
}

void CustomerDialog::setupTable()
{
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(4);
    m_model->setHeaderData(0, Qt::Horizontal, "ID");
    m_model->setHeaderData(1, Qt::Horizontal, "Tên Khách Hàng");
    m_model->setHeaderData(2, Qt::Horizontal, "Số Điện Thoại");
    m_model->setHeaderData(3, Qt::Horizontal, "Điểm Tích Lũy");

    ui->tableCustomers->setModel(m_model);
    ui->tableCustomers->setEditTriggers(QAbstractItemView::NoEditTriggers); // Không cho sửa
    ui->tableCustomers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void CustomerDialog::loadCustomers()
{
    // Xóa dữ liệu cũ
    m_model->removeRows(0, m_model->rowCount());

    // Tải dữ liệu mới từ Store
    m_store->forEachCustomer([this](const QString&, Customer* c) {
        if (!c) return;
        QList<QStandardItem*> row;
        row << new QStandardItem(c->getId());
        row << new QStandardItem(c->getName());
        row << new QStandardItem(c->getPhone());
        row << new QStandardItem(QString::number(c->getPoints()));
        m_model->appendRow(row);
    });
}

void CustomerDialog::on_btnAddCustomer_clicked()
{
    // 1. Lấy dữ liệu từ ô nhập
    QString name = ui->txtName->text().trimmed();
    QString phone = ui->txtPhone->text().trimmed();

    // 2. SỬA LỖI 1: Kiểm tra đầu vào NGAY LẬP TỨC
    if (name.isEmpty() || phone.isEmpty())
    {
        QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng nhập cả tên và số điện thoại.");
        return; // <-- Dừng lại ngay nếu thiếu
    }

    // 3. SỬA LỖI 2: Kiểm tra trùng SĐT TRƯỚC KHI tạo Customer
    if (m_store->findCustomerByPhone(phone))
    {
        QMessageBox::critical(this, "Lỗi", "Đã tồn tại: Số điện thoại này đã được sử dụng.");
        return; // <-- Dừng lại trước khi "đốt" ID
    }

    // 4. Nếu mọi thứ đều ổn, TIẾP TỤC thêm khách hàng
    try
    {
        // Bây giờ việc gọi new Customer là an toàn
        Customer* c = new Customer("", name, phone, 0);
        m_store->addCustomer(c);

        // 5. Tải lại bảng và xóa ô nhập
        loadCustomers();
        ui->txtName->clear();
        ui->txtPhone->clear();
    }
    catch (const DuplicateException& e) // Bắt các lỗi trùng lặp khác (nếu có)
    {
        QMessageBox::critical(this, "Lỗi", e.what());
    }
}
