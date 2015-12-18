#include "StMuMuPair.h"
#include "StElectronTrack.h"
#include "TVector2.h"
#include "TRandom.h"
#include "StMessMgr.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#define ElectronMass 0.000510999

ClassImp(StMuMuPair)

    //----------------------------------------------------------------------------------

StMuMuPair::StMuMuPair()
    : mDauId1(0), mDauId2(0), mDauIndex1(0), mDauIndex2(0), mDauCharge1(0), mDauCharge2(0), 
//    mDauPt1(0),mDauPt2(0),mDauEta1(0),mDauEta2(0),mDauPhi1(0),mDauPhi2(0),
//    mDaudT1(0), mDaudT2(0), mDaunSigPi1(-999.), mDaunSigPi2(-999), mDaudZ1(-999), mDaudZ2(-999), mDauIsHft1(0), mDauIsHft2(0),
//    mDauDcaToVtx1(0),mDauDcaToVtx2(0),
	mDauDcaDist(0),
    mPairDca(0),mPairDecayL(0),mPairY(-999),mCosThetaStar(-999.),mPointingAngle(-999.),
mPairPt(0),mPairEta(0),mPairPhi(0),mPairMass(-999.), 
mPairPPt(0),mPairPEta(0),mPairPPhi(0),mPairPMass(-999.), 
	mPairCtau(-999.), mPairOx(-999.), mPairOy(-999.), mPairOz(-999.)
{

}

/////////////////////////////////////////////////////////////////////////////////////////
// t - the global track.  p - the associated primary track from the first primary vertex
/////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------
StMuMuPair::StMuMuPair(UShort_t dauId1, UShort_t dauId2, UShort_t dauIndex1, UShort_t dauIndex2, Char_t dauCharge1, Char_t dauCharge2,
//        Float_t dauPt1, Float_t dauPt2, Float_t dauEta1, Float_t dauEta2, Float_t dauPhi1, Float_t dauPhi2,
//        Float_t daudT1, Float_t  daudT2, Float_t  daunSigPi1, Float_t  daunSigPi2, Float_t  daudZ1, Float_t  daudZ2, Char_t isHft1, Char_t isHft2,
//        Float_t  dauDcaToVtx1, Float_t  dauDcaToVtx2, 
		Float_t  dauDcaDist,
        Float_t  pairDca, Float_t  pairDecayL, Float_t  pairY, Float_t  cosThetaStar, Float_t  pointingAngle, Float_t  pairPt, Float_t  pairEta, Float_t  pairPhi, Float_t  pairMass, 
		Float_t  pairPPt, Float_t  pairPEta, Float_t  pairPPhi, Float_t  pairPMass, 
		Float_t pairCtau,Float_t pairOx, Float_t pairOy, Float_t pairOz
        )
    : mDauId1(dauId1), mDauId2(dauId2), mDauIndex1(dauIndex1), mDauIndex2(dauIndex2), mDauCharge1(dauCharge1), mDauCharge2(dauCharge2), 
//    mDauPt1(dauPt1),mDauPt2(dauPt2),mDauEta1(dauEta1),mDauEta2(dauEta2),mDauPhi1(dauPhi1),mDauPhi2(dauPhi2),
//    mDaudT1(daudT1), mDaudT2(daudT2), mDaunSigPi1(daunSigPi1), mDaunSigPi2(daunSigPi2), mDaudZ1(daudZ1), mDaudZ2(daudZ2), mDauIsHft1(isHft1), mDauIsHft2(isHft2),
//    mDauDcaToVtx1(dauDcaToVtx1),mDauDcaToVtx2(dauDcaToVtx2),
	mDauDcaDist(dauDcaDist),
    mPairDca(pairDca),mPairDecayL(pairDecayL),mPairY(pairY),mCosThetaStar(cosThetaStar),mPointingAngle(pointingAngle),
mPairPt(pairPt),mPairEta(pairEta),mPairPhi(pairPhi),mPairMass(pairMass),
mPairPPt(pairPPt),mPairPEta(pairPEta),mPairPPhi(pairPPhi),mPairPMass(pairPMass),
	mPairCtau(pairCtau), mPairOx(pairOx), mPairOy(pairOy), mPairOz(pairOz)
{
}

//----------------------------------------------------------------------------------
StMuMuPair::~StMuMuPair()
{
    /* noop */
}
//----------------------------------------------------------------------------------
void StMuMuPair::Print(const Char_t *option) const
{
    if (strcmp(option, "tpc") == 0 || strcmp(option, "") == 0)
    {
        LOG_INFO << "dau1 id=" << dauId1()<<" dau2 id = "<<dauId2()
            << endm;
        LOG_INFO << "mom=" << pairMom() << " mass = "<<pairMass()<<endm;
    }
}
