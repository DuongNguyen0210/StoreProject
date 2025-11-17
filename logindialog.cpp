#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(Store* store, QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog), m_store(store), m_loggedInUser(nullptr)
{
    ui->setupUi(this);

    // Ẩn label lỗi ban đầu
    ui->lblError->setVisible(false);

    // Kết nối nút đăng nhập
    connect(ui->btnLogin, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);

    // Cho phép nhấn Enter để đăng nhập
    connect(ui->txtUsername, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    connect(ui->txtPassword, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

User* LoginDialog::getLoggedInUser() const
{
    return m_loggedInUser;
}

void LoginDialog::onLoginClicked()
{
    QString username = ui->txtUsername->text().trimmed();
    QString password = ui->txtPassword->text().trimmed();

    // Ẩn thông báo lỗi cũ
    ui->lblError->setVisible(false);

    // Kiểm tra trống
    if (username.isEmpty() || password.isEmpty())
    {
        ui->lblError->setText("Vui lòng nhập đầy đủ tên đăng nhập và mật khẩu!");
        ui->lblError->setVisible(true);
        return;
    }

    // Tìm user theo tên
    User* user = m_store->findUserByName(username);

    if (user == nullptr)
    {
        ui->lblError->setText("Tên đăng nhập không tồn tại!");
        ui->lblError->setVisible(true);
        ui->txtPassword->clear();
        return;
    }

    // Kiểm tra mật khẩu
    if (user->getPassword() != password)
    {
        ui->lblError->setText("Mật khẩu không đúng!");
        ui->lblError->setVisible(true);
        ui->txtPassword->clear();
        return;
    }

    // Đăng nhập thành công
    m_loggedInUser = user;
    accept();
}
