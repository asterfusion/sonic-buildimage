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
#include "StoModule_cfg.h"
#include "StoSubModules/StoSubModule_BC.h"

//***************************************************************************

//#define dbg_sto_on

#ifdef dbg_sto_on
	#define dbgsto(x) x
#else
	#define dbgsto(x)
#endif

//***************************************************************************

//List of required functions for the STO Module
typedef struct
{
	CgosFunctionPointer initSubModule;
	CgosFunctionPointer StoAreaCount;
	CgosFunctionPointer StoAreaType;
	CgosFunctionPointer StoAreaSize;
	CgosFunctionPointer StoAreaBlockSize;
	CgosFunctionPointer StoAreaRead;
	CgosFunctionPointer StoAreaWrite;
	CgosFunctionPointer StoAreaErase;
	CgosFunctionPointer StoAreaEraseStatus;
	CgosFunctionPointer StoAreaLock;
	CgosFunctionPointer	StoAreaUnlock;
	CgosFunctionPointer StoAreaIsLocked;
	CgosFunctionPointer exitSubModule;	
	void *__dummy__; /*Required to compile this code with activated gcc constify plugin*/
}STOModuleFunctionList;

//ListItem for SubModule list
typedef struct STOListItem
{
	STOModuleFunctionList Functions;
	unsigned char SubModule;
	struct STOListItem *NextItem;
}STOListItem;

//First List Entry
//The first SubModule is considered the "active" one.
STOListItem *STOFirstListItem;

//***************************************************************************

/***********************************************************************
 * unsigned int StoRegisterSubModule(CgosFunctionPointer initSubModule,
 *									 CgosFunctionPointer StoAreaCount,
 *									 CgosFunctionPointer StoAreaType,
 *									 CgosFunctionPointer StoAreaSize,
 *									 CgosFunctionPointer StoAreaBlockSize,
 *									 CgosFunctionPointer StoAreaRead,
 *									 CgosFunctionPointer StoAreaWrite,
 *									 CgosFunctionPointer StoAreaErase,
 *									 CgosFunctionPointer StoAreaEraseStatus,
 *									 CgosFunctionPointer StoAreaLock,
 *									 CgosFunctionPointer StoAreaUnlock,
 *									 CgosFunctionPointer StoAreaIsLocked,
 *									 CgosFunctionPointer exitSubModule,
 * 									 unsigned char ModuleNumber)									
 ***********************************************************************
 * Description: This function is used to register a sub module.			
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int StoRegisterSubModule(CgosFunctionPointer initSubModule,
  								  CgosFunctionPointer StoAreaCount,
 								  CgosFunctionPointer StoAreaType,
 								  CgosFunctionPointer StoAreaSize,
 								  CgosFunctionPointer StoAreaBlockSize,
 								  CgosFunctionPointer StoAreaRead,
 								  CgosFunctionPointer StoAreaWrite,
 								  CgosFunctionPointer StoAreaErase,
 								  CgosFunctionPointer StoAreaEraseStatus,
 								  CgosFunctionPointer StoAreaLock,
 								  CgosFunctionPointer StoAreaUnlock,
 								  CgosFunctionPointer StoAreaIsLocked,
 								  CgosFunctionPointer exitSubModule,
 								  unsigned char ModuleNumber)
{
	STOListItem *DummyPointer;
	dbgsto(printk("StoRegisterSubModule called\n");)
	
	DummyPointer = OsaMemAlloc(sizeof(STOListItem));
	if(DummyPointer == NULL)
	{
		return CGOS_ERROR;
	}
	
	DummyPointer->Functions.initSubModule = initSubModule;
	DummyPointer->Functions.StoAreaCount = StoAreaCount;
	DummyPointer->Functions.StoAreaType = StoAreaType;
	DummyPointer->Functions.StoAreaSize = StoAreaSize;
	DummyPointer->Functions.StoAreaBlockSize = StoAreaBlockSize;
	DummyPointer->Functions.StoAreaRead = StoAreaRead;
	DummyPointer->Functions.StoAreaWrite = StoAreaWrite;
	DummyPointer->Functions.StoAreaErase = StoAreaErase;
	DummyPointer->Functions.StoAreaEraseStatus = StoAreaEraseStatus;
	DummyPointer->Functions.StoAreaLock = StoAreaLock;
	DummyPointer->Functions.StoAreaUnlock = StoAreaUnlock;
	DummyPointer->Functions.StoAreaIsLocked = StoAreaIsLocked;
	DummyPointer->Functions.exitSubModule = exitSubModule;
	
	DummyPointer->SubModule = ModuleNumber;
	DummyPointer->NextItem = STOFirstListItem;
	
	STOFirstListItem = DummyPointer;
	
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int StoGetCurrentSubModule(unsigned char *ModuleType)	    
 * 																		
 ***********************************************************************
 * Description: This function is used to get the current SubModule.		
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int StoGetCurrentSubModule(unsigned char *ModuleType)
{
	dbgsto(printk("StoGetCurrentSubModule called\n");)
	*ModuleType = STOFirstListItem->SubModule;
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int StoSetDefaultSubModule(unsigned char DefaultSubModule)   
 * 																		
 ***********************************************************************
 * Description: This function is used to set a SubModule as default.	
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int StoSetDefaultSubModule(unsigned char DefaultSubModule)
{
	STOListItem *Dummy1;
	STOListItem *Dummy2;
	dbgsto(printk("StoSetDefaultSubModule called\n");)
	//check if SubModule List is empty
	if(STOFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(STOFirstListItem->SubModule == DefaultSubModule)
		{
			return CGOS_SUCCESS;
		}
		else
		{
			Dummy1 = STOFirstListItem->NextItem;
			Dummy2 = STOFirstListItem;
			
			//Search SubModule list for requested SubModule
			while(Dummy1 != NULL)
			{
				//If found, change to active
				if(Dummy1->SubModule == DefaultSubModule)
				{
					Dummy2->NextItem = Dummy1->NextItem;
					Dummy1->NextItem = STOFirstListItem;
					STOFirstListItem = Dummy1;
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
				dbgsto(printk("StoSetDefaultSubModule error: SubModule not found");)
				return CGOS_ERROR;
			}
		}
	}
	else
	{
		dbgsto(printk("StoSetDefaultSubModule error: No SubModule available");)
		return CGOS_ERROR;
	}
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int StoChangeSubModule(CGOS_DRV_VARS *cdv,					
 * 								  unsigned char NewSubModule)		    
 * 																		
 ***********************************************************************
 * Description: This function changes the used SubModule				
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int StoChangeSubModule(CGOS_DRV_VARS *cdv, unsigned char NewSubModule)
{
	STOListItem *Dummy1;
	STOListItem *Dummy2;
	unsigned int retSuccess = CGOS_SUCCESS;
	
	dbgsto(printk("StoChangeSubModule called\n");)
	
	//check if SubModule List is empty
	if(STOFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(NewSubModule != STOFirstListItem->SubModule)	
		{
			//Exit the current SubModule
			if(STOFirstListItem->Functions.exitSubModule != NULL)
			{
			retSuccess = (STOFirstListItem->Functions.exitSubModule)(cdv);
			}
	
			//Change SubModule
			if(STOFirstListItem->NextItem != NULL)
			{
				Dummy1 = STOFirstListItem->NextItem;
				Dummy2 = STOFirstListItem;
			
				while(Dummy1 != NULL)
				{
					if(Dummy1->SubModule == NewSubModule)
					{
						Dummy2->NextItem = Dummy1->NextItem;
						Dummy1->NextItem = STOFirstListItem;
						STOFirstListItem = Dummy1;
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
			if((STOFirstListItem->Functions.initSubModule != NULL) && (retSuccess == CGOS_SUCCESS))
			{
			retSuccess = (STOFirstListItem->Functions.initSubModule)(cdv);
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
 * unsigned int initStoModule(CGOS_DRV_VARS *cdv) 					    
 * 																	
 ***********************************************************************
 * Description: 										
 * 																	
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	-																	
 ***********************************************************************/
unsigned int initStoModule(CGOS_DRV_VARS *cdv)
  {
	unsigned int retSuccess = CGOS_SUCCESS;
	dbgsto(printk("initStoModule called\n");)
	
	//Initialize SubModuele list
	STOFirstListItem = NULL;
	
	//Register SubModule function pointers via callback.
	//STO_SUBMODULE_* enum in DrvVars.h.
	RegisterStoSubModule_BC(StoRegisterSubModule,STO_SUBMODULE_BC);

	//Set default SubModule.
	switch(cdv->brd->BoardGroup)
	{
		case GROUP_DEFAULT:	
		default: 			StoSetDefaultSubModule(STO_SUBMODULE_BC);
	}

	//Initialize active SubModule
	if(STOFirstListItem->Functions.initSubModule != NULL)
	{
		(STOFirstListItem->Functions.initSubModule)(cdv);
	}
	
	return retSuccess;
  }  

/***********************************************************************
 * unsigned int zCgosStorageAreaCount(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/    
unsigned int zCgosStorageAreaCount(CGOS_DRV_VARS *cdv)
  {
	dbgsto(printk("zCgosStorageAreaCount called\n");)	
	if(STOFirstListItem->Functions.StoAreaCount != NULL)
	{
		return (STOFirstListItem->Functions.StoAreaCount)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosStorageAreaType(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosStorageAreaType(CGOS_DRV_VARS *cdv)
  {
	dbgsto(printk("zCgosStorageAreaType called\n");)	
	if(STOFirstListItem->Functions.StoAreaType != NULL)
	{
		return (STOFirstListItem->Functions.StoAreaType)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosStorageAreaSize(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosStorageAreaSize(CGOS_DRV_VARS *cdv)
  {
	dbgsto(printk("zCgosStorageAreaSize called\n");)	
	if(STOFirstListItem->Functions.StoAreaSize != NULL)
	{
		return (STOFirstListItem->Functions.StoAreaSize)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosStorageAreaBlockSize(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosStorageAreaBlockSize(CGOS_DRV_VARS *cdv)
  {
	dbgsto(printk("zCgosStorageAreaBlockSize called\n");)
	if(STOFirstListItem->Functions.StoAreaBlockSize!= NULL)
	{
		return (STOFirstListItem->Functions.StoAreaBlockSize)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosStorageAreaRead(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosStorageAreaRead(CGOS_DRV_VARS *cdv)
  {
	dbgsto(printk("zCgosStorageAreaRead called\n");)	
	if(STOFirstListItem->Functions.StoAreaRead != NULL)
	{
		return (STOFirstListItem->Functions.StoAreaRead)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosStorageAreaWrite(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosStorageAreaWrite(CGOS_DRV_VARS *cdv)
  {
	dbgsto(printk("zCgosStorageAreaWrite called\n");)
	if(STOFirstListItem->Functions.StoAreaWrite != NULL)
	{
		return (STOFirstListItem->Functions.StoAreaWrite)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }
  
/***********************************************************************
 * unsigned int zCgosStorageAreaErase(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosStorageAreaErase(CGOS_DRV_VARS *cdv)
  {
	dbgsto(printk("zCgosStorageAreaErase called\n");)
	if(STOFirstListItem->Functions.StoAreaErase != NULL)
	{
		return (STOFirstListItem->Functions.StoAreaErase)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosStorageAreaEraseStatus(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosStorageAreaEraseStatus(CGOS_DRV_VARS *cdv)
  {
	dbgsto(printk("zCgosStorageAreaEraseStatus called\n");)
	if(STOFirstListItem->Functions.StoAreaEraseStatus != NULL)
	{
		return (STOFirstListItem->Functions.StoAreaEraseStatus)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }
 
/***********************************************************************
 * unsigned int zCgosStorageAreaLock(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/    
unsigned int zCgosStorageAreaLock(CGOS_DRV_VARS *cdv)					
  {
	dbgsto(printk("zCgosStorageAreaLock called\n");)	
	if(STOFirstListItem->Functions.StoAreaLock != NULL)
	{
		return (STOFirstListItem->Functions.StoAreaLock)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }
 
/***********************************************************************
 * unsigned int zCgosStorageAreaUnlock(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/    
unsigned int zCgosStorageAreaUnlock(CGOS_DRV_VARS *cdv)					
  {
	dbgsto(printk("zCgosStorageAreaUnlock called\n");)	
	if(STOFirstListItem->Functions.StoAreaUnlock != NULL)
	{
		return (STOFirstListItem->Functions.StoAreaUnlock)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }
  
/***********************************************************************
 * unsigned int zCgosStorageAreaIsLocked(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 26.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/    
unsigned int zCgosStorageAreaIsLocked(CGOS_DRV_VARS *cdv)					
  {
	dbgsto(printk("zCgosStorageAreaIsLocked called\n");)	
	if(STOFirstListItem->Functions.StoAreaIsLocked != NULL)
	{
		return (STOFirstListItem->Functions.StoAreaIsLocked)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int StoModuleHandler(CGOS_DRV_FCT *df, 						
 * 								 CGOS_DRV_VARS *cdv)					
 * 																		
 **********************************************************************
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
unsigned int StoModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv)
{
	dbgsto(printk("StoModuleHandler called\n");)	
	return (*df->fct)(cdv);
}  

/***********************************************************************
 * void exitStoModule(CGOS_DRV_VARS *cdv) 							    
 * 																		
 ***********************************************************************
 * Description: This function is called during driver close and should	
 * 				free allocated resources.								
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
void exitStoModule(CGOS_DRV_VARS *cdv)
{
	STOListItem *Dummy;
	dbgsto(printk("exitStoModule called\n");)

	//Call exit function of active SubModule.
	if(STOFirstListItem->Functions.exitSubModule != NULL)
	{
		(STOFirstListItem->Functions.exitSubModule)(cdv);
	}

	//Free SubModule list
	while(STOFirstListItem != NULL)
	{
			Dummy = STOFirstListItem;
			STOFirstListItem = STOFirstListItem->NextItem;
			OsaMemFree(Dummy);
	}
}
