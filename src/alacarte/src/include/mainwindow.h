#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "database.h"

class QAction;
class QMenu;
class QTabWidget;
class QWidget;
class QPlainTextEdit;
class QLabel;
class QTableView;
class QSplitter;
class QCheckBox;
class QLineEdit;
class QDateTimeEdit;
class QRadioButton;
class QComboBox;
class QPushButton;

 class MainWindow : public QMainWindow
 {
     Q_OBJECT

 public:
     MainWindow();

 protected:
     void closeEvent(QCloseEvent *event);

 private slots:
     void about();
     void loadRecipe();
     void saveRecipe();
     void saveAsRecipe();
     void recipeChanged();
     void updateFilter();
     void set();
     void clear();
     void exporttable();

     void setLD();
     void setDD();
     void setCH();
     void setGP();

     void addchannel();
     void rmchannel();
     void updatechannel();
     void updateChannelFilter();
     void channelchange(int i);
     void exportList();
     void exportMake();

     void loadTemplateNames();
     void loadTemplate(QString n);
     void writeTemplate();
     void deleteTemplate();

 private:
     DatabaseManager db;
     QTabWidget *tab;

     std::map<int,int> tagtoid;

     QPlainTextEdit *recipeEdit;
     QLabel *recipeName;
     QTableView *runView;

     QSettings *templates;

     void createActions();
     void createMenus();
     void createStatusBar();
     void readSettings();    

     void writeSettings();
     void loadRecipeHelper(QString filename);
     QMenu *dbMenu;
     QMenu *helpMenu;
     QSplitter *runTab;

     QCheckBox *runidCB,*startdateCB;
     QLineEdit *runidfrom,*runidto;
     QRadioButton *epRB,*ppRB,*enRB,*pnRB,*allRB;
     QRadioButton *srcocRB,*srcdcRB,*srcldRB;
     QRadioButton *fNo,*fVal,*fRng,*fisok;
     QLineEdit  *ffrom;
     QLineEdit  *fto;

 
     QLineEdit *srcldLE, *destLE,*prefixLE,*combhistoLE,*gnuplotLE;
     QCheckBox *combhistoCB,*gnuplotCB;
     
     QComboBox *tempCB;
     QPushButton *tempSave;
     QPushButton *tempDelete;
     QComboBox *colselect;
     QPushButton *coladd,*colrm;

     QAction *loadRecipeAct;
     QAction *saveRecipeAct;
     QAction *saveAsRecipeAct;

     QAction *aboutAct;
     QAction *aboutQtAct;
 };

 #endif
