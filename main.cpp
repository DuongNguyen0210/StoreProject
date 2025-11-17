#include "mainwindow.h"
#include "logindialog.h"
#include "Store.h"
#include "Manager.h"
#include "Cashier.h"
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile styleFile(":/styles/stylesheet.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&styleFile);
        QString style = stream.readAll();
        a.setStyleSheet(style);
        styleFile.close();
    }

    Store store("Cửa hàng tạp hóa");

    store.addUser(new Manager("M1", "admin", "admin123"));
    store.addUser(new Cashier("C1", "nhanvien", "nv123"));
    store.addUser(new Manager("M2", "quanly", "ql456"));

    LoginDialog loginDialog(&store);
    if (loginDialog.exec() != QDialog::Accepted)
    {
        return 0;
    }

    User* loggedInUser = loginDialog.getLoggedInUser();
    if (!loggedInUser)
    {
        QMessageBox::critical(nullptr, "Lỗi", "Không thể xác định người dùng đăng nhập!");
        return 0;
    }

    MainWindow w(loggedInUser, &store);
    w.show();

    return a.exec();
}
