#include "studentinfowidget.h"
#include "ui_studentinfowidget.h"
#include <QSqlQuery>
#include <QDialog>
#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QFileDialog>
#include <QStandardPaths>
#include <QBuffer>
#include <QMessageBox>
#include <QSqlError>
#include "tabledelegates.h"
StudentInfoWidget::StudentInfoWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudentInfoWidget)
{
    ui->setupUi(this);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(100);

    //性别列代理
    ComboBoxDelegate* genderDelegate = new ComboBoxDelegate(this);
    genderDelegate->setItem(QStringList()<<"男"<<"女");
    ui->tableWidget->setItemDelegateForColumn(2, genderDelegate);
    //进度列代理
    ComboBoxDelegate* progressDelegate = new ComboBoxDelegate(this);
    progressDelegate->setItem(QStringList()<<"0%"<<"20%"<<"40%"<<"60%"<<"80%"<<"100%");
    ui->tableWidget->setItemDelegateForColumn(6,progressDelegate);

    //日期列代理
    ui->tableWidget->setItemDelegateForColumn(3, new DataEditDelegate(this));
    //图片列处理
    ui->tableWidget->setItemDelegateForColumn(7, new ImageDelegate(this));
    refreshTable();
}

StudentInfoWidget::~StudentInfoWidget()
{
    delete ui;
}

void StudentInfoWidget::refreshTable()
{
    ui->tableWidget->blockSignals(true);    //在加载表格时，阻止表格发出itemChanged等信号，避免不必要的信号处理
    ui->tableWidget->setRowCount(0);        //清空表格，设置行数为0

    QSqlQuery query("SELECT * FROM studentInfo");
    while(query.next()){
        int row = ui->tableWidget->rowCount();//查看目前有多少行
        ui->tableWidget->insertRow(row);    //行是从0开始算的，所以目前的行数比index大1
        for(int col = 0; col < ui->tableWidget->columnCount() ; ++col){
            QTableWidgetItem* item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);    //设置居中对齐
            if(col == ui->tableWidget->columnCount()-1){    //最后一列是照片，需要特殊处理
                QByteArray photoData = query.value(col).toByteArray();  //转化为qt看得懂的二进制
                if(!photoData.isEmpty()){
                    QPixmap photo;
                    photo.loadFromData(photoData);
                    item->setData(Qt::DecorationRole, photo.scaled(100,100,Qt::KeepAspectRatio));
                    item->setData(Qt::UserRole,photoData);
                }
            }
            else{
                item->setText(query.value(col).toString());
            }
            ui->tableWidget->setItem(row,col,item);//对象树，用于管理内存
        }

    }
    ui->tableWidget->blockSignals(false);
}

void StudentInfoWidget::on_btnAdd_clicked()
{
    QDialog dlg(this);  //使用Dialog创建模态对话框
    dlg.setWindowTitle("添加学生信息");
    dlg.setMinimumSize(600,400);
    //初始对话框布局
    QVBoxLayout *mainLayout = new QVBoxLayout(&dlg);
    QHBoxLayout *contentLayout = new QHBoxLayout();
    mainLayout->addLayout(contentLayout);
    //添加表单和照片区域
    QGroupBox* formGroup = createFormGroup();
    QGroupBox* photoGroup = createPhotoGroup();
    contentLayout->addWidget(formGroup,1);
    contentLayout->addWidget(photoGroup,1);//拉伸因子都设置成1
    //添加按钮区域
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* btnConfirm = new QPushButton(tr("确认"));
    QPushButton* btnCancel = new QPushButton(tr("取消"));
    //配置按钮
    btnCancel->setFixedWidth(150);
    btnConfirm->setFixedWidth(150);
    //添加按钮到布局
    btnLayout->addStretch();
    btnLayout->addWidget(btnConfirm);
    btnLayout->addWidget(btnCancel);
    btnLayout->addStretch();
    //连接按钮到信号
    connect(btnConfirm,&QPushButton::clicked,&dlg,&QDialog::accept);
    connect(btnCancel,&QPushButton::clicked,&dlg,&QDialog::accept);
    mainLayout->addLayout(btnLayout);
    //执行对话框
    if(dlg.exec() == QDialog::Accepted) handleDialogAccepted(formGroup,photoGroup);
}

QGroupBox *StudentInfoWidget::createFormGroup()
{
    QGroupBox* formGroup = new QGroupBox("基本信息");
    QFormLayout* formLayout = new QFormLayout(formGroup);
    //初始化控件
    QLineEdit* idEdit = new QLineEdit();
    idEdit->setObjectName("idEdit");
    QLineEdit* nameEdit = new QLineEdit();
    nameEdit->setObjectName("nameEdit");
    QComboBox* genderBox = new QComboBox();
    genderBox->setObjectName("genderBox");
    QDateEdit* birthdayEdit = new QDateEdit(QDate::currentDate());
    birthdayEdit->setObjectName("birthdayEdit");
    QDateEdit* joinDateEdit = new QDateEdit(QDate::currentDate());
    joinDateEdit->setObjectName("joinDateEdit");
    QLineEdit* goalEdit = new QLineEdit();
    goalEdit->setObjectName("goalEdit");
    QComboBox* progressCombo = new QComboBox();
    progressCombo->setObjectName("progressCombo");
    //配置控件
    genderBox->addItems({tr("男"),tr("女")});
    progressCombo->addItems({tr("0%"),tr("20%"),tr("40%"),tr("60%"),tr("80%"),tr("100%")});
    birthdayEdit->setDisplayFormat("yyyy-MM-dd");
    joinDateEdit->setDisplayFormat("yyyy-MM-dd");
    birthdayEdit->setCalendarPopup(true);
    joinDateEdit->setCalendarPopup(true);
    //添加控件到表单
    formLayout->addRow(tr("编号: "),idEdit);
    formLayout->addRow(tr("姓名: "),nameEdit);
    formLayout->addRow(tr("性别: "),genderBox);
    formLayout->addRow(tr("出生日期: "),birthdayEdit);
    formLayout->addRow(tr("入学日期: "),joinDateEdit);
    formLayout->addRow(tr("学习目标"),goalEdit);
    formLayout->addRow(tr("当前进度"),progressCombo);

    return formGroup;
}

QGroupBox *StudentInfoWidget::createPhotoGroup()
{
    QGroupBox* photoGroup = new QGroupBox("上传照片");
    QVBoxLayout* photoLayout = new QVBoxLayout(photoGroup);
    //初始化控件
    QLabel* lblPhotoPreview = new QLabel();
    QPushButton* btnSelectPhoto = new QPushButton(tr("选择照片"));
    //配置控件
    lblPhotoPreview->setAlignment(Qt::AlignCenter);
    lblPhotoPreview->setMinimumSize(200,200);
    btnSelectPhoto->setFixedSize(100,40);
    //添加控件到布局
    photoLayout->addWidget(lblPhotoPreview);
    photoLayout->addWidget(btnSelectPhoto,0,Qt::AlignHCenter);
    //连接照片选择功能
    connect(btnSelectPhoto,&QPushButton::clicked,[this,lblPhotoPreview](){
        QString fileName = QFileDialog::getOpenFileName(
            this,tr("选择学生照片"),
            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
            tr("图片文件(*.png *.jpg *.jpeg"));
        if(!fileName.isEmpty()){
            QPixmap pixmap(fileName);
            if(!pixmap.isNull()){
                pixmap = pixmap.scaled(//智能缩放
                    lblPhotoPreview->width() - 30,
                    lblPhotoPreview->height() - 30,
                    Qt::KeepAspectRatio
                    );
                lblPhotoPreview->setPixmap(pixmap);
                QBuffer buffer(&photoData);//转换为字符数组
                buffer.open(QIODevice::WriteOnly);
                pixmap.save(&buffer,"png");
            }
            else{
                QMessageBox::warning(this,tr("错误"),tr("无法加载图片文件!"));
            }
        }
    });
    return photoGroup;
}

void StudentInfoWidget::handleDialogAccepted(QGroupBox *formGroup, QGroupBox *photoGroup)
{
    // 1. 获取表单控件
    QLineEdit *idEdit = formGroup->findChild<QLineEdit*>("idEdit");
    QLineEdit *nameEdit = formGroup->findChild<QLineEdit*>("nameEdit");
    QComboBox *genderBox = formGroup->findChild<QComboBox*>("genderBox");
    QDateEdit *birthdayEdit = formGroup->findChild<QDateEdit*>("birthdayEdit");
    QDateEdit *joinDateEdit = formGroup->findChild<QDateEdit*>("joinDateEdit");
    QLineEdit *goalEdit = formGroup->findChild<QLineEdit*>("goalEdit");
    QComboBox *progressCombo = formGroup->findChild<QComboBox*>("progressCombo");
    //数据校验
    if(idEdit->text().isEmpty() || nameEdit->text().isEmpty()){
        QMessageBox::warning(this,tr("错误"),tr("学号和姓名不能为空!"));
        return ;
    }
    //检查学号唯一性
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT id FROM studentInfo WHERE id = ?");
    checkQuery.addBindValue(idEdit->text());
    if (checkQuery.exec() && checkQuery.next()) {
        QMessageBox::critical(this, tr("错误"), tr("学号 %1 已存在!").arg(idEdit->text()));
        return;
    }
    //插入数据
    QSqlDatabase::database().transaction();
    QSqlQuery insertQuery;
    insertQuery.prepare(
        "INSERT INTO studentInfo (id, name, gender, birthday, join_date, study_goal, progress, photo) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
        );
    //绑定参数
    insertQuery.addBindValue(idEdit->text());
    insertQuery.addBindValue(nameEdit->text());
    insertQuery.addBindValue(genderBox->currentData());
    insertQuery.addBindValue(birthdayEdit->date().toString("yyyy-MM-dd"));
    insertQuery.addBindValue(joinDateEdit->date().toString("yyyy-MM-dd"));
    insertQuery.addBindValue(goalEdit->text());
    insertQuery.addBindValue(progressCombo->currentData());
    insertQuery.addBindValue(photoData.isEmpty() ? QVariant() : photoData);

    if (!insertQuery.exec()) {
        QSqlDatabase::database().rollback();
        QMessageBox::critical(this, tr("错误"), tr("插入失败: ") + insertQuery.lastError().text());
        return;
    }
    else{
        QSqlDatabase::database().commit();
        refreshTable();
        QMessageBox::information(this, tr("成功"), tr("已成功添加学生%1").arg(nameEdit->text()));
    }
}


void StudentInfoWidget::on_btnDeleteItem_clicked()
{
    auto selected = ui->tableWidget->selectedItems();
    if(selected.isEmpty()){
        QMessageBox::warning(this,"警告","请先选择要删除的单元格");
        return;
    }

    QSqlDatabase::database().transaction();
    foreach(QTableWidgetItem * item, selected){
        int row = item->row();
        int col = item->column();
        QString id = ui->tableWidget->item(row,0)->text();

        const QStringList columes = {"id", "name", "gender", "birthday", "join_date", "study_goal", "progress", "photo"};
        QSqlQuery query;
        query.prepare(QString("UPDATE studentInfo SET %1 = ? WHERE id = ?").arg(columes[col]));

        query.addBindValue("");
        query.addBindValue(id);

        if(!query.exec()){
            QSqlDatabase::database().rollback();
            QMessageBox::critical(this,"错误","更新失败: " + query.lastError().text());
            return;
        }
    }
    QSqlDatabase::database().commit();
    refreshTable();
}


void StudentInfoWidget::on_btnDeleteLine_clicked()
{
    auto selected = ui->tableWidget->selectionModel()->selectedRows();
    if(selected.isEmpty()){
        QMessageBox::warning(this,"警告","请先选择要删除的行");
        return;
    }
    QSqlDatabase::database().transaction();
    foreach(const QModelIndex & index, selected){
        QString id = ui->tableWidget->item(index.row(),0)->text();
        QSqlQuery query;
        query.prepare("DELETE FROM studentInfo WHERE id = ?");
        query.addBindValue(id);
        if(!query.exec()){
            QSqlDatabase::database().rollback();
            QMessageBox::critical(this,"错误","删除失败"+query.lastError().text());
            return;
        }
    }
    QSqlDatabase::database().commit();
    refreshTable();
}

