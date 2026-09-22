#include "mainwindow.h"
#include "databasemanager.h"
#include "logindialog.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DataBaseManager::instance();
    QFile styleFile(":/style/style.qss");
    if(styleFile.open(QFile::ReadOnly)){
        QString stylesheet = QString(styleFile.readAll());
        a.setStyleSheet(stylesheet);//应用样式表
        styleFile.close();
    }else{
        qWarning() << "Failed to load stylesheet :" << styleFile.errorString();
    }
    LoginDialog loginDlg;
    if(loginDlg.exec() == QDialog::Accepted){
        MainWindow w;
        w.show();
        return QCoreApplication::exec();
    }
    return  0;
}
