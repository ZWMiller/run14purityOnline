#ifndef StEMuPair_hh
#define StEMuPair_hh

#include <cmath>

class StPicoTrack;
class StPicoDst;
class StDcaGeometry;
class StElectronTrack;

#include "TObject.h"
#include "StThreeVectorF.hh"
#include "TLorentzVector.h"
#include "TVector3.h"
#include <stdio.h>
#include <math.h>
#include "StEvent/StDcaGeometry.h"

class StEMuPair : public TObject {
 public:
  StEMuPair();
  ~StEMuPair();
	StEMuPair(UShort_t dauId1, UShort_t dauId2, UShort_t dauIndex1, UShort_t dauIndex2, Char_t dauCharge1, Char_t dauCharge2,
//        Float_t dauPt1, Float_t dauPt2, Float_t dauEta1, Float_t dauEta2, Float_t dauPhi1, Float_t dauPhi2,
 //       Float_t dauInvBeta1, Float_t  daudT2, Float_t  daunSigE1, Float_t  daunSigPi2, Float_t  dauPvE1, Float_t  daudZ2, Char_t   dauIsBsmd1, Char_t isHft1, Char_t isHft2,
//        Float_t  dauDcaToVtx1, Float_t  dauDcaToVtx2, 
        Float_t  dauDcaDist,
        Float_t  pairDca, Float_t  pairDecayL, Float_t  pairY, Float_t  cosThetaStar, Float_t  pointingAngle, 
		Float_t  pairPt, Float_t  pairEta, Float_t  pairPhi, Float_t  pairMass,
		Float_t  pairPPt, Float_t  pairPEta, Float_t  pairPPhi, Float_t  pairPMass
        );

  virtual void Print(const Char_t *option = "") const;  ///< Print track info
            
  Int_t   dauId1() const             { return (Int_t)mDauId1; }
  Int_t   dauId2() const             { return (Int_t)mDauId2; }
  Int_t   dauIndex1() const             { return (Int_t)mDauIndex1; }
  Int_t   dauIndex2() const             { return (Int_t)mDauIndex2; }
//  Float_t  dauPt1()      const          { return (Float_t)mDauPt1; }
//  Float_t  dauPt2()      const          { return (Float_t)mDauPt2; }
//  Float_t  dauEta1()      const          { return (Float_t)mDauEta1; }
//  Float_t  dauEta2()      const          { return (Float_t)mDauEta2; }
  Float_t  dauDcaDist()      const          { return (Float_t)mDauDcaDist; }
  Char_t   dauCharge1()   const       { return (Char_t)mDauCharge1; }
  Char_t   dauCharge2()   const       { return (Char_t)mDauCharge2; }
//  Char_t   dauIsHft1()    const       { return (Char_t)mDauIsHft1;}
//  Char_t   dauIsHft2()    const       { return (Char_t)mDauIsHft2;}
//  Float_t  dauDcaToVtx1()      const       { return (Float_t)mDauDcaToVtx1; }
//  Float_t  dauDcaToVtx2()      const       { return (Float_t)mDauDcaToVtx2; }
  Float_t  pairPt() const;
  Float_t  pairMass() const           { return mPairMass;}
  Float_t  pairY() const              { return mPairY;}
  Float_t  pairEta() const              { return mPairEta;}

  Float_t  pairPPt() const			{return mPairPPt;}
  Float_t  pairPEta() const              { return mPairPEta;}
  Float_t  pairPPhi() const              { return mPairPPhi;}
  Float_t  pairPMass() const              { return mPairPMass;}
  Float_t  pairPY() const;

  StThreeVectorF pairMom() const;
  Float_t  pairDca()      const       { return (Float_t)mPairDca; }
  Float_t  cosThetaStar() const              { return mCosThetaStar;}
  Float_t  pointingAngle() const              { return mPointingAngle;}
  Float_t  pairDecayL()      const       { return (Float_t)mPairDecayL; } 

 protected:
  UShort_t mDauId1; //e 
  UShort_t mDauId2; //mu
  UShort_t mDauIndex1;
  UShort_t mDauIndex2;
  Char_t   mDauCharge1;
  Char_t   mDauCharge2;

//  Float_t  mDauPt1;
//  Float_t  mDauPt2;
//  Float_t  mDauEta1;
//  Float_t  mDauEta2;
//  Float_t  mDauPhi1;
//  Float_t  mDauPhi2;

//  Float_t  mDauInvBeta1;
//  Float_t  mDaudT2;
//  Float_t  mDaunSigE1;
//  Float_t  mDaunSigPi2;
//  Float_t  mDauPvE1;
//  Float_t  mDaudZ2;
//  Char_t   mDauIsBsmd1;

//  Char_t   mDauIsHft1;
//  Char_t   mDauIsHft2;

//  Float_t  mDauDcaToVtx1;
//  Float_t  mDauDcaToVtx2;
  Float_t  mDauDcaDist;

  Float_t  mPairDca;
  Float_t  mPairDecayL; // decay length
  Float_t  mPairY;
  Float_t  mCosThetaStar;
  Float_t  mPointingAngle;
  Float_t  mPairPt;
  Float_t  mPairEta;
  Float_t  mPairPhi;
  Float_t  mPairMass;
  Float_t  mPairPPt;
  Float_t  mPairPEta;
  Float_t  mPairPPhi;
  Float_t  mPairPMass;

  friend class StPicoDst;

  ClassDef(StEMuPair, 1)
};

inline Float_t StEMuPair::pairPt() const
{
    return mPairPt;
}

inline StThreeVectorF StEMuPair::pairMom() const
{
    TVector3 m(0,0,0);
    m.SetPtEtaPhi(mPairPt,mPairEta,mPairPhi);
  return StThreeVectorF(m.X(),m.Y(),m.Z());
}

inline Float_t StEMuPair::pairPY() const
{

	TLorentzVector emu(0,0,0,0);
	emu.SetPtEtaPhiM(pairPPt(),pairPEta(),pairPPhi(),pairPMass());
	return emu.Rapidity();
}

#endif
