#ifndef StEventHeader_hh
#define StEventHeader_hh

class StMuDst;
class StPicoDst;
class StPicoEvent;
//class StMuEvent;
class TClonesArray;
class StMuPrimaryVertex;
class StPicoAnaTree;
class StPicoTrack;
class StPicoAnaTreeMaker;
class StBTofHeader;
#include "StThreeVectorF.hh"
#include "TVector2.h"

class StEventHeader : public TObject {
	public:
		StEventHeader();
		~StEventHeader();
		StEventHeader(const StPicoDst& picoDst, const float*) ;
		//  StEventHeader(StMuEvent*, StBTofHeader*, float*);
		void Clear(const Option_t*) {}

		Int_t    runId() const            { return mRunId; }
		Int_t    eventId() const          { return mEventId; }
		Float_t  bField() const           { return mBField; }
		StThreeVectorF primaryVertex() const { return StThreeVectorF(mVx,mVy,mVz); }
		Int_t    triggerWord() const      { return mTriggerWord; }
		Int_t    triggerWordMtd() const   { return mTriggerWordMtd; }
		Int_t    refMultPos() const       { return (Int_t)mRefMult-mRefMultNeg; }
		Int_t    refMultNeg() const       { return (Int_t)mRefMultNeg; }
		Int_t    refMult() const          { return (Int_t)(mRefMult); }
		Int_t    grefMult() const           { return (Int_t)(mGRefMult); }
		Int_t    numberOfPxlInnerHits() const { return (Int_t)(mNHitsHFT[0]); }
		Int_t    numberOfPxlOuterHits() const { return (Int_t)(mNHitsHFT[1]); }
		Int_t    numberOfIstHits() const      { return (Int_t)(mNHitsHFT[2]); }
		Int_t    numberOfSsdHits() const      { return (Int_t)(mNHitsHFT[3]); }

		Float_t  vzVpd() const            { return mVzVpd; }

		Float_t  ZDCx() const             { return (Float_t)mZDCx; }
		Float_t  BBCx() const             { return (Float_t)mBBCx; }

		Float_t backgroundRate() const             { return mBackgroundRate; }

		UShort_t btofTrayMultiplicity() const { return mbTofTrayMultiplicity ; }
		UShort_t numberOfGlobalTracks() const { return mNumberOfGlobalTracks ; }

		Float_t ranking() const { return mRanking ; }
		UShort_t nBEMCMatch() const { return mNBEMCMatch ; }
		UShort_t nBTOFMatch() const { return mNBTOFMatch ; }

		Int_t   ht_th(const Int_t i) { return mHT_Th[i]; }
		Int_t   jp_th(const Int_t i) { return mJP_Th[i]; }

		// other user's functions
		int      year() const;
		int      day() const;
		float    energy() const;
		bool     isMinBias() const;
		bool     isMBSlow() const;
		bool     isCentral() const;
		bool     isHT() const;    
		bool     isHT11() const; 
		bool     isHT15() const; 
		bool     isHT18() const;
		bool     isHT25() const;
		bool     isMtdTrig() const;
		bool     isDiMuon() const;
		bool     isDiMuonHFT() const;
		bool     isSingleMuon() const;
		bool     isEMuon() const;

		// set functions for trigger thresholds
		void     setHT_Th(const Int_t i, const Int_t th) { mHT_Th[i] = (UChar_t)th; }
		void     setJP_Th(const Int_t i, const Int_t th) { mJP_Th[i] = (UChar_t)th; }  

		// event plane
		Float_t	eventPlane()	{return mEventplane;}
		Float_t	eventplane0()	{return mEventplane0;}
		TVector2 Qvect() {return TVector2(mQx,mQy);}
		TVector2 Q0vect() {return TVector2(mQ0x,mQ0y);}

	protected: //these are written out
		Int_t          mRunId;           // run number
		Int_t          mEventId;         // event number
		Float_t        mBField;          // B field in kilogauss
		Float_t        mVx;              // primary vertex x
		Float_t        mVy;              // primary vertex y
		Float_t        mVz;              // primary vertex z
		Float_t        mVzVpd;           // vpd vz
		UInt_t         mTriggerWord;     // self-defined trigger word - see code for details
		UInt_t         mTriggerWordMtd;  // Added in Run14, st_mtd trigger
		UShort_t       mRefMultNeg;      // TPC refMult neg
		UShort_t       mRefMult;// TPC refMult neg+pos 
		UShort_t       mGRefMult;// 

		UInt_t        mZDCx;           // zdcX
		UInt_t        mBBCx;
		Float_t       mBackgroundRate;
		UShort_t      mbTofTrayMultiplicity ; // BTOF tray multiplicity
		UShort_t      mNumberOfGlobalTracks ; // # of global tracks
		UShort_t      mNHitsHFT[4];

		// From StMuPrimaryVertex
		Float_t mRanking ;
		UShort_t mNBEMCMatch ;
		UShort_t mNBTOFMatch ;

		// Online HT/JP thresholds
		UChar_t mHT_Th[4];
		UChar_t mJP_Th[4];

		Float_t   mQ0x;
		Float_t   mQ0y;
		Float_t   mQx;
		Float_t   mQy;
		Float_t   mEventplane0;
		Float_t   mEventplane;
		Float_t   mQ1x;
		Float_t   mQ1y;
		Float_t   mQ2x;
		Float_t   mQ2y;
		Float_t   mQplusx;
		Float_t   mQplusy;
		Float_t   mQminusx;
		Float_t   mQminusy;
		Float_t   mQetaplusx;
		Float_t   mQetaplusy;
		Float_t   mQetaminusx;
		Float_t   mQetaminusy;

		friend class StPicoAnaTree;

		ClassDef(StEventHeader,1)
};

#endif
