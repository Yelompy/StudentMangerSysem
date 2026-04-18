#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile styleFile(":/style/style.qss");
    if(styleFile.open(QFile::ReadOnly)){
        QString stylesheet = QString(styleFile.readAll());
        a.setStyleSheet(stylesheet);//应用样式表
        styleFile.close();
    }else{
        qWarning() << "Failed to load stylesheet :" << styleFile.errorString();
    }
    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
