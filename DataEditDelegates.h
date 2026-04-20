#ifndef DATAEDITDELEGATES_H
#define DATAEDITDELEGATES_H
#include <QStyledItemDelegate>
#include <QDateEdit>
class DataEditDelegate :public QStyledItemDelegate{
    explicit DataEditDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent){}
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QDateEdit* editor = new QDateEdit(parent);
        editor->setDisplayFormat("yyyy-MM-dd");//设置日期格式
        editor->setCalendarPopup(true);//设置日历弹出
        return editor;
    }
    void setModelData(QWidget* editor, const QAbstractItemModel* model, const QModelIndex& index) const override
    {
        QDateEdit* dateEdit = qobject_cast<QDateEdit*>(editor);
        if(dateEdit){
            model->setData(index, dateEdit->date().toString("yyyy-MM-dd"),Qt::EditRole);
        }
    }
};

#endif // DATAEDITDELEGATES_H
