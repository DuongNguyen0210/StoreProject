#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(Store* store, QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog), m_store(store), m_loggedInUser(nullptr)
{
    ui->setupUi(this);

    ui->lblError->setVisible(false);

    connect(ui->btnLogin, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);

    connect(ui->txtUsername, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    connect(ui->txtPassword, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);

    connect(ui->chkShowPassword, &QCheckBox::toggled, this, [this](bool checked)
    {
        if (checked)
            ui->txtPassword->setEchoMode(QLineEdit::Normal);
        else
            ui->txtPassword->setEchoMode(QLineEdit::Password);
    });
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

    ui->lblError->setVisible(false);

    qDebug() << username << " " << password << '\n';

    if (username.isEmpty() || password.isEmpty())
    {
        ui->lblError->setText("Vui lòng nhập đầy đủ tên đăng nhập và mật khẩu!");
        ui->lblError->setVisible(true);
        return;
    }

    User* user = m_store->findUserByName(username);

    if (user == nullptr || user->getPassword() != password)
    {
        ui->lblError->setText("Tên đăng nhập hoặc mật khẩu không hợp lệ!");
        ui->lblError->setVisible(true);
        return;
    }

    m_loggedInUser = user;
    accept();
}
