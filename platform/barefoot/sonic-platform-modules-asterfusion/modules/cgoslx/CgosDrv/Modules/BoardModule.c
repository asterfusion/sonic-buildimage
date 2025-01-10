/*---------------------------------------------------------------------------
 *
 * Copyright (c) 2022, congatec GmbH. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * The full text of the license may also be found at:        
 * http://opensource.org/licenses/GPL-2.0
 *
 *---------------------------------------------------------------------------
 */ 

#ifdef __linux__
#include <linux/printk.h>
#include <linux/dmi.h>
#endif
 
#include "CgosDrv.h"
#ifdef _WIN64
#define DPFLTR_INFO_LEVEL 3
#define DPFLTR_IHVDRIVER_ID 77

extern NTSYSAPI
ULONG
__cdecl
DbgPrintEx(
	_In_ ULONG ComponentId,
	_In_ ULONG Level,
	_In_z_ _Printf_format_string_ PCSTR Format,
	...
);
#endif
#include "BCModule.h"
#include "I2CSubModules/I2CSubModule_BC.h"
#include "BoardModule_cfg.h"
#include "CGBC.h"

//***************************************************************************

//#define dbgbrd_on

#ifdef dbgbrd_on
#ifdef __linux__
#define dbgbrd(x) printk(x)
#elif _WIN64
#define dbgbrd(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#elif _WIN32
#define dbgbrd(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#endif
#else
#define dbgbrd(x)
#endif

//***************************************************************************

unsigned char convertBCD(unsigned char in)
{
	unsigned char out = 0;

	switch (in)
	{
	case 0x00:	out = 0x30;
		break;
	case 0x01:	out = 0x31;
		break;
	case 0x02:	out = 0x32;
		break;
	case 0x03:	out = 0x33;
		break;
	case 0x04:	out = 0x34;
		break;
	case 0x05:	out = 0x35;
		break;
	case 0x06:	out = 0x36;
		break;
	case 0x07:	out = 0x37;
		break;
	case 0x08:	out = 0x38;
		break;
	case 0x09:	out = 0x39;
		break;
	default: out = 0;
		break;
	}
	return out;
}

unsigned char convertDay(unsigned char hex)
{
	unsigned char ret = 0;
	ret += (hex & 0x0F);
	ret += (10 * (hex >> 4));
	return ret;
}

unsigned char convertMonth(unsigned char hex)
{
	unsigned char ret = 0;
	ret += (hex & 0x0F);
	ret += (10* (hex >> 4));
	return ret;
}

unsigned int convertYear(unsigned char hex)
{
	unsigned int ret = 2000;
	if(hex == 0)
	{
		return 0;
	}
	ret += (hex & 0x0F);
	ret += (10* (hex >> 4));
	return ret;
}

/***********************************************************************
 * unsigned int getBoardInfoFromBC(CGOS_DRV_VARS *cdv) 				    
 * 																		
 ***********************************************************************
 * Description: This function is called to get the board information
 * 				from the Secure Data Area EEPROM via the board controller.				
 * 																		
 * Last Change: 18.03.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int getBoardInfoFromBC(CGOS_DRV_VARS *cdv)
{
#ifdef __linux__
	unsigned int status = 0;
	const char *value;
	char Board[8] = {0};
#endif
	unsigned int i,j;
	unsigned char bcInfoBuffer[64];


	//Get information from the Board Controller and store it in bcInfoBuffer
	for (i = 0; i < 64; i++)
	{
		CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL, CG_SDA_EEP_I2C_ADDR, i, &bcInfoBuffer[i]);
	}
	
	//Copy relevant information from bcInfoBuffer to cdv

	//Last Repair
	cdv->brd->info.dwRepairCounter = bcInfoBuffer[45];
	
	cdv->brd->info.stLastRepairDate.wDay   = convertDay(bcInfoBuffer[37]);
	cdv->brd->info.stLastRepairDate.wMonth = convertMonth(bcInfoBuffer[38]);
	cdv->brd->info.stLastRepairDate.wYear  = convertYear(bcInfoBuffer[39]);

	//Manufacturing Date
	cdv->brd->info.stManufacturingDate.wDay   = convertDay(bcInfoBuffer[34]);
	cdv->brd->info.stManufacturingDate.wMonth = convertMonth(bcInfoBuffer[35]);
	cdv->brd->info.stManufacturingDate.wYear  = convertYear(bcInfoBuffer[36]);
	
	//EAN
	cdv->brd->info.szEAN[0]  = convertBCD((bcInfoBuffer[25] >> 4) & 0x0F);
	cdv->brd->info.szEAN[1]  = convertBCD( bcInfoBuffer[25] & 0x0F);
	cdv->brd->info.szEAN[2]  = convertBCD((bcInfoBuffer[26] >> 4) & 0x0F);
	cdv->brd->info.szEAN[3]  = convertBCD( bcInfoBuffer[26] & 0x0F);
	cdv->brd->info.szEAN[4]  = convertBCD((bcInfoBuffer[27] >> 4) & 0x0F);
	cdv->brd->info.szEAN[5]  = convertBCD( bcInfoBuffer[27] & 0x0F);
	cdv->brd->info.szEAN[6]  = convertBCD((bcInfoBuffer[28] >> 4) & 0x0F);
	cdv->brd->info.szEAN[7]  = convertBCD( bcInfoBuffer[28] & 0x0F);
	cdv->brd->info.szEAN[8]  = convertBCD((bcInfoBuffer[29] >> 4) & 0x0F);
	cdv->brd->info.szEAN[9]  = convertBCD( bcInfoBuffer[29] & 0x0F);
	cdv->brd->info.szEAN[10] = convertBCD((bcInfoBuffer[30] >> 4) & 0x0F);
	cdv->brd->info.szEAN[11] = convertBCD( bcInfoBuffer[30] & 0x0F);
	cdv->brd->info.szEAN[12] = convertBCD((bcInfoBuffer[31] >> 4) & 0x0F);
	cdv->brd->info.szEAN[13] = convertBCD( bcInfoBuffer[31] & 0x0F);
	
	//Part Number
	j = 0;
	for(i = 9 ; i < 25 ; i++)
	{
		if((bcInfoBuffer[i] >= 0x30) && (bcInfoBuffer[i] <= 0x39))
		{
			cdv->brd->info.szPartNumber[j] = bcInfoBuffer[i];
			j++;
		}
	}
	
	//Serial Number
	cdv->brd->info.szSerialNumber[0]  = convertBCD((bcInfoBuffer[3] >> 4) & 0x0F);
	cdv->brd->info.szSerialNumber[1]  = convertBCD( bcInfoBuffer[3] & 0x0F);
	cdv->brd->info.szSerialNumber[2]  = convertBCD((bcInfoBuffer[4] >> 4) & 0x0F);
	cdv->brd->info.szSerialNumber[3]  = convertBCD( bcInfoBuffer[4] & 0x0F);
	cdv->brd->info.szSerialNumber[4]  = convertBCD((bcInfoBuffer[5] >> 4) & 0x0F);
	cdv->brd->info.szSerialNumber[5]  = convertBCD( bcInfoBuffer[5] & 0x0F);
	cdv->brd->info.szSerialNumber[6]  = convertBCD((bcInfoBuffer[6] >> 4) & 0x0F);
	cdv->brd->info.szSerialNumber[7]  = convertBCD( bcInfoBuffer[6] & 0x0F);
	cdv->brd->info.szSerialNumber[8]  = convertBCD((bcInfoBuffer[7] >> 4) & 0x0F);
	cdv->brd->info.szSerialNumber[9]  = convertBCD( bcInfoBuffer[7] & 0x0F);
	cdv->brd->info.szSerialNumber[10] = convertBCD((bcInfoBuffer[8] >> 4) & 0x0F);
	cdv->brd->info.szSerialNumber[11] = convertBCD( bcInfoBuffer[8] & 0x0F);

	//Platform Revision					
	cdv->brd->info.wProductRevision = (bcInfoBuffer[32] << 8) | bcInfoBuffer[33];
	
	//Project Code
#ifndef __linux__
	cdv->brd->info.szBoard[0]    = bcInfoBuffer[41];
	cdv->brd->info.szBoard[1]    = bcInfoBuffer[42];
	cdv->brd->info.szBoard[2]    = bcInfoBuffer[43];
	cdv->brd->info.szBoard[3]    = bcInfoBuffer[44];
#else
	for(i = 0; i < 4; i++)
	{
		cdv->brd->info.szBoard[i] = cdv->brd->BoardName[i];
	}
#endif
	cdv->brd->info.szBoardSub[0] = bcInfoBuffer[41];
	cdv->brd->info.szBoardSub[1] = bcInfoBuffer[42];
	cdv->brd->info.szBoardSub[2] = bcInfoBuffer[43];
	cdv->brd->info.szBoardSub[3] = bcInfoBuffer[44];	
		
	//Manufacturer
	cdv->brd->info.dwManufacturer = bcInfoBuffer[40];
	
#ifdef __linux__
	//BIOS Revision
	value = dmi_get_system_info(DMI_BIOS_VERSION);
	for(i = 0; i < 8; i++)
	{
		if((value + i) != NULL)
		{
			Board[i] = *(value + i);
		}
		else
		{
			status = 1;
			break;
		}
	}
	if(!status) //BIOS information available
	{
		cdv->brd->info.wSystemBiosRevision = ((Board[5]&0x0F) << 8) | ((Board[6]&0x0F) << 4) | (Board[7]&0x0F);
	}
#endif
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int initBoardModule(CGOS_DRV_VARS *cdv) 				    
 * 																		
 **********************************************************************
 * Description: This function is called to copy information from		
 * 				BoardModule_cfg.h into the cdv struct.				
 * 																		
 * Last Change: 10.10.2019 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initBoardModule(CGOS_DRV_VARS *cdv)
  {
	int i = 0;
	unsigned int retSuccess = CGOS_SUCCESS;
	dbgbrd("initBoardModule called\n");
	if(BOARD_COUNT <= CGOS_DRV_BOARD_MAX)			//CGOS_DRV_BOARD_MAX defined in DrvVars.h
	{
		//Initialize with values hardcoded in BoardModule_cfg.h, rework if 
		//everything can be filled dynamically.
		cdv->boardCount = BOARD_COUNT;
		for (i = 0; i < BOARD_COUNT; i++)
		{
			OsaMemCpy(&cdv->brd[i].info,&BOARD_INFO[i],sizeof(CGOSBOARDINFO));
		}
	}
	else
	{
		retSuccess = CGOS_ERROR;
	}
	return retSuccess;
  }

/***********************************************************************
 * unsigned int zCgosBoardClose(CGOS_DRV_VARS *cdv) 				    
 * 																		
 **********************************************************************
 * Description: This function is called to close a board.				
 * 																		
 * Last Change: 											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosBoardClose(CGOS_DRV_VARS *cdv)
  {
	dbgbrd("zCgosBoardClose called\n");
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int zCgosBoardCount(CGOS_DRV_VARS *cdv) 				    
 * 																		
 **********************************************************************
 * Description: Returns the number of boards.				
 * 																		
 * Last Change: 											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosBoardCount(CGOS_DRV_VARS *cdv)
  {
	enum { dwClass,dwFlags };
	unsigned int i,cnt=0;
	dbgbrd("zCgosBoardCount called\n");
	if (!cdv->cin->pars[dwClass]) // no preferred class
    cnt=cdv->boardCount; // just return the total number of boards
	else { // filter requested classes
    unsigned int mask=(unsigned int)-!(cdv->cin->pars[dwFlags]&CGOS_BOARD_OPEN_FLAGS_PRIMARYONLY); // flags
    for (i=0; i<cdv->boardCount; i++)
      if ((cdv->boards[i].info.dwPrimaryClass|(cdv->boards[i].info.dwClasses&mask))&cdv->cin->pars[dwClass])
        cnt++;
    }
	cdv->cout->rets[0]=cnt;
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int zCgosBoardOpen(CGOS_DRV_VARS *cdv) 				    
 * 																		
 **********************************************************************
 * Description: Opens a board.				
 * 																		
 * Last Change: 											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosBoardOpen(CGOS_DRV_VARS *cdv)
  { 
	enum { dwClass,dwFlags };
	unsigned int i,cnt=cdv->cin->type; // number within filtered classes
	dbgbrd("zCgosBoardOpen called\n");
	if (!cdv->cin->pars[dwClass]) {  // no preferred class
    if (cnt>=cdv->boardCount) return CGOS_ERROR;
    cdv->cout->rets[0]=cnt+1; // hCgos is the board number + 1
    return CGOS_SUCCESS;
    }
	else { // filter requested classes
    unsigned int mask=(unsigned int)-!(cdv->cin->pars[dwFlags]&CGOS_BOARD_OPEN_FLAGS_PRIMARYONLY);
    for (i=0; i<cdv->boardCount; i++)
      if ((cdv->boards[i].info.dwPrimaryClass|(cdv->boards[i].info.dwClasses&mask))&cdv->cin->pars[dwClass])
        if (!cnt--) 
        {
          cdv->cout->rets[0]=i+1; // hCgos is the board number + 1
          return CGOS_SUCCESS;
        }
    }
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosBoardOpenByName(CGOS_DRV_VARS *cdv) 				    
 * 																		
 **********************************************************************
 * Description: Opens a board.				
 * 																		
 * Last Change: 											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosBoardOpenByNameA(CGOS_DRV_VARS *cdv)
  { 
	unsigned int i;
	dbgbrd("zCgosBoardOpenByNameA called\n");
	if (cdv->lin<4) return CGOS_ERROR; // REVIEW
	for (i=0; i<cdv->boardCount; i++)
    // REVIEW this assumes that board names are always 4 chars, we don't have a strcmp yet!
    if (*(unsigned int *)cdv->boards[i].info.szBoard==*(unsigned int *)cdv->pin) 
    {
      cdv->cout->rets[0]=i+1; // hCgos is the board number + 1
      return CGOS_SUCCESS;
    }
  return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosBoardGetNameA(CGOS_DRV_VARS *cdv) 				    
 * 																		
 **********************************************************************
 * Description: Returns the name of a board.				
 * 																		
 * Last Change: 											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosBoardGetNameA(CGOS_DRV_VARS *cdv)
  {
	dbgbrd("zCgosBoardGetNameA called\n");
	// should be a OsaStrNCpy
	OsaMemCpy(cdv->pout,cdv->brd->info.szBoard,cdv->lout);
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int zCgosBoardGetInfoA(CGOS_DRV_VARS *cdv) 				    
 * 																		
 **********************************************************************
 * Description: Returns the information struct of a board.				
 * 																		
 * Last Change: 											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosBoardGetInfoA(CGOS_DRV_VARS *cdv)
  {
  	getBoardInfoFromBC(cdv);
	dbgbrd("BoardGetInfoA called\n");
	OsaMemCpy(cdv->pout,&cdv->brd->info,cdv->lout);
	cdv->retcnt += sizeof(CGOSBOARDINFOA);
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int zCgosBoardGetBootCounter(CGOS_DRV_VARS *cdv)
 *
 **********************************************************************
 * Description: This function is called to get the boot counter from
 *              the board controller.
 *
 * Last Change: 10.10.2019 HMI
 *
 * Modifications:
 ***********************************************************************/
unsigned int zCgosBoardGetBootCounter(CGOS_DRV_VARS *cdv)
  {
	unsigned int retSuccess = CGOS_SUCCESS;
	unsigned int BootCount = 0;
	unsigned char rbufi2c[3] = { 0 };
	unsigned char wbuf = CGBC_CMD_INFO_1;
	unsigned char featureByte = 0;
	unsigned char rbuf[14];
	unsigned char sts;

	dbgbrd("zCgosBoardGetBootCounter called\n");

	retSuccess = bcCommand(&wbuf, 1, &rbuf[0], 13, &sts);
	if (retSuccess == CGOS_SUCCESS)
	{
		featureByte = rbuf[2];
		if ((featureByte & 0x04) == 0x04) //Boot Counter supported
		{
			retSuccess = CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL, CG_SDA_EEP_I2C_ADDR, 0x3B, &rbufi2c[0]);
			retSuccess |= CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL, CG_SDA_EEP_I2C_ADDR, 0x3C, &rbufi2c[1]);
			retSuccess |= CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL, CG_SDA_EEP_I2C_ADDR, 0x3D, &rbufi2c[2]);
			if (retSuccess == CGOS_SUCCESS)
			{
				BootCount = (rbufi2c[2] << 16) | (rbufi2c[1] << 8) | rbufi2c[0];
				cdv->cout->rets[0] = BootCount;
			}
		}
		else
		{
			retSuccess = CGOS_NOT_IMPLEMENTED;
		}
	}
	return retSuccess;
  }

/***********************************************************************
 * unsigned int zCgosBoardGetRunningTimeMeter(CGOS_DRV_VARS *cdv)
 *
 **********************************************************************
 * Description: This function is called to get the running time meter
 *              from the board controller.
 *
 * Last Change: 10.10.2019 HMI
 *
 * Modifications:
 ***********************************************************************/
unsigned int zCgosBoardGetRunningTimeMeter(CGOS_DRV_VARS *cdv)
  {
	unsigned int retSuccess = CGOS_SUCCESS;
	unsigned int RunningTime = 0;
	unsigned char rbufi2c[2] = { 0 };
	unsigned char wbuf = CGBC_CMD_INFO_1;
	unsigned char featureByte = 0;
	unsigned char rbuf[14];
	unsigned char sts;

	dbgbrd("zCgosBoardGetRunningTimeMeter called\n");	

	retSuccess = bcCommand(&wbuf, 1, &rbuf[0], 13, &sts);
	if (retSuccess == CGOS_SUCCESS)
	{
		featureByte = rbuf[2];
		if ((featureByte & 0x08) == 0x08) //Running Time Meter supported
		{
			retSuccess = CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL, CG_SDA_EEP_I2C_ADDR, 0x3E, &rbufi2c[0]);
			retSuccess |= CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL, CG_SDA_EEP_I2C_ADDR, 0x3F, &rbufi2c[1]);
			if (retSuccess == CGOS_SUCCESS)
			{
				RunningTime = (rbufi2c[1] << 8) | rbufi2c[0];
				cdv->cout->rets[0] = RunningTime;
			}
		}
		else
		{
			retSuccess = CGOS_NOT_IMPLEMENTED;
		}
	}
	return retSuccess;
  }

/***********************************************************************
 * unsigned int zCgosBoardGetOption(CGOS_DRV_VARS *cdv) 			    
 * 																		
 ***********************************************************************
 * Description: Not implemented											
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosBoardGetOption(CGOS_DRV_VARS *cdv)
  {
	dbgbrd("zCgosBoardGetOption called\n");	
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int zCgosBoardSetOption(CGOS_DRV_VARS *cdv) 			    
 * 																		
 ***********************************************************************
 * Description: Not implemented											
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosBoardSetOption(CGOS_DRV_VARS *cdv)
  {
	dbgbrd("zCgosBoardSetOption called\n");
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int zCgosBoardGetBootErrorLog(CGOS_DRV_VARS *cdv) 		    
 * 																		
 ***********************************************************************
 * Description: Not implemented											
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosBoardGetBootErrorLog(CGOS_DRV_VARS *cdv)
  {
	dbgbrd("zCgosBoardGetBootErrorLog called\n");
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int BoardModuleHandler(CGOS_DRV_FCT *df, 					
 * 								   CGOS_DRV_VARS *cdv)					
 * 																		
 ***********************************************************************
 * Description: This function is called by the module dispatcher and 	 
 * 				calls the appropriate module function for the requested 
 * 				function call. This function is a defined entry point	
 * 				to the module and can be used for module specific		
 * 				parameter checking.										
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int BoardModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv)
{
	dbgbrd("BoardModuleHandler called\n");
	return (*df->fct)(cdv);
}

/***********************************************************************
 * void exitBoardModule(CGOS_DRV_VARS *cdv) 						    
 * 																		
 ***********************************************************************
 * Description: This function is called during driver close and should	
 * 				free allocated resources.								
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
void exitBoardModule(CGOS_DRV_VARS *cdv)
{
	dbgbrd("exitBoardModule called\n");
}
