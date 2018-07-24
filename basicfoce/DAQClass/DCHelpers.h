////////////////////////////////////////////////
//	DCHelpers.h -- DaqCOM helper functions
//	hlp 2001_04_03
/////////////////////////////////////////////////

#ifndef __DCHELPERS_H_
#define __DCHELPERS_H_

#define WM_KICKIDLE         0x036A  // (params unused) causes idles to kick in
#define dcTRY					try{
#define dcCATCH				}catch(_com_error &e){dump_daqcom_error(e);}	


///////////////////////////////////////
//	Error Helpers

void dump_daqcom_error(_com_error &e);
bool DoEvents();
#endif