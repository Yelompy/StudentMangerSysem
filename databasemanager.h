#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>

class DataBaseManager : public QObject
{
    Q_OBJECT
public:
    static DataBaseManager& instance();
    void closeDataBase();
    bool openDataBase(const QString& path);
    QString getDataBasePath() const;
    void setDataBasePath(const QString& path);
    ~DataBaseManager();
private:
    explicit DataBaseManager(QObject *parent = nullptr);
    QSqlDatabase db;
    QString dbPath = "D:/Yelompy/Documents/c++/Qt_project/chap02/StudentManagerSystem/sqllite/axbTechManSys.db";

signals:
};

#endif // DATABASEMANAGER_H
