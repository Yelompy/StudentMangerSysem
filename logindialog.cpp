#include "logindialog.h"
#include "ui_logindialog.h"
#include "settings.h"
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QByteArray>
 #include <QCryptographicHash>
#include <QMessageBox>
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    checkAndCreateInitialUser();
    setWindowTitle("教学管理系统");
    setWindowIcon(QIcon(":/ico/login.png"));
    setFixedSize(260, 180);
    //创建控件
    QLabel* usernameLabel = new QLabel("用户名:", this);
    QLabel* passwordLabel = new QLabel("密  码:", this);
    usernameLineEdit = new QLineEdit(this);
    passwordLineEtid = new QLineEdit(this);
    passwordLineEtid->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("登录", this);
    cancelButton = new QPushButton("取消", this);
    //布局
    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(usernameLabel, 0, 0);
    mainLayout->addWidget(usernameLineEdit, 0, 1);
    mainLayout->addWidget(passwordLabel, 1, 0);
    mainLayout->addWidget(passwordLineEtid, 1, 1);
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout, 2, 0, 1, 2);//跨两列
    setLayout(mainLayout);
    //连接
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::reject);
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::on_loginButton_clicked);
    //尝试加载缓存登录信息
    QString cachedUsername, cachedPassword;
    if(LoadCredentials(cachedUsername, cachedPassword)){
        usernameLineEdit->setText(cachedUsername);
        passwordLineEtid->setText(cachedPassword);
    }
}
LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::checkAndCreateInitialUser()
{
    const QString initialUsername = "admin";
    const QString initialPassword = "admin123";
    QSqlQuery query;
    query.exec("SELECT COUNT(*) FROM users");
    if(query.next() && query.value(0).toInt() == 0){
        QString hashedInitialPassword = hashPassword(initialPassword);
        query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
        query.bindValue(":username", initialUsername);
        query.bindValue(":password", hashedInitialPassword);
        if(!query.exec())   qDebug() << "插入初始用户失败:" << query.lastError().text();
    }
}
//哈希加密
QString LoginDialog::hashPassword(const QString &password)
{
    QByteArray passwordBytes = password.toUtf8();
    QByteArray hashBytes =  QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
    return QString(hashBytes.toHex());
}
//用户登陆验证
void LoginDialog::on_loginButton_clicked()
{
    QString username = usernameLineEdit->text();
    QString password = passwordLineEtid->text();
    if(validateUser(username, password)){
        saveCredentials(username, password);
        Settings::instance().setLastUser(username);
        accept();
    }
    else QMessageBox::warning(this, "登陆失败", "用户名或密码错误。");
}
//验证数据
bool LoginDialog::validateUser(const QString &username, const QString &password)
{
    QString hashedPassword = hashPassword(password);
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", hashedPassword);
    if(!query.exec()){
        qDebug() << "查询错误" << query.lastError().text();
        return false;
    }
    return query.next();
}
//保存数据
void LoginDialog::saveCredentials(const QString &username, const QString &password)
{
    Settings::instance().getQSettings().setValue("username", username);
    QString encryptedPassword = encryptPassword(password);
    Settings::instance().getQSettings().setValue("password", encryptedPassword);
}
//密码加密
const QByteArray encryptionKey = "your_encryption_key"; //密钥
QString LoginDialog::encryptPassword(const QString &password)
{
    QByteArray passwordBytes = password.toUtf8();
    QByteArray encryptedBytes;
    for(int i = 0; i < passwordBytes.size(); ++i){
        encryptedBytes.append(passwordBytes[i] ^ encryptionKey[i % encryptionKey.size()]);
    }
    return encryptedBytes.toBase64();
}
//密码解密
QString LoginDialog::decryptPassword(const QString &encryptedPassword)
{
    QByteArray encryptedBytes = QByteArray::fromBase64(encryptedPassword.toUtf8());
    QByteArray decryptedBytes;
    for(int i = 0; i < encryptedBytes.size() ; ++i){
        decryptedBytes.append(encryptedBytes[i] ^ encryptionKey[i % encryptionKey.size()]);
    }
    return QString::fromUtf8(decryptedBytes);
}
//加载缓存的用户名和加密后的密码
bool LoginDialog::LoadCredentials(QString &username, QString &password)
{
    username = Settings::instance().getQSettings().value("username").toString();
    QString encryptedPassword = Settings::instance().getQSettings().value("password").toString();
    if(!username.isEmpty() && !encryptedPassword.isEmpty()){
        password = decryptPassword(encryptedPassword);
        return true;
    }
    return false;
}


