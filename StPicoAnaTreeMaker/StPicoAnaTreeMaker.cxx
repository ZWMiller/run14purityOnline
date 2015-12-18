#include "StRoot/StPicoDstMaker/StPicoDst.h"
#include "StRoot/StPicoDstMaker/StPicoTrack.h"
#include "StRoot/StPicoDstMaker/StPicoV0.h"
#include "StRoot/StPicoDstMaker/StPicoDstMaker.h"
#include "StRoot/StPicoDstMaker/StPicoEvent.h"
#include "StRoot/StPicoDstMaker/StPicoMtdHit.h"
#include "StRoot/StPicoDstMaker/StPicoConstants.h"
#include "StRoot/StPicoDstMaker/StPicoMtdPidTraits.h"
#include "StRoot/StPicoDstMaker/StPicoBTofPidTraits.h"
#include "StRoot/StPicoDstMaker/StPicoEmcPidTraits.h"
#include "StRoot/StPicoDstMaker/StPicoEmcTrigger.h"
#include "StDcaGeometry.h"

#include "StEmcUtil/geometry/StEmcGeom.h"
#include "StEmcUtil/others/emcDetectorName.h"
#include "StThreeVectorF.hh"
#include "StLorentzVectorF.hh"
#include "TLorentzVector.h"
#include "SystemOfUnits.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TFile.h"
#include "TRegexp.h"
#include "TChain.h"
#include "TRandom.h"
#include "TProfile2D.h"
#include <map>
#include "StPicoAnaTreeMaker.h"
#include "StAnaTree.h"
#include "StAnaTreeArrays.h"
#include "StEventHeader.h"
#include "StElectronTrack.h"
#include "StMuonTrack.h"
#include "StHadronTrack.h"
#include "StEEPair.h"
#include "StEMuPair.h"
#include "StMuMuPair.h"
#include "StEmcTrigger.h"
#include "StMtdTrigger.h"
#include "mRunIds.h"

#define MAXFILESIZE 1900000000
#define ElectronMass 0.000510999
#define MuonMass 0.1056583715
//#include <iostream.h>

//using namespace std;

//ofstream SaveFile("runidlist.txt");

ClassImp(StPicoAnaTreeMaker)

	//-----------------------------------------------------------------------------
	StPicoAnaTreeMaker::StPicoAnaTreeMaker(Int_t mode, const char* fileName, StPicoDstMaker *picoMaker, const char *name)
: StMaker(name),
	mIoMode(mode), mOutputFile(0), mOutputHistFile(0), mChain(0), mTTree(0), mSplit(99), mCompression(9), mBufferSize(65536*4), mSaveHadron(0)
{
	assignArrays();
	streamerOff();
	zeroArrays();
	createArrays();
	mPicoDstMaker = picoMaker;
	mPicoDst = 0;
	mAnaTree = new StAnaTree();
	TH1F::SetDefaultSumw2();

	mEmcGeom = StEmcGeom::getEmcGeom(detname[0].Data());

	if(mIoMode==ioWrite) {
		mOutputFileName = fileName;
	}
	if(mIoMode==ioRead) {
		mInputFileName = fileName;
	}
	mEventCounter = 0;

	mNMaxRunId = 4000;
	mNMaxCentrality = 20;
	mTriggerSelection = 0; 

	mVzCut[0] = -100; mVzCut[1] = 100;
	mVzDiffCut[0] = -3; mVzDiffCut[1] = 3;
	mPtCut[0] = 0.2; mPtCut[1] = 100;
	mEtaCut[0] = -1.; mEtaCut[1] = 1.;
	mDcaCut[0] = 0.; mDcaCut[1] = 1.5;
	mnHitsFitCut[0] = 15; mnHitsFitCut[1] = 50;
	mnHitsDedxCut[0] = 12; mnHitsDedxCut[1] = 50;
	mRatioCut[0] = 0.52; mRatioCut[1] = 1.0;
	mHFTTrackCut = false;

	mEPtCut[0] = 0.2; mEPtCut[1] = 100;
	mEEtaCut[0] = -1.; mEEtaCut[1] = 1.;
	mEInvBetaCut[0] = 0.97; mEInvBetaCut[1] = 1.03;
	mELocalYCut[0] = -2; mELocalYCut[1] = 2;
	mELocalZCut[0] = -3; mELocalZCut[1] = 3;
	mEnSigECut[0] = -2; mEnSigECut[1] = 2.5;

	mEmcEPtCut[0] = 1.; mEmcEPtCut[1] = 100;
	mEmcEEtaCut[0] = -1.; mEmcEEtaCut[1] = 1.;
	mEmcEPveCut[0] = 0.2; mEmcEPveCut[1] = 4.;

	mEnEtaCut[0] = 0; mEnEtaCut[1] = 20;
	mEnPhiCut[0] = 0; mEnPhiCut[1] = 20;
	mEZDistCut[0] = -3; mEZDistCut[1] = 3;
	mEPhiDistCut[0] = -0.08; mEPhiDistCut[1] = 0.08;

	mMuPtCut[0] = 1.0; mMuPtCut[1] = 100.;
	mMuEtaCut[0] = -0.65; mMuEtaCut[1] = 0.65;
	mMunSigPiCut[0] = -2; mMunSigPiCut[1] = 3.5;
	mMudTCut[0] = -1; mMudTCut[1] = 1.5;
	mMudYCut[0] = -60; mMudYCut[1] = 60;
	mMudZCut[0] = -30; mMudZCut[1] = 30;

	mDauEPtCut[0] = 0.2; mDauEPtCut[1]  = 100.;
	mDauEEtaCut[0] = -1.; mDauEEtaCut[1] = 1.;
	mDauEDcaToVtxCut[0] = 0; mDauEDcaToVtxCut[1] = 3;
	mDauEDcaDistCut[0] = 0; mDauEDcaDistCut[1] = 5;

	mDauMuPtCut[0] = 1.0; mDauMuPtCut[1] = 100.;
	mDauMuEtaCut[0] = -0.65; mDauMuEtaCut[1] = 0.65;
	mDauMuDcaToVtxCut[0] = 0.; mDauMuDcaToVtxCut[1] = 5.;
	mCosThetaStarCut[0] = -1.; mCosThetaStarCut[1] = 1.;
	mPointingAngleCut[0] = -3.14159; mPointingAngleCut[1] = 3.14159;
	mPairDcaCut[0] = 0.; mPairDcaCut[1] = 1000.;
	mPairDecayLCut[0] = 0.; mPairDecayLCut[1] = 1000.;
	mPairYCut[0] = -1.; mPairYCut[1] = 1.;
	mPairMassCut[0] = 0.; mPairMassCut[1] = 20;

	mSizeAll = 0;
	for(int i=0;i<6;i++) mSizeBranch[i]=0;


}

//----------------------------------------------------------------------------- 
StPicoAnaTreeMaker::~StPicoAnaTreeMaker()
{ /*  */ }
//-----------------------------------------------------------------------
void StPicoAnaTreeMaker::assignArrays()
{
	mAnaTreeArrays  = mAnaTreeAllArrays + 0;
}
//-----------------------------------------------------------------------
void StPicoAnaTreeMaker::clearArrays()
{
	for ( int i=0; i<__NANATREEARRAYS__; i++) {
		mAnaTreeAllArrays[i]->Clear();
		StAnaTreeArrays::anaTreeArrayCounters[i] = 0;
	}
}
//-----------------------------------------------------------------------
void StPicoAnaTreeMaker::zeroArrays()
{
	memset(mAnaTreeAllArrays, 0, sizeof(void*)*__NANATREEARRAYS__);
	memset(mStatusArrays, (char)1, sizeof(mStatusArrays)); // defaul all ON
}
//-----------------------------------------------------------------------
void StPicoAnaTreeMaker::SetStatus(const char *arrType, int status)
{
	static const char *specNames[] = {"All",0};
	//static const char *specNames[] = {"All","Electron","Muon","Hadron","EEPair","EMuPair","MuMuPair",0};
	static const int specIndex[] = {
		0,
		__NANATREEARRAYS__,
		-1};

	if (strncmp(arrType,"St",2)==0) arrType+=2;  //Ignore first "St"
	for (int i=0;specNames[i];i++) {
		if (strcmp(arrType,specNames[i])) continue;
		char *sta=mStatusArrays+specIndex[i];
		int   num=specIndex[i+1]-specIndex[i];
		memset(sta,status,num);
		LOG_INFO << "StPicoAnaTreeMaker::SetStatus " << status << " to " << specNames[i] << endm;
		if (mIoMode==ioRead)
			setBranchAddresses(mChain);
		return;
	}

	TRegexp re(arrType,1);
	for (int i=0;i<__NANATREEARRAYS__;i++) {
		Ssiz_t len;
		if (re.Index(StAnaTreeArrays::anaTreeArrayNames[i],&len) < 0)   continue;
		LOG_INFO << "StPicoAnaTreeMaker::SetStatus " << status << " to " << StAnaTreeArrays::anaTreeArrayNames[i] << endm;
		mStatusArrays[i]=status;
	}
	if (mIoMode==ioRead)
		setBranchAddresses(mChain);
}
//-----------------------------------------------------------------------
void StPicoAnaTreeMaker::setBranchAddresses(TChain* chain) {
	if(!chain) return;
	chain->SetBranchStatus("*",0);
	TString ts;
	for ( int i=0; i<__NANATREEARRAYS__; i++) {
		if (mStatusArrays[i]==0) continue;
		const char *bname=StAnaTreeArrays::anaTreeArrayNames[i];
		TBranch *tb = chain->GetBranch(bname);
		if(!tb) {
			LOG_WARN << "setBranchAddress: Branch name " << bname << " does not exist!" << endm;
			continue;
		}
		ts = bname; ts += "*";
		chain->SetBranchStatus(ts,1);
		chain->SetBranchAddress(bname,mAnaTreeAllArrays+i);
		assert(tb->GetAddress()==(char*)(mAnaTreeAllArrays+i));
	}
	mTTree = mChain->GetTree();
}
//-----------------------------------------------------------------------
void  StPicoAnaTreeMaker::streamerOff() {
	StEventHeader::Class()->IgnoreTObjectStreamer();
	StElectronTrack::Class()->IgnoreTObjectStreamer();
	StMuonTrack::Class()->IgnoreTObjectStreamer();
	StHadronTrack::Class()->IgnoreTObjectStreamer();
	StEEPair::Class()->IgnoreTObjectStreamer();
	StEMuPair::Class()->IgnoreTObjectStreamer();
	StMuMuPair::Class()->IgnoreTObjectStreamer();
	StEmcTrigger::Class()->IgnoreTObjectStreamer();
	StMtdTrigger::Class()->IgnoreTObjectStreamer();
}
//-----------------------------------------------------------------------
void StPicoAnaTreeMaker::createArrays() {
	for ( int i=0; i<__NANATREEARRAYS__; i++) {
		clonesArray(mAnaTreeAllArrays[i],StAnaTreeArrays::anaTreeArrayTypes[i],StAnaTreeArrays::anaTreeArraySizes[i],StAnaTreeArrays::anaTreeArrayCounters[i]);
	}
	mAnaTree->set(this);
}
//-----------------------------------------------------------------------
TClonesArray* StPicoAnaTreeMaker::clonesArray(TClonesArray*& p, const char* type, int size, int& counter) {
	if(p) return p;
	p = new TClonesArray(type, size);
	counter=0;
	return p;
}


//----------------------------------------------------------------------------- 
Int_t StPicoAnaTreeMaker::Init() {
	if (mIoMode == ioRead) {
		openRead();     // if read, don't care about phi weight files
	} else if (mIoMode == ioWrite) {
		openWrite();
	}

	return kStOK;
}

//-----------------------------------------------------------------------
void StPicoAnaTreeMaker::openRead() {
	if(!mChain) mChain = new TChain("anaTree");
	string dirFile = mInputFileName.Data();
	if (dirFile.find(".list")!=string::npos)  {
		ifstream inputStream(dirFile.c_str());
		if(!(inputStream.good())) {
			LOG_ERROR << "ERROR: Cannot open list file " << dirFile << endm;
		}
		char line[512];
		int nFile=0;
		string ltest;
		while (inputStream.good()) {
			inputStream.getline(line,512);
			string aFile = line;      
			if (inputStream.good() && aFile.find(".anaTree.root")!=string::npos) {
				//        TFile *ftmp = new TFile(line);
				TFile *ftmp = TFile::Open(line);
				if(ftmp && ftmp->IsOpen() && ftmp->GetNkeys()) {
					LOG_INFO << " Read in picoDst file " << line << endm;
					mChain->Add(line);
					nFile++;
				}
			}
		}
		LOG_INFO << " Total " << nFile << " files have been read in. " << endm;
	} else if (dirFile.find(".anaTree.root")!=string::npos)  {
		mChain->Add(dirFile.c_str());
	} else {
		LOG_WARN << " No good input file to read ... " << endm;
	}
	if(mChain) {
		setBranchAddresses(mChain);
		mAnaTree->set(this);
	}
	return;
}
//-----------------------------------------------------------------------
void StPicoAnaTreeMaker::openWrite() {

	mOutputFile = new TFile(mOutputFileName.Data(),"RECREATE");
	LOG_INFO << " Output file: " << mOutputFileName.Data() << " created." << endm;
	mOutputFile->SetCompressionLevel(mCompression);
	TBranch* branch;
	int bufsize = mBufferSize;
	if (mSplit) bufsize /= 4;
	mTTree = new TTree("anaTree","StAnaTree",mSplit);
	mTTree->SetMaxTreeSize(MAXFILESIZE);
	mTTree->SetAutoSave(1000000);
	for ( int i=0; i<__NANATREEARRAYS__; i++) {
		if (mStatusArrays[i]==0) {
			cout << " Branch " << StAnaTreeArrays::anaTreeArrayNames[i] << " status is OFF! " << endl;
			continue;
		}
		branch = mTTree->Branch(StAnaTreeArrays::anaTreeArrayNames[i],&mAnaTreeAllArrays[i],bufsize,mSplit);
	}
	declareHistos();
}
///-----------------------------------------------------------------------
void StPicoAnaTreeMaker::declareHistos() {
	LOG_INFO << " StPicoAnaTreeMaker::declareHistos() " << endm;
	TString mOutputHistFileName = mOutputFileName;
	mOutputHistFileName.ReplaceAll(".anaTree.root",".hist.root");
	mOutputHistFile = new TFile(mOutputHistFileName.Data(),"RECREATE");
	/* define pID histograms here */
	mhnEvents = new TH1F("mhnEvents", "hnEvents; ",40,0,40);
	mhnTracks = new TH1F("mhnTracks", "hnTracks; ",30,0,30);
	mhnSigEvsP = new TH2F("mhnSigEvsP", "nSigmaElectron vs p; p (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhnSigEvsPt = new TH2F("mhnSigEvsPt", "nSigmaElectron vs p_{T}; p_{T} (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhTofEnSigEvsPCut = new TH2F("mhTofEnSigEvsPCut", "nSigmaElectron vs p w/ TOF cut; p (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhTofEnSigEvsPtCut = new TH2F("mhTofEnSigEvsPtCut", "nSigmaElectron vs p_{T} w/ TOF cut; p_{T} (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhEmcEnSigEvsPCut = new TH2F("mhEmcEnSigEvsPCut", "nSigmaElectron vs p w/ EMC cut; p (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhEmcEnSigEvsPtCut = new TH2F("mhEmcEnSigEvsPtCut", "nSigmaElectron vs p_{T} w/ EMC cut; p_{T} (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhSmdEnSigEvsPCut = new TH2F("mhSmdEnSigEvsPCut", "nSigmaElectron vs p w/ SMD cut; p (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhSmdEnSigEvsPtCut = new TH2F("mhSmdEnSigEvsPtCut", "nSigmaElectron vs p_{T} w/ SMD cut; p_{T} (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);

	mhInvBetavsP = new TH2F("mhInvBetavsP", "1/#beta vs p; p (GeV/c); 1/#beta;",2000,0,20,300,0+1e-6,6+1e-6);
	mhInvBetavsPt = new TH2F("mhInvBetavsPt", "1/#beta vs p_{T}; p_{T} (GeV/c); 1/#beta;",2000,0,20,300,0+1e-6,6+1e-6);

	mhM2vsP = new TH2F("mhM2vsP", "m^{2} vs p; p (GeV/c); m^{2} (GeV)^{2};",1000,0,10,300,-1+1e-6,2+1e-6);
	mhM2vsPt = new TH2F("mhM2vsPt", "m^{2} vs p_{T}; p_{T} (GeV/c); m^{2} (GeV)^{2};",1000,0,10,300,-1.+1e-6,2+1e-6);

	mhTofLocalYvsTray = new TH2F("mhTofLocalYvsTray", "localY vs tray; tray; localY (cm);",121,0,121,400,-4,4);
	mhTofLocalZvsTray = new TH2F("mhTofLocalZvsTray", "localZ vs tray; tray; localZ (cm);",121,0,121,600,-6,6);

	mhnEtavsnPhi = new TH2F("mhnEtavsnPhi","nEta vs nPhi; nPhi; nEta;",10,0,10,10,0,10);
	mhZDistvsPt = new TH2F("mhZDistvsPt", "zDist vs p_{T}; p_{T} (GeV/c); zDist (cm);",2000,0,20,200,-6+1e-6,6+1e-6);
	mhPhiDistvsPt = new TH2F("mhPhiDistvsPt", "phiDist vs p_{T}; p_{T} (GeV/c); phiDist (rad);",2000,0,20,400,-0.1+1e-8,0.1+1e-8);

	mhEvPvsPt = new TH2F("mhEvPvsPt", "E/p vs p_{T}; p_{T} (GeV/c); E/p ;",2000,0,20,600,0,6);
	mhPvEvsPt = new TH2F("mhPvEvsPt", "p/E vs p_{T}; p_{T} (GeV/c); p/E ;",2000,0,20,600,0,6);

	mhnSigPivsP = new TH2F("mhnSigPivsP", "nSigmaPi vs p; p (GeV/c); n#sigma_{#pi};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhnSigPivsPt = new TH2F("mhnSigPivsPt", "nSigmaPi vs p_{T}; p_{T} (GeV/c); n#sigma_{#pi};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhMtdMunSigPivsP = new TH2F("mhMtdMunSigPivsP", "nSigmaPi vs p; p (GeV/c); n#sigma_{#pi};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhMtdMunSigPivsPt = new TH2F("mhMtdMunSigPivsPt", "nSigmaPi vs p_{T}; p_{T} (GeV/c); n#sigma_{#pi};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhMtdMunSigPivsPCut = new TH2F("mhMtdMunSigPivsPCut", "nSigmaPi vs p w/ MTD cut; p (GeV/c); n#sigma_{#pi};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhMtdMunSigPivsPtCut = new TH2F("mhMtdMunSigPivsPtCut", "nSigmaPi vs p_{T} w/ MTD cut; p_{T} (GeV/c); n#sigma_{#pi};",1000,0,10,1000,-10+1e-6,10+1e-6);

	mhMtddZvsPt = new TH2F("mhMtddZvsPt","MTD dZ vs p_{T}; p_{T} (GeV/c); #DeltaZ (cm)",1000,0,10,400,-100+1e-8,100+1e-8);
	mhMtddYvsPt = new TH2F("mhMtddYvsPt","MTD dY vs p_{T}; p_{T} (GeV/c); #DeltaY (cm)",1000,0,10,400,-100+1e-8,100+1e-8);
	mhMtddTvsPt = new TH2F("mhMtddTvsPt","MTD dT vs p_{T}; p_{T} (GeV/c); #DeltaT (ns)",1000,0,10,800,-10,10);
	mhMtddZvsMod = new TH2F("mhMtddZvsMod","MTD dZ vs module; backleg*5+module; #DeltaZ (cm)",150,0,150,400,-100,100);
	mhMtddYvsMod = new TH2F("mhMtddYvsMod","MTD dY vs module; backleg*5+module; #DeltaY (cm)",150,0,150,400,-100,100);
	mhMtddTvsMod = new TH2F("mhMtddTvsMod","MTD dT vs module; backleg*5+module; #DeltaZ (ns)",150,0,150,800,-10,10);


	mhnSigE2PivsP = new TH2F("mhnSigE2PivsP", "2pi nSigmaElectron vs p; p (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhnSigEPivsP = new TH2F("mhnSigEPivsP", "pi nSigmaElectron vs p; p (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhnSigEKvsP = new TH2F("mhnSigEKvsP", "k nSigmaElectron vs p; p (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhnSigEPvsP = new TH2F("mhnSigEPvsP", "p nSigmaElectron vs p; p (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);

	mhnSigE2PivsPt = new TH2F("mhnSigE2PivsPt", "2pi nSigmaElectron vs p; p_{T} (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhnSigEPivsPt = new TH2F("mhnSigEPivsPt", "pi nSigmaElectron vs p; p_{T} (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhnSigEKvsPt = new TH2F("mhnSigEKvsPt", "k nSigmaElectron vs p; p_{T} (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhnSigEPvsPt = new TH2F("mhnSigEPvsPt", "p nSigmaElectron vs p; p_{T} (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);

	mhnSigEPivsPwTOF = new TH2F("mhnSigEPivsPwTOF", "pi nSigmaElectron vs p; p (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhnSigEKvsPwTOF = new TH2F("mhnSigEKvsPwTOF", "k nSigmaElectron vs p; p (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhnSigEPvsPwTOF = new TH2F("mhnSigEPvsPwTOF", "p nSigmaElectron vs p; p (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);

	mhnSigEPivsPtwTOF = new TH2F("mhnSigEPivsPtwTOF", "pi nSigmaElectron vs p; p_{T} (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhnSigEKvsPtwTOF = new TH2F("mhnSigEKvsPtwTOF", "k nSigmaElectron vs p; p_{T} (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);
	mhnSigEPvsPtwTOF = new TH2F("mhnSigEPvsPtwTOF", "p nSigmaElectron vs p; p_{T} (GeV/c); n#sigma_{e};",1000,0,10,1000,-10+1e-6,10+1e-6);

	mhDsmAdcvsAdc0 = new TH2F("mhDsmAdcvsAdc0", "mhDsmAdcvsAdc0; Adc0; dsmAdc;",1000,0,2000,500,0,1000);
	mhDsmAdcvsP = new TH2F("mhDsmAdcvsP", "mhDsmAdcvsP; p (GeV/c); dsmAdc;",500,0,20,500,0,1000);
	mhDsmAdcvsE = new TH2F("mhDsmAdcvsE", "mhDsmAdcvsE; E (GeV); dsmAdc;",500,0,20,500,0,1000);
	mhAdc0vsP = new TH2F("mhAdc0vsP", "mhAdc0vsP; p (GeV/c); adc0;",500,0,20,1000,0,2000);
	mhAdc0vsE = new TH2F("mhAdc0vsE", "mhAdc0vsE; E (GeV); adc0;",500,0,20,1000,0,2000);

	cosfarwest_correction = new TProfile2D("coscorrect_farwest","coscorrect_farwest",mNMaxRunId,0,mNMaxRunId,mNMaxCentrality,0,mNMaxCentrality);
	sinfarwest_correction = new TProfile2D("sincorrect_farwest","sincorrect_farwest",mNMaxRunId,0,mNMaxRunId,mNMaxCentrality,0,mNMaxCentrality);
	coswest_correction = new TProfile2D("coscorrect_west","coscorrect_west",mNMaxRunId,0,mNMaxRunId,mNMaxCentrality,0,mNMaxCentrality);
	sinwest_correction = new TProfile2D("sincorrect_west","sincorrect_west",mNMaxRunId,0,mNMaxRunId,mNMaxCentrality,0,mNMaxCentrality);
	coseast_correction = new TProfile2D("coscorrect_east","coscorrect_east",mNMaxRunId,0,mNMaxRunId,mNMaxCentrality,0,mNMaxCentrality);
	sineast_correction = new TProfile2D("sincorrect_east","sincorrect_east",mNMaxRunId,0,mNMaxRunId,mNMaxCentrality,0,mNMaxCentrality);
	cosfareast_correction = new TProfile2D("coscorrect_fareast","coscorrect_fareast",mNMaxRunId,0,mNMaxRunId,mNMaxCentrality,0,mNMaxCentrality);
	sinfareast_correction = new TProfile2D("sincorrect_fareast","sincorrect_fareast",mNMaxRunId,0,mNMaxRunId,mNMaxCentrality,0,mNMaxCentrality);

	TFile f("./recenter_correction.root");
	if(f.IsOpen())
	{
		cout<<"reading correction.root"<<endl;
		TProfile2D *temp_cos_farwest = (TProfile2D*)f.Get("etapluszplusQx");
		TProfile2D *temp_sin_farwest = (TProfile2D*)f.Get("etapluszplusQy");
		TProfile2D *temp_cos_west = (TProfile2D*)f.Get("etapluszminusQx");
		TProfile2D *temp_sin_west = (TProfile2D*)f.Get("etapluszminusQy");
		TProfile2D *temp_cos_east = (TProfile2D*)f.Get("etaminuszplusQx");
		TProfile2D *temp_sin_east = (TProfile2D*)f.Get("etaminuszplusQy");
		TProfile2D *temp_cos_fareast = (TProfile2D*)f.Get("etaminuszminusQx");
		TProfile2D *temp_sin_fareast = (TProfile2D*)f.Get("etaminuszminusQy");

		cosfarwest_correction->Add(temp_cos_farwest);
		sinfarwest_correction->Add(temp_sin_farwest);
		coswest_correction->Add(temp_cos_west);
		sinwest_correction->Add(temp_sin_west);
		coseast_correction->Add(temp_cos_east);
		sineast_correction->Add(temp_sin_east);
		cosfareast_correction->Add(temp_cos_fareast);
		sinfareast_correction->Add(temp_sin_fareast);
	}
	else {
		for(int j=0;j<mNMaxRunId;j++) {//runnumber
			for(int k=0; k<mNMaxCentrality;k++) {//centrality
				cosfarwest_correction->SetBinContent(j+1,k+1, 0.);
				sinfarwest_correction->SetBinContent(j+1,k+1, 0.);
				coswest_correction->SetBinContent(j+1,k+1, 0.);
				sinwest_correction->SetBinContent(j+1,k+1, 0.);
				coseast_correction->SetBinContent(j+1,k+1, 0.);
				sineast_correction->SetBinContent(j+1,k+1, 0.);
				cosfareast_correction->SetBinContent(j+1,k+1, 0.);
				sinfareast_correction->SetBinContent(j+1,k+1, 0.);
			}
		}
		cout<<"###NOTE###: correction does not exist!!!"<<endl;
	}
	f.Close();


}
//_____________________________________________________________________________
void StPicoAnaTreeMaker::writeHistos() {
	LOG_INFO << "StPicoAnaTreeMaker::writeHistos() " << endm;
	/* define pID histograms here */
	mOutputHistFile->Write();
	mOutputHistFile->Close();
}
//_____________________________________________________________________________
void StPicoAnaTreeMaker::printCuts(){

	LOG_INFO<<"anaTree Printing cuts:"<<endm;
	LOG_INFO<<mVzCut[0]<<" < mVzCut < "<<mVzCut[1]<<endm;
	LOG_INFO<<mVzDiffCut[0]<<" < mVzDiffCut < "<<mVzDiffCut[1]<<endm;
	LOG_INFO<<mPtCut[0]<<" < mPtCut < "<<mPtCut[1]<<endm;
	LOG_INFO<<mEtaCut[0]<<" < mEtaCut < "<< mEtaCut[1]<<endm;
	//LOG_INFO<<mDcaCut[0]<<" < mDcaCut < "<< mDcaCut[1]<<endm;
	LOG_INFO<<mnHitsFitCut[0]<<" < mnHitsFitCut < "<<mnHitsFitCut[1]<<endm;
	LOG_INFO<<mnHitsDedxCut[0]<<" < mnHitsDedxCut < "<< mnHitsDedxCut[1]<<endm;
	LOG_INFO<<mRatioCut[0]<<" < mRatioCut < "<< mRatioCut[1]<<endm;
	LOG_INFO<<mHFTTrackCut<<" < mHFTTrackCut < "<< mHFTTrackCut<<endm;

	LOG_INFO<<mEPtCut[0]<<" < mEPtCut < "<< mEPtCut[1]<<endm;
	LOG_INFO<<mEEtaCut[0]<<" < mEEtaCut < "<<mEEtaCut[1]<<endm;
	LOG_INFO<<mEInvBetaCut[0]<<" < mEInvBetaCut < "<<mEInvBetaCut[1]<<endm;
	LOG_INFO<<mELocalYCut[0]<<" < mELocalYCut < "<< mELocalYCut[1]<<endm;
	LOG_INFO<<mELocalZCut[0]<<" < mELocalZCut < "<< mELocalZCut[1]<<endm;
	LOG_INFO<<mEnSigECut[0]<<" < mEnSigECut < "<<mEnSigECut[1]<<endm;
	LOG_INFO<<mEmcEPtCut[0]<<" < mEmcEPtCut < "<<mEmcEPtCut[1]<<endm;
	LOG_INFO<<mEmcEEtaCut[0]<<" < mEmcEEtaCut < "<< mEmcEEtaCut[1]<<endm;
	LOG_INFO<<mEmcEPveCut[0]<<" < mEmcEPveCut < "<< mEmcEPveCut[1]<<endm;
	LOG_INFO<<mEnEtaCut[0]<<" < mEnEtaCut < "<< mEnEtaCut[1]<<endm;
	LOG_INFO<<mEnPhiCut[0]<<" < mEnPhiCut < "<< mEnPhiCut[1]<<endm;
	LOG_INFO<<mEZDistCut[0]<<" < mEZDistCut < "<<mEZDistCut[1]<<endm;
	LOG_INFO<<mEPhiDistCut[0]<<" <mEPhiDistCut < "<<mEPhiDistCut[1]<<endm;

	LOG_INFO<<mMuPtCut[0]<<" < mMuPtCut < "<<mMuPtCut[1]<<endm;
	LOG_INFO<<mMuEtaCut[0]<<" < mMuEtaCut < "<< mMuEtaCut[1]<<endm;
	LOG_INFO<<mMunSigPiCut[0]<<" < mMunSigPiCut < "<<mMunSigPiCut[1]<<endm;
	LOG_INFO<<mMudTCut[0]<<" < mMudTCut < "<<mMudTCut[1]<<endm;
	LOG_INFO<<mMudZCut[0]<<" < mMudZCut < "<<mMudZCut[1]<<endm;
	LOG_INFO<<mMudYCut[0]<<" < mMudYCut < "<<mMudYCut[1]<<endm;

	LOG_INFO<<mDauEPtCut[0]<<" < mDauEPtCut < "<<mDauEPtCut[1]<<endm;
	LOG_INFO<<mDauEEtaCut[0]<<" < mDauEEtaCut < "<< mDauEEtaCut[1]<<endm;
	LOG_INFO<<mDauEDcaToVtxCut[0]<<" < mDauEDcaToVtxCut < "<<mDauEDcaToVtxCut[1]<<endm; 
	LOG_INFO<<mDauEDcaDistCut[0]<<" < mDauEDcaDistCut < "<<mDauEDcaDistCut[1]<<endm;
	LOG_INFO<<mDauMuPtCut[0]<<" < mDauMuPtCut < "<< mDauMuPtCut[1]<<endm;
	LOG_INFO<<mDauMuEtaCut[0]<<" < mDauMuEtaCut < "<<mDauMuEtaCut[1]<<endm;
	LOG_INFO<<mDauMuDcaToVtxCut[0]<<"< mDauMuDcaToVtxCut <" <<mDauMuDcaToVtxCut[1]<<endm;
	LOG_INFO<<mCosThetaStarCut[0]<<" < mCosThetaStarCut < "<<mCosThetaStarCut[1]<<endm; 
	LOG_INFO<<mPointingAngleCut[0]<<" < mPointingAngleCut <" << mPointingAngleCut[1]<<endm;
	LOG_INFO<<mPairDcaCut[0]<<" < mPairDcaCut < "<< mPairDcaCut[1]<<endm;
	LOG_INFO<<mPairDecayLCut[0]<<" < mPairDecayLCut < "<<mPairDecayLCut[1]<<endm; 
	LOG_INFO<<mPairYCut[0]<<" < mPairYCut < "<< mPairYCut[1]<<endm;
	LOG_INFO<<mPairMassCut[0]<<" < mPairMassCut < "<<mPairMassCut[1]<<endm;

}
//_____________________________________________________________________________
void StPicoAnaTreeMaker::Clear(const char *){
	if (mIoMode==ioRead)
		return;
	clearArrays();
}
//_____________________________________________________________________________
void StPicoAnaTreeMaker::closeRead() {
	if (mChain) mChain->Delete();
	mChain = 0;
}
//_____________________________________________________________________________
void StPicoAnaTreeMaker::closeWrite() {
	if(mIoMode==ioWrite) {
		if(mOutputFile) {
			mOutputFile->Write();
			mOutputFile->Close();
			writeHistos();
		}
	}
}

//----------------------------------------------------------------------------- 
Int_t StPicoAnaTreeMaker::Finish() {
	//	SaveFile.close();
	if (mIoMode == ioRead) {
		closeRead();     // if read, don't care about phi weight files
	} else if (mIoMode == ioWrite) {
		closeWrite();
		printCuts();
	}

	for(int i=0;i<__NANATREEARRAYS__;i++){
		int size = 0;
		if(i==0) size=sizeof(StEventHeader);
		if(i==1) size=sizeof(StElectronTrack);
		if(i==2) size=sizeof(StMuonTrack);
		if(i==3) size=sizeof(StHadronTrack);
		if(i==4) size=sizeof(StEEPair);
		if(i==5) size=sizeof(StEMuPair);
		if(i==6) size=sizeof(StMuMuPair);
		if(i==7) size=sizeof(StEmcTrigger);
		if(i==8) size=sizeof(StMtdTrigger);
		LOG_INFO<<"i="<<i<<" size = "<<size<<" mSizeBranch = "<<mSizeBranch[i]/mSizeAll*100.<<" %  = "<<mSizeBranch[i]<<endm;
	}
	LOG_INFO<<"mSizeAll = "<<mSizeAll<<endm;

	return kStOK;
}

//----------------------------------------------------------------------------- 
Int_t StPicoAnaTreeMaker::Make() {

	int returnStarCode = kStOK;
	if (mIoMode == ioWrite){
		returnStarCode = MakeWrite();
	}
	else if (mIoMode == ioRead) returnStarCode = MakeRead();

	for(int i=0;i<__NANATREEARRAYS__;i++){
		if(!mAnaTreeAllArrays[i]) continue;
		int nent = mAnaTreeAllArrays[i]->GetEntries();
		int size = 0;
		if(i==0) size=sizeof(StEventHeader);
		if(i==1) size=sizeof(StElectronTrack);
		if(i==2) size=sizeof(StMuonTrack);
		if(i==3) size=sizeof(StHadronTrack);
		if(i==4) size=sizeof(StEEPair);
		if(i==5) size=sizeof(StEMuPair);
		if(i==6) size=sizeof(StMuMuPair);
		if(i==7) size=sizeof(StEmcTrigger);
		if(i==8) size=sizeof(StMtdTrigger);
		mSizeBranch[i] += nent*size;
		mSizeAll += nent*size; 
	}

	return returnStarCode;

}//end of main fucntion

//-----------------------------------------------------------------------
Int_t StPicoAnaTreeMaker::MakeRead() {
	if (!mChain) {
		LOG_WARN << " No input files ... ! EXIT" << endm;
		return kStWarn;
	}
	mChain->GetEntry(mEventCounter++);
	mAnaTree->set(this);
	return kStOK;
}

//-----------------------------------------------------------------------
Int_t StPicoAnaTreeMaker::MakeWrite() {
	if(!mPicoDstMaker) {
		LOG_WARN << " No PicoDstMaker! Skip! " << endm;
		return kStWarn;
	}

	mPicoDst = mPicoDstMaker->picoDst();

	if(!mPicoDst) {
		LOG_WARN << " No PicoDst! Skip! " << endm;
		return kStWarn;
	}

	if(passEvent(mPicoDst)){
		fillEventHeader();
		fillTracks();
		fillPairs();
		fillEmcTrigger();
		if(mTriggerSelection==mtd) fillMtdTrigger();
		mTTree->Fill();
	}

	return kStOK;
}

//-----------------------------------------------------------------------
bool StPicoAnaTreeMaker::passEvent(StPicoDst *pico){
	/* event selection */		
	if(!pico->event()) return false;
	mhnEvents->Fill(0);
	StThreeVectorF mPrimaryVertex = pico->event()->primaryVertex();
	Float_t vzVpd = pico->event()->vzVpd();
	if(mTriggerSelection==minbias){
		if(pico->event()->isMinBias()){ 
			mhnEvents->Fill(1);
			if(mPrimaryVertex.z()<mVzCut[0]||mPrimaryVertex.z()>mVzCut[1]) return false;
			mhnEvents->Fill(2);
			if(vzVpd-mPrimaryVertex.z()<mVzDiffCut[0]||vzVpd-mPrimaryVertex.z()>mVzDiffCut[1]) return false;
			mhnEvents->Fill(3);

			int triggerWord = pico->event()->triggerWord();
			if(triggerWord & 0x1) mhnEvents->Fill(4);
			if(triggerWord>>1 & 0x1) mhnEvents->Fill(5);
			if(triggerWord>>2 & 0x1) mhnEvents->Fill(6);
			if(triggerWord>>3 & 0x1) mhnEvents->Fill(7);
			if(triggerWord>>4 & 0x1) mhnEvents->Fill(8);
			if(triggerWord>>5 & 0x1) mhnEvents->Fill(9);
			if(triggerWord>>6 & 0x1) mhnEvents->Fill(10);
			if(triggerWord>>7 & 0x1) mhnEvents->Fill(11);
			if(triggerWord>>8 & 0x1) mhnEvents->Fill(12);
			if(triggerWord>>9 & 0x1) mhnEvents->Fill(13);
			if(triggerWord>>10 & 0x1) mhnEvents->Fill(14);
			if(triggerWord>>11 & 0x1) mhnEvents->Fill(15);

			return true;
		}else return false;

	}else if(mTriggerSelection==ht){
		if(pico->event()->isHT()){ 
			mhnEvents->Fill(1);
			if(mPrimaryVertex.z()<mVzCut[0]||mPrimaryVertex.z()>mVzCut[1]) return false;
			mhnEvents->Fill(2);
			//if(vzVpd-mPrimaryVertex.z()<mVzDiffCut[0]||vzVpd-mPrimaryVertex.z()>mVzDiffCut[1]) return false;
			//mhnEvents->Fill(3);

			int triggerWord = pico->event()->triggerWord();
			if(triggerWord>>19 & 0x1) mhnEvents->Fill(4);
			if(triggerWord>>20 & 0x1) mhnEvents->Fill(5);
			if(triggerWord>>21 & 0x1) mhnEvents->Fill(6);
			if(triggerWord>>22 & 0x1) mhnEvents->Fill(7);
			if(triggerWord>>23 & 0x1) mhnEvents->Fill(8);
			if(triggerWord>>24 & 0x1) mhnEvents->Fill(9);
			return true;
		}else return false;
	}else if(mTriggerSelection==mtd){
		if(pico->event()->isMtdTrig()){ 
			mhnEvents->Fill(1);
			if(mPrimaryVertex.z()<mVzCut[0]||mPrimaryVertex.z()>mVzCut[1]) return false;
			mhnEvents->Fill(2);
			//if(vzVpd-mPrimaryVertex.z()<mVzDiffCut[0]||vzVpd-mPrimaryVertex.z()>mVzDiffCut[1]) return false;
			//mhnEvents->Fill(3);

			int triggerWordMtd = pico->event()->triggerWordMtd();
			for(int i=0;i<18;i++){
				if(triggerWordMtd>>i & 0x1) mhnEvents->Fill(4+i);
			}

			return true;
		}else return false;
	}else{
		LOG_ERROR<<"Wrong trigger selection: "<<mTriggerSelection<<endm;
	}

	return false;
}
//-----------------------------------------------------------------------
void StPicoAnaTreeMaker::fillEventHeader() {
	/* fill event header */

    StPicoEvent* ev = mPicoDst->event() ;
    int runId = ev->runId();
	int centrality = getCentrality();
	getRecenterCor(runId, centrality);

	int counter = mAnaTreeArrays[anaTreeEvent]->GetEntries();
	new((*(mAnaTreeArrays[anaTreeEvent]))[counter]) StEventHeader(*mPicoDst, mRecenterCor);

}

//-----------------------------------------------------------------------
void StPicoAnaTreeMaker::fillTracks() {
	/* fill tracks */

	Int_t nTracks = mPicoDst->numberOfTracks();
	for(int i=0;i<nTracks;i++) {
		//eID
		StPicoTrack *t = (StPicoTrack*)mPicoDst->track(i);
		if(!isGoodTrack(t)) continue;

		int fElecFlag = 0;
		int isTofE = 0;
		int isEmcE = 0;
		if(isTofElectron(t)) isTofE = 1;
		if(isEmcElectron(t)) isEmcE = 1;
		if(isTofE==1&&isEmcE==0) fElecFlag = 1;
		if(isTofE==0&&isEmcE==1) fElecFlag = 2;
		if(isTofE==1&&isEmcE==1)  fElecFlag = 3;

		if(fElecFlag>0){
			int counter = mAnaTreeArrays[anaTreeETrack]->GetEntries();
			new((*(mAnaTreeArrays[anaTreeETrack]))[counter]) StElectronTrack(mPicoDst,t);
		}

		//muID
		if(isMuon(t)){ 
			int counter = mAnaTreeArrays[anaTreeMuTrack]->GetEntries();
			new((*(mAnaTreeArrays[anaTreeMuTrack]))[counter]) StMuonTrack(mPicoDst,t);
		}

		if(mSaveHadron){
			// save hadrons
			if(isHadron(t)){ 
				int counter = mAnaTreeArrays[anaTreeHTrack]->GetEntries();
				new((*(mAnaTreeArrays[anaTreeHTrack]))[counter]) StHadronTrack(mPicoDst,t);
			}
		}
	}
}
//-----------------------------------------------------------------------
void StPicoAnaTreeMaker::fillPairs() {
	/* fill ee pairs */
	int nElec = mAnaTree->numberOfETracks();
	int nMu = mAnaTree->numberOfMuTracks();
	for(int i=0;i<nElec;i++){
		StElectronTrack *eTrk1 = (StElectronTrack*)mAnaTree->eTrack(i);
		for(int j=i+1;j<nElec;j++){
			StElectronTrack *eTrk2 = (StElectronTrack*)mAnaTree->eTrack(j);
			passEEPair(eTrk1,eTrk2,i,j);
		}
	}

	/* fill emu pairs */
	for(int i=0;i<nElec;i++){
		StElectronTrack *eTrk1 = (StElectronTrack*)mAnaTree->eTrack(i);
		for(int j=0;j<nMu;j++){
			StMuonTrack *muTrk2 = (StMuonTrack*)mAnaTree->muTrack(j);
			passEMuPair(eTrk1,muTrk2,i,j);
		}
	}

	/* fill mumu pairs */
	for(int i=0;i<nMu;i++){
		StMuonTrack *muTrk1 = (StMuonTrack*)mAnaTree->muTrack(i);
		for(int j=i+1;j<nMu;j++){
			StMuonTrack *muTrk2 = (StMuonTrack*)mAnaTree->muTrack(j);
			passMuMuPair(muTrk1,muTrk2,i,j);
		}
	}
}

void StPicoAnaTreeMaker::fillEmcTrigger() {
	for(int i=0;i<mPicoDst->numberOfEmcTriggers();i++){
		StPicoEmcTrigger *emc = (StPicoEmcTrigger*)mPicoDst->emcTrigger(i);
		int flag = emc->flag();
		int trgId = emc->id();
		int adc = emc->adc(); //dsm adc
		int adc0 = 0;
		int eId = -1; Float_t pMat = 0.;
		Float_t eMat = 0.;
		Float_t f_dPhi = -999.;
		Float_t f_dEta = -999.;

		int nElec = mAnaTree->numberOfETracks();
		for(int j=0;j<nElec;j++){
			StElectronTrack *eTrk = (StElectronTrack*)mAnaTree->eTrack(j);
			float pt = eTrk->pMom().perp();
			if(pt<1) continue;
			pMat = eTrk->pMom().mag();
			eMat = eTrk->e();
			int trkEmcId = eTrk->towerId();
			if(trkEmcId<1||trkEmcId>4800) continue;
			Float_t triggerTowerPhi, triggerTowerEta;
			Float_t trackProjectionPhi, trackProjectionEta;
			mEmcGeom->getEtaPhi(trgId, triggerTowerEta, triggerTowerPhi);
			mEmcGeom->getEtaPhi(trkEmcId, trackProjectionEta, trackProjectionPhi);

			Float_t dPhi = triggerTowerPhi - trackProjectionPhi;
			while (dPhi > TMath::Pi()) dPhi -= 2 * TMath::Pi();
			while (dPhi < -TMath::Pi()) dPhi += 2 * TMath::Pi();
			Float_t dEta = triggerTowerEta - trackProjectionEta;

			if (fabs(dPhi) < 0.06 && fabs(dEta) < 0.06 && fabs(dPhi) < fabs(f_dPhi) && fabs(dEta) < fabs(f_dEta))
			{
				f_dPhi = dPhi;
				f_dEta = dEta;
				eId = j;
				adc0 = eTrk->adc0();
			}
		}
		int counter = mAnaTreeArrays[anaTreeEmcTrigger]->GetEntries();
		new((*(mAnaTreeArrays[anaTreeEmcTrigger]))[counter]) StEmcTrigger(flag,trgId,adc,eId,adc0);
		if(eId>=0&&eId<60000) {
			StElectronTrack *eTrk = (StElectronTrack*)mAnaTree->eTrack(eId);
			eTrk->setEmcTriggerId(counter);
			mhAdc0vsP->Fill(pMat,adc0);
			mhAdc0vsE->Fill(eMat,adc0);
		}
		mhDsmAdcvsAdc0->Fill(adc0,adc);
		mhDsmAdcvsP->Fill(pMat,adc);
		mhDsmAdcvsE->Fill(eMat,adc);
	}
}

//-------------------------------------------------------------
void StPicoAnaTreeMaker::fillMtdTrigger() {
		StPicoMtdTrigger *mtd = (StPicoMtdTrigger*)mPicoDst->mtdTrigger(0);
		
		int counter = mAnaTreeArrays[anaTreeMtdTrigger]->GetEntries();
		new((*(mAnaTreeArrays[anaTreeMtdTrigger]))[counter]) StMtdTrigger(mtd);
}

//-------------------------------------------------------------
Bool_t StPicoAnaTreeMaker::isGoodTrack(StPicoTrack* t)
{
	if(!t) return false;
	mhnTracks->Fill(0);

	double p = 0.,pt=0.,eta=-999.,ratio=0.;
	int nHitsFit = 0, nHitsDedx = 0, nHitsMax = 0; 
	int nHitsMapHFT = 0;

	p = t->pMom().mag();
	pt = t->pMom().perp();
	eta = t->pMom().pseudoRapidity();
	StPhysicalHelixD helix = t->helix();
	StThreeVectorF vertexPos = mPicoDst->event()->primaryVertex();
	Double_t thePath = helix.pathLength(vertexPos);
	StThreeVectorF dcaPos = helix.at(thePath);
	Float_t dca = (dcaPos-vertexPos).mag();
	nHitsFit = t->nHitsFit();
	nHitsMax = t->nHitsMax();
	nHitsDedx = t->nHitsDedx();
	ratio = 1.*t->nHitsFit()/nHitsMax;
	nHitsMapHFT = t->nHitsMapHFT();
	bool isHFTTrack = t->isHFTTrack();

	if(pt<mPtCut[0]||pt>mPtCut[1]) return false;
	mhnTracks->Fill(1);
	if(eta<mEtaCut[0]||eta>mEtaCut[1]) return false;
	mhnTracks->Fill(2);
	//if(dca<mDcaCut[0]||dca>mDcaCut[1]) return false;
	mhnTracks->Fill(3);
	if(nHitsFit<mnHitsFitCut[0]||nHitsFit>mnHitsFitCut[1]) return false;
	mhnTracks->Fill(4);
	if(nHitsDedx<mnHitsDedxCut[0]||nHitsDedx>mnHitsDedxCut[1]) return false;
	mhnTracks->Fill(5);
	if(ratio<mRatioCut[0]||ratio>mRatioCut[1]) return false;
	mhnTracks->Fill(6);
	if(mHFTTrackCut && !isHFTTrack ) return false;
	mhnTracks->Fill(7);

	Float_t nSigE = t->nSigmaElectron();
	Float_t nSigPi = t->nSigmaPion();
	mhnSigEvsP->Fill(p,nSigE); 
	mhnSigEvsPt->Fill(pt,nSigE); 

	mhnSigPivsP->Fill(p,nSigPi); 
	mhnSigPivsPt->Fill(pt,nSigPi); 

	mhnSigE2PivsP->Fill(p,t->nSigmaElectron()-2.*t->nSigmaPion());
	mhnSigEPivsP->Fill(p,t->nSigmaElectron()-t->nSigmaPion());
	mhnSigEKvsP->Fill(p,t->nSigmaElectron()-t->nSigmaKaon());
	mhnSigEPvsP->Fill(p,t->nSigmaElectron()-t->nSigmaProton());

	mhnSigE2PivsPt->Fill(pt,t->nSigmaElectron()-2.*t->nSigmaPion());
	mhnSigEPivsPt->Fill(pt,t->nSigmaElectron()-t->nSigmaPion());
	mhnSigEKvsPt->Fill(pt,t->nSigmaElectron()-t->nSigmaKaon());
	mhnSigEPvsPt->Fill(pt,t->nSigmaElectron()-t->nSigmaProton());

	double m2 = -999., invBeta=-999.;
	int index2TofPid = t->bTofPidTraitsIndex();
	if (index2TofPid>=0){
		StPicoBTofPidTraits *tofPid = mPicoDst->btofPidTraits(index2TofPid);
		Float_t beta = tofPid->btofBeta();
		if(beta!=0) invBeta = 1./beta;
		Float_t mom = t->pMom().mag();
		if(beta!=0){
			m2 = mom*mom*( 1.0/(beta*beta)-1.0);
		}
	}

	mhM2vsP->Fill(p,m2);
	mhM2vsPt->Fill(p,m2);

	if(fabs(m2-0.0193)<0.02){ 
		mhnSigEPivsPwTOF->Fill(p,t->nSigmaElectron());
		mhnSigEPivsPtwTOF->Fill(pt,t->nSigmaElectron());
	}
	if(fabs(m2-0.243)<0.1){ 
		mhnSigEKvsPwTOF->Fill(p,t->nSigmaElectron());
		mhnSigEKvsPtwTOF->Fill(pt,t->nSigmaElectron());
	}
	if(fabs(m2-0.867)<0.2){ 
		mhnSigEPvsPwTOF->Fill(p,t->nSigmaElectron());
		mhnSigEPvsPtwTOF->Fill(pt,t->nSigmaElectron());
	}

	return true;

}

//-------------------------------------------------------------
Bool_t StPicoAnaTreeMaker::isTofElectron(StPicoTrack* t)
{
	/* TOF+TPC electron PID*/
	Float_t p = 0., pt = 0., invBeta = -999., localY = -999., localZ = -999., m2=-999.;
	int tray = -1;
	int index2TofPid = t->bTofPidTraitsIndex();
	if (index2TofPid>=0){
		StPicoBTofPidTraits *tofPid = mPicoDst->btofPidTraits(index2TofPid);
		Float_t beta = tofPid->btofBeta();
		if(beta>0) invBeta = 1./beta;
		else return false;
		Float_t mom = t->pMom().mag();
		if(beta>0){
			m2 = mom*mom*( 1.0/(beta*beta)-1.0);
		}
		localY = tofPid->btofYLocal();
		localZ = tofPid->btofZLocal();
		tray = tofPid->btofCellId()/192;
	}else{
		return false;
	}

	mhnTracks->Fill(8);
	p = t->pMom().mag();
	pt = t->pMom().perp();
	Float_t nSigE = t->nSigmaElectron();
	mhInvBetavsP->Fill(p, invBeta);
	mhInvBetavsPt->Fill(pt, invBeta);

	mhTofLocalYvsTray->Fill(tray, localY);
	mhTofLocalZvsTray->Fill(tray, localZ);

	if(invBeta<mEInvBetaCut[0] || invBeta>mEInvBetaCut[1]) return false;
	mhnTracks->Fill(9);
	//if(localY<mELocalYCut[0] || localY>mELocalYCut[1]) return false;
	//mhnTracks->Fill(10);
	//if(localZ<mELocalZCut[0] || localZ>mELocalZCut[1]) return false;
	//mhnTracks->Fill(11);

	mhTofEnSigEvsPCut->Fill(p, nSigE);
	mhTofEnSigEvsPtCut->Fill(pt, nSigE);

	if(nSigE<mEnSigECut[0] || nSigE>mEnSigECut[1]) return false;
	mhnTracks->Fill(12);

	return true;
}

//-------------------------------------------------------------
Bool_t StPicoAnaTreeMaker::isEmcElectron(StPicoTrack* t)
{
	/* EMC+TPC electron PID*/
	int index2EmcPid = t->emcPidTraitsIndex();
	Float_t e = -999., zDist = -999., phiDist = -999., nEta = 0, nPhi = 0;
	Float_t p = t->pMom().mag();
	Float_t pt = t->pMom().perp();
	Float_t eta = t->pMom().pseudoRapidity();
	Float_t nSigE = t->nSigmaElectron();
	if (index2EmcPid>=0){
		StPicoEmcPidTraits *emcPid = mPicoDst->emcPidTraits(index2EmcPid);
		e = emcPid->e0();
		zDist = emcPid->zDist();
		phiDist = emcPid->phiDist();
		nEta = emcPid->nEta();
		nPhi = emcPid->nPhi();
	}else{
		return false;
	}
	mhnTracks->Fill(13);

	Float_t pve = 0;
	Float_t evp = 0;
	if(e!=0) pve = p/e;
	if(p!=0) evp = e/p;


	mhnEtavsnPhi->Fill(nPhi,nEta);
	mhZDistvsPt->Fill(pt, zDist);
	mhPhiDistvsPt->Fill(pt, phiDist);
	mhEvPvsPt->Fill(pt,evp);
	mhPvEvsPt->Fill(pt,pve);

	if(pt<mEmcEPtCut[0] || pt>mEmcEPtCut[1]) return false;
	mhnTracks->Fill(14);
	if(eta<mEmcEEtaCut[0] || eta>mEmcEEtaCut[1]) return false;
	mhnTracks->Fill(15);
	if(pve<mEmcEPveCut[0] || pve<mEmcEPveCut[1]) return false;
	mhnTracks->Fill(16);
	//if(zDist<mEZDistCut[0] || zDist>mEZDistCut[1]) return false;
	mhnTracks->Fill(17);
	//if(phiDist<mEPhiDistCut[0] || phiDist>mEPhiDistCut[1]) return false;
	mhnTracks->Fill(18);

	mhEmcEnSigEvsPCut->Fill(p,nSigE);
	mhEmcEnSigEvsPtCut->Fill(pt,nSigE);

	//if(nEta<mEnEtaCut[0] || nEta>mEnEtaCut[1]) return false;
	mhnTracks->Fill(19);
	//if(nPhi<mEnPhiCut[0] || nPhi>mEnPhiCut[1]) return false;
	mhnTracks->Fill(20);

	mhSmdEnSigEvsPCut->Fill(p,nSigE);
	mhSmdEnSigEvsPtCut->Fill(pt,nSigE);

	if(nSigE<mEnSigECut[0] || nSigE>mEnSigECut[1]) return false;
	mhnTracks->Fill(21);

	return true;
}

//-------------------------------------------------------------
Bool_t StPicoAnaTreeMaker::isMuon(StPicoTrack* t)
{
	int index2MtdPid = t->mtdPidTraitsIndex();
	Float_t dT = -999., dZ = -999., dY = -999.;
	int backleg = -1, module = -1; 
	if (index2MtdPid>=0){
		StPicoMtdPidTraits *mtdPid = mPicoDst->mtdPidTraits(index2MtdPid);
		dT = mtdPid->deltaTimeOfFlight();
		dZ = mtdPid->deltaZ();
		dY = mtdPid->deltaY();
		backleg = mtdPid->backleg();
		module = mtdPid->module();
	}else{
		return false;
	}
	mhnTracks->Fill(22);

	int mod = -1;
	if(backleg>0&&module>0) mod = (backleg-1)*5+module;

	Float_t p = t->pMom().mag();
	Float_t pt = t->pMom().perp();
	Float_t eta = t->pMom().pseudoRapidity();
	Float_t nSigPi = t->nSigmaPion();

	if(pt<mMuPtCut[0] || pt>mMuPtCut[1]) return false;
	mhnTracks->Fill(22);
	if(eta<mMuEtaCut[0] || eta>mMuEtaCut[1]) return false;
	mhnTracks->Fill(23);

	mhMtdMunSigPivsP->Fill(p, nSigPi);
	mhMtdMunSigPivsPt->Fill(pt, nSigPi);

	if(nSigPi>mMunSigPiCut[0] && nSigPi<mMunSigPiCut[1]){
		mhMtddZvsPt->Fill(pt,dZ);
		mhMtddYvsPt->Fill(pt,dY);
		mhMtddTvsPt->Fill(pt,dT);
		mhMtddZvsMod->Fill(mod, dZ);
		mhMtddYvsMod->Fill(mod, dY);
		mhMtddTvsMod->Fill(mod, dT);
	}

	if(dT<mMudTCut[0] || dT>mMudTCut[1]) return false;
	mhnTracks->Fill(24);
	if(dY<mMudYCut[0] || dY>mMudYCut[1]) return false;
	mhnTracks->Fill(25);
	if(dZ<mMudZCut[0] || dZ>mMudZCut[1]) return false;
	mhnTracks->Fill(26);
	mhMtdMunSigPivsPCut->Fill(p,nSigPi);
	mhMtdMunSigPivsPtCut->Fill(pt,nSigPi);

	if(nSigPi<mMunSigPiCut[0] || nSigPi>mMunSigPiCut[1]) return false;
	mhnTracks->Fill(27);

	return true;
}
//---------------------------------------------------------------
Bool_t StPicoAnaTreeMaker::passEEPair(StElectronTrack *t1, StElectronTrack *t2, Int_t index1, Int_t index2)
{
	/* ee pair cut */

	if(mDauEEtaCut[0]<mEEtaCut[0] || mDauEEtaCut[1]>mEEtaCut[1]){
		LOG_WARN<<"Error: wider daughter electron eta cut in pairing, please check your cuts"<<endm;
		return false;
	}

	StThreeVectorF mom(0,0,0);
	UShort_t dauId1 = t1->id();
	UShort_t dauId2 = t2->id();
	Char_t dauCharge1 = t1->charge(); 
	Char_t dauCharge2 = t2->charge();
	Float_t dauPt1 = t1->gPt();
	Float_t dauPt2 = t2->gPt();
	Float_t dauEta1 = t1->gEta();
	Float_t dauEta2 = t2->gEta();
	Float_t dauPhi1 = t1->gPhi();
	Float_t dauPhi2 = t2->gPhi();
	bool isHft1 = t1->isHFTTrack();
	bool isHft2 = t2->isHFTTrack();

	Float_t beta1 = t1->beta();
	Float_t beta2 = t2->beta();
	Float_t invBeta1 = -999, invBeta2 = -999;
	if(beta1!=0) invBeta1 = 1./beta1; 
	if(beta2!=0) invBeta2 = 1./beta2; 

	Float_t nSigE1 = t1->nSigmaElectron();
	Float_t nSigE2 = t2->nSigmaElectron();

	Float_t pve1 = t1->pve();
	Float_t pve2 = t2->pve();
	Float_t zDist1 = t1->zDist();
	Float_t zDist2 = t2->zDist();
	Float_t phiDist1 = t1->phiDist();
	Float_t phiDist2 = t2->phiDist();
	UChar_t nEta1 = t1->nEta();
	UChar_t nEta2 = t2->nEta();
	UChar_t nPhi1 = t1->nPhi();
	UChar_t nPhi2 = t2->nPhi();
	Int_t isBsmd1 = 0, isBsmd2 = 0;
	if(nEta1>mEnEtaCut[0] && nEta1<mEnEtaCut[1]
			&& nPhi1>mEnPhiCut[0] && nPhi1>mEnPhiCut[1]
			&& zDist1>mEZDistCut[0] && zDist1<mEZDistCut[1]
			&& phiDist1>mEPhiDistCut[0] && phiDist1<mEPhiDistCut[1]){
		isBsmd1 = 1;
	}
	if(nEta2>mEnEtaCut[0] && nEta2<mEnEtaCut[1]
			&& nPhi2>mEnPhiCut[0] && nPhi2>mEnPhiCut[1]
			&& zDist2>mEZDistCut[0] && zDist2<mEZDistCut[1]
			&& phiDist2>mEPhiDistCut[0] && phiDist2<mEPhiDistCut[1]){
		isBsmd2 = 1;
	}

	//if(dauPt1<mDauEPtCut[0]||dauPt1>mDauEPtCut[1]) return false;
	//if(dauPt2<mDauEPtCut[0]||dauPt2>mDauEPtCut[1]) return false;
	//if(dauEta1<mDauEEtaCut[0]||dauEta1>mDauEEtaCut[1]) return false;
	//if(dauEta2<mDauEEtaCut[0]||dauEta2>mDauEEtaCut[1]) return false;
	//if(invBeta1<mEInvBetaCut[0]||invBeta1>mEInvBetaCut[1]) return false;
	//if(invBeta2<mEInvBetaCut[0]||invBeta2>mEInvBetaCut[1]) return false;
	//if(nSigE1<mEnSigECut[0]||nSigE1>mEnSigECut[1]) return false;
	//if(nSigE2<mEnSigECut[0]||nSigE2>mEnSigECut[1]) return false;


	StPhysicalHelixD helix1 = t1->helix();
	StPhysicalHelixD helix2 = t2->helix();
	StThreeVectorF vertexPos = mPicoDst->event()->primaryVertex();
	Double_t thePath1 = helix1.pathLength(vertexPos);
	Double_t thePath2 = helix2.pathLength(vertexPos);
	StThreeVectorF dcaPos1 = helix1.at(thePath1);
	StThreeVectorF dcaPos2 = helix2.at(thePath2);
	Float_t dauDcaToVtx1 = (dcaPos1-vertexPos).mag();
	Float_t dauDcaToVtx2 = (dcaPos2-vertexPos).mag();

	//if(dauDcaToVtx1<mDauEDcaToVtxCut[0]||dauDcaToVtx1>mDauEDcaToVtxCut[1]) return false;
	//if(dauDcaToVtx2<mDauEDcaToVtxCut[0]||dauDcaToVtx2>mDauEDcaToVtxCut[1]) return false;

	pairD s = helix1.pathLengths(helix2);
	StThreeVectorF pos1 = helix1.at(s.first);
	StThreeVectorF pos2 = helix2.at(s.second);
	StThreeVectorF pairPos = (pos1+pos2)/2.;
	Float_t dauDcaDist = (pos2-pos1).mag();
	//if(dauDcaDist<mDauEDcaDistCut[0]||dauDcaDist>mDauEDcaDistCut[1]) return false;

	float bField = mPicoDst->event()->bField();
	StThreeVectorF mom1 = helix1.momentumAt(s.first,bField*kilogauss);
	StThreeVectorF mom2 = helix2.momentumAt(s.second,bField*kilogauss);
	StLorentzVectorF dau1(mom1,mom1.massHypothesis(ElectronMass));
	StLorentzVectorF dau2(mom2,mom2.massHypothesis(ElectronMass));
	StLorentzVectorF pair;
	pair = dau1+dau2;
	Float_t pairPt = pair.perp();
	Float_t pairEta = pair.pseudoRapidity();
	Float_t pairPhi = pair.phi();
	Float_t pairMass = pair.m();
	Float_t pairY = pair.rapidity();
	StPhysicalHelixD pairHelix = StPhysicalHelixD(pair.vect(),pairPos,bField*kilogauss,0);
	StThreeVectorF pairDcaPos = pairHelix.at(pairHelix.pathLength(vertexPos));
	StThreeVectorF const vtxToV0 = pairPos - vertexPos;
	Float_t pairDca = (pairDcaPos-vertexPos).mag();
	Float_t pointingAngle = vtxToV0.angle(pair.vect());
	Float_t pairDecayL = vtxToV0.mag();

	StThreeVectorF pmom1 = t1->pMom();
	StLorentzVectorF pdau1(pmom1,pmom1.massHypothesis(ElectronMass));
	StThreeVectorF pmom2 = t2->pMom();
	StLorentzVectorF pdau2(pmom2,pmom2.massHypothesis(ElectronMass));
	StLorentzVectorF ppair;
	ppair = pdau1+pdau2;
	Float_t pairPPt   = ppair.perp();
	Float_t pairPEta  = ppair.pseudoRapidity();
	Float_t pairPPhi  = ppair.phi();
	Float_t pairPMass = ppair.m();

	StThreeVectorF pairDecayLxy(vtxToV0.x(),vtxToV0.y(),0);
	StThreeVectorF pairPxy(pair.px(),pair.py(),0);
	Float_t Lxy = pairDecayLxy.dot(pairPxy)/pairPxy.perp();
	Float_t pairCtau = -999.;
	if(pair.perp()!=0){
		pairCtau = pairMass*Lxy/pair.perp();
	}

	// calculate cosThetaStar
	StLorentzVectorF const pairMomReverse(-pair.px(), -pair.py(), -pair.pz(), pair.e());
	StLorentzVectorF const dau1MomStar = dau1.boost(pairMomReverse);
	Float_t cosThetaStar = std::cos(dau1MomStar.vect().angle(pair.vect()));

	int rndSeed = (int)(dauPt1+dauPt2)*1000;
	gRandom->SetSeed(rndSeed);
	// calculate phiV
	StThreeVectorF e1Mom,e2Mom;
	if(t1->charge()>0&&t2->charge()<0){
		e1Mom = t2->pMom();//e-
		e2Mom = t1->pMom();//e+
	}else if(t1->charge()<0&&t2->charge()>0){
		e1Mom = t1->pMom();//e-
		e2Mom = t2->pMom();//e+
	}else{
		if(gRandom->Uniform(0,1)>0.5){
			e1Mom = t1->pMom();
			e2Mom = t2->pMom();
		}else{
			e1Mom = t2->pMom();
			e2Mom = t1->pMom();
		}
	}
	float mN = 1.;
	if(bField<0.) mN = -1.;
	StThreeVector<float> pu=e1Mom+e2Mom;
	StThreeVector<float> pv=e1Mom.cross(e2Mom);
	StThreeVector<float> pw=pu.cross(pv);
	StThreeVector<float> pnz(0.,0.,mN);
	StThreeVector<float> pwc=pu.cross(pnz);
	Float_t pairPhiV = pw.angle(pwc);

	//if(pairDca<mPairDcaCut[0] || pairDca>mPairDcaCut[1]) return false;
	//if(pairDecayL<mPairDecayLCut[0] || pairDecayL>mPairDecayLCut[1]) return false;
	//if(pairY<mPairYCut[0] || pairY>mPairYCut[1]) return false;
	//if(pairMass<mPairMassCut[0] || pairMass>mPairMassCut[1]) return false;
	//if(cosThetaStar<mCosThetaStarCut[0] || cosThetaStar>mCosThetaStarCut[1]) return false;
	//if(pointingAngle<mPointingAngleCut[0] || pointingAngle>mPointingAngleCut[1]) return false;

	int counter = mAnaTreeArrays[anaTreeEEPair]->GetEntries();
	new((*(mAnaTreeArrays[anaTreeEEPair]))[counter]) StEEPair(index1,  index2,  dauCharge1,  dauCharge2,
			//		dauPt1,  dauPt2,  dauEta1,  dauEta2,  dauPhi1,  dauPhi2,
			//		invBeta1, invBeta2, nSigE1, nSigE2, pve1, pve2, 
			//		isHft1, isHft2, isBsmd1, isBsmd2,
			//		dauDcaToVtx1,   dauDcaToVtx2,   
			dauDcaDist,
			pairDca,   pairDecayL,   pairY,   cosThetaStar,   pointingAngle,   pairPhiV,   
			pairPt,   pairEta,   pairPhi,   pairMass, 
			pairPPt,   pairPEta,   pairPPhi,   pairPMass, 
			pairCtau, pairPos.x(), pairPos.y(), pairPos.z());

	return true;
}
//---------------------------------------------------------------
Bool_t StPicoAnaTreeMaker::passEMuPair(StElectronTrack *t1, StMuonTrack *t2, Int_t index1, Int_t index2)
{
	/* emu pair cut */
	if(mDauEEtaCut[0]<mEEtaCut[0] || mDauEEtaCut[1]>mEEtaCut[1]){
		LOG_WARN<<"Error: wider daughter electron eta cut in pairing, please check your cuts"<<endm;
		return false;
	}
	if(mDauMuEtaCut[0]<mMuEtaCut[0] || mDauMuEtaCut[1]>mMuEtaCut[1]){
		LOG_WARN<<"Error: wider daughter electron eta cut in pairing, please check your cuts"<<endm;
		return false;
	}


	StThreeVectorF mom(0,0,0);
	UShort_t dauId1 = t1->id();
	UShort_t dauId2 = t2->id();
	Char_t dauCharge1 = t1->charge(); 
	Char_t dauCharge2 = t2->charge();
	Float_t dauPt1 = t1->gPt();
	Float_t dauPt2 = t2->gPt();
	Float_t dauEta1 = t1->gEta();
	Float_t dauEta2 = t2->gEta();
	Float_t dauPhi1 = t1->gPhi();
	Float_t dauPhi2 = t2->gPhi();
	bool isHft1 = t1->isHFTTrack();
	bool isHft2 = t2->isHFTTrack();

	Float_t beta1 = t1->beta();
	Float_t invBeta1 = -999;
	if(beta1!=0) invBeta1 = 1./beta1; 

	Float_t nSigE1 = t1->nSigmaElectron();
	Float_t nSigPi2 = t2->nSigmaPion();

	Float_t daudT2 = t2->deltaTimeOfFlight();
	Float_t daudZ2 = t2->deltaZ();

	Float_t pve1 = t1->pve();
	Float_t zDist1 = t1->zDist();
	Float_t phiDist1 = t1->phiDist();
	UChar_t nEta1 = t1->nEta();
	UChar_t nPhi1 = t1->nPhi();
	Int_t isBsmd1 = 0;
	if(nEta1>mEnEtaCut[0] && nEta1<mEnEtaCut[1]
			&& nPhi1>mEnPhiCut[0] && nPhi1>mEnPhiCut[1]
			&& zDist1>mEZDistCut[0] && zDist1<mEZDistCut[1]
			&& phiDist1>mEPhiDistCut[0] && phiDist1<mEPhiDistCut[1]){
		isBsmd1 = 1;
	}

	//if(dauPt1<mDauEPtCut[0]||dauPt1>mDauEPtCut[1]) return false;
	//if(dauPt2<mDauMuPtCut[0]||dauPt2>mDauMuPtCut[1]) return false;

	//if(dauEta1<mDauEEtaCut[0]||dauEta1>mDauEEtaCut[1]) return false;
	//if(dauEta2<mDauMuEtaCut[0]||dauEta2>mDauMuEtaCut[1]) return false;
	//if(invBeta1<mEInvBetaCut[0]||invBeta1>mEInvBetaCut[1]) return false;
	//if(nSigE1<mEnSigECut[0]||nSigE1>mEnSigECut[1]) return false;
	//if(nSigPi2<mMunSigPiCut[0]||nSigPi2>mMunSigPiCut[1]) return false;
	//if(daudT2<mMudTCut[0]||daudT2>mMudTCut[1]) return false;
	//if(daudZ2<mMudZCut[0]||daudZ2>mMudZCut[1]) return false;

	StPhysicalHelixD helix1 = t1->helix();
	StPhysicalHelixD helix2 = t2->helix();
	StThreeVectorF vertexPos = mPicoDst->event()->primaryVertex();
	Double_t thePath1 = helix1.pathLength(vertexPos);
	Double_t thePath2 = helix2.pathLength(vertexPos);
	StThreeVectorF dcaPos1 = helix1.at(thePath1);
	StThreeVectorF dcaPos2 = helix2.at(thePath2);
	Float_t dauDcaToVtx1 = (dcaPos1-vertexPos).mag();
	Float_t dauDcaToVtx2 = (dcaPos2-vertexPos).mag();

	//if(dauDcaToVtx1<mDauEDcaToVtxCut[0]||dauDcaToVtx1>mDauEDcaToVtxCut[1]) return false;
	//if(dauDcaToVtx2<mDauMuDcaToVtxCut[0]||dauDcaToVtx2>mDauMuDcaToVtxCut[1]) return false;

	pairD s = helix1.pathLengths(helix2);
	StThreeVectorF pos1 = helix1.at(s.first);
	StThreeVectorF pos2 = helix2.at(s.second);
	StThreeVectorF pairPos = (pos1+pos2)/2.;
	Float_t dauDcaDist = (pos2-pos1).mag();
	//if(dauDcaDist<mDauEDcaDistCut[0]||dauDcaDist>mDauEDcaDistCut[1]) return false;

	float bField = mPicoDst->event()->bField();
	StThreeVectorF mom1 = helix1.momentumAt(s.first,bField*kilogauss);
	StThreeVectorF mom2 = helix2.momentumAt(s.second,bField*kilogauss);
	StLorentzVectorF dau1(mom1,mom1.massHypothesis(ElectronMass));
	StLorentzVectorF dau2(mom2,mom2.massHypothesis(MuonMass));
	StLorentzVectorF pair;
	pair = dau1+dau2;
	Float_t pairPt = pair.perp();
	Float_t pairEta = pair.pseudoRapidity();
	Float_t pairPhi = pair.phi();
	Float_t pairMass = pair.m();
	Float_t pairY = pair.rapidity();
	StPhysicalHelixD pairHelix = StPhysicalHelixD(pair.vect(),pairPos,bField*kilogauss,0);
	StThreeVectorF pairDcaPos = pairHelix.at(pairHelix.pathLength(vertexPos));
	StThreeVectorF const vtxToV0 = pairPos - vertexPos;
	Float_t pairDca = (pairDcaPos-vertexPos).mag();
	Float_t pointingAngle = vtxToV0.angle(pair.vect());
	Float_t pairDecayL = vtxToV0.mag();

	StThreeVectorF pmom1 = t1->pMom();
	StLorentzVectorF pdau1(pmom1,pmom1.massHypothesis(ElectronMass));
	StThreeVectorF pmom2 = t2->pMom();
	StLorentzVectorF pdau2(pmom2,pmom2.massHypothesis(MuonMass));
	StLorentzVectorF ppair;
	ppair = pdau1+pdau2;
	Float_t pairPPt   = ppair.perp();
	Float_t pairPEta  = ppair.pseudoRapidity();
	Float_t pairPPhi  = ppair.phi();
	Float_t pairPMass = ppair.m();

	// calculate cosThetaStar
	StLorentzVectorF const pairMomReverse(-pair.px(), -pair.py(), -pair.pz(), pair.e());
	StLorentzVectorF const dau1MomStar = dau1.boost(pairMomReverse);
	Float_t cosThetaStar = std::cos(dau1MomStar.vect().angle(pair.vect()));

	//if(pairDca<mPairDcaCut[0] || pairDca>mPairDcaCut[1]) return false;
	//if(pairDecayL<mPairDecayLCut[0] || pairDecayL>mPairDecayLCut[1]) return false;
	//if(pairY<mPairYCut[0] || pairY>mPairYCut[1]) return false;
	//if(pairMass<mPairMassCut[0] || pairMass>mPairMassCut[1]) return false;
	//if(cosThetaStar<mCosThetaStarCut[0] || cosThetaStar>mCosThetaStarCut[1]) return false;
	//if(pointingAngle<mPointingAngleCut[0] || pointingAngle>mPointingAngleCut[1]) return false;

	int counter = mAnaTreeArrays[anaTreeEMuPair]->GetEntries();
	new((*(mAnaTreeArrays[anaTreeEMuPair]))[counter]) StEMuPair(dauId1,  dauId2,  index1,  index2,  dauCharge1,  dauCharge2,
			//		dauPt1,  dauPt2,  dauEta1,  dauEta2,  dauPhi1,  dauPhi2,
			//		invBeta1, daudT2, nSigE1, nSigPi2, pve1, daudZ2, isBsmd1, isHft1, isHft2,
			//		dauDcaToVtx1,   dauDcaToVtx2,   
			dauDcaDist,
			pairDca,   pairDecayL,   pairY,   cosThetaStar,   pointingAngle,   
			pairPt,   pairEta,   pairPhi,   pairMass,
			pairPPt,   pairPEta,   pairPPhi,   pairPMass);


	return true;
}

//---------------------------------------------------------------
Bool_t StPicoAnaTreeMaker::passMuMuPair(StMuonTrack *t1, StMuonTrack *t2, Int_t index1, Int_t index2)
{
	/* mumu pair cut */
	if(mDauMuEtaCut[0]<mMuEtaCut[0] || mDauMuEtaCut[1]>mMuEtaCut[1]){
		LOG_WARN<<"Error: wider daughter electron eta cut in pairing, please check your cuts"<<endm;
		return false;
	}

	StThreeVectorF mom(0,0,0);
	UShort_t dauId1 = t1->id();
	UShort_t dauId2 = t2->id();
	Char_t dauCharge1 = t1->charge(); 
	Char_t dauCharge2 = t2->charge();
	Float_t dauPt1 = t1->gPt();
	Float_t dauPt2 = t2->gPt();
	Float_t dauEta1 = t1->gEta();
	Float_t dauEta2 = t2->gEta();
	Float_t dauPhi1 = t1->gPhi();
	Float_t dauPhi2 = t2->gPhi();
	bool isHft1 = t1->isHFTTrack();
	bool isHft2 = t2->isHFTTrack();

	Float_t nSigPi1 = t1->nSigmaPion();
	Float_t nSigPi2 = t2->nSigmaPion();

	Float_t daudT1 = t1->deltaTimeOfFlight();
	Float_t daudT2 = t2->deltaTimeOfFlight();
	Float_t daudZ1 = t1->deltaZ();
	Float_t daudZ2 = t2->deltaZ();

	//if(dauPt1<mDauMuPtCut[0]||dauPt1>mDauMuPtCut[1]) return false;
	//if(dauPt2<mDauMuPtCut[0]||dauPt2>mDauMuPtCut[1]) return false;
	//if(dauEta1<mDauMuEtaCut[0]||dauEta1>mDauMuEtaCut[1]) return false;
	//if(dauEta2<mDauMuEtaCut[0]||dauEta2>mDauMuEtaCut[1]) return false;
	//if(nSigPi1<mMunSigPiCut[0]||nSigPi1>mMunSigPiCut[1]) return false;
	//if(nSigPi2<mMunSigPiCut[0]||nSigPi2>mMunSigPiCut[1]) return false;
	//if(daudT1<mMudTCut[0]||daudT1>mMudTCut[1]) return false;
	//if(daudT2<mMudTCut[0]||daudT2>mMudTCut[1]) return false;
	//if(daudZ1<mMudZCut[0]||daudZ1>mMudZCut[1]) return false;
	//if(daudZ2<mMudZCut[0]||daudZ2>mMudZCut[1]) return false;

	StPhysicalHelixD helix1 = t1->helix();
	StPhysicalHelixD helix2 = t2->helix();
	StThreeVectorF vertexPos = mPicoDst->event()->primaryVertex();
	Double_t thePath1 = helix1.pathLength(vertexPos);
	Double_t thePath2 = helix2.pathLength(vertexPos);
	StThreeVectorF dcaPos1 = helix1.at(thePath1);
	StThreeVectorF dcaPos2 = helix2.at(thePath2);
	Float_t dauDcaToVtx1 = (dcaPos1-vertexPos).mag();
	Float_t dauDcaToVtx2 = (dcaPos2-vertexPos).mag();

	//if(dauDcaToVtx1<mDauMuDcaToVtxCut[0]||dauDcaToVtx1>mDauMuDcaToVtxCut[1]) return false;
	//if(dauDcaToVtx2<mDauMuDcaToVtxCut[0]||dauDcaToVtx2>mDauMuDcaToVtxCut[1]) return false;

	pairD s = helix1.pathLengths(helix2);
	StThreeVectorF pos1 = helix1.at(s.first);
	StThreeVectorF pos2 = helix2.at(s.second);
	StThreeVectorF pairPos = (pos1+pos2)/2.;
	Float_t dauDcaDist = (pos2-pos1).mag();
	//if(dauDcaDist<mDauEDcaDistCut[0]||dauDcaDist>mDauEDcaDistCut[1]) return false;

	float bField = mPicoDst->event()->bField();
	StThreeVectorF mom1 = helix1.momentumAt(s.first,bField*kilogauss);
	StThreeVectorF mom2 = helix2.momentumAt(s.second,bField*kilogauss);
	StLorentzVectorF dau1(mom1,mom1.massHypothesis(MuonMass));
	StLorentzVectorF dau2(mom2,mom2.massHypothesis(MuonMass));
	StLorentzVectorF pair;
	pair = dau1+dau2;
	Float_t pairPt = pair.perp();
	Float_t pairEta = pair.pseudoRapidity();
	Float_t pairPhi = pair.phi();
	Float_t pairMass = pair.m();
	Float_t pairY = pair.rapidity();
	StPhysicalHelixD pairHelix = StPhysicalHelixD(pair.vect(),pairPos,bField*kilogauss,0);
	StThreeVectorF pairDcaPos = pairHelix.at(pairHelix.pathLength(vertexPos));
	StThreeVectorF const vtxToV0 = pairPos - vertexPos;
	Float_t pairDca = (pairDcaPos-vertexPos).mag();
	Float_t pointingAngle = vtxToV0.angle(pair.vect());
	Float_t pairDecayL = vtxToV0.mag();

	StThreeVectorF pmom1 = t1->pMom();
	StLorentzVectorF pdau1(pmom1,pmom1.massHypothesis(MuonMass));
	StThreeVectorF pmom2 = t2->pMom();
	StLorentzVectorF pdau2(pmom2,pmom2.massHypothesis(MuonMass));
	StLorentzVectorF ppair;
	ppair = pdau1+pdau2;
	Float_t pairPPt   = ppair.perp();
	Float_t pairPEta  = ppair.pseudoRapidity();
	Float_t pairPPhi  = ppair.phi();
	Float_t pairPMass = ppair.m();

	StThreeVectorF pairDecayLxy(vtxToV0.x(),vtxToV0.y(),0);
	StThreeVectorF pairPxy(pair.px(),pair.py(),0);
	Float_t Lxy = pairDecayLxy.dot(pairPxy)/pairPxy.perp();
	Float_t pairCtau = -999.;
	if(pair.perp()!=0){
		pairCtau = pairMass*Lxy/pair.perp();
	}

	// calculate cosThetaStar
	StLorentzVectorF const pairMomReverse(-pair.px(), -pair.py(), -pair.pz(), pair.e());
	StLorentzVectorF const dau1MomStar = dau1.boost(pairMomReverse);
	Float_t cosThetaStar = std::cos(dau1MomStar.vect().angle(pair.vect()));

	//if(pairDca<mPairDcaCut[0] || pairDca>mPairDcaCut[1]) return false;
	//if(pairDecayL<mPairDecayLCut[0] || pairDecayL>mPairDecayLCut[1]) return false;
	//if(pairY<mPairYCut[0] || pairY>mPairYCut[1]) return false;
	//if(pairMass<mPairMassCut[0] || pairMass>mPairMassCut[1]) return false;
	//if(cosThetaStar<mCosThetaStarCut[0] || cosThetaStar>mCosThetaStarCut[1]) return false;
	//if(pointingAngle<mPointingAngleCut[0] || pointingAngle>mPointingAngleCut[1]) return false;

	int counter = mAnaTreeArrays[anaTreeMuMuPair]->GetEntries();
	new((*(mAnaTreeArrays[anaTreeMuMuPair]))[counter]) StMuMuPair(dauId1,  dauId2,  index1,  index2,  dauCharge1,  dauCharge2,
			//		dauPt1,  dauPt2,  dauEta1,  dauEta2,  dauPhi1,  dauPhi2,
			//		daudT1, daudT2, nSigPi1, nSigPi2, daudZ1, daudZ2, isHft1, isHft2,
			//		dauDcaToVtx1,   dauDcaToVtx2,   
			dauDcaDist,
			pairDca,   pairDecayL,   pairY,   cosThetaStar,   pointingAngle,   
			pairPt,   pairEta,   pairPhi,   pairMass, 
			pairPPt,   pairPEta,   pairPPhi,   pairPMass, 
			pairCtau, pairPos.x(), pairPos.y(), pairPos.z());

	return true;
}

const float *StPicoAnaTreeMaker::getRecenterCor(int runId, int centrality){

	int runnumberPointer = TMath::BinarySearch(sizeof(mRunIds_run14_auau200)/sizeof(int),mRunIds_run14_auau200,runId);
	mRecenterCor[0] =  cosfarwest_correction->GetBinContent(runnumberPointer+1,centrality+1);
	mRecenterCor[1] =  sinfarwest_correction->GetBinContent(runnumberPointer+1,centrality+1);
	mRecenterCor[2] =  coswest_correction->GetBinContent(runnumberPointer+1,centrality+1);
	mRecenterCor[3] =  sinwest_correction->GetBinContent(runnumberPointer+1,centrality+1);
	mRecenterCor[4] =  coseast_correction->GetBinContent(runnumberPointer+1,centrality+1);
	mRecenterCor[5] =  sineast_correction->GetBinContent(runnumberPointer+1,centrality+1);
	mRecenterCor[6] =  cosfareast_correction->GetBinContent(runnumberPointer+1,centrality+1);
	mRecenterCor[7] =  sinfareast_correction->GetBinContent(runnumberPointer+1,centrality+1);

	return mRecenterCor;
}

Int_t StPicoAnaTreeMaker::getCentrality() {
	int mCent_Year14_200GeV[] = {10,21,40,71,116,179,263,373,441};
	for(int i=0;i<9;i++) {
		if(mPicoDst->event()->grefMult()<= mCent_Year14_200GeV[i]) {
			return i;
		}
	}
	return 9;
}
