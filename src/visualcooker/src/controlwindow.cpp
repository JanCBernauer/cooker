#include "controlwindow.h"

#include <iostream>



#include "TApplication.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TGSplitter.h"
#include "TCanvas.h"
#include "TError.h"
#include "TGDockableFrame.h"
#include "TColor.h"
#include "TPaveStats.h"
#include "Plugin.h"

class DockableTab:public TGTab
{

public:
  DockableTab(const TGWindow* p = 0, UInt_t w = 1, UInt_t h = 1, GContext_t norm = GetDefaultGC()(), FontStruct_t font = GetDefaultFontStruct(), UInt_t options = kChildFrame, Pixel_t back = GetDefaultFrameBackground()):TGTab(p,w,h,norm,font,options,back)
  {};
  ~DockableTab()
  {
  };
  TGCompositeFrame *AddTab(const char *name)
  {
    TGCompositeFrame *t=TGTab::AddTab(name);
    TGDockableFrame *dock=new TGDockableFrame(t);
    dock->SetFixedSize(false);
    dock->EnableHide(false);
    t->AddFrame(dock,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,0,0,0,0));
    return dock;
  }
  
};


void ControlWindow::rangeChanged()
{
  float from,to;
  rangeSlider->GetPosition(&from,&to);
  fromLabel->SetText(from);
  toLabel->SetText(to);
  
}


void ControlWindow::flipBtns(bool val)
{
  cancelBtn->SetEnabled(!val);
  stepBtn->SetEnabled(val);
  do10Btn->SetEnabled(val);
  do100Btn->SetEnabled(val);
  do1000Btn->SetEnabled(val);
  doRangeBtn->SetEnabled(val);
  doAllBtn->SetEnabled(val);
  skipBtn->SetEnabled(val);
  skip10Btn->SetEnabled(val);
  skip100Btn->SetEnabled(val);
  skip1000Btn->SetEnabled(val);
  bkskipBtn->SetEnabled(val);
  bkskip10Btn->SetEnabled(val);
  bkskip100Btn->SetEnabled(val);
  bkskip1000Btn->SetEnabled(val);

  clearBtn->SetEnabled(val);
  gotoBtn->SetEnabled(val);
}

void ControlWindow::processRange(int from, int to)

{

  flipBtns(false);
  statusBar->SetText("Processing...",2);
  char eventcnt[100];
  progressBar->SetRange(from,to);
  progressBar->Reset();
  clock_t t1=clock();
  for (int i=from;i<to ;i++)
    {
      eventnr=i;
      clock_t t2=clock();

      if ((t2-t1)*2/CLOCKS_PER_SEC)
	{
	  t1=t2;
	  progressBar->SetPosition(i+1);
	  sprintf(eventcnt,"Event %i/%i",eventnr,eventcount);
	  statusBar->SetText(eventcnt ,1);
	}
      int code=chef->processEvent(eventnr);
      if (code &Plugin::redo) i--;
      if (code &Plugin::stop)
	break;
      
      int inter = gROOT->IsInterrupted();
      if (gSystem->ProcessEvents()) break;
      if (inter) break;
    }
  eventnr++;
  progressBar->SetPosition(to);
  sprintf(eventcnt,"Event %i/%i",eventnr,eventcount);
  statusBar->SetText(eventcnt ,1);
  flipBtns(true);
  statusBar->SetText("idle",2);
  showHisto(lastHisto);

}

void ControlWindow::doEvents(int num)
{
  processRange(eventnr,eventnr+num);
}



void ControlWindow::doRange()
{
  float from,to;
  rangeSlider->GetPosition(&from,&to);
  processRange(from,to);
}

void ControlWindow::doFromHere()
{
  processRange(eventnr,eventcount);
}
void ControlWindow::skip(int num)
{
  char eventcnt[100];
  eventnr+=num;
  if (eventnr>=eventcount)       eventnr=eventcount-1;
  if (eventnr<0) eventnr=0;
  sprintf(eventcnt,"Event %i/%i",eventnr,eventcount);
  statusBar->SetText(eventcnt ,1);

}
void ControlWindow::goTo()
{
  char eventcnt[100];
  eventnr=gotoNumber->GetIntNumber();

  if (eventnr>=eventcount) eventnr=eventcount-1;
  if (eventnr<0) eventnr=0;
  sprintf(eventcnt,"Event %i/%i",eventnr,eventcount);
  statusBar->SetText(eventcnt ,1);
  doEvents(1);
}

void ControlWindow::eventlistBackward()
{
  if (eventlistposition<=0)
    return;
  eventlistposition--;
  gotoNumber->SetIntNumber(eventlist[eventlistposition]);
  goTo();
}

void ControlWindow::eventlistForward()
{
  if (eventlistposition>=int(eventlist.size())-1) // typecast is needed because position is -1 upon start!
    return;
  eventlistposition++;
  gotoNumber->SetIntNumber(eventlist[eventlistposition]);
  goTo();
}

void ControlWindow::clear()
{
  for (unsigned int i=0; i<histograms.size();i++)
    histograms[i]->Reset();
}


ControlWindow::ControlWindow(const TGWindow *p, UInt_t w, UInt_t h):TGMainFrame(p,w,h)
{

  eventnr=0;
  eventcount=1000;
  tabs=new DockableTab(this,1000,480);
  AddFrame(tabs,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,3,3,3,3));
  infoTab=tabs->AddTab("Info");
  histoTab=tabs->AddTab("Histograms");
  
  TGLayoutHints *btnhint=new TGLayoutHints(kLHintsNormal,0,2,0,0);

  TGHorizontalFrame *hframe=new TGHorizontalFrame(this,1000,40);
  stepBtn=new TGTextButton(hframe,"&Step");
  stepBtn->Connect("Clicked()","ControlWindow",this,"doEvents(=1)");

  hframe->AddFrame(stepBtn,btnhint);
  do10Btn =new TGTextButton(hframe,"+10");
  do10Btn->Connect("Clicked()","ControlWindow",this,"doEvents(=10)");
  hframe->AddFrame(do10Btn,btnhint);
  do100Btn=new TGTextButton(hframe,"+100");
  do100Btn->Connect("Clicked()","ControlWindow",this,"doEvents(=100)");
  hframe->AddFrame(do100Btn,btnhint);
  do1000Btn=new TGTextButton(hframe,"+1000");
  do1000Btn->Connect("Clicked()","ControlWindow",this,"doEvents(=1000)");
  hframe->AddFrame(do1000Btn,btnhint);


  fromLabel=new TGLabel(hframe,"0",TGLabel::GetDefaultGC()(), TGLabel::GetDefaultFontStruct(), kChildFrame | kFixedSize);
  fromLabel->SetWidth(50);
  fromLabel->SetHeight(24);
  hframe->AddFrame(fromLabel,new TGLayoutHints(kLHintsNormal,10,2,0,0));
  rangeSlider=new TGDoubleHSlider(hframe);
  rangeSlider->SetRange(0,1000000);
  hframe->AddFrame(rangeSlider,new TGLayoutHints(kLHintsExpandX,2,2,0,0));
  toLabel=new TGLabel(  hframe,"1000000",TGLabel::GetDefaultGC()(), TGLabel::GetDefaultFontStruct(), kChildFrame | kFixedSize);
  toLabel->SetWidth(50);
  toLabel->SetHeight(24);
  hframe->AddFrame(toLabel,btnhint);
  rangeSlider->Connect("PositionChanged()","ControlWindow",this,"rangeChanged()");
  rangeChanged();

  doRangeBtn=new TGTextButton(hframe,"Do Range");
  doRangeBtn->Connect("Clicked()","ControlWindow",this,"doRange()");
  hframe->AddFrame(doRangeBtn,btnhint);

  doAllBtn=new TGTextButton(hframe,"&Run");
  doAllBtn->Connect("Clicked()","ControlWindow",this,"doFromHere()");
  hframe->AddFrame(doAllBtn,btnhint);


  gotoNumber=new TGNumberEntry(hframe,0,6);
  gotoNumber->SetNumStyle(TGNumberFormat::kNESInteger);
  gotoNumber->SetNumAttr(TGNumberFormat::kNEANonNegative);
  gotoNumber->GetNumberEntry()->Connect("ReturnPressed()","ControlWindow",this,"goTo()");
  //gotoNumber->SetNumber(63); // Temp for Event Display Work
  hframe->AddFrame(gotoNumber,new TGLayoutHints(kLHintsNormal,50,0,0,0));
  

  gotoBtn=new TGTextButton(hframe,"Goto");
  gotoBtn->Connect("Clicked()","ControlWindow",this,"goTo()");
  hframe->AddFrame(gotoBtn,btnhint);
  
  // create forward and backward buttons for eventlist
  // in case this has been specified on the command line:
  elbwdBtn=new TGTextButton(hframe, "<");
  elbwdBtn->Connect("Clicked()","ControlWindow",this,"eventlistBackward()");
  elbwdBtn->SetToolTipText("browse eventlist backwards,\nspecify eventlist file on commandline with option -e", 750);
  hframe->AddFrame(elbwdBtn,btnhint);
  elfwdBtn=new TGTextButton(hframe, ">");
  elfwdBtn->Connect("Clicked()","ControlWindow",this,"eventlistForward()");
  elfwdBtn->SetToolTipText("browse eventlist forward,\nspecify eventlist file on commandline with option -e", 750);
  hframe->AddFrame(elfwdBtn,btnhint);
  eventlistposition=-1; // start at begin of eventlist

  clearBtn=new TGTextButton(hframe,"Clear");
  clearBtn->Connect("Clicked()","ControlWindow",this,"clear()");
  hframe->AddFrame(clearBtn,new TGLayoutHints(kLHintsNormal,20,0,0,0));

  AddFrame(hframe, new TGLayoutHints(kLHintsExpandX,3,3,3,3));
  hframe=new TGHorizontalFrame(this,1000,40);

  skipBtn=new TGTextButton(hframe,"Skip");
  skipBtn->Connect("Clicked()","ControlWindow",this,"skip(=1)");
  hframe->AddFrame(skipBtn,btnhint);
  skip10Btn=new TGTextButton(hframe,"Skip 10");
  skip10Btn->Connect("Clicked()","ControlWindow",this,"skip(=10)");
  hframe->AddFrame(skip10Btn,btnhint);
  skip100Btn=new TGTextButton(hframe,"Skip 100");
  skip100Btn->Connect("Clicked()","ControlWindow",this,"skip(=100)");
  hframe->AddFrame(skip100Btn,btnhint);
  skip1000Btn=new TGTextButton(hframe,"Skip 1000");
  skip1000Btn->Connect("Clicked()","ControlWindow",this,"skip(=1000)");
  hframe->AddFrame(skip1000Btn,btnhint);
  skip10KBtn=new TGTextButton(hframe,"Skip 10K");
  skip10KBtn->Connect("Clicked()","ControlWindow",this,"skip(=10000)");
  hframe->AddFrame(skip10KBtn,btnhint);
  skip100KBtn=new TGTextButton(hframe,"Skip 100K");
  skip100KBtn->Connect("Clicked()","ControlWindow",this,"skip(=100000)");
  hframe->AddFrame(skip100KBtn,btnhint);

  bkskipBtn=new TGTextButton(hframe,"&Back");
  bkskipBtn->Connect("Clicked()","ControlWindow",this,"skip(=-1)");
  hframe->AddFrame(bkskipBtn,new TGLayoutHints(kLHintsNormal,20,2,0,0));
  bkskip10Btn=new TGTextButton(hframe,"Back 10");
  bkskip10Btn->Connect("Clicked()","ControlWindow",this,"skip(=-10)");
  hframe->AddFrame(bkskip10Btn,btnhint);
  bkskip100Btn=new TGTextButton(hframe,"Back 100");
  bkskip100Btn->Connect("Clicked()","ControlWindow",this,"skip(=-100)");
  hframe->AddFrame(bkskip100Btn,btnhint);
  bkskip1000Btn=new TGTextButton(hframe,"Back 1000");
  bkskip1000Btn->Connect("Clicked()","ControlWindow",this,"skip(=-1000)");
  hframe->AddFrame(bkskip1000Btn,btnhint);
  bkskip10KBtn=new TGTextButton(hframe,"Back 10K");
  bkskip10KBtn->Connect("Clicked()","ControlWindow",this,"skip(=-10000)");
  hframe->AddFrame(bkskip10KBtn,btnhint);
  bkskip100KBtn=new TGTextButton(hframe,"Back 100K");
  bkskip100KBtn->Connect("Clicked()","ControlWindow",this,"skip(=-100000)");
  hframe->AddFrame(bkskip100KBtn,btnhint);

  AddFrame(hframe, new TGLayoutHints(kLHintsExpandX,3,3,3,3));
  hframe=new TGHorizontalFrame(this,1000,40);
  progressBar=new TGHProgressBar(hframe,TGHProgressBar::kFancy);
  progressBar->SetRange(0,1);
  progressBar->SetPosition(1);
  hframe->AddFrame(progressBar,new TGLayoutHints(kLHintsExpandX,3,3,3,3));
  cancelBtn =new TGTextButton(hframe,"Cancel");
  cancelBtn->Connect("Clicked()","TROOT",gROOT,"SetInterrupt()");
  cancelBtn->SetEnabled(false);
  hframe->AddFrame(cancelBtn,btnhint);


  AddFrame(hframe, new TGLayoutHints(kLHintsExpandX,3,3,3,3));

  statusBar=new TGStatusBar(this);
  statusBar->SetParts(3);
  statusBar->SetText("No file",0);
  statusBar->SetText("Event 0/0",1);
  statusBar->SetText("idle",2);
  AddFrame(statusBar, new TGLayoutHints(kLHintsExpandX ,3,3,3,3));
  debug=0;
}

void ControlWindow::LTshowHisto(TGListTreeItem* entry, Int_t btn)
{
  int id=((long) entry->GetUserData())-1;
  showHisto(id);
}

void ControlWindow::showHisto(int id)
{
  if (id<0) return;
  bool drawsame=false;
  if (drawoptCB->GetState()==kButtonDown)
    drawsame=true;
  lastHisto=id;
  canvas->GetCanvas()->cd();
  if (!drawsame)
    canvas->GetCanvas()->Clear();
  if (strstr(histograms[id]->ClassName(), "TH2"))
    histograms[id]->Draw("colz");
  else
    {
      if (drawsame)
	{
	  TPaveStats *st = (TPaveStats*)gPad->GetPrimitive("stats");
	  canvas->GetCanvas()->Update();
	  st->SetName(Form("%s_stats", histograms[id]->GetName()));
	  histograms[id]->Draw("sames");
	}
      else
	histograms[id]->Draw();
    };
  histograms[id]->SetLineColor(TColor::GetColor(drawCS->GetColor()));
  canvas->GetCanvas()->Update();
  TPaveStats *st = (TPaveStats*)gPad->GetPrimitive("stats");
  st->SetTextColor(TColor::GetColor(drawCS->GetColor()));
  // st->SetX1NDC(newx1); //new x start position
  // st->SetX2NDC(newx2); //new x end position
  // newhist->Draw("sames")
  canvas->GetCanvas()->Update();
}

void ControlWindow::addHisto(const char * path, TH1D* histo)
{
  histograms.push_back(histo);
  
  //split by /
  TString tsp(path);
  tsp.Remove(0,tsp.First(":")+1);
  TObjArray *dirs=tsp.Tokenize("/");
  TGListTreeItem *pos=0;
  for (int i=0;i<dirs->GetEntriesFast()-1;i++)
    {
      //traverse/make directories
      TGListTreeItem *npos=histLB->FindChildByName(pos,((TObjString *) dirs->At(i))->GetString().Data());
      if (!npos)
	{
	  npos=histLB->AddItem(pos,((TObjString *) dirs->At(i))->GetString().Data());
	}
      pos=npos;
    }
  
  
  histLB->AddItem(pos,((TObjString *) dirs->At(dirs->GetEntriesFast()-1))->GetString().Data(),histicon,histicon)->SetUserData((void *) (histograms.size()),false);
  
  delete dirs;	  

}

void ControlWindow::startup(std::string rname)
{
  //everything is set up. So let's fill in the numbers
  recipename=rname;

  //load histogram icons
  histicon = gClient->GetPicture("h1_t.xpm");


  //discover Histograms
  TGHorizontalFrame *hframe=new TGHorizontalFrame(histoTab,300,40);

  lastHisto=-1;
  


  // Create vertical splitter
  

  TGVerticalFrame *fV1 = new TGVerticalFrame(hframe, 10, 10, kFixedWidth);
  TGVerticalFrame *fV2 = new TGVerticalFrame(hframe, 10, 10);


  TGCanvas *mcanvas=new TGCanvas(fV1,10,10);
  histLB=new TGListTree(mcanvas,kHorizontalFrame);

  histLB->Connect("Clicked(TGListTreeItem*, Int_t)","ControlWindow",this,"LTshowHisto(TGListTreeItem*, Int_t)");

  TGHorizontalFrame *fH1 = new TGHorizontalFrame(fV1, 10, 10);

  drawCS = new TGColorSelect(fH1, 31);
  fH1->AddFrame(drawCS);
  drawoptCB = new TGCheckButton(fH1, "->Draw(\"same\")");
  fH1->AddFrame(drawoptCB);

  canvas=new TRootEmbeddedCanvas("Histo Canvas",fV2);
 
  fV1->AddFrame(mcanvas,new TGLayoutHints(kLHintsTop | kLHintsExpandX|kLHintsExpandY ,0,0,5,5));
  fV1->AddFrame(fH1, new TGLayoutHints(kLHintsExpandX));
  fV2->AddFrame(canvas,new TGLayoutHints(kLHintsTop | kLHintsExpandX |kLHintsExpandY,0,0,5,5));

  fV1->Resize(220, fV1->GetDefaultHeight());
  fV2->Resize(400, fV1->GetDefaultHeight()); 

  hframe->AddFrame(fV1,new TGLayoutHints(kLHintsLeft | kLHintsExpandY));
  TGVSplitter *splitter = new TGVSplitter(hframe,2,30); 
  splitter->SetFrame(fV1, kTRUE); 
  hframe->AddFrame(splitter,new TGLayoutHints(kLHintsLeft | kLHintsExpandY));
  hframe->AddFrame(fV2,new TGLayoutHints(kLHintsRight|kLHintsExpandX|kLHintsExpandY));


  histoTab->AddFrame(hframe,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,0,0,0,0));

  gErrorIgnoreLevel = 2001;
  chef->addRepo("Callbacks", (TObject *)static_cast<FrameworkCallbacks*>(this));
  gErrorIgnoreLevel = 0;

  chef->defineHistograms();
 
  TIter next(gDirectory->GetList());
  TObject* obj;
 

  while(obj= (TObject*)next())
    if(obj->InheritsFrom(TH1::Class()))
      addHisto(obj->GetName(),(TH1D *)obj);
 
 
  chef->addRepo("Tab Widget",tabs);


  chef->startup();

  eventcount=chef->in->GetEntries();  
  rangeSlider->SetRange(0,eventcount);
  statusBar->SetText("Press Step (Alt-S) or Run (Alt-R) to begin.",1);

  TGLabel *lab;

  TGLayoutHints *hinta=new TGLayoutHints(kLHintsNormal,0,2,10,0);
  TGLayoutHints *hintb=new TGLayoutHints(kLHintsNormal,0,2,0,0);

  lab=new TGLabel(  infoTab,"Recipe Name:");
  infoTab->AddFrame(lab,hintb); 
  lab=new TGLabel(  infoTab,recipename.c_str());
  infoTab->AddFrame(lab,hintb); 

  lab=new TGLabel(  infoTab,"Init XML:");
  infoTab->AddFrame(lab,hinta); 
  lab=new TGLabel(  infoTab,chef->recipe.InitXML.c_str());
  infoTab->AddFrame(lab,hintb); 

  lab=new TGLabel(  infoTab,"Data file:");
  infoTab->AddFrame(lab,hinta); 
  lab=new TGLabel(  infoTab,chef->infile->GetEndpointUrl()->GetUrl());
  infoTab->AddFrame(lab,hintb);
  chef->addRepo("Infile",lab);  // For the Event Display

  lab=new TGLabel(  infoTab,"Output file:");
  infoTab->AddFrame(lab,hinta); 
  if (chef->outfile)
    lab=new TGLabel(  infoTab,chef->outfile->GetEndpointUrl()->GetUrl());
  else
    lab=new TGLabel(  infoTab,"none");
  infoTab->AddFrame(lab,hintb); 


  statusBar->SetText(chef->infile->GetEndpointUrl()->GetUrl(),0);

  //discover histograms and add to second tab




  MapSubwindows();
  Layout();
  SetWindowName("Visual Cooker");
  SetIconName("Visual Cooker");
  Resize(GetDefaultSize());
  MapWindow();
}



ControlWindow::~ControlWindow()
{
  Cleanup();
};

void ControlWindow::CloseWindow()
{
  std::cout<<"--Finalize--"<<std::endl;
  chef->finalize();

  gApplication->Terminate(0);
}


ClassImp(ControlWindow);
