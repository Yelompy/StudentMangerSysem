#ifndef HONORWALL_H
#define HONORWALL_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class HonorWall;
}
class QPushButton;
class QScrollArea;
class QGridLayout;

constexpr int imgH = 500;
constexpr int imgW = 300;

class ClickableLabel : public QLabel{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget* parent = nullptr) : QLabel(parent) {}
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent* event) override{
        emit clicked();
        QLabel::mousePressEvent(event);
    }
};

class HonorWall : public QWidget
{
    Q_OBJECT

public:
    explicit HonorWall(QWidget *parent = nullptr);
    ~HonorWall();

private:
    void setupUI();
    void addImage();
    void deleteImage();
    void modifyImage();
    void loadImagesFromDatabase();
    void addImageToWall(const QString& imagePath);
    void addImageToUI(const QPixmap& pixmap);
    void onImageClicked();
    void reorderImages();
    QPushButton* addButton;
    QPushButton* modifyButton;
    QPushButton* deleteButton;
    QScrollArea* scrollArea;
    QWidget* contentWidget;
    QGridLayout* gridLayout;
    ClickableLabel* selectedLabel = nullptr;
    Ui::HonorWall *ui;
};

#endif // HONORWALL_H
