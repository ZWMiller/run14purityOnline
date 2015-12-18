#include "StEmcTrigger.h"
#include "StPicoDstMaker/StPicoConstants.h"
#include "StMessMgr.h"

ClassImp(StEmcTrigger)

//----------------------------------------------------------------------------------
StEmcTrigger::StEmcTrigger()
{
  Clear();
}

//----------------------------------------------------------------------------------
StEmcTrigger::StEmcTrigger(int flag, int id, int adc, int eId, int adc0)
{
  Clear();

  if(flag<0) mFlag = 0;
  if(id  <0) mId   = 0;
  if(adc <0) mAdc  = 0;
  if(eId <0) mEId  = Pico::USHORTMAX;
  if(adc0<0) mAdc0 = 0;

  mFlag = (flag>Pico::UCHARMAX)  ? Pico::UCHARMAX  : (UChar_t)flag;
  mId   = (id  >Pico::USHORTMAX) ? Pico::USHORTMAX : (UShort_t)id;
  mAdc  = (adc >Pico::USHORTMAX) ? Pico::USHORTMAX : (UShort_t)adc;
  mEId = (eId >Pico::USHORTMAX) ? Pico::USHORTMAX : (UShort_t)eId;
  mAdc0  = (adc0 >Pico::USHORTMAX) ? Pico::USHORTMAX : (UShort_t)adc0;
}

//----------------------------------------------------------------------------------
StEmcTrigger::~StEmcTrigger()
{ /* noop */ }

//----------------------------------------------------------------------------------
void StEmcTrigger::Clear(const Option_t* opt)
{
  mFlag = 0;
  mId = 0;
  mAdc = 0;
  mEId = 0;
  mAdc0 = 0;

}
//----------------------------------------------------------------------------------
void StEmcTrigger::Print(const Char_t *option) const {
  LOG_INFO << " Flag = " << mFlag << " Id = " << mId << " Adc = " << mAdc << " mEId = "<<mEId<< endm;
}
