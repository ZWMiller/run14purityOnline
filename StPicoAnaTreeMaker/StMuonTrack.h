#ifndef StMuonTrack_hh
#define StMuonTrack_hh

#include <cmath>

class StPicoTrack;
class StPicoDst;
class StPicoMtdPidTraits;
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

class StMuonTrack : public TObject {
 public:
  StMuonTrack();
  ~StMuonTrack();
  StMuonTrack(StPicoDst *picoDst, StPicoTrack *t);
  virtual void Print(const Char_t *option = "") const;  ///< Print track info
            
  Int_t   id() const             { return (Int_t)mId; }
  Float_t chi2() const           { return (Float_t)mChi2/1000.; }
  Float_t gPt() const;
  Float_t gEta() const;
  Float_t gPhi() const;
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
  Int_t matchFlag() const      { return (Int_t)mMatchFlag;}
  Float_t deltaTimeOfFlight() const { return mdT;}
  Float_t deltaZ()  const       { return mdZ;}
  Int_t  triggerFlag()  const       { return mTriggerFlag;}
  Int_t  channel()  const       { return mChannel;} //
  Int_t  backleg()  const       { return mChannel/60+1;} //1-30
  Int_t  module()  const       { return (mChannel%60)/12+1;} //1-5
  Int_t  cell()  const       { return (mChannel%60)%12;} //0-11


//  const Float_t* params() const     { return mPar; }
//  const Float_t* errMatrix() const  { return mErrMatrix; }

//  StDcaGeometry dcaGeometry() const;
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
  UInt_t   mMap0;             // TopologyMap data0
  UInt_t   mMap1;             // TopologyMap data1
  
  // a copy of the StMuTrack::dcaGeometry() parameters
  //Float_t  mPar[6];                                            
  //Float_t  mErrMatrix[15];
  
  StThreeVectorF mGMom;
  Float_t   mCurv;
  Float_t	mDip;
  Float_t	mPhase;
  StThreeVectorF mOrigin;
  Char_t	mH;
 
  // pidTraits
  Char_t   mTofMatchFlag;
  Float_t  mBeta;
  Float_t  mMassSquare;
  Float_t  mLocalY;
  Float_t  mLocalZ;
 
  // pidTraits
  Char_t   mMatchFlag;
  Short_t  mChannel;
  Float_t  mdT;
  Float_t  mdY;
  Float_t  mdZ;
  Char_t   mTriggerFlag;

  friend class StAnaTree;

  ClassDef(StMuonTrack, 1)
};
inline Float_t StMuonTrack::gPt() const
{
  //return 1./fabs(mPar[3]);
  return mGMom.perp();
}
inline Float_t StMuonTrack::gEta() const
{

  //float ptt = gPt();
  //StThreeVectorF gmom(ptt*std::cos(mPar[2]),ptt*std::sin(mPar[2]),ptt*mPar[4]);
  //return gmom.pseudoRapidity();
  return mGMom.pseudoRapidity();
}
inline Float_t StMuonTrack::gPhi() const
{
  //float ptt = gPt();
  //StThreeVectorF gmom(ptt*std::cos(mPar[2]),ptt*std::sin(mPar[2]),ptt*mPar[4]);
  //return gmom.phi();
  return mGMom.phi();;
}


inline StThreeVectorF StMuonTrack::gMom() const
{
  //float ptt = gPt();
  //return StThreeVectorF(ptt*std::cos(mPar[2]),ptt*std::sin(mPar[2]),ptt*mPar[4]);
  return mGMom;
}

//inline StDcaGeometry StMuonTrack::dcaGeometry() const
//{
//  StDcaGeometry a;
//  a.set(mPar, mErrMatrix);
//  return a;
//}
      
//inline StPhysicalHelixD StMuonTrack::helix() const
//{
//  return dcaGeometry().helix();
//}        

inline StPhysicalHelixD StMuonTrack::helix() const
{
	  //dcaGeometry().helix();
  	return StPhysicalHelixD(mCurv,mDip,mPhase,mOrigin,mH);
}        

#endif
