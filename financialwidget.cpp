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

FinancialWidget::FinancialWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FinancialWidget)
{
    ui->setupUi(this);
    setupUI();
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

}
