#ifndef StHadronTrack_hh
#define StHadronTrack_hh

#include <cmath>

class StPicoTrack;
class StPicoDst;
class StPicoBTofPidTraits;
class StPicoEmcPidTraits;
class StDcaGeometry;

#include "TObject.h"
#include "StThreeVectorF.hh"
#include "TVector2.h"
#include <stdio.h>
#include <math.h>
#include "StEvent/StDcaGeometry.h"

// Macro to control EMC variables
#define EMCON 1
class StPicoEvent;

class StHadronTrack : public TObject {
 public:
  StHadronTrack();
  ~StHadronTrack();
  StHadronTrack(StPicoDst *picoDst, StPicoTrack *t);
  virtual void Print(const Char_t *option = "") const;  ///< Print track info
            
  Int_t   id() const             { return (Int_t)mId; }
  Float_t chi2() const           { return (Float_t)mChi2/1000.; }
  Float_t gPt() const;
  StThreeVectorF gMom() const;
  StThreeVectorF const & pMom() const    { return mPMomentum; }
  Short_t charge() const         { return (mNHitsFit>0) ? +1 : -1; }
  Int_t   nHitsFit() const       { return (mNHitsFit>0) ? (Int_t)mNHitsFit : (Int_t)(-1*mNHitsFit); }
  Int_t   nHitsDedx() const      { return (Int_t)mNHitsDedx; }
  Int_t   nHitsMax() const       { return (Int_t)mNHitsMax; }
  Int_t   nHitsMapHFT() const    { return (Int_t)(mMap0 >> 1 & 0x7F); }
  UInt_t  map0() const { return (UInt_t)mMap0; }
  UInt_t  map1() const { return (UInt_t)mMap1; }
  Float_t dEdx() const           { return (Float_t)mDedx/1000.; }
  Float_t nSigmaPion() const     { return (Float_t)mNSigmaPion/100.; }
  Float_t nSigmaKaon() const     { return (Float_t)mNSigmaKaon/100.; }
  Float_t nSigmaProton() const   { return (Float_t)mNSigmaProton/100.; }
  Float_t nSigmaElectron() const { return (Float_t)mNSigmaElectron/100.; }
  Float_t dca() const           { return (Float_t)mDca; }
  Float_t dcaXY() const           { return (Float_t)mDcaXY; }
  Float_t dcaZ() const           { return (Float_t)mDcaZ; }

  const Float_t* params() const     { return mPar; }
  const Float_t* errMatrix() const  { return mErrMatrix; }

  StDcaGeometry dcaGeometry() const;
  StPhysicalHelixD helix() const;
  Bool_t isHFTTrack() const { return (nHitsMapHFT()>>0 & 0x1) && (nHitsMapHFT()>>1 & 0x3) && (nHitsMapHFT()>>3 & 0x3); }
          

 protected:
  UShort_t mId;               // track Id
  UShort_t mChi2;             // chi2*1000
  StThreeVectorF mPMomentum;  // primary momentum, (0.,0.,0.) if none
  UShort_t mDedx;             // dEdx*1000
  Float_t  mDca;              // dca
  Float_t  mDcaXY;            // dcaXY
  Float_t  mDcaZ;             // dcaZ
  Char_t   mNHitsFit;         // q*nHitsFit
  Char_t   mNHitsMax;         // nHitsMax - TPC
  UChar_t  mNHitsDedx;        // nHitsDedx
  Short_t  mNSigmaPion;       // nsigmaPi * 100
  Short_t  mNSigmaKaon;       // nsigmaK * 100
  Short_t  mNSigmaProton;     // nsigmaP * 100
  Short_t  mNSigmaElectron;   // nsigmaE * 100
  UChar_t  mNHitsMapHFT;      // the hit map in all HFT layers
  UInt_t   mMap0;             // TopologyMap data0
  UInt_t   mMap1;             // TopologyMap data1
  
  // a copy of the StMuTrack::dcaGeometry() parameters
  Float_t  mPar[6];                                            
  Float_t  mErrMatrix[15];
 
  // pidTraits
  Char_t   mTofMatchFlag;
  Float_t  mBeta;
  Float_t  mMassSquare;
  Float_t  mLocalY;
  Float_t  mLocalZ;

  // these variables are extracted from the standard BEMC cluster algorithm
  Short_t  mBEMCId;           // index in bemcPoint array
  Short_t  mBTOWADC0;         // adc0 higest adc in the cluster
  Short_t  mBTOWE0;           // E0*1000 highest tower in the cluster
  Short_t  mBTOWE;            // EMC point E*1000 
  Short_t  mBEMCDistZ;        // z*100
  Short_t  mBEMCDistPhi;      // phi*10000
  UChar_t  mBSMDNEta;         // # of hits in eta
  UChar_t  mBSMDNPhi;         // # of hits in phi

  // these variables are purely from single tower or nearby towers  
  Short_t  mBTOWId;           // projected tower Id 1-4800
  Char_t   mBTOWId23;         // emc 2nd and 3rd closest tower local id  ( 2nd X 10 + 3rd), each id 0-8
  Short_t  mBTOWE1;           // E1*1000 matched (closest) tower E
  Short_t  mBTOWE2;           // E2*1000 2nd closest tower E
  Short_t  mBTOWE3;           // E3*1000 3rd closest tower E
  Short_t  mBTOWDistEta;      // eta*10000 distance between track and matched tower center
  Short_t  mBTOWDistPhi;      // phi*10000 distance between track and matched tower center

  friend class StPicoDst;

  ClassDef(StHadronTrack, 1)
};
inline Float_t StHadronTrack::gPt() const
{
  return 1./fabs(mPar[3]);
}

inline StThreeVectorF StHadronTrack::gMom() const
{
  float ptt = gPt();
  return StThreeVectorF(ptt*std::cos(mPar[2]),ptt*std::sin(mPar[2]),ptt*mPar[4]);
}

inline StDcaGeometry StHadronTrack::dcaGeometry() const
{
  StDcaGeometry a;
  a.set(mPar, mErrMatrix);
  return a;
}
      
inline StPhysicalHelixD StHadronTrack::helix() const
{
  return dcaGeometry().helix();
}        
#endif
