/*---------------------------------------------------------------------------
 *
 * Copyright (c) 2022, congatec GmbH. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the BSD 2-clause license which 
 * accompanies this distribution. 
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the BSD 2-clause license for more details.
 *
 * The full text of the license may be found at:        
 * http://opensource.org/licenses/BSD-2-Clause   
 *
 *---------------------------------------------------------------------------
 */
 
#ifndef _CGOSTEST_H_
#define _CGOSTEST_H_


#ifdef __cplusplus
extern "C" {
#endif

#if 0

#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <malloc.h>
//#include <conio.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#endif
#include "cgos.h"
#include "cgospriv.h"
#define UINT32	unsigned int
#define	INT32	int
#define UINT16	unsigned short
#define INT16	short

#ifdef WIN32
#define cgostest _tmain
#else
#define cgostest main
#define _TCHAR char
#define _T(s) s
#define getch getchar
#define Sleep(n) usleep(n*1000)
#endif

#endif

#ifdef _UNICODE


#define PRINTF wprintf
#define FPRINTF fwprintf
#define TOLOWER towlower
#define STRCMP wcscmp
#define FOPEN _wfopen
#define SSCANF swscanf
#define SCANF wscanf
#define STRNCMP _wcsnicmp
#define TOLOWER towlower
#define SPRINTF wsprintf

#else //UNICODE

#define PRINTF printf
#define FPRINTF fprintf
#define TOLOWER tolower
#define STRCMP strcmp
#ifdef WIN32
#define STRNCMP _strnicmp
#else
#define STRNCMP strncasecmp
#endif
#define FOPEN fopen
#define SSCANF sscanf
#define SCANF scanf
#define TOLOWER tolower
#define SPRINTF sprintf

#endif //_UNICODE


//---------------------
// Version definition
//---------------------
#define MAJOR_VERSION_NUMBER    1
#define MINOR_VERSION_NUMBER    2
#define BUILD_NUMBER            1


//+---------------------------------------------------------------------------
//       congatec system information structure
//+---------------------------------------------------------------------------
typedef struct
{
    unsigned char       BaseBiosVersion[9]; // 9 bytes for base BIOS version and zero termination.  
    unsigned char       OEMBiosVersion[9];  // 9 bytes for OEM BIOS version and zero termination.  
    unsigned char       FirmwareVersion[9]; // 9 bytes for CGBC firmware revision and zero termination.  
    unsigned int		CgosAPIVersion;     // CGOS library/API version.
    unsigned int		CgosDrvVersion;     // CGOS driver version.
    CGOSBOARDINFO       CgosBoardInfo;      // CGOS board information structure
} CG_INFO_STRUCT ;

//+---------------------------------------------------------------------------
//       test module structure
//+---------------------------------------------------------------------------
typedef struct
{
    char modSelector[16];
    char modDescription[64];
    void (*fpModEntry) (int argc, _TCHAR* argv[]);
	int  optFlag;
} CGOSTEST_MODULE;

#define CGBC_CMD_GET_FW_REV    0x21


#ifdef __cplusplus
}
#endif

#endif // _CGOSTEST_H_
