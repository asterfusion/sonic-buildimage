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
#include "VgaSubModules/VgaSubModule_BC.h"

//***************************************************************************

//#define dbg_vga_on

#ifdef dbg_vga_on
	#define dbgvga(x) x
#else
	#define dbgvga(x)
#endif


//***************************************************************************

//List of required functions for the VGA Module
typedef struct
{
	CgosFunctionPointer initSubModule;
	CgosFunctionPointer VgaCount;
	CgosFunctionPointer VgaGetInfo;
	CgosFunctionPointer VgaGetContrast;
	CgosFunctionPointer VgaSetContrast;
	CgosFunctionPointer VgaGetContrastEnable;
	CgosFunctionPointer VgaSetContrastEnable;
	CgosFunctionPointer VgaGetBacklight;
	CgosFunctionPointer VgaSetBacklight;
	CgosFunctionPointer VgaGetBacklightEnable;
	CgosFunctionPointer	VgaSetBacklightEnable;
	CgosFunctionPointer VgaEndDarkBoot;
	CgosFunctionPointer exitSubModule;	
	void *__dummy__; /*Required to compile this code with activated gcc constify plugin*/
}VGAModuleFunctionList;


//ListItem for SubModule list
typedef struct VGAListItem
{
	VGAModuleFunctionList Functions;
	unsigned char SubModule;
	struct VGAListItem *NextItem;
}VGAListItem;

//First List Entry
//The first SubModule is considered the "active" one.
VGAListItem *VGAFirstListItem;

//***************************************************************************

/***********************************************************************
 * unsigned int VgaRegisterSubModule(CgosFunctionPointer initSubModule,
 *									 CgosFunctionPointer VgaCount,
 *								     CgosFunctionPointer VgaGetInfo,
 *									 CgosFunctionPointer VgaGetContrast,
 *									 CgosFunctionPointer VgaSetContrast,
 *								     CgosFunctionPointer VgaGetContrastEnable,
 *								     CgosFunctionPointer VgaSetContrastEnable,
 *								     CgosFunctionPointer VgaGetBacklight,
 *								     CgosFunctionPointer VgaSetBacklight,
 *								     CgosFunctionPointer VgaGetBacklightEnable,
 *								     CgosFunctionPointer VgaSetBacklightEnable,
 *								     CgosFunctionPointer VgaEndDarkBoot,
 *								     CgosFunctionPointer exitSubModule,
 * 									 unsigned char ModuleNumber)	
 * 																		
 ***********************************************************************
 * Description: This function is used to register a sub module.			
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int VgaRegisterSubModule(CgosFunctionPointer initSubModule,				    
 							  	  CgosFunctionPointer VgaCount,
 								  CgosFunctionPointer VgaGetInfo,
								  CgosFunctionPointer VgaGetContrast,
 								  CgosFunctionPointer VgaSetContrast,
 								  CgosFunctionPointer VgaGetContrastEnable,
 								  CgosFunctionPointer VgaSetContrastEnable,
 								  CgosFunctionPointer VgaGetBacklight,
 								  CgosFunctionPointer VgaSetBacklight,
 								  CgosFunctionPointer VgaGetBacklightEnable,
 								  CgosFunctionPointer VgaSetBacklightEnable,
 								  CgosFunctionPointer VgaEndDarkBoot,
 								  CgosFunctionPointer exitSubModule,
 								  unsigned char ModuleNumber)
{
	VGAListItem *DummyPointer;
	dbgvga(printk("VgaRegisterSubModule called\n");)
	
	DummyPointer = OsaMemAlloc(sizeof(VGAListItem));
	if(DummyPointer == NULL)
	{
		return CGOS_ERROR;
	}
	
	DummyPointer->Functions.initSubModule = initSubModule;
	DummyPointer->Functions.VgaCount = VgaCount;
	DummyPointer->Functions.VgaGetInfo = VgaGetInfo;
	DummyPointer->Functions.VgaGetContrast = VgaGetContrast;
	DummyPointer->Functions.VgaSetContrast = VgaSetContrast;
	DummyPointer->Functions.VgaGetContrastEnable = VgaGetContrastEnable;
	DummyPointer->Functions.VgaSetContrastEnable = VgaSetContrastEnable;
	DummyPointer->Functions.VgaGetBacklight = VgaGetBacklight;
	DummyPointer->Functions.VgaSetBacklight = VgaSetBacklight;
	DummyPointer->Functions.VgaGetBacklightEnable = VgaGetBacklightEnable;
	DummyPointer->Functions.VgaSetBacklightEnable = VgaSetBacklightEnable;
	DummyPointer->Functions.VgaEndDarkBoot = VgaEndDarkBoot;
	DummyPointer->Functions.exitSubModule = exitSubModule;
	
	DummyPointer->SubModule = ModuleNumber;
	DummyPointer->NextItem = VGAFirstListItem;
	
	VGAFirstListItem = DummyPointer;
	
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int VgaGetCurrentSubModule(unsigned char *ModuleType)	    
 * 																		
 ***********************************************************************
 * Description: This function is used to get the current SubModule.		
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int VgaGetCurrentSubModule(unsigned char *ModuleType)
{
	dbgvga(printk("VgaGetCurrentSubModule called\n");)
	*ModuleType = VGAFirstListItem->SubModule;
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int VgaSetDefaultSubModule(unsigned char DefaultSubModule)   
 * 																		
 ***********************************************************************
 * Description: This function is used to set a SubModule as default.	
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int VgaSetDefaultSubModule(unsigned char DefaultSubModule)
{
	VGAListItem *Dummy1;
	VGAListItem *Dummy2;
	dbgvga(printk("VgaSetDefaultSubModule called\n");)
	//check if SubModule List is empty
	if(VGAFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(VGAFirstListItem->SubModule == DefaultSubModule)
		{
			return CGOS_SUCCESS;
		}
		else
		{
			Dummy1 = VGAFirstListItem->NextItem;
			Dummy2 = VGAFirstListItem;
			
			//Search SubModule list for requested SubModule
			while(Dummy1 != NULL)
			{
				//If found, change to active
				if(Dummy1->SubModule == DefaultSubModule)
				{
					Dummy2->NextItem = Dummy1->NextItem;
					Dummy1->NextItem = VGAFirstListItem;
					VGAFirstListItem = Dummy1;
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
				dbgvga(printk("VgaSetDefaultSubModule error: SubModule not found");)
				return CGOS_ERROR;
			}
		}
	}
	else
	{
		dbgvga(printk("VgaSetDefaultSubModule error: No SubModule available");)
		return CGOS_ERROR;
	}
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int VgaChangeSubModule(CGOS_DRV_VARS *cdv,					
 * 								  unsigned char NewSubModule)		    
 * 																		
 ***********************************************************************
 * Description: This function changes the used SubModule				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int VgaChangeSubModule(CGOS_DRV_VARS *cdv, unsigned char NewSubModule)
{
	VGAListItem *Dummy1;
	VGAListItem *Dummy2;
	unsigned int retSuccess = CGOS_SUCCESS;
	
	dbgvga(printk("VgaChangeSubModule called\n");)
	
	//check if SubModule List is empty
	if(VGAFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(NewSubModule != VGAFirstListItem->SubModule)	
		{
			//Exit the current SubModule
			if(VGAFirstListItem->Functions.exitSubModule != NULL)
			{
			retSuccess = (VGAFirstListItem->Functions.exitSubModule)(cdv);
			}
	
			//Change SubModule
			if(VGAFirstListItem->NextItem != NULL)
			{
				Dummy1 = VGAFirstListItem->NextItem;
				Dummy2 = VGAFirstListItem;
			
				while(Dummy1 != NULL)
				{
					if(Dummy1->SubModule == NewSubModule)
					{
						Dummy2->NextItem = Dummy1->NextItem;
						Dummy1->NextItem = VGAFirstListItem;
						VGAFirstListItem = Dummy1;
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
			if((VGAFirstListItem->Functions.initSubModule != NULL) && (retSuccess == CGOS_SUCCESS))
			{
			retSuccess = (VGAFirstListItem->Functions.initSubModule)(cdv);
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
 * unsigned int initVgaModule(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description: 					
 * 																		
 * Last Change: 25.06.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initVgaModule(CGOS_DRV_VARS *cdv)
  {
	unsigned int retSuccess = CGOS_SUCCESS;
	dbgvga(printk("initVgaModule called\n");)
	
	//Initialize SubModuele list
	VGAFirstListItem = NULL;
	
	//Register SubModule function pointers via callback.
	//VGA_SUBMODULE_* enum in DrvVars.h.
	RegisterVgaSubModule_BC(VgaRegisterSubModule,VGA_SUBMODULE_BC);

	//Set default SubModule.
	switch(cdv->brd->BoardGroup)
	{
		case GROUP_DEFAULT:
		default: 			VgaSetDefaultSubModule(VGA_SUBMODULE_BC);
	}

	//Initialize active SubModule
	if(VGAFirstListItem->Functions.initSubModule != NULL)
	{
		(VGAFirstListItem->Functions.initSubModule)(cdv);
	}
	
	return retSuccess;
  }

/***********************************************************************
 * unsigned int zCgosVgaCount(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosVgaCount(CGOS_DRV_VARS *cdv)
  {
	dbgvga(printk("zCgosVgaCount called\n");)	
	if(VGAFirstListItem->Functions.VgaCount != NULL)
	{
		return (VGAFirstListItem->Functions.VgaCount)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosVgaGetInfo(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/    
unsigned int zCgosVgaGetInfo(CGOS_DRV_VARS *cdv)						
  {
	dbgvga(printk("zCgosVgaGetInfo called\n");)
	if(VGAFirstListItem->Functions.VgaGetInfo != NULL)
	{
		return (VGAFirstListItem->Functions.VgaGetInfo)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosVgaGetContrast(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosVgaGetContrast(CGOS_DRV_VARS *cdv)
  {
	dbgvga(printk("zCgosVgaGetContrast called\n");)
	if(VGAFirstListItem->Functions.VgaGetContrast != NULL)
	{
		return (VGAFirstListItem->Functions.VgaGetContrast)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosVgaSetContrast(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosVgaSetContrast(CGOS_DRV_VARS *cdv)
  {
	dbgvga(printk("zCgosVgaSetContrast called\n");)
	if(VGAFirstListItem->Functions.VgaSetContrast != NULL)
	{
		return (VGAFirstListItem->Functions.VgaSetContrast)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosVgaGetContrastEnable(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosVgaGetContrastEnable(CGOS_DRV_VARS *cdv)
  {
	dbgvga(printk("zCgosVgaGetContrastEnable called\n");)	
	if(VGAFirstListItem->Functions.VgaGetContrastEnable != NULL)
	{
		return (VGAFirstListItem->Functions.VgaGetContrastEnable)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosVgaSetContrastEnable(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosVgaSetContrastEnable(CGOS_DRV_VARS *cdv)
  {
	dbgvga(printk("zCgosVgaSetContrastEnable called\n");)
	if(VGAFirstListItem->Functions.VgaSetContrastEnable != NULL)
	{
		return (VGAFirstListItem->Functions.VgaSetContrastEnable)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosVgaGetBacklight(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosVgaGetBacklight(CGOS_DRV_VARS *cdv)
  {
	dbgvga(printk("zCgosVgaGetBacklight called\n");)
	if(VGAFirstListItem->Functions.VgaGetBacklight != NULL)
	{
		return (VGAFirstListItem->Functions.VgaGetBacklight)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosVgaSetBacklight(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosVgaSetBacklight(CGOS_DRV_VARS *cdv)
  {
	dbgvga(printk("zCgosVgaSetBacklight called\n");)	
	if(VGAFirstListItem->Functions.VgaSetBacklight != NULL)
	{
		return (VGAFirstListItem->Functions.VgaSetBacklight)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosVgaGetBacklightEnable(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosVgaGetBacklightEnable(CGOS_DRV_VARS *cdv)
  {
	dbgvga(printk("zCgosVgaGetBacklightEnable called\n");)	
	if(VGAFirstListItem->Functions.VgaGetBacklightEnable != NULL)
	{
		return (VGAFirstListItem->Functions.VgaGetBacklightEnable)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosVgaSetBacklightEnable(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosVgaSetBacklightEnable(CGOS_DRV_VARS *cdv)
  {
	dbgvga(printk("zCgosVgaSetBacklightEnable called\n");)	
	if(VGAFirstListItem->Functions.VgaSetBacklightEnable != NULL)
	{
		return (VGAFirstListItem->Functions.VgaSetBacklightEnable)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}  
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosVgaEndDarkBoot(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosVgaEndDarkBoot(CGOS_DRV_VARS *cdv)
  {
	dbgvga(printk("zCgosVgaEndDarkBoot called\n");)	
	if(VGAFirstListItem->Functions.VgaEndDarkBoot != NULL)
	{
		return (VGAFirstListItem->Functions.VgaEndDarkBoot)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int VgaModuleHandler(CGOS_DRV_FCT *df, 						
 * 								 CGOS_DRV_VARS *cdv)					
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
unsigned int VgaModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv)
{
	dbgvga(printk("VgaModuleHandler called\n");)	
	return (*df->fct)(cdv);
}  

/***********************************************************************
 * void exitVgaModule(CGOS_DRV_VARS *cdv) 							    
 * 																		
 ***********************************************************************
 * Description: This function is called during driver close and should	
 * 				free allocated resources.							
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
void exitVgaModule(CGOS_DRV_VARS *cdv)
{
	VGAListItem *Dummy;
	dbgvga(printk("exitVgaModule called\n");)

	//Call exit function of active SubModule.
	if(VGAFirstListItem->Functions.exitSubModule != NULL)
	{
		(VGAFirstListItem->Functions.exitSubModule)(cdv);
	}

	//Free SubModule list
	while(VGAFirstListItem != NULL)
	{
			Dummy = VGAFirstListItem;
			VGAFirstListItem = VGAFirstListItem->NextItem;
			OsaMemFree(Dummy);
	}
}
