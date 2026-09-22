#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}
class QPushButton;
class QLineEdit;
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    void checkAndCreateInitialUser();
    QString hashPassword(const QString& password);
    void on_loginButton_clicked();
    bool validateUser(const QString& username, const QString& password);
    void saveCredentials(const QString& username, const QString& password);
    QString encryptPassword(const QString& password);
    bool LoadCredentials(QString& username, QString& password);
    QString decryptPassword(const QString& encryptedPassword);
    QLineEdit* usernameLineEdit;
    QLineEdit* passwordLineEtid;
    QPushButton* loginButton;
    QPushButton* cancelButton;
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
