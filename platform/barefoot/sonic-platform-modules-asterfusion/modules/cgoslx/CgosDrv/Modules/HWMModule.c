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
#endif

#include "CgosDrv.h"
//#include "CGBC.h"
#include "BCModule.h"
#include "HWMModule_cfg.h"
#include "HWMSubModules/HWMSubModule_BC.h"

//***************************************************************************

//#define dbg_hwm_on

#ifdef dbg_hwm_on
	#define dbghwm(x) x
#else
	#define dbghwm(x)
#endif

//***************************************************************************
//List of required functions for the HWM Module
typedef struct
{
	CgosFunctionPointer initSubModule;
	CgosFunctionPointer TempCount;
	CgosFunctionPointer TempGetInfo;
	CgosFunctionPointer TempGetCurrent;
	CgosFunctionPointer TempSetLimits;
	CgosFunctionPointer FanCount;
	CgosFunctionPointer FanGetInfo;
	CgosFunctionPointer FanGetCurrent;
	CgosFunctionPointer FanSetLimits;
	CgosFunctionPointer VoltCount;
	CgosFunctionPointer VoltGetInfo;
	CgosFunctionPointer VoltGetCurrent;
	CgosFunctionPointer VoltSetLimits;
	CgosFunctionPointer PerfGetCurrent;
	CgosFunctionPointer PerfSetCurrent;
	CgosFunctionPointer PerfGetPolCaps;
	CgosFunctionPointer PerfGetPol;
	CgosFunctionPointer PerfSetPol;
	CgosFunctionPointer exitSubModule;	
	void *__dummy__; /*Required to compile this code with activated gcc constify plugin*/
}HWMModuleFunctionList;

//ListItem for SubModule list
typedef struct HWMListItem
{
	HWMModuleFunctionList Functions;
	unsigned char SubModule;
	struct HWMListItem *NextItem;
}HWMListItem;

//First List Entry
//The first SubModule is considered the "active" one.
HWMListItem *HWMFirstListItem;

//***************************************************************************

/***********************************************************************
 * unsigned int HWMRegisterSubModule(CgosFunctionPointer initSubModule,
 *									 CgosFunctionPointer TempCount,
 *									 CgosFunctionPointer TempGetInfo,
 *									 CgosFunctionPointer TempGetCurrent,
 *									 CgosFunctionPointer TempSetLimits,
 *									 CgosFunctionPointer FanCount,
 *									 CgosFunctionPointer FanGetInfo,
 *									 CgosFunctionPointer FanGetCurrent,
 *									 CgosFunctionPointer FanSetLimits,
 *									 CgosFunctionPointer VoltCount,
 *									 CgosFunctionPointer VoltGetInfo,
 *									 CgosFunctionPointer VoltGetCurrent,
 *									 CgosFunctionPointer VoltSetLimits,
 *									 CgosFunctionPointer PerfGetCurrent,
 *									 CgosFunctionPointer PerfSetCurrent,
 *									 CgosFunctionPointer PerfGetPolCaps,
 *									 CgosFunctionPointer PerfGetPol,
 *									 CgosFunctionPointer PerfSetPol,
 *									 CgosFunctionPointer exitSubModule,
 * 									 unsigned char ModuleNumber)	
 * 																		
 ***********************************************************************
 * Description: This function is used to register a sub module.			
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int HWMRegisterSubModule(CgosFunctionPointer initSubModule,
 								CgosFunctionPointer TempCount,
 								CgosFunctionPointer TempGetInfo,
								CgosFunctionPointer TempGetCurrent,
 								CgosFunctionPointer TempSetLimits,
 								CgosFunctionPointer FanCount,
 								CgosFunctionPointer FanGetInfo,
 								CgosFunctionPointer FanGetCurrent,
 								CgosFunctionPointer FanSetLimits,
 								CgosFunctionPointer VoltCount,
 								CgosFunctionPointer VoltGetInfo,
 								CgosFunctionPointer VoltGetCurrent,
 								CgosFunctionPointer VoltSetLimits,
 								CgosFunctionPointer PerfGetCurrent,
 								CgosFunctionPointer PerfSetCurrent,
								CgosFunctionPointer PerfGetPolCaps,
 								CgosFunctionPointer PerfGetPol,
 								CgosFunctionPointer PerfSetPol,
 								CgosFunctionPointer exitSubModule,
 								unsigned char ModuleNumber)
{
	HWMListItem *DummyPointer;
	dbghwm(printk("HWMRegisterSubModule called\n");)
	
	DummyPointer = OsaMemAlloc(sizeof(HWMListItem));
	if(DummyPointer == NULL)
	{
		return CGOS_ERROR;
	}
	
	DummyPointer->Functions.initSubModule = initSubModule;
	
	DummyPointer->Functions.TempCount = TempCount;
	DummyPointer->Functions.TempGetInfo = TempGetInfo;
	DummyPointer->Functions.TempGetCurrent = TempGetCurrent;
	DummyPointer->Functions.TempSetLimits = TempSetLimits;
	
	DummyPointer->Functions.FanCount = FanCount;
	DummyPointer->Functions.FanGetInfo = FanGetInfo;
	DummyPointer->Functions.FanGetCurrent = FanGetCurrent;
	DummyPointer->Functions.FanSetLimits = FanSetLimits;
	
	DummyPointer->Functions.VoltCount = VoltCount;
	DummyPointer->Functions.VoltGetInfo = VoltGetInfo;
	DummyPointer->Functions.VoltGetCurrent = VoltGetCurrent;
	DummyPointer->Functions.VoltSetLimits = VoltSetLimits;
	
	DummyPointer->Functions.PerfGetCurrent = PerfGetCurrent;
	DummyPointer->Functions.PerfSetCurrent = PerfSetCurrent;
	DummyPointer->Functions.PerfGetPolCaps = PerfGetPolCaps;
	DummyPointer->Functions.PerfGetPol = PerfGetPol;
	DummyPointer->Functions.PerfSetPol = PerfSetPol;
	
	DummyPointer->Functions.exitSubModule = exitSubModule;
	
	DummyPointer->SubModule = ModuleNumber;
	DummyPointer->NextItem = HWMFirstListItem;
	
	HWMFirstListItem = DummyPointer;
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int HWMGetCurrentSubModule(unsigned char *ModuleType)	    
 * 																		
 ***********************************************************************
 * Description: This function is used to get the current SubModule.		
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int HWMGetCurrentSubModule(unsigned char *ModuleType)
{
	dbghwm(printk("HWMGetCurrentSubModule called\n");)
	*ModuleType = HWMFirstListItem->SubModule;
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int HWMSetDefaultSubModule(unsigned char DefaultSubModule)   
 * 																		
 ***********************************************************************
 * Description: This function is used to set a SubModule as default.	
 * 																		
 * Last Change: 07.02.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int HWMSetDefaultSubModule(unsigned char DefaultSubModule)
{
	HWMListItem *Dummy1;
	HWMListItem *Dummy2;
	dbghwm(printk("HWMSetDefaultSubModule called\n");)
	//check if SubModule List is empty
	if(HWMFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(HWMFirstListItem->SubModule == DefaultSubModule)
		{
			return CGOS_SUCCESS;
		}
		else
		{
			Dummy1 = HWMFirstListItem->NextItem;
			Dummy2 = HWMFirstListItem;
			
			//Search SubModule list for requested SubModule
			while(Dummy1 != NULL)
			{
				//If found, change to active
				if(Dummy1->SubModule == DefaultSubModule)
				{
					Dummy2->NextItem = Dummy1->NextItem;
					Dummy1->NextItem = HWMFirstListItem;
					HWMFirstListItem = Dummy1;
					break;
				}
				else
				{
					Dummy2 = Dummy1;
					Dummy1 = Dummy1->NextItem;
				}
			}
			if(Dummy1 == NULL)
			{
				dbghwm(printk("HWMSetDefaultSubModule error: SubModule not found");)
				return CGOS_ERROR;
			}
		}
	}
	else
	{
		dbghwm(printk("HWMSetDefaultSubModule error: No SubModule available");)
		return CGOS_ERROR;
	}
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int HWMChangeSubModule(CGOS_DRV_VARS *cdv,					
 * 								  unsigned char NewSubModule)		    
 * 																		
 ***********************************************************************
 * Description: This function changes the used SubModule				
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int HWMChangeSubModule(CGOS_DRV_VARS *cdv, unsigned char NewSubModule)
{
	HWMListItem *Dummy1;
	HWMListItem *Dummy2;
	unsigned int retSuccess = CGOS_SUCCESS;
	
	dbghwm(printk("HWMChangeSubModule called\n");)
	
	//check if SubModule List is empty
	if(HWMFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(NewSubModule != HWMFirstListItem->SubModule)	
		{
			//Exit the current SubModule
			if(HWMFirstListItem->Functions.exitSubModule != NULL)
			{
			retSuccess = (HWMFirstListItem->Functions.exitSubModule)(cdv);
			}
	
			//Change SubModule
			if(HWMFirstListItem->NextItem != NULL)
			{
				Dummy1 = HWMFirstListItem->NextItem;
				Dummy2 = HWMFirstListItem;
			
				while(Dummy1 != NULL)
				{
					if(Dummy1->SubModule == NewSubModule)
					{
						Dummy2->NextItem = Dummy1->NextItem;
						Dummy1->NextItem = HWMFirstListItem;
						HWMFirstListItem = Dummy1;
						break;
					}
					else
					{
						Dummy2 = Dummy1;
						Dummy1 = Dummy1->NextItem;
					}
				}
				if(Dummy1 == NULL)	//Requested SubModule not found
				{
					retSuccess = CGOS_ERROR;
				}
			}
		
			//Init the new SubModule
			if((HWMFirstListItem->Functions.initSubModule != NULL) && (retSuccess == CGOS_SUCCESS))
			{
			retSuccess = (HWMFirstListItem->Functions.initSubModule)(cdv);
			}
		}
	}
	else
	{
		retSuccess = CGOS_ERROR;
	}
	return retSuccess;
}

/***********************************************************************
 * unsigned int initHWMModule(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description: This function calls all init functions of this module.	
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initHWMModule(CGOS_DRV_VARS *cdv)
  {	
	unsigned int retSuccess = CGOS_SUCCESS;
	dbghwm(printk("initHWMModule called\n");)
	
	//Initialize SubModuele list
	HWMFirstListItem = NULL;
	
	//Register SubModule function pointers via callback.
	//HWM_SUBMODULE_* enum in DrvVars.h.
	RegisterHWMSubModule_BC(HWMRegisterSubModule,HWM_SUBMODULE_BC);

	//Set default SubModule.
	switch(cdv->brd->BoardGroup)
	{
		case GROUP_DEFAULT:
		default: 		HWMSetDefaultSubModule(HWM_SUBMODULE_BC);
	}


	//Initialize active SubModule
	if(HWMFirstListItem->Functions.initSubModule != NULL)
	{
		(HWMFirstListItem->Functions.initSubModule)(cdv);
	}
	
	return retSuccess;
  }

/* **********************************************************************
 * 																	    *
 *						Temperature Functions						    *
 * 																	    *
 ************************************************************************/

/***********************************************************************
 * unsigned int zCgosTemperatureCount(CGOS_DRV_VARS *cdv)			    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosTemperatureCount(HCGOS hCgos) 				    
 * 																	    
 * Inputs:															    
 *   -												    			    
 * 																	    
 * Outputs:															    
 *   cdv->cout->rets[0]												    
 * 																		
 ***********************************************************************
 * Description: Gets temperature sensor	count.							
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosTemperatureCount(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosTemperatureCount called\n");)
	if(HWMFirstListItem->Functions.TempCount != NULL)
	{
		return (HWMFirstListItem->Functions.TempCount)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosTemperatureGetInfo(CGOS_DRV_VARS *cdv)			    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosTemperatureGetInfo(HCGOS hCgos, 				    
 * 								 		 unsigned long dwUnit,			
 * 								  		 CGOSTEMPERATUREINFO *pInfo)	
 * Inputs:															    
 *   dwUnit <->		cdv->cin->type					    			    
 * 																	    
 * Outputs:															    
 *   pInfo 	<-> 	cdv->pout										    
 * 																	
 ***********************************************************************
 * Description: Gets info struct of the requested temperature sensor
 * 																		
 * Last Change: 06.12.2017 HMI										
 * 																	
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosTemperatureGetInfo(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosTemperatureGetInfo called\n");)
	if(HWMFirstListItem->Functions.TempGetInfo != NULL)
	{
		return (HWMFirstListItem->Functions.TempGetInfo)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }
  
/***********************************************************************
 * unsigned int zCgosTemperatureGetCurrent(CGOS_DRV_VARS *cdv)		    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosTemperatureGetCurrent(HCGOS hCgos, 			    
 * 								 	 		unsigned long dwUnit,		
 * 								  	 		unsigned long *pdwSetting,	
 * 											unsigned long *pdwStatus)	
 * Inputs:															    
 *   dwUnit 	<->		cdv->cin->type				    			    
 * 																	    
 * Outputs:															    
 *   pdwSetting 	<-> 	cdv->cout->rets[0]						    
 * 	 pdwStatus		<->		cdv->cout->rets[1]						
 * 																		
 ***********************************************************************
 * Description: Gets status and value/error code of the	requested		
 * 				temperature sensor.										
 * 																		
 * Last Change: 07.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosTemperatureGetCurrent(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosTemperatureGetCurrent called\n");)
	if(HWMFirstListItem->Functions.TempGetCurrent != NULL)
	{
		return (HWMFirstListItem->Functions.TempGetCurrent)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }
  
unsigned int zCgosTemperatureSetLimits(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosTemperatureSetLimits called\n");)	
	if(HWMFirstListItem->Functions.TempSetLimits != NULL)
	{
		return (HWMFirstListItem->Functions.TempSetLimits)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/* **********************************************************************
 * 																	    *
 *							 Fan Functions							    *
 * 																	    *
 ************************************************************************/

/***********************************************************************
 * unsigned int zCgosFanCount(CGOS_DRV_VARS *cdv)					    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosFanCount(HCGOS hCgos) 						    
 * 																	    
 * Inputs:															    
 *   -												    			    
 * 																	    
 * Outputs:															    
 *   cdv->cout->rets[0]												    
 * 																		
 ***********************************************************************
 * Description: Gets fan sensor	count.									
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosFanCount(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosFanCount called\n");)	
	if(HWMFirstListItem->Functions.FanCount != NULL)
	{
		return (HWMFirstListItem->Functions.FanCount)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosFanGetInfo(CGOS_DRV_VARS *cdv)					    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosFanGetInfo(HCGOS hCgos, 						    
 * 								 unsigned long dwUnit,				    
 * 								 CGOSFANINFO *pInfo)				    
 * Inputs:															    
 *   dwUnit <->		cdv->cin->type					    			    
 * 																	    
 * Outputs:															    
 *   pInfo 	<-> 	cdv->pout										    
 * 																		
 ***********************************************************************
 * Description: Gets info struct of the requested fan sensor			
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosFanGetInfo(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosFanGetInfo called\n");)	
	if(HWMFirstListItem->Functions.FanGetInfo != NULL)
	{
		return (HWMFirstListItem->Functions.FanGetInfo)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosFanGetCurrent(CGOS_DRV_VARS *cdv)				    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosFanGetCurrent(HCGOS hCgos, 			 			
 * 								 	unsigned long dwUnit,				
 * 								  	unsigned long *pdwSetting,			
 * 									unsigned long *pdwStatus)			
 * Inputs:															    
 *   dwUnit 	<->		cdv->cin->type				    			    
 * 																	    
 * Outputs:															    
 *   pdwSetting 	<-> 	cdv->cout->rets[0]						    
 * 	 pdwStatus		<->		cdv->cout->rets[1]							
 * 																		
 ***********************************************************************
 * Description: Gets status and value/error code of the					
 * 				requested fan sensor									
 * 																		
 * Last Change: 07.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosFanGetCurrent(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosFanGetCurrent called\n");)
	if(HWMFirstListItem->Functions.FanGetCurrent != NULL)
	{
		return (HWMFirstListItem->Functions.FanGetCurrent)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }


unsigned int zCgosFanSetLimits(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosFanSetLimits called\n");)	
	if(HWMFirstListItem->Functions.FanSetLimits != NULL)
	{
		return (HWMFirstListItem->Functions.FanSetLimits)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }
  
/* **********************************************************************
 * 																	    *
 *						Voltage Functions							    *
 * 																	    *
 ************************************************************************/  

/***********************************************************************
 * unsigned int zCgosVoltageCount(CGOS_DRV_VARS *cdv)				    
 * 																	
 ***********************************************************************
 * Cgos Function: CgosVoltageCount(HCGOS hCgos) 					   
 * 																	    
 * Inputs:															    
 *   -												    			    
 * 																	    
 * Outputs:															    
 *   cdv->cout->rets[0]												    
 * 																	
 ***********************************************************************
 * Description: Gets voltage sensor	count.								
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosVoltageCount(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosVoltageCount called\n");)		
	if(HWMFirstListItem->Functions.VoltCount != NULL)
	{
		return (HWMFirstListItem->Functions.VoltCount)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosVoltageGetInfo(CGOS_DRV_VARS *cdv)				    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosVoltageGetInfo(HCGOS hCgos, 					    
 * 								 	 unsigned long dwUnit,			    
 * 								  	 CGOSVOLTAGEINFO *pInfo)		    
 * Inputs:															    
 *   dwUnit <->		cdv->cin->type					    			    
 * 																	    
 * Outputs:															    
 *   pInfo 	<-> 	cdv->pout										    
 * 																		
 ***********************************************************************
 * Description: Gets info struct of the requested voltage sensor		
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosVoltageGetInfo(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosVoltageGetInfo called\n");)	
	if(HWMFirstListItem->Functions.VoltGetInfo != NULL)
	{
		return (HWMFirstListItem->Functions.VoltGetInfo)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosVoltageGetCurrent(CGOS_DRV_VARS *cdv)			    
 * 																	
 ***********************************************************************
 * Cgos Function: CgosVoltageGetCurrent(HCGOS hCgos, 			   		
 * 								 	 	unsigned long dwUnit,			
 * 								  	 	unsigned long *pdwSetting,		
 * 										unsigned long *pdwStatus)		
 * Inputs:															    
 *   dwUnit		 <->		cdv->cin->type			    			    
 * 																	    
 * Outputs:															    
 *   pdwSetting 	<-> 	cdv->cout->rets[0]						   
 * 	 pdwStatus		<->		cdv->cout->rets[1]							
 * 																		
 ***********************************************************************
 * Description: Gets status and value/error code of the					
 * 				requested voltage sensor								
 * 																		
 * Last Change: 07.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosVoltageGetCurrent(CGOS_DRV_VARS *cdv)					
  {
	dbghwm(printk("zCgosVoltageGetCurrent called\n");)	
	if(HWMFirstListItem->Functions.VoltGetCurrent != NULL)
	{
		return (HWMFirstListItem->Functions.VoltGetCurrent)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

unsigned int zCgosVoltageSetLimits(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosVoltageSetLimits called\n");)	
	if(HWMFirstListItem->Functions.VoltSetLimits != NULL)
	{
		return (HWMFirstListItem->Functions.VoltSetLimits)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }
  
/* **********************************************************************
 * 																	    *
 *						Performance Functions						    *
 * 																	    *
 ************************************************************************/
 
/***********************************************************************
 * unsigned int zCgosPerformanceGetCurrent(CGOS_DRV_VARS *cdv) 		    
 * 																		
 ***********************************************************************
 * Description: Not Implemented											
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosPerformanceGetCurrent(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosPerformanceGetCurrent called\n");)
	if(HWMFirstListItem->Functions.PerfGetCurrent != NULL)
	{
		return (HWMFirstListItem->Functions.PerfGetCurrent)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosPerformanceSetCurrent(CGOS_DRV_VARS *cdv) 		    
 * 																		
 ***********************************************************************
 * Description: Not Implemented											
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:													
 ***********************************************************************/
unsigned int zCgosPerformanceSetCurrent(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosPerformanceSetCurrent called\n");)
	if(HWMFirstListItem->Functions.PerfSetCurrent != NULL)
	{
		return (HWMFirstListItem->Functions.PerfSetCurrent)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosPerformanceGetPolicyCaps(CGOS_DRV_VARS *cdv) 	    
 * 																		
 ***********************************************************************
 * Description: Not Implemented											
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosPerformanceGetPolicyCaps(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosPerformanceGetPolicyCaps called\n");)	
	if(HWMFirstListItem->Functions.PerfGetPolCaps != NULL)
	{
		return (HWMFirstListItem->Functions.PerfGetPolCaps)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosPerformanceGetPolicy(CGOS_DRV_VARS *cdv) 		    
 * 																		
 ***********************************************************************
 * Description: Not Implemented											
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosPerformanceGetPolicy(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosPerformanceGetPolicy called\n");)	
	if(HWMFirstListItem->Functions.PerfGetPol != NULL)
	{
		return (HWMFirstListItem->Functions.PerfGetPol)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosPerformanceSetPolicy(CGOS_DRV_VARS *cdv) 		    
 * 																		
 ***********************************************************************
 * Description: Not Implemented											
 * 																	
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosPerformanceSetPolicy(CGOS_DRV_VARS *cdv)
  {
	dbghwm(printk("zCgosPerformanceSetPolicy called\n");)	
	if(HWMFirstListItem->Functions.PerfSetPol != NULL)
	{
		return (HWMFirstListItem->Functions.PerfSetPol)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int HWMModuleHandler(CGOS_DRV_FCT *df, 						
 * 								 CGOS_DRV_VARS *cdv)					
 * 																		
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
unsigned int HWMModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv)
{
	dbghwm(printk("HWMModuleHandler called\n");)
	return (*df->fct)(cdv);
}

/***********************************************************************
 * void exitHWMModule(CGOS_DRV_VARS *cdv) 							    
 * 																		
 ***********************************************************************
 * Description: This function is called during driver close and should	
 * 				free allocated resources.								
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
void exitHWMModule(CGOS_DRV_VARS *cdv)
{
	
	HWMListItem *Dummy;
	dbghwm(printk("exitHWMModule called\n");)
	
	//Call exit function of active SubModule.
	if(HWMFirstListItem->Functions.exitSubModule != NULL)
	{
		(HWMFirstListItem->Functions.exitSubModule)(cdv);
	}

	//Free SubModule list
	while(HWMFirstListItem != NULL)
	{
			Dummy = HWMFirstListItem;
			HWMFirstListItem = HWMFirstListItem->NextItem;
			OsaMemFree(Dummy);
	}
}
