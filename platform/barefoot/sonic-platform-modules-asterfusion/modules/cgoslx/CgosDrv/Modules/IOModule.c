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
#include "IOModule_cfg.h"
#include "IOSubModules/IOSubModule_BC.h"

//***************************************************************************

//#define dbg_io_on

#ifdef dbg_io_on
	#define dbgio(x) x
#else
	#define dbgio(x)
#endif

//***************************************************************************
//List of required functions for the IO Module
typedef struct
{
	CgosFunctionPointer initSubModule;
	CgosFunctionPointer Count;
	CgosFunctionPointer Read;
	CgosFunctionPointer Write;
	CgosFunctionPointer GetDirection;
	CgosFunctionPointer SetDirection;
	CgosFunctionPointer IsAvailable;
	CgosFunctionPointer GetDirectionCaps;
	CgosFunctionPointer exitSubModule;
	void *__dummy__; /*Required to compile this code with activated gcc constify plugin*/	
}IOModuleFunctionList;

//ListItem for SubModule list
typedef struct IOListItem
{
	IOModuleFunctionList Functions;
	unsigned char SubModule;
	struct IOListItem *NextItem;
}IOListItem;

//First List Entry
//The first SubModule is considered the "active" one.
IOListItem *IOFirstListItem;

//***************************************************************************

/***********************************************************************
 * unsigned int IORegisterSubModule(CgosFunctionPointer initSubModule,	
 *								  CgosFunctionPointer Count,
 *								  CgosFunctionPointer Read,				
 *								  CgosFunctionPointer Write,				
 *								  CgosFunctionPointer GetDirection,		
 *								  CgosFunctionPointer SetDirection,	
 *								  CgosFunctionPointer IsAvailable,		
 *								  CgosFunctionPointer GetDirectionCaps,	
 *								  CgosFunctionPointer exitSubModule,
 * 								  unsigned char ModuleNumber)	
 * 																		
 ***********************************************************************
 * Description: This function is used to register a sub module.			
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int IORegisterSubModule(CgosFunctionPointer initSubModule,
								 CgosFunctionPointer Count,
								 CgosFunctionPointer Read,
								 CgosFunctionPointer Write,
								 CgosFunctionPointer GetDirection,
								 CgosFunctionPointer SetDirection,
								 CgosFunctionPointer IsAvailable,
								 CgosFunctionPointer GetDirectionCaps,
								 CgosFunctionPointer exitSubModule,
								 unsigned char ModuleNumber)
{
	IOListItem *DummyPointer;
	dbgio(printk("IORegisterSubModule called\n");)
	
	DummyPointer = OsaMemAlloc(sizeof(IOListItem));
	if(DummyPointer == NULL)
	{
		return CGOS_ERROR;
	}
	
	DummyPointer->Functions.initSubModule = initSubModule;
	DummyPointer->Functions.Count = Count;
	DummyPointer->Functions.Read = Read;
	DummyPointer->Functions.Write = Write;
	DummyPointer->Functions.GetDirection = GetDirection;
	DummyPointer->Functions.SetDirection = SetDirection;
	DummyPointer->Functions.IsAvailable = IsAvailable;
	DummyPointer->Functions.GetDirectionCaps = GetDirectionCaps;
	DummyPointer->Functions.exitSubModule = exitSubModule;
	
	DummyPointer->SubModule = ModuleNumber;
	DummyPointer->NextItem = IOFirstListItem;
	
	IOFirstListItem = DummyPointer;
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int IOGetCurrentSubModule(unsigned char *ModuleType)	    
 * 																		
 ***********************************************************************
 * Description: This function is used to get the current SubModule.		
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int IOGetCurrentSubModule(unsigned char *ModuleType)
{
	dbgio(printk("IOGetCurrentSubModule called\n");)
	*ModuleType = IOFirstListItem->SubModule;
	return CGOS_SUCCESS;
}
/***********************************************************************
 * unsigned int IOSetDefaultSubModule(unsigned char DefaultSubModule)   
 * 																		
 ***********************************************************************
 * Description: This function is used to set a SubModule as default.	
 * 																		
 * Last Change: 07.02.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int IOSetDefaultSubModule(unsigned char DefaultSubModule)
{
	IOListItem *Dummy1;
	IOListItem *Dummy2;
	dbgio(printk("IOSetDefaultSubModule called\n");)
	//check if SubModule List is empty
	if(IOFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(IOFirstListItem->SubModule == DefaultSubModule)
		{
			return CGOS_SUCCESS;
		}
		else
		{
			Dummy1 = IOFirstListItem->NextItem;
			Dummy2 = IOFirstListItem;
			
			//Search SubModule list for requested SubModule
			while(Dummy1 != NULL)
			{
				//If found, change to active
				if(Dummy1->SubModule == DefaultSubModule)
				{
					Dummy2->NextItem = Dummy1->NextItem;
					Dummy1->NextItem = IOFirstListItem;
					IOFirstListItem = Dummy1;
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
				dbgio(printk("IOSetDefaultSubModule error: SubModule not found");)
				return CGOS_ERROR;
			}
		}
	}
	else
	{
		dbgio(printk("IOSetDefaultSubModule error: No SubModule available");)
		return CGOS_ERROR;
	}
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int IOChangeSubModule(CGOS_DRV_VARS *cdv,					
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
unsigned int IOChangeSubModule(CGOS_DRV_VARS *cdv, unsigned char NewSubModule)
{
	IOListItem *Dummy1;
	IOListItem *Dummy2;
	unsigned int retSuccess = CGOS_SUCCESS;
	
	dbgio(printk("IOChangeSubModule called\n");)
	
	//check if SubModule List is empty
	if(IOFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(NewSubModule != IOFirstListItem->SubModule)	
		{
			//Exit the current SubModule
			if(IOFirstListItem->Functions.exitSubModule != NULL)
			{
			retSuccess = (IOFirstListItem->Functions.exitSubModule)(cdv);
			}
	
			//Change SubModule
			if(IOFirstListItem->NextItem != NULL)
			{
				Dummy1 = IOFirstListItem->NextItem;
				Dummy2 = IOFirstListItem;
			
				while(Dummy1 != NULL)
				{
					if(Dummy1->SubModule == NewSubModule)
					{
						Dummy2->NextItem = Dummy1->NextItem;
						Dummy1->NextItem = IOFirstListItem;
						IOFirstListItem = Dummy1;
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
			if((IOFirstListItem->Functions.initSubModule != NULL) && (retSuccess == CGOS_SUCCESS))
			{
			retSuccess = (IOFirstListItem->Functions.initSubModule)(cdv);
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
 * unsigned int initIOModule(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description: This function registers all SubModule functions and		
 * 				calls the init function of the default SubModule.		
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int initIOModule(CGOS_DRV_VARS *cdv)
{
	unsigned int retSuccess = CGOS_SUCCESS;
	dbgio(printk("initIOModule called\n");)
	
	//Initialize SubModuele list
	IOFirstListItem = NULL;
	
	//Register SubModule function pointers via callback.
	//IO_SUBMODULE_* enum in DrvVars.h.
	RegisterIOSubModule_BC(IORegisterSubModule,IO_SUBMODULE_BC);

	//Set default SubModule.
	switch(cdv->brd->BoardGroup)
	{
		case GROUP_DEFAULT:
		default: 			IOSetDefaultSubModule(IO_SUBMODULE_BC);
	}

	//Initialize active SubModule
	if(IOFirstListItem->Functions.initSubModule != NULL)
	{
		(IOFirstListItem->Functions.initSubModule)(cdv);
	}
	
	return retSuccess;
}

/***********************************************************************
 * unsigned int zCgosIOCount(CGOS_DRV_VARS *cdv)		  				
 * 																		
 ***********************************************************************
 * Description: Calls CgosIOCount from the chosen SubModule.			
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/    
unsigned int zCgosIOCount(CGOS_DRV_VARS *cdv)
  {
	dbgio(printk("zCgosIOCount called\n");)
    if(IOFirstListItem->Functions.Count != NULL)
	{
		return (IOFirstListItem->Functions.Count)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosIORead(CGOS_DRV_VARS *cdv)					  		
 * 																		
 ***********************************************************************
 * Description: Calls CgosIORead from the chosen SubModule.			
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosIORead(CGOS_DRV_VARS *cdv)
{
	dbgio(printk("zCgosIORead called\n");)
	if(IOFirstListItem->Functions.Read != NULL)
	{
		return (IOFirstListItem->Functions.Read)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
}

/***********************************************************************
 * unsigned int zCgosIOWrite(CGOS_DRV_VARS *cdv)				  		
 * 																		
 ***********************************************************************
 * Description: Calls CgosIOWrite from the chosen SubModule.			
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosIOWrite(CGOS_DRV_VARS *cdv)
  {
	dbgio(printk("zCgosIOWrite called\n");)
	if(IOFirstListItem->Functions.Write != NULL)
	{
		return (IOFirstListItem->Functions.Write)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosIOGetDirection(CGOS_DRV_VARS *cdv)			  		
 * 																		
 ***********************************************************************
 * Description: Calls CgosIOGetDirecton from the chosen SubModule.		
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosIOGetDirection(CGOS_DRV_VARS *cdv)
  {
	dbgio(printk("zCgosIOGetDirection called\n");)
	if(IOFirstListItem->Functions.GetDirection != NULL)
	{
		return (IOFirstListItem->Functions.GetDirection)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosIOSetDirectionCGOS_DRV_VARS *cdv)			  		
 * 																		
 ***********************************************************************
 * Description: Calls CgosIOSetDirecton from the chosen SubModule.		
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosIOSetDirection(CGOS_DRV_VARS *cdv)
  {
	dbgio(printk("zCgosIOSetDirection called\n");)
	if(IOFirstListItem->Functions.SetDirection != NULL)
	{
		return (IOFirstListItem->Functions.SetDirection)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosIOIsAvailable(CGOS_DRV_VARS *cdv)			  		
 * 																		
 ***********************************************************************
 * Description: Calls CgosIOIsAvailable from the chosen SubModule.		
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosIOIsAvailable(CGOS_DRV_VARS *cdv)
  {
	dbgio(printk("zCgosIOIsAvailable called\n");)
	if(IOFirstListItem->Functions.IsAvailable != NULL)
	{
		return (IOFirstListItem->Functions.IsAvailable)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }
  
/***********************************************************************
 * unsigned int zCgosIOGetDirectionCaps(CGOS_DRV_VARS *cdv)		  		
 * 																		
 ***********************************************************************
 * Description: Calls CgosIOGetDirectonCaps from the chosen SubModule.	
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosIOGetDirectionCaps(CGOS_DRV_VARS *cdv)
  {  
	dbgio(printk("zCgosIOGetDirectionCaps called\n");)
	if(IOFirstListItem->Functions.GetDirectionCaps != NULL)
	{
		return (IOFirstListItem->Functions.GetDirectionCaps)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int IOModuleHandler(CGOS_DRV_FCT *df, 						
 * 								CGOS_DRV_VARS *cdv)						
 * 																		
 ***********************************************************************
 * Description: This function is called by the module dispatcher and 	
 * 				calls the appropriate module function for the requested 
 * 				function call. This function is a defined entry point	
 * 				to the module and can be used for module specific		
 * 				parameter checking or to change the used SubModule		
 * 				before passing the call.								
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int IOModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv)
{
	dbgio(printk("IOModuleHandler called\n");)
	return (*df->fct)(cdv);
}

/***********************************************************************
 * void exitIOModule(CGOS_DRV_VARS *cdv)		  						
 * 																		
 ***********************************************************************
 * Description: Calls exitIOModule from the chosen SubModule.			
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
void exitIOModule(CGOS_DRV_VARS *cdv)
{
	IOListItem *Dummy;
	dbgio(printk("exitIOModule called\n");)
	
	//Call exit function of active SubModule.
	if(IOFirstListItem->Functions.exitSubModule != NULL)
	{
		(IOFirstListItem->Functions.exitSubModule)(cdv);
	}

	//Free SubModule list
	while(IOFirstListItem != NULL)
	{
			Dummy = IOFirstListItem;
			IOFirstListItem = IOFirstListItem->NextItem;
			OsaMemFree(Dummy);
	}
}
