#include "StAnaTreeArrays.h"

//              ARRAY NAMES
//============================================================================================
const char* StAnaTreeArrays::anaTreeArrayNames [__NANATREEARRAYS__] = {"Event","ETracks","MuTracks","HTracks",
								 "EEPair","EMuPair","MuMuPair", "EmcTrigger", "MtdTrigger"
};

//              ARRAY TYPES
//============================================================================================
const char* StAnaTreeArrays::anaTreeArrayTypes [__NANATREEARRAYS__] = {"StEventHeader","StElectronTrack","StMuonTrack","StHadronTrack",
								 "StEEPair","StEMuPair","StMuMuPair", "StEmcTrigger", "StMtdTrigger"
};

//              ARRAY SIZES
//============================================================================================
// These are intial sizes. Automatically resized if too small.
// Choosing too large initial values gives a performance penalty when reading
int StAnaTreeArrays::anaTreeArraySizes [__NANATREEARRAYS__    ] = {1,100,100,1000,
							     100,100,100,1000,100
};

//              ARRAY COUNTERS
//============================================================================================
int   StAnaTreeArrays::anaTreeArrayCounters [__NANATREEARRAYS__ ] = {0,0,0,0,
							       0,0,0,0,0
};

StAnaTreeArrays::StAnaTreeArrays()
{}
