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
#include "I2CModule_cfg.h"
#include "I2CSubModules/I2CSubModule_BC.h"

//***************************************************************************

//#define dbg_i2c_on

#ifdef dbg_i2c_on
    #define dbgi2c(x) x
#else
    #define dbgi2c(x)
#endif

//***************************************************************************
//List of required functions for the I2C Module
typedef struct
{
	CgosFunctionPointer initSubModule;
	CgosFunctionPointer Count;
	CgosFunctionPointer Type;
	CgosFunctionPointer IsAvailable;
	CgosFunctionPointer Read;
	CgosFunctionPointer Write;
	CgosFunctionPointer ReadRegister;
	CgosFunctionPointer WriteRegister;
	CgosFunctionPointer WriteReadCombined;
	CgosFunctionPointer MaxFrequency;
	CgosFunctionPointer GetFrequency;
	CgosFunctionPointer SetFrequency;
	CgosFunctionPointer exitSubModule;
	void *__dummy__ ;/*Required to compile this code with activated gcc constify plugin*/
}I2CModuleFunctionList;

//ListItem for SubModule list
typedef struct I2CListItem
{
	I2CModuleFunctionList Functions;
	unsigned char SubModule;
	struct I2CListItem *NextItem;
}I2CListItem;

//First List Entry
//The first SubModule is considered the "active" one.
I2CListItem *I2CFirstListItem;

//***************************************************************************

/***********************************************************************
 * unsigned int I2CRegisterSubModule(CgosFunctionPointer initSubModule	
 *								 	 CgosFunctionPointer Count				
 *									 CgosFunctionPointer Read				
 *								 	 CgosFunctionPointer Write				
 *								 	 CgosFunctionPointer GetDirection		
 *								 	 CgosFunctionPointer SetDirection;		
 *								 	 CgosFunctionPointer IsAvailable		
 *								 	 CgosFunctionPointer GetDirectionCaps	
 *								 	 CgosFunctionPointer exitSubModule)	
 * 																		
 ***********************************************************************
 * Description: This function is used to register a sub module.			
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int I2CRegisterSubModule(	CgosFunctionPointer initSubModule,
									CgosFunctionPointer Count,
									CgosFunctionPointer Type,
									CgosFunctionPointer IsAvailable,
									CgosFunctionPointer Read,
									CgosFunctionPointer Write,
									CgosFunctionPointer ReadRegister,
									CgosFunctionPointer WriteRegister,
									CgosFunctionPointer WriteReadCombined,
									CgosFunctionPointer MaxFrequency,
									CgosFunctionPointer GetFrequency,
									CgosFunctionPointer SetFrequency,
									CgosFunctionPointer exitSubModule,
									unsigned char ModuleNumber)
{
	I2CListItem *DummyPointer;
	dbgi2c(printk("I2CRegisterSubModule called\n");)
	
	DummyPointer = OsaMemAlloc(sizeof(I2CListItem));
	if(DummyPointer == NULL)
	{
		return CGOS_ERROR;
	}
	
	DummyPointer->Functions.initSubModule = initSubModule;
	DummyPointer->Functions.Count = Count;
	DummyPointer->Functions.Type = Type;
	DummyPointer->Functions.IsAvailable = IsAvailable;
	DummyPointer->Functions.Read = Read;
	DummyPointer->Functions.Write = Write;
	DummyPointer->Functions.ReadRegister = ReadRegister;
	DummyPointer->Functions.WriteRegister = WriteRegister;
	DummyPointer->Functions.WriteReadCombined = WriteReadCombined;
	DummyPointer->Functions.MaxFrequency = MaxFrequency;
	DummyPointer->Functions.GetFrequency = GetFrequency;
	DummyPointer->Functions.SetFrequency = SetFrequency;
	DummyPointer->Functions.exitSubModule = exitSubModule;
	
	DummyPointer->SubModule = ModuleNumber;
	DummyPointer->NextItem = I2CFirstListItem;
	
	I2CFirstListItem = DummyPointer;
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int I2CGetCurrentSubModule(unsigned char *ModuleType)	    
 * 																		
 ***********************************************************************
 * Description: This function is used to get the current SubModule.		
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int I2CGetCurrentSubModule(unsigned char *ModuleType)
{
	dbgi2c(printk("I2CGetCurrentSubModule called\n");)
	*ModuleType = I2CFirstListItem->SubModule;
	return CGOS_SUCCESS;
}
/***********************************************************************
 * unsigned int I2CSetDefaultSubModule(unsigned char DefaultSubModule)   
 * 																		
 ***********************************************************************
 * Description: This function is used to set a SubModule as default.	
 * 																		
 * Last Change: 07.02.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int I2CSetDefaultSubModule(unsigned char DefaultSubModule)
{
	I2CListItem *Dummy1;
	I2CListItem *Dummy2;
	dbgi2c(printk("I2CSetDefaultSubModule called\n");)
	//check if SubModule List is empty
	if(I2CFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(I2CFirstListItem->SubModule == DefaultSubModule)
		{
			return CGOS_SUCCESS;
		}
		else
		{
			Dummy1 = I2CFirstListItem->NextItem;
			Dummy2 = I2CFirstListItem;
			
			//Search SubModule list for requested SubModule
			while(Dummy1 != NULL)
			{
				//If found, change to active
				if(Dummy1->SubModule == DefaultSubModule)
				{
					Dummy2->NextItem = Dummy1->NextItem;
					Dummy1->NextItem = I2CFirstListItem;
					I2CFirstListItem = Dummy1;
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
				dbgi2c(printk("I2CSetDefaultSubModule error: SubModule not found");)
				return CGOS_ERROR;
			}
		}
	}
	else
	{
		dbgi2c(printk("I2CSetDefaultSubModule error: No SubModule available");)
		return CGOS_ERROR;
	}
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int I2CChangeSubModule(CGOS_DRV_VARS *cdv,					
 * 								   unsigned char NewSubModule)		    
 * 																		
 ***********************************************************************
 * Description: This function changes the used SubModule				
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 * 	- 																	
 ***********************************************************************/
unsigned int I2CChangeSubModule(CGOS_DRV_VARS *cdv, unsigned char NewSubModule)
{
	I2CListItem *Dummy1;
	I2CListItem *Dummy2;
	unsigned int retSuccess = CGOS_SUCCESS;
	
	dbgi2c(printk("I2CChangeSubModule called\n");)
	
	//check if SubModule List is empty
	if(I2CFirstListItem != NULL)
	{
		//check if requested SubModule is already active
		if(NewSubModule != I2CFirstListItem->SubModule)	
		{
			//Exit the current SubModule
			if(I2CFirstListItem->Functions.exitSubModule != NULL)
			{
			retSuccess = (I2CFirstListItem->Functions.exitSubModule)(cdv);
			}
	
			//Change IOSubModule
			if(I2CFirstListItem->NextItem != NULL)
			{
				Dummy1 = I2CFirstListItem->NextItem;
				Dummy2 = I2CFirstListItem;
			
				while(Dummy1 != NULL)
				{
					if(Dummy1->SubModule == NewSubModule)
					{
						Dummy2->NextItem = Dummy1->NextItem;
						Dummy1->NextItem = I2CFirstListItem;
						I2CFirstListItem = Dummy1;
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
			if((I2CFirstListItem->Functions.initSubModule != NULL) && (retSuccess == CGOS_SUCCESS))
			{
			retSuccess = (I2CFirstListItem->Functions.initSubModule)(cdv);
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
 * unsigned int initI2CModule(CGOS_DRV_VARS *cdv)                       
 *                                                                      
 ***********************************************************************
 * Description: This function is called to copy information from        
 *              I2CModule_cfg.h into the cdv struct.                    
 *                                                                      
 * Last Change: 12.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/
unsigned int initI2CModule(CGOS_DRV_VARS *cdv)
  {
    unsigned int retSuccess = CGOS_SUCCESS;
    dbgi2c(printk("initI2CModule called\n");)
    
    //Initialize SubModuele list
	I2CFirstListItem = NULL;
	
	//Register SubModule function pointers via callback.
	//I2C_SUBMODULE_* enum in DrvVars.h.
	RegisterI2CSubModule_BC(I2CRegisterSubModule,I2C_SUBMODULE_BC);
	
	//Set default SubModule.
	switch(cdv->brd->BoardGroup)
	{
		case GROUP_DEFAULT:
		default: 				I2CSetDefaultSubModule(I2C_SUBMODULE_BC);
	}

	//Initialize active SubModule
	if(I2CFirstListItem->Functions.initSubModule != NULL)
	{
		(I2CFirstListItem->Functions.initSubModule)(cdv);
	}
	
	return retSuccess;
  }

/***********************************************************************
 * unsigned int zCgosI2CCount(CGOS_DRV_VARS *cdv)                       
 *                                                                      
 ***********************************************************************
 * Cgos Function: CgosI2CCount(HCGOS hCgos)                             
 *                                                                      
 * Inputs:                                                              
 *   -                                                                  
 *                                                                      
 * Output:                                                              
 * cdv->cout->rets[0]                                                   
 *                                                                     
 ***********************************************************************
 * Description: Gets the i2c count.                                     
 *                                                                      
 * Last Change: 06.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/ 
unsigned int zCgosI2CCount(CGOS_DRV_VARS *cdv)
  {
    dbgi2c(printk("zCgosI2CCount called\n");)   
	if(I2CFirstListItem->Functions.Count != NULL)
	{
		return (I2CFirstListItem->Functions.Count)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }
 
/***********************************************************************
 * unsigned int zCgosI2CIsAvailable(CGOS_DRV_VARS *cdv)                 
 *                                                                      
 ***********************************************************************
 * Cgos Function: CgosI2CIsAvailable(HCGOS hCgos,                       
 *                                   unsigned long dwUnit)              
 *                                                                      
 * Inputs:                                                              
 * dwUnit <->   cdv->cin->type                                          
 *                                                                      
 * Output:                                                              
 * cdv->cout->rets[0]                                                   
 *                                                                      
 ***********************************************************************
 * Description: See if i2c dwUnit is available                          
 *                                                                      
 * Last Change: 06.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/ 
unsigned int zCgosI2CIsAvailable(CGOS_DRV_VARS *cdv)
  {
    dbgi2c(printk("zCgosI2CIsAvailable called\n");)
	if(I2CFirstListItem->Functions.IsAvailable != NULL)
	{
		return (I2CFirstListItem->Functions.IsAvailable)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  } 

/***********************************************************************
 * unsigned int zCgosI2CType(CGOS_DRV_VARS *cdv)                        
 *                                                                      
 ***********************************************************************
 * Cgos Function: CgosI2CType(HCGOS hCgos,                              
 *                            unsigned long dwUnit)                    
 *                                                                      
 * Inputs:                                                              
 *   -                                                                  
 *                                                                      
 * Output:                                                              
 * cdv->cout->rets[0]                                                  
 *                                                                      
 ***********************************************************************
 * Description: Gets the cgos type of dwUnit                            
 *                                                                      
 * Last Change: 06.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/ 
unsigned int zCgosI2CType(CGOS_DRV_VARS *cdv)
  { 
    dbgi2c(printk("zCgosI2CType called\n");)
	if(I2CFirstListItem->Functions.Type != NULL)
	{
		return (I2CFirstListItem->Functions.Type)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosI2CRead(CGOS_DRV_VARS *cdv)                        
 *                                                                      
 ***********************************************************************
 * Cgos Function: CgosI2CRead(HCGOS hCgos,                              
 *                            unsigned long dwUnit,                     
 *                            unsigned char bAddr,                      
 *                            unsigned char *pBytes,                    
 *                            unsigned long dwLen                       
 * Inputs:                                                              
 * dwUnit       <->     cdv->cin->type                                 
 * bAddr        <->     cdv->cin->pars[0]                               
 * dwLen        <->     cdv->lout                                      
 *                                                                      
 * Output:                                                              
 * pBytes       <->     (unsigned char*)cdv->pout                       
 *                                                                      
 ***********************************************************************
 * Description: Read dwLen bytes from bAddr at bus dwUnit.              
 *                                                                      
 * Last Change: 24.01.2018 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/ 
 
unsigned int zCgosI2CRead(CGOS_DRV_VARS *cdv)
  {
    dbgi2c(printk("zCgosI2CRead called\n");)
	if(I2CFirstListItem->Functions.Read != NULL)
	{
		return (I2CFirstListItem->Functions.Read)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosI2CWrite(CGOS_DRV_VARS *cdv)                       
 *                                                                      
 ***********************************************************************
 * Cgos Function: CgosI2CWrite(HCGOS hCgos,                             
 *                             unsigned long dwUnit,                    
 *                             unsigned char bAddr,                     
 *                             unsigned char *pBytes,                   
 *                             unsigned long dwLen                      
 * Inputs:                                                              
 * dwUnit       <->     cdv->cin->type                                  
 * bAddr        <->     cdv->cin->pars[0]                               
 * pBytes       <->     cdv->pin                                        
 * dwLen        <->     cdv->nInBufferSize - sizeof(CGOSIOCTLIN)        
 *                                                                      
 * Outputs:                                                             
 *   -                                                                  
 *                                                                      
 ***********************************************************************
 * Description: Write dwLen bytes to bAddr at dwUnit.                   
 *                                                                      
 * Last Change: 06.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/ 
unsigned int zCgosI2CWrite(CGOS_DRV_VARS *cdv)
  {
    dbgi2c(printk("zCgosI2CWrite called\n");)
    if(I2CFirstListItem->Functions.Write != NULL)
	{
		return (I2CFirstListItem->Functions.Write)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosI2CReadRegister(CGOS_DRV_VARS *cdv)                
 *                                                                      
 ***********************************************************************
 * Cgos Function: CgosI2CReadRegister(HCGOS hCgos,                      
 *                                    unsigned long dwUnit,             
 *                                    unsigned char bAddr,              
 *                                    unsigned short wReg,              
 *                                    unsigned char *pDataByte          
 * Inputs:                                                              
 * dwUnit       <->     cdv->cin->type                                  
 * bAddr        <->     cdv->cin->pars[0]                               
 * wReg         <->     cdv->cin->pars[1]                               
 *                                                                      
 * Output:                                                              
 * pData Byte   <->     cdv->cout->rets[0]                              
 *                                                                      
 ***********************************************************************
 * Description: Reads one byte from a register.                         
 *                                                                      
 * Last Change: 06.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/ 
unsigned int zCgosI2CReadRegister(CGOS_DRV_VARS *cdv)
  { 
    dbgi2c(printk("zCgosI2CReadRegister called\n");)    
    if(I2CFirstListItem->Functions.ReadRegister != NULL)
	{
		return (I2CFirstListItem->Functions.ReadRegister)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosI2CWriteRegister(CGOS_DRV_VARS *cdv)               
 *                                                                      
 ***********************************************************************
 * Cgos Function: CgosI2CWriteRegister(HCGOS hCgos,                     
 *                                     unsigned long dwUnit,            
 *                                     unsigned char bAddr,             
 *                                     unsigned short wReg,             
 *                                     unsigned char bData             
 * Inputs:                                                              
 * dwUnit       <->     cdv->cin->type                                  
 * bAddr        <->     cdv->cin->pars[0]                               
 * wReg         <->     cdv->cin->pars[1]                               
 * bData        <->     cdv->cin->pars[2]                               
 *                                                                      
 * Outputs:                                                             
 *   -                                                                  
 *                                                                      
 ***********************************************************************
 * Description: Writes one byte to a register.                          
 *                                                                      
 * Last Change: 06.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/ 
unsigned int zCgosI2CWriteRegister(CGOS_DRV_VARS *cdv)
  { 
    dbgi2c(printk("zCgosI2CWriteRegister called\n");)
    if(I2CFirstListItem->Functions.WriteRegister != NULL)
	{
		return (I2CFirstListItem->Functions.WriteRegister)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosI2CWriteReadCombined(CGOS_DRV_VARS *cdv)           
 *                                                                      
 ***********************************************************************
 * Cgos Function: CgosI2CWriteReadCombined(HCGOS hCgos,                 
 *                                         unsigned long dwUnit,        
 *                                         unsigned char bAddr,         
 *                                         unsigned char *pBytesWrite,  
 *                                         unsigned long dwLenWrite,    
 *                                         unsigned char *pBytesRead,   
 *                                         unsigned long dwLenRead)     
 * Inputs:                                                             
 * dwUnit       <->     cdv->cin->type                                  
 * bAddr        <->     cdv->cin->pars[0]                               
 * pBytesWrite  <->     cdv->lin                                        
 * dwLenWrite   <->     cdv->lin                                        
 * dwLenRead    <->     cdv->lout                                      
 *                                                                      
 * Output:                                                              
 * pBytesRead   <->     (unsigned char*)cdv->pout                       
 *                                                                      
 ***********************************************************************
 * Description: Execute a combined write and read action.               
 *                                                                      
 * Last Change: 06.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/  
unsigned int zCgosI2CWriteReadCombined(CGOS_DRV_VARS *cdv)
  { 
    dbgi2c(printk("zCgosI2CWriteReadCombined called\n");)
    if(I2CFirstListItem->Functions.WriteReadCombined != NULL)
	{
		return (I2CFirstListItem->Functions.WriteReadCombined)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }
  
/***********************************************************************
 * unsigned int zCgosI2CGetMaxFrequency(CGOS_DRV_VARS *cdv)             
 *                                                                     
 ***********************************************************************
 * Cgos Function: CgosI2CGetMaxFrequency(HCGOS hCgos,                   
 *                                       unsigned long dwUnit,          
 *                                       unsigned long *pdwSetting)     
 * Inputs:                                                              
 *   -                                                                  
 *                                                                      
 * Output:                                                             
 * pdwSetting   <->     cdv->cout->rets[0]                              
 *                                                                     
 ***********************************************************************
 * Description: Gets the maximum i2c frequency.                         
 *                                                                      
 * Last Change: 06.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/  
unsigned int zCgosI2CGetMaxFrequency(CGOS_DRV_VARS *cdv)
  {
    dbgi2c(printk("zCgosI2CGetMaxFrequency called\n");) 
    if(I2CFirstListItem->Functions.MaxFrequency != NULL)
	{
		return (I2CFirstListItem->Functions.MaxFrequency)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosI2CGetFrequency(CGOS_DRV_VARS *cdv)                
 *                                                                      
 ***********************************************************************
 * Cgos Function: CgosI2CGetFrequency(HCGOS hCgos,                      
 *                                    unsigned long dwUnit,             
 *                                    unsigned long *pdwSetting)        
 * Input:                                                               
 * dwUnit       <->     cdv->cin->type;                                
 *                                                                      
 * Output:                                                              
 * pdwSetting   <->     cdv->cout->rets[0]                              
 *                                                                      
 ***********************************************************************
 * Description: Get the current i2c frequency.                          
 *                                                                      
 * Last Change: 06.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/  
unsigned int zCgosI2CGetFrequency(CGOS_DRV_VARS *cdv)               
  {
    dbgi2c(printk("zCgosI2CGetFrequency called\n");)
    if(I2CFirstListItem->Functions.GetFrequency != NULL)
	{
		return (I2CFirstListItem->Functions.GetFrequency)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int zCgosI2CSetFrequency(CGOS_DRV_VARS *cdv)                
 *                                                                      
 ***********************************************************************
 * Cgos Function: CgosI2CSetFrequency(HCGOS hCgos,                      
 *                                    unsigned long dwUnit,             
 *                                    unsigned long pdwSetting)         
 * Inputs:                                                              
 * dwUnit       <->     cdv->cin->type;                                
 * pdwSetting   <->     cdv->cout->pars[0];                             
 *                                                                      
 * Outputs:                                                             
 *   -                                                                  
 *                                                                      
 ***********************************************************************
 * Description: Set i2c frequency to pdwSetting.                       
 *                                                                      
 * Last Change: 06.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/  
unsigned int zCgosI2CSetFrequency(CGOS_DRV_VARS *cdv)                   
  {
    dbgi2c(printk("zCgosI2CSetFrequency called\n");)
    if(I2CFirstListItem->Functions.SetFrequency!= NULL)
	{
		return (I2CFirstListItem->Functions.SetFrequency)(cdv);
	}
	else
	{
		return CGOS_NOT_IMPLEMENTED;
	}
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int I2CModuleHandler(CGOS_DRV_FCT *df,                      
 *                               CGOS_DRV_VARS *cdv)                    
 *                                                                      
 ***********************************************************************
 * Description: This function is called by the module dispatcher and    
 *              calls the appropriate module function for the requested 
 *              function call. This function is a defined entry point   
 *              to the module and can be used for module specific      
 *              parameter checking.                                     
 *                                                                      
 * Last Change: 12.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/
unsigned int I2CModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv)
{
    dbgi2c(printk("I2CModuleHandler called\n");)
    return (*df->fct)(cdv);
}

/***********************************************************************
 * void exitI2CModule(CGOS_DRV_VARS *cdv)                               
 *                                                                      
 ***********************************************************************
 * Description: This function is called during driver close and should  
 *              free allocated resources.                               
 *                                                                      
 * Last Change: 12.12.2017 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/
void exitI2CModule(CGOS_DRV_VARS *cdv)
{
	I2CListItem *Dummy;
	dbgi2c(printk("exitI2CModule called\n");)
	
	//Call exit function of active SubModule.
	if(I2CFirstListItem->Functions.exitSubModule != NULL)
	{
		(I2CFirstListItem->Functions.exitSubModule)(cdv);
	}

	//Free SubModule list
	while(I2CFirstListItem != NULL)
	{
			Dummy = I2CFirstListItem;
			I2CFirstListItem = I2CFirstListItem->NextItem;
			OsaMemFree(Dummy);
	}
}
