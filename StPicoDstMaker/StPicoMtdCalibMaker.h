#ifndef STPICOMTDCALIMAKER_H
#define STPICOMTDCALIMAKER_H

/***************************************************************************
 *
 * $Id: StPicoMtdCalibMaker.h,v 1.1 2015/04/20 02:26:25 marr Exp $ 
 * StPicoMtdCalibMaker - class to in inplement Mtd related Calibration paraments
 * Author: Xinjie Huang
 *--------------------------------------------------------------------------
 *
 ***************************************************************************/
#include "TMath.h"
#include "StMaker.h"

#include <string>
#include <vector>
#ifndef ST_NO_NAMESPACES
using std::string;
using std::vector;
#endif

class TFile;
class StMuDst;
class StPicoDst;

class StPicoMtdCalibMaker : public StMaker{
 public:
  StPicoMtdCalibMaker(const char* name="picoMtdCalib");   /// Default constructor
  virtual ~StPicoMtdCalibMaker();                     /// Destructor
    
  virtual Int_t Init();
  virtual Int_t InitRun(Int_t);
  virtual Int_t Make();
  
  void printConfig();

  void setDebug(const bool debug)         { mDebug = debug;             }
  void setApplyT0(const bool apply)       { mApplyT0 = apply;           } 
  void setApplyDy(const bool apply)       { mApplyDy = apply;           }   
  void setApplyDz(const bool apply)       { mApplyDz = apply;           }        
  void setInitFromFile(const bool val)    { mInitFromFile = val;        }       
  void setCalibFileT0(const char *file)   { mCalibFileT0 = file;        }
  void setCalibFileDy(const char *file)   { mCalibFileDy = file;        }
  void setCalibFileDz(const char *file)   { mCalibFileDz = file;        }

 protected:
  void processPicoDst();
      
 private:
  enum{
    mNBackleg = 30,    // 30 Backlegs
    mNModule = 5,      // 5 Modules for each Backlegs
    mNCell = 12        // 12 cells per module
  };

  double          mMtdT0Corr[mNBackleg][mNModule];    // T0 offset 
  double          mMtdDyCorr[mNBackleg][mNModule];    // dy correction for each module
  double          mMtdDzCorr[mNBackleg][mNModule][mNCell];    // dz correction for each cell 
  double          mTStart;                                    // Collision start time

  StPicoDst*      mPicoDst;
  bool            mDebug;            // switch to debug mod

  bool            mApplyT0;
  bool            mApplyDy;
  bool            mApplyDz;
  bool            mInitFromFile;     // switch for reading from files    
  string          mCalibFileT0;      // filename for T0 calibration parameters
  string          mCalibFileDy;      // filename for dy position correction parameters
  string          mCalibFileDz;      // filename for dz position correction parameters

  virtual const Char_t *GetCVS() const 
  {
    static const char cvs[]="Tag $Name:  $Id: built " __DATE__ " " __TIME__ ; return cvs;
  }
    
  ClassDef(StPicoMtdCalibMaker,0);
};

#endif
