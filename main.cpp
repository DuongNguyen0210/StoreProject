#include "ui/mainwindow.h"
#include "dialogs/logindialog.h"
#include "core/Store.h"
#include "models/Manager.h"
#include "models/Cashier.h"
#include "core/StorePersistence.h"

#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QCoreApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList styleModules =
    {
        ":/styles/styles/base.qss",
        ":/styles/styles/frames.qss",
        ":/styles/styles/buttons.qss",
        ":/styles/styles/inputs.qss",
        ":/styles/styles/tables.qss",
        ":/styles/styles/labels.qss",
        ":/styles/styles/scrollbars.qss",
        ":/styles/styles/misc.qss",
        ":/styles/styles/login.qss",
        ":/styles/styles/thongke.qss",
        ":/styles/styles/dialogs.qss"
    };
    QString combinedStyle;
    for (const QString& module : styleModules)
    {
        QFile file(module);
        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream stream(&file);
            combinedStyle += stream.readAll() + "\n";
            file.close();
        }
    }

    a.setStyleSheet(combinedStyle);

    Store store("Cửa hàng tạp hóa");

    QString dataPath = QCoreApplication::applicationDirPath() + "/../../../data/store_data.txt";
    dataPath = QDir::cleanPath(dataPath);
    StorePersistence::load(store, dataPath);

    LoginDialog loginDialog(&store);
    if (loginDialog.exec() != QDialog::Accepted)
        return 0;

    User* loggedInUser = loginDialog.getLoggedInUser();
    MainWindow w(loggedInUser, &store);
    w.show();

    int result = a.exec();

    StorePersistence::save(store, dataPath);

    return result;
}
