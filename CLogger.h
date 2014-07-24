#ifndef __CLOGGER__
#define __CLOGGER__
#include "common.h"
class CLogger
{
	public:
		CLogger();
		~CLogger();
		UINT32 Write(char *s);
		UINT32 Init(char *filename);
	private:
		char m_fileName[MAX_FILENAME_LEN];
};

#endif

