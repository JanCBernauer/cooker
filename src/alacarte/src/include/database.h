
#ifndef __DATABASE_H_
#define __DATABASE_H_

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QFile>
#include "checkpivotmodel.h"

 
class DatabaseManager : public QObject
    {
    public:
        DatabaseManager(QObject *parent = 0);
        ~DatabaseManager();
 
    public:
        bool openDB();
	bool reloadDB();
        QSqlError lastError();
	checkpivotmodel *model;

    private:
        QSqlDatabase db;
	QObject *par;

    };

#endif
