#ifndef __CHECKABLESQLMODEL_H_
#define __CHECKABLESQLMODEL_H_

#include <QFile>
#include <QtSql>
#include <vector>


class sqlfilter
{
 public:
  enum conditiontype {dontcare,equal,inrange,isok};
  conditiontype condition;
  int iarg;
  double dfrom,dto;
  QString name;
  int tagid;
  QString clause(QString name);

  sqlfilter(int t);
  sqlfilter(){};
  bool operator==(const sqlfilter &other) const;

};

class checkpivotmodel : public QSqlQueryModel
{

public:
    checkpivotmodel(  QObject * parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::DisplayRole);
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    void buildQuery();


    void clear();
    void set();
    QSet<int> checkedids;
    QString addFilter;
    std::vector<sqlfilter> infocolumns;
    std::vector<sqlfilter> detectorcolumns;

protected:
    int key( const int row ) const;
    double dbl( const int row,const int col ) const;
    int I( const int row,const int col ) const;

private:
    int primarykey;
    int firstdetectorcolumn;
};

#endif
