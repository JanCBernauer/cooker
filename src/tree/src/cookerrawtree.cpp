/** 
 * This file has the implementations of the CRT 
 */

#include "../../../include/cookerrawtree.h"

CRTBase::CRTBase() {};
CRTBase::~CRTBase() {};
ClassImp(CRTBase);

CRTConfigFile::CRTConfigFile() {};
CRTConfigFile::~CRTConfigFile() {};
ClassImp(CRTConfigFile);

CRTRunInfo::CRTRunInfo() {};
CRTRunInfo::~CRTRunInfo() {};
ClassImp(CRTRunInfo);

CRTEventInfo::CRTEventInfo() {};
CRTEventInfo::~CRTEventInfo() {};
ClassImp(CRTEventInfo);


CRTBinaryBlob::CRTBinaryBlob() {
data=NULL;
};
CRTBinaryBlob::~CRTBinaryBlob() {};
ClassImp(CRTBinaryBlob);

