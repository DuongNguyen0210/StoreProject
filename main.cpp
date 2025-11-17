#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QApplication>

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

    MainWindow w;
    w.show();
    return a.exec();
}
