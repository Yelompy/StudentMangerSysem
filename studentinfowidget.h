#ifndef STUDENTINFOWIDGET_H
#define STUDENTINFOWIDGET_H

#include <QWidget>

namespace Ui {
class StudentInfoWidget;
}
class QGroupBox;
class StudentInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StudentInfoWidget(QWidget *parent = nullptr);
    ~StudentInfoWidget();

private slots:
    void on_btnAdd_clicked();

    void on_btnDeleteItem_clicked();

    void on_btnDeleteLine_clicked();

private:
    QGroupBox* createFormGroup();
    QGroupBox* createPhotoGroup();
    void handleDialogAccepted(QGroupBox* formGroup,QGroupBox* photoGroup);
    void refreshTable();

    QByteArray photoData;
    Ui::StudentInfoWidget *ui;
};

#endif // STUDENTINFOWIDGET_H
