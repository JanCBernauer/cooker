#ifndef __CONTROLWINDOW_H_
#define __CONTROLWINDOW_H_

#include "TGClient.h"
#include "TGButton.h"
#include "TGStatusBar.h"
#include "TGTab.h"
#include "TGNumberEntry.h"
#include "TGDoubleSlider.h"
#include "TGLabel.h"
#include "TRootEmbeddedCanvas.h"
#include "TGListTree.h"
#include "TGProgressBar.h"
#include "chef.h"
#include "TH1.h"
#include "TGColorSelect.h"

#include <string>
#include <vector>

class ControlWindow: public TGMainFrame,public FrameworkCallbacks{
 private:
  TGTab *tabs;
  TGStatusBar *statusBar;
  TGCompositeFrame *infoTab, *histoTab;
  TGTextButton *stepBtn,*do10Btn,*do100Btn,*do1000Btn,*doRangeBtn,*doAllBtn,*clearBtn,*gotoBtn,*cancelBtn;
  TGTextButton *skipBtn, *skip10Btn, *skip100Btn, *skip1000Btn, *skip10KBtn, *skip100KBtn,*bkskipBtn, *bkskip10Btn, *bkskip100Btn, *bkskip1000Btn, *bkskip10KBtn, *bkskip100KBtn;
  TGHProgressBar *progressBar;
  TGNumberEntry *gotoNumber;
  TGDoubleHSlider *rangeSlider;
  TGListTree *histLB;
  TGCheckButton *drawoptCB;
  TGColorSelect *drawCS;
  TGLabel *fromLabel,*toLabel;
  TRootEmbeddedCanvas  *canvas;
  std::string recipename;
  std::vector<TH1*> histograms;
  int lastHisto;
  const TGPicture *histicon;

 public:
  virtual void addHisto(const char *path,TH1D *h);
  int debug;
  Chef *chef;
  int eventnr,eventcount;
  std::vector<int> eventlist;
  int eventlistposition;
  TGTextButton *elfwdBtn,*elbwdBtn;
  ControlWindow(const TGWindow *p, UInt_t w, UInt_t h);
  ~ControlWindow();
  // make it die at the end
  virtual void CloseWindow();
  void rangeChanged();
  void doEvents(int nr);
  void doRange();
  void doFromHere();
  void skip(int num);
  void goTo();
  void eventlistForward();
  void eventlistBackward();
  void clear();
  void processRange(int from, int to);
  void flipBtns(bool val);
  void startup(std::string rname);
  void LTshowHisto(TGListTreeItem* entry, Int_t btn);
  void showHisto(int id);
  ClassDef(ControlWindow,1);
};
#endif
