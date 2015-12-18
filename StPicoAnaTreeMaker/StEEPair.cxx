#include "StEEPair.h"
#include "StElectronTrack.h"
#include "TVector2.h"
#include "TRandom.h"
#include "StMessMgr.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#define ElectronMass 0.000510999

ClassImp(StEEPair)

    //----------------------------------------------------------------------------------

StEEPair::StEEPair()
    : mDauIndex1(0), mDauIndex2(0), mDauCharge1(0), mDauCharge2(0), 
    //mDauPt1(0),mDauPt2(0),mDauEta1(0),mDauEta2(0),mDauPhi1(0),mDauPhi2(0),
    //mDauInvBeta1(0), mDauInvBeta2(0), mDaunSigE1(-999.), mDaunSigE2(-999), mDauPvE1(-999), mDauPvE2(-999), 
    //mDauIsHft1(0), mDauIsHft2(0), mDauIsBsmd1(0), mDauIsBsmd2(0),
    //mDauDcaToVtx1(0),mDauDcaToVtx2(0),
	mDauDcaDist(0),
    mPairDca(0),mPairDecayL(0),mPairY(-999),mCosThetaStar(-999.),mPointingAngle(-999.),mPairPhiV(-999.),
mPairPt(0),mPairEta(0),mPairPhi(0),mPairMass(-999.),
mPairPPt(0),mPairPEta(0),mPairPPhi(0),mPairPMass(-999.),
	mPairCtau(-999.),mPairOx(-999.),mPairOy(-999.),mPairOz(-999.)
{

}

/////////////////////////////////////////////////////////////////////////////////////////
// t - the global track.  p - the associated primary track from the first primary vertex
/////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------
StEEPair::StEEPair(UShort_t dauIndex1, UShort_t dauIndex2, Char_t dauCharge1, Char_t dauCharge2,
        //Float_t dauPt1, Float_t dauPt2, Float_t dauEta1, Float_t dauEta2, Float_t dauPhi1, Float_t dauPhi2,
        //Float_t dauInvBeta1, Float_t  dauInvBeta2, Float_t  daunSigE1, Float_t  daunSigE2, Float_t  dauPvE1, Float_t  dauPvE2, 
        //Char_t dauIsHft1, Char_t dauIsHft2, Char_t   dauIsBsmd1, Char_t   dauIsBsmd2,
        //Float_t  dauDcaToVtx1, Float_t  dauDcaToVtx2, 
		Float_t  dauDcaDist,
        Float_t  pairDca, Float_t  pairDecayL, Float_t  pairY, Float_t  cosThetaStar, Float_t  pointingAngle, Float_t  pairPhiV, 
		Float_t  pairPt, Float_t  pairEta, Float_t  pairPhi, Float_t  pairMass, 
		Float_t  pairPPt, Float_t  pairPEta, Float_t  pairPPhi, Float_t  pairPMass, 
		Float_t pairCtau, Float_t pairOx, Float_t pairOy, Float_t pairOz
        )
    : mDauIndex1(dauIndex1), mDauIndex2(dauIndex2), mDauCharge1(dauCharge1), mDauCharge2(dauCharge2), 
   // mDauPt1(dauPt1),mDauPt2(dauPt2),mDauEta1(dauEta1),mDauEta2(dauEta2),mDauPhi1(dauPhi1),mDauPhi2(dauPhi2),
    //mDauInvBeta1(dauInvBeta1), mDauInvBeta2(dauInvBeta2), mDaunSigE1(daunSigE1), mDaunSigE2(daunSigE2), mDauPvE1(dauPvE1), mDauPvE2(dauPvE2), 
    //mDauIsHft1(dauIsHft1), mDauIsHft2(dauIsHft2), mDauIsBsmd1(dauIsBsmd1), mDauIsBsmd2(dauIsBsmd2),
    //mDauDcaToVtx1(dauDcaToVtx1*1000),mDauDcaToVtx2(dauDcaToVtx2*1000),
	mDauDcaDist(dauDcaDist*1000),
    mPairDca(pairDca),mPairDecayL(pairDecayL),mPairY(pairY*10000),mCosThetaStar(cosThetaStar*10000),mPointingAngle(pointingAngle*10000),mPairPhiV(pairPhiV*10000),
mPairPt(pairPt),mPairEta(pairEta*10000),mPairPhi(pairPhi*10000),mPairMass(pairMass), 
mPairPPt(pairPPt),mPairPEta(pairPEta*10000),mPairPPhi(pairPPhi*10000),mPairPMass(pairPMass), 
	mPairCtau(pairCtau), mPairOx(pairOx*100), mPairOy(pairOy*100), mPairOz(pairOz*100)
{
}

//----------------------------------------------------------------------------------
StEEPair::~StEEPair()
{
    /* noop */
}
//----------------------------------------------------------------------------------
void StEEPair::Print(const Char_t *option) const
{
    if (strcmp(option, "tpc") == 0 || strcmp(option, "") == 0)
    {
        LOG_INFO << "mom=" << pairMom() << " mass = "<<pairMass()<<endm;
    }
}
