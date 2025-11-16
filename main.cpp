#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/themes/dark_theme.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    a.setStyleSheet(stream.readAll());
    MainWindow w;
    w.show();
    return a.exec();
}
