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
#include <linux/module.h> 
#endif

#include "CgosDrv.h"
#include "CGBC.h"
#ifdef _WIN64
#undef UINT8
#undef UINT16
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
#include "../BCModule.h"
#include "WDSubModule_BC_cfg.h"

//***************************************************************************

//#define dbg_wd_sub_bc_on

#ifdef dbg_wd_sub_bc_on
#ifdef __linux__
#define dbgwd(x) printk(x)
#elif _WIN64
#define dbgwd(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#elif _WIN32
#define dbgwd(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#endif
#else
#define dbgwd(x)
#endif

//***************************************************************************
// Function prototypes for register function

unsigned int initWDSubModule_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosWDogGetInfo_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosWDogCount_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosWDogIsAvailable_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosWDogTrigger_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosWDogGetConfigStruct_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosWDogSetConfigStruct_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosWDogSetConfig_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosWDogDisable_BC(CGOS_DRV_VARS *cdv);
unsigned int exitWDSubModule_BC(CGOS_DRV_VARS *cdv);

//***************************************************************************

/***********************************************************************
 * unsigned int RegisterWDSubModule_BC									
 * 																		
 ***********************************************************************
 * Description: This function uses the RegisterSubModule callback to	
 * 			  	register the SubModule functions.						
 * 																		
 * Last Change: 20.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int RegisterWDSubModule_BC(unsigned int (*RegisterSubModule)(	CgosFunctionPointer initSubModule,
																		CgosFunctionPointer GetInfo,
																		CgosFunctionPointer Count,
																		CgosFunctionPointer IsAvailable,
																		CgosFunctionPointer Trigger,
																		CgosFunctionPointer GetTriggerCount,
																		CgosFunctionPointer SetTriggerCount,
																		CgosFunctionPointer GetConfigStruct,
																		CgosFunctionPointer SetConfigStruct,
																		CgosFunctionPointer SetConfig,
																		CgosFunctionPointer Disable,
																		CgosFunctionPointer exitSubModule,
																		unsigned char ModuleNumber),
								    unsigned char Number)
{
	return RegisterSubModule(	initWDSubModule_BC,
								CgosWDogGetInfo_BC,
								CgosWDogCount_BC,
								CgosWDogIsAvailable_BC,
								CgosWDogTrigger_BC,
								NULL,		//Get Trigger Count
								NULL,		//Set Trigger Count
								CgosWDogGetConfigStruct_BC,
								CgosWDogSetConfigStruct_BC,
								CgosWDogSetConfig_BC,
								CgosWDogDisable_BC,
								exitWDSubModule_BC,
								Number);
}

/***********************************************************************
 * unsigned int initWDModule_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description: This function is called to copy information from		
 * 				WDModule_cfg.h into the cdv struct.						
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initWDSubModule_BC(CGOS_DRV_VARS *cdv)
  {
	int i = 0;	
	unsigned int retSuccess = CGOS_SUCCESS;
	dbgwd("initWDSubModule_BC called\n");	
	if(WD_COUNT <= CGOS_DRV_WDOG_MAX)		//CGOS_DRV_WDOG_MAX defined in DrvVars.h
	{
		cdv->brd->wdogCount = WD_COUNT;
	
		for(i = 0; i < WD_COUNT; i++)
		{
			OsaMemCpy(&cdv->wdog[i].config,&WD_CONFIG[i],sizeof(CGOSWDCONFIG));
			OsaMemCpy(&cdv->wdog[i].info,&WD_INFO[i],sizeof(CGOSWDINFO));
		}
	}
	else
	{
		retSuccess = CGOS_ERROR;
	}
	return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosWDogGetInfo_BC(CGOS_DRV_VARS *cdv)			  		
 * 																		
 ***********************************************************************
 * Cgos Function: CgosWDogGetInfo(HCGOS hCgos, 						    
 * 								  unsigned long dwUnit,				    
 * 								  CGOSWDINFO *pInfo)				    
 * Inputs:															    
 *   dwUnit <->		cdv->cin->type					    			    
 * 																	    
 * Outputs:															    
 *   pInfo 	<-> 	cdv->pout										    
 * 																		
 ***********************************************************************
 * Description: Get the info struct of the watchdog.					
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosWDogGetInfo_BC(CGOS_DRV_VARS *cdv)
  {
	dbgwd("CgosWDogGetInfo_BC called\n");
	if(cdv->cin->type < cdv->brd->wdogCount)
	{
		OsaMemCpy(cdv->pout,&cdv->wdog[cdv->cin->type].info,sizeof(CGOSWDINFO));
	}
	else
	{
		return CGOS_ERROR;
	}
	cdv->retcnt += sizeof(CGOSWDINFO);
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosWDogCount_BC(CGOS_DRV_VARS *cdv)				  		
 * 																		
 ***********************************************************************
 * Cgos Function: CgosWDogCount(HCGOS hCgos) 						    
 * 																	    
 * Inputs:															    
 *   -												    			    
 * 																	    
 * Outputs:															    
 *   cdv->cout->rets[0]												    
 * 																		
 ***********************************************************************
 * Description: Get the number of available watchdogs.					
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosWDogCount_BC(CGOS_DRV_VARS *cdv)
  {
	cdv->cout->rets[0] = cdv->brd->wdogCount;						
	dbgwd("CgosWDogCount_BC called\n");
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosWDogIsAvailable_BC(CGOS_DRV_VARS *cdv)				
 * 																		
 ***********************************************************************
 * Cgos Function: CgosWDogIsAvailable(HCGOS hCgos, 					    
 * 									  unsigned int dwUnit)			    
 * Inputs:															    
 *   -												    			    
 * 																	    
 * Outputs:															    
 *   cdv->cout->rets[0]												    
 * 																		
 ***********************************************************************
 * Description: Check if watchdog with number dwUnit is available.		
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosWDogIsAvailable_BC(CGOS_DRV_VARS *cdv)
  {
	dbgwd("CgosWDogIsAvailable_BC called\n");
	cdv->cout->rets[0]=cdv->cin->type<cdv->brd->wdogCount;
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosWDogTrigger_BC(CGOS_DRV_VARS *cdv)			  		
 * 																		
 ***********************************************************************
 * Cgos Function: CgosWDogTrigger(HCGOS hCgos, 						    
 * 								  unsigned long dwUnit)				    
 * Inputs:															    
 *   -												    			    
 * 																	    
 * Outputs:															    
 *   -																    
 * 																		
 ***********************************************************************
 * Description: Triggers the watchdog dwUnit. Only BC watchdog			
 * 				implemented at the moment.								
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosWDogTrigger_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned char command = CGBC_CMD_WD_TRIGGER;
	unsigned char sts = 0;
	dbgwd("CgosWDogTrigger_BC called\n");
	bcCommand(&command, 1, NULL, 0, &sts); 
	return CGOS_SUCCESS;
  }
  
/***********************************************************************
 * unsigned int CgosWDogGetTriggerCount_BC(CGOS_DRV_VARS *cdv)	  		
 * 																		
 ***********************************************************************
 * Cgos Function: CgosWDogGetTriggerCount(HCGOS hCgos, 				    
 * 								  		  unsigned long dwUnit)		    
 *																	    
 ***********************************************************************
 * Description: Not implemented											
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosWDogGetTriggerCount_BC(CGOS_DRV_VARS *cdv)
  {
    dbgwd("CgosWDogGetTriggerCount_BC called\n");
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosWDogSetTriggerCount_BC(CGOS_DRV_VARS *cdv)			
 * 																		
 ***********************************************************************
 * Cgos Function: CgosWDogSetTriggerCount(HCGOS hCgos, 				    
 * 								  		  unsigned long dwUnit)		    	
 * 										  unsigned int cnt   		    
 * 																		
 ***********************************************************************
 * Description: Not implemented											
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosWDogSetTriggerCount_BC(CGOS_DRV_VARS *cdv)
  {
    dbgwd("CgosWDogSetTriggerCount_BC called\n");
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosWDogGetConfigStruct_BC(CGOS_DRV_VARS *cdv)	  		
 * 																		
 ***********************************************************************
 * Cgos Function: CgosWDogGetConfigStruct(HCGOS hCgos, 				    
 * 										  unsigned long dwUnit,		    
 * 										  CGOSWDCONFIG *pConfig)	    
 * Inputs:															    
 *   dwUnit <->		cdv->cin->type					    			    
 * 																	    
 * Outputs:															    
 * pConfig 	<-> 	cdv->pout;						 				    
 * 																		
 ***********************************************************************
 * Description: Get the config struct of watchdog number dwUnit.		
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosWDogGetConfigStruct_BC(CGOS_DRV_VARS *cdv)
  {
	dbgwd("CgosWDogGetConfigStruct_BC called\n");
	if(cdv->cin->type < cdv->brd->wdogCount)
	{
		OsaMemCpy(cdv->pout,&cdv->wdog[cdv->cin->type].config,sizeof(CGOSWDCONFIG));
	}
	else
	{
		return CGOS_ERROR;
	}
	cdv->retcnt += sizeof(CGOSWDCONFIG); 
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosWDogSetConfigStruct_BC(CGOS_DRV_VARS *cdv)	  		
 * 																		
 ***********************************************************************
 * Cgos Function: CgosWDogSetConfigStruct(HCGOS hCgos, 				    
 * 										  unsigned long dwUnit,		    
 * 										  CGOSWDCONFIG *pConfig)	    
 * Inputs:															    
 * pConfig 	<-> 	(CGOSWDCONFIG*)cdv->pin;						    
 * dwUnit	<->		cdv->cin->type					    			    
 * 																	    
 * Outputs:															    
 *   -																    
 * 																		
 ***********************************************************************
 * Description: Overwrite the config struct of watchdog number dwUnit
 * 				and send the new config struct to the board controller.	
 * 				Only the BC watchdog is implemented at the moment.		
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosWDogSetConfigStruct_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned char wbuf[15];
	unsigned char sts = 0;
	unsigned int unit = cdv->cin->type;
	int i = 0;
	if(unit < cdv->brd->wdogCount)
	{
		OsaMemCpy(&cdv->wdog[unit].config,(CGOSWDCONFIG*)cdv->pin,sizeof(CGOSWDCONFIG));  
		dbgwd("CgosWDogSetConfigStruct_BC called\n");	

		wbuf[0]  = CGBC_CMD_WD_INIT;
		wbuf[1]  = cdv->wdog[unit].config.dwOpMode; 	//wd mode

		wbuf[2]  = cdv->wdog[unit].config.dwStageCount;	//wd control byte
		for(i = 0; i < cdv->wdog[unit].config.dwStageCount; i++)
		{
			wbuf[2] |= (cdv->wdog[unit].config.stStages[i].dwEvent << (2*i+2)); 
		}

		if(cdv->wdog[unit].config.dwMode != CGOS_WDOG_MODE_STAGED)
		{
			wbuf[3]  =  cdv->wdog[unit].config.dwTimeout & 0xFF; 			 //stage 1 timeout low byte
			wbuf[4]  = (cdv->wdog[unit].config.dwTimeout & 0xFF00)	>>  8;   //stage 1 timeout middle byte
			wbuf[5]  = (cdv->wdog[unit].config.dwTimeout & 0xFF0000) >> 16;  //stage 1 timeout high byte
		}
		else if(cdv->wdog[unit].config.dwMode == CGOS_WDOG_MODE_STAGED)
		{
			for(i = 0; i < cdv->wdog[unit].config.dwStageCount; i++)
			{
				wbuf[3+i*3]  =  cdv->wdog[unit].config.stStages[i].dwTimeout & 0xFF;			 //stage i+1 timeout low byte
				wbuf[4+i*3]  = (cdv->wdog[unit].config.stStages[i].dwTimeout & 0xFF00) 	>>  8;   //stage i+1 timeout middle byte
				wbuf[5+i*3]  = (cdv->wdog[unit].config.stStages[i].dwTimeout & 0xFF0000) >> 16;	 //stage i+1 timeout high byte
			}
		}
		wbuf[12] =  cdv->wdog[unit].config.dwDelay & 0xFF; 				//wd delay low byte
		wbuf[13] = (cdv->wdog[unit].config.dwDelay & 0xFF00)   >>  8;	//wd delay middle byte
		wbuf[14] = (cdv->wdog[unit].config.dwDelay & 0xFF0000) >> 16; 	//wd delay high byte

		bcCommand(&wbuf[0],15,NULL,0,&sts);
	}
	else
	{
		return CGOS_ERROR;
	}
	return CGOS_SUCCESS;
  }



/***********************************************************************
 * unsigned int CgosWDogSetConfig_BC(CGOS_DRV_VARS *cdv)			  		
 * 																		
 ***********************************************************************
 * Cgos Function: CgosWDogSetConfig(HCGOS hCgos, 					    
 * 									unsigned long dwUnit,			    
 * 									unsigned long timeout,	 		    
 * 									unsigned long delay,			    
 * 									unsigned long mode)				    
 * Inputs:															    
 * timeout 	<-> 	cdv->cin->pars[0];								    
 * delay 	<-> 	cdv->cin->pars[1];								   
 * mode 	<-> 	cdv->cin->pars[2];								    
 * 																	    
 * Outputs:															    
 * 	-																    
 * 																		
 ***********************************************************************
 * Description: Short form of CgosWDogSetConfigStruct, the information	
 * 				isn't stored in the cdv struct and sent to the BC 		
 * 				immediately.											
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosWDogSetConfig_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned char wbuf[15];
	unsigned char sts = 0;
	unsigned int retSuccess = CGOS_SUCCESS;
	
	dbgwd("CgosWDogSetConfig_BC called\n");
	
	wbuf[0]  = CGBC_CMD_WD_INIT;
	wbuf[1]  = cdv->cin->pars[2];
	if(cdv->cin->pars[2] == CGOS_WDOG_MODE_REBOOT_PC)
	{
		wbuf[2]  = 1 | (CGBC_WD_EVENT_RST << 2);
	}
	else if(cdv->cin->pars[2] == CGOS_WDOG_MODE_RESTART_OS)
	{
		wbuf[2]  = 1 | (CGBC_WD_EVENT_BTN << 2);
	}
	else
	{
		retSuccess = CGOS_ERROR;
	}
	
	wbuf[3]  =  cdv->cin->pars[0] & 0xFF; 				//stage 1 timeout low byte
	wbuf[4]  = (cdv->cin->pars[0] & 0xFF00)	  >>  8;	//stage 1 timeout middle byte
	wbuf[5]  = (cdv->cin->pars[0] & 0xFF0000) >> 16; 	//stage 1 timeout high byte
	
	wbuf[12] =  cdv->cin->pars[1] & 0xFF; 				//wd delay low byte
	wbuf[13] = (cdv->cin->pars[1] & 0xFF00)   >>  8;	//wd delay middle byte
	wbuf[14] = (cdv->cin->pars[1] & 0xFF0000) >> 16; 	//wd delay high byte
	
	bcCommand(&wbuf[0],15,NULL,0,&sts);
	
	return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosWDogDisable_BC(CGOS_DRV_VARS *cdv)			  		
 * 																		
 ***********************************************************************
 * Cgos Function: CgosWDogDisable(HCGOS hCgos, 						    
 * 								  unsigned long dwUnit)				   
 * 																	    
 * Inputs:															    
 * 	 -																    
 * Outputs:															    
 *   -																    
 * 																		
 ***********************************************************************
 * Description: Disables the BC watchdog.								
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosWDogDisable_BC(CGOS_DRV_VARS *cdv)
  { 
	unsigned char wbuf[15];
	unsigned char sts = 0;
	
	dbgwd("CgosWDogDisable_BC called\n");
	
	wbuf[0] = CGBC_CMD_WD_INIT;
	wbuf[1] = CGBC_WD_DISABLED;
	
	bcCommand(&wbuf[0],15,NULL,0,&sts);
	
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int exitWDSubModule_BC(CGOS_DRV_VARS *cdv)					
 * 																		
 ***********************************************************************
 * Description: This function exits this SubModule.						
 * 																		
 * Last Change: 20.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int exitWDSubModule_BC(CGOS_DRV_VARS *cdv)
{
	dbgwd("exitWDSubModule_BC called\n");
	return CGOS_SUCCESS;
}
