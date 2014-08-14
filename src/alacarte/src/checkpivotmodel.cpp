#include "checkpivotmodel.h"
#include <QColor>
#include <QBrush>




QString sqlfilter::clause(QString name)
{
  QString cond=QString(" and %1.tagid=%2").arg(name).arg(tagid);

  if (condition==sqlfilter::equal)
    return cond+QString(" and %1.value=%2 ").arg(name).arg(iarg);
  if(condition==sqlfilter::inrange)
    return cond+ QString(" and %1.value>=%2 and %1.value<=%3").arg(name).arg(dfrom).arg(dto);
  if(condition==sqlfilter::isok)
    return cond+ QString(" and %1.value & 1").arg(name);

  return cond;
}

sqlfilter::sqlfilter(int t):condition(dontcare),dfrom(0),dto(1),iarg(0),tagid(t)
{
  QSqlQueryModel tagnames;
  tagnames.setQuery(QString("select tagname from tagnames where tagid=%1").arg(t));
  name=tagnames.record(0).value("tagname").toString();
}

bool sqlfilter::operator==(const sqlfilter &other) const
{
  return other.tagid==tagid;
}

checkpivotmodel::checkpivotmodel(  QObject * parent )
  : QSqlQueryModel(parent)
{
  primarykey=0;
  infocolumns.clear();
  detectorcolumns.clear();
  addFilter="";
  buildQuery();
}





Qt::ItemFlags checkpivotmodel::flags(const QModelIndex &index) const
{
    return ( (QSqlQueryModel::flags(index) | Qt::ItemIsUserCheckable ) & ~Qt::ItemIsEditable );
}


void checkpivotmodel::buildQuery()
{
  
   QString query,from,where;
   query="select df.runid,df.value";
   from=" from detectorFlag as df";
   where=" where df.tagid=128";
   int colid=2;
   
   for (unsigned int i=0;i<infocolumns.size();i++)
   {
     query+=QString(",rd%1.value").arg(i);
     from+=QString(" join rundata as rd%1 on df.runid=rd%1.runid ").arg(i);
     where+=infocolumns[i].clause(QString("rd%1").arg(i));
   }

  

   for (unsigned int i=0;i<detectorcolumns.size();i++)
   {
     query+=QString(",df%1.value").arg(i);
     from+=QString(" join detectorFlag as df%1 on df.runid=df%1.runid ").arg(i);
     where+=detectorcolumns[i].clause(QString("df%1").arg(i));
   }


   QString q=query+from+where+addFilter+" group by df.runid order by df.runid";
   setQuery(q);
   while (canFetchMore()) fetchMore();
   setHeaderData(0, Qt::Horizontal, tr("Run ID"));
   setHeaderData(1, Qt::Horizontal, tr("Type"));
   for (unsigned int i=0;i<infocolumns.size();i++)
   {

     setHeaderData(colid,Qt::Horizontal,infocolumns[i].name ); 
     colid++;
  }
   firstdetectorcolumn=colid;

   for (unsigned int i=0;i<detectorcolumns.size();i++)
   {
     setHeaderData(colid,Qt::Horizontal,detectorcolumns[i].name);
     colid++;
  }

}


int checkpivotmodel::key( const int row ) const
{
    return index( row, primarykey ).data().toInt();
}

double checkpivotmodel::dbl(const int row, const int col) const
{
    return index( row, col ).data().toDouble();
}

int checkpivotmodel::I(const int row, const int col) const
{
    return index( row, col ).data().toInt();
}



QVariant checkpivotmodel::data(const QModelIndex& index, int role) const
{
  if( role == Qt::CheckStateRole && index.column() == primarykey )
    return checkedids.contains( key(index.row()))?Qt::Checked:Qt::Unchecked;
  
  if (role==Qt::DisplayRole)
    {
      if (index.column()>=firstdetectorcolumn)
	return QString("0x%1").arg( (int) QSqlQueryModel::data(index, role).toInt(),8,16,QChar('0'));
    }
  
  if (role== Qt::ForegroundRole)
    {
      if (index.column()>=firstdetectorcolumn)
	if (( QSqlQueryModel::data(index, Qt::DisplayRole).toInt() &0x1)>0)
	    return QBrush(Qt::darkGreen);
	  else
	    return QBrush(Qt::red);
    }

   
  if (role == Qt::BackgroundRole)
    {
      QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
      
      double mag=dbl(index.row(),9);
      double spec=dbl(index.row(),10);
      int low=210;
      if (checkedids.contains(key(index.row())))
  	  low=160;
      gradient.setSpread(QGradient::RepeatSpread);
      gradient.setCoordinateMode(QGradient::StretchToDeviceMode	);
      switch (I(index.row(),1))
	{
	case 1:
	  gradient.setColorAt(0,QColor::fromRgb(low,low,255));
  	  gradient.setColorAt(0.4,QColor::fromRgb(low,low,255));
	  gradient.setColorAt(0.6,QColor::fromRgb(255,low,low));
	  gradient.setColorAt(1,QColor::fromRgb(255,low,low));
	  break;
	case 2:
	  gradient.setColorAt(0,QColor::fromRgb(low,low,255));
  	  gradient.setColorAt(0.4,QColor::fromRgb(low,low,255));
	  gradient.setColorAt(0.6,QColor::fromRgb(255,255,low));
	  gradient.setColorAt(1,QColor::fromRgb(255,255,low));
	  break;
	case 3:	
	  gradient.setColorAt(0,QColor::fromRgb(low,255,low));
      	  gradient.setColorAt(0.4,QColor::fromRgb(low,255,low));
	  gradient.setColorAt(0.6,QColor::fromRgb(255,low,low));
	  gradient.setColorAt(1,QColor::fromRgb(255,low,low));
	  break;
	case 4:
	  gradient.setColorAt(0,QColor::fromRgb(low,255,low));
      	  gradient.setColorAt(0.4,QColor::fromRgb(low,255,low));
	  gradient.setColorAt(0.6,QColor::fromRgb(255,255,low));
	  gradient.setColorAt(1,QColor::fromRgb(255,255,low));
	  break;
	default: 
      	  gradient.setColorAt(0,Qt::white);
      	  gradient.setColorAt(1,Qt::white);
  	  break;
	}
      return QBrush(gradient);
    }


				
  return QSqlQueryModel::data(index, role);
}


bool checkpivotmodel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if( role == Qt::CheckStateRole && index.column() == primarykey )
    {
      if ( value == Qt::Checked )
	checkedids.insert(key(index.row()));
      else
	checkedids.remove(key(index.row()));
      emit dataChanged( this->index(index.row(),0), this->index(index.row(),this->rowCount() ));
      return true;
    }
  return QSqlQueryModel::setData( index, value, role );
}



bool checkpivotmodel::removeRows ( int row, int count, const QModelIndex & parent )
{
  int i;
  
  beginRemoveRows( parent, row, row+count-1 );
  
  for ( i = row; i < row+count ; i++ )
    {
      if(checkedids.contains(key(i)))
	{
	  checkedids.erase( checkedids.find( key(i)));
	  emit dataChanged( this->index(i, 0, parent), this->index(i, 0, parent) );
	}
      }
	  
   endRemoveRows();
	
    return QSqlQueryModel::removeRows( row, count, parent );
}



void checkpivotmodel::clear()
{
    checkedids.clear();
    emit dataChanged( this->index(0, 0), this->index(this->rowCount()-1, 0) );
}

void checkpivotmodel::set()
{
  checkedids.clear();
  for (int i=0;i<this->rowCount();i++)
    {
      checkedids.insert(key(i));
      
    }
    emit dataChanged( this->index(0, 0), this->index(this->rowCount()-1, 0) );
 
}

