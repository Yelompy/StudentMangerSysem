#ifndef SYSTEMSETTINGSWIDGET_H
#define SYSTEMSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class SyStemSettingsWidget;
}
class QLineEdit;
class QPushButton;
class QCheckBox;
class QPushButton;
class QTextEdit;
class QGridLayout;
class SyStemSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SyStemSettingsWidget(QWidget *parent = nullptr);
    ~SyStemSettingsWidget();

private:
    void creatUI();
    void loadSettings();
    void browseDatabasePath();
    void saveSettings();
    void updatePassword();
    bool validatePasswordChanged();
    QLineEdit* dbPathEdit;
    QPushButton* browseBtn;
    QLineEdit* oldPwEdit;
    QLineEdit* newPwEdit;
    QLineEdit* confirmPwEdit;
    QCheckBox* cacheCheckBox;
    QPushButton* saveBtn;
    QTextEdit*  versonInfoEdit;
    QGridLayout* mainLayout;
    Ui::SyStemSettingsWidget *ui;
};

#endif // SYSTEMSETTINGSWIDGET_H
