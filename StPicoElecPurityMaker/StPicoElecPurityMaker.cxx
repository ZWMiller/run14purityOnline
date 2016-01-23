#include "StPicoElecPurityMaker.h"
#include "StRoot/StPicoDstMaker/StPicoDst.h"
#include "StRoot/StPicoDstMaker/StPicoTrack.h"
#include "StRoot/StPicoDstMaker/StPicoV0.h"
#include "StRoot/StPicoDstMaker/StPicoDstMaker.h"
#include "StRoot/StPicoDstMaker/StPicoEvent.h"
#include "StRoot/StPicoDstMaker/StPicoMtdHit.h"
#include "StRoot/StPicoDstMaker/StPicoConstants.h"
#include "StRoot/StPicoDstMaker/StPicoMtdPidTraits.h"
#include "StRoot/StPicoDstMaker/StPicoBTofPidTraits.h"
#include "StRoot/StPicoDstMaker/StPicoEmcPidTraits.h"
#include "StRoot/StPicoDstMaker/StPicoEmcTrigger.h"
#include "StDcaGeometry.h"

#include "StThreeVectorF.hh"
#include "TLorentzVector.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TFile.h"
#include <map>
#include<iostream>
#include<fstream>
#include "mBadRunList.h"
#include "StBTofUtil/tofPathLength.hh"
#define USHRT_MAX 65535

Bool_t fillhistflag=1;
ofstream runidfiles;
Int_t runIndex;
Int_t randomId;
map<Int_t,Int_t> mTotalRunId;
Int_t mTotalRun = 599;//603


ClassImp(StPicoElecPurityMaker)

  //-----------------------------------------------------------------------------
  StPicoElecPurityMaker::StPicoElecPurityMaker(const char* name, StPicoDstMaker *picoMaker, const char* outName)
: StMaker(name)
{
  mPicoDstMaker = picoMaker;
  mPicoDst = 0;
  TH1F:: SetDefaultSumw2();//zaochen add
  mOutName = outName;

  mNBadRuns = sizeof(mBadRuns)/sizeof(int);

  // ZWM Add (declare default cut values, can change in Ana with same function)
  SetDefaultCuts();
}

//----------------------------------------------------------------------------- 
StPicoElecPurityMaker::~StPicoElecPurityMaker()
{ /*  */ }

//----------------------------------------------------------------------------- 
Int_t StPicoElecPurityMaker::Init() {
  cout << " --------------------------------------------------- in purity init ------------------------------------------------------- " << endl;
  if(mOutName!="") {
    fout = new TFile(mOutName.Data(),"RECREATE");
  }else{
    fout = new TFile("picoQA_test.root","RECREATE");
  }
  DeclareHistograms();

  //runidfiles.open("/star/u/zye20/zye20/zaochen/analysis/run14picoQA/runnumber.txt", ios::out|ios::app);


  if(fillhistflag){
    //read in the runlist.dat
    ifstream indata;
    indata.open("StRoot/StPicoElecPurityMaker/mTotalRunList.dat");
    mTotalRunId.clear();
    if(indata.is_open()){
      cout<<"read in total run number list and recode run number ...";
      Int_t oldId;
      Int_t newId=0;
      while(indata>>oldId){
        mTotalRunId[oldId] = newId;
        newId++;
      }
      cout<<" [OK]"<<endl;  

    }else{
      cout<<"Failed to load the total run number list !!!"<<endl;
      return kFALSE;
    }

    indata.close();

    for(map<Int_t,Int_t>::iterator iter=mTotalRunId.begin();iter!=mTotalRunId.end();iter++)
      cout<<iter->second<<" \t"<<iter->first<<endl;
    cout<<endl;
    // 
    //read in the runidlist


  }//


  return kStOK;
}

//----------------------------------------------------------------------------- 
Int_t StPicoElecPurityMaker::Finish() {
  fout->cd();
  fout->Write();
  fout->Close();
  return kStOK;
}

//-----------------------------------------------------------------------------
void StPicoElecPurityMaker::DeclareHistograms() {

  // Define parameters for histo definitions that change often
  Float_t nSigLim = 20+1e-6; // To fix bin undulation
  Float_t betaLow = 0.8;
  Float_t betaHigh = 1.8;
  fout->cd();
  for(int tr=0; tr<numTrigs; tr++)
  {
    hNEvents[tr] = new TH1F(Form("hNEvents_%i",tr),"number of events: 0 for total and 2 for MBs events", 10, 0, 10 );
    htriggerindex[tr] =new TH1F(Form("htriggerindex_%i",tr),"triggerindex", 25,0,25);
    mVz_vpd[tr] = new TH1F(Form("vz_vpd_%i",tr),"VZ_VPD distribution (cm)",400,-200,200);
    mVz_tpc[tr] = new TH1F(Form("vz_tpc_%i",tr),"the Vz_TPC distribution (cm) ",400,-200,200);
    mVz_vpdtpc[tr] = new TH2F(Form("Vz_vpdtpc_%i",tr),"VertexZ: VPD VS TPC;TPC;VPD;", 400,-200,200,400,-200,200);
    mdVz[tr] = new TH1F(Form("dVz_%i",tr),"VertexZ 'TPC-VPD' (cm)", 400,-200,200);
    mdVz_tpcVz[tr] = new TH2F(Form("dVz_tpcVz_%i",tr),"VertexZ 'TPC-VPD vs TPC' (cm);VZtpc; VZ(tpc-vpd)",400,-200,200,200,-100,100);
    mVxy[tr] = new TH2F(Form("vxy_%i",tr),"Vertex: Vy Vs Vx distribution (cm);vx;vy; ",200,-10,10,200,-10,10);
    mVRvsVZ[tr] = new TH2F(Form("VRvsVZ_%i",tr),"Vertex: VR vs VZ  (cm);VZ;VR; ", 400, -200, 200,200,0,20);

    mNptracks[tr] = new TH1F(Form("Nptracks_%i",tr),"#primary tracks",200,0,2000);    
    mNgtracks[tr] = new TH1F(Form("Ngtracks_%i",tr),"# global tracks ",200,0,5000);    

    mtrketa_pt[tr] = new TH2F(Form("trketa_pt_%i",tr),"trketa_pt; pt; eta;", 200,0,20,200,-2,2);
    mtrkphi_pt[tr] = new TH2F(Form("trkphi_pt_%i",tr),"trkphi_pt; pt; phi;",200,0,20,200,0,6.3);

    mtrkpt[tr] = new TH1F(Form("trkpt_%i",tr),"the pt distribution of all tracks",200,0,20);
    mtrketa[tr] = new TH1F(Form("trketa_%i",tr),"eta ",200,-2,2);
    mtrkphi[tr] = new TH1F(Form("trkphi_%i",tr),"the phi distribution of all tracks",200,0,6.3);

    mnSigmaPI_Pt_BEMC[tr][0] = new TH2F(Form("nSigmaPI_Pt_BEMC_%i",tr),"nSigmapion vs Pt of all tracks using BEMC; Pt; nSigmaPI;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaP_Pt_BEMC[tr][0] = new TH2F(Form("nSigmaP_Pt_BEMC_%i",tr),"nSigmaproton vs Pt of all tracks using BEMC; Pt; nSigmaP;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaK_Pt_BEMC[tr][0] = new TH2F(Form("nSigmaK_Pt_BEMC_%i",tr),"nSigmaK vs Pt of all tracks using BEMC; Pt; nSigmaK;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_Pt_BEMC[tr][0] = new TH2F(Form("nSigmaE_Pt_BEMC_%i",tr),"nSigmaE vs Pt of all tracks using BEMC; Pt; nSigmaE;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaPI_Pt_BEMC[tr][1] = new TH2F(Form("nSigmaPI_Pt_BEMC_HFT_%i",tr),"nSigmapion vs Pt of HFT tracks; Pt; nSigmaPI;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaP_Pt_BEMC[tr][1] = new TH2F(Form("nSigmaP_Pt_BEMC_HFT_%i",tr),"nSigmaproton vs Pt of HFT tracks; Pt; nSigmaP;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaK_Pt_BEMC[tr][1] = new TH2F(Form("nSigmaK_Pt_BEMC_HFT_%i",tr),"nSigmaK vs Pt of HFT tracks; Pt; nSigmaK;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_Pt_BEMC[tr][1] = new TH2F(Form("nSigmaE_Pt_BEMC_HFT_%i",tr),"nSigmaE vs Pt of HFT tracks; Pt; nSigmaE;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaPI_Pt_SMD[tr][0] = new TH2F(Form("nSigmaPI_Pt_SMD_%i",tr),"nSigmaPI vs Pt of all tracks using SMD; Pt; nSigmaPI;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaP_Pt_SMD[tr][0] = new TH2F(Form("nSigmaP_Pt_SMD_%i",tr),"nSigmaP vs Pt of all tracks using SMD; Pt; nSigmaP;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaK_Pt_SMD[tr][0] = new TH2F(Form("nSigmaK_Pt_SMD_%i",tr),"nSigmaK vs Pt of all tracks using SMD; Pt; nSigmaK;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_Pt_SMD[tr][0] = new TH2F(Form("nSigmaE_Pt_SMD_%i",tr),"nSigmaE vs Pt of all tracks using SMD; Pt; nSigmaE;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaPI_Pt_SMD[tr][1] = new TH2F(Form("nSigmaPI_Pt_SMD_HFT_%i",tr),"nSigmaPI vs Pt of HFT tracks using SMD; Pt; nSigmaPI;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaP_Pt_SMD[tr][1] = new TH2F(Form("nSigmaP_Pt_SMD_HFT_%i",tr),"nSigmaP vs Pt of HFT tracks using SMD; Pt; nSigmaP;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaK_Pt_SMD[tr][1] = new TH2F(Form("nSigmaK_Pt_SMD_HFT_%i",tr),"nSigmaK vs Pt of HFT tracks using SMD; Pt; nSigmaK;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_Pt_SMD[tr][1] = new TH2F(Form("nSigmaE_Pt_SMD_HFT_%i",tr),"nSigmaE vs Pt of HFT tracks using SMD; Pt; nSigmaE;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaPI_Pt_TOF[tr][0] = new TH2F(Form("nSigmaPI_Pt_TOF_%i",tr),"nSigmaPI vs Pt of all tracks using TOF; Pt; nSigmaPI;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaP_Pt_TOF[tr][0] = new TH2F(Form("nSigmaP_Pt_TOF_%i",tr),"nSigmaP vs Pt of all tracks using TOF; Pt; nSigmaP;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaK_Pt_TOF[tr][0] = new TH2F(Form("nSigmaK_Pt_TOF_%i",tr),"nSigmaK vs Pt of all tracks using TOF; Pt; nSigmaK;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_Pt_TOF[tr][0] = new TH2F(Form("nSigmaE_Pt_TOF_%i",tr),"nSigmaE vs Pt of all tracks using TOF; Pt; nSigmaE;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaPI_Pt_TOF[tr][1] = new TH2F(Form("nSigmaPI_Pt_TOF_HFT_%i",tr),"nSigmaPI vs Pt of HFT tracks using TOF; Pt; nSigmaPI;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaP_Pt_TOF[tr][1] = new TH2F(Form("nSigmaP_Pt_TOF_HFT_%i",tr),"nSigmaP vs Pt of HFT tracks using TOF; Pt; nSigmaP;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaK_Pt_TOF[tr][1] = new TH2F(Form("nSigmaK_Pt_TOF_HFT_%i",tr),"nSigmaK vs Pt of HFT tracks using TOF; Pt; nSigmaK;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_Pt_TOF[tr][1] = new TH2F(Form("nSigmaE_Pt_TOF_HFT_%i",tr),"nSigmaE vs Pt of HFT tracks using TOF; Pt; nSigmaE;",400,0,20,200,-nSigLim,nSigLim);

    mnsigmaPI[tr] = new TH1F(Form("nsigmaPI_%i",tr),"nsigmapion of all tracks",200,-nSigLim,nSigLim);
    mnsigmaK[tr] = new TH1F(Form("nsigmaK_%i",tr),"nsigmaKaon of all tracks",200,-nSigLim,nSigLim);
    mnsigmaE[tr] = new TH1F(Form("nsigmaE_%i",tr),"nsigmaElectron of all tracks",200,-nSigLim,nSigLim);
    mnsigmaP[tr] = new TH1F(Form("nsigmaP_%i",tr),"nsigmaProton of all tracks",200,-nSigLim,nSigLim);

    mdedx_Pt[tr] = new TH2F(Form("Dedx_Pt_%i",tr),"dedx(keV/cm) vs Pt; Pt; Dedx;",200,-20,20,250,0,10);
    minvsBeta_Pt[tr] = new TH2F(Form("invsBeta_Pt_%i",tr),"1/Beta vs Pt; Pt; 1/beta;",300,0,6,400,betaLow,betaHigh);
    mtofM2_Pt[tr] = new TH2F(Form("tofM2_Pt_%i",tr),"tofM2 vs momentum; P; tofM2;",200,0,20,200,-0.5,4.5);

    mdedxvsBeta[tr] = new TH2F(Form("ndedxvsBeta_%i",tr),"de/dX vs 1/Beta; ",400,betaLow,betaHigh,250,0,10);
    mnSigmaEvsBeta[tr] = new TH2F(Form("nSigmaEvsBeta_%i",tr),"nSigmaE vs 1/Beta; ",400,betaLow,betaHigh,50,-15,15);
    mnSigmaPIvsBeta[tr] = new TH2F(Form("nSigmaPIvsBeta_%i",tr),"nSigmaPI vs 1/Beta; ",400,betaLow,betaHigh,50,-15,15);
    mnSigmaKvsBeta[tr] = new TH2F(Form("nSigmaKvsBeta_%i",tr),"nSigmaK vs 1/Beta; ",400,betaLow,betaHigh,50,-15,15);
    mnSigmaPvsBeta[tr] = new TH2F(Form("nSigmaPvsBeta_%i",tr),"nSigmaP vs 1/Beta; ",400,betaLow,betaHigh,50,-15,15);
    mtofm2vsBeta[tr] = new TH2F(Form("ntofm2vsBeta_%i",tr),"tofM2 vs 1/Beta; ",400,betaLow,betaHigh,200,-0.5,4.5);

    mtoftray_localY[tr] = new TH2F(Form("toftray_localY_%i",tr),"localY VS toftray; tray; localY;",120,0,120,100,-4,4);
    mtoftray_localZ[tr] = new TH2F(Form("toftray_localZ_%i",tr),"localZ VS toftray; tray; localZ;",120,0,120,100,-4,4);
    //mtofhitPosXYZ[tr] = new TH3F(Form("tofhitPosXYZ_%i",tr),"tofhitPosXYZ",400,-200,200,400,-200,200,400,-200,200);
    mtoftray_matchflag[tr] = new TH2F(Form("toftray_matchflag_%i",tr),"toftray_matchflag;tofTray; tofmatchflag;",120,0,120,4,0,4);
    mtoftray_module[tr] = new TH2F(Form("toftray_module_%i",tr),"toftray_module;tofTray; module",120,0,120,40,0,40);

    //---------------------mtd------------------------
    hNTracks[tr] = new TH1F(Form("hNTracks_%i",tr),"Number of Tracks before (0) and after cuts (2);",10,0,10);

    mnSigmaE_KEnh_Pt[tr][0] = new TH2F(Form("nSigmaE_KEnh_Pt_%i",tr),"nSigmaE vs pT;pT; Kaon Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_PEnh_Pt[tr][0] = new TH2F(Form("nSigmaE_PEnh_Pt_%i",tr),"nSigmaE vs pT;pT; Proton Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_PiEnh_Pt[tr][0] = new TH2F(Form("nSigmaE_PiEnh_Pt_%i",tr),"nSigmaE vs pT;pT; Pion Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_KEnh_Pt[tr][1] = new TH2F(Form("nSigmaE_KEnh_Pt_HFT_%i",tr),"nSigmaE vs pT with HFT;pT; Kaon Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_PEnh_Pt[tr][1] = new TH2F(Form("nSigmaE_PEnh_Pt_HFT)%i",tr),"nSigmaE vs pT with HFT;pT; Proton Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_PiEnh_Pt[tr][1] = new TH2F(Form("nSigmaE_PiEnh_Pt_HFT_%i",tr),"nSigmaE vs pT with HFT;pT; Pion Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaK_KEnh_Pt[tr][0] = new TH2F(Form("nSigmaK_KEnh_Pt_%i",tr),"nSigmaK vs pT; Kaon Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaP_PEnh_Pt[tr][0] = new TH2F(Form("nSigmaP_PEnh_Pt_%i",tr),"nSigmaP vs pT; Proton Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaPi_PiEnh_Pt[tr][0] = new TH2F(Form("nSigmaPi_PiEnh_Pt_%i",tr),"nSigmaPi vs pT; Pion Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaK_KEnh_Pt[tr][1] = new TH2F(Form("nSigmaK_KEnh_Pt_HFT_%i",tr),"nSigmaK vs pT with HFT;pT; Kaon Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaP_PEnh_Pt[tr][1] = new TH2F(Form("nSigmaP_PEnh_Pt_HFT_%i",tr),"nSigmaP vs pT with HFT;pT; Proton Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaPi_PiEnh_Pt[tr][1] = new TH2F(Form("nSigmaPi_PiEnh_Pt_HFT_%i",tr),"nSigmaPi vs pT with HFT;pT; Pion Enhanced;",400,0,20,200,-nSigLim,nSigLim);

  }
}// er chen

//----------------------------------------------------------------------------- 
void StPicoElecPurityMaker::Clear(Option_t *opt) {

}

//----------------------------------------------------------------------------- 
Int_t StPicoElecPurityMaker::Make() {
  if(!mPicoDstMaker) {
    LOG_WARN << " No PicoDstMaker! Skip! " << endm;
    return kStWarn;
  }

  mPicoDst = mPicoDstMaker->picoDst();

  if(!mPicoDst) {
    LOG_WARN << " No PicoDst! Skip! " << endm;
    return kStWarn;
  }

  StPicoEvent* event=mPicoDst->event();
  //=================zaochen add====================
  if(!event) return kStOK;
  //=================event selection================
  int runId = event->runId();
  for(int i=0;i<mNBadRuns;i++){
    if(runId==mBadRuns[i]) return kStOK;
  }

  trig = 99;
  if( event->isMinBias()) FillHistograms(0,event); // Decide what type of trigger you have, use to select what histos to fill
  if( isBHT1( event ) )   FillHistograms(1,event);
  if( isBHT2( event ) )   FillHistograms(2,event);
  if( isBHT3( event ) )   FillHistograms(3,event);
  if( trig == 99 ) return kStOK; // if no trigger match, throw out event

  return kStOK;
}

Int_t StPicoElecPurityMaker::FillHistograms(Int_t trig, StPicoEvent* event)
{
  //=================event selection================
  hNEvents[trig]->Fill(0);
  if(! passEventCuts(event,trig) ) return kStOK;
  hNEvents[trig]->Fill(2);

  if(fillhistflag){	

    Int_t RUNID = event->runId();
    map<Int_t, Int_t>::iterator iter = mTotalRunId.find(RUNID);
    if(iter != mTotalRunId.end())
      runIndex = iter->second;
    else{
      runIndex = -1;
      cout<<"sorry, no runNumber in runNumber list"<<endl;
      cout<<"the RUNID:="<<RUNID<<endl;
    }

    //if(runIndex<0)return kStOK;

  }//

  //int triggerWord = event->triggerWord();
  //	if(triggerWord>>19 & 0x1
  Int_t triggerWORD=event->triggerWord();
  for(Int_t i=0; i<25; i++){
    if( triggerWORD>>i & 0x1 ) htriggerindex[trig]->Fill(i);

  }

  //	runidfiles<<event->runId()<<endl;

  //cout<<"run number is:"<<event->runId()<<endl;

  //======================ALL QA PLOTS ===============




  //---------------event information----------------------------
  Double_t vzvpd=event->vzVpd();
  Double_t vztpc=event->primaryVertex().z();
  Double_t vxtpc=event->primaryVertex().x();
  Double_t vytpc=event->primaryVertex().y();
  Double_t dvz=vztpc-vzvpd;
  Double_t vr=sqrt(vxtpc*vxtpc+vytpc*vytpc);

  // Event Cuts
  if(fabs(vxtpc)<1.0e-5)  return kStOK;
  if(fabs(vytpc)<1.0e-5)  return kStOK;
  if(fabs(vztpc)<1.0e-5)  return kStOK;
  //if(fabs(fzvpd) < vZcut) return kStOK;
  //if(fabs(dvz) < dvZcut)  return kStOK;

  if(fillhistflag){
    mVz_tpc[trig]->Fill(vztpc);
    mVz_vpd[trig]->Fill(vzvpd);
    mdVz[trig]->Fill(dvz);  
    mVz_vpdtpc[trig]->Fill(vztpc,vzvpd);
    mdVz_tpcVz[trig]->Fill(vztpc,dvz);
    mVxy[trig]->Fill( event->primaryVertex().x(), event->primaryVertex().y() );
    mVRvsVZ[trig]->Fill(vztpc, vr);
  }//


  //Int_t Nptrks = mPicoDst->numberOfTracks();
  Float_t Ranking = event->ranking();
  Float_t zdcx = event->ZDCx();
  Float_t bbcx = event->BBCx();
  zdcx=zdcx/1000.;
  bbcx=bbcx/1000.;
  Int_t Nmtdhits = mPicoDst->numberOfMtdHits();
  Int_t Ntofhits = mPicoDst->numberOfBTofHits();
  Int_t NRefmultPos=event->refMultPos();
  Int_t NRefmultNeg=event->refMultNeg();
  Int_t NGnremult=event->grefMult();
  Int_t NRefmult=event->refMult();
  Int_t NGtrks = event->numberOfGlobalTracks();
  Int_t Ntofmatch = event->nBTOFMatch();
  //	Int_t Nbemchits = event->
  Int_t Nbemcmatch = event->nBEMCMatch();

  //----------track information------------------------  
  Int_t numberoftracks = mPicoDst->numberOfTracks();
  if(fillhistflag){	
    mNptracks[trig]->Fill(numberoftracks);
    mNgtracks[trig]->Fill(NGtrks);
  }//

  //tVzTPC=event->primaryVertex().z()

  StThreeVectorF vertexPos;
  vertexPos = event->primaryVertex();

  Int_t Nprimarytracks=0;
  Int_t ntofmatchcount=0;
  Int_t nmtdmatchcount=0;
  Int_t nbemcmatchcount=0;
  Int_t nhftmatchcount=0;
  Int_t nmuons=0;
  Int_t ntofelecton=0;
  Int_t nbemcelectron=0;
  Float_t particleM[3]={0.938,0.140,0.494};

  // TRACK LOOP
  for(int i=0; i<numberoftracks; i++){

    StPicoTrack* track=(StPicoTrack*) mPicoDst->track(i);

    // Check if pass track quality if fails... skip it
    hNTracks[trig]->Fill(0);
    trkHFTflag = 0;
    if(!passGoodTrack(event, track, trig)) continue;
    hNTracks[trig]->Fill(2);

    Double_t meta,mpt,mphi,mcharge,mdedx;

    //change to global track
    meta=track->gMom(event->primaryVertex(),event->bField()).pseudoRapidity();
    if(track->pMom().mag()!=0) Nprimarytracks++;
    mphi=RotatePhi(track->gMom(event->primaryVertex(),event->bField()).phi());
    mpt=track->gMom(event->primaryVertex(),event->bField()).perp();
    mcharge=track->charge();
    mdedx=track->dEdx();

    if(mcharge==0||meta==0||mphi==0||mdedx==0/*||track->pMom().mag()!=0*/) continue; //remove neutral, untracked, or primary tracks
    hNTracks[trig]->Fill(4);
    if(track->isHFTTrack()){
      nhftmatchcount++;
      if(fabs(event->primaryVertex().z()) < vZcutHFT[trig])      
      {
        trkHFTflag = 1; 
        hNTracks[trig]->Fill(6);
      }
    }

    Float_t mmomentum=track->gMom(event->primaryVertex(),event->bField()).mag();
    Double_t nsigpi=track->nSigmaPion();
    Double_t nsigk=track->nSigmaKaon();
    Double_t nsigp=track->nSigmaProton();
    Double_t nsige=track->nSigmaElectron();

    mtrkpt[trig]->Fill( mpt );
    mtrketa[trig]->Fill( meta );
    mtrkphi[trig]->Fill( mphi );
    //   mtrketaphi[trig]->Fill( mphi,meta );
    mnsigmaPI[trig]->Fill( nsigpi );
    mnsigmaP[trig] ->Fill( nsigp );
    mnsigmaK[trig] ->Fill( nsigk );
    mnsigmaE[trig] ->Fill( nsige );

    mtrketa_pt[trig]->Fill(mpt*mcharge,meta);
    mtrkphi_pt[trig]->Fill(mpt*mcharge,mphi);

    mdedx_Pt[trig]->Fill(mpt*mcharge,track->dEdx());

    // BEMC nSig
    if(passBEMCCuts(event, track, trig))
    {
      mnSigmaPI_Pt_BEMC[trig][0]->Fill(mpt,nsigpi);
      mnSigmaP_Pt_BEMC[trig][0]->Fill(mpt,nsigp);
      mnSigmaE_Pt_BEMC[trig][0]->Fill(mpt,nsige);
      mnSigmaK_Pt_BEMC[trig][0]->Fill(mpt,nsigk);
      if(trkHFTflag == 1)
      {
        mnSigmaPI_Pt_BEMC[trig][trkHFTflag]->Fill(mpt,nsigpi);
        mnSigmaP_Pt_BEMC[trig][trkHFTflag]->Fill(mpt,nsigp);
        mnSigmaE_Pt_BEMC[trig][trkHFTflag]->Fill(mpt,nsige);
        mnSigmaK_Pt_BEMC[trig][trkHFTflag]->Fill(mpt,nsigk);
      }

      // SMD and BEMC
      if( passSMDCuts(event, track, trig) )
      {
        mnSigmaPI_Pt_SMD[trig][0]->Fill(mpt,nsigpi);
        mnSigmaP_Pt_SMD[trig][0]->Fill(mpt,nsigp);
        mnSigmaE_Pt_SMD[trig][0]->Fill(mpt,nsige);
        mnSigmaK_Pt_SMD[trig][0]->Fill(mpt,nsigk);

        if(trkHFTflag == 1)
        {
          mnSigmaPI_Pt_SMD[trig][trkHFTflag]->Fill(mpt,nsigpi);
          mnSigmaP_Pt_SMD[trig][trkHFTflag]->Fill(mpt,nsigp);
          mnSigmaE_Pt_SMD[trig][trkHFTflag]->Fill(mpt,nsige);
          mnSigmaK_Pt_SMD[trig][trkHFTflag]->Fill(mpt,nsigk);
        }
      }
    }
    // TOF Information
    if(passTOFCuts(event, track, trig))
    {
      Int_t tofpidid=track->bTofPidTraitsIndex();
      if(tofpidid>0){
        ntofmatchcount++;
        StPicoBTofPidTraits* btofpidtrait=(StPicoBTofPidTraits*) mPicoDst->btofPidTraits(tofpidid);

        Float_t beta=btofpidtrait->btofBeta();
        StPhysicalHelixD helix = track->helix();
        if(beta<1e-4||beta>=(USHRT_MAX-1)/20000){
          Float_t tof = btofpidtrait->btof();
          StThreeVectorF btofHitPos = btofpidtrait->btofHitPos();
          float L = tofPathLength(&vertexPos, &btofHitPos, helix.curvature()); 
          if(tof>0) beta = L/(tof*(c_light/1.0e9));
        }
        Float_t tofbeta = 1./beta;
        //Float_t tofbeta = 1./beta;
        Double_t tofm2=mmomentum*mmomentum*( 1.0/(tofbeta*tofbeta)-1.0);
        minvsBeta_Pt[trig]->Fill(mpt,tofbeta);
        if(tofbeta>0){
          mtofM2_Pt[trig]->Fill(mpt,tofm2);
        }

        // For Purity
        mdedxvsBeta    [trig]->Fill(tofbeta, track->dEdx());
        mnSigmaEvsBeta [trig]->Fill(tofbeta, nsige);
        mnSigmaPIvsBeta[trig]->Fill(tofbeta, nsigpi);
        mnSigmaKvsBeta [trig]->Fill(tofbeta, nsigk);
        mnSigmaPvsBeta [trig]->Fill(tofbeta, nsigp);
        mtofm2vsBeta   [trig]->Fill(tofbeta, tofm2);

        mnSigmaPI_Pt_TOF[trig][0]->Fill(mpt,nsigpi);
        mnSigmaP_Pt_TOF[trig][0]->Fill(mpt,nsigp);
        mnSigmaE_Pt_TOF[trig][0]->Fill(mpt,nsige);
        mnSigmaK_Pt_TOF[trig][0]->Fill(mpt,nsigk);

        if(trkHFTflag == 1)
        {
          mnSigmaPI_Pt_TOF[trig][trkHFTflag]->Fill(mpt,nsigpi);
          mnSigmaP_Pt_TOF[trig][trkHFTflag]->Fill(mpt,nsigp);
          mnSigmaE_Pt_TOF[trig][trkHFTflag]->Fill(mpt,nsige);
          mnSigmaK_Pt_TOF[trig][trkHFTflag]->Fill(mpt,nsigk);
        }

        if(kaonEnhCutLow < tofbeta && tofbeta < kaonEnhCutHigh)
        {
          mnSigmaE_KEnh_Pt[trig][0]->Fill(mpt,nsige);
          mnSigmaK_KEnh_Pt[trig][0]->Fill(mpt,nsigk);
          if(trkHFTflag == 1)
          {
            mnSigmaE_KEnh_Pt[trig][trkHFTflag]->Fill(mpt,nsige);
            mnSigmaK_KEnh_Pt[trig][trkHFTflag]->Fill(mpt,nsigk);
          }
        }

        if(pionEnhCutLow < tofbeta && tofbeta < pionEnhCutHigh)
        {
          mnSigmaE_PiEnh_Pt [trig][0]->Fill(mpt,nsige);
          mnSigmaPi_PiEnh_Pt[trig][0]->Fill(mpt,nsigpi);
          if(trkHFTflag == 1)
          {
            mnSigmaE_PiEnh_Pt[trig][trkHFTflag]->Fill(mpt,nsige);
            mnSigmaPi_PiEnh_Pt[trig][trkHFTflag]->Fill(mpt,nsigk);
          }
        }

        if(protonEnhCutLow < tofbeta && tofbeta < protonEnhCutHigh)
        {
          mnSigmaE_PEnh_Pt[trig][0]->Fill(mpt,nsige);
          mnSigmaP_PEnh_Pt[trig][0]->Fill(mpt,nsigp);
          if(trkHFTflag == 1)
          {
            mnSigmaE_PEnh_Pt[trig][trkHFTflag]->Fill(mpt,nsige);
            mnSigmaP_PEnh_Pt[trig][trkHFTflag]->Fill(mpt,nsigk);
          }
        }

        Int_t tofcellid=   btofpidtrait->btofCellId();
        Int_t toftray= (int)tofcellid/192 + 1;
        Int_t tofmodule= (int)((tofcellid%192)/6.)+1;

        Float_t toflocaly = btofpidtrait->btofYLocal();
        Float_t toflocalz = btofpidtrait->btofZLocal();
        // Float_t tofhitPosx = btofpidtrait->btofHitPos().x();
        // Float_t tofhitPosy = btofpidtrait->btofHitPos().y();
        // Float_t tofhitPosz = btofpidtrait->btofHitPos().z();
        if(fillhistflag){
          mtoftray_localY[trig]->Fill(toftray,toflocaly);
          mtoftray_localZ[trig]->Fill(toftray,toflocalz);
          mtoftray_matchflag[trig]->Fill(toftray,btofpidtrait->btofMatchFlag());
          mtoftray_module[trig]->Fill(toftray,tofmodule);

        }//
      }
    }// End TOF
  }//loop of all tracks
  return kStOK;
}//end of main filling fucntion

//=====================ZAOCHEN'S FUNCTION=======================================


bool StPicoElecPurityMaker::isBHT1(StPicoEvent *event)
{
  int triggerWord = event->triggerWord();
  if(triggerWord>>19 & 0x1 || triggerWord>>20 & 0x1) return true;
  else return false;
}

//-----------------------------------------                                              
bool StPicoElecPurityMaker::isBHT2(StPicoEvent *event)
{
  int triggerWord = event->triggerWord();
  if(triggerWord>>21 & 0x1 || triggerWord>>22 & 0x1) return true;
  else return false;
}

//---------------------------------------------------  
bool StPicoElecPurityMaker::isBHT3(StPicoEvent *event)
{
  int triggerWord = event->triggerWord();
  if(triggerWord>>23 & 0x1 || triggerWord>>24 & 0x1) return true;
  else return false;
}


//------------------------------------------------------------- 

Bool_t StPicoElecPurityMaker::passGoodTrack(StPicoEvent* event, StPicoTrack* track, int trig)
{
  double fithitfrac, chargeq, fhitsdEdx, fhitsFit,feta; 
  double pt = track->gMom(event->primaryVertex(),event->bField()).perp();
  feta=track->gMom(event->primaryVertex(),event->bField()).pseudoRapidity();
  fhitsFit = track->nHitsFit();
  fithitfrac=fhitsFit/track->nHitsMax();
  fhitsdEdx = track->nHitsDedx();
  chargeq=track->charge();
  double PtCut = 0.2;

  double mdca;
  // Get DCA info
  StThreeVectorF vertexPos;
  vertexPos = event->primaryVertex();
  StDcaGeometry *dcaG = new StDcaGeometry();
  dcaG->set(track->params(),track->errMatrix());
  StPhysicalHelixD helix = dcaG->helix();
  delete dcaG;
  StThreeVectorF dcaPoint = helix.at( helix.pathLength(vertexPos.x(), vertexPos.y())  );
  double dcamag= (dcaPoint-vertexPos).mag();
  StThreeVectorF dcaP = helix.momentumAt( vertexPos.x(),vertexPos.y() );
  double dcaXY= ( (dcaPoint-vertexPos).x()*dcaP.y()-(dcaPoint-vertexPos).y()*dcaP.x() )/dcaP.perp();
  double dcaZ= dcaPoint.z() - vertexPos.z();
  mdca = dcamag;

  if(pt> PtCut && fhitsFit >= nhitsFitCut && fhitsdEdx >= nhitsdEdxCut && fithitfrac >= nhitsRatioCut && fabs(chargeq)>0 && fabs(feta) <= etaCut && mdca < dcaCut && mdca > 0.) return true;
  else return false;
}


//------------------------------------------------------------- 

Bool_t StPicoElecPurityMaker::passSMDCuts(StPicoEvent* event, StPicoTrack* track, int trig)
{
  // Get BEMC/SMD info
  Int_t emcpidtraitsid=track->emcPidTraitsIndex();
  double mpoe;
  int bemcId, btowId, nPhi,nEta;
  float zDist, phiDist,e0,adc0,dsmadc;
  if(emcpidtraitsid>=0){
    StPicoEmcPidTraits* emcpidtraits=(StPicoEmcPidTraits*) mPicoDst->emcPidTraits(emcpidtraitsid);

    bemcId = emcpidtraits->bemcId();
    adc0   = emcpidtraits->adc0();
    e0     = emcpidtraits->e0();
    btowId = emcpidtraits->btowId();
    zDist = emcpidtraits->zDist();
    phiDist = emcpidtraits->phiDist();
    nEta = emcpidtraits->nEta();
    nPhi = emcpidtraits->nPhi();
    mpoe = track->gMom(event->primaryVertex(),event->bField()).mag()/emcpidtraits->e();
    // get DSM Adc by finding the tower with same id as trk, then getting that ADC
    int nTrgs = mPicoDst->numberOfEmcTriggers();
    for(int j=0;j<nTrgs;j++){
      StPicoEmcTrigger *trg = (StPicoEmcTrigger*)mPicoDst->emcTrigger(j);
      if((trg->flag() & 0xf)){
        int trgId = trg->id();
        if(btowId == trgId){
          dsmadc = trg->adc();
        }
      }
    }
  }
  else 
  {
    mpoe = 0.0; 
    nPhi = -999;
  } // if no BEMC, set value = not in range
  double mpt  = track->gMom(event->primaryVertex(),event->bField()).perp();

  if(mpt > bemcPtCut && fabs(zDist) < zDistCut && (phiDist) < phiDistCut && nEta >= nEtaCut && nPhi >= nPhiCut)
    return true;
  else 
    return false;
}

Bool_t StPicoElecPurityMaker::passBEMCCuts(StPicoEvent* event, StPicoTrack* track, int trig)
{
  // Get BEMC info
  Int_t emcpidtraitsid=track->emcPidTraitsIndex();
  double mpoe;
  int dsmadc = 1;
  int bemcId, btowId, nPhi,nEta;
  float zDist, phiDist,e0,adc0;
  if(emcpidtraitsid>=0){
    StPicoEmcPidTraits* emcpidtraits=(StPicoEmcPidTraits*) mPicoDst->emcPidTraits(emcpidtraitsid);

    bemcId = emcpidtraits->bemcId();
    adc0   = emcpidtraits->adc0();
    e0     = emcpidtraits->e0();
    btowId = emcpidtraits->btowId();
    zDist = emcpidtraits->zDist();
    phiDist = emcpidtraits->phiDist();
    nEta = emcpidtraits->nEta();
    nPhi = emcpidtraits->nPhi();
    mpoe = track->gMom(event->primaryVertex(),event->bField()).mag()/emcpidtraits->e();
    // get DSM Adc by finding the tower with same id as trk, then getting that ADC
    int nTrgs = mPicoDst->numberOfEmcTriggers();
    for(int j=0;j<nTrgs;j++){
      StPicoEmcTrigger *trg = (StPicoEmcTrigger*)mPicoDst->emcTrigger(j);
      if((trg->flag() & 0xf)){
        int trgId = trg->id();
        if(btowId == trgId){
          cout << "bTowId: " << btowId << " ";
          cout << "trgID: " << trgId << " ";
          dsmadc = trg->adc();
          cout << "trg->adc(): " << dsmadc << endl;
          break;
        }
      }
    }
  }
  else 
    mpoe = 0.0; // if no BEMC, set value = 0

  double mpt  = track->gMom(event->primaryVertex(),event->bField()).perp();
  //cout << "pT: " << mpt << " p/E: " << mpoe << " e0: " << e0 << " dsmadc: " << dsmadc << endl;
  if( mpt > bemcPtCut && mpoe > poeCutLow && mpoe < poeCutHigh && dsmadc > getDsmAdcCut(trig) )
    return true;
  else 
    return false;
}

Bool_t StPicoElecPurityMaker::passTOFCuts(StPicoEvent* event, StPicoTrack* track, int trig)
{
  // Get TOF Infor
  Float_t invBeta=9999;
  Float_t toflocaly=9999;
  Float_t tofMatchFlag = -1;
  Int_t tofpidid=track->bTofPidTraitsIndex();
  if(tofpidid>0){
    StPicoBTofPidTraits* btofpidtrait=(StPicoBTofPidTraits*) mPicoDst->btofPidTraits(tofpidid);

    //------tof information start----------
    Float_t tofbeta=btofpidtrait->btofBeta();
    invBeta = (1/tofbeta) - 1.0;
    toflocaly = btofpidtrait->btofYLocal();
    tofMatchFlag = btofpidtrait->btofMatchFlag(); 
  }
  double mpt  = track->gMom(event->primaryVertex(),event->bField()).perp();
  if(mpt < tofPtCut && fabs(invBeta) < tofInvBetaCut && tofMatchFlag > 0 && fabs(toflocaly) < toflocalyCut)
    return true;
  else return false;
}

//-------------------------------------------------------------

Bool_t StPicoElecPurityMaker::Ismuontrack(StPicoEvent* event, StPicoTrack* track)
{
  double pt;
  pt=track->gMom(event->primaryVertex(),event->bField()).perp();
  Int_t mtdpid=track->mtdPidTraitsIndex();
  if(mtdpid<=0)return false;
  StPicoMtdPidTraits* mtdpidtrait=(StPicoMtdPidTraits*) mPicoDst->mtdPidTraits(mtdpid);      
  double mtddz=mtdpidtrait->deltaZ();
  double mtddt=mtdpidtrait->deltaTimeOfFlight();
  if(track->nSigmaPion()<3.0&&track->nSigmaPion()>-1.0&&pt>1.0&&fabs(mtddz)<20.0&&mtddt>-1.0&&mtddt<1.0) return true;
  else return false;

}

//----------------------------------------------------------------
Bool_t StPicoElecPurityMaker::passEventCuts(StPicoEvent* event, int trig)
{
  Double_t vzvpd = event->vzVpd();
  Double_t vztpc = event->primaryVertex().z();
  Double_t dvz = vzvpd - vztpc;
  if(fabs(vztpc) < vZcut[trig] && fabs(dvz) < dvZcut[trig]) return true;
  else return false;
}

//---------------------------------------------------------------
Double_t StPicoElecPurityMaker::RotatePhi(Double_t phi) const
{
  Double_t outPhi = phi;
  Double_t pi=TMath::Pi();
  while(outPhi<0) outPhi += 2*pi;
  while(outPhi>2*pi) outPhi -= 2*pi;
  return outPhi;
}

//===================================================================

void StPicoElecPurityMaker::SetDefaultCuts()
{ 
  numTrigs = 4;
  setNSigECuts(-1.5,3.0);
  setNSigPCuts(-20,20);
  setNSigKCuts(-20,20);
  setNSigPiCuts(-20,20);
  setvZCuts(0,6.0,4.0);  // (vZ, delVz)
  setvZCuts(1,30.0,4.0); // (vZ, delVz)
  setvZCuts(2,30.0,4.0); // (vZ, delVz)
  setvZCuts(3,30.0,4.0); // (vZ, delVz)
  setvZCutsHFT(0,6.0,4.0);  // (vZ, delVz)
  setvZCutsHFT(1,6.0,4.0); // (vZ, delVz)
  setvZCutsHFT(2,6.0,4.0); // (vZ, delVz)
  setvZCutsHFT(3,6.0,4.0); // (vZ, delVz)
  setPrimaryPtCut(3.0, 1.5); // pT < 3 (TOF), pT >1.5 (BEMC)
  setPrimaryEtaCut(1.0); // |eta| < 1.0
  setPrimaryDCACut(1.5); // eDCA < 1.5 cm
  setNhitsCuts(15.,20.,0.52); // nHitsdEdx >= 15, nHitsFit >= 20, nHitsRatio >= 0.52
  setPoECut(0.3, 1.5); // 0.3 < p/E < 1.5
  setToFBetaCut(0.03); // |1/B -1| < 0.03
  setToFLocalyCut(1.8); // |tof_localy| < 1.8
  setKaonEnhCut(0.9,1.1); // 0.9<1/B<1.1 Kaon Species Select
  setPionEnhCut(0.9,1.1); // 0.9<1/B<1.1 Pion Species Select
  setProtonEnhCut(0.9,1.1); // 0.9<1/B<1.1 Proton Species Select
  setDsmAdcCut(0,0); // dsmADC cut sets (not in MB): Use getDsmAdcCut(trig) to return value
  setDsmAdcCut(1,15); // dsmADC cut sets ()
  setDsmAdcCut(2,18); // dsmADC cut sets ()
  setDsmAdcCut(3,21); // dsmADC cut sets ()
  setSMDCuts(0,0,3.,0.8); // nEta>=, nPhi>=, zDist<, phiDist< 
}

