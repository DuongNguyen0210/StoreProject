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

    // Load modular stylesheets
    // NOTE: Resource path = prefix + file path
    // Example: prefix="/styles" + file="styles/base.qss" = ":/styles/styles/base.qss"
    QStringList styleModules = {
        ":/styles/styles/base.qss",
        ":/styles/styles/frames.qss",
        ":/styles/styles/buttons.qss",
        ":/styles/styles/inputs.qss",
        ":/styles/styles/tables.qss",
        ":/styles/styles/labels.qss",
        ":/styles/styles/scrollbars.qss",
        ":/styles/styles/misc.qss",
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
        else
        {
            qWarning() << "Failed to load stylesheet module:" << module;
        }
    }
    
    a.setStyleSheet(combinedStyle);

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
