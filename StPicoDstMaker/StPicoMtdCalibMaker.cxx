/*******************************************************************
 *
 * StPicoMtdCalibMaker.cxx
 * 
 * Author: Rongrong Ma
 *****************************************************************
 *
 * Description: - Calibration Maker to do the calibration for Mtd
 *
 *              - in PicoDst files
 *
 *************************************************************************/

#include <iostream>
#include "StPicoDst.h"
#include "StPicoEvent.h"
#include "StPicoDstMaker.h"
#include "StPicoTrack.h"
#include "StPicoMtdHit.h"
#include "StPicoMtdTrigger.h"
#include "StPicoMtdPidTraits.h"
#include "StPicoBTofPidTraits.h"
#include "StPicoMtdCalibMaker.h"

ClassImp(StPicoMtdCalibMaker)

//_____________________________________________________________________________
StPicoMtdCalibMaker::StPicoMtdCalibMaker(const char *name) : StMaker(name)
{
  /// default constructor

  mPicoDst                = 0x0;
  mDebug                  = kFALSE;

  mApplyT0                = kFALSE;
  mApplyDy                = kFALSE;
  mApplyDz                = kFALSE;
  mInitFromFile           = kTRUE;  // default initialization from database
  mCalibFileT0            = "";
  mCalibFileDy            = "";
  mCalibFileDz            = "";
}

//_____________________________________________________________________________
StPicoMtdCalibMaker::~StPicoMtdCalibMaker()
{ 
  /// default destructor
}

//____________________________________________________________________________
Int_t StPicoMtdCalibMaker::Init()
{
  printConfig();
  return kStOK;
}

//____________________________________________________________________________
Int_t StPicoMtdCalibMaker::InitRun(Int_t runnumber)
{
  //return kStOK;
  //initialize correction table
  Int_t dblSize = sizeof(Double_t);
  memset(mMtdT0Corr,      0, mNBackleg*mNModule*mNCell*dblSize);
  memset(mMtdDyCorr,      0, mNBackleg*mNModule*dblSize);
  memset(mMtdDzCorr,      0, mNBackleg*mNModule*mNCell*dblSize);

  if(!mInitFromFile) //init from database
    {
      LOG_ERROR << "No data base mode" << endm;
      return kStErr;
    }
  else
    {
      LOG_INFO << "Initializing calibration parameters from local files" << endm;
      ifstream inData;
      int backlegId, moduleId, cellId;
      if(mApplyT0)
	{
	  //load T0 offset parameters from local file
	  if (mCalibFileT0.length()==0)
	    {
	      LOG_ERROR << "Please input the local file path for T0 offset parameters" << endm;
	      return kStErr;
	    }

	  inData.open(mCalibFileT0.c_str());
	  if(!inData.is_open())
	    {
	      LOG_ERROR << "Unable to get the T0 offset parameters from local file" <<endm;
	      LOG_ERROR << "Check if this file exists: " << mCalibFileT0.c_str() << endm;
	      return kStErr;
	    }
	  double t0Corr;

	  for(Int_t i=0;i<mNBackleg;i++) 
	    {
	      for(Int_t j=0;j<mNModule;j++) 
		{
		  inData >> backlegId >> moduleId >> t0Corr;
		  mMtdT0Corr[backlegId-1][moduleId-1]=t0Corr;
		  if (mDebug) { LOG_INFO << "Backleg = " << backlegId 
					 << ", moduel = " << moduleId 
					 << ", mMtdT0Corr = " <<mMtdT0Corr[backlegId-1][moduleId-1] 
					 << endm;}
		}
	    }
	  inData.close();
	}

      if(mApplyDy)
	{
	  //load dy offset parameters from local file
	  if (mCalibFileDy.length()==0)
	    {
	      LOG_ERROR << "Please input the local file path for Dy offset parameters" << endm;
	      return kStErr;
	    }
	  if (mDebug) { LOG_INFO << " Local file for Dy offset : " << mCalibFileDy << endm; }
	  inData.open(mCalibFileDy.c_str());
	  if(!inData.is_open())
	    {
	      LOG_ERROR << "Unable to get the Dy offset parameters from local file" <<endm;
	      LOG_ERROR << "Check if this file exists: " << mCalibFileDy.c_str() << endm;
	      return kStErr;
	    }
	  Double_t yCorr;
	  
	  for(Int_t i=0;i<mNBackleg;i++)
	    {
	      for(Int_t j=0;j<mNModule;j++)
		{
		  inData>>backlegId>>moduleId;
		  inData>>yCorr;
		  mMtdDyCorr[backlegId-1][moduleId-1]=yCorr;
		  if (mDebug) { LOG_INFO << "mMtdDyCorr=" <<mMtdDyCorr[backlegId-1][moduleId-1]<< endm;}
		}
	    }
	  inData.close();
	}

      if(mApplyDz)
	{
	  //load dz offset parameters from local file
	  if (mCalibFileDz.length()==0)
	    {
	      LOG_ERROR << "Please input the local file path for Dz offset parameters" << endm;
	      return kStErr;
	    }
	  if (mDebug) { LOG_INFO << " Local file for Dz offset : " << mCalibFileDz << endm; }
	  inData.open(mCalibFileDz.c_str());
	  if(!inData.is_open())
	    {
	      LOG_ERROR << "Unable to get the Dz offset parameters from local file" <<endm;
	      LOG_ERROR << "Check if this file exists: " << mCalibFileDz.c_str() << endm;
	      return kStErr;
	    }
	  Double_t zCorr;
	  
	  for(Int_t i=0;i<mNBackleg;i++)
	    {
	      for(Int_t j=0;j<mNModule;j++)
		{
		  for(Int_t l=0;l<mNCell;l++)
		    {
		      inData>>backlegId>>moduleId>>cellId;
		      inData>>zCorr;
		      mMtdDzCorr[backlegId-1][moduleId-1][cellId]=zCorr;
		      if (mDebug) { LOG_INFO << "mMtdDzCorr=" <<mMtdDzCorr[backlegId-1][moduleId-1][cellId] << endm;}
		    }
		}
	    }
	  inData.close();
	}
    }
  return kStOK;
}

//_____________________________________________________________________________
Int_t StPicoMtdCalibMaker::Make()
{
  if (mDebug) { LOG_INFO << "StPicoMtdCalibMaker::Maker: starting ..." << endm; }

  StPicoDstMaker *picoDstMaker = (StPicoDstMaker*) GetMaker("picoDst");
  if(picoDstMaker)
    {
      mPicoDst = picoDstMaker->picoDst();
      if(mPicoDst) processPicoDst();
    }
  else
    {
      LOG_WARN << "PicoDst is not available!" << endm;
    }

  return kStOK;
}

//_____________________________________________________________________________
void StPicoMtdCalibMaker::processPicoDst()
{
  Int_t nPidTraits = mPicoDst->numberOfMtdPidTraits();
  for(Int_t i=0; i<nPidTraits; i++)
    {
      StPicoMtdPidTraits *mtdPid = mPicoDst->mtdPidTraits(i);
      if(!mtdPid) continue;

      int backleg = mtdPid->backleg();
      int module  = mtdPid->module();
      int cell    = mtdPid->cell();
      
      //cout<< backleg << "  " << module << "  " << cell << endl;
      //cout << "Old dtof = " << mtdPid->deltaTimeOfFlight() << endl;
      if(mApplyT0) mtdPid->setDeltaTimeOfFlight(mtdPid->deltaTimeOfFlight()+mMtdT0Corr[backleg-1][module-1]);
      if(mApplyDy) mtdPid->setDeltaY(mtdPid->deltaY()+mMtdDyCorr[backleg-1][module-1]);
      if(mApplyDz) mtdPid->setDeltaZ(mtdPid->deltaZ()+mMtdDzCorr[backleg-1][module-1][cell]);
      //cout << "New dtof = " << mtdPid->deltaTimeOfFlight() << endl;
    }
}

//_____________________________________________________________________________
void StPicoMtdCalibMaker::printConfig()
{
  const char *decision[2] = {"no","yes"};

  printf("============== StPicoMtdCalibMaker =============\n");
  printf("Initilze local files: %s\n",decision[mInitFromFile]);
  printf("Apply T0: %s\n",decision[mApplyT0]);
  printf("Apply Dy: %s\n",decision[mApplyDy]);
  printf("Apply Dz: %s\n",decision[mApplyDz]);
  printf("\n");
}


//
// $Id: $
// $Log: $

