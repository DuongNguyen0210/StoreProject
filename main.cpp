#include "mainwindow.h"
#include "logindialog.h"
#include "Store.h"
#include "Manager.h"
#include "Cashier.h"
#include "StorePersistence.h"

#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QCoreApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile styleFile(":/styles/stylesheet.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&styleFile);
        a.setStyleSheet(stream.readAll());
        styleFile.close();
    }

    Store store("Cửa hàng tạp hóa");

    QString dataPath = QCoreApplication::applicationDirPath() + QDir::separator() + "store_data.txt";
    StorePersistence::load(store, dataPath);

    LoginDialog loginDialog(&store);
    if (loginDialog.exec() != QDialog::Accepted)
    {
        return 0;
    }

    User* loggedInUser = loginDialog.getLoggedInUser();
    MainWindow w(loggedInUser, &store);
    w.show();

    int result = a.exec();

    StorePersistence::save(store, dataPath);

    return result;
}
