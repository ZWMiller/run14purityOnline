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

  fout->cd();

  hNEvents   = new TH1F("hNEvents","number of events: 0 for total and 2 for MBs events", 10, 0, 10 );
  htriggerindex =new TH1F("htriggerindex","triggerindex", 25,0,25);
  mVz_vpd     = new TH1F("vz_vpd", "VZ_VPD distribution (cm)",400,-200,200);
  mVz_tpc     = new TH1F("vz_tpc","the Vz_TPC distribution (cm) ",400,-200,200);
  mVz_vpdtpc  = new TH2F("Vz_vpdtpc","VertexZ: VPD VS TPC;TPC;VPD;", 400,-200,200,400,-200,200);
  mdVz        = new TH1F("dVz", "VertexZ 'TPC-VPD' (cm)", 400,-200,200);
  mdVz_tpcVz  = new TH2F("dVz_tpcVz","VertexZ 'TPC-VPD vs TPC' (cm);VZtpc; VZ(tpc-vpd)",400,-200,200,200,-100,100);
  mVxy        = new TH2F("vxy","Vertex: Vy Vs Vx distribution (cm);vx;vy; ",200,-10,10,200,-10,10);
  mVRvsVZ    = new TH2F("VRvsVZ","Vertex: VR vs VZ  (cm);VZ;VR; ", 400, -200, 200,200,0,20);

  mRanking_nPtrks=new TH2F("Ranking_nPtrks","Ranking Vs nPrimaryTracks;nprimarytrks; ranking;",200,0,2000,100,-10,10);
  mnPtrks_nGtrks=new TH2F("nPtrks_nGtrks","nPrimarytrks_nGlobaltrks;nglobaltrks;nprimarytrks;",500,0,5000,200,0,2000);

  mnRefMult_nGRefMult=new TH2F("nRefMult_nGRefMult","nRefMult_nGRefMult;nGRefMult;nRefmult;",200,0,1000,200,0,1000);
  mnRefMult_nPtrks=new TH2F("nRefMult_nPtrks","nRefMult_nPrimarytrks;nprimarytrks;nRefmult;",200,0,2000,200,0,1000);
  mnRefMult_nGtrks=new TH2F("nRefMult_nGtrks","nRefMult_nGlobaltrks;nglobaltrks;nRefmult;",250,0,5000,200,0,1000);

  mnGRefMult_nPtrks=new TH2F("nGRefMult_nPtrks","nGRefMult_nPrimarytrks;nprimarytrks;nGRefmult;",200,0,2000,200,0,1000);
  mnGRefMult_nGtrks=new TH2F("nGRefMult_nGtrks","nGRefMult_nGlobaltrks;nglobaltrks;nGRefmult;",250,0,5000,200,0,1000);

  mnPtrks_nTofHits=new TH2F("nPtrks_nTofHits","nPrimarytrks_nTofHits;ntofhits;nprimarytrks;",250,0,2000,200,0,2000);
  mnPtrks_nMtdHits=new TH2F("nPtrks_nMtdHits","nPrimarytrks_nMtdHits;nmtdhits;nprimarytrks;",50,0,50,500,0,2000);
  mnTofHits_nMtdHits=new TH2F("nTofHits_nMtdHits","nTofHits_nMtdHits;nmtdhits;ntofhits;",50,0,50,200,0,2000);
  mnTofMatch_nTofHits=new TH2F("nTofMatch_nTofHits","nTofMatch_nTofHits;ntofhits;ntofmatch;",300,0,1500,300,0,1500);


  mNptracks    = new TH1F("Nptracks","#primary tracks",200,0,2000);    
  mNgtracks    = new TH1F("Ngtracks","# global tracks ",200,0,5000);    

  mtrketa_pt  = new TH2F("trketa_pt", "trketa_pt; pt; eta;", 200,0,20,200,-2,2);
  mtrkphi_pt  = new TH2F("trkphi_pt","trkphi_pt; pt; phi;",200,0,20,200,0,6.3);

  mtrkpt      = new TH1F("trkpt","the pt distribution of all tracks",200,0,20);
  mtrketa     = new TH1F("trketa","eta ",200,-2,2);
  mtrkphi     = new TH1F("trkphi","the phi distribution of all tracks",200,0,6.3);

  mnsigmaPI_Pt   = new TH2F("nsigmaPI_Pt", "nsigmapion vs Pt of all tracks; Pt; nsigmaPI;",400,0,20,200,-20,20);
  mnsigmaP_Pt   = new TH2F("nsigmaP_Pt", "nsigmaproton vs Pt of all tracks; Pt; nsigmaP;",400,0,20,200,-20,20);
  mnsigmaK_Pt   = new TH2F("nsigmaK_Pt", "nsigmaK vs Pt of all tracks; Pt; nsigmaK;",400,0,20,200,-20,20);
  mnsigmaE_Pt   = new TH2F("nsigmaE_Pt", "nsigmaE vs Pt of all tracks; Pt; nsigmaE;",400,0,20,200,-20,20);

  mnsigmaPI   = new TH1F("nsigmaPI", "nsigmapion of all tracks",200,-20,20);
  mnsigmaK    = new TH1F("nsigmaK", "nsigmaKaon of all tracks",200,-20,20);
  mnsigmaE    = new TH1F("nsigmaE", "nsigmaElectron of all tracks",200,-20,20);
  mnsigmaP    = new TH1F("nsigmaP", "nsigmaProton of all tracks",200,-20.,20.);

  mdedx_Pt        = new TH2F("Dedx_Pt","dedx(keV/cm) vs Pt; Pt; Dedx;",200,-20,20,250,0,10);
  minvsBeta_Pt  = new TH2F("invsBeta_Pt","1/Beta vs Pt; Pt; 1/beta;",300,0,6,200,0.90,1.12);
  mtofM2_Pt     = new TH2F("tofM2_Pt", "tofM2 vs momentum; P; tofM2;",200,0,20,200,-0.5,4.5);


  mdedxvsBeta      = new TH2F("ndedxvsBeta","de/dX vs 1/Beta; ",200,0.90,1.12,250,0,10);
  mnSigmaEvsBeta   = new TH2F("nSigmaEvsBeta","nSigmaE vs 1/Beta; ",200,0.90,1.12,50,-15,15);
  mnSigmaPIvsBeta  = new TH2F("nSigmaPIvsBeta","nSigmaPI vs 1/Beta; ",200,0.90,1.12,50,-15,15);
  mnSigmaKvsBeta   = new TH2F("nSigmaKvsBeta","nSigmaK vs 1/Beta; ",200,0.90,1.12,50,-15,15);
  mnSigmaPvsBeta   = new TH2F("nSigmaPvsBeta","nSigmaP vs 1/Beta; ",200,0.90,1.12,50,-15,15);
  mtofm2vsBeta      = new TH2F("ntofm2vsBeta","tofM2 vs 1/Beta; ",200,0.90,1.12,200,-0.5,4.5);

  mtoftray_localY = new TH2F("toftray_localY","localY VS toftray; tray; localY;",120,0,120,100,-4,4);
  mtoftray_localZ = new TH2F("toftray_localZ","localZ VS toftray; tray; localZ;",120,0,120,100,-4,4);
  //mtofhitPosXYZ   = new TH3F("tofhitPosXYZ","tofhitPosXYZ",400,-200,200,400,-200,200,400,-200,200);
  mtoftray_matchflag = new TH2F("toftray_matchflag","toftray_matchflag;tofTray; tofmatchflag;",120,0,120,4,0,4);
  mtoftray_module = new TH2F("toftray_module","toftray_module;tofTray; module",120,0,120,40,0,40);

  //---------------------mtd------------------------
  mmtdBeta_Pt   = new TH2F("mtdBeta_Pt","mtdBeta VS Pt; Pt (GeV/c); mtdBeta;",200,0,20,200,0.95,1.12);
  mmtdBeta_channel   = new TH2F("mtdBeta_channel","mtdBeta_channel; channel; mtdbeta;",300,0,1800,300,0.95,1.12);

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
  hNEvents->Fill(0);
  if(!event) return kStOK;
  //=================event selection================
  int runId = event->runId();
  for(int i=0;i<mNBadRuns;i++){
    if(runId==mBadRuns[i]) return kStOK;
  }

  if( ! event->isMinBias()) return kStOK; // If not MinBias, get out of loop
  //if( ! isBHT1( event ) ) return kStOK;

  //=================event selection================

  if(! passEventCuts(event) ) return kStOK;
  hNEvents->Fill(2);

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
    if( triggerWORD>>i & 0x1 ) htriggerindex->Fill(i);

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
    mVz_tpc->Fill(vztpc);
    mVz_vpd->Fill(vzvpd);
    mdVz->Fill(dvz);  
    mVz_vpdtpc->Fill(vztpc,vzvpd);
    mdVz_tpcVz->Fill(vztpc,dvz);
    mVxy->Fill( event->primaryVertex().x(), event->primaryVertex().y() );
    mVRvsVZ->Fill(vztpc, vr);
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
    mNptracks->Fill(numberoftracks);
    mNgtracks->Fill(NGtrks);
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
    if(!track->isHFTTrack() || !passGoodTrack(track) || !passEIDCuts(event, track)) continue;

    Double_t meta,mpt,mphi,mcharge,mdedx;
    //change to global track
    meta=track->pMom().pseudoRapidity();
    if(track->pMom().mag()!=0) Nprimarytracks++;
    mphi=RotatePhi(track->pMom().phi());
    mpt=track->pMom().perp();
    mcharge=track->charge();
    mdedx=track->dEdx();
    if(mcharge==0||meta==0||mphi==0||mdedx==0) continue;
    if(track->isHFTTrack()){
      nhftmatchcount++;
    }

    Float_t mmomentum=track->pMom().mag();
    Double_t rationhits = 0.00;
    rationhits=(double)track->nHitsFit()/track->nHitsMax();
    // cout<<"track->nHitsMax()="<<track->nHitsMax()<<"track->nHitsFit()"<<track->nHitsFit()<<endl;
    // cout<<"ratiohits"<<rationhits<<endl;
    Double_t nsigpi=track->nSigmaPion();
    Double_t nsigk=track->nSigmaKaon();
    Double_t nsigp=track->nSigmaProton();	     
    if(fillhistflag){
      mtrkpt->Fill( mpt );
      mtrketa->Fill( meta );
      mtrkphi->Fill( mphi );
      //   mtrketaphi->Fill( mphi,meta );
      mnsigmaPI->Fill( track->nSigmaPion() );
      mnsigmaP->Fill(  track->nSigmaProton() );
      mnsigmaK->Fill(  track->nSigmaKaon() );
      mnsigmaE->Fill(  track->nSigmaElectron() );

      mtrketa_pt->Fill(mpt*mcharge,meta);
      mtrkphi_pt->Fill(mpt*mcharge,mphi);

      mdedx_Pt->Fill(mpt*mcharge,track->dEdx());
      mnsigmaPI_Pt->Fill(mpt,track->nSigmaPion());
      mnsigmaP_Pt->Fill(mpt,track->nSigmaProton());
      mnsigmaE_Pt->Fill(mpt,track->nSigmaElectron());
      mnsigmaK_Pt->Fill(mpt,track->nSigmaKaon());

    }

    //------tpc information end-----


    Int_t tofpidid=track->bTofPidTraitsIndex();
    if(tofpidid>0){
      ntofmatchcount++;
      StPicoBTofPidTraits* btofpidtrait=(StPicoBTofPidTraits*) mPicoDst->btofPidTraits(tofpidid);

      //------tof information start----------
      Float_t tofbeta=btofpidtrait->btofBeta();
      Double_t tofm2=mmomentum*mmomentum*( 1.0/(tofbeta*tofbeta)-1.0);
      if(fillhistflag){		   
        minvsBeta_Pt->Fill(mpt,1/tofbeta);
        if(tofbeta>0){
          mtofM2_Pt->Fill(mpt,tofm2);
        }

        // For Purity

        Float_t tofbeta=btofpidtrait->btofBeta();
        mdedxvsBeta    ->Fill(tofbeta, track->dEdx());
        mnSigmaEvsBeta ->Fill(tofbeta, track->nSigmaElectron());
        mnSigmaPIvsBeta->Fill(tofbeta, track->nSigmaPion());
        mnSigmaKvsBeta ->Fill(tofbeta, track->nSigmaKaon());
        mnSigmaPvsBeta ->Fill(tofbeta, track->nSigmaProton());
        mtofm2vsBeta   ->Fill(tofbeta, tofm2);
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
        mtoftray_localY->Fill(toftray,toflocaly);
        mtoftray_localZ->Fill(toftray,toflocalz);
        mtoftray_matchflag->Fill(toftray,btofpidtrait->btofMatchFlag());
        mtoftray_module->Fill(toftray,tofmodule);

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
        mmtdBeta_Pt->Fill(mpt,mtdbeta);
        mmtdBeta_channel->Fill(mtdchannel,mtdbeta);
      }//
    }

    if(Ismuontrack(track)) {
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
    mRanking_nPtrks->Fill(Nptrks,Ranking);
    mnPtrks_nGtrks->Fill(NGtrks,Nptrks);
    mnRefMult_nPtrks->Fill(Nptrks,NRefmultPos+NRefmultNeg);
    mnRefMult_nGtrks->Fill(NGtrks,NRefmultPos+NRefmultNeg);
    mnGRefMult_nPtrks->Fill(Nptrks, NGnremult);
    mnGRefMult_nGtrks->Fill(NGtrks, NGnremult);	   

    mnRefMult_nGRefMult->Fill(NGnremult,NRefmultPos+NRefmultNeg);
    mnPtrks_nMtdHits->Fill(Nmtdhits,Nptrks);
    mnPtrks_nTofHits->Fill(ntofmatchcount,Nptrks);
    mnTofHits_nMtdHits->Fill(Nmtdhits,ntofmatchcount);                
    mnTofMatch_nTofHits->Fill(ntofmatchcount,Ntofmatch);  	

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
}//end of main fucntion







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

Bool_t StPicoElecPurityMaker::passGoodTrack(StPicoTrack* track)
{
  double fithitfrac, chargeq, fhitsdEdx, fhitsFit,feta; 
  double pt = track->pMom().perp();
  feta=track->pMom().pseudoRapidity();
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

    mpoe = track->pMom().mag()/emcpidtraits->e();
  }
  else mpoe = 0.0; // if no BEMC, set value = 0

  mpt  = track->pMom().perp();
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

Bool_t StPicoElecPurityMaker::Ismuontrack(StPicoTrack* track)
{
  double pt;
  pt=track->pMom().perp();
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
}

