#include "StEventHeader.h"
//#include "StPicoConstants.h"
#include "StEventTypes.h"
#include "StTree.h"
#include "StuRefMult.hh"
#include "StPicoDstMaker/StPicoDst.h"
#include "StPicoDstMaker/StPicoEvent.h"
#include "StPicoDstMaker/StPicoTrack.h"
#include "TVector2.h"
#include "TRandom3.h"
#include "StMuDSTMaker/COMMON/StMuDst.h" 
#include "StMuDSTMaker/COMMON/StMuTrack.h" 
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuMtdHeader.h"
#include "StBTofHeader.h"
#include "StMessMgr.h"
//#include "StPicoUtilities.h"

ClassImp(StEventHeader)

StEventHeader::StEventHeader()
{}

//StEventHeader::StEventHeader(StMuEvent* ev, StBTofHeader *header, float *Q)
StEventHeader::StEventHeader(const StPicoDst& picoDst, const float *recenterCor)
{
	StPicoEvent* ev = picoDst.event() ;

	mRunId = ev->runId();
	mEventId = ev->eventId();
	//mFillId = ev->fillId;
	mBField=ev->bField();

	StThreeVectorF mPrimaryVertex = ev->primaryVertex();
	if( mPrimaryVertex.x()==mPrimaryVertex.y()&&mPrimaryVertex.y()==mPrimaryVertex.z() ){
		mVx = -999.;
		mVy = -999.;
		mVz = -999.;
	}else{
		mVx = mPrimaryVertex.x();
		mVy = mPrimaryVertex.y();
		mVz = mPrimaryVertex.z();
	}
	mVzVpd = ev->vzVpd();

	mTriggerWord = ev->triggerWord();
	mTriggerWordMtd = ev->triggerWordMtd();

	mRefMultNeg = (UShort_t)(ev->refMultNeg());
	mRefMult = (UShort_t)(ev->refMultNeg()) + (UShort_t)(ev->refMultPos());
	mGRefMult = (UShort_t)(ev->grefMult());

	mRanking = ev->ranking() ;
	mNBEMCMatch = ev->nBEMCMatch() ;
	mNBTOFMatch = ev->nBTOFMatch() ;

	mZDCx = (UInt_t)ev->ZDCx();
	mBBCx = (UInt_t)ev->BBCx();
	mBackgroundRate = ev->backgroundRate();
	mbTofTrayMultiplicity = ev->btofTrayMultiplicity() ;
	mNumberOfGlobalTracks = ev->numberOfGlobalTracks() ;
	mNHitsHFT[0] = (UShort_t)ev->numberOfPxlInnerHits();
	mNHitsHFT[1] = (UShort_t)ev->numberOfPxlOuterHits();
	mNHitsHFT[2] = (UShort_t)ev->numberOfIstHits();
	mNHitsHFT[3] = (UShort_t)ev->numberOfSsdHits();


	for(int i=0;i<4;i++) {
		setHT_Th(i,0);
		setJP_Th(i,0);
	}

	StThreeVectorF vtxPos = picoDst.event()->primaryVertex();
	Int_t nTracks = picoDst.numberOfTracks();

	int Fcount = 0;
	TRandom3 *random = new TRandom3();
	Double_t cossum = 0;
	Double_t sinsum = 0;
	Double_t cossum0 = 0;
	Double_t sinsum0 = 0;
	Double_t cossumQ1 = 0.;
	Double_t sinsumQ1 = 0.;
	Double_t sinsumQ2 = 0.;
	Double_t cossumQ2 = 0.;
	Double_t sinsumQplus = 0.;
	Double_t cossumQplus = 0.;
	Double_t sinsumQminus = 0.;
	Double_t cossumQminus = 0.;
	Double_t sinsumQetaplus = 0.;
	Double_t cossumQetaplus = 0.;
	Double_t sinsumQetaminus = 0.;
	Double_t cossumQetaminus = 0.;



	//for(int i=0;i<nTracks;i++) {
	//	StPicoTrack *t = (StPicoTrack*)picoDst.track(i);
	//	Int_t q = t->charge();
	//	Float_t dca = t->helix().distance(vtxPos,kFALSE);;
	//	double ratio = (double)t->nHitsFit()*1./t->nHitsMax();
	//	Float_t pt   = t->pMom().perp();
	//	Float_t eta  = t->pMom().pseudoRapidity();
	//	Float_t phi  = t->pMom().phi();
	//	Int_t nHitsFit = t->nHitsFit();

	//	if(fabs(q)!=1) continue;
	//	if(fabs(nHitsFit)<15) continue;
	//	if((pt>=2.0)||(pt<0.2)) continue;
	//	if(fabs(eta)>=1.0) continue;
	//	if(ratio < 0.52) continue;
	//	if(ratio >= 1.0) continue;
	//	if(dca >= 2.0) continue;
	//	Fcount++;
	//}
	for(int n=0;n<nTracks;n++){
		StPicoTrack *t = (StPicoTrack*)picoDst.track(n);
		Int_t q = t->charge();
		Float_t dca = t->helix().distance(vtxPos,kFALSE);;
		double ratio = (double)t->nHitsFit()*1./t->nHitsMax();
		Double_t vertZ = vtxPos.z();
		Float_t pt   = t->pMom().perp();
		Float_t eta  = t->pMom().pseudoRapidity();
		Float_t phi  = t->pMom().phi();
		if(phi<0.0) phi += (2.*TMath::Pi());
		Int_t nHitsFit = t->nHitsFit();

		if(fabs(q)!=1) continue;
		if(fabs(nHitsFit)<15) continue;
		if((pt>=2.0)||(pt<0.2)) continue;
		if(fabs(eta)>=1.0) continue;
		if(ratio < 0.52) continue;
		if(ratio >= 1.05) continue;
		if(dca >= 2.0) continue;
		//if((nSigmaElectron<=3.&&nSigmaElectron>=-3.)&&(fabs(1.-(1.0/beta))<=0.04)&&(beta>0.))continue;

		Double_t cos_part =0.;
		Double_t sin_part =0.;
		Double_t coscorrect =0;
		Double_t sincorrect =0;
		if(eta > 0.0 && vertZ > 0.0){
			coscorrect = recenterCor[0];
			sincorrect = recenterCor[1];
		}
		else if(eta > 0.0 && vertZ < 0.0){
			coscorrect = recenterCor[2];
			sincorrect = recenterCor[3];
		}
		else if(eta < 0.0 && vertZ > 0.0){
			coscorrect = recenterCor[4];
			sincorrect = recenterCor[5];
		}
		else{
			coscorrect = recenterCor[6];
			sincorrect = recenterCor[7];
		}

		cos_part = pt*cos(2.*phi)-coscorrect;
		sin_part = pt*sin(2.*phi)-sincorrect;

		//Event Plane Vector
		cossum0 += pt*cos(2.*phi);
		sinsum0 += pt*sin(2.*phi);
		cossum += cos_part;
		sinsum += sin_part;
		if(q >0){
			cossumQplus += cos_part;
			sinsumQplus += sin_part;
		}
		else{
			cossumQminus += cos_part;
			sinsumQminus += sin_part;
		}
		if(eta>0){
			cossumQetaplus += cos_part;
			sinsumQetaplus += sin_part;
		}

		else{
			cossumQetaminus += cos_part;
			sinsumQetaminus += sin_part;
		}

		if(random->Uniform(0,1)>0.5) {//subevent 1
			cossumQ1 += cos_part;
			sinsumQ1 += sin_part;
		}
		else {//subevent 2
			cossumQ2 += cos_part;
			sinsumQ2 += sin_part;
		}
	}
	random->Delete();
	mQ0x = cossum0;
	mQ0y = sinsum0;

	mQx = cossum;
	mQy = sinsum;

	mQ1x = cossumQ1;
	mQ1y = sinsumQ1;
	mQ2x = cossumQ2;
	mQ2y = sinsumQ2;

	mQplusx = cossumQplus;
	mQplusy = sinsumQplus;
	mQminusx = cossumQminus;
	mQminusy = sinsumQminus;

	mQetaplusx = cossumQetaplus;
	mQetaplusy = sinsumQetaplus;
	mQetaminusx = cossumQetaminus;
	mQetaminusy = sinsumQetaminus;

	TVector2 *Q0 = new TVector2(cossum0, sinsum0);
	if((Q0->Mod())!=0.0){
		double  eventPlane0 = 0.5*Q0->Phi();
		if(eventPlane0<0.0)eventPlane0+=TMath::Pi();
		mEventplane0 = eventPlane0;
	}

	TVector2 *Q = new TVector2(cossum, sinsum);
	if((Q->Mod())!=0.0){
		double  eventPlane = 0.5*Q->Phi();
		if(eventPlane<0.0)eventPlane+=TMath::Pi();
		mEventplane=eventPlane;
	}
}

StEventHeader::~StEventHeader()
{ }

int StEventHeader::year() const
{
	return mRunId/1000000 - 1 + 2000;
}

int StEventHeader::day() const
{
	return (mRunId%1000000)/1000;
}

float StEventHeader::energy() const
{
	if(year()<2010) return 0.0; // not applicable for data before year 2010
	if(year()==2010) {
		if(day()<=77) return 200.0;
		else if(day()<=98) return 62.4;
		else if(day()<=112) return 39.;
		else if(day()<=147) return 7.7;
		else return 11.5;
	} else if(year()==2011) {
		if(day()>=112&&day()<=122) return 19.6;
		if(day()>=172&&day()<=179) return 27.;
		if(day()>=123&&day()<=171) return 200.;
	} else if(year()==2013) {
		return 510.;
	} else if(year()==2014) {
		if(day()>=046&&day()<=070) return 14.5;
		if(day()>=077) return 200.;
	}
	return 0.0;
}

bool StEventHeader::isMinBias() const  // continue to be updated
{
	if(year()<2010) return kFALSE;  // not applicable for data before year 2010
	if(year()==2010) {
		if(fabs(energy()-200.)<1.e-4) {
			return kTRUE;      // 200 GeV, minbias stored in a separated output, always true
		} else if(fabs(energy()-62.4)<1.e-4) {
			return ( mTriggerWord & 0x7 );
		} else if(fabs(energy()-39.)<1.e-4) {
			return ( mTriggerWord & 0x1 );
		} else if(fabs(energy()-11.5)<1.e-4) {
			return ( mTriggerWord & 0x3 );
		} else if(fabs(energy()-7.7)<1.e-4) {
			return ( mTriggerWord & 0x3 );
		}
	} else if(year()==2011) {
		if(fabs(energy()-19.6)<1.e-4) {
			return ( mTriggerWord & 0x7 );
		} else if(fabs(energy()-27.)<1.e-4) {
			return ( mTriggerWord & 0x1 );
		} else if(fabs(energy()-200.)<1.e-4) {
			return ( mTriggerWord>>2 & 0x1f );  // return vpd-minbias-protected
			//      return kTRUE;     // 200 GeV, only minbias
		}
	} else if(year()==2014) {
		if(fabs(energy()-14.5)<1.e-4) {
			return ( (mTriggerWord>>2 & 0x1) || (mTriggerWord>>5 & 0x1) );
		} else if(fabs(energy()-200.)<1.e-4) {
			return ( mTriggerWord & 0x1f );
		}
	}

	return kFALSE;
}

bool StEventHeader::isMBSlow() const  // continue to be updated
{
	if(year()<2010) return kFALSE;  // not applicable for data before year 2010
	if(year()==2010) {
		if(fabs(energy()-200.)<1.e-4) {
			return kFALSE;     // no mbslow data produced yet
		} else if(fabs(energy()-62.4)<1.e-4) { 
			return ( mTriggerWord>>3 & 0x1 );    
		} else if(fabs(energy()-39.)<1.e-4) {
			return ( mTriggerWord>>1 & 0x1 );
		} else if(fabs(energy()-11.5)<1.e-4) {
			return ( mTriggerWord>>2 & 0x3 );
		} else if(fabs(energy()-7.7)<1.e-4) {
			return ( mTriggerWord>>2 & 0x1 );
		}
	} else if(year()==2011) {
		if(fabs(energy()-19.6)<1.e-4) {
			return ( mTriggerWord>>3 & 0x7 );
		} else if(fabs(energy()-27.)<1.e-4) {
			return ( mTriggerWord>>1 & 0x1 );  
		} else if(fabs(energy()-200.)<1.e-4) {
			return kFALSE;
		}
	}
	return kFALSE;
}

bool StEventHeader::isCentral() const  // continue to be updated
{
	if(year()<2010) return kFALSE;  // not applicable for data before year 2010
	if(year()==2010) { 
		if(fabs(energy()-200.)<1.e-4) {
			return kTRUE;      // 200 GeV, central stored in a separated output, always true
		} else if(fabs(energy()-62.4)<1.e-4) { 
			return ( mTriggerWord>>4 & 0x1 );    
		}
	} else if(year()==2014) {
		if(fabs(energy()-200.)<1.e-4) {
			return ( mTriggerWord>>18 & 0x1 );
		}
	}
	return kFALSE;
}

bool StEventHeader::isHT() const    // continue to be updated
{
	if(year()<2010) return kFALSE;  // not applicable for data before year 2010
	if(year()==2010) {
		if(fabs(energy()-200.)<1.e-4) {
			return kFALSE;      // 200 GeV, no HT data so far
		} else if(fabs(energy()-62.4)<1.e-4) { 
			return ( mTriggerWord>>5 & 0xF );    
		} else if(fabs(energy()-39.)<1.e-4) {
			return ( mTriggerWord>>2 & 0x1 );
		} else if(fabs(energy()-7.7)<1.e-4) {
			return ( mTriggerWord>>3 & 0x1 );
		}
	} else if(year()==2011) {
		if(fabs(energy()-19.6)<1.e-4) {
			return ( mTriggerWord>>6 & 0x1 );
		} else if(fabs(energy()-27.)<1.e-4) {
			return ( mTriggerWord>>2 & 0x1 );  
		} else if(fabs(energy()-200.)<1.-4) {
			return kFALSE;
		}
	} else if(year()==2014) {
		if(fabs(energy()-200.)<1.e-4) { 
			return ( mTriggerWord>>19 & 0x3f );
		}
	}
	return kFALSE;
}

bool StEventHeader::isHT11() const    // continue to be updated
{ 
	if(!isHT()) return kFALSE;
	if(year()==2010) { 
		if(fabs(energy()-62.4)<1.e-4) {
			return ( mTriggerWord>>5 & 0x7 );
		} 
	}
	return kTRUE;  // default HT trigger ht-11
} 

bool StEventHeader::isHT15() const    // continue to be updated
{ 
	if(!isHT()) return kFALSE;
	if(year()==2010) { 
		if(fabs(energy()-62.4)<1.e-4) {
			return ( mTriggerWord>>8 & 0x1 );
		} 
	} else if(year()==2014) {
		if(fabs(energy()-200.)<1.e-4) { 
			return ( mTriggerWord>>19 & 0x3 );
		}
	}              
	return kFALSE;
} 
bool StEventHeader::isHT18() const    // continue to be updated
{
	if(!isHT()) return kFALSE;
	if(year()==2014) {
		if(fabs(energy()-200.)<1.e-4) {
			return ( mTriggerWord>>21 & 0x3 );
		}
	}
	return kFALSE;
}
bool StEventHeader::isHT25() const    // continue to be updated
{
	if(!isHT()) return kFALSE;
	if(year()==2014) {
		if(fabs(energy()-200.)<1.e-4) {
			return ( mTriggerWord>>23 & 0x3 );
		}
	}
	return kFALSE;
}

bool StEventHeader::isMtdTrig() const    // continue to be updated
{ 
	if(isDiMuon() || isSingleMuon() || isEMuon())
		return kTRUE;
	else
		return kFALSE;
}

bool StEventHeader::isDiMuon() const    // continue to be updated
{ 
	if(year()==2014)
	{ 
		for(Int_t i=0; i<8; i++)
		{
			if(mTriggerWordMtd & (1<<i)) 
				return kTRUE;
		}
	}
	else if(year()==2013)
	{ 
		for(Int_t i=0; i<2; i++)
		{
			if(mTriggerWordMtd & (1<<i)) 
				return kTRUE;
		}
	}
	return kFALSE;
}

bool StEventHeader::isDiMuonHFT() const    // continue to be updated
{ 
	if(year()==2014)
	{ 
		for(Int_t i=5; i<8; i++)
		{
			if(mTriggerWordMtd & (1<<i)) 
				return kTRUE;
		}
	}
	return kFALSE;
} 

bool StEventHeader::isSingleMuon() const    // continue to be updated
{ 
	if(year()==2014)
	{ 
		for(Int_t i=13; i<18; i++)
		{
			if(mTriggerWordMtd & (1<<i)) 
				return kTRUE;
		}
	}
	else if(year()==2013)
	{ 
		for(Int_t i=5; i<7; i++)
		{
			if(mTriggerWordMtd & (1<<i)) 
				return kTRUE;
		}
	}
	return kFALSE;
}

bool StEventHeader::isEMuon() const    // continue to be updated
{ 
	if(year()==2014)
	{ 
		for(Int_t i=8; i<13; i++)
		{
			if(mTriggerWordMtd & (1<<i)) 
				return kTRUE;
		}
	}
	else if(year()==2013)
	{ 
		for(Int_t i=2; i<5; i++)
		{
			if(mTriggerWordMtd & (1<<i)) 
				return kTRUE;
		}
	}
	return kFALSE;
}


