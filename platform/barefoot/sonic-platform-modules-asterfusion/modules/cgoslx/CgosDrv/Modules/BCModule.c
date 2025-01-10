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
#include <linux/ioport.h> 
#include <linux/dmi.h>
#include <asm/io.h>
#endif

//#include "CGBC.h"
#include "CgosDrv.h"
#include "BCModule_cfg.h"
#include "BCSubModules/BCSubModule_BC4_BASE_E.h"
#include "BCSubModules/BCSubModule_BC4_BASE_C.h"
#include "BCSubModules/BCSubModule_GEN5.h"

//***************************************************************************

//#define dbg_bc_on

#ifdef dbg_bc_on
	#define dbgbc(x) x
#else
	#define dbgbc(x)
#endif

//***************************************************************************

typedef struct {
  unsigned int xfct;
  unsigned int (*fct)(CGOS_DRV_VARS *cdv);
  unsigned int minin;
  unsigned int minout;
  unsigned int flags;
  } CGOS_DRV_FCT;

//***************************************************************************
//List of required functions for the BC Module
typedef struct
{
	CgosFunctionPointer initSubModule;
	CgosFunctionPointer Probe;
	CgosCommandPointer BCCommand;
	CgosFunctionPointer GetInfo;
	CgosFunctionPointer SetControl;
	CgosFunctionPointer ReadWrite;	
	CgosFunctionPointer exitSubModule;
	void *__dummy__; /*Required to compile this code with activated gcc constify plugin*/
}BCModuleFunctionList;


//ListItem for SubModule list
typedef struct BCListItem
{
	BCModuleFunctionList Functions;
	unsigned char SubModule;
	struct BCListItem *NextItem;
}BCListItem;

//First List Entry
//The first SubModule is considered the "active" one.
BCListItem *BCFirstListItem;

//***************************************************************************


/***********************************************************************
 * unsigned int BCRegisterSubModule(CgosFunctionPointer initSubModule,
 *                                  CgosFunctionPointer	Probe,
 * 									CgosCommandPointer BCCommand,
 *									CgosFunctionPointer GetInfo,
 *								    CgosFunctionPointer SetControl,
 *									CgosFunctionPointer ReadWrite,
 * 									CgosFunctionPointer exitSubModule,
 * 									unsigned char ModuleNumber)	
 * 																		
 ***********************************************************************
 * Description: This function is used to register a sub module.			
 * 																		
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int BCRegisterSubModule(CgosFunctionPointer initSubModule,
								 CgosFunctionPointer Probe,
								 CgosCommandPointer BCCommand,
							 	 CgosFunctionPointer GetInfo,
							     CgosFunctionPointer SetControl,
								 CgosFunctionPointer ReadWrite,
								 CgosFunctionPointer exitSubModule,
 								 unsigned char ModuleNumber)
{
	BCListItem *DummyPointer;
	dbgbc(printk("BCRegisterSubModule called\n");)
	
	DummyPointer = OsaMemAlloc(sizeof(BCListItem));
	if(DummyPointer == NULL)
	{
		return CGOS_ERROR;
	}
	
	DummyPointer->Functions.initSubModule = initSubModule;
	DummyPointer->Functions.Probe = Probe;
	DummyPointer->Functions.BCCommand = BCCommand;
	DummyPointer->Functions.GetInfo = GetInfo;
	DummyPointer->Functions.SetControl = SetControl;
	DummyPointer->Functions.ReadWrite = ReadWrite;
	DummyPointer->Functions.exitSubModule = exitSubModule;
	
	DummyPointer->SubModule = ModuleNumber;
	DummyPointer->NextItem = BCFirstListItem;
	
	BCFirstListItem = DummyPointer;
	
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int BCGetCurrentSubModule(unsigned char *ModuleType)	    
 * 																		
 ***********************************************************************
 * Description: This function is used to get the current SubModule.		
 * 																		
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int BCGetCurrentSubModule(unsigned char *ModuleType)
{
	dbgbc(printk("BCGetCurrentSubModule called\n");)
	*ModuleType = BCFirstListItem->SubModule;
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int BCSetDefaultSubModule(unsigned char DefaultSubModule)   
 * 																		
 ***********************************************************************
 * Description: This function is used to set a SubModule as default.	
 * 																		
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int BCSetDefaultSubModule(unsigned char DefaultSubModule)
{
	BCListItem *Dummy1;
	BCListItem *Dummy2;
	dbgbc(printk("BCSetDefaultSubModule called\n");)
	//check if SubModule List is empty
	if(BCFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(BCFirstListItem->SubModule == DefaultSubModule)
		{
			return CGOS_SUCCESS;
		}
		else
		{
			Dummy1 = BCFirstListItem->NextItem;
			Dummy2 = BCFirstListItem;
			
			//Search SubModule list for requested SubModule
			while(Dummy1 != NULL)
			{
				//If found, change to active
				if(Dummy1->SubModule == DefaultSubModule)
				{
					Dummy2->NextItem = Dummy1->NextItem;
					Dummy1->NextItem = BCFirstListItem;
					BCFirstListItem = Dummy1;
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
				dbgbc(printk("BCSetDefaultSubModule error: SubModule not found");)
				return CGOS_ERROR;
			}
		}
	}
	else
	{
		dbgbc(printk("BCSetDefaultSubModule error: No SubModule available");)
		return CGOS_ERROR;
	}
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int BCChangeSubModule(CGOS_DRV_VARS *cdv,					
 * 								  unsigned char NewSubModule)		    
 * 																		
 ***********************************************************************
 * Description: This function changes the used SubModule				
 * 																		
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int BCChangeSubModule(CGOS_DRV_VARS *cdv, unsigned char NewSubModule)
{
	BCListItem *Dummy1;
	BCListItem *Dummy2;
	unsigned int retSuccess = CGOS_SUCCESS;
	
	dbgbc(printk("BCChangeSubModule called\n");)
	
	//check if SubModule List is empty
	if(BCFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(NewSubModule != BCFirstListItem->SubModule)	
		{
			//Exit the current SubModule
			if(BCFirstListItem->Functions.exitSubModule != NULL)
			{
			retSuccess = (BCFirstListItem->Functions.exitSubModule)(cdv);
			}
	
			//Change SubModule
			if(BCFirstListItem->NextItem != NULL)
			{
				Dummy1 = BCFirstListItem->NextItem;
				Dummy2 = BCFirstListItem;
			
				while(Dummy1 != NULL)
				{
					if(Dummy1->SubModule == NewSubModule)
					{
						Dummy2->NextItem = Dummy1->NextItem;
						Dummy1->NextItem = BCFirstListItem;
						BCFirstListItem = Dummy1;
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
			if((BCFirstListItem->Functions.initSubModule != NULL) && (retSuccess == CGOS_SUCCESS))
			{
			retSuccess = (BCFirstListItem->Functions.initSubModule)(cdv);
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
 * int Comp(char *comp1, char *comp2) 					    
 * 																		
 ***********************************************************************
 * Description: 					
 * 																		
 * Last Change: 10-Nov-2020 Hoffmann Michael											
 * 																		
 * Modifications: 													
 ***********************************************************************/ 
 int Comp(char *comp1, char *comp2)
 {
 	int i;
 	
 	for(i = 0; i < 4; i++)
 	{
 		if(((comp1 + i) == NULL) || ((comp2 + i) == NULL))
 		{
 			return -1;
 		} 
 		if(*(comp1 + i) != *(comp2 + i))
 		{
 			return 0;
 		}
 	}
 	return 1;
 }
 
 /***********************************************************************
 * unsigned int GetBoardControllerType(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description: 					
 * 																		
 * Last Change: 10-Nov-2020 Hoffmann Michael										
 * 																		
 * Modifications: 													
 ***********************************************************************/
  unsigned int GetBoardControllerType(CGOS_DRV_VARS *cdv)
 {
 #ifdef __linux__
	unsigned char Letter;
	const char *value;
	char Board[4] = {0};
	int i;

	//Get Project Code	
	value = dmi_get_system_info(DMI_BIOS_VERSION);
	for(i = 0; i < 4; i++)
	{
		if ((value + i) != NULL)		//MOD23 v
		{
			Letter = *(value + i);
			if ((Letter >= 0x30) && (Letter <= 0x39)) //Letter is ASCII Number
			{
				Board[i] = Letter;
				cdv->brd->BoardName[i] = Letter;
			}
			else if ((Letter >= 0) && (Letter <= 9)) //Letter is int Number
			{
				//convert to ASCII
				Board[i] = Letter + 0x30; 
				cdv->brd->BoardName[i] = Letter + 0x30; 
			}
			else //Letter is no number
			{
				Board[i] = *(value + i);
				cdv->brd->BoardName[i] = *(value + i); //CGOSBOARDINFOs Project Code field (information from BC) could be used instead if smbios information
			}
		}								//MOD23 ^
		else
		{
			return CGOS_ERROR;
		}
		
	}
	
	//Check if BC4 Base E
	for(i = 0;i < BC4_BASE_E_COUNT; i++)
	{
		if(Comp(&(BC4_BASE_E_LIST[i].Letters[0]),&Board[0]) == 1)
		{
			cdv->brd->BC_Type = BC_TYPE_BC4_BASE_E;
			return CGOS_SUCCESS;
		}
	}

	//Check if BC4 Base C
	for(i = 0;i < BC4_BASE_C_COUNT; i++)
	{
		if(Comp(&(BC4_BASE_C_LIST[i].Letters[0]),&Board[0]) == 1)
		{
			cdv->brd->BC_Type = BC_TYPE_BC4_BASE_C;
			return CGOS_SUCCESS;
		}
	}

	//Check if BC GEN5
	for(i = 0;i < GEN5_COUNT; i++)
	{
		if(Comp(&(GEN5_LIST[i].Letters[0]),&Board[0]) == 1)
		{
			cdv->brd->BC_Type = BC_TYPE_GEN5;
			return CGOS_SUCCESS;
		}
	}
#endif
 	return CGOS_ERROR;
 }
 
/***********************************************************************
 * unsigned int initBCModule(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description: 					
 * 																		
 * Last Change: 23.09.2019 HMI											
 * 																		
 * Modifications: 													
 ***********************************************************************/
unsigned int initBCModule(CGOS_DRV_VARS *cdv)
  {
	unsigned int retSuccess = CGOS_SUCCESS;
	dbgbc(printk("initBCModule called\n");)
	
	//Initialize SubModuele list
	BCFirstListItem = NULL;
	
	//Register SubModule function pointers via callback.
	//BC_SUBMODULE_* enum in DrvVars.h.
	RegisterBCSubModule_BC4_BASE_E(BCRegisterSubModule,BC_SUBMODULE_BC4_BASE_E);
	RegisterBCSubModule_BC4_BASE_C(BCRegisterSubModule,BC_SUBMODULE_BC4_BASE_C);
	RegisterBCSubModule_GEN5(BCRegisterSubModule,BC_SUBMODULE_GEN5);

//MOD23 v
#ifdef __linux__	
	GetBoardControllerType(cdv);
#else
	//cdv->brd->BC_Type = BC_TYPE_BC4_BASE_C;
	//cdv->brd->BC_Type = BC_TYPE_BC4_BASE_E;
	cdv->brd->BC_Type = BC_TYPE_GEN5;
#endif

		switch(cdv->brd->BC_Type)
		{
			case BC_TYPE_BC4_BASE_C:	BCSetDefaultSubModule(BC_SUBMODULE_BC4_BASE_C);
							break;
			case BC_TYPE_BC4_BASE_E:	BCSetDefaultSubModule(BC_SUBMODULE_BC4_BASE_E);
							break;
			case BC_TYPE_GEN5:			BCSetDefaultSubModule(BC_SUBMODULE_GEN5);
							break;
			default:					cdv->brd->BC_Type = BC_TYPE_GEN5; //In case no BC Type was found, default to GEN5
										BCSetDefaultSubModule(BC_SUBMODULE_GEN5);
		}
//MOD23 ^

	//Initialize active SubModule
	if(retSuccess == CGOS_SUCCESS)
	{
		if(BCFirstListItem->Functions.initSubModule != NULL)
		{
			retSuccess = (BCFirstListItem->Functions.initSubModule)(cdv);
		}
	}

	return retSuccess;
  }
/***********************************************************************
 * unsigned int bcCommand(unsigned char *cmdDataBPtr,					
 *						  unsigned char  cmdByteCount,					
 *					      unsigned char *rcvDataBPtr,					
 *					      unsigned char  rcvByteCount,					
 *					      unsigned char *retValueBPtr )	  				
 * 																		
 * Inputs:																
 * 	cmdDataBPtr															
 * 	cmdByteCount														
 * 	rcvByteCount														
 * 																		
 * Outputs:																
 * 	rcvDataBPtr															
 * 	retValueBPtr														
 * 																		
 * Return:																
 * 	CGOS_SUCCESS or CGOS_ERROR											
 * 																		
 ***********************************************************************
 * Description: This function is used to communicate with the board		
 * 				controller												
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int bcCommand(unsigned char *cmdDataBPtr, unsigned char  cmdByteCount,
					   unsigned char *rcvDataBPtr, unsigned char  rcvByteCount,
					   unsigned char *retValueBPtr )
{
	if(BCFirstListItem->Functions.BCCommand != NULL)
	{
		return (BCFirstListItem->Functions.BCCommand)(cmdDataBPtr, cmdByteCount, rcvDataBPtr,
													  rcvByteCount, retValueBPtr);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
}

/***********************************************************************
 * unsigned int zCgosCgbcGetInfo(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosCgbcGetInfo(CGOS_DRV_VARS *cdv)						
  {    
	dbgbc(printk("zCgosCgbcGetInfo called\n");)	
	if(BCFirstListItem->Functions.GetInfo != NULL)
	{
		return (BCFirstListItem->Functions.GetInfo)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosCgbcSetControl(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int zCgosCgbcSetControl(CGOS_DRV_VARS *cdv)					
  { 
	dbgbc(printk("zCgosCgbcSetControl called\n");)	
	if(BCFirstListItem->Functions.SetControl != NULL)
	{
		return (BCFirstListItem->Functions.SetControl)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosCgbcReadWrite(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosCgbcReadWrite(CGOS_DRV_VARS *cdv)					
  { 
	dbgbc(printk("zCgosCgbcReadWrite called\n");)	
	if(BCFirstListItem->Functions.ReadWrite != NULL)
	{
		return (BCFirstListItem->Functions.ReadWrite)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosCgbcHandleCommand(CGOS_DRV_VARS *cdv)			    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosCgbcHandleCommand(HCGOS hCgos, 				
 * 										unsigned char *pBytesWrite, 	
 * 										unsigned int dwLenWrite, 		
 * 										unsigned char *pBytesRead, 		
 * 										unsigned int dwLenRead, 		
 * 										unsigned int *pdwStatus)		
 *																		
 * Inputs:																
 * 	pBytesWrite		<->		cdv->pin									
 * 	dwLenWrite		<->		cdv->lin									
 * 	dwLenRead		<->		cdv->lout									
 * 																		
 * Outputs:																
 * 	pBytesRead		<->		cdv->pout									
 * 	pdwStatus		<->		cdv->cout->rets[0]							
 * 																		
 * Return:																
 * 	CGOS_SUCCESS														
 * 																		
 ***********************************************************************
 * Description: This function is used to communicate with the board		
 * 				controller.												
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int zCgosCgbcHandleCommand(CGOS_DRV_VARS *cdv)	
{
	unsigned char ret = 0;
	dbgbc(printk("zCgosCgbcHandleCommand called\n");)
	bcCommand(cdv->pin,cdv->lin,cdv->pout,cdv->lout,&ret);
	cdv->cout->rets[0] = ret;
	cdv->retcnt += cdv->lout;
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int BCModuleHandler(CGOS_DRV_FCT *df, 						
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
unsigned int BCModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv)
{
	dbgbc(printk("BCModuleHandler called\n");)
	return (*df->fct)(cdv);	
}

/***********************************************************************
 * void exitBCModule(CGOS_DRV_VARS *cdv) 							    
 * 																		
 ***********************************************************************
 * Description: This function is called during driver close and should	
 * 				free allocated resources.							
 * 																		
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
void exitBCModule(CGOS_DRV_VARS *cdv)
{
	BCListItem *Dummy;
	dbgbc(printk("exitBCModule called\n");)

	//Call exit function of active SubModule.
	if(BCFirstListItem->Functions.exitSubModule != NULL)
	{
		(BCFirstListItem->Functions.exitSubModule)(cdv);
	}

	//Free SubModule list
	while(BCFirstListItem != NULL)
	{
			Dummy = BCFirstListItem;
			BCFirstListItem = BCFirstListItem->NextItem;
			OsaMemFree(Dummy);
	}
}

