#include "database.h"
#include <QDir>
#include <QSqlTableModel>
#include <QTableView>
#include <QSqlQuery>
#include <QSqlRecord>


#include <iostream>
bool DatabaseManager::openDB()
    {
    // Find QSLite driver
      db = QSqlDatabase::addDatabase("QSQLITE");
 
    QString path(QDir::home().path());
    path.append(QDir::separator()).append(".cooker/shared/alacarte/rundata.db");
    path = QDir::toNativeSeparators(path);
    db.setDatabaseName(path);
 
    if (db.open())
      {
	qDebug()<<"Success opening the database\n";
      }
    if (model) delete model;
    model=new checkpivotmodel(this);
    while (model->canFetchMore()&& model->rowCount()>0)
      model->fetchMore();
    }




bool DatabaseManager::reloadDB()
{
  QSqlDatabase src=QSqlDatabase::addDatabase("QPSQL");
  src.setHostName("oweb");
  src.setDatabaseName("runinfo");
  if (!src.open("runinfo","runinfo"))
      return false;
  
  QSqlTableModel srctbl(par,src);
  srctbl.setTable("runinfo");
  srctbl.setEditStrategy(QSqlTableModel::OnManualSubmit);
  srctbl.select();

  db.exec("DROP table if exists runinfo;");
  db.exec("CREATE table runinfo(time timestamp,runid int,starttime timestamp,stoptime timestamp,runtime float8,events int,luminosity float8,deadtime float8,beam float8,flow float8,magnet float8,energy float8,sym_moeller float8,filename text);");
  db.exec("create index runinfo_index on runinfo(runid);");
  QSqlTableModel dsttbl(par,db);
  dsttbl.setTable("runinfo");
  dsttbl.setEditStrategy(QSqlTableModel::OnManualSubmit);
  dsttbl.select();
  db.transaction();

  dsttbl.insertRows(0,srctbl.rowCount());
  for (int i=0;i<srctbl.rowCount();i++) 
    {
      std::cout<<i<<"/"<<srctbl.rowCount()<<"\r";
      dsttbl.setRecord(i,srctbl.record(i));
    }  
  dsttbl.submitAll();
  
  db.commit();  
}
 
QSqlError DatabaseManager::lastError()
    {
    // If opening database has failed user can ask 
    // error description by QSqlError::text()
    return db.lastError();
    }
 

DatabaseManager::DatabaseManager(QObject *parent )
{
  par=parent;
  model=NULL;
}

DatabaseManager::~DatabaseManager()
{
  db.close();
}
