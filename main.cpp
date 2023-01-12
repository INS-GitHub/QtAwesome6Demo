#include "mainwindow.h"

#include <QtWidgets/QApplication>
#include <QFile>

void setStyle(QApplication& app)
{
    QFile qss(":/lightstyle.qss");
    qss.open(QFile::ReadOnly);
    app.setStyleSheet(qss.readAll());
    qss.close();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setStyle(a);

    MainWindow w;
    w.setFixedSize(1280, 800);
    w.show();
    return a.exec();
}
