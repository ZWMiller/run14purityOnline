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

    mRanking_nPtrks[tr] =new TH2F(Form("Ranking_nPtrks_%i",tr),"Ranking Vs nPrimaryTracks;nprimarytrks; ranking;",200,0,2000,100,-10,10);
    mnPtrks_nGtrks[tr] =new TH2F(Form("nPtrks_nGtrks_%i",tr),"nPrimarytrks_nGlobaltrks;nglobaltrks;nprimarytrks;",500,0,5000,200,0,2000);

    mnRefMult_nGRefMult[tr] =new TH2F(Form("nRefMult_nGRefMult_%i",tr),"nRefMult_nGRefMult;nGRefMult;nRefmult;",200,0,1000,200,0,1000);
    mnRefMult_nPtrks[tr] =new TH2F(Form("nRefMult_nPtrks_%i",tr),"nRefMult_nPrimarytrks;nprimarytrks;nRefmult;",200,0,2000,200,0,1000);
    mnRefMult_nGtrks[tr] =new TH2F(Form("nRefMult_nGtrks_%i",tr),"nRefMult_nGlobaltrks;nglobaltrks;nRefmult;",250,0,5000,200,0,1000);

    mnGRefMult_nPtrks[tr] =new TH2F(Form("nGRefMult_nPtrks_%i",tr),"nGRefMult_nPrimarytrks;nprimarytrks;nGRefmult;",200,0,2000,200,0,1000);
    mnGRefMult_nGtrks[tr] =new TH2F(Form("nGRefMult_nGtrks_%i",tr),"nGRefMult_nGlobaltrks;nglobaltrks;nGRefmult;",250,0,5000,200,0,1000);

    mnPtrks_nTofHits[tr] =new TH2F(Form("nPtrks_nTofHits_%i",tr),"nPrimarytrks_nTofHits;ntofhits;nprimarytrks;",250,0,2000,200,0,2000);
    mnPtrks_nMtdHits[tr] =new TH2F(Form("nPtrks_nMtdHits_%i",tr),"nPrimarytrks_nMtdHits;nmtdhits;nprimarytrks;",50,0,50,500,0,2000);
    mnTofHits_nMtdHits[tr] =new TH2F(Form("nTofHits_nMtdHits_%i",tr),"nTofHits_nMtdHits;nmtdhits;ntofhits;",50,0,50,200,0,2000);
    mnTofMatch_nTofHits[tr] =new TH2F(Form("nTofMatch_nTofHits_%i",tr),"nTofMatch_nTofHits;ntofhits;ntofmatch;",300,0,1500,300,0,1500);


    mNptracks[tr] = new TH1F(Form("Nptracks_%i",tr),"#primary tracks",200,0,2000);    
    mNgtracks[tr] = new TH1F(Form("Ngtracks_%i",tr),"# global tracks ",200,0,5000);    

    mtrketa_pt[tr] = new TH2F(Form("trketa_pt_%i",tr),"trketa_pt; pt; eta;", 200,0,20,200,-2,2);
    mtrkphi_pt[tr] = new TH2F(Form("trkphi_pt_%i",tr),"trkphi_pt; pt; phi;",200,0,20,200,0,6.3);

    mtrkpt[tr] = new TH1F(Form("trkpt_%i",tr),"the pt distribution of all tracks",200,0,20);
    mtrketa[tr] = new TH1F(Form("trketa_%i",tr),"eta ",200,-2,2);
    mtrkphi[tr] = new TH1F(Form("trkphi_%i",tr),"the phi distribution of all tracks",200,0,6.3);

    mnsigmaPI_Pt[tr] = new TH2F(Form("nsigmaPI_Pt_%i",tr),"nsigmapion vs Pt of all tracks; Pt; nsigmaPI;",400,0,20,200,-nSigLim,nSigLim);
    mnsigmaP_Pt[tr] = new TH2F(Form("nsigmaP_Pt_%i",tr),"nsigmaproton vs Pt of all tracks; Pt; nsigmaP;",400,0,20,200,-nSigLim,nSigLim);
    mnsigmaK_Pt[tr] = new TH2F(Form("nsigmaK_Pt_%i",tr),"nsigmaK vs Pt of all tracks; Pt; nsigmaK;",400,0,20,200,-nSigLim,nSigLim);
    mnsigmaE_Pt[tr] = new TH2F(Form("nsigmaE_Pt_%i",tr),"nsigmaE vs Pt of all tracks; Pt; nsigmaE;",400,0,20,200,-nSigLim,nSigLim);

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
    mmtdBeta_Pt[tr] = new TH2F(Form("mtdBeta_Pt_%i",tr),"mtdBeta VS Pt; Pt (GeV/c); mtdBeta;",200,0,20,200,0.95,1.12);
    mmtdBeta_channel[tr] = new TH2F(Form("mtdBeta_channel_%i",tr),"mtdBeta_channel; channel; mtdbeta;",300,0,1800,300,0.95,1.12);

    hNTracks[tr] = new TH1F(Form("hNTracks_%i",tr),"Number of Tracks before (0) and after cuts (2);",10,0,10);

    mnSigmaE_KEnh_Pt[tr] = new TH2F(Form("nSigmaE_KEnh_Pt_%i",tr),"nSigmaE vs pT; Kaon Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_PEnh_Pt[tr] = new TH2F(Form("nSigmaE_PEnh_Pt_%i",tr),"nSigmaE vs pT; Proton Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaE_PiEnh_Pt[tr] = new TH2F(Form("nSigmaE_PiEnh_Pt_%i",tr),"nSigmaE vs pT; Pion Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaK_KEnh_Pt[tr] = new TH2F(Form("nSigmaK_KEnh_Pt_%i",tr),"nSigmaK vs pT; Kaon Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaP_PEnh_Pt[tr] = new TH2F(Form("nSigmaP_PEnh_Pt_%i",tr),"nSigmaP vs pT; Proton Enhanced;",400,0,20,200,-nSigLim,nSigLim);
    mnSigmaPi_PiEnh_Pt[tr] = new TH2F(Form("nSigmaPi_PiEnh_Pt_%i",tr),"nSigmaPi vs pT; Pion Enhanced;",400,0,20,200,-nSigLim,nSigLim);

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
  if(! passEventCuts(event) ) return kStOK;
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

    // Don't plot DCA currently, no need to calculate
    /*StDcaGeometry *dcaG = new StDcaGeometry();
      dcaG->set(track->params(),track->errMatrix());
      StPhysicalHelixD helix = dcaG->helix();
      delete dcaG;

      StThreeVectorF dcaPoint = helix.at( helix.pathLength(vertexPos.x(), vertexPos.y())  );
      double dcamag= (dcaPoint-vertexPos).mag();
      StThreeVectorF dcaP = helix.momentumAt( vertexPos.x(),vertexPos.y() );
      double dcaXY= ( (dcaPoint-vertexPos).x()*dcaP.y()-(dcaPoint-vertexPos).y()*dcaP.x() )/dcaP.perp();
      double dcaZ= dcaPoint.z() - vertexPos.z();
      */

    // Check if pass track quality && eID, if fail either... skip it
    hNTracks[trig]->Fill(0);
    if(!track->isHFTTrack() || !passGoodTrack(event, track) || !passEIDCuts(event, track)) continue;
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
    }

    Float_t mmomentum=track->gMom(event->primaryVertex(),event->bField()).mag();
    Double_t rationhits = 0.00;
    rationhits=(double)track->nHitsFit()/track->nHitsMax();
    // cout<<"track->nHitsMax()="<<track->nHitsMax()<<"track->nHitsFit()"<<track->nHitsFit()<<endl;
    // cout<<"ratiohits"<<rationhits<<endl;
    Double_t nsigpi=track->nSigmaPion();
    Double_t nsigk=track->nSigmaKaon();
    Double_t nsigp=track->nSigmaProton();
    Double_t nsige=track->nSigmaElectron();
    if(fillhistflag){
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
      mnsigmaPI_Pt[trig]->Fill(mpt,nsigpi);
      mnsigmaP_Pt[trig]->Fill(mpt,nsigp);
      mnsigmaE_Pt[trig]->Fill(mpt,nsige);
      mnsigmaK_Pt[trig]->Fill(mpt,nsigk);

    }

    //------tpc information end-----


    Int_t tofpidid=track->bTofPidTraitsIndex();
    if(tofpidid>0){

      ntofmatchcount++;
      StPicoBTofPidTraits* btofpidtrait=(StPicoBTofPidTraits*) mPicoDst->btofPidTraits(tofpidid);

      //------tof information start----------
      Float_t beta=btofpidtrait->btofBeta();
      StPhysicalHelixD helix = track->helix();
     /* if(beta<1e-4||beta>=(USHRT_MAX-1)/20000){
        Float_t tof = btofpidtrait->btof();
        StThreeVectorF btofHitPos = btofpidtrait->btofHitPos();
        float L = tofPathLength(&vertexPos, &btofHitPos, helix.curvature()); 
        if(tof>0) beta = L/(tof*(c_light/1.0e9));
      }
      Float_t tofbeta = 1./(UShort_t)(beta*20000);*/
      Float_t tofbeta = 1./beta;
      Double_t tofm2=mmomentum*mmomentum*( 1.0/(tofbeta*tofbeta)-1.0);
      if(fillhistflag){		   
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
      
        if(kaonEnhCutLow < tofbeta && tofbeta < kaonEnhCutHigh)
        {
          mnSigmaE_KEnh_Pt[trig]->Fill(mpt,nsige);
          mnSigmaK_KEnh_Pt[trig]->Fill(mpt,nsigk);
        }
        if(pionEnhCutLow < tofbeta && tofbeta < pionEnhCutHigh)
        {
          mnSigmaE_PiEnh_Pt [trig]->Fill(mpt,nsige);
          mnSigmaPi_PiEnh_Pt[trig]->Fill(mpt,nsigpi);
        }
        if(protonEnhCutLow < tofbeta && tofbeta < protonEnhCutHigh)
        {
          mnSigmaE_PEnh_Pt[trig]->Fill(mpt,nsige);
          mnSigmaP_PEnh_Pt[trig]->Fill(mpt,nsigp);
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

      double mnsigE=track->nSigmaElectron();


    }//-------tof information end 


    //-------mtd information start-----------

    Int_t mtdpidtraitsid=track->mtdPidTraitsIndex();
    if(mtdpidtraitsid>=0){
      nmtdmatchcount++;
      StPicoMtdPidTraits* mtdpidtraits=(StPicoMtdPidTraits*) mPicoDst->mtdPidTraits(mtdpidtraitsid);
      Int_t mtdbackleg = mtdpidtraits->backleg();
      Int_t mtdmodule = mtdpidtraits->module();
      Int_t mtdcell   = mtdpidtraits->cell();
      Float_t mtddT = mtdpidtraits->deltaTimeOfFlight();
      Float_t mtddz = mtdpidtraits->deltaZ();
      Float_t mtddy = mtdpidtraits->deltaY();
      Float_t mtdbeta = mtdpidtraits->beta();
      Int_t mtdbgcell=mtdbackleg*12+mtdcell;
      Int_t mtdchannel=(mtdbackleg-1)*60+(mtdmodule-1)*12+mtdcell;// (backleg-1) * 60 + (module-1) * 12 + cell
      if(fillhistflag){
        mmtdBeta_Pt[trig]->Fill(mpt,mtdbeta);
        mmtdBeta_channel[trig]->Fill(mtdchannel,mtdbeta);
      }//
    }

    if(Ismuontrack(event, track)) {
      nmuons++; 

    }

    //----------mtd information end------------

    //----------BEMC----------------
    Int_t emcpidtraitsid=track->emcPidTraitsIndex();
    if(emcpidtraitsid>=0){
      nbemcmatchcount++;
      StPicoEmcPidTraits* emcpidtraits=(StPicoEmcPidTraits*) mPicoDst->emcPidTraits(emcpidtraitsid);
      float PovE=mmomentum/emcpidtraits->e();

      if( passEventCuts(event) ){
        if(mpt>1.50&&PovE>0.3&&PovE<1.5){
          nbemcelectron++;		      
        }
      }
    }//end of bemc 




  }//loop of all tracks

  Int_t Nptrks=Nprimarytracks;

  if(fillhistflag){
    //event information QA
    mRanking_nPtrks[trig]->Fill(Nptrks,Ranking);
    mnPtrks_nGtrks[trig]->Fill(NGtrks,Nptrks);
    mnRefMult_nPtrks[trig]->Fill(Nptrks,NRefmultPos+NRefmultNeg);
    mnRefMult_nGtrks[trig]->Fill(NGtrks,NRefmultPos+NRefmultNeg);
    mnGRefMult_nPtrks[trig]->Fill(Nptrks, NGnremult);
    mnGRefMult_nGtrks[trig]->Fill(NGtrks, NGnremult);	   

    mnRefMult_nGRefMult[trig]->Fill(NGnremult,NRefmultPos+NRefmultNeg);
    mnPtrks_nMtdHits[trig]->Fill(Nmtdhits,Nptrks);
    mnPtrks_nTofHits[trig]->Fill(ntofmatchcount,Nptrks);
    mnTofHits_nMtdHits[trig]->Fill(Nmtdhits,ntofmatchcount);                
    mnTofMatch_nTofHits[trig]->Fill(ntofmatchcount,Ntofmatch);  	

    //------------------------------------------------------//
    //---------- make run by run QA of ---------------------//
    //------------------------------------------------------//

    if( passEventCuts(event) ){

      float tofmatchratio= (float)ntofmatchcount/NGnremult;
      float mtdmatchratio=(float)nmtdmatchcount/NGnremult;
      float bemcmatchratio=(float)nbemcmatchcount/NGnremult;
      float hftmatchratio=(float)nhftmatchcount/NGnremult;
      float mtdratio=(float)Nmtdhits/NGnremult;		
      float muonratio=(float)nmuons/NGnremult;
      float toferatio=(float)ntofelecton/NGnremult;
      float bemceratio=(float)nbemcelectron/NGnremult;
      float tofmatchratio2= (float)ntofmatchcount/NRefmult;
      float mtdmatchratio2=(float)nmtdmatchcount/NRefmult;
      float bemcmatchratio2=(float)nbemcmatchcount/NRefmult;
      float hftmatchratio2=(float)nhftmatchcount/NRefmult;
      float mtdratio2=(float)Nmtdhits/NRefmult;		
      float muonratio2=(float)nmuons/NRefmult;
      float toferatio2=(float)ntofelecton/NRefmult;
      float bemceratio2=(float)nbemcelectron/NRefmult;
    }


  }//


  //  mPicoDst->Print();
  //  mPicoDst->printTracks();

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

Bool_t StPicoElecPurityMaker::passGoodTrack(StPicoEvent* event, StPicoTrack* track)
{
  double fithitfrac, chargeq, fhitsdEdx, fhitsFit,feta; 
  double pt = track->gMom(event->primaryVertex(),event->bField()).perp();
  feta=track->gMom(event->primaryVertex(),event->bField()).pseudoRapidity();
  fhitsFit = track->nHitsFit();
  fithitfrac=fhitsFit/track->nHitsMax();
  fhitsdEdx = track->nHitsDedx();
  chargeq=track->charge();
  if(pt>0.2 && fhitsFit >= nhitsFitCut && fhitsdEdx >= nhitsdEdxCut && fithitfrac >= nhitsRatioCut && fabs(chargeq)>0 && fabs(feta) <= etaCut) return true;
  else return false;
}


//------------------------------------------------------------- 

Bool_t StPicoElecPurityMaker::passEIDCuts(StPicoEvent* event, StPicoTrack* track)
{
  double mpt, mdca, mpoe;

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

  // Get PoE info
  Int_t emcpidtraitsid=track->emcPidTraitsIndex();
  if(emcpidtraitsid>=0){
    StPicoEmcPidTraits* emcpidtraits=(StPicoEmcPidTraits*) mPicoDst->emcPidTraits(emcpidtraitsid);

    mpoe = track->gMom(event->primaryVertex(),event->bField()).mag()/emcpidtraits->e();
  }
  else mpoe = 0.0; // if no BEMC, set value = 0

  mpt  = track->gMom(event->primaryVertex(),event->bField()).perp();
  mdca = dcamag;

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

  if( mpt > ePtCut && mdca < dcaCut && mpoe > poeCutLow && mpoe < poeCutHigh && invBeta < tofInvBetaCut && tofMatchFlag > 0 && fabs(toflocaly) < toflocalyCut)
    return true;
  else
    return false;
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
Bool_t StPicoElecPurityMaker::passEventCuts(StPicoEvent* event)
{
  Double_t vzvpd = event->vzVpd();
  Double_t vztpc = event->primaryVertex().z();
  Double_t dvz = vzvpd - vztpc;
  if(fabs(vztpc) < vZcut && fabs(dvz) < dvZcut) return true;
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
  setvZCuts(6.0,4.0); // (vZ, delVz)
  setPrimaryPtCut(1.5); // pT > 1.5 GeV
  setPrimaryEtaCut(0.7); // |eta| < 0.7
  setPrimaryDCACut(1.5); // eDCA < 1.5 cm
  setNhitsCuts(15.,20.,0.52); // nHitsdEdx >= 15, nHitsFit >= 20, nHitsRatio >= 0.52
  setPoECut(0.3, 1.5); // 0.3 < p/E < 1.5
  setToFBetaCut(0.3); // 1/B -1 < 0.3
  setToFLocalyCut(1.8); // |tof_localy| < 1.8
  setKaonEnhCut(0.9,1.1); // 0.1<1/B<1.5 Kaon Species Select
  setPionEnhCut(0.9,1.1); // 0.1<1/B<1.5 Pion Species Select
  setProtonEnhCut(0.9,1.1); // 0.1<1/B<1.5 Proton Species Select
}

