#include "honorwall.h"
#include "ui_honorwall.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QScrollArea>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QFileDialog>
#include <QBuffer>
#include <QDate>

HonorWall::HonorWall(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HonorWall)
{
    ui->setupUi(this);
    setupUI();
    loadImagesFromDatabase();
}

HonorWall::~HonorWall()
{
    delete ui;
}

void HonorWall::setupUI()
{
    //主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    //按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    //添加按钮
    addButton =new QPushButton("添加图片", this);
    deleteButton = new QPushButton("删除图片", this);
    modifyButton = new QPushButton("修改图片", this);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(modifyButton);
    //连接
    connect(addButton, &QPushButton::clicked, this, &HonorWall::addImage);
    connect(deleteButton, &QPushButton::clicked, this, &HonorWall::deleteImage);
    connect(modifyButton, &QPushButton::clicked, this ,&HonorWall::modifyImage);

    mainLayout->addLayout(buttonLayout);

    //滚动区域
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    //内容区域
    contentWidget = new QWidget(scrollArea);
    gridLayout = new QGridLayout(contentWidget);
    contentWidget->setLayout(gridLayout);
    //设置滚动区域内容
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);

    setLayout(mainLayout);

}

void HonorWall::addImage()
{
    QString imagePath = QFileDialog::getOpenFileName(this, "选择图片", "", "图片文件 (*.png *.jpg *.jpeg *.bmp)");
    if(!imagePath.isEmpty())    addImageToWall(imagePath);
}

void HonorWall::deleteImage()
{
    if(!selectedLabel){
        QMessageBox::warning(this, "错误", "请先选择一张照片!");
        return;
    }
    if(QMessageBox::question(this, "确认删除", "确定要删除这张照片吗？") != QMessageBox::Yes)  return;
    int id = selectedLabel->property("id").toInt();
    QSqlQuery query;
    query.prepare("DELETE FROM honorWall WHERE id = :id");
    query.bindValue(":id", id);
    if(!query.exec()){
        qWarning() << "删除失败： " << query.lastError().text();
        return;
    }
    //从界面删除文件
    gridLayout->removeWidget(selectedLabel);
    delete selectedLabel;
    selectedLabel = nullptr;
    reorderImages();
}

void HonorWall::modifyImage()
{
    if(!selectedLabel){
        QMessageBox::warning(this, "错误", "请先选择一张照片");
        return;
    }
    //打开文件对话框选择新文件
    QString imagePath = QFileDialog::getOpenFileName(this, "选择图片", "", "图片文件 (*.png *.jpg *.jpeg *.bmp)");
    if(imagePath.isEmpty()) return;
    QPixmap pixmap(imagePath);
    if(pixmap.isNull()){
        QMessageBox::warning(this, "错误", "无法加载图片!");
        return;
    }
    //转换为二进制
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");

    int id = selectedLabel->property("id").toInt();
    //更新数据库
    QSqlQuery query;
    query.prepare("UPDATE honorWall SET image_data = :image_data WHERE id = :id");
    query.bindValue(":image_data", imageData);
    query.bindValue(":id", id);
    if(!query.exec()){
        qWarning() << "更新数据失败: " << query.lastError().text();
        return;
    }
    QPixmap scaledPixmap = pixmap.scaled(imgW, imgH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    selectedLabel->setPixmap(scaledPixmap);
}

void HonorWall::loadImagesFromDatabase()
{
    QLayoutItem* item;
    while((item = gridLayout->takeAt(0)) != nullptr){
        if(item->widget())  item->widget()->setParent(nullptr);
        delete item;//删除布局项
    }
    //从数据库中加载照片
    QSqlQuery query("SELECT id, image_data FROM honorWall");
    while(query.next()){
        int id = query.value(0).toInt();
        QByteArray imageData = query.value(1).toByteArray();

        QPixmap pixmap;
        pixmap.loadFromData(imageData);
        if(!pixmap.isNull()){
            //显示图片到界面
            ClickableLabel* imageLabel = new ClickableLabel(contentWidget);
            QPixmap scaledPixmap = pixmap.scaled(imgW, imgH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            imageLabel->setPixmap(scaledPixmap);
            imageLabel->setAlignment(Qt::AlignCenter);
            imageLabel->setStyleSheet("border 1px solid #ccc; padding 5px;");
            imageLabel->setProperty("id", id);//设置id属性

            connect(imageLabel, &ClickableLabel::clicked, this, &HonorWall::onImageClicked);

            //动态添加到网格布局
            int row = gridLayout->count() / 3;
            int col = gridLayout->count() % 3;
            gridLayout->addWidget(imageLabel, row, col);
        }
        else{
            qWarning() << "无法加载图片数据!";
        }
    }
}

void HonorWall::addImageToWall(const QString &imagePath)
{
    QPixmap pixmap(imagePath);
    if(pixmap.isNull()){
        QMessageBox::warning(this, "错误", "无法加载图片!");
        return;
    }
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    //将图片插入信息库
    QSqlQuery query;
    query.prepare("INSERT INTO honorWall (image_data, description, added_data)"
                  "VALUES(:image_data, :description, :added_data)");
    query.bindValue(":image_data", imageData);
    query.bindValue(":description", "未填写描述");
    query.bindValue(":added_data", QDate::currentDate().toString());
    if(!query.exec()){
        qWarning() << "插入数据失败:" << query.lastError().text();
        return;
    }
    addImageToUI(pixmap);
}

void HonorWall::addImageToUI(const QPixmap &pixmap)
{
    if(pixmap.isNull()){
        qWarning() << "图片无效!";
        return;
    }
    QPixmap scaledPixmap = pixmap.scaled(imgW, imgH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //创建可点击的QLabel存图片
    ClickableLabel* imageLabel = new ClickableLabel(contentWidget);
    if(!imageLabel){
        qWarning() << "无法创建QLabel!";
        return;
    }
    imageLabel->setPixmap(scaledPixmap);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet("border 1px solid #ccc; padding 5px;");
    connect(imageLabel, &ClickableLabel::clicked, this, &HonorWall::onImageClicked);
    int row = gridLayout->count() / 3;
    int col = gridLayout->count() % 3;
    gridLayout->addWidget(imageLabel, row, col);
}

void HonorWall::onImageClicked()
{
    if(selectedLabel){
        selectedLabel->setStyleSheet("border: 1px solid #ccc; padding 5px;");
    }
    selectedLabel = qobject_cast<ClickableLabel*>(sender());
    if(selectedLabel){
        selectedLabel->setStyleSheet("border: 2px solid #ccc; padding 5px;");
    }
}

void HonorWall::reorderImages()
{
    QLayoutItem* item;
    while((item = gridLayout->takeAt(0)) != nullptr){
        if(item->widget())  item->widget()->setParent(nullptr);
        delete item;//删除布局项
    }
    loadImagesFromDatabase();
}
