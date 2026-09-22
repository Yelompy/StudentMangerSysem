#include "systemsettingswidget.h"
#include "ui_systemsettingswidget.h"
#include "settings.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
SyStemSettingsWidget::SyStemSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SyStemSettingsWidget)
{
    ui->setupUi(this);
    creatUI();
    loadSettings();
}

SyStemSettingsWidget::~SyStemSettingsWidget()
{
    delete ui;
}

void SyStemSettingsWidget::creatUI()
{
    dbPathEdit = new QLineEdit(this);
    browseBtn = new QPushButton("浏览...", this);
    oldPwEdit = new QLineEdit(this);
    newPwEdit = new QLineEdit(this);
    confirmPwEdit = new QLineEdit(this);
    cacheCheckBox = new QCheckBox("记住登录信息", this);
    saveBtn = new QPushButton("保存", this);
    versonInfoEdit = new QTextEdit(this);

    oldPwEdit->setEchoMode(QLineEdit::Password);
    newPwEdit->setEchoMode(QLineEdit::Password);
    confirmPwEdit->setEchoMode(QLineEdit::Password);

    versonInfoEdit->setPlainText("教学管理系统 1.0\n开发环境: QT C++ 6.11.0, Qt Creator 19.0.1, Win11");
    versonInfoEdit->setReadOnly(true);
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(new QLabel("数据库路径: ", this), 0, 0);
    mainLayout->addWidget(dbPathEdit, 0, 1);
    mainLayout->addWidget(browseBtn, 0, 2);
    mainLayout->addWidget(new QLabel("旧密码: ", this), 1, 0);
    mainLayout->addWidget(oldPwEdit, 1, 1, 1, 2);
    mainLayout->addWidget(new QLabel("新密码: ",this), 2, 0);
    mainLayout->addWidget(newPwEdit, 2, 1, 1, 2);
    mainLayout->addWidget(new QLabel("确认密码", this), 3, 0);
    mainLayout->addWidget(confirmPwEdit, 3, 1, 1, 2);
    mainLayout->addWidget(cacheCheckBox, 4, 0, 1, 3);
    mainLayout->addWidget(saveBtn, 5, 1, 1, 2);
    mainLayout->addWidget(versonInfoEdit, 6, 0, 1, 3);
    setLayout(mainLayout);
    connect(browseBtn, &QPushButton::clicked, this, &SyStemSettingsWidget::browseDatabasePath);
    connect(saveBtn, &QPushButton::clicked, this, &SyStemSettingsWidget::saveSettings);
}

void SyStemSettingsWidget::loadSettings()
{
    dbPathEdit->setText(Settings::instance().getDatabasePath());
    cacheCheckBox->setChecked(Settings::instance().getCacheEnabled());
}

void SyStemSettingsWidget::browseDatabasePath()
{
    QString path = QFileDialog::getSaveFileName(
        this,
        "选择数据库文件",
        "",
        "SQLite Databases (*.db *.sqlite)"
        );
    if(!path.isEmpty()) dbPathEdit->setText(path);
}

void SyStemSettingsWidget::saveSettings()
{
    QString newDbPath = dbPathEdit->text();
    Settings::instance().setDatabasePath(newDbPath);
    Settings::instance().setCacheEnabled(cacheCheckBox->isChecked());
    if(!newPwEdit->text().isEmpty()){
        updatePassword();
    }
    if(newDbPath != Settings::instance().getDatabasePath()){
        QMessageBox::information(this, "提示", "数据库路径修改将在重启后生效");
    }
}

void SyStemSettingsWidget::updatePassword()
{
    if(!validatePasswordChanged())  return;
    QString newHash = QString(QCryptographicHash::hash(
        newPwEdit->text().toUtf8(),
        QCryptographicHash::Sha256
                                  ).toHex());
    QSqlQuery query;
    query.prepare("UPDATE users SET password = ? WHERE username = ?");
    query.addBindValue(newHash);
    query.addBindValue(Settings::instance().getLastUser());
    if(!query.exec()){
        QMessageBox::critical(this, "错误", "密码更新失败: " + query.lastError().text());
        return;
    }
    QMessageBox::information(this, "提示", "密码更新成功");
}
//密码有效性验证
bool SyStemSettingsWidget::validatePasswordChanged()
{
    if(newPwEdit->text() != confirmPwEdit->text()){
        QMessageBox::warning(this, "错误", "新密码与确认密码不一致");
        return false;
    }
    QString currentUser = Settings::instance().getLastUser();
    if(currentUser.isEmpty()){
        QMessageBox::warning(this, "错误", "未找到当前用户");
        return false;
    }
    QSqlQuery query;
    query.prepare("SELECT password FROM users WHERE username = ?");
    query.addBindValue(currentUser);
    if(!query.exec() || !query.next()){
        QMessageBox::critical(this, "错误", "数据库查询失败: " + query.lastError().text());
        return false;
    }
    QString storeHash = query.value(0).toString();
    QString inputHash = QString(QCryptographicHash::hash(
        oldPwEdit->text().toUtf8(),
        QCryptographicHash::Sha256
                                    ).toHex());
    if(storeHash != inputHash){
        QMessageBox::warning(this, "错误", "旧密码不正确");
        return false;
    }
    return true;

}
