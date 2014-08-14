#include <QtGui>
#include "mainwindow.h"

#include <iostream>
#include <algorithm>

 MainWindow::MainWindow()
 {
   createActions();
   createMenus();
   createStatusBar();

   db.openDB();

   QString path(QDir::home().path());
   path.append(QDir::separator()).append(".cooker/shared/alacarte/templates.ini");
   path = QDir::toNativeSeparators(path);
   templates=new QSettings(path,  QSettings::IniFormat);
   templates->setFallbacksEnabled(false);

   tab=new QTabWidget(this);
   setCentralWidget(tab);


   // recipe selection
   QWidget *recipeTab = new QWidget;
   QVBoxLayout *recipeLayout=new QVBoxLayout;
   QToolBar *recipeTB=new QToolBar;
   recipeTB->addAction(loadRecipeAct);
   recipeTB->addSeparator();
   recipeTB->addAction(saveRecipeAct);
   recipeTB->addAction(saveAsRecipeAct);
   recipeTB->addSeparator();
     
   recipeName=new QLabel;
   recipeName->setText("");

   recipeTB->addWidget(recipeName);

   recipeLayout->addWidget(recipeTB);
   
   recipeEdit=new QPlainTextEdit;
   recipeLayout->addWidget(recipeEdit);
   connect(recipeEdit, SIGNAL(textChanged()), this, SLOT(recipeChanged()));

   recipeTab->setLayout(recipeLayout);
   tab->addTab(recipeTab,"Recipe");

   // run selection
   runTab = new QSplitter;


   QWidget *runLeft=new QWidget;
   QVBoxLayout *v=new QVBoxLayout;
   
   QGroupBox *grp;
   QGridLayout *gl;
   
   
   grp=new QGroupBox("RunID");
   gl=new QGridLayout;
   runidCB=new QCheckBox("Filter by Run ID");
   runidfrom=new QLineEdit();
   runidto=new QLineEdit();
   QLabel * l1=new QLabel("From:");
   QLabel * l2=new QLabel("To:");
   gl->addWidget(runidCB,0,0,1,4);
   gl->addWidget(l1,1,0);
   gl->addWidget(runidfrom,1,1);
   gl->addWidget(l2,1,2);  
   gl->addWidget(runidto,1,3);
   connect(runidCB, SIGNAL(stateChanged(int)), this, SLOT(updateFilter()));
   connect(runidfrom, SIGNAL(editingFinished()), this, SLOT(updateFilter()));
   connect(runidto, SIGNAL(editingFinished()), this, SLOT(updateFilter()));

   grp->setLayout(gl);
   v->addWidget(grp);

   grp=new QGroupBox("Type");
   gl=new QGridLayout;
   epRB=new QRadioButton("Electrons - Positive");
   ppRB=new QRadioButton("Positrons - Positive");
   enRB=new QRadioButton("Electrons - Negative");
   pnRB=new QRadioButton("Positrons - Negative");
   allRB=new QRadioButton("All");
   gl->addWidget(epRB,1,0);
   gl->addWidget(ppRB,2,0);
   gl->addWidget(enRB,1,1);
   gl->addWidget(pnRB,2,1);
   gl->addWidget(allRB,0,0);

   connect(epRB, SIGNAL(toggled(bool)), this, SLOT(updateFilter()));
   connect(ppRB, SIGNAL(toggled(bool)), this, SLOT(updateFilter()));
   connect(enRB, SIGNAL(toggled(bool)), this, SLOT(updateFilter()));
   connect(pnRB, SIGNAL(toggled(bool)), this, SLOT(updateFilter()));
   connect(allRB, SIGNAL(toggled(bool)), this, SLOT(updateFilter()));

   grp->setLayout(gl);
   v->addWidget(grp);

   grp=new QGroupBox("Columns");
   gl=new QGridLayout;
   
   colselect=new QComboBox();
   QSqlQueryModel tagnames;
   tagnames.setQuery("select tagid,tagname from tagnames order by tagname");
   for (int i=0;i<tagnames.rowCount();i++)
     {
     colselect->addItem(tagnames.record(i).value("tagname").toString(),tagnames.record(i).value("tagid").toInt());
     tagtoid[tagnames.record(i).value("tagid").toInt()]=i;
     }
   coladd=new QPushButton("+");
   colrm=new QPushButton("-");
   colrm->setEnabled(false);
   
   connect (coladd, SIGNAL(clicked(bool)), this, SLOT(addchannel()));
   connect (colrm, SIGNAL(clicked(bool)), this, SLOT(rmchannel()));
   connect (colselect,SIGNAL(currentIndexChanged(int)),this,SLOT(updatechannel()));

   gl->addWidget(colselect,0,0,1,4);
   gl->addWidget(coladd,0,4);
   gl->addWidget(colrm,0,5);


   fNo=new QRadioButton("No filter");
   fisok=new QRadioButton("Is OK");
   fVal=new QRadioButton("Equal to");
   fRng=new QRadioButton("In range");

   fNo->setEnabled(false);
   fVal->setEnabled(false);
   fRng->setEnabled(false);
   fisok->setEnabled(false);
   
   connect(fNo,SIGNAL(toggled(bool)), this, SLOT(updateChannelFilter()));
   connect(fVal,SIGNAL(toggled(bool)), this, SLOT(updateChannelFilter()));
   connect(fRng,SIGNAL(toggled(bool)), this, SLOT(updateChannelFilter()));
   connect(fisok,SIGNAL(toggled(bool)), this, SLOT(updateChannelFilter()));

   gl->addWidget(fNo,1,0,1,2);
   gl->addWidget(fVal,1,2,1,2);
   gl->addWidget(fRng,1,4,1,2);
   gl->addWidget(fisok,1,6,1,2);

   ffrom=new QLineEdit();
   fto=new QLineEdit();
   ffrom->setEnabled(false);
   fto->setEnabled(false);
   gl->addWidget(ffrom,2,0,1,4);
   gl->addWidget(fto,2,4,1,4);
   connect(ffrom, SIGNAL(editingFinished()), this, SLOT(updateChannelFilter()));
   connect(fto, SIGNAL(editingFinished()), this, SLOT(updateChannelFilter()));


   grp->setLayout(gl);
   v->addWidget(grp);

   grp=new QGroupBox("Selection");
   gl=new QGridLayout;

   QPushButton *selall=new QPushButton("Select all");
   QPushButton *selclear=new QPushButton("Clear selection");
   QPushButton *selexport=new QPushButton("Export");

   gl->addWidget(selall,0,0);
   gl->addWidget(selclear,0,1);
   gl->addWidget(selexport,0,2);

   connect(selall, SIGNAL(clicked(bool)), this, SLOT(set()));
   connect(selclear, SIGNAL(clicked(bool)), this, SLOT(clear()));
   connect(selexport, SIGNAL(clicked(bool)), this, SLOT(exporttable()));

   grp->setLayout(gl);
   v->addWidget(grp);


   grp=new QGroupBox("Load/Save Configuration");
   gl=new QGridLayout;

   tempCB=new QComboBox();
   tempSave=new QPushButton("Save");
   tempDelete=new QPushButton("Remove");

   gl->addWidget(tempCB,0,0,1,2);
   gl->addWidget(tempSave,1,0);
   gl->addWidget(tempDelete,1,1);

   tempCB->setEditable(true);
   loadTemplateNames();

   connect(tempCB, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadTemplate(QString)));
   connect(tempSave, SIGNAL(clicked(bool)), this, SLOT(writeTemplate()));
   connect(tempDelete, SIGNAL(clicked(bool)), this, SLOT(deleteTemplate()));

   grp->setLayout(gl);
   v->addWidget(grp);


   v->addStretch();

   runLeft->setLayout(v);
   
   runView=new QTableView(this); 
   runView->setModel(db.model);
   runView->setSortingEnabled(false);
   runView->setCornerButtonEnabled(false);
   runTab->addWidget(runLeft);

   runTab->addWidget(runView);
   connect (runView->horizontalHeader(),SIGNAL(sectionClicked(int)),this, SLOT(channelchange(int)));

   tab->addTab(runTab,"Runs");


   // source/dest selection
   QWidget *sourceTab=new QWidget;
   v=new QVBoxLayout;
      
   
   grp=new QGroupBox("Source");
   gl=new QGridLayout;
   srcocRB=new QRadioButton("From /data/TESTS on ocompile");
   srcdcRB=new QRadioButton("From DESY dcache");
   srcldRB=new QRadioButton("From local directory:");
   srcldLE=new QLineEdit();
   QPushButton *srcldSB=new QPushButton("Select");

   srcdcRB->setEnabled(false);
   gl->addWidget(srcocRB,0,0);
   gl->addWidget(srcdcRB,1,0);
   gl->addWidget(srcldRB,2,0);
   gl->addWidget(srcldLE,2,1);  
   gl->addWidget(srcldSB,2,2);

   connect(srcldSB,SIGNAL(clicked(bool)),this,SLOT(setLD()));

   grp->setLayout(gl);
   v->addWidget(grp);

   grp=new QGroupBox("Destination");
   gl=new QGridLayout;
   destLE=new QLineEdit();
   QLabel *destLB=new QLabel("Destination Dir:");
   QPushButton *destSB=new QPushButton("Select");
   QLabel *prefixLB=new QLabel("File Prefix:");
   prefixLE=new QLineEdit();

   gl->addWidget(destLB,0,0);
   gl->addWidget(destLE,0,1);
   gl->addWidget(destSB,0,2);
   gl->addWidget(prefixLB,1,0);
   gl->addWidget(prefixLE,1,1);

   grp->setLayout(gl);
   v->addWidget(grp);
   connect(destSB,SIGNAL(clicked(bool)),this,SLOT(setDD()));

   grp=new QGroupBox("Post process");
   gl=new QGridLayout;
   combhistoCB=new QCheckBox("Combine Histograms to root file:");
   combhistoLE=new QLineEdit();
   QPushButton *combhistoSB=new QPushButton("Select");

   gnuplotCB=new QCheckBox("Output histogram gnuplot file:");
   gnuplotLE=new QLineEdit();
   QPushButton * gnuplotSB=new QPushButton("Select");


   gl->addWidget(combhistoCB,0,0);
   gl->addWidget(combhistoLE,0,1);
   gl->addWidget(combhistoSB,0,2);

   gl->addWidget(gnuplotCB,1,0);
   gl->addWidget(gnuplotLE,1,1);
   gl->addWidget(gnuplotSB,1,2);

   connect(combhistoSB,SIGNAL(clicked(bool)),this,SLOT(setCH()));
   connect(gnuplotSB,SIGNAL(clicked(bool)),this,SLOT(setGP()));
   grp->setLayout(gl);
   v->addWidget(grp);

   v->addStretch();
   
   sourceTab->setLayout(v);
   tab->addTab(sourceTab,"Source/Destination");

   // export/run selection
   QWidget *exportTab=new QWidget;
   v=new QVBoxLayout;
   QPushButton *exportlistPB =new QPushButton("Export selected run list to text file");
   QPushButton *exportmakePB =new QPushButton("Export to Makefile");

   v->addWidget(exportlistPB);
   v->addWidget(exportmakePB);

   connect(exportlistPB,SIGNAL(clicked(bool)),this,SLOT(exportList()));
   connect(exportmakePB,SIGNAL(clicked(bool)),this,SLOT(exportMake()));
   

   v->addStretch();

   exportTab->setLayout(v);
   tab->addTab(exportTab,"Execute");

   readSettings();

   setUnifiedTitleAndToolBarOnMac(true);
 }


void MainWindow::set()
{
 db.model->set();
}

void MainWindow::clear()
{
 db.model->clear();
}


void MainWindow::exporttable()
{
  QString f=QFileDialog::getSaveFileName(this,"Data file to export to","","Data files (*.dat)");
  if( !f.isNull())
    {
      QFile out(f);
      if (out.open(QIODevice::WriteOnly | QIODevice::Text))
	{
	  QTextStream os(&out);
	  for (int i=0;i<db.model->rowCount();i++)
	    if (db.model->data(db.model->index(i,0),Qt::CheckStateRole).toBool())
	      {
		for (int j=0;j<db.model->columnCount();j++)
		  os<<db.model->index(i,j).data().toString()<<"\t";
		os<<"\n";
	      }
	}
      out.close();
}

}

void MainWindow::channelchange(int i)
{

  if (i<2) return;
  i-=2;
  if ((unsigned int) i<db.model->infocolumns.size())
      colselect->setCurrentIndex(tagtoid[db.model->infocolumns[i].tagid]);
  else
    {
      i-=db.model->infocolumns.size();
      colselect->setCurrentIndex(tagtoid[db.model->detectorcolumns[i].tagid]);
    }
  updatechannel();
}

void MainWindow::addchannel()
{
  int tagid=colselect->itemData(colselect->currentIndex()).toInt();
  QSqlQueryModel tagtype;
  tagtype.setQuery(QString("select count(runid) as count from detectorflag where tagid=%1").arg(tagid));

  if (tagtype.record(0).value("count").toInt()>0)
    db.model->detectorcolumns.push_back(sqlfilter(tagid));
  else
    db.model->infocolumns.push_back(sqlfilter(tagid));

  updatechannel();
}

void MainWindow::rmchannel()
{
  int tagid=colselect->itemData(colselect->currentIndex()).toInt();
  db.model->detectorcolumns.erase(std::remove(db.model->detectorcolumns.begin(),db.model->detectorcolumns.end(),sqlfilter(tagid)),db.model->detectorcolumns.end());
  db.model->infocolumns.erase(std::remove(db.model->infocolumns.begin(),db.model->infocolumns.end(),sqlfilter(tagid)),db.model->infocolumns.end());
  updatechannel();
}


void MainWindow::updateChannelFilter()
{
  int tagid=colselect->itemData(colselect->currentIndex()).toInt();
  std::vector<sqlfilter>::iterator it= std::find(db.model->detectorcolumns.begin(),db.model->detectorcolumns.end(),sqlfilter(tagid));
  if (it==db.model->detectorcolumns.end())
    it=std::find(db.model->infocolumns.begin(),db.model->infocolumns.end(),sqlfilter(tagid));

  if (fNo->isChecked())
    {
      it->condition=sqlfilter::dontcare;
      ffrom->setEnabled(false);
      fto->setEnabled(false);
    }
  if (fisok->isChecked())
    {
      it->condition=sqlfilter::isok;
      ffrom->setEnabled(false);
      fto->setEnabled(false);
    }
  if (fVal->isChecked())
    {
      it->condition=sqlfilter::equal;
      ffrom->setEnabled(true);
      fto->setEnabled(false);
      it->iarg=ffrom->text().toInt();
    }
  if (fRng->isChecked())
    {
      it->condition=sqlfilter::inrange;
      ffrom->setEnabled(true);
      fto->setEnabled(true);
      it->dfrom=ffrom->text().toDouble();
      it->dto=fto->text().toDouble();

    }
  statusBar()->showMessage(tr("Processing..."));
  db.model->buildQuery();
  statusBar()->showMessage(tr("Done"),1000);
}

void MainWindow::updatechannel()
{
  int tagid=colselect->itemData(colselect->currentIndex()).toInt();
  bool isin=((std::count(db.model->detectorcolumns.begin(),db.model->detectorcolumns.end(),sqlfilter(tagid))+
	      std::count(db.model->infocolumns.begin(),db.model->infocolumns.end(),sqlfilter(tagid)))>0);
  coladd->setEnabled(!isin);
  colrm->setEnabled(isin);
  
  fNo->setEnabled(isin);
  fisok->setEnabled(isin);
  fVal->setEnabled(isin);
  fRng->setEnabled(isin);
  

  if (isin)
    {
      std::vector<sqlfilter>::iterator it= std::find(db.model->detectorcolumns.begin(),db.model->detectorcolumns.end(),sqlfilter(tagid));
      if (it==db.model->detectorcolumns.end())
	it=std::find(db.model->infocolumns.begin(),db.model->infocolumns.end(),sqlfilter(tagid));
      
      if (it->condition==sqlfilter::dontcare)
	{ 
	  fNo->setChecked(true);
	  fisok->setChecked(false);
	  fVal->setChecked(false);
	  fRng->setChecked(false);
	  fto->setEnabled(false);
	  ffrom->setEnabled(false);
	}
      if (it->condition==sqlfilter::equal)
	{
	  fNo->setChecked(false);
	  fisok->setChecked(false);
	  fVal->setChecked(true);
	  fRng->setChecked(false);
	  ffrom->setEnabled(true);
	  fto->setEnabled(false);
	  ffrom->setText(QString("%1").arg(it->iarg));
	}
      if (it->condition==sqlfilter::inrange)
	{
	  fNo->setChecked(false);
	  fisok->setChecked(false);
	  fVal->setChecked(false);
	  fRng->setChecked(true);
	  fto->setEnabled(true);
	  ffrom->setEnabled(true);
	  ffrom->setText(QString("%1").arg(it->dfrom));
	  fto->setText(QString("%1").arg(it->dto));
	}
      if (it->condition==sqlfilter::isok)
	{
	  fNo->setChecked(false);
	  fisok->setChecked(true);
	  fVal->setChecked(false);
	  fRng->setChecked(false);
	  fto->setEnabled(false);
	  ffrom->setEnabled(false);
	}


    }
  else
    {
      fto->setEnabled(false);
      ffrom->setEnabled(false);
    }
  statusBar()->showMessage(tr("Processing..."));
  db.model->buildQuery();
  statusBar()->showMessage(tr("Done"),1000);
}


void MainWindow::updateFilter()
{

  QString filter;
  if (runidCB->isChecked())
    {
      filter+=" AND df.runid>="+runidfrom->text()+" AND df.runid<="+runidto->text();
    }

  if (epRB->isChecked())
    filter+=" AND df.value=1";
  if (ppRB->isChecked())
    filter+=" AND df.value=2";
  if (enRB->isChecked())
    filter+=" AND df.value=3";
  if (pnRB->isChecked())
    filter+=" AND df.value=4";

  db.model->addFilter=filter;
  statusBar()->showMessage(tr("Processing..."));
  db.model->buildQuery();
  statusBar()->showMessage(tr("Done"),1000);
}

void MainWindow::recipeChanged()
{
  saveRecipeAct->setEnabled(true);
}

void MainWindow::loadRecipeHelper(QString filename)
{
  if (filename.isEmpty()) return;
  QFile file(filename);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("Application"),
			 tr("Cannot read file %1:\n%2.")
			 .arg(filename)
			 .arg(file.errorString()));
    return;
  }
  
  QTextStream in(&file);
  recipeEdit->setPlainText(in.readAll());
  saveRecipeAct->setEnabled(false);
  statusBar()->showMessage(tr("File loaded"), 2000);
  recipeName->setText(filename);
}


void MainWindow::loadRecipe()
{
  QString f=QFileDialog::getOpenFileName(this, tr("Open Recipe"),QString::null,"Recipe files (*.xml)");
  if (!f.isEmpty())
    {
      loadRecipeHelper(f);
    }
}

void MainWindow::saveRecipe()
{
  QString fileName=recipeName->text();
  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("Application"),
			 tr("Cannot write file %1:\n%2.")
			 .arg(fileName)
			 .arg(file.errorString()));
    return;
  }
  
  QTextStream out(&file);
  out << recipeEdit->toPlainText();
  statusBar()->showMessage(tr("File saved"), 2000);
  saveRecipeAct->setEnabled(false);
  return;
}

void MainWindow::saveAsRecipe()
{

  QString f=QFileDialog::getSaveFileName(this,tr("Save recipe as"),recipeName->text(),"Recipe files (*.xml)");
  if (!f.isNull())
    {
      recipeName->setText(f);
      saveRecipe();
    }
}


 void MainWindow::closeEvent(QCloseEvent *event)
 {
   writeSettings();
   event->accept();
 }

 void MainWindow::about()
 {
    QMessageBox::about(this, tr("About Application"),
             tr("A la carte -  A program to choose your menu."));
 }


 void MainWindow::createActions()
 {

   loadRecipeAct = new QAction(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton),tr("&Load Recipe"), this);
   loadRecipeAct->setStatusTip(tr("Load Recipe from file"));
   connect(loadRecipeAct, SIGNAL(triggered()), this, SLOT(loadRecipe()));

   saveRecipeAct = new QAction(QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton),tr("&Save Recipe"), this);
   saveRecipeAct->setStatusTip(tr("Save Recipe to file"));
   saveRecipeAct->setEnabled(false);
   connect(saveRecipeAct, SIGNAL(triggered()), this, SLOT(saveRecipe()));

   saveAsRecipeAct = new QAction(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton),tr("Save&As Recipe"), this);
   saveAsRecipeAct->setStatusTip(tr("Save Recipe to file with filename"));
   connect(saveAsRecipeAct, SIGNAL(triggered()), this, SLOT(saveAsRecipe()));   


   aboutAct = new QAction(tr("&About"), this);
   aboutAct->setStatusTip(tr("Show the application's About box"));
   connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
   
   aboutQtAct = new QAction(tr("About &Qt"), this);
   aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
   connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
 }

 void MainWindow::createMenus()
 {
     dbMenu = menuBar()->addMenu(tr("&DB"));   
     helpMenu = menuBar()->addMenu(tr("&Help"));
     helpMenu->addAction(aboutAct);
     helpMenu->addAction(aboutQtAct);
 }

void MainWindow::createStatusBar()
 {
     statusBar()->showMessage(tr("Ready"));
 }



void MainWindow::setLD()
{
  srcldLE->setText(QFileDialog::getExistingDirectory(this,"Source Directory for runs"));
}

void MainWindow::setDD()
{
  destLE->setText(QFileDialog::getExistingDirectory(this,"Destination Directory"));
}
void MainWindow::setCH()
{
  combhistoLE->setText(QFileDialog::getSaveFileName(this,"Root file to export to","","Root files (*.root)"));
}
void MainWindow::setGP()
{
  gnuplotLE->setText(QFileDialog::getSaveFileName(this,"Gnuplot data file to export to","","Gnuplot data files (*.dat)"));
}


void MainWindow::exportList()
{
  QString filename=QFileDialog::getSaveFileName(this,"Text file to export to","","Text files (*.txt)");
  if (filename!="")
    {
      QFile out(filename);
      if (out.open(QIODevice::WriteOnly | QIODevice::Text))
	{
	  QTextStream os(&out);
	  for (int i=0;i<db.model->rowCount();i++)
	    if (db.model->data(db.model->index(i,0),Qt::CheckStateRole).toBool())
	  	os<<db.model->index(i,0).data().toInt()<<"\n";
	}
      out.close();
    }
}

void MainWindow::exportMake()
{
  QString filename=QFileDialog::getSaveFileName(this,"Makefile to export to","","Makefiles (Makefile*)");
  QString all="all:\t";
  QString allroot="";
  if (filename!="")
    {
      QFile out(filename);
      if (out.open(QIODevice::WriteOnly | QIODevice::Text))
	{
	  QTextStream os(&out);

	  // all 
	  for (int i=0;i<db.model->rowCount();i++)
	    if (db.model->data(db.model->index(i,0),Qt::CheckStateRole).toBool())
	      {
	  	allroot+=destLE->text()+"/"+prefixLE->text()+db.model->index(i,0).data().toString()+".root ";
	  	std::cout<<i<<endl;
	      }

	  all+=allroot;
	  if (combhistoCB->isChecked())
	      all+=combhistoLE->text()+" ";

	  if (gnuplotCB->isChecked())
	      all+=gnuplotLE->text();
	  
	  os<<all<<"\n.PHONY:\tall\n";
	  // individual run files
	  os<<destLE->text()+"/"+prefixLE->text()+"%.root:\t"+recipeName->text()+"\n";
	  os<<"\tcooker $^ ";
	  if (srcocRB->isChecked())
	    os<<"/data/tests/ROOTfiles/";
	  if (srcldRB->isChecked())
	    os<<srcldLE->text()+"/";
	  os<<"run_$*_trigger*.root $@\n\n"  ;
	    
	  if (combhistoCB->isChecked())
	    {
	      os<<combhistoLE->text()+":\t"+allroot+"\n";
	      os<<"\tcombine -R $@ $^\n";
	    }

	  if (gnuplotCB->isChecked())
	    {
	      os<<gnuplotLE->text()+":\t"+allroot+"\n";
	      os<<"\tcombine -G $@ $^\n";
	    }
	
  

	}
      out.close();
    }
}

 void MainWindow::readSettings()
 {
   QSettings settings;
   QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
   QSize size = settings.value("size", QSize(400, 400)).toSize();
   resize(size);
   move(pos);
   loadRecipeHelper(settings.value("recipename","").toString());
   runTab->restoreState(settings.value("runsplitter").toByteArray());

   srcocRB->setChecked(settings.value("srcocchecked",true).toBool());
   srcdcRB->setChecked(settings.value("srcdcchecked",false).toBool());
   srcldRB->setChecked(settings.value("srcldchecked",false).toBool());

   srcldLE->setText(settings.value("srcldtext","").toString());
   destLE->setText(settings.value("desttext","").toString());
   prefixLE->setText(settings.value("prefixtext","").toString());
   combhistoLE->setText(settings.value("combhistotext","").toString());
   gnuplotLE->setText(settings.value("gnuplottext","").toString());
   
   combhistoCB->setChecked(settings.value("combhistochecked",false).toBool());
   gnuplotCB->setChecked(settings.value("gnuplotchecked",false).toBool());

 }


 void MainWindow::writeSettings()
 {
   QSettings settings;
   settings.setValue("pos", pos());
   settings.setValue("size", size());
   settings.setValue("recipename",recipeName->text());
   settings.setValue("runsplitter",runTab->saveState());

   settings.setValue("srcocchecked",srcocRB->isChecked());
   settings.setValue("srcdcchecked",srcdcRB->isChecked());
   settings.setValue("srcldchecked",srcldRB->isChecked());

   settings.setValue("srcldtext",srcldLE->text());
   settings.setValue("desttext",destLE->text());
   settings.setValue("prefixtext",prefixLE->text());
   settings.setValue("combhistotext",combhistoLE->text());
   settings.setValue("gnuplottext",gnuplotLE->text());

   settings.setValue("combhistochecked",combhistoCB->isChecked());
   settings.setValue("gnuplotchecked",gnuplotCB->isChecked());


 }



void MainWindow::loadTemplateNames()
{

  QStringList names=templates->childGroups();
  QString oldindex=tempCB->currentText();
  tempCB->clear();
  for (int i=0;i<names.size();i++)
    {
    tempCB->addItem(names[i]);
    if (names[i]==oldindex) tempCB->setCurrentIndex(i);
    }
}

void MainWindow::loadTemplate(QString n)
{
  templates->beginGroup(n);
  if (templates->value("valid",0)==1)
    {
      runidfrom->setText(templates->value("runidfrom",0).toString());
      runidto->setText(templates->value("runidto",10000).toString());
      runidCB->setChecked(templates->value("runidchecked",false).toBool());
      
      epRB->setChecked(templates->value("epchecked",false).toBool());
      ppRB->setChecked(templates->value("ppchecked",false).toBool());
      enRB->setChecked(templates->value("enchecked",false).toBool());
      pnRB->setChecked(templates->value("pnchecked",false).toBool());
      allRB->setChecked(templates->value("allchecked",true).toBool());
     
      db.model->infocolumns.clear();
      db.model->detectorcolumns.clear();
      int size=templates->beginReadArray("infos");
      for (int i=0;i<size;i++)
	{
	  templates->setArrayIndex(i);
	  sqlfilter d;
	  d.condition=(sqlfilter::conditiontype) templates->value("condition").toInt();
	  d.iarg=templates->value("iarg").toInt();
	  d.dfrom=templates->value("dfrom").toDouble();
	  d.dto=templates->value("dto").toDouble();
	  d.name=templates->value("name").toString();
	  d.tagid=templates->value("tagid").toInt();
	  db.model->infocolumns.push_back(d);
	}
      templates->endArray();
      size=templates->beginReadArray("dfs");
      for (int i=0;i<size;i++)
	{
	  templates->setArrayIndex(i);
	  sqlfilter d;
	  d.condition=(sqlfilter::conditiontype) templates->value("condition").toInt();
	  d.iarg=templates->value("iarg").toInt();
	  d.dfrom=templates->value("dfrom").toDouble();
	  d.dto=templates->value("dto").toDouble();
	  d.name=templates->value("name").toString();
	  d.tagid=templates->value("tagid").toInt();
	  db.model->detectorcolumns.push_back(d);
	}
      templates->endArray();

      updateFilter();     
      updateChannelFilter();
    }
  templates->endGroup();
}

void MainWindow::writeTemplate()
{
  templates->beginGroup(tempCB->currentText());
  templates->setValue("valid",1);
  templates->setValue("runidchecked",runidCB->isChecked());
  templates->setValue("epchecked",epRB->isChecked());
  templates->setValue("ppchecked",ppRB->isChecked());
  templates->setValue("enchecked",enRB->isChecked());
  templates->setValue("pnchecked",pnRB->isChecked());
  templates->setValue("allchecked",allRB->isChecked());
  templates->setValue("runidfrom",runidfrom->text());
  templates->setValue("runidto",runidto->text());

  templates->beginWriteArray("infos");
  for (unsigned int i=0;i<db.model->infocolumns.size();i++)
    {
      templates->setArrayIndex(i);
      templates->setValue("condition",(int) db.model->infocolumns[i].condition);
      templates->setValue("iarg",db.model->infocolumns[i].iarg);
      templates->setValue("dfrom",db.model->infocolumns[i].dfrom);
      templates->setValue("dto",db.model->infocolumns[i].dto);
      templates->setValue("name",db.model->infocolumns[i].name);
      templates->setValue("tagid",db.model->infocolumns[i].tagid);
    }
  templates->endArray();
  templates->beginWriteArray("dfs");
  for (unsigned int i=0;i<db.model->detectorcolumns.size();i++)
    {
      templates->setArrayIndex(i);
      templates->setValue("condition",(int) db.model->detectorcolumns[i].condition);
      templates->setValue("iarg",db.model->detectorcolumns[i].iarg);
      templates->setValue("dfrom",db.model->detectorcolumns[i].dfrom);
      templates->setValue("dto",db.model->detectorcolumns[i].dto);
      templates->setValue("name",db.model->detectorcolumns[i].name);
      templates->setValue("tagid",db.model->detectorcolumns[i].tagid);
    }
  templates->endArray();

  templates->endGroup();
  loadTemplateNames();
  updateFilter();

}

void MainWindow::deleteTemplate()
{

  templates->remove(tempCB->currentText());
  loadTemplateNames();
  updateFilter();
}

