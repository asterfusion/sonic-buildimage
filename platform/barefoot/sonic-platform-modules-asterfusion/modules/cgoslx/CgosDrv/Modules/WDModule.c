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
//#include "CGBC.h"
#include "BCModule.h"
#include "WDModule_cfg.h"
#include "WDSubModules/WDSubModule_BC.h"

//***************************************************************************

//#define dbg_wd_on

#ifdef dbg_wd_on
	#define dbgwd(x) x
#else
	#define dbgwd(x)
#endif

//***************************************************************************
//List of required functions for the WD Module
typedef struct
{
	CgosFunctionPointer initSubModule;
	CgosFunctionPointer GetInfo;
	CgosFunctionPointer Count;
	CgosFunctionPointer IsAvailable;
	CgosFunctionPointer Trigger;
	CgosFunctionPointer GetTriggerCount;
	CgosFunctionPointer SetTriggerCount;
	CgosFunctionPointer GetConfigStruct;
	CgosFunctionPointer SetConfigStruct;
	CgosFunctionPointer SetConfig;
	CgosFunctionPointer Disable;
	CgosFunctionPointer exitSubModule;
	void *__dummy__ ;/*Required to compile this code with activated gcc constify plugin*/
}WDModuleFunctionList;

//ListItem for SubModule list
typedef struct WDListItem
{
	WDModuleFunctionList Functions;
	unsigned char SubModule;
	struct WDListItem *NextItem;
}WDListItem;

//First List Entry
//The first SubModule is considered the "active" one.
WDListItem *WDFirstListItem;

//***************************************************************************

/***********************************************************************
 * unsigned int WDRegisterSubModule(CgosFunctionPointer initSubModule,
 *									CgosFunctionPointer GetInfo,
 *									CgosFunctionPointer Count,
 *									CgosFunctionPointer IsAvailable,
 *									CgosFunctionPointer Trigger,
 *									CgosFunctionPointer GetTriggerCount,
 *									CgosFunctionPointer SetTriggerCount,
 *									CgosFunctionPointer GetConfigStruct,
 *									CgosFunctionPointer SetConfigStruct,
 *									CgosFunctionPointer SetConfig,
 *									CgosFunctionPointer Disable,
 *									CgosFunctionPointer exitSubModule,
 *									unsigned char ModuleNumber)												
 ***********************************************************************
 * Description: This function is used to register a sub module.			
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int WDRegisterSubModule(	CgosFunctionPointer initSubModule,
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
									unsigned char ModuleNumber)
{
	WDListItem *DummyPointer;
	dbgwd(printk("WDRegisterSubModule called\n");)
	
	DummyPointer = OsaMemAlloc(sizeof(WDListItem));
	if(DummyPointer == NULL)
	{
		return CGOS_ERROR;
	}
	
	DummyPointer->Functions.initSubModule = initSubModule;
	DummyPointer->Functions.GetInfo = GetInfo;
	DummyPointer->Functions.Count = Count;
	DummyPointer->Functions.IsAvailable = IsAvailable;
	DummyPointer->Functions.Trigger = Trigger;
	DummyPointer->Functions.GetTriggerCount = GetTriggerCount;
	DummyPointer->Functions.SetTriggerCount = SetTriggerCount;
	DummyPointer->Functions.GetConfigStruct = GetConfigStruct;
	DummyPointer->Functions.SetConfigStruct = SetConfigStruct;
	DummyPointer->Functions.SetConfig = SetConfig;
	DummyPointer->Functions.Disable = Disable;
	DummyPointer->Functions.exitSubModule = exitSubModule;
	
	DummyPointer->SubModule = ModuleNumber;
	DummyPointer->NextItem = WDFirstListItem;
	
	WDFirstListItem = DummyPointer;
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int WDGetCurrentSubModule(unsigned char *ModuleType)	    
 * 																		
 ***********************************************************************
 * Description: This function is used to get the current SubModule.		
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int WDGetCurrentSubModule(unsigned char *ModuleType)
{
	dbgwd(printk("WDGetCurrentSubModule called\n");)
	*ModuleType = WDFirstListItem->SubModule;
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int WDSetDefaultSubModule(unsigned char DefaultSubModule)   
 * 																		
 ***********************************************************************
 * Description: This function is used to set a SubModule as default.	
 * 																		
 * Last Change: 07.02.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int WDSetDefaultSubModule(unsigned char DefaultSubModule)
{
	WDListItem *Dummy1;
	WDListItem *Dummy2;
	dbgwd(printk("WDSetDefaultSubModule called\n");)
	//check if SubModule List is empty
	if(WDFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(WDFirstListItem->SubModule == DefaultSubModule)
		{
			return CGOS_SUCCESS;
		}
		else
		{
			Dummy1 = WDFirstListItem->NextItem;
			Dummy2 = WDFirstListItem;
			
			//Search SubModule list for requested SubModule
			while(Dummy1 != NULL)
			{
				//If found, change to active
				if(Dummy1->SubModule == DefaultSubModule)
				{
					Dummy2->NextItem = Dummy1->NextItem;
					Dummy1->NextItem = WDFirstListItem;
					WDFirstListItem = Dummy1;
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
				dbgwd(printk("WDSetDefaultSubModule error: SubModule not found");)
				return CGOS_ERROR;
			}
		}
	}
	else
	{
		dbgwd(printk("WDSetDefaultSubModule error: No SubModule available");)
		return CGOS_ERROR;
	}
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int WDChangeSubModule(CGOS_DRV_VARS *cdv,					
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
unsigned int WDChangeSubModule(CGOS_DRV_VARS *cdv, unsigned char NewSubModule)
{
	WDListItem *Dummy1;
	WDListItem *Dummy2;
	unsigned int retSuccess = CGOS_SUCCESS;
	
	dbgwd(printk("WDChangeSubModule called\n");)
	
	//check if SubModule List is empty
	if(WDFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(NewSubModule != WDFirstListItem->SubModule)	
		{
			//Exit the current SubModule
			if(WDFirstListItem->Functions.exitSubModule != NULL)
			{
			retSuccess = (WDFirstListItem->Functions.exitSubModule)(cdv);
			}
	
			//Change SubModule
			if(WDFirstListItem->NextItem != NULL)
			{
				Dummy1 = WDFirstListItem->NextItem;
				Dummy2 = WDFirstListItem;
			
				while(Dummy1 != NULL)
				{
					if(Dummy1->SubModule == NewSubModule)
					{
						Dummy2->NextItem = Dummy1->NextItem;
						Dummy1->NextItem = WDFirstListItem;
						WDFirstListItem = Dummy1;
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
			if((WDFirstListItem->Functions.initSubModule != NULL) && (retSuccess == CGOS_SUCCESS))
			{
			retSuccess = (WDFirstListItem->Functions.initSubModule)(cdv);
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
 * unsigned int initWDModule(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description: This function is called to copy information from		
 * 				WDModule_cfg.h into the cdv struct.						
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initWDModule(CGOS_DRV_VARS *cdv)
  {
	unsigned int retSuccess = CGOS_SUCCESS;
	
	//Initialize SubModuele list
	WDFirstListItem = NULL;
	
	//Register SubModule function pointers via callback.
	//WD_SUBMODULE_* enum in DrvVars.h.
	RegisterWDSubModule_BC(WDRegisterSubModule,WD_SUBMODULE_BC);

	//Set default SubModule.
	switch(cdv->brd->BoardGroup)
	{
		case GROUP_DEFAULT:
		default: 			WDSetDefaultSubModule(WD_SUBMODULE_BC);
	}


	//Initialize active SubModule
	if(WDFirstListItem->Functions.initSubModule != NULL)
	{
		(WDFirstListItem->Functions.initSubModule)(cdv);
	}
	
	return retSuccess;
  }

/***********************************************************************
 * unsigned int zCgosWDogGetInfo(CGOS_DRV_VARS *cdv)			  		
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
unsigned int zCgosWDogGetInfo(CGOS_DRV_VARS *cdv)
  {
	dbgwd(printk("zCgosWDogGetInfo called\n");)
    if(WDFirstListItem->Functions.GetInfo != NULL)
	{
		return (WDFirstListItem->Functions.GetInfo)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosWDogCount(CGOS_DRV_VARS *cdv)				  		
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
unsigned int zCgosWDogCount(CGOS_DRV_VARS *cdv)
  {
	dbgwd(printk("zCgosWDogCount called\n");)
    if(WDFirstListItem->Functions.Count != NULL)
	{
		return (WDFirstListItem->Functions.Count)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosWDogIsAvailable(CGOS_DRV_VARS *cdv)				
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
unsigned int zCgosWDogIsAvailable(CGOS_DRV_VARS *cdv)
  {
	dbgwd(printk("zCgosWDogIsAvailable called\n");)
    if(WDFirstListItem->Functions.IsAvailable != NULL)
	{
		return (WDFirstListItem->Functions.IsAvailable)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosWDogTrigger(CGOS_DRV_VARS *cdv)			  		
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
unsigned int zCgosWDogTrigger(CGOS_DRV_VARS *cdv)
  {
	dbgwd(printk("zCgosWDogTrigger called\n");)
    if(WDFirstListItem->Functions.Trigger != NULL)
	{
		return (WDFirstListItem->Functions.Trigger)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }
  
/***********************************************************************
 * unsigned int zCgosWDogGetTriggerCount(CGOS_DRV_VARS *cdv)	  		
 * 																		
 ***********************************************************************
 * Cgos Function: CgosWDogGetTriggerCount(HCGOS hCgos, 				    
 * 								  		  unsigned long dwUnit)		    
 *																	    
 ***********************************************************************
 * Description: 										
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosWDogGetTriggerCount(CGOS_DRV_VARS *cdv)
  {
	dbgwd(printk("zCgosWDogGetTriggerCount called\n");)
    if(WDFirstListItem->Functions.GetTriggerCount != NULL)
	{
		return (WDFirstListItem->Functions.GetTriggerCount)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosWDogSetTriggerCount(CGOS_DRV_VARS *cdv)			
 * 																		
 ***********************************************************************
 * Cgos Function: CgosWDogSetTriggerCount(HCGOS hCgos, 				    
 * 								  		  unsigned long dwUnit)		    	
 * 										  unsigned int cnt   		    
 * 																		
 ***********************************************************************
 * Description: 											
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosWDogSetTriggerCount(CGOS_DRV_VARS *cdv)
  {
	dbgwd(printk("zCgosWDogSetTriggerCount called\n");)
    if(WDFirstListItem->Functions.SetTriggerCount != NULL)
	{
		return (WDFirstListItem->Functions.SetTriggerCount)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosWDogGetConfigStruct(CGOS_DRV_VARS *cdv)	  		
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
unsigned int zCgosWDogGetConfigStruct(CGOS_DRV_VARS *cdv)
  {
	dbgwd(printk("zCgosWDogGetConfigStruct called\n");)
    if(WDFirstListItem->Functions.GetConfigStruct != NULL)
	{
		return (WDFirstListItem->Functions.GetConfigStruct)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosWDogSetConfigStruct(CGOS_DRV_VARS *cdv)	  		
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
unsigned int zCgosWDogSetConfigStruct(CGOS_DRV_VARS *cdv)
  {
	dbgwd(printk("zCgosWDogSetConfigStruct called\n");)
    if(WDFirstListItem->Functions.SetConfigStruct != NULL)
	{
		return (WDFirstListItem->Functions.SetConfigStruct)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }



/***********************************************************************
 * unsigned int zCgosWDogSetConfig(CGOS_DRV_VARS *cdv)			  		
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
unsigned int zCgosWDogSetConfig(CGOS_DRV_VARS *cdv)
  {
	dbgwd(printk("zCgosWDogSetConfig called\n");)
    if(WDFirstListItem->Functions.SetConfig != NULL)
	{
		return (WDFirstListItem->Functions.SetConfig)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosWDogDisable(CGOS_DRV_VARS *cdv)			  		
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
unsigned int zCgosWDogDisable(CGOS_DRV_VARS *cdv)
  { 
	dbgwd(printk("zCgosWDogDisable called\n");)
    if(WDFirstListItem->Functions.Disable != NULL)
	{
		return (WDFirstListItem->Functions.Disable)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int WDModuleHandler(CGOS_DRV_FCT *df, 						
 * 								CGOS_DRV_VARS *cdv)						
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
unsigned int WDModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv)
{
	dbgwd(printk("WDModuleHandler called\n");)
	return (*df->fct)(cdv);
}

/***********************************************************************
 * void exitWDModule(CGOS_DRV_VARS *cdv) 							    
 * 																	
 ***********************************************************************
 * Description: This function is called during driver close and should	
 * 				free allocated resources.							
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																	
 * Modifications:														
 ***********************************************************************/
void exitWDModule(CGOS_DRV_VARS *cdv)
{
	WDListItem *Dummy;
	dbgwd(printk("exitWDModule called\n");)
	
	//Call exit function of active SubModule.
	if(WDFirstListItem->Functions.exitSubModule != NULL)
	{
		(WDFirstListItem->Functions.exitSubModule)(cdv);
	}

	//Free SubModule list
	while(WDFirstListItem != NULL)
	{
			Dummy = WDFirstListItem;
			WDFirstListItem = WDFirstListItem->NextItem;
			OsaMemFree(Dummy);
	}
}
