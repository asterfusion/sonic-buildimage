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

/*---------------
 * Include files
 *---------------
 */
#include "OsAL.h"
#include <stdio.h>

#undef UNICODE

#include "generic.h"
#include "Cgos.h"
#include "CgosPriv.h"
#include "cgostest.h"

#undef TEXT

#ifndef TEXT
#define TEXT(s) s
#endif

/*--------------
 * Externs used
 *--------------
 */


/*--------------------
 * Local definitions
 *--------------------
 */
void HandleTestAll(int argc, _TCHAR* argv[]);
void HandleGeneric(int argc, _TCHAR* argv[]);
void HandleVga(int argc, _TCHAR* argv[]);
void HandleStorage(int argc, _TCHAR* argv[]);
void HandleI2C(int argc, _TCHAR* argv[]);
void HandleWD(int argc, _TCHAR* argv[]);
void HandleMonitor(int argc, _TCHAR* argv[]);
void HandleSupported(int argc, _TCHAR* argv[]);
void HandleGPIO(int argc, _TCHAR* argv[]);

//MOD23 v moved here from generic.h
/* temporary buffer */
char logBuffer[512];
//MOD23 ^

#define EEP_SIZE 128

/*-------------------------
 * Module global variables
 *-------------------------
 */

/* flag definitions */
#define flgWdogReboot 0x0001
unsigned int flagByte = 0;


HCGOS hCgos=0;

static char szLogFile[] = TEXT("cgostest.csv");

static char signonMes[] = "\n"
                            "congatec CGOS interface test utility ---  Version %d.%d.%d\n"
                            "(C) Copyright 2005-2019 congatec AG\n";

static CGOSTEST_MODULE moduleList[] =   {
								{"/w", "force watchdog to reboot system after watchdog test\n", NULL, flgWdogReboot }, 
                                {"ALL", "Test all functions.", HandleTestAll, 0},
                                {"GENERIC", "Test generic board functions.", HandleGeneric, 0},
                                {"VGA", "Test video functions.", HandleVga, 0},
                                {"STORAGE", "Test storage area functions.", HandleStorage, 0},
                                {"I2C", "Test I2C bus functions.", HandleI2C, 0},
                                {"WD", "Test watchdog functions.", HandleWD, 0},
                                {"MONITOR", "Test monitor functions.", HandleMonitor, 0},
                                {"GPIO", "Test GPIO functions.", HandleGPIO, 0},
                                /*{"SUPPORT", "Generate list of supported CGOS functions.", HandleSupported}*/
                                        };


char buf[8192];		

unsigned int ulLastError = 0;

/*---------------------------------------------------------------------------
 * Name: Log
 * Desc: Write strings referenced by input pointers to a logfile.
 * Inp:  pszField1, pszField2, pszField3 :
 *       Pointer to zero terminated strings to be written to logfile.
 * Outp: none
 *---------------------------------------------------------------------------
 */
void Log(char *pszField1, char *pszField2, char *pszField3)
{
  sprintf(logBuffer,"%s,%s,%s,%08Xh\n",pszField1,pszField2,pszField3,ulLastError);
  ulLastError = 0;
  LogBuf(logBuffer);
}  
  
	
	


/*---------------------------------------------------------------------------
 * Name: ShowSignon
 * Desc: Clear screen and display CGUTLCMD signon message.
 * Inp:  none
 * Outp: none
 *---------------------------------------------------------------------------
 */
void ShowSignon(void)
{
	  sprintf(logBuffer,signonMes,MAJOR_VERSION_NUMBER, MINOR_VERSION_NUMBER, BUILD_NUMBER );
	  report(logBuffer);        
}

/*---------------------------------------------------------------------------
 * Name: ShowUsage
 * Desc: Display parameters for this module.
 * Inp:  none
 * Outp: none
 *---------------------------------------------------------------------------
 */
static void ShowUsage(void)
{
    unsigned short i;

	sprintf(logBuffer,TEXT("\nUsage:\n\ncgostest [Options] [TEST]\n\nSupported Tests & Options:\n\n"));
    for(i=0; i < (sizeof moduleList / sizeof moduleList[0]); i++)
    {
        strcat(logBuffer, moduleList[i].modSelector);
        strcat(logBuffer, "\t");
        strcat(logBuffer, moduleList[i].modDescription);
        strcat(logBuffer, "\n");
    }
    report(logBuffer);
}

/*---------------------------------------------------------------------------
 * Name: GenerateInfo
 * Desc: Collect all test relevant information and append it to the log file.
 * Inp:  none
 * Outp: none
 *---------------------------------------------------------------------------
 */
void GenerateInfo(void)
{
    CG_INFO_STRUCT CgInfoStruct = {{0xFF}};
    unsigned char CgbcCmdWriteBuf[32] = {0};
    unsigned char CgbcCmdReadBuf[32] = {0};
    unsigned int CgbcCmdStatus = 0;

    if(hCgos == 0)
    {
        if (!CgosLibInitialize()) 
        {
            if (!CgosLibInstall(1)) 
            {  
                report(TEXT("\nERROR: Failed to generate test information!\n"));
                return;
            }
            if (!CgosLibInitialize()) 
            {
                report(TEXT("\nERROR: Failed to generate test information!\n"));
                return;
            }

        }
            
        if (!CgosBoardOpen(0,0,0,&hCgos)) 
        {
            report(TEXT("\nERROR: Failed to generate test information!\n"));
            return;
        }
    }

    /* Get CGOS version information */
    CgInfoStruct.CgosAPIVersion = CgosLibGetVersion();
    CgInfoStruct.CgosDrvVersion = CgosLibGetDrvVersion();

    /* Get board controller firmware version */
    CgbcCmdWriteBuf[0] = CGBC_CMD_GET_FW_REV;
   
    if(CgosCgbcHandleCommand(hCgos, &CgbcCmdWriteBuf[0], 1, &CgbcCmdReadBuf[0], 3, &CgbcCmdStatus))
    {
        sprintf((char *)(&(CgInfoStruct.FirmwareVersion)),"CGBCP%c%c%c",CgbcCmdReadBuf[0], CgbcCmdReadBuf[1],CgbcCmdReadBuf[2]);
    }


    /* Get standard CGOS information structure */
    CgInfoStruct.CgosBoardInfo.dwSize = sizeof(CgInfoStruct.CgosBoardInfo);
    CgosBoardGetInfo(hCgos, &(CgInfoStruct.CgosBoardInfo));

    sprintf(logBuffer,
    TEXT(",,,\n")
    TEXT("CGOSTEST TEST SYSTEM INFORMATION,,,\n")
    TEXT("CGOSTEST Version,rev. %d.%d.%d,,\n")
    TEXT("CGOS API Version,0x%08x,,\n")
    TEXT("CGOS Driver Version,0x%08x,,\n")
    TEXT("System BIOS Version,%sR%03x,,\n")
    TEXT("BC Firmware Version,%s,,\n")
    TEXT("Board Name,%s,,\n")
    TEXT("Board Sub Name,%s,,\n")
    TEXT("Product Revision,%c.%c (0x%04X),,\n")
    TEXT("Part Number,%s,,\n")
    TEXT("EAN Code,%s,,\n")
    TEXT("Serial Number,%s,,\n")
    TEXT(",,,\n"),
    MAJOR_VERSION_NUMBER,
    MINOR_VERSION_NUMBER,
    BUILD_NUMBER,
    CgInfoStruct.CgosAPIVersion,
    CgInfoStruct.CgosDrvVersion,
    CgInfoStruct.CgosBoardInfo.szBoard, CgInfoStruct.CgosBoardInfo.wSystemBiosRevision,
    &(CgInfoStruct.FirmwareVersion[0]),
    CgInfoStruct.CgosBoardInfo.szBoard,
    CgInfoStruct.CgosBoardInfo.szBoardSub,
    CgInfoStruct.CgosBoardInfo.wProductRevision>>8, CgInfoStruct.CgosBoardInfo.wProductRevision & 0xFF, CgInfoStruct.CgosBoardInfo.wProductRevision>>8,   
    CgInfoStruct.CgosBoardInfo.szPartNumber,
    CgInfoStruct.CgosBoardInfo.szEAN,
    CgInfoStruct.CgosBoardInfo.szSerialNumber
    );
    LogBuf(logBuffer);

    CgosBoardClose(hCgos);
    hCgos = 0;    
    CgosLibUninitialize();

    return;
}
/*---------------------------------------------------------------------------
 * Name:        CgosOpen
 * Desc:        Try to open and initalize the CGOS interface.
 * Inp:         bLog - TRUE: Write to log file, FALSE do not write to log file.
 * Outp:        Status:
 *              FALSE   - Error
 *              TRUE    - Success
 *---------------------------------------------------------------------------
 */
unsigned short CgosOpen(unsigned char bLog)
{        
    if (!CgosLibInitialize()) 
    {
        if (!CgosLibInstall(1)) 
        {  
            report(TEXT("\nERROR: Failed to install CGOS interface.\n"));  
            if(bLog)
            {
                Log("CgosLibInstall", "FAILED", "");
            }
            return FALSE;
        }
        if(bLog)
        {
            Log("CgosLibInstall", "SUCCESS", "");
        }
        if (!CgosLibInitialize()) 
        {
            report(TEXT("\nERROR: Failed to initialize CGOS interface.\n"));            
            if(bLog)
            {
                Log("CgosLibInitialize", "FAILED", "");
            }
            return FALSE;
        }

    }
    if(bLog)
    {
        Log("CgosLibInitialize", "SUCCESS", "");
    }
        
    if (!CgosBoardOpen(0,0,0,&hCgos)) 
    {
        report(TEXT("\nERROR: Failed to open board interface.\n"));  
        if(bLog)
        {
            Log("CgosBoardOpen", "FAILED", "");      
        }
        return FALSE;
    }
    if(bLog)
    {
        Log("CgosBoardOpen", "SUCCESS", "");  
    }

    if(CgosLibSetLastErrorAddress(&ulLastError))
    {
        if(CgosLibGetLastError() == ulLastError)
        {
            if(bLog)
            {
                Log("CgosLibGetLastError", "SUCCESS", "");  
            }
        }
        else
        {
            if(bLog)
            {
                Log("CgosLibGetLastError", "FAILED", "");  
            }
        }
        if(bLog)
        {
            Log("CgosLibSetLastErrorAddress", "SUCCESS", "");  
        }
    }
    else
    {
        if(bLog)
        {
            Log("CgosLibSetLastErrorAddress", "FAILED", "");  
        }
    }
    return TRUE;          
}

/*---------------------------------------------------------------------------
 * Name:        CgosClose
 * Desc:        Try to close and un-initalize the CGOS interface.
 * Inp:         bLog - TRUE: Write to log file, FALSE do not write to log file.
 * Outp:        Status:
 *              FALSE   - Error
 *              TRUE    - Success
 *---------------------------------------------------------------------------
 */
unsigned short CgosClose(unsigned char bLog)
{        
    if (hCgos) 
    {
        if(!CgosBoardClose(hCgos))
        {
            report(TEXT("\nERROR: Failed to close CGOS interface.\n"));  
            if(bLog)
            {
                Log("CgosBoardClose", "FAILED", "");      
            }
        }
        else
        {
            if(bLog)
            {
                Log("CgosBoardClose", "SUCCESS", "");      
            }
        }
        hCgos = 0;
    }

    if(!CgosLibUninitialize())
    {
        report(TEXT("\nERROR: Failed to un-initialize CGOS interface.\n"));            
        if(bLog)
        {
            Log("CgosLibUninitialize", "FAILED", "");      
        }
    }
    else
    {
        if(bLog)
        {
            Log("CgosLibUninitialize", "SUCCESS", "");      
        }
    }
    return TRUE;          
}

/*---------------------------------------------------------------------------
 * Name: HandleGeneric
 * Desc: Test generic CGOS functions.
 * Inp:  argc   - Number of command line arguments passed
 *       argv[] - Array of pointers to command line parameters
 * Outp: none       
 *---------------------------------------------------------------------------
 */
void HandleGeneric(int argc, _TCHAR* argv[])
{
    char szBoardName[16] = {0x00}; 
    char ch; 
    CGOSBOARDINFO   CgosBoardInfo ={0x00};
    unsigned int ulBootCount, ulRunTime;

    report(TEXT("\n****************************************\n"));
    report(TEXT("Testing generic CGOS functions.\n"));
    report(TEXT("****************************************\n"));
    report(TEXT("Enter [s] to skip test, any other key to continue.\n"));

    ch = getchar();
    if(ch == 's')
    {
        return;
    }

    report(TEXT("Close board interface.\n\n"));
    if(!CgosBoardClose(hCgos))
    {
        report(TEXT("\nERROR: CgosBoardClose failed.\n"));
        Log("CgosBoardClose", "FAILED", "");      
        return;
    }
    else
    {
        Log("CgosBoardClose", "SUCCESS", "");
    }

    sprintf(logBuffer,
    	TEXT("Total number of available boards: %u\n")
    	TEXT("Number of primary CPU boards: %u\n")
    	TEXT("Number of primary VGA boards: %u\n")
    	TEXT("Number of boards with CPU functionality: %u\n")
    	TEXT("Number of boards with VGA functionality: %u\n"),
    	CgosBoardCount(0,0),
    	CgosBoardCount(CGOS_BOARD_CLASS_CPU,CGOS_BOARD_OPEN_FLAGS_PRIMARYONLY),
    	CgosBoardCount(CGOS_BOARD_CLASS_VGA,CGOS_BOARD_OPEN_FLAGS_PRIMARYONLY),
      CgosBoardCount(CGOS_BOARD_CLASS_CPU,0),
      CgosBoardCount(CGOS_BOARD_CLASS_VGA,0));
    report(logBuffer);  
    Log("CgosBoardCount", "SUCCESS", "");


    if(!CgosBoardGetName(hCgos, &szBoardName[0], sizeof(szBoardName)))
    {
        report(TEXT("\nERROR: CgosBoardGetName failed.\n"));
        Log("CgosBoardGetName", "FAILED", "");
    }
    else
    {
      sprintf(logBuffer,
    	  TEXT("Board name: %s\n"),
    	  &szBoardName[0]);
      report(logBuffer);  
      Log("CgosBoardGetName", "SUCCESS", "");
    }

    report(TEXT("\nRe-open board interface.\n"));
    if(!CgosBoardOpenByName(&szBoardName[0], &hCgos))
    {
        report(TEXT("\nERROR: CgosBoardOpenByName failed.\n"));
        Log("CgosBoardOpenByName", "FAILED", "");
        return;
    }
    else
    {
        Log("CgosBoardOpenByName", "SUCCESS", "");
    }

    getchar();

    CgosBoardInfo.dwSize = sizeof(CgosBoardInfo);
    if(!CgosBoardGetInfo(hCgos, &CgosBoardInfo))
    {
        report(TEXT("\nERROR: CgosBoardGetInfo failed.\n"));
        Log("CgosBoardGetInfo", "FAILED", "");
    }
    else
    {
        Log("CgosBoardGetInfo", "SUCCESS", "");
		sprintf(buf,
			TEXT("Board Information Structure\n\n")
			TEXT("Size of Structure: %u\n")
			TEXT("Flags: %Xh\n")
			TEXT("Board Name: \"%s\"\n")
			TEXT("Board Sub Name: \"%s\"\n")
			TEXT("Manufacturer Name: \"%s\"\n")
        	TEXT("Manufacturer Code: %u\n")
        	TEXT("Manufacturing Date: %04d.%02d.%02d\n")
        	TEXT("Last Repair Date: %04d.%02d.%02d\n")
        	TEXT("Repair Counter: %u\n")
        	TEXT("Serial Number: \"%s\"\n")
        	TEXT("Part Number: \"%s\"\n")
        	TEXT("EAN: \"%s\"\n")
        	TEXT("Product Revision: %c.%c (%04Xh)\n")
        	TEXT("System BIOS Revision: %03x\n")
        	TEXT("BIOS Interface Revision: %03x\n")
        	TEXT("BIOS Interface Build Revision: %03x\n"),
        CgosBoardInfo.dwSize,
        CgosBoardInfo.dwFlags,
        CgosBoardInfo.szBoard,
        CgosBoardInfo.szBoardSub,
        CgosBoardInfo.szManufacturer,
        CgosBoardInfo.dwManufacturer,
        CgosBoardInfo.stManufacturingDate.wYear,CgosBoardInfo.stManufacturingDate.wMonth,CgosBoardInfo.stManufacturingDate.wDay,
        CgosBoardInfo.stLastRepairDate.wYear,CgosBoardInfo.stLastRepairDate.wMonth,CgosBoardInfo.stLastRepairDate.wDay,
        CgosBoardInfo.dwRepairCounter,
        CgosBoardInfo.szSerialNumber,
        CgosBoardInfo.szPartNumber,
        CgosBoardInfo.szEAN,
        CgosBoardInfo.wProductRevision>>8,
        CgosBoardInfo.wProductRevision&0xff,
        CgosBoardInfo.wProductRevision,
        CgosBoardInfo.wSystemBiosRevision,
        CgosBoardInfo.wBiosInterfaceRevision,
        CgosBoardInfo.wBiosInterfaceBuildRevision
        );
        report(buf);
    }

    report(TEXT("\n"));
    if(!CgosBoardGetBootCounter(hCgos, &ulBootCount))
    {
        report(TEXT("\nERROR: CgosBoardGetBootCounter failed.\n"));
        Log("CgosBoardGetBootCounter", "FAILED", "");
    }
    else
    {
        Log("CgosBoardGetBootCounter", "SUCCESS", "");
		sprintf(buf, TEXT("Boot Counter: %u\n"), ulBootCount);
		report(buf);
    }

    if(!CgosBoardGetRunningTimeMeter(hCgos, &ulRunTime))
    {
        report(TEXT("\nERROR: CgosBoardGetRunningTimeMeter failed.\n"));
        Log("CgosBoardGetRunningTimeMeter", "FAILED", "");
    }
    else
    {
        Log("CgosBoardGetRunningTimeMeter", "SUCCESS", "");
		sprintf(buf, TEXT("Running Time Meter: %u\n"),ulRunTime);
		report(buf);
    }


    return;
}

/*---------------------------------------------------------------------------
 * Name: HandleVga
 * Desc: Test CGOS video functions.
 * Inp:  argc   - Number of command line arguments passed
 *       argv[] - Array of pointers to command line parameters
 * Outp: none       
 *---------------------------------------------------------------------------
 */
void HandleVga(int argc, _TCHAR* argv[])
{
    unsigned int ulVgaCount = 0, ulUnit, ulBlVal, ulBlState, ulContVal, ulContState;
    CGOSVGAINFO vgaInfo = {0};
    unsigned char ch;

    report(TEXT("\n****************************************\n"));
    report(TEXT("Testing CGOS video functions.\n"));
    report(TEXT("****************************************\n"));
    report(TEXT("Enter [s] to skip test, any other key to continue.\n"));
    ch = getchar();
    if(ch == 's')
    {
        return;
    }

    /* Get number of available VGA devices */    
    if((ulVgaCount =CgosVgaCount(hCgos)) != 0)
    {
		sprintf(buf, TEXT("\nNumber of available VGA devices: %u.\n"),ulVgaCount);
		report(buf);
        Log("CgosVgaCount", "SUCCESS", "");
    }
    else
    {
        report(TEXT("\nERROR: Failed to get number of VGA devices.\n"));
        Log("CgosVgaCount", "FAILED", "");
        return;
    }

    /* Display info structure for all VGA devices */
    vgaInfo.dwSize = sizeof(vgaInfo);
    for (ulUnit=0; ulUnit < ulVgaCount; ulUnit++)
    {
		if(CgosVgaGetInfo(hCgos, ulUnit, &vgaInfo))
		{
			sprintf(buf, 
             TEXT("\nVGA unit %u information:\n")
             TEXT("dwType:             %Xh\n")
             TEXT("dwFlags:            %Xh\n")
             TEXT("dwNativeWidth:      %Xh\n")                                 /*MOD001*/
             TEXT("dwNativeHeight:     %Xh\n")                                 /*MOD001*/
             TEXT("dwRequestedWidth:   %Xh\n")
             TEXT("dwRequestedHeight:  %Xh\n")
             TEXT("dwRequestedBpp:     %Xh\n")
             TEXT("dwMaxBacklight:     %Xh\n"), 
			ulUnit,
			vgaInfo.dwType,
			vgaInfo.dwFlags,
            vgaInfo.dwNativeWidth,
			vgaInfo.dwNativeHeight,			
			vgaInfo.dwRequestedWidth,
			vgaInfo.dwRequestedHeight,
			vgaInfo.dwRequestedBpp,
			vgaInfo.dwMaxBacklight);
			report(buf);

            sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, vgaInfo.dwType);
            Log("CgosVgaGetInfo", "SUCCESS", buf);
            report(TEXT("\nPress [ENTER] to continue...\n"));
            getchar();

		}
		else
        {
			sprintf(buf, TEXT("\nERROR: Failed to get VGA information for unit %u.\n"),ulUnit);
			report(buf);

            sprintf(buf, "Unit number: %u", ulUnit);
			Log("CgosVgaGetInfo", "FAILED", buf);
        }      
    }

    /* By defintion the unit number 0 always specifies the LFP if available.
       So use this unit number to test backlight and contrast functions */
    report(TEXT("\n"));

    /***************************/
    /* Test backlight functions */
    /***************************/
    if(CgosVgaGetBacklight(hCgos, 0, &ulBlVal))
    {
		sprintf(buf, TEXT("Current backlight value : %u percent\n"),ulBlVal);
		report(buf);
        Log("CgosVgaGetBacklight", "SUCCESS", "");
    }
    else
    {
        report(TEXT("ERROR: Failed to get current backlight value.\n"));
        Log("CgosVgaGetBacklight", "FAILED", "");
    }

    report(TEXT("Set backlight value to 50 percent.\n"));
    if(CgosVgaSetBacklight(hCgos, 0, 50))
    {
        report(TEXT("Press [ENTER] to continue...\n"));
        getchar();
        report(TEXT("Set backlight value to 0 percent.\n"));
        if(CgosVgaSetBacklight(hCgos, 0, 0))
        {
            report(TEXT("Press [ENTER] to continue...\n"));
            getchar();
            report(TEXT("Set backlight value to 100 percent.\n"));
            if(CgosVgaSetBacklight(hCgos, 0, 100))
            {
                Log("CgosVgaSetBacklight", "SUCCESS", "");
            }
            else
            {
                report(TEXT("ERROR: Failed to set backlight value.\n"));
                Log("CgosVgaSetBacklight", "FAILED", "");
            }
        }
        else
        {
            report(TEXT("ERROR: Failed to set backlight value.\n"));
            Log("CgosVgaSetBacklight", "FAILED", "");
        }
    }
    else
    {
        report(TEXT("ERROR: Failed to set backlight value.\n"));
        Log("CgosVgaSetBacklight", "FAILED", "");
    }


    /* Test backlight enable functions */
    report(TEXT("\nToggling backlight enable signal.\n"));
    report(TEXT("Press [ENTER] to continue...\n"));
    getchar();

    report(TEXT("Set backlight enable state to OFF.\n"));
    if(CgosVgaSetBacklightEnable(hCgos, 0, 0))
    {
        report(TEXT("Press [ENTER] to continue...\n"));
        getchar();
        report(TEXT("Set backlight enable state to ON.\n"));
        if(CgosVgaSetBacklightEnable(hCgos, 0, 1))
        {
            Log("CgosVgaSetBacklightEnable", "SUCCESS", "");
        }
        else
        {
            report(TEXT("ERROR: Failed to set backlight enable state.\n"));
            Log("CgosVgaSetBacklightEnable", "FAILED", "");
        }
    }
    else
    {
        report(TEXT("ERROR: Failed to set backlight enable state.\n"));
        Log("CgosVgaSetBacklightEnable", "FAILED", "");
    }

    if(CgosVgaGetBacklightEnable(hCgos, 0, &ulBlState))
    {
		sprintf(buf, TEXT("Current backlight enable state: %u.\n"),ulBlState);
		report(buf);
        Log("CgosVgaGetBacklightEnable", "SUCCESS", "");
    }
    else
    {
        report(TEXT("ERROR: Failed to get backlight enable state.\n"));
        Log("CgosVgaGetBacklightEnable", "FAILED", "");
    }

    /***************************/
    /* Test contrast functions */
    /**************************/
    report(TEXT("\n"));

    if(CgosVgaGetContrast(hCgos, 0, &ulContVal))
    {
 		sprintf(buf, TEXT("Current contrast value : %u percent\n"),ulContVal);
		report(buf);
        Log("CgosVgaGetContrast", "SUCCESS", "");
    }
    else
    {
        report(TEXT("ERROR: Failed to get current contrast value.\n"));
        Log("CgosVgaGetContrast", "FAILED", "");
    }


    report(TEXT("Set contrast value to 50 percent.\n"));
    if(CgosVgaSetContrast(hCgos, 0, 50))
    {
        report(TEXT("Press [ENTER] to continue...\n"));
        getchar();
        report(TEXT("Set contrast value to 0 percent.\n"));
        if(CgosVgaSetContrast(hCgos, 0, 0))
        {
            report(TEXT("Press [ENTER] to continue...\n"));
            getchar();
            report(TEXT("Set contrast value to 100 percent.\n"));
            if(CgosVgaSetContrast(hCgos, 0, 100))
            {
                Log("CgosVgaSetContrast", "SUCCESS", "");
            }
            else
            {
                report(TEXT("ERROR: Failed to set contrast value.\n"));
                Log("CgosVgaSetContrast", "FAILED", "");
            }
        }
        else
        {
            report(TEXT("ERROR: Failed to set contrast value.\n"));
            Log("CgosVgaSetContrast", "FAILED", "");
        }
    }
    else
    {
        report(TEXT("ERROR: Failed to set contrast value.\n"));
        Log("CgosVgaSetContrast", "FAILED", "");
    }


    /* Test contrast enable functions */
    report(TEXT("\nToggling contrast enable signal.\n"));
    report(TEXT("Press [ENTER] to continue...\n"));
    getchar();

    report(TEXT("Set contrast enable state to OFF.\n"));
    if(CgosVgaSetContrastEnable(hCgos, 0, 0))
    {
        report(TEXT("Press [ENTER] to continue...\n"));
        getchar();
        report(TEXT("Set contrast enable state to ON.\n"));
        if(CgosVgaSetContrastEnable(hCgos, 0, 1))
        {
            Log("CgosVgaSetContrastEnable", "SUCCESS", "");
        }
        else
        {
            report(TEXT("ERROR: Failed to set contrast enable state.\n"));
            Log("CgosVgaSetContrastEnable", "FAILED", "");
        }
    }
    else
    {
        report(TEXT("ERROR: Failed to set contrast enable state.\n"));
        Log("CgosVgaSetContrastEnable", "FAILED", "");
    }

    if(CgosVgaGetContrastEnable(hCgos, 0, &ulContState))
    {
 		sprintf(buf, TEXT("Current contrast enable state: %u.\n"),ulContState);
		report(buf);
        Log("CgosVgaGetContrastEnable", "SUCCESS", "");
    }
    else
    {
        report(TEXT("ERROR: Failed to get contrast enable state.\n"));
        Log("CgosVgaGetContrastEnable", "FAILED", "");
    }
    return;
}

/*---------------------------------------------------------------------------
 * Name: HandleStorage
 * Desc: Test CGOS storage area functions.
 * Inp:  argc   - Number of command line arguments passed
 *       argv[] - Array of pointers to command line parameters
 * Outp: none       
 *---------------------------------------------------------------------------
 */
void HandleStorage(int argc, _TCHAR* argv[])
{
    char ch;
    unsigned int ulUnit, ulAreaCount, ulAreaType, ulAreaSize, ulAreaBlocksize, ulAreaIndex, ulEraseState;
    unsigned char *pTestBuf = NULL;
	char *bufPtr;
    unsigned char bufLockPW[] = "werner";

    report(TEXT("\n****************************************\n"));
    report(TEXT("Testing CGOS storage area functions.\n"));
    report(TEXT("****************************************\n"));
    report(TEXT("Enter [s] to skip test, any other key to continue.\n"));
    ch = getchar();
    if(ch == 's')
    {
        return;
    }
    
    /* Get number of public storage areas */    
    if((ulAreaCount = CgosStorageAreaCount(hCgos, 0)) != 0)
    {
 		sprintf(buf, TEXT("\nNumber of public storage areas: %u.\n"),ulAreaCount);
		report(buf);
        Log("CgosStorageAreaCount", "SUCCESS", "");
    }
    else
    {
        report(TEXT("\nERROR: Failed to get number of public storage areas.\n"));
        Log("CgosStorageAreaCount", "FAILED", "");
        return;
    }

    report(TEXT("\n"));
    for(ulUnit = 0; ulUnit < ulAreaCount; ulUnit++)
    {
 		sprintf(buf, TEXT("Storage area %u:\n"), ulUnit);
		report(buf);
        
        ulAreaType = CgosStorageAreaType(hCgos, ulUnit);
 		sprintf(buf, TEXT("Type: %08Xh\n"), ulAreaType);
		report(buf);

        sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
        Log("CgosStorageAreaType","SUCCESS",buf);

        ulAreaSize = CgosStorageAreaSize(hCgos, ulUnit);
 		sprintf(buf, TEXT("Size: %u\n"), ulAreaSize);
		report(buf);
        
		sprintf(buf, "Unit number: %u unit type: %08Xh size: %u", ulUnit, ulAreaType, ulAreaSize);
        Log("CgosStorageAreaSize","SUCCESS",buf);

        ulAreaBlocksize = CgosStorageAreaBlockSize(hCgos, ulUnit);
 		sprintf(buf, TEXT("Blocksize: %u\n"), ulAreaBlocksize);
		report(buf);

        sprintf(buf, "Unit number: %u unit type: %08Xh block size: %u", ulUnit, ulAreaType, ulAreaBlocksize);
        Log("CgosStorageAreaBlockSize","SUCCESS",buf);
        report(TEXT("\n"));

        /* Allocate test buffer */
        if((pTestBuf = (unsigned char*) malloc(ulAreaSize)) == NULL)
        {
            report(TEXT("ERROR: Failed to allocate test buffer. Skip further tests.\n"));
            return;
        }

        /* Test read function */
        report(TEXT("Press [ENTER] to read storage area, [s] to skip this test.\n"));
        if(getchar() != 's')
        {
            if(CgosStorageAreaRead(hCgos, ulAreaType, 0, pTestBuf, ulAreaSize))
            {
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaRead","SUCCESS",buf);
				bufPtr = buf;
                for(ulAreaIndex = 0; ulAreaIndex < ulAreaSize; ulAreaIndex++)
				{
			 		sprintf(bufPtr, TEXT("%02Xh "),*(pTestBuf + ulAreaIndex));
					bufPtr += 4;

					if ((ulAreaIndex * 4) >= sizeof(buf))
						break;
				}
				report(buf);
                report(TEXT("\nStorage area successfully read.\n"));
            }
            else
            {
                report(TEXT("ERROR: Failed to read storage area.\n"));
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaRead","FAILED",buf);
            }            
            report(TEXT("\n"));
        }

        /* Test erase function */
        report(TEXT("Press [ENTER] to erase storage area, [s] to skip this test.\n"));
        if(getchar() != 's')
        {
            if(CgosStorageAreaErase(hCgos, ulAreaType, 0, ulAreaSize))
            {
                report(TEXT("Storage area successfully erased.\n"));
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaErase","SUCCESS",buf);
            }
            else
            {
                report(TEXT("ERROR: Failed to erase storage area.\n"));
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaErase","FAILED",buf);
            }
            report(TEXT("\n"));

            /* Test erase status function */
            if(CgosStorageAreaEraseStatus(hCgos, ulAreaType, 0, ulAreaSize, &ulEraseState))
            {
			 	sprintf(buf, TEXT("Erase state: %Xh\n"), ulEraseState);
				report(buf);
                sprintf(buf, "Unit number: %u unit type: %08Xh state: %Xh", ulUnit, ulAreaType, ulEraseState);
                Log("CgosStorageAreaEraseStatus","SUCCESS",buf);
            }
            else
            {
                report(TEXT("ERROR: Failed to get erase state.\n"));
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaEraseStatus","FAILED",buf);
            }
            report(TEXT("\n"));
        }


        /* Test write function */
        report(TEXT("Press [ENTER] to write to storage area, [s] to skip this test.\n"));
        if(getchar() != 's')
        {
            if(CgosStorageAreaWrite(hCgos, ulAreaType, 0, pTestBuf, ulAreaSize))
            {
                report(TEXT("Storage area successfully written.\n"));
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaWrite","SUCCESS",buf);
            }
            else
            {
                report(TEXT("ERROR: Failed to write to storage area.\n"));
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaWrite","FAILED",buf);
            }           
            report(TEXT("\n"));
        }

        /* Test lock functions */
        report(TEXT("Press [ENTER] to test lock functions, [s] to skip this test.\n"));
        if(getchar() != 's')
        {        
            report(TEXT("\n"));
            if(CgosStorageAreaIsLocked(hCgos, ulAreaType, 0))
            {
                report(TEXT("Storage area is locked.\n"));
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaIsLocked","SUCCESS",buf);
            }
            else
            {
                report(TEXT("Storage area is not locked.\n"));
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaIsLocked","FAILED",buf);
            }
            
            if(CgosStorageAreaLock(hCgos, ulAreaType, 0, &bufLockPW[0], 6))
            {
                report(TEXT("Storage area successfully locked.\n"));
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaLock","SUCCESS",buf);
            }
            else
            {
                report(TEXT("Failed to lock storage area.\n"));
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaLock","FAILED",buf);
            }

            if(CgosStorageAreaUnlock(hCgos, ulAreaType, 0, &bufLockPW[0], 6))
            {
                report(TEXT("Storage area successfully unlocked.\n"));
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaUnlock","SUCCESS",buf);
            }
            else
            {
                report(TEXT("Failed to unlock storage area.\n"));
                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, ulAreaType);
                Log("CgosStorageAreaUnlock","FAILED",buf);
            }
            report(TEXT("\n"));
        }
        

        if(pTestBuf)
        {
            free(pTestBuf);
        }

        report(TEXT("\n"));
        report(TEXT("Press [ENTER] to continue with next area, [e] to end test.\n"));
        if(getchar() == 'e')
        {
            break;
        }
        report(TEXT("\n"));
    }

    return;
}

/*---------------------------------------------------------------------------
 * Name: HandleI2C
 * Desc: Test CGOS I2C bus functions.
 * Inp:  argc   - Number of command line arguments passed
 *       argv[] - Array of pointers to command line parameters
 * Outp: none       
 *---------------------------------------------------------------------------
 */
void HandleI2C(int argc, _TCHAR* argv[])
{     
    unsigned char bEEPAddr, bLogWriteSuccess, bLogReadSuccess, bLogReadState, bLogDataError;
    unsigned char bufEEPOut[EEP_SIZE], bufEEPIn[EEP_SIZE];
    unsigned short usEEPIndex;
    char ch;
    unsigned int ulUnit, ulI2CBusCount, ulI2CBusType;
    unsigned int ulPrimaryI2C = 0xFFFFFFFF;
    unsigned int ulDDCI2C = 0xFFFFFFFF;
    unsigned int ulCurrentFreq, ulMaxFreq;
	char *bufPtr;


    report(TEXT("\n****************************************\n"));
    report(TEXT("Testing CGOS I2C functions.\n"));
    report(TEXT("****************************************\n"));
    report(TEXT("Enter [s] to skip test, any other key to continue.\n"));
    ch = getchar();
    if(ch == 's')
    {
        return;
    }
    
    /* Get number of available I2C busses */    
    if((ulI2CBusCount =CgosI2CCount(hCgos)) != 0)
    {
		sprintf(buf, TEXT("\nNumber of available I2C busses: %u.\n"),ulI2CBusCount); report(buf);
        Log("CgosI2CCount", "SUCCESS", "");
    }
    else
    {
        report(TEXT("\nERROR: Failed to get number of I2C busses.\n"));
        Log("CgosI2CCount", "FAILED", "");
        return;
    }

    /* Check type and availability */
    report(TEXT("\n"));     
    for(ulUnit = 0; ulUnit < ulI2CBusCount; ulUnit++)
    {
        ulI2CBusType = CgosI2CType(hCgos, ulUnit);
        sprintf(buf, "Unit number: %u I2C bus type: %08Xh", ulUnit, ulI2CBusType);
        Log("CgosI2CType","SUCCESS",buf);

        if(ulI2CBusType == CGOS_I2C_TYPE_PRIMARY)
        {
            ulPrimaryI2C = ulUnit;
        }
        else if(ulI2CBusType == CGOS_I2C_TYPE_DDC)
        {
            ulDDCI2C = ulUnit;
        }

        if(CgosI2CIsAvailable(hCgos, ulI2CBusType))
        {
		    sprintf(buf, TEXT("Bus number %u is of type %08Xh and available.\n"), ulUnit,ulI2CBusType); report(buf);
            sprintf(buf, "Unit number: %u I2C bus type: %08Xh", ulUnit, ulI2CBusType);
            Log("CgosI2CIsAvailable","SUCCESS",buf);            
        }
        else
        {
		    sprintf(buf, TEXT("ERROR: Failed to get availability state for bus number %u, type %08dh\n"), ulUnit , ulI2CBusType); report(buf);
            sprintf(buf, "Unit number: %u I2C bus type: %08Xh", ulUnit, ulI2CBusType);
            Log("CgosI2CIsAvailable","FAILED",buf);            
        }
    }

    /* Perform standard I2C bus read/write tests */
    report(TEXT("\nThe next test assumes that an EEPROM with a min. size of 128 bytes is\n"));
    report(TEXT("connected to the standard I2C bus. The I2C bus address of the EEPROM is\n"));
    report(TEXT("expected to be A0h. The contents of this EEPROM will be filled with a\n"));
    report(TEXT("pattern during the test.\n"));
    report(TEXT("\nEnter [s] to skip the test, [c] to change the EEPROM base address or any other\n"));
    report(TEXT("key to start the test using the above assumptions.\n"));

    report(TEXT("\n"));
    ch = getchar();
    if(ch != 's')
    {
        bEEPAddr = 0xA0;
        if(ch == 'c')
        {
            /* Get new parameters */
						unsigned int addr;
            report(TEXT("Enter new EEPROM base address (hex.):\n"));
            scanf("%02X",&addr);
            report(TEXT("\n"));
            bEEPAddr = (char) (addr & 0xFF);
        }

        if(CgosI2CGetMaxFrequency(hCgos, ulPrimaryI2C, &ulMaxFreq))
        {                
            sprintf(buf, TEXT("Max. frequency of primary I2C bus: %dHz\n"), ulMaxFreq);
            report(buf);
            Log("CgosI2CGetMaxFrequency","SUCCESS","CGOS_I2C_TYPE_PRIMARY");                
        }
        else
        {
            report(TEXT("Failed to get max. frequency of primary I2C bus.\n"));
            Log("CgosI2CGetMaxFrequency","FAILED","CGOS_I2C_TYPE_PRIMARY");
        }

        if(CgosI2CGetFrequency(hCgos, ulPrimaryI2C, &ulCurrentFreq))
        {                
            sprintf(buf, TEXT("Current frequency of primary I2C bus: %dHz\n"), ulCurrentFreq);
            report(buf);
            Log("CgosI2CGetFrequency","SUCCESS","CGOS_I2C_TYPE_PRIMARY");                
        }
        else
        {
            report(TEXT("Failed to get current frequency of primary I2C bus.\n"));
            Log("CgosI2CGetFrequency","FAILED","CGOS_I2C_TYPE_PRIMARY");
        }

        if(CgosI2CSetFrequency(hCgos, ulPrimaryI2C, ulMaxFreq))
        {                
            sprintf(buf, TEXT("Set current frequency of primary I2C bus to %dHz.\n"), ulMaxFreq);
            report(buf);
            Log("CgosI2CSetFrequency","SUCCESS","CGOS_I2C_TYPE_PRIMARY");                
        }
        else
        {
            report(TEXT("Failed to set current frequency of primary I2C bus.\n"));
            Log("CgosI2CSetFrequency","FAILED","CGOS_I2C_TYPE_PRIMARY");
        }

        if(CgosI2CGetFrequency(hCgos, ulPrimaryI2C, &ulCurrentFreq))
        {                
            sprintf(buf, TEXT("Current frequency of primary I2C bus: %dHz\n"), ulCurrentFreq);
            report(buf);
            Log("CgosI2CGetFrequency","SUCCESS","CGOS_I2C_TYPE_PRIMARY");                
        }
        else
        {
            report(TEXT("Failed to get current frequency of primary I2C bus.\n"));
            Log("CgosI2CGetFrequency","FAILED","CGOS_I2C_TYPE_PRIMARY");
        }

        report(TEXT("\n"));
        report(TEXT("Press key to continue.\n"));
        ch = getchar();
        report(TEXT("\n"));
            

        report(TEXT("Fill EEPROM using CgosI2CWriteRegister function.\n"));
        report(TEXT("Afterwards read data back using CgosI2CReadRegister and compare with the\n"));
        report(TEXT("original write data.\n"));

        bLogWriteSuccess = TRUE;
        bLogReadSuccess = TRUE;
        bLogReadState = TRUE;
        bLogDataError = FALSE;
        
		bufPtr = buf;
        for(usEEPIndex = 0; usEEPIndex < EEP_SIZE; usEEPIndex++)
        {
            /* Use this to set the in/out buffers */
            bufEEPOut[usEEPIndex] = (unsigned char)usEEPIndex;
            bufEEPIn[usEEPIndex] = 0x00;

            /* Write data */
            if(!(CgosI2CWriteRegister(hCgos, ulPrimaryI2C, bEEPAddr, usEEPIndex, bufEEPOut[usEEPIndex])))
            {
                report(TEXT("ERROR: Failed to write to EEPROM.\n"));
                bLogWriteSuccess = FALSE;
                bLogReadState = FALSE;
                break;
            }

            /* Give 10ms delay to complete EEPROM internal write cycle */
            Sleep(10);

            /* Read data back */
            if(CgosI2CReadRegister(hCgos, ulPrimaryI2C, (unsigned char) (bEEPAddr | 0x01), usEEPIndex, &bufEEPIn[usEEPIndex]))
            {
				sprintf(bufPtr, TEXT("%02Xh "),bufEEPIn[usEEPIndex]);
				bufPtr += 4;
                if(bufEEPIn[usEEPIndex] != bufEEPOut[usEEPIndex])
                {
                    report(TEXT("\nERROR: Read back value does not match.\n"));
                    bLogDataError = TRUE;
                    bLogReadSuccess = FALSE;
                    break;
                }
            }
            else
            {
                report(TEXT("\nERROR: Failed to read from EEPROM.\n"));
                bLogReadSuccess = FALSE;
                break;
            }

            /* Clear in buffer again */
            bufEEPIn[usEEPIndex] = 0x00;
        }
		
		if (bLogReadSuccess)
			report(buf);

        report(TEXT("\n"));
        if(bLogWriteSuccess == TRUE)
        {
            report(TEXT("Data successfully written to EEPROM.\n"));
            Log("CgosI2CWriteRegister","SUCCESS","CGOS_I2C_TYPE_PRIMARY");
        }
        else
        {
            report(TEXT("Failed to write data - read back skipped.\n"));
            Log("CgosI2CWriteRegister","FAILED","CGOS_I2C_TYPE_PRIMARY");
        }
        if(bLogReadState == TRUE)
        {
            if(bLogReadSuccess == TRUE)
            {
                report(TEXT("Data read back and compared successfully.\n"));
                Log("CgosI2CReadRegister","SUCCESS","CGOS_I2C_TYPE_PRIMARY");
            }
            else if (bLogDataError)
            {
                report(TEXT("ERROR: Read back data does not match write data.\n"));
                Log("CgosI2CReadRegister","FAILED","CGOS_I2C_TYPE_PRIMARY (Data Error)");
            }
            else
            {
                report(TEXT("ERROR: Failed to read data from EEPROM.\n"));
                Log("CgosI2CReadRegister","FAILED","CGOS_I2C_TYPE_PRIMARY (Function Error)");
            }
        }
        
        report(TEXT("\n"));

        /* Now perform same test using standard read/write functions */
        report(TEXT("Fill EEPROM using CgosI2CWrite function.\n"));
        report(TEXT("Afterwards read data back using CgosI2CRead and compare with the original\n"));
        report(TEXT("write data.\n"));
       
        /* As EEPROMs always need some time to store the given data internally, we can 
           not write the whole block. Thus this is only a test whether the CgosI2CWrite
           function principally works. However, we cannot write the whole block of data
           with one call, but have to split it up in single writes. First we always set
           the index, then we send the data to be stored at the given index. */
        bLogWriteSuccess = TRUE;
        bLogReadSuccess = TRUE;
        bLogReadState = TRUE;
        bLogDataError = FALSE;

        for(usEEPIndex = 0; usEEPIndex < EEP_SIZE; usEEPIndex++)
        {
            /* Iniitalize buffers */
            bufEEPOut[usEEPIndex] = ((unsigned char)usEEPIndex) | 0x80;
            bufEEPIn[usEEPIndex] = 0x00;
#ifdef NEVER
            /* Write index */           
            if(!(CgosI2CWrite(hCgos, ulPrimaryI2C, bEEPAddr, (unsigned char *)&usEEPIndex, 1)))
            {
                report(TEXT("ERROR: Failed to write to EEPROM.\n"));
                bLogWriteSuccess = FALSE;
                bLogReadState = FALSE;
                break;
            }
            
            /* Give 10ms delay to complete EEPROM internal write cycle */
            Sleep(10);

            /* Write data */
            if(!(CgosI2CWrite(hCgos, ulPrimaryI2C, bEEPAddr, &bufEEPOut[usEEPIndex], 1)))
            {
                report(TEXT("ERROR: Failed to write to EEPROM.\n"));
                bLogWriteSuccess = FALSE;
                bLogReadState = FALSE;
                break;
            }
#endif
            /* Write data */
            if(!(CgosI2CWriteRegister(hCgos, ulPrimaryI2C, bEEPAddr, usEEPIndex, bufEEPOut[usEEPIndex])))
            {
                report(TEXT("ERROR: Failed to write to EEPROM.\n"));
                bLogWriteSuccess = FALSE;
                bLogReadState = FALSE;
                break;
            }
            /* Give 10ms delay to complete EEPROM internal write cycle */
            Sleep(10);
        }

        /* Set EEPROM index back to start */
        bufEEPIn[0] = 0x00;
        if(!(CgosI2CWrite(hCgos, ulPrimaryI2C, bEEPAddr, (unsigned char *)&bufEEPIn[0], 1)))
        {
            report(TEXT("ERROR: Failed to write to EEPROM.\n"));
            bLogWriteSuccess = FALSE;
            bLogReadState = FALSE;
        }
        Sleep(100);

        if(bLogWriteSuccess == TRUE)
        {
            report(TEXT("Data successfully written to EEPROM.\n"));
            Log("CgosI2CWrite","SUCCESS","CGOS_I2C_TYPE_PRIMARY");
        }
        else
        {
            report(TEXT("Failed to write data - read back skipped.\n"));
            Log("CgosI2CWrite","FAILED","CGOS_I2C_TYPE_PRIMARY");
        }

        /* We only read the data back if the write function succeeded */
        if(bLogReadState == TRUE)
        {
            /* Now read whole EEPROM with on read command */
            if(CgosI2CRead(hCgos, ulPrimaryI2C, (unsigned char) (bEEPAddr | 0x01), &bufEEPIn[0], EEP_SIZE))
            {
                /* Function successful, now compare read and write data */
				bufPtr = buf;
                for(usEEPIndex = 0; usEEPIndex < EEP_SIZE; usEEPIndex++)
                {
					sprintf(bufPtr, TEXT("%02Xh "),bufEEPIn[usEEPIndex]);
					bufPtr += 4;
                    if(bufEEPIn[usEEPIndex] != bufEEPOut[usEEPIndex])
                    {
                        report(TEXT("\nERROR: Read back value does not match.\n"));
                        bLogDataError = TRUE;
                        bLogReadSuccess = FALSE;
                        break;
                    }
                }
				report(buf);
            }
            else
            {
                report(TEXT("\nERROR: Failed to read from EEPROM.\n"));
                bLogReadSuccess = FALSE;
            }

            report(TEXT("\n"));

            if(bLogReadSuccess == TRUE)
            {
                report(TEXT("Data read back and compared successfully.\n"));
                Log("CgosI2CRead","SUCCESS","CGOS_I2C_TYPE_PRIMARY");
            }
            else if (bLogDataError)
            {
                report(TEXT("ERROR: Read back data does not match write data.\n"));
                Log("CgosI2CRead","FAILED","CGOS_I2C_TYPE_PRIMARY (Data Error)");
            }
            else
            {
                report(TEXT("ERROR: Failed to read data from EEPROM.\n"));
                Log("CgosI2CRead","FAILED","CGOS_I2C_TYPE_PRIMARY (Function Error)");
            }
        }
    }


    /* Perform EPI I2C bus read/write tests */
    report(TEXT("\nThe next test assumes that an EEPROM with a min. size of 128 bytes is\n"));
    report(TEXT("connected to the EPI I2C bus. The I2C bus address of the EEPROM is\n"));
    report(TEXT("expected to be A0h. The contents of this EEPROM will be filled with a\n"));
    report(TEXT("pattern during the test.\n"));
    report(TEXT("Enter [s] to skip the test, [c] to change the EEPROM base address or any other\n"));
    report(TEXT("key to start the test using the above assumptions.\n"));

    report(TEXT("\n"));
    ch = getchar();
    if(ch != 's')
    {
        bEEPAddr = 0xA0;
        if(ch == 'c')
        {
            /* Get new parameters */
            unsigned int addr;
            report(TEXT("Enter new EEPROM base address (hex.):\n"));
            scanf("%02X", &addr);
            report(TEXT("\n"));
            bEEPAddr = (char) (addr & 0xFF);
        }
        report(TEXT("Fill EEPROM using CgosI2CWriteRegister function, read back using"));
        report(TEXT("CgosI2CReadRegister and compare.\n"));

        bLogWriteSuccess = TRUE;
        bLogReadSuccess = TRUE;
        bLogReadState = TRUE;
        bLogDataError = FALSE;
        
		bufPtr = buf;
        for(usEEPIndex = 0; usEEPIndex < EEP_SIZE; usEEPIndex++)
        {
            /* Use this to set the in/out buffers */
            bufEEPOut[usEEPIndex] = (unsigned char)usEEPIndex;
            bufEEPIn[usEEPIndex] = 0x00;

            /* Write data */
            if(!(CgosI2CWriteRegister(hCgos, ulDDCI2C, bEEPAddr, usEEPIndex, bufEEPOut[usEEPIndex])))
            {
                report(TEXT("ERROR: Failed to write to EEPROM.\n"));
                bLogWriteSuccess = FALSE;
                bLogReadState = FALSE;
                break;
            }

            /* Give 10ms delay to complete EEPROM internal write cycle */
            Sleep(10);

            /* Read data back */
            if(CgosI2CReadRegister(hCgos, ulDDCI2C, (unsigned char) (bEEPAddr | 0x01), usEEPIndex, &bufEEPIn[usEEPIndex]))
            {
				sprintf(bufPtr, TEXT("%02Xh "),bufEEPIn[usEEPIndex]); 
				bufPtr += 4;
                if(bufEEPIn[usEEPIndex] != bufEEPOut[usEEPIndex])
                {
                    report(TEXT("\nERROR: Read back value does not match.\n"));
                    bLogDataError = TRUE;
                    bLogReadSuccess = FALSE;
                    break;
                }
            }
            else
            {
                report(TEXT("\nERROR: Failed to read from EEPROM.\n"));
                bLogReadSuccess = FALSE;
                break;
            }

            /* Clear in buffer again */
            bufEEPIn[usEEPIndex] = 0x00;
        }
		if (bLogReadSuccess == TRUE)
			report(buf);

        report(TEXT("\n"));

        if(bLogWriteSuccess == TRUE)
        {
            report(TEXT("Data successfully written to EEPROM.\n"));
            Log("CgosI2CWriteRegister","SUCCESS","CGOS_I2C_TYPE_DDC");
        }
        else
        {
            report(TEXT("Failed to write data - read back skipped.\n"));
            Log("CgosI2CWriteRegister","FAILED","CGOS_I2C_TYPE_DDC");
        }
        if(bLogReadState == TRUE)
        {
            if(bLogReadSuccess == TRUE)
            {
                report(TEXT("Data read back and compared successfully.\n"));
                Log("CgosI2CReadRegister","SUCCESS","CGOS_I2C_TYPE_DDC");
            }
            else if (bLogDataError)
            {
                report(TEXT("ERROR: Read back data does not match write data.\n"));
                Log("CgosI2CReadRegister","FAILED","CGOS_I2C_TYPE_DDC (Data Error)");
            }
            else
            {
                report(TEXT("ERROR: Failed to read data from EEPROM.\n"));
                Log("CgosI2CReadRegister","FAILED","CGOS_I2C_TYPE_DDC (Function Error)");
            }
        }
        
        report(TEXT("\n"));

        /* Now perform same test using standard read/write functions */
        report(TEXT("Fill EEPROM using CgosI2CWrite function.\n"));
        report(TEXT("Afterwards read data back using CgosI2CRead and compare\n"));
        report(TEXT("with the original write data.\n"));
       
        /* As EEPROMs always need some time to store the given data internally, we can 
           not write the whole block. Thus this is only a test whether the CgosI2CWrite
           function principally works. However, we cannot write the whole block of data
           with one call, but have to split it up in single writes. First we always set
           the index, then we send the data to be stored at the given index. */
        bLogWriteSuccess = TRUE;
        bLogReadSuccess = TRUE;
        bLogReadState = TRUE;
        bLogDataError = FALSE;

        for(usEEPIndex = 0; usEEPIndex < EEP_SIZE; usEEPIndex++)
        {
            /* Iniitalize buffers */
            bufEEPOut[usEEPIndex] = ((unsigned char)usEEPIndex) | 0x80;
            bufEEPIn[usEEPIndex] = 0x00;
#ifdef NEVER
            /* Write index */
            if(!(CgosI2CWrite(hCgos, ulDDCI2C, bEEPAddr, (unsigned char *)&usEEPIndex, 1)))
            {
                report(TEXT("ERROR: Failed to write to EEPROM.\n"));
                bLogWriteSuccess = FALSE;
                bLogReadState = FALSE;
                break;
            }

            /* Give 10ms delay to complete EEPROM internal write cycle */
            Sleep(10);

            /* Write data */
            if(!(CgosI2CWrite(hCgos, ulDDCI2C, bEEPAddr, &bufEEPOut[usEEPIndex], 1)))
            {
                report(TEXT("ERROR: Failed to write to EEPROM.\n"));
                bLogWriteSuccess = FALSE;
                bLogReadState = FALSE;
                break;
            }
#endif
            /* Write data */
            if(!(CgosI2CWriteRegister(hCgos, ulDDCI2C, bEEPAddr, usEEPIndex, bufEEPOut[usEEPIndex])))
            {
                report(TEXT("ERROR: Failed to write to EEPROM.\n"));
                bLogWriteSuccess = FALSE;
                bLogReadState = FALSE;
                break;
            }
            /* Give 10ms delay to complete EEPROM internal write cycle */
            Sleep(10);
        }

        /* Set EEPROM index back to start */
        bufEEPIn[0] = 0x00;
        if(!(CgosI2CWrite(hCgos, ulDDCI2C, bEEPAddr, (unsigned char *)&bufEEPIn[0], 1)))
        {
            report(TEXT("ERROR: Failed to write to EEPROM.\n"));
            bLogWriteSuccess = FALSE;
            bLogReadState = FALSE;
        }
        Sleep(100);

        if(bLogWriteSuccess == TRUE)
        {
            report(TEXT("Data successfully written to EEPROM.\n"));
            Log("CgosI2CWrite","SUCCESS","CGOS_I2C_TYPE_DDC");
        }
        else
        {
            report(TEXT("Failed to write data - read back skipped.\n"));
            Log("CgosI2CWrite","FAILED","CGOS_I2C_TYPE_DDC");
        }

        /* We only read the data back if the write function succeeded */
        if(bLogReadState == TRUE)
        {
            /* Now read whole EEPROM with on read command */
            if(CgosI2CRead(hCgos, ulDDCI2C, (unsigned char) (bEEPAddr | 0x01), &bufEEPIn[0], EEP_SIZE))
            {                                                                       /*MOD001*/
                /* Function successful, now compare read and write data */
				bufPtr = buf;
                for(usEEPIndex = 0; usEEPIndex < EEP_SIZE; usEEPIndex++)
                {
					sprintf(bufPtr, TEXT("%02Xh "),bufEEPIn[usEEPIndex]);
					bufPtr += 4;
                    if(bufEEPIn[usEEPIndex] != bufEEPOut[usEEPIndex])
                    {
                        report(TEXT("\nERROR: Read back value does not match.\n"));
                        bLogDataError = TRUE;
                        bLogReadSuccess = FALSE;
                        break;
                    }
                }
				report(buf);                                                        /*MOD001*/
            }
            else
            {
                report(TEXT("\nERROR: Failed to read from EEPROM.\n"));
                bLogReadSuccess = FALSE;
            }

            report(TEXT("\n"));
            if(bLogReadSuccess == TRUE)
            {
                report(TEXT("Data read back and compared successfully.\n"));
                Log("CgosI2CRead","SUCCESS","CGOS_I2C_TYPE_DDC");
            }
            else if (bLogDataError)
            {
                report(TEXT("ERROR: Read back data does not match write data.\n"));
                Log("CgosI2CRead","FAILED","CGOS_I2C_TYPE_DDC (Data Error)");
            }
            else
            {
                report(TEXT("ERROR: Failed to read data from EEPROM.\n"));
                Log("CgosI2CRead","FAILED","CGOS_I2C_TYPE_DDC (Function Error)");
            }
        }
    }

    Log("CgosI2CWriteReadCombined","! NOT TESTED !","");
    return;

}

/*---------------------------------------------------------------------------
 * Name: HandleWD
 * Desc: Test CGOS watchdog functions.
 * Inp:  argc   - Number of command line arguments passed
 *       argv[] - Array of pointers to command line parameters
 * Outp: none       
 *---------------------------------------------------------------------------
 */
void HandleWD(int argc, _TCHAR* argv[])
{
    char ch;
    CGOSWDCONFIG wdConfig = {0};
    CGOSWDINFO wdInfo = {0};
    unsigned char ucCleanConf[sizeof(wdConfig)] = {0};
    unsigned int ulUnit, ulWDCount;
	unsigned short i;
    

    report(TEXT("\n****************************************\n"));
    report(TEXT("Testing CGOS watchdog functions.\n"));
    report(TEXT("****************************************\n"));
    report(TEXT("Enter [s] to skip test, any other key to continue.\n"));
    ch = getchar();
    if(ch == 's')
    {
        return;
    }

    /* Clear watchdog configuration structure */
    memcpy(&wdConfig, ucCleanConf, sizeof(wdConfig));
    wdConfig.dwSize = sizeof(wdConfig);

    if((ulWDCount = CgosWDogCount(hCgos)) != 0)
    {
		sprintf(buf, TEXT("\nNumber of available watchdogs: %u.\n"),ulWDCount); report(buf);
        Log("CgosWDogCount","SUCCESS","");
    }
    else
    {
        report(TEXT("\nERROR: Failed to get number of available watchdogs.\n"));
        Log("CgosWDogCount","FAILED","");
    }

    ulUnit = 0;
    if(CgosWDogIsAvailable(hCgos, ulUnit))
    {
		sprintf(buf, TEXT("\nWatchdog number %u is available.\n"),ulUnit); report(buf);
        Log("CgosWDogIsAvailable","SUCCESS","");
    }
    else
    {
		sprintf(buf, TEXT("\nERROR: Failed to get availability state of watchdog %u.\n"),ulUnit); report(buf);
        Log("CgosWDogIsAvailable","FAILED","");
    }

    wdInfo.dwSize = sizeof(wdInfo);
    if(CgosWDogGetInfo(hCgos, ulUnit, &wdInfo))
    {
		sprintf(buf, TEXT("Information structure for watchdog %u:\n"),ulUnit); report(buf);
        Log("CgosWDogGetInfo","SUCCESS","");

		sprintf(buf, 
        TEXT("Size of Structure: %u\n")
        TEXT("Flags: %Xh\n")
        TEXT("Min Timeout: %u\n")
        TEXT("Max Timeout: %u\n")
        TEXT("Min Delay: %u\n")
        TEXT("Max Delay: %u\n")
        TEXT("Supported OpModes: %Xh\n")
        TEXT("Max Stage Count: %u\n")
        TEXT("Supported Events: %Xh\n")
        TEXT("Type: %08Xh\n"),
        wdInfo.dwSize,
        wdInfo.dwFlags,
        wdInfo.dwMinTimeout,
        wdInfo.dwMaxTimeout,
        wdInfo.dwMinDelay,
        wdInfo.dwMaxDelay,
        wdInfo.dwOpModes,
        wdInfo.dwMaxStageCount,
        wdInfo.dwEvents,
        wdInfo.dwType);
        report(buf);
    }
    else
    {
		sprintf(buf, TEXT("ERROR: Failed to get information structure for watchdog %u.\n"),ulUnit); report(buf);
        Log("CgosWDogGetInfo","FAILED","");
    }
    report(TEXT("\nPress [ENTER] to continue...\n"));
    getchar();

    report(TEXT("\nThe next test will initialize the watchdog using the simplified\n"));
    report(TEXT("CgosWDogSetConfig command. The watchdog will be configured to generate\n"));
    report(TEXT("a reset event after a delay of 30secs. and a timeout of 10secs.\n"));
    report(TEXT("For reference the watchdog configuration will be read back and displayed.\n"));
    report(TEXT("Once started, you can wait for the watchdog to generate the event or\n"));
    report(TEXT("enter [t] to trigger and [d] to disable the watchdog.\n"));

    report(TEXT("\nEnter [s] to skip this test or any other key to start.\n\n"));
    ch = getchar();
    if(ch != 's')
    {
        /* Ensure log file is updated on file system */
        if (LogFlushReopen(szLogFile) == FALSE)
        {
            report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
            return;
        }

        if(CgosWDogSetConfig(hCgos, ulUnit, 10000, 30000, CGOS_WDOG_MODE_REBOOT_PC))
        {
            report(TEXT("Watchdog started.\n"));
            Log("CgosWDogSetConfig", "SUCCESS", "");

            /* Ensure log file is updated on file system */
            if (LogFlushReopen(szLogFile) == FALSE)
            {
                report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                return;
            }

            if(CgosWDogGetConfigStruct(hCgos, ulUnit, &wdConfig))
            {
                Log("CgosWDogGetConfigStruct", "SUCCESS", "");

                /* Ensure log file is updated on file system */
                if (LogFlushReopen(szLogFile) == FALSE)
                {
                    report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                    return;
                }

				sprintf(buf, TEXT("Current watchdog %u configuration:\n"),ulUnit); report(buf);

				sprintf(buf, 
				TEXT("Size of Structure: %u\n")
                TEXT("Timeout: %u\n")
                TEXT("Delay: %u\n")
                TEXT("Mode: %Xh\n")
                TEXT("Operating Mode: %Xh\n")
                TEXT("Stage Count: %u\n")
                TEXT("Event Stage 1: %08Xh\n")
                TEXT("Timeout Stage 1: %u\n")
                TEXT("Event Stage 2: %08Xh\n")
                TEXT("Timeout Stage 2: %u\n")
                TEXT("Event Stage 3: %08Xh\n")
                TEXT("Timeout Stage 3: %u\n"),
                wdConfig.dwSize,
                wdConfig.dwTimeout,
                wdConfig.dwDelay,
                wdConfig.dwMode,
                wdConfig.dwOpMode,
                wdConfig.dwStageCount,                
                wdConfig.stStages[0].dwEvent,
                wdConfig.stStages[0].dwTimeout,
                wdConfig.stStages[1].dwEvent,
                wdConfig.stStages[1].dwTimeout,
                wdConfig.stStages[2].dwEvent,
                wdConfig.stStages[2].dwTimeout);
				report(buf);
			}
            else
            {
                Log("CgosWDogGetConfigStruct", "FAILED", "");

                /* Ensure log file is updated on file system */
                if (LogFlushReopen(szLogFile) == FALSE)
                {
                    report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                    return;
                }

                report(TEXT("ERROR: Failed to read back watchdog configuration.\n"));
            }

			/* trigger the watchdog three times */
#if 1
			for(i=0; i<3; i++)
			{

				if(CgosWDogTrigger(hCgos, ulUnit))
                {
                   report(TEXT("Watchdog triggered.\n"));
                   Log("CgosWDogTrigger", "SUCCESS", "");
                   /* Ensure log file is updated on file system */
                   if (LogFlushReopen(szLogFile) == FALSE)
                        {
                            report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                            break;
                        }
                }
                else
                {
                   report(TEXT("ERROR: Failed to trigger watchdog.\n"));
                   Log("CgosWDogTrigger", "FAILED", "");
                   /* Ensure log file is updated on file system */
                   if (LogFlushReopen(szLogFile) == FALSE)
                        {
                            report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                            break;
                        }
                }

				/* sleep 5s */
				Sleep(5000);
			}

			if (flagByte & flgWdogReboot)
			{
				/* force a system reset through the watchdog */
				Log("Cgos Watchdog","reboot","forced");
				report(TEXT("\nSystem now should reboot within a few seconds\n"));
				return;
			}

			/* if we're here, the watchdog obviously didn't got us ;-) */
            if(CgosWDogDisable(hCgos, ulUnit))
            {
                report(TEXT("Watchdog disabled.\n"));
                Log("CgosWDogDisable", "SUCCESS", "");
                /* Ensure log file is updated on file system */
                if (LogFlushReopen(szLogFile) == FALSE)
                {
                     report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                     return;
                }
            }
            else
            {
                report(TEXT("ERROR: Failed to disable watchdog.\n"));
                Log("CgosWDogDisable", "FAILED", "");
                /* Ensure log file is updated on file system */
                if (LogFlushReopen(szLogFile) == FALSE)
                {
                     report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                     return;
                }
            }

#else
            do
            {
                ch = getchar();
                if(ch == 't')
                {
                    if(CgosWDogTrigger(hCgos, ulUnit))
                    {
                        report(TEXT("Watchdog triggered.\n"));
                        Log("CgosWDogTrigger", "SUCCESS", "");
                        /* Ensure log file is updated on file system */
                        if (LogFlushReopen(szLogFile) == FALSE)
                        {
                            report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                            return;
                        }
                    }
                    else
                    {
                        report(TEXT("ERROR: Failed to trigger watchdog.\n"));
                        Log("CgosWDogTrigger", "FAILED", "");
                        /* Ensure log file is updated on file system */
                        if (LogFlushReopen(szLogFile) == FALSE)
                        {
                            report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                            return;
                        }
                    }
                }
                else if(ch == 'd')
                {
                    if(CgosWDogDisable(hCgos, ulUnit))
                    {
                        report(TEXT("Watchdog disabled.\n"));
                        Log("CgosWDogDisable", "SUCCESS", "");
                        /* Ensure log file is updated on file system */
                        if (LogFlushReopen(szLogFile) == FALSE)
                        {
                            report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                            return;
                        }
                    }
                    else
                    {
                        report(TEXT("ERROR: Failed to disable watchdog.\n"));
                        Log("CgosWDogDisable", "FAILED", "");
                        /* Ensure log file is updated on file system */
                        if (LogFlushReopen(szLogFile) == FALSE)
                        {
                            report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                            return;
                        }
                    }
                }
            }while(ch != 'd');
#endif

        }
        else
        {
            report(TEXT("ERROR: Failed to initialize watchdog.\n"));
            Log("CgosWDogSetConfig", "FAILED", "");
            /* Ensure log file is updated on file system */
            if (LogFlushReopen(szLogFile) == FALSE)
            {
                report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                return;
            }
        }   
    }


    /* Clear watchdog configuration structure */
    memcpy(&wdConfig, ucCleanConf, sizeof(wdConfig));
    wdConfig.dwSize = sizeof(wdConfig);

    report(TEXT("\nPress [ENTER] to continue...\n"));
    getchar();

    report(TEXT("\nThe next test will initialize the watchdog using the full featured\n"));
    report(TEXT("CgosWDogSetConfigStruct command. The watchdog will be configured to generate\n"));
    report(TEXT("a power button event as first stage after a delay of 30secs. and a timeout\n")); 
    report(TEXT("of 10secs. The second stage after an additional timeout of 20secs will\n")); 
    report(TEXT("generate a reset event. Operation mode is set to single event.\n")); 
    report(TEXT("For reference the watchdog configuration will be read back and displayed.\n"));
    report(TEXT("Once started, you can wait for the watchdog to generate the event or\n"));
    report(TEXT("enter [t] to trigger and [d] to disable the watchdog.\n"));

    report(TEXT("\nEnter [s] to skip this test or any other key to start.\n\n"));
    ch = getchar();
    if(ch != 's')
    {
        /* Ensure log file is updated on file system */
        if (LogFlushReopen(szLogFile) == FALSE)
        {
            report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
            return;
        }
        /* Init watchdog stages */
        wdConfig.dwMode = CGOS_WDOG_MODE_STAGED;
        wdConfig.dwStageCount = 2;
        wdConfig.dwDelay = 30000;
        wdConfig.dwOpMode = CGOS_WDOG_OPMODE_SINGLE_EVENT;
        wdConfig.stStages[0].dwEvent = CGOS_WDOG_EVENT_BTN;
        wdConfig.stStages[0].dwTimeout = 10000;
        wdConfig.stStages[1].dwEvent = CGOS_WDOG_EVENT_RST;
        wdConfig.stStages[1].dwTimeout = 20000;

        if(CgosWDogSetConfigStruct(hCgos, ulUnit, &wdConfig))
        {
            report(TEXT("Watchdog started.\n"));
            Log("CgosWDogSetConfigStruct", "SUCCESS", "");
            /* Ensure log file is updated on file system */
            if (LogFlushReopen(szLogFile) == FALSE)
            {
                report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                return;
            }

            /* Clear watchdog configuration structure */
            memcpy(&wdConfig, ucCleanConf, sizeof(wdConfig));
            wdConfig.dwSize = sizeof(wdConfig);
            
            if(CgosWDogGetConfigStruct(hCgos, ulUnit, &wdConfig))
            {
                Log("CgosWDogGetConfigStruct", "SUCCESS", "");
                /* Ensure log file is updated on file system */
                if (LogFlushReopen(szLogFile) == FALSE)
                {
                    report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                    return;
                }
                sprintf(buf, TEXT("Current watchdog %u configuration:\n"),ulUnit); report(buf);

                sprintf(buf,
                "Size of Structure: %u\n"
                "Timeout: %u\n"
                "Delay: %u\n"
                "Mode: %Xh\n"
                "Operating Mode: %Xh\n"
                "Stage Count: %u\n"
                "Event Stage 1: %08Xh\n"
                "Timeout Stage 1: %u\n"
                "Event Stage 2: %08Xh\n"
                "Timeout Stage 2: %u\n"
                "Event Stage 3: %08Xh\n"
                "Timeout Stage 3: %u\n",
                wdConfig.dwSize,
                wdConfig.dwTimeout,
                wdConfig.dwDelay,
                wdConfig.dwMode,
                wdConfig.dwOpMode,
                wdConfig.dwStageCount,                
                wdConfig.stStages[0].dwEvent,
                wdConfig.stStages[0].dwTimeout,
                wdConfig.stStages[1].dwEvent,
                wdConfig.stStages[1].dwTimeout,
                wdConfig.stStages[2].dwEvent,
                wdConfig.stStages[2].dwTimeout
                );
                report(buf);
            }
            else
            {
                Log("CgosWDogGetConfigStruct", "FAILED", "");
                /* Ensure log file is updated on file system */
                if (LogFlushReopen(szLogFile) == FALSE)
                {
                    report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                    return;
                }
                report(TEXT("ERROR: Failed to read back watchdog configuration.\n"));
            }
            do
            {
                ch = getchar();
                if(ch == 't')
                {
                    if(CgosWDogTrigger(hCgos, ulUnit))
                    {
                        report(TEXT("Watchdog triggered.\n"));
                        Log("CgosWDogTrigger", "SUCCESS", "");
                        /* Ensure log file is updated on file system */
                        if (LogFlushReopen(szLogFile) == FALSE)
                        {
                            report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                            return;
                        }
                    }
                    else
                    {
                        report(TEXT("ERROR: Failed to trigger watchdog.\n"));
                        Log("CgosWDogTrigger", "FAILED", "");
                        /* Ensure log file is updated on file system */
                        if (LogFlushReopen(szLogFile) == FALSE)
                        {
                            report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                            return;
                        }
                    }
                }
                else if(ch == 'd')
                {
                    if(CgosWDogDisable(hCgos, ulUnit))
                    {
                        report(TEXT("Watchdog disabled.\n"));
                        Log("CgosWDogDisable", "SUCCESS", "");
                        /* Ensure log file is updated on file system */
                        if (LogFlushReopen(szLogFile) == FALSE)
                        {
                            report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                            return;
                        }
                    }
                    else
                    {
                        report(TEXT("ERROR: Failed to disable watchdog.\n"));
                        Log("CgosWDogDisable", "FAILED", "");
                        /* Ensure log file is updated on file system */
                        if (LogFlushReopen(szLogFile) == FALSE)
                        {
                            report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                            return;
                        }
                    }
                }
            }while(ch != 'd');
        }
        else
        {
            report(TEXT("ERROR: Failed to initialize watchdog.\n"));
            Log("CgosWDogSetConfigStruct", "FAILED", "");
            /* Ensure log file is updated on file system */
            if (LogFlushReopen(szLogFile) == FALSE)
            {
                report(TEXT("\nERROR: Failed to flush and re-open log file!\n"));
                return;
            }
        }
    }
    return;
}

/*---------------------------------------------------------------------------
 * Name: HandleMonitor
 * Desc: Test CGOS monitor functions.
 * Inp:  argc   - Number of command line arguments passed
 *       argv[] - Array of pointers to command line parameters
 * Outp: none       
 *---------------------------------------------------------------------------
 */
void HandleMonitor(int argc, _TCHAR* argv[])
{
    CGOSTEMPERATUREINFO tempInfo = {0};
    CGOSFANINFO fanInfo = {0};
    CGOSVOLTAGEINFO voltageInfo = {0};
    unsigned char ch;
    unsigned int ulUnit, ulSetting, ulState,ulMonCount;

    report(TEXT("\n****************************************\n"));
    report(TEXT("Testing CGOS monitor functions.\n"));
    report(TEXT("****************************************\n"));
    report(TEXT("Enter [s] to skip test, any other key to continue.\n"));
    ch = getchar();
    if(ch == 's')
    {
        return;
    }

    tempInfo.dwSize = sizeof (tempInfo);
    voltageInfo.dwSize = sizeof (voltageInfo);
    fanInfo.dwSize = sizeof (fanInfo);

    report(TEXT("\n"));
    ulMonCount = CgosTemperatureCount(hCgos);
    Log("CgosTemperatureCount", "SUCCESS", "");
    sprintf(buf, TEXT("Number of temperature monitors: %u\n"), ulMonCount); report(buf);

    if(ulMonCount != 0)
    {
        for(ulUnit = 0; ulUnit < ulMonCount; ulUnit++)
        {
            if(CgosTemperatureGetInfo(hCgos, ulUnit, &tempInfo))
            {
				sprintf( buf,
				 TEXT("\nTemperature monitor %u information:\n")
                 TEXT("Type:           %Xh\n")
                 TEXT("Flags:          %Xh\n")
                 TEXT("Alarm:          %Xh\n")
                 TEXT("Resolution:     %d\n")
                 TEXT("Max. Value:     %d\n")
                 TEXT("Min. Value:     %d\n")
                 TEXT("AlarmHigh:      %d\n")
                 TEXT("AlarmLow:       %d\n")
                 TEXT("HystHigh:       %d\n")
                 TEXT("HystLow:        %d\n"),
				 ulUnit,
				 tempInfo.dwType,
				 tempInfo.dwFlags,
				 tempInfo.dwAlarm,
				 (int) (tempInfo.dwRes),
				 (int)(tempInfo.dwMax),
				 (int)(tempInfo.dwMin),
				 (int)(tempInfo.dwAlarmHi),
				 (int)(tempInfo.dwAlarmLo),
				 (int)(tempInfo.dwHystHi),
				 (int)(tempInfo.dwHystLo) );
				report(buf);

                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, tempInfo.dwType);
                Log("CgosTemperatureGetInfo", "SUCCESS", buf);
            }
            else
            {
                sprintf(buf, TEXT("ERROR: Failed to get info for temperature monitor %u.\n"), ulUnit); report(buf);
                sprintf(buf, "Unit number: %u", ulUnit);
                Log("CgosTemperatureGetInfo", "FAILED", buf);
            }
            do
            {
                if(CgosTemperatureGetCurrent(hCgos, ulUnit, &ulSetting, &ulState))
                {
                    sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, tempInfo.dwType);
                    Log("CgosTemperatureGetCurrent", "SUCCESS", buf);

					sprintf(buf,
						TEXT("\nCurrent setting:    %u\n")
                        TEXT("Current status:     %u\n"),
							ulSetting,
							ulState);
					report(buf);
//                    ch = getchar();
					ch = 'n';
                }
                else
                {
                    sprintf(buf, TEXT("ERROR: Failed to get current temperature monitor %u value.\n"), ulUnit);
					report(buf);
                    sprintf(buf, "Unit number: %u", ulUnit);
                    Log("CgosTemperatureGetCurrent", "FAILED", buf);
                    ch = 'n';
                }
            }
            while(ch == 'y');
            report(TEXT("\nPress [ENTER] to continue with next monitor.\n"));
            getchar();
        }

    }

    report(TEXT("\n"));
    ulMonCount = CgosVoltageCount(hCgos);
    Log("CgosVoltageCount", "SUCCESS", "");
    sprintf(buf, TEXT("Number of voltage monitors: %u\n"), ulMonCount);
    report(buf);
    if(ulMonCount != 0)
    {
        for(ulUnit = 0; ulUnit < ulMonCount; ulUnit++)
        {
            if(CgosVoltageGetInfo(hCgos, ulUnit, &voltageInfo))
            {
				sprintf(buf,
					TEXT("\nVoltage monitor %u information:\n")
					TEXT("Type:           %Xh\n")
					TEXT("Flags:          %Xh\n")
					TEXT("Alarm:          %Xh\n")
					TEXT("Resolution:     %d\n")
					TEXT("Nominal value:  %d\n")
					TEXT("Max. Value:     %d\n")
					TEXT("Min. Value:     %d\n")
					TEXT("AlarmHigh:      %d\n")
					TEXT("AlarmLow:       %d\n")
					TEXT("HystHigh:       %d\n")
					TEXT("HystLow:        %d\n"),
                ulUnit,
				voltageInfo.dwType,
				voltageInfo.dwFlags,
				voltageInfo.dwAlarm,
				(int) voltageInfo.dwRes,
				(int) voltageInfo.dwNom,
				(int) voltageInfo.dwMax,
				(int) voltageInfo.dwMin,
				(int) voltageInfo.dwAlarmHi,
				(int) voltageInfo.dwAlarmLo,
				(int) voltageInfo.dwHystHi,
				(int) voltageInfo.dwHystLo);
				report(buf);

				sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, voltageInfo.dwType);
                Log("CgosVoltageGetInfo", "SUCCESS", buf);
            }
            else
            {
                sprintf(buf, TEXT("ERROR: Failed to get info for voltage monitor %u.\n"), ulUnit);
				report(buf);
                sprintf(buf, "Unit number: %u", ulUnit);
                Log("CgosVoltageGetInfo", "FAILED", buf);
            }

            do
            {
                if(CgosVoltageGetCurrent(hCgos, ulUnit, &ulSetting, &ulState))
                {
                    sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, voltageInfo.dwType);
                    Log("CgosVoltageGetCurrent", "SUCCESS", buf);
                    report(TEXT("\n"));

                    sprintf(buf, TEXT("Current setting:    %u\n")
                     TEXT("Current status:     %u\n"),
						ulSetting,
						ulState );
					report(buf);
//                    ch = getchar();
					ch = 'n';
                }
                else
                {
                    sprintf(buf, TEXT("ERROR: Failed to get current voltage monitor %u value.\n"), ulUnit);
					report(buf);
                    sprintf(buf, "Unit number: %u", ulUnit);
                    Log("CgosVoltageGetCurrent", "FAILED", buf);
                    ch = 'n';
                }
            }while(ch == 'y');
            report(TEXT("\nPress [ENTER] to continue with next monitor.\n"));
            getchar();
        }
    }

    report(TEXT("\n"));
    ulMonCount = CgosFanCount(hCgos);
    Log("CgosFanCount", "SUCCESS", "");
	sprintf(buf, TEXT("Number of fan monitors: %u\n"), ulMonCount); report(buf);    
    if(ulMonCount != 0)
    {
        for(ulUnit = 0; ulUnit < ulMonCount; ulUnit++)
        {
            if(CgosFanGetInfo(hCgos, ulUnit, &fanInfo))
            {
                sprintf(buf,
					TEXT("\nFan monitor %u information:\n")
					TEXT("Type:           %Xh\n")
					TEXT("Flags:          %Xh\n")
					TEXT("Alarm:          %Xh\n")
					TEXT("Nominal value:  %d\n")
					TEXT("Max. Value:     %d\n")
					TEXT("Min. Value:     %d\n")
					TEXT("AlarmHigh:      %d\n")
					TEXT("AlarmLow:       %d\n")
					TEXT("HystHigh:       %d\n")
					TEXT("HystLow:        %d\n"),
					ulUnit,
					fanInfo.dwType,
					fanInfo.dwFlags,
					fanInfo.dwAlarm,
					(int) fanInfo.dwSpeedNom,
					(int) fanInfo.dwMax,
					(int) fanInfo.dwMin,
					(int) fanInfo.dwAlarmHi,
					(int) fanInfo.dwAlarmLo,
					(int) fanInfo.dwHystHi,
					(int) fanInfo.dwHystLo);
				report(buf);

                sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, fanInfo.dwType);
                Log("CgosFanGetInfo", "SUCCESS", buf);
            }
            else
            {
                sprintf(buf, TEXT("ERROR: Failed to get info for fan monitor %u.\n"), ulUnit);
				report(buf);
                sprintf(buf, "Unit number: %u", ulUnit);
                Log("CgosFanGetInfo", "FAILED", buf);
            }

            do
            {
                if(CgosFanGetCurrent(hCgos, ulUnit, &ulSetting, &ulState))
                {
                    sprintf(buf, "Unit number: %u unit type: %08Xh", ulUnit, fanInfo.dwType);
                    Log("CgosFanGetCurrent", "SUCCESS", buf);
                    sprintf(buf,
						TEXT("\nCurrent setting:    %u\n")
						TEXT("Current status:     %u\n"),
						ulSetting,
						ulState);
                    report(buf);
//                    ch = getchar();
					ch = 'n';
                }
                else
                {
                    sprintf(buf, TEXT("ERROR: Failed to get current fan monitor %u value.\n"), ulUnit);
					report(buf);
                    sprintf(buf, "Unit number: %u", ulUnit);
                    Log("CgosFanGetCurrent", "FAILED", buf);
                    ch = 'n';
                }
            }while(ch == 'y');
            report(TEXT("\nPress [ENTER] to continue with next monitor.\n"));
            getchar();
        }
    }
    return;
}

/*---------------------------------------------------------------------------
 * Name: HandleSupported
 * Desc: Generate and save list of supported/unsupported CGOS functions.
 * Inp:  argc   - Number of command line arguments passed
 *       argv[] - Array of pointers to command line parameters
 * Outp: none       
 *---------------------------------------------------------------------------
 */
void HandleSupported(int argc, _TCHAR* argv[])
{
    return;
}

/*---------------------------------------------------------------------------
 * Name: HandleGPIO
 * Desc: Test CGOS GPIO functions.
 * Inp:  argc   - Number of command line arguments passed
 *       argv[] - Array of pointers to command line parameters
 * Outp: none       
 *---------------------------------------------------------------------------
 */
void HandleGPIO(int argc, _TCHAR* argv[])
{
    unsigned int ulGpioUnitCount = 0;
    unsigned int ulUnit;
    unsigned int ulInputPins, ulOutputPins, ulCurrentPinDirection,ulGPIOPinState;
    unsigned short w;
    unsigned char ch ,cUnitName[128];
    unsigned short usGpioHwFail = FALSE;

    report(TEXT("\n****************************************\n"));
    report(TEXT("Testing CGOS GPIO functions.\n"));
    report(TEXT("****************************************\n"));
    report(TEXT("Enter [s] to skip test, any other key to continue.\n"));
    ch = getchar();
    if(ch == 's')
    {
        return;
    }
    
    /* 
    #################
        CgosIOCount
    #################
    Get number of implemented GPIO units. A single GPIO unit can handle up to
    32 GPIs/GPOs/GPIOs
    */
    report(TEXT("\nFunction: CgosIOCount\n"));
    report(TEXT("Get number of available GPIO handler units.\n"));
    report(TEXT("A single GPIO unit can handle up to 32 GPIs/GPOs/GPIOs.\n"));
    ulGpioUnitCount = CgosIOCount(hCgos);
    Log("CgosIOCount", "SUCCESS", "");
    sprintf(buf, TEXT("\nNumber of available GPIO units: %d\n"),ulGpioUnitCount); report(buf);

    report(TEXT("\nPress [ENTER] to continue...\n"));
    getchar();

    for(ulUnit = 0; ulUnit < ulGpioUnitCount; ulUnit++)
    {
        /*
        ######################## 
            CgosIOIsAvailable 
        ########################
        Check whether GPIO unit is available.
        */
        report(TEXT("\nFunction: CgosIOIsAvailable\n"));
        report(TEXT("Check whether GPIO unit with given index can be used.\n"));
        if (!CgosIOIsAvailable(hCgos, ulUnit))
        {
            sprintf(buf, TEXT("\nERROR: Failed to get availability state for GPIO unit %u\n"), ulUnit); report(buf);
            sprintf(buf, "Unit number: %u", ulUnit);
            Log("CgosIOIsAvailable","FAILED",buf);            

        }
        else
        {
		    sprintf(buf, TEXT("\nGPIO unit %d is available.\n"),ulUnit); report(buf);
            sprintf(buf, "Unit number: %u", ulUnit);
            Log("CgosIOIsAvailable","SUCCESS",buf);            

            /*
            ####################
                CgosIOGetName
            ####################
            Get name of GPIO unit (zero terminated string).
            */

            report(TEXT("\nFunction: CgosIOGetName\n"));
            report(TEXT("Return name of GPIO unit.\n"));
            for(w=0;w<128;w++)
            {
                cUnitName[w] = 0x00;
            }
            if(CgosIOGetName(hCgos, ulUnit, ( char * ) &cUnitName[0], 64))
            {
                sprintf(buf, TEXT("\nGPIO unit %d name: %s\n"),ulUnit, &cUnitName[0]); report(buf);
                sprintf(buf, "Unit number: %u name:  %s", ulUnit, &cUnitName[0]);
                Log("CgosIOGetName","SUCCESS",buf);            
            }
            else
            {
                sprintf(buf, TEXT("\nERROR: Failed to get name of GPIO unit %d!\n"),ulUnit); report(buf);
                sprintf(buf, "Unit number: %u", ulUnit);
                Log("CgosIOGetName","FAILED",buf);            
            }
            report(TEXT("\nPress [ENTER] to continue...\n"));
            getchar();

            /*
            ############################
                CgosIOGetDirectionCaps
            ############################
            Get direction capabilities of GPIOs handled by a certain GPIO handler.
            */
            report(TEXT("\nFunction: CgosIOGetDirectionCaps\n"));
            report(TEXT("Return direction capabilities of the pins handled by the selected GPIO unit.\n"));
            report(TEXT("A bit set in the input pin field indicates that this bit can handle a GPI.\n"));
            report(TEXT("A bit set in the output pin field indicates that this bit can handle a GPO.\n"));
            report(TEXT("A bit set in input and output pin field indicates that the corresponding pin's\n"));
            report(TEXT("direction can be changed, i.e. this bit handles a GPIO.\n"));
            report(TEXT("A bit set only in the input pin field handles a hardwired GPI.\n"));
            report(TEXT("A bit set only in the output pin field handles a hardwired GPO.\n"));
            report(TEXT("Bit positions set neither in the input nor the output pin fields have\n"));
            report(TEXT("no corresponding pin at all.\n"));
            if(CgosIOGetDirectionCaps(hCgos, ulUnit, &ulInputPins, &ulOutputPins))
            {
                sprintf(buf, TEXT("\nGPIO unit %d \nInput pins   : %08Xh\nOutput pins  : %08Xh\n"),ulUnit,ulInputPins,ulOutputPins); report(buf);
                sprintf(buf, "Unit number: %u Input pins: %08Xh Output pins: %08Xh", ulUnit, ulInputPins, ulOutputPins);
                Log("CgosIOGetDirectionCaps","SUCCESS",buf);    
            }
            else
            {
                sprintf(buf, TEXT("\nERROR: Failed to get direction capabilities of GPIO unit %d!\n"),ulUnit); report(buf);
                sprintf(buf, "Unit number: %u", ulUnit);
                Log("CgosIOGetDirectionCaps","FAILED",buf);            
            }
            report(TEXT("\nPress [ENTER] to continue...\n"));
            getchar();

            /*
            #########################
                CgosIOGetDirection
            #########################
            Get current direction of the GPIO pins. A bit set to 1 in this field indicates that
            the respective pin is (currently) configured as input, a bit set to 0 indicates that
            the respective pin is (currently) configured as output, or not implemented
            (cross check with result of CgosIOGetDirectionCaps which returns the implemented pins).
            */
            report(TEXT("\nFunction: CgosIOGetDirection\n"));
            report(TEXT("Get current direction of the GPIO pins. A bit set to 1 in this field indicates\n"));
            report(TEXT("that the respective pin is configured as input, a bit set to 0 indicates that\n"));
            report(TEXT("the respective pin is configured as output, or not implemented\n"));
            report(TEXT("(cross check with result of CgosIOGetDirectionCaps).\n"));
            if(CgosIOGetDirection(hCgos, ulUnit, &ulCurrentPinDirection))
            {
                sprintf(buf, TEXT("\nGPIO unit %d \nCurrent pin direction configuration: %08Xh\n"),ulUnit,ulCurrentPinDirection); report(buf);
                sprintf(buf, "Unit number: %u Current pin direction configuration: %08Xh", ulUnit, ulCurrentPinDirection);
                Log("CgosIOGetDirection","SUCCESS",buf);    
            }
            else
            {                
                sprintf(buf, TEXT("\nERROR: Failed to get current pin direction configuration of GPIO unit %d!\n"),ulUnit); report(buf);
                sprintf(buf, "Unit number: %u", ulUnit);
                Log("CgosIOGetDirection","FAILED",buf);            
            }
            report(TEXT("\nPress [ENTER] to continue...\n"));
            getchar();

            /*
            #########################
                CgosIOSetDirection
            #########################
            Changing the pin direction configuration is not supported for the COMExpress 
            GPIO unit as GPI/GPO configuration is fixed by spec./design.
            Therefore the respective function will fail for COMExpress (added for completeness only).
            */
            report(TEXT("\nFunction: CgosIOSetDirection\n"));
            report(TEXT("Try to change current pin direction configuration.\n"));
            report(TEXT("(Fails if pin direction configuration cannot be changed (e.g. COMExpress))\n"));
            if(CgosIOSetDirection(hCgos, ulUnit, 0xFFFFFFFF))   /*Try to configure all pins as input*/
            {
                sprintf(buf, TEXT("\nGPIO unit %d \nPin direction configuration changed\n"),ulUnit); report(buf);
                sprintf(buf, "Unit number: %u", ulUnit);
                Log("CgosIOSetDirection","SUCCESS",buf);    

            }
            else
            {
                sprintf(buf, TEXT("\nERROR: Failed to change current pin direction configuration of GPIO unit %d!\n"),ulUnit); report(buf);
                sprintf(buf, "Unit number: %u", ulUnit);
                Log("CgosIOSetDirection","FAILED",buf);            
            }
            report(TEXT("\nPress [ENTER] to continue...\n"));
            getchar();
           
            /*
            ################
                CgosIORead
            ################
            Read current state of the GPIO pins. A bit set to 1 indicates the respective pin is high,
            a bit set to 0 indicates the current pin is low or not implemented. Again the mask of implemented 
            pins derived from the results of CgosIOGetDirectionCaps has to be considered.
            For pins configured as output, the last state written to this pin is returned
            (assuming that an output pin always can be set to the desired/requested state).
            */
            report(TEXT("\nFunction: CgosIORead\n"));
            report(TEXT("Read current state of the GPIO pins. A bit set to 1 indicates the respective\n"));
            report(TEXT("pin is high. A bit set to 0 indicates the corresponding pin is low or\n"));
            report(TEXT("not implemented. Again the mask of implemented pins derived from the results\n"));
            report(TEXT("of CgosIOGetDirectionCaps has to be considered.\n"));             
            report(TEXT("For pins configured as output, the last state written to this pin is returned,\n"));
            report(TEXT("assuming that an output pin always can be set to the desired/requested state.\n"));
            if(CgosIORead(hCgos, ulUnit, &ulGPIOPinState))
            {
                sprintf(buf, TEXT("\nGPIO unit %d \nCurrent GPIO pin state: %08Xh\n"),ulUnit,ulGPIOPinState); report(buf);
                sprintf(buf, "Unit number: %u Current GPIO pin state: %08Xh", ulUnit, ulGPIOPinState);
                Log("CgosIORead","SUCCESS",buf);    
            }
            else
            {
                sprintf(buf, TEXT("\nERROR: Failed to read pin state of GPIO unit %d!\n"),ulUnit); report(buf);
                sprintf(buf, "Unit number: %u", ulUnit);
                Log("CgosIORead","FAILED",buf);            
            }
            report(TEXT("\nPress [ENTER] to continue...\n"));
            getchar();

            /*
            #################
                CgosIOWrite
            #################
            Set state of GPIO pins. Of course only the state of pins configured as outputs can be changed.
            However, the function will gracefully ignore values set for input pins or pins that are not
            even implemented.
            */
            report(TEXT("\nFunction: CgosIOWrite\n"));
            report(TEXT("Set state of GPIO pins.\n"));
            report(TEXT("Of course only the state of pins configured as outputs can be changed.\n"));            
            report(TEXT("However, the function will gracefully ignore values set for input pins or pins\n"));
            report(TEXT("that are not even implemented.\n"));
            
            report(TEXT("\nSet all output pins to LOW (CgosIOWrite)\n"));
            if(!CgosIOWrite(hCgos, ulUnit, 0x00000000))
            {
                sprintf(buf, TEXT("\nERROR: Failed to set output pins of GPIO unit %d!\n"),ulUnit); report(buf);
                sprintf(buf, "Unit number: %u", ulUnit);
                Log("CgosIOWrite","FAILED",buf);            
            }
            else
            {
                sprintf(buf, "Unit number: %u", ulUnit);
                Log("CgosIOWrite","SUCCESS",buf);         
            }

            report(TEXT("\nPress [ENTER] to continue...\n"));
            getchar();

            report(TEXT("\nSet all output pins to HIGH (CgosIOWrite)\n"));
            if(!CgosIOWrite(hCgos, ulUnit, 0xFFFFFFFF))
            {
                sprintf(buf, TEXT("\nERROR: Failed to set output pins of GPIO unit %d!\n"),ulUnit); report(buf);
                sprintf(buf, "Unit number: %u", ulUnit);
                Log("CgosIOWrite","FAILED",buf);            
            }


            report(TEXT("\nPress [ENTER] for automatic COMExpress GPIO HW test, [m] for manual test.\n"));
            ch = getchar();
            if(ch == 'm')
            {
                Log("GPIO Hardware Test","! NOT TESTED !","");             

                report(TEXT("\nSet all output pins to HIGH (CgosIOWrite)\n"));
                if(!CgosIOWrite(hCgos, ulUnit, 0xFFFFFFFF))
                {
                    sprintf(buf, TEXT("\nERROR: Failed to set output pins of GPIO unit %d!\n"),ulUnit); report(buf);
                }
                report(TEXT("Read current pin state (CgosIORead)\n"));
                if(CgosIORead(hCgos, ulUnit, &ulGPIOPinState))
                {
                    sprintf(buf, TEXT("Current GPIO pin state: %08Xh\n"),ulGPIOPinState); report(buf);
                }
                else
                {
                    sprintf(buf, TEXT("\nERROR: Failed to read pin state of GPIO unit %d!\n"),ulUnit); report(buf);
                }
                report(TEXT("\nPress [ENTER] to continue...\n"));
                getchar();

                report(TEXT("\nSet all output pins to LOW (CgosIOWrite)\n"));
                if(!CgosIOWrite(hCgos, ulUnit, 0x00000000))
                {
                    sprintf(buf, TEXT("\nERROR: Failed to set output pins of GPIO unit %d!\n"),ulUnit); report(buf);
                }
                report(TEXT("Read current pin state (CgosIORead)\n"));
                if(CgosIORead(hCgos, ulUnit, &ulGPIOPinState))
                {
                    sprintf(buf, TEXT("Current GPIO pin state: %08Xh\n"),ulGPIOPinState); report(buf);
                }
                else
                {
                    sprintf(buf, TEXT("\nERROR: Failed to read pin state of GPIO unit %d!\n"),ulUnit); report(buf);
                }

            }
            else
            {
                usGpioHwFail = TRUE;
                CgosIOWrite(hCgos, ulUnit, 0x00000000);
                CgosIORead(hCgos, ulUnit, &ulGPIOPinState);
                if((ulGPIOPinState & 0x000000FF) == 0x00000000)
                {
                    CgosIOWrite(hCgos, ulUnit, 0x00000010);
                    CgosIORead(hCgos, ulUnit, &ulGPIOPinState);
                    if((ulGPIOPinState & 0x000000FF)== 0x00000011)
                    {
                        CgosIOWrite(hCgos, ulUnit, 0x00000020);
                        CgosIORead(hCgos, ulUnit, &ulGPIOPinState);
                        if((ulGPIOPinState & 0x000000FF) == 0x00000022)
                        {
                            CgosIOWrite(hCgos, ulUnit, 0x00000040);
                            CgosIORead(hCgos, ulUnit, &ulGPIOPinState);
                            if((ulGPIOPinState & 0x000000FF)== 0x00000044)
                            {
                                CgosIOWrite(hCgos, ulUnit, 0x00000080);
                                CgosIORead(hCgos, ulUnit, &ulGPIOPinState);
                                if((ulGPIOPinState & 0x000000FF)== 0x00000088)
                                {
                                    CgosIOWrite(hCgos, ulUnit, 0x000000F0);
                                    CgosIORead(hCgos, ulUnit, &ulGPIOPinState);
                                    if((ulGPIOPinState & 0x000000FF)== 0x000000FF)
                                    {
                                        usGpioHwFail = FALSE;
                                        report(TEXT("\nAutomatic COMExpress GPIO hardware test successful!\n"));
                                        Log("GPIO Hardware Test","SUCCESS","");
                                    }
                                }
                            }
                        }                                              
                    }
                }                                                                                             
            }   
            if(usGpioHwFail == TRUE)
            {
                report(TEXT("\nERROR: Automatic COMExpress GPIO hardware test failed!\n"));
                Log("GPIO Hardware Test","FAILED","");                
            }

            /*
            #####################
                CgosIOXorAndXor
            #####################
            This function is reserved for future usage. Meant to easy read/write/modify accesses.
            */
            sprintf(buf, "Unit number: %u", ulUnit);
            Log("CgosIOXorAndXor","! NOT TESTED !",buf);            
        }
    }
    return;
}

/*---------------------------------------------------------------------------
 * Name: HandleTestAll
 * Desc: Test all CGOS functions.
 * Inp:  argc   - Number of command line arguments passed
 *       argv[] - Array of pointers to command line parameters
 * Outp: none       
 *---------------------------------------------------------------------------
 */
void HandleTestAll(int argc, _TCHAR* argv[])
{
    HandleGeneric(argc, argv);
    report(TEXT("\nPress [ENTER] to continue...\n"));
    getchar();
    HandleVga(argc, argv);
    report(TEXT("\nPress [ENTER] to continue...\n"));
    getchar();
    HandleStorage(argc, argv);
    report(TEXT("\nPress [ENTER] to continue...\n"));
    getchar();
    HandleI2C(argc, argv);
    report(TEXT("\nPress [ENTER] to continue...\n"));
    getchar();
    HandleMonitor(argc, argv);
    report(TEXT("\nPress [ENTER] to continue...\n"));
    getchar();
    HandleGPIO(argc, argv);
    report(TEXT("\nPress [ENTER] to continue...\n"));
    getchar();
    HandleWD(argc, argv);

    return;
}




/*---------------------------------------------------------------------------
 * Name: cgostest
 * Desc: cgostest entry and main dispatch routine.
 * Inp:  argc   - number of command line parameters passed
 *       argv[] - array of pointers to command line parameter strings
 * Outp: exit code (0=success ; 1=error)
 *---------------------------------------------------------------------------
 */
int main(int argc, _TCHAR* argv[])
//int cgostest(int argc, _TCHAR* argv[])
{
	  PTSTR pOption;

    unsigned short i=1;
	int len;
    unsigned char bAppendMode = FALSE;
    char ch;

#if defined (UNDER_CE)
    _TCHAR ws[] = L" \t";
    _TCHAR tmpBuf[64];

#else
	char ws[] = " ";
	char lpCmdLine[512] ="";
	char tmpBuf[64];

	//memset(lpCmdLine,0,sizeof(lpCmdLine));
	while (--argc)
	{
		strcat(lpCmdLine,argv[i]);
		strcat(lpCmdLine," ");
		i++;
	}
#endif

    /* Display signon message. */
    ShowSignon();

  	pOption = _tcstok(lpCmdLine, ws);

    if (!pOption)
    {
		ShowUsage();
		exit(-1);
    }

	while (pOption)
	  {
     /* Parse through module list and check whether the respective module has been selected. */
     for(i=0; i < (sizeof moduleList / sizeof moduleList[0]); i++)
     {
		len = (int) strlen(moduleList[i].modSelector);	
     	a2w(tmpBuf,moduleList[i].modSelector,len);	
        if (STRNCMP(pOption, tmpBuf,len) == 0)
        {
            if (moduleList[i].fpModEntry != NULL)
            {
                if(LogOpen(&szLogFile[0], "rt")!= FALSE)
                {
                    report(TEXT("\nCGOSTEST log file already exists!"));              
                    report(TEXT("\nIf you want to append to the existing file please enter [a]."));
                    report(TEXT("\nOtherwise the existing log file will be overwritten.\n"));
                    ch = getchar();
                    if(ch == 'a')
                    {
                        bAppendMode = TRUE;
                    }
                    LogClose();
                    
                    Sleep(100);
                }
                if(bAppendMode == TRUE)
                {
                    /* Append to existing file */
                    if (LogOpen(&szLogFile[0], "a+t")== FALSE)
                    {
                        report(TEXT("\nERROR: Failed to append to log file!\n"));
                        break;
                    }
                }
                else
                {
                    /* Create new log file */
                    if (LogOpen(&szLogFile[0], "wt")== FALSE)
                    {
                        report(TEXT("\nERROR: Failed to create log file!\n"));
                        break;
                    }
                }
                /* Generate info header */
                report(TEXT("\nGenerating log file header ...\n"));
                GenerateInfo();                    
                report(TEXT("Done!\n"));

                /* Start test module(s) */
                if(CgosOpen(TRUE))
                {
                    (*moduleList[i].fpModEntry) (0, NULL);   
                    CgosClose(TRUE);                    
                }
                LogClose();
            }
			else
			{
//				report(TEXT("found flag."));
				flagByte |= moduleList[i].optFlag;
			}

            break;
        }
        if(i == (sizeof moduleList / sizeof moduleList[0]) -1)
        {
            /* Reached end of module list -> error. */
            report(TEXT("\nERROR: You have to select a test!\n"));
            ShowUsage();
        }
	   }

		pOption = _tcstok(NULL, ws);
	  }

    exit(0);
}


