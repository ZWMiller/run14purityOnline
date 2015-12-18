#ifndef StMuMuPair_hh
#define StMuMuPair_hh

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

class StMuMuPair : public TObject {
 public:
  StMuMuPair();
  ~StMuMuPair();
	StMuMuPair(UShort_t dauId1, UShort_t dauId2, UShort_t dauIndex1, UShort_t dauIndex2, Char_t dauCharge1, Char_t dauCharge2,
//        Float_t dauPt1, Float_t dauPt2, Float_t dauEta1, Float_t dauEta2, Float_t dauPhi1, Float_t dauPhi2,
//        Float_t  daunSigPi1, Float_t  daunSigPi2, Float_t  daudT1, Float_t  daudT2, Float_t daudZ1, Float_t daudZ2, Char_t isHft1, Char_t isHft2,
//        Float_t  dauDcaToVtx1, Float_t  dauDcaToVtx2, 
        Float_t  dauDcaDist,
        Float_t  pairDca, Float_t  pairDecayL, Float_t  pairY, Float_t  cosThetaStar, Float_t  pointingAngle, 
		Float_t  pairPt, Float_t  pairEta, Float_t  pairPhi, Float_t  pairMass, 
		Float_t  pairPPt, Float_t  pairPEta, Float_t  pairPPhi, Float_t  pairPMass, 
		Float_t pairCtau, Float_t pairOx, Float_t pairOy, Float_t pairOz

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
  Char_t   dauCharge1()   const       { return (Char_t)mDauCharge1; }
  Char_t   dauCharge2()   const       { return (Char_t)mDauCharge2; }
//  Char_t   dauIsHft1()    const       { return (Char_t)mDauIsHft1;}
//  Char_t   dauIsHft2()    const       { return (Char_t)mDauIsHft2;}
//  Float_t  dauDcaToVtx1()      const       { return (Float_t)mDauDcaToVtx1; }
//  Float_t  dauDcaToVtx2()      const       { return (Float_t)mDauDcaToVtx2; }
  Float_t  dauDcaDist()      const          { return (Float_t)mDauDcaDist; }
  Float_t  pairPt() const			  {return mPairPt;}
  Float_t  pairMass() const           { return mPairMass;}
  Float_t  pairEta() const              { return mPairEta;}
  Float_t  pairY() const              { return mPairY;}
  Float_t  pairPPt() const			  {return mPairPPt;}
  Float_t  pairPMass() const           { return mPairPMass;}
  Float_t  pairPEta() const              { return mPairPEta;}
  Float_t  pairPPhi() const              { return mPairPPhi;}
  Float_t  pairPY() const;

  StThreeVectorF pairMom() const;
  StThreeVectorF pairOrigin() const;
  Float_t  pairDca()      const       { return (Float_t)mPairDca; }
  Float_t  cosThetaStar() const              { return mCosThetaStar;}
  Float_t  pointingAngle() const              { return mPointingAngle;}
  Float_t  pairDecayL()      const       { return (Float_t)mPairDecayL; }
  Float_t  pairCtau()      const       { return (Float_t)mPairCtau; }




 protected:
  UShort_t mDauId1;
  UShort_t mDauId2;
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
//
//  Float_t  mDaudT1;
//  Float_t  mDaudT2;
//  Float_t  mDaunSigPi1;
//  Float_t  mDaunSigPi2;
//  Float_t  mDaudZ1;
//  Float_t  mDaudZ2;
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
  Float_t  mPairCtau;
  Float_t  mPairOx;
  Float_t  mPairOy;
  Float_t  mPairOz;

  friend class StPicoDst;

  ClassDef(StMuMuPair, 1)
};

inline StThreeVectorF StMuMuPair::pairMom() const
{
    TVector3 m(0,0,0);
    m.SetPtEtaPhi(mPairPt,mPairEta,mPairPhi);
  return StThreeVectorF(m.X(),m.Y(),m.Z());
}
inline StThreeVectorF StMuMuPair::pairOrigin() const
{
  return StThreeVectorF(mPairOx,mPairOy,mPairOz);
}
inline Float_t StMuMuPair::pairPY() const
{

	TLorentzVector mumu(0,0,0,0);
	mumu.SetPtEtaPhiM(pairPPt(),pairPEta(),pairPPhi(),pairPMass());
	return mumu.Rapidity();
}

#endif
