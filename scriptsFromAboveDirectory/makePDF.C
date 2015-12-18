
#include <stdlib.h>
#include <iostream>
#ifndef __CINT__
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TPDF.h"
#include "TH2S.h"
#include "TH1D.h"
#include "TF1.h"
#include "TObject.h"
#include "TLine.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#endif 

void setpad(TVirtualPad *pad,float left, float right, float top, float bottom, int logy = 0, int logz = 0);
TLegend* myLeg(Double_t xlow, Double_t ylow, Double_t xup, Double_t yup,Int_t textFont=42,Double_t textSize=0.05);
void setcolz();
void setstyle();

void makePDF(const char* FileName="test")
{

  gROOT->Reset();
  setcolz();
  setstyle();   
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1);
  gStyle->SetOptTitle(1);
  TGaxis::SetMaxDigits(3);

  char name[100];
  sprintf(name, "%s.root", FileName);
  TFile *f = new TFile(name);

  TCanvas *c1 = new TCanvas("c1", "c1",0,0,800,800);
  //setpad(c1,0.001,0.001,0.001,0.001, 0, 0);
  setpad(c1,0.1,0.1,0.05,0.12, 0, 0);
  TPad *pad = new TPad("pad","",0.00,0.00,1.00,1.00);
  pad->Draw();

  TIter nextkey(f->GetListOfKeys());
  TObject *t;

  sprintf(name, "%s.pdf", FileName);
  TPDF *pdf = new TPDF(name);
  //Set front page
  c1->cd();
  //pad->cd();
  //setpad(gPad,0.1,0.1,0.05,0.12, 0, 0);
  TBox *bLabel = new TBox(0.01, 0.88, 0.99, 0.99);
  bLabel->SetFillColor(kBlack);
  bLabel->Draw();
  TLatex tl;
  tl.SetNDC();
  tl.SetTextColor(kWhite);
  tl.SetTextSize(0.063);
  char tlName[100];
  char tlName2[100];

  TString titlename = FileName;
  int found = titlename.Last('/');
  if(found >= 0){
    titlename.Replace(0, found+1, "");
  } 
  sprintf(tlName, "RUN 14 AuAu 200 GeV");
  sprintf(tlName2,"Purity Analysis");
  tl.SetTextSize(0.075);
  tl.SetTextColor(kBlack);
  tl.DrawLatex(0.1, 0.8,tlName);
  tl.DrawLatex(0.1, 0.7,tlName2);

  TBox *bFoot = new TBox(0.01, 0.01, 0.99, 0.12);
  bFoot->SetFillColor(kBlack);
  bFoot->Draw();
  tl.SetTextColor(kWhite);
  tl.SetTextSize(0.05);
  tl.DrawLatex(0.05, 0.05, (new TDatime())->AsString());
  tl.SetTextColor(kBlack);
  tl.SetTextSize(0.04);
  tl.DrawLatex(0.1, 0.14, titlename);
  sprintf(tlName,"\#eID: -1 < n#sigma_{e TPC} < 3, #left|gDCA#right| < 1 cm");
  tl.DrawLatex(0.1, 0.6,tlName);
  sprintf(tlName,"\#nHitsFit > 20, nHits#frac{dE}{dx} > 15, nHitFit/Max > 0.52");
  tl.DrawLatex(0.1, 0.53,tlName);
  sprintf(tlName,"\#n#phi > 1, n#eta > 1");
  tl.DrawLatex(0.1, 0.46,tlName);
  sprintf(tlName,"\#Event: #left|V_{z}#right| < 35 cm, #left|V_{z}-V_{z-VPD}#right| < 6 cm");
  tl.DrawLatex(0.1,0.38,tlName);

  c1->cd();
  c1->Update();

  c1->cd();
  int nCounts = 0;
  int nFig = 0;
  TKey *key;
  TLine line;
  line.SetLineStyle(7);
  line.SetLineWidth(1);
  TString tName;
  tl.SetTextSize(0.035);

  //Loop through TH1 and TH2 histos and place them on PDF
  while ((key = (TKey*)nextkey())) {
    t = dynamic_cast<TObject*>(key->ReadObj());
    if(t){
      //cout<<"t->ClassName() : "<<t->ClassName()<<" t->GetName() : "<<t->GetName()<<endl;
      if(!strcmp(t->ClassName(),"TH2D")||!strcmp(t->ClassName(),"TH2F")||!strcmp(t->ClassName(),"TH2S")){
        TH2F *h2 = (TH2F*)t;
        tName = t->GetName();
        //if(tName.Contains("adcChan")){
        //cout<<"tname = "<<tName.Data()<<" nCounts = "<<nCounts<<" nFig = "<<nFig<<endl;
        //h2->SetAxisRange(1,1200,0"X");
        //h2->SetAxisRange(0,4096,"Y");
        //pad->cd(nCounts+1);
        //setpad(gPad,0.1,0.2,0.1,0.15, 0, 1);
        //if(tName.Contains("mtddeltaT_channel")){
        // t->RebinY(5);
        // }
        gPad->SetLogy(0);
        gPad->SetLogz();
        h2->Draw("colz");
        nCounts++;
        nFig++;
        c1->cd();
        c1->Update();
        //}
      }
      if(!strcmp(t->ClassName(),"TH1D")||!strcmp(t->ClassName(),"TH1F")||!strcmp(t->ClassName(),"TH1S")){
        TH1F *h1 = (TH1F*)t;
        tName = t->GetName();
        gPad->SetLogy();
        h1->Draw();
        c1->cd();
        c1->Update();
      }
    }
  }

  pdf->Close();
  f->Close(); 
  return;
}
void setpad(TVirtualPad *pad,float left, float right, float top, float bottom, int logy, int logz){
  pad->SetFillColor(10);
  pad->SetBorderMode(0);
  pad->SetBorderSize(0);
  pad->SetFrameFillColor(10);
  pad->SetFrameBorderMode(0);
  pad->SetFrameBorderSize(0);
  pad->SetLeftMargin(left);
  pad->SetRightMargin(right);
  pad->SetTopMargin(top);
  pad->SetBottomMargin(bottom);
  pad->SetLogy(logy);
  pad->SetLogz(logz);
  return;
}
TLegend* myLeg(Double_t xlow, Double_t ylow, Double_t xup, Double_t yup, Int_t textFont, Double_t textSize)
{
  TLegend *leg = new TLegend(xlow,ylow,xup,yup);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetFillColor(10);
  leg->SetTextFont(textFont);
  leg->SetTextSize(textSize);
  return leg;
}
void setcolz()
{
  const Int_t NRGBs = 7, NCont = 999;
  gStyle->SetNumberContours(NCont);
  Double_t stops[NRGBs] = { 0.00, 0.17, 0.34, 0.51, 0.68, 0.85, 1.00 };
  Double_t red[NRGBs]   = { 0.53, 0.00, 0.00, 0.00, 0.97, 0.77, 0.97 };
  Double_t green[NRGBs] = { 0.17, 0.00, 0.97, 0.97, 0.97, 0.00, 0.00 };
  Double_t blue[NRGBs]  = { 0.89, 0.97, 0.97, 0.00, 0.00, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  cout << "colz is customized!" << endl;

  /*const Int_t NRGBs=5;
    const Int_t NCont=32;

    Double_t stops[NRGBs]={0.00,0.34,0.61,0.84,1.00};
    Double_t red[NRGBs]={0.00,0.00,0.87,1.00,0.51};
    Double_t green[NRGBs]={0.00,0.81,1.00,0.20,0.00};
    Double_t blue[NRGBs]={0.51,1.00,0.12,0.00,0.00};
    TColor::CreateGradientColorTable(NRGBs,stops,red,green,blue,NCont);
    gStyle->SetNumberContours(NCont);*/
}
void setstyle()
{
  TStyle* myStyle = new TStyle("myStyle","Styles");
  myStyle->SetPalette(1,0); // avoid horrible default color scheme
  myStyle->SetOptStat("e");
  myStyle->SetOptTitle(1);
  myStyle->SetOptDate(0);
  myStyle->SetLabelSize(0.045,"xyz"); // size of axis value font
  myStyle->SetTitleX(0.2f);
  myStyle->SetTitleY(0.96f);
  myStyle->SetTitleW(0.5f);
  myStyle->SetTickLength(0.01,"xyz");
  myStyle->SetTitleFont(62,"xyz"); // font option 
  myStyle->SetLabelFont(62,"xyz");
  myStyle->SetTitleOffset(0.8,"z");
  myStyle->SetTitleOffset(1.2,"y");
  myStyle->SetTitleFillColor(0);
  myStyle->SetLineWidth(2);
  myStyle->SetCanvasDefW(700);
  myStyle->SetCanvasDefH(600);
  myStyle->SetCanvasColor(0);// canvas...
  myStyle->SetCanvasBorderMode(0);
  myStyle->SetCanvasBorderSize(0);
  myStyle->SetPadColor(0);
  myStyle->SetPadBorderSize(1);
  myStyle->SetPadBorderMode(-1);
  myStyle->SetPadBottomMargin(0.14); //margins...
  myStyle->SetPadTopMargin(0.06);
  myStyle->SetPadLeftMargin(0.14);
  myStyle->SetPadRightMargin(0.04);
  myStyle->SetPadGridX(0); // grids, tickmarks
  myStyle->SetPadGridY(0);
  myStyle->SetPadTickX(1);
  myStyle->SetPadTickY(1);
  myStyle->SetFrameBorderSize(1);
  myStyle->SetFrameBorderMode(-1);
  myStyle->SetFrameFillColor(0);
  myStyle->SetFrameLineWidth(1);
  myStyle->SetPaperSize(20,24); // US letter size
  gROOT->SetStyle("myStyle");
  cout << "Styles are Set!" << endl;

}


