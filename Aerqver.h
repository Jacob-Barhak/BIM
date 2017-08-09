#ifndef __AERQVER_H__
#define __AERQVER_H__

#define AER_UNIDEX_600PC   600
#define AER_UNIDEX_631     631
#define AER_UNIDEX_31      (AER_UNIDEX_631)
#define AER_UNIDEX_500     500

typedef struct tagAER_VERSION
{
   USHORT   wUnidex;    // AER_UNIDEX_xxxx 
   USHORT   wInternal;  // Internal Version Number 
   USHORT   wMajor;     // Major Version Number 
   USHORT   wMinor;     // Minor Version Number 
} AER_VERSION;
typedef AER_VERSION   *PAER_VERSION;



// this file has the version info in it 
//#include"aerqver.rc"

#endif
// __AERQVER_H__ 
