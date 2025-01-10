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
 
//***************************************************************************
#ifdef __linux__ 
#include <linux/printk.h>
#else
#include <stdio.h>
#endif

#include "CgosDrv.h"
#include "Modules.h"
#include "CgosBld.h"

//***************************************************************************

//#define dbg_on

#ifdef dbg_on
	#define dbg(x) x
#else
	#define dbg(x)
#endif

//***************************************************************************

#define CGOS_DRIVER_MINOR 0

//***************************************************************************

#define INVALID_UNIT ((unsigned int)-1)

//***************************************************************************

typedef struct ModuleList
{
	unsigned char BC;
	unsigned char HWM;
	unsigned char I2C;
	unsigned char IO;
	unsigned char VGA;
	unsigned char Board;
	unsigned char WD;
	unsigned char STO;
} ModuleList;

ModuleList IsInitialized;

/***********************************************************************
 * unsigned int initCDV(CGOS_DRV_VARS *cdv) 						    
 * 																		
 ***********************************************************************
 * Description: This function is called to set up the translated		
 * 				pointers and to copy information from BIOS to the 		
 * 				cdv struct.												
 * 																		
 * Last Change: 08.01.2020 HMI											
 * 																	
 * Modifications:														
 ***********************************************************************/
unsigned int initCDV(CGOS_DRV_VARS *cdv)
  {	
	dbg(printk("initCDV called\n");)

	//Initialize pointers:
	cdv->brd  = &cdv->boards[0];
	cdv->io   = &cdv->brd->io[0];
	cdv->sto  = &cdv->brd->sto[0];
	cdv->vga  = &cdv->brd->vga[0];		
	cdv->wdog = &cdv->brd->wdog[0];
	cdv->hwm  = &cdv->brd->hwm;
	cdv->i2c  = &cdv->brd->i2cTable[0];

	return CGOS_SUCCESS;
  }
//***************************************************************************

unsigned int zCgosSuccess(CGOS_DRV_VARS *cdv)
  {
	dbg(printk("zCgosSuccess called\n");)
	cdv->cout->rets[0]=0;
	return CGOS_SUCCESS;
  }

unsigned int zCgosDrvGetVersion(CGOS_DRV_VARS *cdv)
  {
	dbg(printk("zCgosDrvGetVersion called\n");)
	cdv->cout->rets[0]=(((((unsigned int)CGOS_DRIVER_MAJOR)<<8)|CGOS_DRIVER_MINOR)<<16)|CGOS_BUILD_NUMBER;
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * int Compare(char *comp1, char *comp2) 					    
 * 																		
 ***********************************************************************
 * Description: 					
 * 																		
 * Last Change: 26-Jan-2021 Hoffmann Michael											
 * 																		
 * Modifications: 													
 ***********************************************************************/ 
 int Compare(char *comp1, char *comp2)
 {
 	int i;
 	
 	for(i = 0; i < 4; i++)
 	{
 		if(((comp1 + i) == NULL) || ((comp2 + i) == NULL))
 		{
 			return 0;
 		} 
 		if(*(comp1 + i) != *(comp2 + i))
 		{
 			return 0;
 		}
 	}
 	return 1;
 }

/***********************************************************************
 * unsigned int SetBoardGroup(CGOS_DRV_VARS *cdv)						    
 * 																		
 ***********************************************************************
 * Description: Set the board group to allow dynamic driver 
 *              configuration depending on board.											
 * 																		
 * Last Change: 26-Jan-2021 Hoffmann Michael										
 * 																	
 * Modifications:														
 ***********************************************************************/
unsigned int SetBoardGroup(CGOS_DRV_VARS *cdv)
{
#ifdef __linux__
	char CSAS[4] = {'C','S','A','S'};
	
	if(Compare(&cdv->brd->BoardName[0],&CSAS[0]))
	{
		cdv->brd->BoardGroup = GROUP_CSAS;
	}
	else
	{
		cdv->brd->BoardGroup = GROUP_DEFAULT;
	}
#else
	cdv->brd->BoardGroup = GROUP_DEFAULT;
#endif
	return CGOS_SUCCESS;
}

//***************************************************************************

#define CGOS_DRV_FCT_MAX (sizeof(DrvFcts)/sizeof(*DrvFcts))

//Flags for module dispatcher
#define CDFF_x    0x0000
#define CDFF_     0x0000
#define CDFF_THIS 0x0001	//Function is in CgosDrv.c
#define CDFF_BRD  0x0100	//Dispatch to BoardModule
#define CDFF_VGA  0x0200	//Dispatch to VgaModule
#define CDFF_STO  0x0400	//Dispatch to StoModule
#define CDFF_I2C  0x0800	//Dispatch to I2CModule
#define CDFF_IO   0x1000	//Dispatch to IOModule
#define CDFF_WD   0x2000	//Dispatch to WDModule
#define CDFF_HWM  0x4000	//Dispatch to HWMModule
#define CDFF_BC   0x8000	//Dispatch to BCModule

#define CDFF_MASK 0xFF00

//***************************************************************************

// driver function call dispatch
#define X(name) zCgos##name
#define o(name) NULL
#define Z(name) zCgosSuccess

// the first parameter describes how the call will be dispatched:
//   o: just return not implemented
//   Z: just return success and zero results
//   X: dispatch to the drivers zCgos* function with the base name
// the second parameter is the base name
// the third parameter is a flag for the ModuleDispatcher function

// Do not delete, comment or move functions, order and count are important!!!
// If you don't want them, set the first parameter from "X" to "o".

#define df(oX,name,f0,f1,mi,mo) { xCgos##name,oX(name),mi,mo,CDFF_##f0|CDFF_##f1 }

CGOS_DRV_FCT DrvFcts[]={
  df(X,DrvGetVersion           ,THIS,x  ,0,0),		
  df(X,BoardClose              ,BRD ,x  ,0,0),
  df(X,BoardCount              ,BRD ,x  ,0,0),
  df(X,BoardOpen               ,BRD ,x  ,0,0),
  df(X,BoardOpenByNameA        ,BRD ,x  ,0,0),
  df(X,BoardGetNameA           ,BRD ,x  ,0,CGOS_BOARD_MAX_LEN_ID_STRING),		
  df(X,BoardGetInfoA           ,BRD ,x  ,0,sizeof(CGOSBOARDINFOA)),
  df(X,BoardGetBootCounter     ,BRD ,x  ,0,0),
  df(X,BoardGetRunningTimeMeter,BRD ,x  ,0,0),
  df(o,BoardGetOption          ,BRD ,x  ,0,0),
  df(o,BoardSetOption          ,BRD ,x  ,0,0),		
  df(X,BoardGetBootErrorLog    ,BRD ,x  ,0,0),
  df(X,VgaCount                ,VGA ,x  ,0,0),
  df(X,VgaGetContrast          ,VGA ,x  ,0,0),
  df(X,VgaSetContrast          ,VGA ,x  ,0,0),
  df(X,VgaGetContrastEnable    ,VGA ,x  ,0,0),		
  df(X,VgaSetContrastEnable    ,VGA ,x  ,0,0),
  df(X,VgaGetBacklight         ,VGA ,x  ,0,0),
  df(X,VgaSetBacklight         ,VGA ,x  ,0,0),
  df(X,VgaGetBacklightEnable   ,VGA ,x  ,0,0),
  df(X,VgaSetBacklightEnable   ,VGA ,x  ,0,0),		
  df(X,VgaEndDarkBoot          ,VGA ,x  ,0,0),
  df(X,VgaGetInfo              ,VGA ,x  ,0,sizeof(CGOSVGAINFO)),
  df(X,StorageAreaCount        ,STO ,x  ,0,0), // no STO type checks
  df(X,StorageAreaType         ,STO ,x  ,0,0),
  df(X,StorageAreaSize         ,STO ,x  ,0,0),		
  df(X,StorageAreaBlockSize    ,STO ,x  ,0,0),
  df(X,StorageAreaRead         ,STO ,x  ,0,0),
  df(X,StorageAreaWrite        ,STO ,x  ,0,0),
  df(X,StorageAreaErase        ,STO ,x  ,0,0),
  df(X,StorageAreaEraseStatus  ,STO ,x  ,0,0),		
  df(X,I2CCount                ,I2C ,x  ,0,0),
  df(X,I2CType                 ,I2C ,x  ,0,0),
  df(X,I2CIsAvailable          ,I2C ,x  ,0,0),
  df(X,I2CRead                 ,I2C ,x  ,0,0),
  df(X,I2CWrite                ,I2C ,x  ,0,0),		
  df(X,I2CReadRegister         ,I2C ,x  ,0,0),
  df(X,I2CWriteRegister        ,I2C ,x  ,0,0),
  df(X,I2CWriteReadCombined    ,I2C ,x  ,0,0),
  df(X,IOCount                 ,IO  ,x  ,0,0),
  df(X,IOIsAvailable           ,IO  ,x  ,0,0),		
  df(X,IORead                  ,IO  ,x  ,0,0),
  df(X,IOWrite                 ,IO  ,x  ,0,0),
  df(o,IOXorAndXor             ,IO  ,x  ,0,0),
  df(X,IOGetDirection          ,IO  ,x  ,0,0),
  df(X,IOSetDirection          ,IO  ,x  ,0,0),		
  df(X,IOGetDirectionCaps      ,IO  ,x  ,0,0),
  df(o,IOGetNameA              ,x   ,x  ,0,0),
  df(X,WDogCount               ,WD  ,x  ,0,0),
  df(X,WDogIsAvailable         ,WD  ,x  ,0,0),
  df(X,WDogTrigger             ,WD  ,x  ,0,0),		
  df(o,WDogGetTriggerCount     ,WD  ,x  ,0,0),
  df(o,WDogSetTriggerCount     ,WD  ,x  ,0,0),
  df(X,WDogGetConfigStruct     ,WD  ,x  ,0,sizeof(unsigned int)*4), // first 4 pars of CGOSWDCONFIG are mandatory
  df(X,WDogSetConfigStruct     ,WD  ,x  ,sizeof(unsigned int)*4,0),
  df(X,WDogSetConfig           ,WD  ,x  ,0,0),		
  df(X,WDogDisable             ,WD  ,x  ,0,0),
  df(X,WDogGetInfo             ,WD  ,x  ,0,sizeof(CGOSWDINFO)),
  df(o,PerformanceGetCurrent   ,HWM ,x  ,0,0),
  df(o,PerformanceSetCurrent   ,HWM ,x  ,0,0),
  df(o,PerformanceGetPolicyCaps,HWM ,x  ,0,0),		
  df(o,PerformanceGetPolicy    ,HWM ,x  ,0,0),
  df(o,PerformanceSetPolicy    ,HWM ,x  ,0,0),
  df(X,TemperatureCount        ,HWM ,x  ,0,0),
  df(X,TemperatureGetInfo      ,HWM ,x  ,0,sizeof(CGOSTEMPERATUREINFO)),
  df(X,TemperatureGetCurrent   ,HWM ,x  ,0,0),		
  df(X,TemperatureSetLimits    ,HWM ,x  ,sizeof(CGOSTEMPERATUREINFO),0),
  df(X,FanCount                ,HWM ,x  ,0,0),
  df(X,FanGetInfo              ,HWM ,x  ,0,sizeof(CGOSFANINFO)),
  df(X,FanGetCurrent           ,HWM ,x  ,0,0),
  df(X,FanSetLimits            ,HWM ,x  ,sizeof(CGOSFANINFO),0),		
  df(X,VoltageCount            ,HWM ,x  ,0,0),							
  df(X,VoltageGetInfo          ,HWM ,x  ,0,sizeof(CGOSVOLTAGEINFO)),	
  df(X,VoltageGetCurrent       ,HWM ,x  ,0,0),							
  df(X,VoltageSetLimits        ,HWM ,x  ,sizeof(CGOSVOLTAGEINFO),0),
  df(o,Cgeb                    ,x   ,x  ,0,0),		
  df(o,CgebTransAddr           ,x   ,x  ,0,0),
  df(o,CgebDbgLevel            ,x   ,x  ,0,0),	
  df(X,CgbcGetInfo             ,BC  ,x  ,0,sizeof(CGOSBCINFO)),
  df(X,CgbcSetControl          ,BC  ,x  ,0,0),
  df(X,CgbcReadWrite           ,BC  ,x  ,0,0),		
  df(X,CgbcHandleCommand       ,BC  ,x  ,0,0),
  df(X,StorageAreaLock         ,STO ,x  ,0,0),
  df(X,StorageAreaUnlock       ,STO ,x  ,0,0),
  df(X,StorageAreaIsLocked     ,STO ,x  ,0,0),
  df(X,I2CGetMaxFrequency      ,I2C ,x  ,0,0),		
  df(X,I2CGetFrequency         ,I2C ,x  ,0,0),
  df(X,I2CSetFrequency         ,I2C ,x  ,0,0),		
  };

#undef X
#undef o
#undef Z
#undef df
#undef CALL

//***************************************************************************

/***********************************************************************
 * unsigned int ModuleDispatcher(CGOS_DRV_FCT *df, 						
 * 								 CGOS_DRV_VARS *cdv)					
 * 																		
 ***********************************************************************
 * Description: Dispatches the function call to the module handler		
 * 				function of the appropriate module.						
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int ModuleDispatcher(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv)
{
	unsigned int flag = (df->flags) & CDFF_MASK;
	dbg(printk("ModuleDispatcher called\n");)
	if(df->flags == CDFF_THIS)
	{
		return (*df->fct)(cdv);
	}
	else if(flag == CDFF_BC)
	{
		return BCModuleHandler(df,cdv);
	}
	else if(flag == CDFF_BRD)
	{
		return BoardModuleHandler(df,cdv);
	}
	else if(flag == CDFF_IO)
	{
		return IOModuleHandler(df,cdv);
	}
	else if(flag == CDFF_WD)
	{
		return WDModuleHandler(df,cdv);
	}
	else if(flag == CDFF_I2C)
	{
		return I2CModuleHandler(df,cdv);
	}
	else if(flag == CDFF_STO)
	{
		return StoModuleHandler(df,cdv);
	}
	else if(flag == CDFF_VGA)
	{
		return VgaModuleHandler(df,cdv);
	}
	else if(flag == CDFF_HWM)
	{
		return HWMModuleHandler(df,cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
}

//***************************************************************************

/***********************************************************************
 * unsigned int UlaDeviceIoControl(void *hDriver, 						
 * 								   unsigned int dwIoControlCode,		
 *								   void *pInBuffer, 					
 *								   unsigned int nInBufferSize,			
 *								   void *pOutBuffer, 					
 *								   unsigned int nOutBufferSize,			
 *								   unsigned int *pBytesReturned)		
 * 																		
 ***********************************************************************
 * Description: Driver ioctl. CGOS_DRV_VARS *cdv is set up with input	
 * 				parameters and the ModuleDispatcher(df,cdv) function	
 * 				is called.												
 * 																		
 * Last Change: 05.12.2017 HMI										
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int UlaDeviceIoControl(void *hDriver, unsigned int dwIoControlCode,
    void *pInBuffer, unsigned int nInBufferSize,
    void *pOutBuffer, unsigned int nOutBufferSize,
    unsigned int *pBytesReturned)
  {
	CGOS_DRV_VARS *cdv=hDriver;
	CGOSIOCTLIN *cin;
	CGOSIOCTLOUT *cout;
	CGOS_DRV_FCT *df;
  
	if (pBytesReturned) *pBytesReturned=0;

	// check basic ioctl pars
	if (!cdv || cdv==CGOS_INVALID_HANDLE) return CGOS_INVALID_PARAMETER;
	if (dwIoControlCode!=CGOS_IOCTL) return CGOS_NOT_IMPLEMENTED;
	if (nInBufferSize<sizeof(CGOSIOCTLIN)) return CGOS_INVALID_PARAMETER;
	if (nOutBufferSize<sizeof(CGOSIOCTLOUT)) return CGOS_INVALID_PARAMETER;
	if (!pInBuffer) return CGOS_INVALID_PARAMETER;
	if (!pOutBuffer) return CGOS_INVALID_PARAMETER;

	// set up cgos in and out par structures
	cdv->cin=cin=(CGOSIOCTLIN *)pInBuffer;
	cdv->cout=cout=(CGOSIOCTLOUT *)pOutBuffer;
	cdv->nInBufferSize=nInBufferSize;
	cdv->nOutBufferSize=nOutBufferSize;
	cdv->status=0;

	dbg(printk("UlaDeviceIoControl called, Fkt: %u\n", cin->fct);)																

	df=DrvFcts+cin->fct;
	// REVIEW: this is just a sanity check
	if (df->xfct!=cin->fct) {return CGOS_NOT_IMPLEMENTED;} 
	// check additional parameter requirements
	if (nInBufferSize<sizeof(CGOSIOCTLIN)+df->minin){return CGOS_INVALID_PARAMETER; }
	if (nOutBufferSize<sizeof(CGOSIOCTLOUT)+df->minout) {return CGOS_INVALID_PARAMETER;}
	cdv->retcnt=df->minout;

	// pure data buffers
	cdv->pin=cin+1;
	cdv->pout=cout+1;
	cdv->lin=nInBufferSize-sizeof(CGOSIOCTLIN);
	cdv->lout=nOutBufferSize-sizeof(CGOSIOCTLOUT);

	cdv->unit=cin->type; // defaults to untranslated type
	
	if(df->fct)
	{
		cout->status = ModuleDispatcher(df,cdv);
	}
	else 
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	cdv->retcnt+=sizeof(CGOSIOCTLOUT);
	
	if (pBytesReturned) *pBytesReturned=cdv->retcnt;
	return CGOS_SUCCESS;	
  }

//***************************************************************************

#if (!defined(_MSC_VER) && !defined(__cdecl))
#define __cdecl
#endif


/***********************************************************************
 * void *UlaOpenDriver(unsigned long reserved)							
 * 																		
 ***********************************************************************
 * Description: Opens the driver. CGOS_DRV_VARS *cdv is allocated and 	
 * 				all neccessary init functions are called.				
 * 																		
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
void *UlaOpenDriver(unsigned long reserved)
  {
	CGOS_DRV_VARS *cdv;
	unsigned int retSuccess = CGOS_SUCCESS;
	dbg(printk("UlaOpenDriver called\n");)

	IsInitialized.BC = 0;
	IsInitialized.HWM = 0;
	IsInitialized.I2C = 0;
	IsInitialized.IO = 0;
	IsInitialized.VGA = 0;
	IsInitialized.Board = 0;
	IsInitialized.WD = 0;
	IsInitialized.STO = 0;
	
	cdv=OsaMemAlloc(sizeof(*cdv));
	if (!cdv) 
	{
		retSuccess = CGOS_ERROR;
	}
	else
	{
		OsaMemSet(cdv,0,sizeof(*cdv)); // clears boardCount;
		cdv->osDrvVars=(void *)reserved;

		//Call init functions of all modules
		if(initCDV(cdv) == CGOS_SUCCESS)
		{
			//Module initialization
			dbg(printk("CgosDrv: Start Module initialization\n");)
			if(initBCModule(cdv) == CGOS_SUCCESS)
			{
				dbg(printk("CgosDrv: BC init done\n");)
				IsInitialized.BC = 1;

				//Set the board group so module init function know their configuration
				SetBoardGroup(cdv);

				if(initI2CModule(cdv) == CGOS_SUCCESS)
				{
					dbg(printk("CgosDrv: I2C init done\n");)
					IsInitialized.I2C = 1;
					
					if(initBoardModule(cdv) == CGOS_SUCCESS)
					{
						dbg(printk("CgosDrv: Board init done\n");)
						IsInitialized.Board = 1;
					}
					else
					{
						retSuccess = CGOS_ERROR;
					}
				}
				else
				{
					retSuccess = CGOS_ERROR;
				}
				
				if(initIOModule(cdv) == CGOS_SUCCESS)
				{
					dbg(printk("CgosDrv: IO init done\n");)	
					IsInitialized.IO = 1;
				}
				else
				{
					retSuccess = CGOS_ERROR;
				}
				
				if(initWDModule(cdv) == CGOS_SUCCESS)
				{
					dbg(printk("CgosDrv: WD init done\n");)
					IsInitialized.WD = 1;
				}
				else
				{
					retSuccess = CGOS_ERROR;
				}
				
				if(initStoModule(cdv) == CGOS_SUCCESS)
				{
					dbg(printk("CgosDrv: STO init done\n");)
					IsInitialized.STO = 1;
				}
				else
				{
					retSuccess = CGOS_ERROR;
				}
				
				if(initVgaModule(cdv) == CGOS_SUCCESS)
				{
					dbg(printk("CgosDrv: VGA init done\n");)
					IsInitialized.VGA = 1;
				}
				else
				{
					retSuccess = CGOS_ERROR;
				}
				
				if(initHWMModule(cdv) == CGOS_SUCCESS)
				{
					dbg(printk("CgosDrv: HWM init done\n");)
					IsInitialized.HWM = 1;
				}
				else
				{
					retSuccess = CGOS_ERROR;
				}
								
			}
			else
			{
				retSuccess = CGOS_ERROR;
			}
			
			//Call Functions to get information
			if(IsInitialized.BC && IsInitialized.Board && IsInitialized.I2C)
			{
				retSuccess |= getBoardInfoFromBC(cdv);
			}
		}
		else
		{
			retSuccess = CGOS_ERROR;
		}
		
		if(retSuccess != CGOS_SUCCESS)
		{
			OsaMemFree(cdv);
		}
	}
	return (retSuccess == CGOS_SUCCESS) ? cdv : NULL;  
  }

/***********************************************************************
 * void *UlaCloseDriver(void *hDriver)									
 * 																		
 ***********************************************************************
 * Description: Closes the driver. CGOS_DRV_VARS *cdv is freed and 		
 * 				all neccessary exit functions are called.				
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																		
 * Modifications:														
 * 	- 12.12.2017 HMI: Added exitModule functions						
 ***********************************************************************/
void UlaCloseDriver(void *hDriver)
  {
	CGOS_DRV_VARS *cdv=hDriver;
	dbg(printk("UlaCloseDriver called\n");)
	
	// Module Cleanup
	if(IsInitialized.HWM)
	{
		exitHWMModule(cdv);
	}
	if(IsInitialized.I2C)
	{
		exitI2CModule(cdv);
	}
	if(IsInitialized.IO)
	{
		exitIOModule(cdv);
	}
	if(IsInitialized.STO)
	{
		exitStoModule(cdv);
	}
	if(IsInitialized.VGA)
	{
		exitVgaModule(cdv);
	}
	if(IsInitialized.WD)
	{
		exitWDModule(cdv);
	}
	if(IsInitialized.Board)
	{
		exitBoardModule(cdv);
	}
	if (!cdv) return;
	OsaMemFree(cdv);
  }

unsigned int UlaGetBoardCount(void *hDriver)
  { 
	CGOS_DRV_VARS *cdv=hDriver;
	dbg(printk("UlaGetBoardCount called\n");)	
	return cdv->boardCount;
  }

unsigned char *UlaGetBoardName(void *hDriver, unsigned int Index)
  {  
	CGOS_DRV_VARS *cdv=hDriver;
	dbg(printk("UlaGetBoardName called\n");)	
	if (Index>=cdv->boardCount) return NULL;
	return cdv->boards[Index].info.szBoard;
  }

//***************************************************************************
