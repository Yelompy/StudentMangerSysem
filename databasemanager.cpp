#include "databasemanager.h"
#include <QDebug>
#include <QSqlError>

DataBaseManager &DataBaseManager::instance()
{
    static DataBaseManager instance;
    return instance;
}

void DataBaseManager::closeDataBase()
{
    if(db.isOpen()){
        db.close();
    }
}

bool DataBaseManager::openDataBase(const QString &path)
{
    db.setDatabaseName(path);
    if(!db.open()){
        qDebug() << "无法打开数据库: " << db.lastError().text();
        return false;
    }
    return true;
}

QString DataBaseManager::getDataBasePath() const
{
    return dbPath;
}

void DataBaseManager::setDataBasePath(const QString &path)
{
    if(path != dbPath){
        dbPath = path;
        closeDataBase();
        openDataBase(path);
    }
}

DataBaseManager::~DataBaseManager()
{
    closeDataBase();
}

DataBaseManager::DataBaseManager(QObject *parent)
    : QObject{parent}
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    openDataBase(dbPath);
}
