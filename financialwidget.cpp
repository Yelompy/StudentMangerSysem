#include "financialwidget.h"
#include "ui_financialwidget.h"
#include <QBoxLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QErrorMessage>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLabel>
#include <QDate>
#include <QChartView>
#include <QDateEdit>
#include <QPushButton>
#include <QSqlQuery>
#include <QHeaderView>
#include <QFormLayout>
#include <QLineEdit>
#include <QSqlError>
#include <QPieSeries>
#include <QPieSlice>
#include <QBrush>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>

FinancialWidget::FinancialWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FinancialWidget)
{
    ui->setupUi(this);
    setupUI();
    populateStudentComboBox();
    loadFinancialRecords();
}

FinancialWidget::~FinancialWidget()
{
    delete ui;
}

void FinancialWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* topLayout = new QHBoxLayout();
    QHBoxLayout* middleLayout = new QHBoxLayout();
    chartView = new QChartView();
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(middleLayout, 60);//占60%高度
    mainLayout->addWidget(chartView, 40);//占40%高度
    //顶部筛选条件与按钮布局
    topLayout->addWidget(new QLabel("学生姓名: ", this));
    studentComboBox = new QComboBox(this);
    topLayout->addWidget(studentComboBox);

    topLayout->addWidget(new QLabel("起始日期: ", this));
    startDateEdit = new QDateEdit(QDate::currentDate().addMonths(-1));
    startDateEdit->setCalendarPopup(true);
    topLayout->addWidget(startDateEdit);

    topLayout->addWidget(new QLabel("终止日期: ", this));
    endDateEdit = new QDateEdit(QDate::currentDate());
    endDateEdit->setCalendarPopup(true);
    topLayout->addWidget(endDateEdit);

    addButton = new QPushButton("添加");
    deleteButton = new QPushButton("删除");
    editButton = new QPushButton("修改");
    topLayout->addWidget(addButton);
    topLayout->addWidget(deleteButton);
    topLayout->addWidget(editButton);
    topLayout->addStretch();
    //主内容布局
    tableWidget = new QTableWidget();
    tableWidget->setFixedWidth(550);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(true);
    QStringList header = QStringList() << "ID" << "学生姓名" << "缴费日期" << "金额" << "支付类型" << "备注";
    tableWidget->setColumnCount(header.count());
    tableWidget->setHorizontalHeaderLabels(header);
    tableWidget->setColumnHidden(0, true);
    middleLayout->addWidget(tableWidget);
    pieChartView = new QChartView();
    middleLayout->addWidget(pieChartView);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(200);
    //连接
    connect(addButton, &QPushButton::clicked, this, &FinancialWidget::addRecord);
    connect(editButton, &QPushButton::clicked, this, &FinancialWidget::editRecord);
    connect(deleteButton, &QPushButton::clicked, this, &FinancialWidget::deleteRecord);
    connect(startDateEdit, &QDateEdit::dateChanged, this, &FinancialWidget::loadFinancialRecords);
    connect(endDateEdit, &QDateEdit::dateChanged, this, &FinancialWidget::loadFinancialRecords);

}

void FinancialWidget::loadFinancialRecords()
{
    tableWidget->setRowCount(0);

    QString studentId = studentComboBox->currentData().toString();
    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();

    QString queryStr = QString(
        "SELECT fr.id, s.name, fr.payment_date, fr.amount, fr.payment_type, fr.notes "
        "FROM financialRecords fr "
        "JOIN studentInfo s ON fr.student_id = s.id "
        "WHERE fr.payment_date BETWEEN '%1' AND '%2' %3"
        ).arg(startDate.toString("yyyy-MM-dd"),
            endDate.toString("yyyy-MM-dd"),
        (studentId != "-1") ? QString("AND fr.student_id = '%1'").arg(studentId) : "");
    QSqlQuery query(queryStr);

    while(query.next()){
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        for(int col = 0; col < 6; ++col){
            QTableWidgetItem* item = new QTableWidgetItem(query.value(col).toString());
            item->setTextAlignment(Qt::AlignCenter);
            tableWidget->setItem(row, col, item);
        }
    }

    tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    updateChart();    //更新折线图
    updatePieChart(); //更新饼图
}
//添加财务记录
void FinancialWidget::populateStudentComboBox()
{
    studentComboBox->clear();
    studentComboBox->addItem("所有学生", QVariant("-1"));//"-1"代表所有学生

    QSqlQuery query("SELECT id, name FROM studentInfo");
    while(query.next()){
        QString id = query.value(0).toString();
        QString name = query.value(1).toString();
        studentComboBox->addItem(name, QVariant(id));
    }
}
//添加按钮功能
void FinancialWidget::addRecord()
{
    QDialog dialog(this);
    dialog.setWindowTitle("添加缴费记录");
    QFormLayout form(&dialog);
    //学生名称下拉菜单
    QComboBox* studentNameCombobox = new QComboBox(&dialog);
    QSqlQuery query("SELECT id, name FROM studentInfo");
    while(query.next()){
        QString id = query.value(0).toString();
        QString name = query.value(1).toString();
        studentNameCombobox->addItem(name, QVariant(id));   //将学生id与姓名关联
    }
    QDateEdit* paymentDateEdit = new QDateEdit(&dialog);
    paymentDateEdit->setDate(QDate::currentDate());
    paymentDateEdit->setCalendarPopup(true);

    QLineEdit* amountEdit = new QLineEdit(&dialog);
    QLineEdit* feeTypeEdit = new QLineEdit(&dialog);
    QLineEdit* remarkEdit = new QLineEdit(&dialog);

    form.addRow("学生名称: ", studentNameCombobox);
    form.addRow("缴费日期: ", paymentDateEdit);
    form.addRow("金额: ", amountEdit);
    form.addRow("支付类型: ", feeTypeEdit);
    form.addRow("备注: ", remarkEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    buttonBox.button(QDialogButtonBox::Ok)->setText("确定");
    buttonBox.button(QDialogButtonBox::Cancel)->setText("取消");
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if(dialog.exec() == QDialog::Accepted){
        QString studentId = studentNameCombobox->currentData().toString();
        QString paymentDate = paymentDateEdit->date().toString("yyyy-MM-dd");
        double amount = amountEdit->text().toDouble();
        QString feeType = feeTypeEdit->text();
        QString remark = remarkEdit->text();
        //准备SQL查询
        QSqlQuery query;
        query.prepare("INSERT INTO financialRecords (student_id, payment_date, amount, payment_type, notes) "
                      "VALUES(:student_id, :payment_date, :amount, :payment_type, :notes)");
        query.bindValue(":student_id", studentId);
        query.bindValue(":payment_date", paymentDate);
        query.bindValue(":amount", amount);
        query.bindValue(":payment_type", feeType);
        query.bindValue(":notes", remark);
        //执行SQL语句
        if(query.exec()){
            qDebug() << "记录添加成功! ";
            loadFinancialRecords();//刷新表格
        }
        else qDebug() << "记录添加失败: " << query.lastError().text();
    }

}
//绘制饼图
void FinancialWidget::updatePieChart()
{
    QString studentId = studentComboBox->currentData().toString();
    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();

    QString queryStr = QString(
        "SELECT payment_type, SUM(amount) "
        "FROM financialRecords "
        "WHERE payment_date BETWEEN '%1' AND '%2' %3 "
        "GROUP BY payment_type")
        .arg(startDate.toString("yyyy-MM-dd"))
        .arg(endDate.toString("yyyy-MM-dd"))
        .arg(studentId != "-1" ? QString("AND student_id = '%1'").arg(studentId) : "");

    QSqlQuery query(queryStr);

    QPieSeries* series = new QPieSeries();
    qreal totalAmount = 0;
    while(query.next()){
        QString type = query.value(0).toString();
        qreal value = query.value(1).toDouble();

        totalAmount += value;

        if(value > 0){
            QString LegendLabel = QString("%1 %2元").arg(type).arg(value);
            QPieSlice* slice = new QPieSlice(LegendLabel, value);
            slice->setLabelVisible(false);
            series->append(slice);
        }
    }

    QChart* chart =new QChart();
    chart->addSeries(series);
    chart->setTitle("支付类型分布");

    //图例设置
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setBackgroundVisible(true);
    chart->legend()->setBrush(QBrush(Qt::white));
    chart->legend()->setLabelColor(Qt::black);
    chart->legend()->setContentsMargins(10, 10, 10, 10);

    //饼图尺寸
    series->setPieSize(0.75);

    if(pieChartView->chart()){
        delete pieChartView->chart();
    }
    pieChartView->setChart(chart);
    //强制重绘
    pieChartView->repaint();
}
//绘制折线图
void FinancialWidget::updateChart()
{//获取并验证日期范围
    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();
    if(startDate > endDate){
        std::swap(startDate, endDate);
        startDateEdit->setDate(startDate);
        endDateEdit->setDate(endDate);
    }
    //构建安全sql查询
    QString studentId = studentComboBox->currentData().toString();
    QString queryStr = QString(
        "SELECT DATE(payment_date) AS day, SUM(amount) AS total "
        "FROM financialRecords "
        "WHERE payment_date BETWEEN :startDate AND :endDate "
        "%1 GROUP BY day ORDER BY day")
        .arg(studentId != "-1" ? "AND student_id = :studentId" : "");
    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":startDate", startDate.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", endDate.toString("yyyy-MM-dd"));
    if(studentId != "-1")   query.bindValue(":studentId", studentId);
    if(!query.exec())   qCritical() << "[SQL错误]" << query.lastError().text();
    //处理查询数据
    QMap<QDate, qreal> dayData;
    qreal maxAmount = 0;
    while(query.next()){
        QDate day = QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");
        if(!day.isValid())  continue;
        qreal amount = query.value(1).toDouble();
        dayData[day] = amount;
        if(amount > maxAmount)  maxAmount = amount;
    }
    //创建图表系列
    QLineSeries* series = new QLineSeries();
    series->setName("销售额");
    QPen pen(Qt::blue);
    series->setPen(pen);
    QDate currentDate = startDate;
    while(currentDate <= endDate){
        qreal value = dayData.value(currentDate, 0.0);
        series->append(currentDate.startOfDay().toMSecsSinceEpoch(), value);
        currentDate = currentDate.addDays(1);
    }
    //配置坐标轴
    QChart* chart = new QChart();
    chart->addSeries(series);
    QDateTimeAxis* axiX = new QDateTimeAxis();  //x轴
    axiX->setFormat("yyyy-MM-dd");
    axiX->setTitleText("日期");
    axiX->setRange(startDate.startOfDay(), endDate.startOfDay());
    chart->addAxis(axiX, Qt::AlignBottom);
    series->attachAxis(axiX);

    QValueAxis* axiY = new QValueAxis();
    axiY->setTitleText("金额(元)");
    axiY->setLabelFormat("%.0f");
    axiY->setRange(0, std::ceil(maxAmount / 10) * 10 + 10);//上扩十元余量
    chart->addAxis(axiY, Qt::AlignLeft);
    series->attachAxis(axiY);
    //应用图表
    if(chartView->chart())  delete chartView->chart();
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chart->legend()->setVisible(false);
}
//用户修改财务信息
void FinancialWidget::editRecord()
{
    int currentRow = tableWidget->currentRow();
    if(currentRow < 0){
        QMessageBox::warning(this, "警告", "请选择要修改的记录! ");
        return;
    }
    //获取当前行数据
    QString id = tableWidget->item(currentRow, 0)->text();
    QString studentName = tableWidget->item(currentRow, 1)->text();
    QString paymentDate = tableWidget->item(currentRow, 2)->text();
    QString amount = tableWidget->item(currentRow, 3)->text();
    QString feeType = tableWidget->item(currentRow, 4)->text();
    QString remark = tableWidget->item(currentRow, 5)->text();
    QDialog dialog(this);
    dialog.setWindowTitle("修改缴费记录");
    QFormLayout form(&dialog);
    //学生名称下拉菜单
    QComboBox* studentNameComboBox = new QComboBox(&dialog);
    QSqlQuery query("SELECT id, name FROM studentInfo");
    while(query.next()){
        QString id = query.value(0).toString();
        QString name = query.value(1).toString();
        studentNameComboBox->addItem(name, QVariant(id));
    }
    studentNameComboBox->setCurrentText(studentName);//设置当前学生名称
    QLineEdit* paymentDateEdit = new QLineEdit(paymentDate, &dialog);
    QLineEdit* amountEdit = new QLineEdit(amount, &dialog);
    QLineEdit* feeTypeEdit = new QLineEdit(feeType, &dialog);
    QLineEdit* remarkEdit = new QLineEdit(remark, &dialog);
    form.addRow("学生名称: ", studentNameComboBox);
    form.addRow("缴费日期: ", paymentDateEdit);
    form.addRow("金额: ", amountEdit);
    form.addRow("支付类型: ", feeTypeEdit);
    form.addRow("备注: ", remarkEdit);
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    buttonBox.button(QDialogButtonBox::Ok)->setText("确定");
    buttonBox.button(QDialogButtonBox::Cancel)->setText("取消");
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if(dialog.exec() == QDialog::Accepted){
        QString studentId = studentNameComboBox->currentData().toString();  //studentId是字符串类型
        QString paymentDate = paymentDateEdit->text();
        double amount = amountEdit->text().toDouble();
        QString feeType = feeTypeEdit->text();
        QString remark = remarkEdit->text();
        //准备SQL查询
        QSqlQuery query;
        query.prepare("UPDATE financialRecords SET student_id = :student_id, payment_date = :payment_date, "
                      "amount = :amount, payment_type = :payment_type, notes = :notes WHERE id = :id");
        query.bindValue(":student_id", studentId);
        query.bindValue(":payment_date", paymentDate);
        query.bindValue(":amount", amount);
        query.bindValue(":payment_type", feeType);
        query.bindValue(":notes", remark);
        query.bindValue(":id", id);
        //执行SQL查询
        if(query.exec()){
            qDebug() << "记录修改成功!";
            loadFinancialRecords();
        }
        else    qDebug() << "修改记录失败: " << query.lastError().text();
    }

}

void FinancialWidget::deleteRecord()
{
    int currentRow = tableWidget->currentRow();
    if(currentRow < 0){
        QMessageBox::warning(this, "警告", "请选择要删除的记录!");
        return ;
    }
    //获取ID列的值
    int id = tableWidget->item(currentRow, 0)->text().toInt();
    //确认删除操作
    QMessageBox confirmBox(this);
    confirmBox.setWindowTitle("确认删除");
    confirmBox.setText("确定要删除该记录吗?");
    //设置按钮为中文
    QPushButton* yesButton = confirmBox.addButton("确定", QMessageBox::YesRole);
    QPushButton* noButton = confirmBox.addButton("取消", QMessageBox::NoRole);
    //设置默认按钮
    confirmBox.setDefaultButton(noButton);

    //显示对话框并等待用户选择
    confirmBox.exec();

    if(confirmBox.clickedButton() == yesButton){
        //用户点击了确定按钮
        QSqlQuery query;
        query.prepare("DELETE FROM financialRecords WHERE id = :id");
        query.bindValue(":id", id);

        if(query.exec()){
            qDebug() << "记录删除成功!";
            loadFinancialRecords();
        }
        else{
            qDebug() << "删除记录失败: " << query.lastError().text();
            QMessageBox::warning(this, "错误", "记录删除失败: " + query.lastError().text());
        }
    }
}
