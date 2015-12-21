#ifndef StPicoElecPurityMaker_h
#define StPicoElecPurityMaker_h
#include "StRoot/StPicoDstMaker/StPicoTrack.h"
#include "StMaker.h"
class StPicoDst;
class StPicoEvent;
class StPicoDstMaker;
class StPicoMtdHit;
class StPicoTrack;
class TString;
class TH1F;
class TH2F;
class TH3F;
class TFile;
class StPicoElecPurityMaker : public StMaker {
 public:
     StPicoElecPurityMaker(const char *name, StPicoDstMaker *picoMaker, const char *outName);
     virtual ~StPicoElecPurityMaker();
     Bool_t passGoodTrack(StPicoTrack* ); // ZWM
     Bool_t passEIDCuts(StPicoEvent*, StPicoTrack* );   // ZWM
     Bool_t Ismuontrack(StPicoTrack* );
     Bool_t IspassTOFcuts(StPicoTrack*);
     Bool_t IspassBEMCcuts(StPicoTrack*);
     Bool_t passEventCuts(StPicoEvent*);  // ZWM
     Bool_t isBHT1(StPicoEvent*);
     Bool_t isBHT2(StPicoEvent*);
     Bool_t isBHT3(StPicoEvent*);
     Double_t RotatePhi(Double_t phi) const;   
     //virtual Int_t Init();
     //virtual Int_t Make();
     //virtual void  Clear(Option_t *opt="");
     //virtual Int_t Finish();
     Int_t Init();
     Int_t Make();
     void  Clear(Option_t *opt="");
     Int_t Finish();

     void    DeclareHistograms();
     void    WriteHistograms();

     void  SetDefaultCuts();               // ZWM

      // ZWM Functions
     void   setNSigECuts(float l, float h)  { nSigELow = l;  nSigEHigh = h; };
     void   setNSigPCuts(float l, float h)  { nSigPLow = l;  nSigPHigh = h; };
     void   setNSigKCuts(float l, float h)  { nSigKLow = l;  nSigKHigh = h; };
     void   setNSigPiCuts(float l, float h) { nSigPiLow = l; nSigEHigh = h; };
     void   setvZCuts(float vz, float dvz)  { vZcut = vz; dvZcut = dvz; };
     void   setPrimaryPtCut(float pt)       { ePtCut = pt; };
     void   setPrimaryEtaCut(float et)      { etaCut = et; };
     void   setPrimaryDCACut(float dca)     { dcaCut = dca; };
     void   setNhitsCuts(float dedx, float fit, float ratio) 
        { nhitsdEdxCut = dedx; nhitsFitCut = fit; nhitsRatioCut = ratio; };
     void   setPoECut(float pEl, float pEh) { poeCutLow = pEl; poeCutHigh = pEh; };
     void   setToFBetaCut(float iB)         { tofInvBetaCut = iB; };
     void   setToFLocalyCut(float lY)       { toflocalyCut = lY; };

  private:
   StPicoDstMaker *mPicoDstMaker;
   StPicoDst      *mPicoDst;
   
   // Event cut vars
   float vZcut;
   float dvZcut;
   // Track cut vars
   float ePtCut, etaCut, dcaCut;
   float nhitsdEdxCut, nhitsFitCut, nhitsRatioCut;
   float poeCutLow, poeCutHigh;
   float tofInvBetaCut,toflocalyCut;
   
   float nSigELow, nSigEHigh;
   float nSigPLow, nSigPHigh;
   float nSigKLow, nSigKHigh;
   float nSigPiLow, nSigPiHigh;
   TString    mOutName;

     Int_t   mNBadRuns;       

    
   TFile*	   fout;
    //-----event QA-----


    TH1F*      hNEvents;
    TH1F*    	htriggerindex;
    TH1F*      mVz_vpd;
    TH1F*      mVz_tpc;
    TH2F*      mVz_vpdtpc;
    TH1F*      mdVz;
    TH2F*      mdVz_tpcVz;
    TH2F*      mVxy;
    TH2F*      mVRvsVZ;

    TH2F*      mRanking_nPtrks;
    TH2F*      mnPtrks_nGtrks;
    TH2F*      mnRefMult_nGRefMult;
    TH2F*      mnRefMult_nPtrks;
    TH2F*      mnRefMult_nGtrks;
    TH2F*      mnGRefMult_nPtrks;
    TH2F*      mnGRefMult_nGtrks;

    TH2F*      mnPtrks_nTofHits;
    TH2F*      mnPtrks_nMtdHits;

    TH2F*      mnTofHits_nMtdHits;
    TH2F*      mnTofMatch_nTofHits;
    

    //---------Track QA----------
    //-------TPC information------
    TH1F*      mNptracks;
    TH1F*      mNgtracks;
    TH1F*      mtrkpt;
    TH1F*      mtrkphi;
    TH1F*      mtrketa;
    TH1F*      mnsigmaK;
    TH1F*      mnsigmaE;
    TH1F*      mnsigmaP;
    TH1F*      mnsigmaPI;

    TH2F*      mtrketaphi;
    TH2F*      mtrketa_pt;
    TH2F*      mtrkphi_pt;
    TH2F*      mdedx_Pt;

    TH2F*      mnsigmaPI_Pt;
    TH2F*      mnsigmaP_Pt;
    TH2F*      mnsigmaK_Pt;
    TH2F*      mnsigmaE_Pt;
    //----TPC information end-----

    //-----TOF INFORMATION start----
    TH2F*      minvsBeta_Pt;
    TH2F*      mtofM2_Pt;
    //        mp vs nsigmaE
    TH2F*      mtoftray_localY;
    TH2F*      mtoftray_localZ;
    //TH3F*      mtofhitPosXYZ;
    TH2F*      mtoftray_matchflag;
    TH2F*      mtoftray_module;

    //----TOF information end----
    
    //---------mtd information-------------
    TH2F*      mmtdBeta_Pt;
    TH2F*      mmtdBeta_channel;
 
    // For Purity
    TH2F*      mnSigmaEvsBeta;
    TH2F*      mnSigmaPIvsBeta;
    TH2F*      mnSigmaKvsBeta;
    TH2F*      mnSigmaPvsBeta;
    TH2F*      mdedxvsBeta;
    TH2F*      mtofm2vsBeta;
	
    ClassDef(StPicoElecPurityMaker, 1)
};

#endif
