#include "StElectronTrack.h"
#include "StMessMgr.h"
#include "TVector2.h"
#include "TMath.h"
#include "StPicoDstMaker/StPicoTrack.h"
#include "StPicoDstMaker/StPicoEvent.h"
#include "StPicoDstMaker/StPicoDst.h"
#include "StPicoDstMaker/StPicoBTofPidTraits.h"
#include "StPicoDstMaker/StPicoEmcPidTraits.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"

ClassImp(StElectronTrack)

//----------------------------------------------------------------------------------
StElectronTrack::StElectronTrack() : mId(0), mChi2(65536), mPMomentum(0., 0., 0.), mDedx(0),
   mNHitsFit(0), mNHitsMax(0), mNHitsDedx(0), mNSigmaPion(32768), mNSigmaKaon(32768),
   mNSigmaProton(32768), mNSigmaElectron(32768), mMap0(0),
   mMap1(0), mGMom(0.,0.,0.), 
	mCurv(0), mDip(0), mPhase(0), mOrigin(0,0,0), mH(0), 
    mTofMatchFlag(0), mBeta(-999), mMassSquare(0), mLocalY(-999), mLocalZ(-999),
    mBEMCId(-9999), mBTOWADC0(-9999), mBTOWE0(-9999), mBTOWE(-9999),
  mBEMCDistZ(-9999), mBEMCDistPhi(-9999), mBSMDNEta(255), mBSMDNPhi(255),
  mBTOWId(-9999), mBTOWId23(127),
  mBTOWE1(-9999), mBTOWE2(-9999), mBTOWE3(-9999),
  mBTOWDistEta(-9999), mBTOWDistPhi(-9999), mEmcTrgId(-1)
{

}

/////////////////////////////////////////////////////////////////////////////////////////
// t - the global track.  p - the associated primary track from the first primary vertex
/////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------
StElectronTrack::StElectronTrack(StPicoDst *picoDst, StPicoTrack* t)
: mId(0), mChi2(65536), mPMomentum(0., 0., 0.), mDedx(0),
   mNHitsFit(0), mNHitsMax(0), mNHitsDedx(0), mNSigmaPion(32768), mNSigmaKaon(32768),
   mNSigmaProton(32768), mNSigmaElectron(32768), mMap0(0),
   mMap1(0), mGMom(0.,0.,0.), 
	mCurv(0), mDip(0), mPhase(0), mOrigin(0,0,0), mH(0), 
    mTofMatchFlag(0), mBeta(-999), mMassSquare(0), mLocalY(-999), mLocalZ(-999),
	mBEMCId(-9999), mBTOWADC0(-9999), mBTOWE0(-9999), mBTOWE(-9999),
  mBEMCDistZ(-9999), mBEMCDistPhi(-9999), mBSMDNEta(255), mBSMDNPhi(255),
  mBTOWId(-9999), mBTOWId23(127),
  mBTOWE1(-9999), mBTOWE2(-9999), mBTOWE3(-9999),
  mBTOWDistEta(-9999), mBTOWDistPhi(-9999),mEmcTrgId(-1)
{
      mId        = (UShort_t)t->id();
      mChi2      = (t->chi2() * 1000. > 65536) ? 65536 : (UShort_t)(TMath::Nint(t->chi2() * 1000.));
      mPMomentum = t->pMom();
      int q      = t->charge();
      mDedx      = (t->dEdx()*1000. > 65536) ? 65536 : (UShort_t)(TMath::Nint(t->dEdx()*1000.));
      mNHitsFit  = t->nHitsFit()*q;
      mNHitsMax  = t->nHitsMax();
      mNHitsDedx = (UChar_t)(t->nHitsDedx());
      mNSigmaPion = (fabs(t->nSigmaPion() * 100.) > 32768) ? 32768 : (Short_t)(TMath::Nint(t->nSigmaPion() * 100.));
      mNSigmaKaon = (fabs(t->nSigmaKaon() * 100.) > 32768) ? 32768 : (Short_t)(TMath::Nint(t->nSigmaKaon() * 100.));
      mNSigmaProton = (fabs(t->nSigmaProton() * 100.) > 32768) ? 32768 : (Short_t)(TMath::Nint(t->nSigmaProton() * 100.));
      mNSigmaElectron = (fabs(t->nSigmaElectron() * 100.) > 32768) ? 32768 : (Short_t)(TMath::Nint(t->nSigmaElectron() * 100.));

      mMap0 = t->map0(); // see hitMap definition in StTrackTopologyMap
	  mMap1 = t->map1();
      //const float* params = t->params();
      //const float* errMatrix = t->errMatrix();
      //for (int i = 0; i < 6; i++) mPar[i] = params[i];
      //for (int i = 0; i < 15; i++) mErrMatrix[i] = errMatrix[i];

      StThreeVectorF vertexPos = picoDst->event()->primaryVertex();
      StPhysicalHelixD helix = t->helix();
	  mGMom = t->gMom(vertexPos,picoDst->event()->bField());
	  mOrigin = helix.origin();
	  mCurv = helix.curvature();
	  mDip = helix.dipAngle();
	  mPhase = helix.phase();
	  mH = helix.h();
      StThreeVectorF dcaPoint = helix.at(helix.pathLength(vertexPos.x(), vertexPos.y()));
      mDcaZ = dcaPoint.z() - vertexPos.z();
      mDcaXY = helix.geometricSignedDistance(vertexPos.x(),vertexPos.y());
      
	  //wrong!
	  //StThreeVectorF dcaP = helix.momentumAt(vertexPos.x(), vertexPos.y());
      //mDcaXY = ((dcaPoint-vertexPos).x()*dcaP.y()-(dcaPoint-vertexPos).y()*dcaP.x())/dcaP.perp();

      double thePath = helix.pathLength(vertexPos);
      StThreeVectorF dcaPos = helix.at(thePath);
      mDca = (dcaPos-vertexPos).mag();

      int index2TofPid = t->bTofPidTraitsIndex();
      if (index2TofPid>=0){
        StPicoBTofPidTraits *tofPid = picoDst->btofPidTraits(index2TofPid);
        mTofMatchFlag = tofPid->btofMatchFlag();
        mBeta = tofPid->btofBeta();
        Float_t mom = mPMomentum.mag();
        if(mBeta!=0){
               mMassSquare = mom*mom*( 1.0/(mBeta*mBeta)-1.0);
        }
        mLocalY = tofPid->btofYLocal();
        mLocalZ = tofPid->btofZLocal();
      }

      int index2EmcPid = t->emcPidTraitsIndex();
      if (index2EmcPid>=0){
        StPicoEmcPidTraits *emcPid = picoDst->emcPidTraits(index2EmcPid);
        mBEMCId = emcPid->bemcId();
        mBTOWADC0 = emcPid->adc0();
        mBTOWE0 = emcPid->e0()*1000;
        mBTOWE = emcPid->e()*1000;
        mBEMCDistZ = emcPid->zDist()*100;
        mBEMCDistPhi = emcPid->phiDist()*10000;
        mBSMDNEta = emcPid->nEta();
        mBSMDNPhi = emcPid->nPhi();

        mBTOWId = emcPid->btowId();
        mBTOWId23 = emcPid->btowId2()*10 + emcPid->btowId3();
        mBTOWE1 = emcPid->e1();
        mBTOWE2 = emcPid->e2();
        mBTOWE3 = emcPid->e3();

        mBTOWDistEta = emcPid->etaTowDist()*10000;
        mBTOWDistPhi = emcPid->phiTowDist()*10000;
        
      }
}

//----------------------------------------------------------------------------------
StElectronTrack::~StElectronTrack()
{
   /* noop */
}
//----------------------------------------------------------------------------------
void StElectronTrack::Print(const Char_t *option) const
{
   if (strcmp(option, "tpc") == 0 || strcmp(option, "") == 0)
   {
      LOG_INFO << "id=" << id() <<" charge = "<<charge()
               << " chi2=" << chi2()
               << endm;
      LOG_INFO << "pMom=" << pMom() << endm;
      LOG_INFO << "gpt =" << gPt() << " gEta = "<<gEta()<<" gPhi = "<<gPhi()<<endm;
      LOG_INFO << " nHitsFit = " << nHitsFit() << " nHitsdEdx = " << nHitsDedx() << endm;
      LOG_INFO << " nSigma Pi/K/P/E = " << nSigmaPion() << "/" << nSigmaKaon() << "/" << nSigmaProton() << "/" << nSigmaElectron() << endm;
	  LOG_INFO << " beta = "<<beta()<<" dca = "<<dca()<<endm;
   }
}
