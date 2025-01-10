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
#include <linux/delay.h>
#endif

#include "CgosDrv.h"
#include "CGBC.h"
#include "../I2CSubModules/I2CSubModule_BC.h"
#ifdef _WIN64
#include <windows.h>
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
#include "StoSubModule_BC_cfg.h"

//***************************************************************************

//#define dbg_sto_sub_bc_on

#ifdef dbg_sto_sub_bc_on
#ifdef __linux__
#define dbgsto(x) printk(x)
#elif _WIN64
#define dbgsto(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#elif _WIN32
#define dbgsto(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#endif
#else
#define dbgsto(x)
#endif

//***************************************************************************
// Function prototypes for register function

unsigned int initStoSubModule_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosStorageAreaCount_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosStorageAreaType_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosStorageAreaSize_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosStorageAreaBlockSize_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosStorageAreaRead_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosStorageAreaWrite_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosStorageAreaErase_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosStorageAreaEraseStatus_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosStorageAreaLock_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosStorageAreaUnlock_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosStorageAreaIsLocked_BC(CGOS_DRV_VARS *cdv);
unsigned int exitStoSubModule_BC(CGOS_DRV_VARS *cdv);

//***************************************************************************
//Helper Functions

unsigned int power(unsigned int x, unsigned int y)
{
	if(y == 0){return 1;}
	else
	{
		return x*power(x,y-1);
	}
}

#ifndef __linux__
void msleep(int ms)
{
}
#endif

//***************************************************************************

/***********************************************************************
 * unsigned int RegisterStoSubModule_BC									
 * 																		
 ***********************************************************************
 * Description: This function uses the RegisterSubModule callback to	
 * 			  	register the SubModule functions.						
 * 																		
 * Last Change: 24.03.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int RegisterStoSubModule_BC(unsigned int (*RegisterSubModule)(	CgosFunctionPointer initSubModule,				    
																	    CgosFunctionPointer count,
 								                                        CgosFunctionPointer type,
								                                        CgosFunctionPointer size,
 								                                        CgosFunctionPointer blockSize,
 								                                        CgosFunctionPointer read,
 								                                        CgosFunctionPointer write,
 								                                        CgosFunctionPointer erase,
 								                                        CgosFunctionPointer erasestatus,
 								                                        CgosFunctionPointer lock,
 								                                        CgosFunctionPointer unlock,
 								                                        CgosFunctionPointer islocked,
 								                                        CgosFunctionPointer exitSubModule,
																		unsigned char ModuleNumber),
								                    unsigned char Number)
{
	return RegisterSubModule(	initStoSubModule_BC,
								CgosStorageAreaCount_BC,
								CgosStorageAreaType_BC,
								CgosStorageAreaSize_BC,
								CgosStorageAreaBlockSize_BC,
								CgosStorageAreaRead_BC,
								CgosStorageAreaWrite_BC,
								CgosStorageAreaErase_BC,
								CgosStorageAreaEraseStatus_BC,
								CgosStorageAreaLock_BC,
								CgosStorageAreaUnlock_BC,
								CgosStorageAreaIsLocked_BC,
								exitStoSubModule_BC,
								Number);
	
}

/***********************************************************************
 * unsigned int initStoSubModule(CGOS_DRV_VARS *cdv) 					    
 * 																	
 ***********************************************************************
 * Description: Scans for available storage areas and provides the 
 * 		information to the driver.										
 * 																	
 * Last Change: 24.03.2020 HMI											
 * 																		
 * Modifications:														
 * 	-																	
 ***********************************************************************/
unsigned int initStoSubModule_BC(CGOS_DRV_VARS *cdv)
{
	unsigned char writeBuf;
	unsigned char readBuf[14];
	unsigned char stat;
    
	dbgsto("initStoSubModule called\n");
	
	cdv->brd->stoCount = 0;
	cdv->brd->stoPrivCount = 0;
	
	writeBuf = CGBC_CMD_INFO_1;
	if(!bcCommand(&writeBuf,sizeof(writeBuf),&readBuf[0],sizeof(readBuf),&stat))
	{
		if(readBuf[6] != 0)	//Secure Data EEPROM supported
		{
			cdv->brd->sto[cdv->brd->stoCount].info.size = sizeof(CGEB_STORAGEAREA_INFO);
			cdv->brd->sto[cdv->brd->stoCount].info.type = CGOS_STORAGE_AREA_SDA;
			cdv->brd->sto[cdv->brd->stoCount].info.flags = 0x01; //Default is locked
			cdv->brd->sto[cdv->brd->stoCount].info.areaSize = power(2,(readBuf[6] & 0x1F));
			cdv->brd->sto[cdv->brd->stoCount].info.blockSize = 0;
			cdv->brd->stoCount++;
		}
		
		if(readBuf[8] != 0) //User EEPROM supported
		{
			cdv->brd->sto[cdv->brd->stoCount].info.size = sizeof(CGEB_STORAGEAREA_INFO);
			cdv->brd->sto[cdv->brd->stoCount].info.type = CGOS_STORAGE_AREA_EEPROM;
			//cdv->brd->sto[cdv->brd->stoCount].info.flags =
			cdv->brd->sto[cdv->brd->stoCount].info.areaSize = power(2,(readBuf[8] & 0x1F));
			cdv->brd->sto[cdv->brd->stoCount].info.blockSize = 0;
			cdv->brd->stoCount++;
		}
		
		if(readBuf[9] != 0) //BIOS parameter EEPROM supported
		{
			cdv->brd->stoPriv[cdv->brd->stoPrivCount].info.size = sizeof(CGEB_STORAGEAREA_INFO);
			cdv->brd->stoPriv[cdv->brd->stoPrivCount].info.type = CGOS_STORAGE_AREA_EEPROM_BIOS;
			//cdv->brd->stoPriv[cdv->brd->stoPrivCount].info.flags =
			cdv->brd->stoPriv[cdv->brd->stoPrivCount].info.areaSize = power(2,(readBuf[9] & 0x1F));
			cdv->brd->stoPriv[cdv->brd->stoPrivCount].info.blockSize = 0;
			cdv->brd->stoPrivCount++;
		}

		if(readBuf[11] != 0) //User RAM supported
		{
			cdv->brd->sto[cdv->brd->stoCount].info.size = sizeof(CGEB_STORAGEAREA_INFO);
			cdv->brd->sto[cdv->brd->stoCount].info.type = CGOS_STORAGE_AREA_RAM;
			//cdv->brd->sto[cdv->brd->stoCount].info.flags =
			cdv->brd->sto[cdv->brd->stoCount].info.areaSize = power(2,(readBuf[11] & 0x1F));
			cdv->brd->sto[cdv->brd->stoCount].info.blockSize = 0;
			cdv->brd->stoCount++;
		}
		
		if(readBuf[12] != 0) //BIOS parameter RAM supported
		{
			cdv->brd->stoPriv[cdv->brd->stoPrivCount].info.size = sizeof(CGEB_STORAGEAREA_INFO);
			cdv->brd->stoPriv[cdv->brd->stoPrivCount].info.type = CGOS_STORAGE_AREA_RAM_BIOS;
			//cdv->brd->stoPriv[cdv->brd->stoPrivCount].info.flags =
			cdv->brd->stoPriv[cdv->brd->stoPrivCount].info.areaSize = power(2,(readBuf[12] & 0x1F));
			cdv->brd->stoPriv[cdv->brd->stoPrivCount].info.blockSize = 0;
			cdv->brd->stoPrivCount++;
		}
	}
	else
	{
		return CGOS_ERROR;
	} 
	
	return CGOS_SUCCESS;
}  

/***********************************************************************
 * unsigned int IsAreaType(unsigned int input)					    
 * 																		
 ***********************************************************************
 * Description:	Checks if input is one of the storage area types,
 *              and returns 1 for true or 0 for false.			
 * 																		
 * Last Change: 01.04.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/   
 unsigned int IsAreaType(unsigned int input)
 {
 	switch(input)
 	{
 		case CGOS_STORAGE_AREA_SDA:
		case CGOS_STORAGE_AREA_EEPROM_BIOS:
		case CGOS_STORAGE_AREA_RAM_BIOS:
		case CGOS_STORAGE_AREA_FLASH_STATIC:
		case CGOS_STORAGE_AREA_FLASH_DYNAMIC:
		case CGOS_STORAGE_AREA_FLASH_ALL:
		case CGOS_STORAGE_AREA_MPFA:
	 	case CGOS_STORAGE_AREA_EEPROM:
		case CGOS_STORAGE_AREA_FLASH:
		case CGOS_STORAGE_AREA_CMOS:
		case CGOS_STORAGE_AREA_RAM: return 1;
	 	default: return 0;	
	}
 }
 
/***********************************************************************
 * unsigned int IsAreaPresent(unsigned int input)					    
 * 																		
 ***********************************************************************
 * Description:	Checks if storage area type input is one of the storage 
 *              areas found by the driver. 
 *              The return values only work because the maximum value
 *              for stoCount is set to 16.
 * 																		
 * Last Change: 03.04.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
 unsigned int IsAreaPresent(unsigned int input, CGOS_DRV_VARS *cdv)
 {
	unsigned int i;
	for(i = 0; i < cdv->brd->stoCount; i++)
	{
		if(cdv->brd->sto[i].info.type == input)
		{
			return 0x10 + i;
		}
	} 
	for(i = 0; i < cdv->brd->stoPrivCount; i++)
	{
		if(cdv->brd->stoPriv[i].info.type == input)
		{
			return 0x20 + i;
		}
	}
	return 0;	
 }
 
/***********************************************************************
 * unsigned int CheckParameters(CGOS_DRV_VARS *cdv, 
 *                              unsigned int *areaIndex, 
 *                              unsigned int *storageArea,
 *                              unsigned char index,
 *                              unsigned char length)					    
 * 																		
 ***********************************************************************
 * Description:	Checks if the storage area is addressed by its type
 *              or by its index and checks if all input parameters are
 *              legal.
 * 																		
 * Last Change: 29.05.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/ 
unsigned int CheckParameters(CGOS_DRV_VARS *cdv, 
                             unsigned int *areaIndex, 
                             unsigned int *storageArea,
                             unsigned char index,
                             unsigned char length)
{
	//See if cdv->cin->type is a index or a storage area type
	if(IsAreaType(cdv->cin->type)) // storage area type
	{
		*areaIndex = IsAreaPresent(cdv->cin->type, cdv); 
		if(*areaIndex)
		{
			if(*areaIndex >= 0x20) //private Area
			{
				//Make sure only data within the storage area can be accessed
				if((index + length) > cdv->brd->stoPriv[*areaIndex & 0x0F].info.areaSize)
				{
					 return CGOS_ERROR;
				}
				else
				{
					*storageArea = cdv->cin->type;
				}			
			}
			else if(*areaIndex >= 0x10) //public Area
			{
				//Make sure only data within the storage area can be accessed
				if((index + length) > cdv->brd->sto[*areaIndex & 0x0F].info.areaSize)
				{
					 return CGOS_ERROR;
				}
				else
				{	
					*storageArea = cdv->cin->type;
				}
			}
			else
			{
				return CGOS_ERROR;
			}
		}
		else
		{
			return CGOS_ERROR;
		}
	}
	else 	// index
	{
		*areaIndex = cdv->cin->type;
		if(*areaIndex < cdv->brd->stoCount)
		{
			//Make sure only data within the storage area can be accessed
			if((index + length) > cdv->brd->sto[*areaIndex].info.areaSize)
			{
				 return CGOS_ERROR;
			}
			*storageArea = cdv->brd->sto[*areaIndex].info.type;
		}
		else
		{
			return CGOS_ERROR;
		}
	}
	return CGOS_SUCCESS;
}

/***********************************************************************
 * unsigned int CgosStorageAreaCount_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:	Returns the number of available storage areas.			
 * 																		
 * Last Change: 24.03.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/   
unsigned int CgosStorageAreaCount_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned int ret = 0;
	unsigned int i;

	dbgsto("CgosStorageAreaCount_BC called\n");

	if(cdv->cin->type == CGOS_STORAGE_AREA_UNKNOWN)
	{
		cdv->cout->rets[0] = cdv->brd->stoCount;
	}
	else
	{
		for(i = 0; i < cdv->brd->stoCount; i++)
		{
			if( cdv->brd->sto[i].info.type == cdv->cin->type)
			{
				ret++;
			}
		}
		cdv->cout->rets[0] = ret;
	}
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosStorageAreaType_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:	Return the CGOS_STORAGE_AREA_* type of the chosen
 * 		storage area.			
 * 																		
 * Last Change: 24.03.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/ 
unsigned int CgosStorageAreaType_BC(CGOS_DRV_VARS *cdv)
  {
	dbgsto("CgosStorageAreaType_BC called\n");	
	cdv->cout->rets[0]=cdv->sto[cdv->cin->type].info.type;	
	return CGOS_SUCCESS;		
  }

/***********************************************************************
 * unsigned int CgosStorageAreaSize_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:	Returns the size of the chosen storage area.			
 * 																		
 * Last Change: 20.04.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/ 
unsigned int CgosStorageAreaSize_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned int areaNumber;
	dbgsto("CgosStorageAreaSize_BC called\n");	
	
	//See if cdv->cin->type is a index or a storage area type
	if(IsAreaType(cdv->cin->type)) //cdv->cin->type is storage area type
	{
		areaNumber = IsAreaPresent(cdv->cin->type,cdv);
		if(areaNumber >= 0x20) //private storage area
		{
			if((areaNumber & 0x0F) < cdv->brd->stoPrivCount)
			{
				cdv->cout->rets[0] = cdv->brd->stoPriv[areaNumber & 0x0F].info.areaSize;
				return CGOS_SUCCESS;
			}
			else 
			{
				return CGOS_ERROR;
			}
		}
		else if(areaNumber >= 0x10) //public storage area
		{
			if((areaNumber & 0x0F) < cdv->brd->stoCount)
			{
				cdv->cout->rets[0] = cdv->brd->sto[areaNumber & 0x0F].info.areaSize;
				return CGOS_SUCCESS;
			}
			else
			{
				return CGOS_ERROR;
			}		
		}
		else
		{
			return CGOS_ERROR;
		}
	}
	else //cdv->cin->type is index
	{
		cdv->cout->rets[0] = cdv->sto[cdv->cin->type].info.areaSize;
	}		
	return CGOS_SUCCESS;	
  }

/***********************************************************************
 * unsigned int CgosStorageAreaBlockSize_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:	Returns the block size of the chosen storage area.			
 * 																		
 * Last Change: 20.04.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/ 
unsigned int CgosStorageAreaBlockSize_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned int areaNumber;
	dbgsto("CgosStorageAreaBlockSize_BC called\n");
	//See if cdv->cin->type is a index or a storage area type
	if(IsAreaType(cdv->cin->type)) //cdv->cin->type is storage area type
	{
		areaNumber = IsAreaPresent(cdv->cin->type, cdv);
		if(areaNumber >= 0x20) //private storage area
		{
			if((areaNumber & 0x0F) < cdv->brd->stoPrivCount)
			{
				cdv->cout->rets[0] = cdv->brd->stoPriv[areaNumber & 0x0F].info.blockSize;
				return CGOS_SUCCESS;
			}
			else
			{
				return CGOS_ERROR;
			}
		}
		else if(areaNumber >= 0x10) //public storage area
		{
			if((areaNumber & 0x0F) < cdv->brd->stoCount)
			{
				cdv->cout->rets[0] = cdv->brd->sto[areaNumber & 0x0F].info.blockSize;
				return CGOS_SUCCESS;
			}
			else
			{
				return CGOS_ERROR;
			}
		}
		else
		{
			return CGOS_ERROR;
		}
	}
	else //cdv->cin->type is index
	{
		cdv->cout->rets[0] = cdv->sto[cdv->cin->type].info.blockSize;
	}				
	return CGOS_SUCCESS;	
  }

/***********************************************************************
 * unsigned int CgosStorageAreaRead_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:	Reads from a given storage area.		
 * 																		
 * Last Change: 20.04.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/ 
unsigned int CgosStorageAreaRead_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned int storageArea;
	unsigned int areaIndex;
	unsigned char index = cdv->cin->pars[0];
	unsigned char length = cdv->cin->pars[1];
	unsigned int i;
	dbgsto("CgosStorageAreaRead_BC called\n");

	if(CheckParameters(cdv, &areaIndex, &storageArea, index, length) == CGOS_ERROR)
	{
		return CGOS_ERROR;
	}

	switch(storageArea)
	{
		case CGOS_STORAGE_AREA_SDA: 	
						for(i = 0; i < length; i++)
						{
							CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_SDA_EEP_I2C_ADDR,index+i,(unsigned char*)cdv->pout + i);
						}
						cdv->retcnt += cdv->lout;
						break;
		case CGOS_STORAGE_AREA_EEPROM:
						for(i = 0; i < length; i++)
						{
							CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_USER_EEP_I2C_ADDR,index+i,(unsigned char*)cdv->pout + i);
						}
						cdv->retcnt += cdv->lout;
						break;
		case CGOS_STORAGE_AREA_RAM:	
						for(i = 0; i < length; i++)
						{
							CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_USER_RAM_I2C_ADDR,index+i,(unsigned char*)cdv->pout + i);
						}
						cdv->retcnt += cdv->lout;
						break;
		case CGOS_STORAGE_AREA_EEPROM_BIOS:	
						for(i = 0; i < length; i++)
						{
							CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_BIOS_EEP_I2C_ADDR,index+i,(unsigned char*)cdv->pout + i);
						}
						cdv->retcnt += cdv->lout;
						break;
		case CGOS_STORAGE_AREA_RAM_BIOS:	
						for(i = 0; i < length; i++)
						{
							CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_BIOS_RAM_I2C_ADDR,index+i,(unsigned char*)cdv->pout + i);
						}
						cdv->retcnt += cdv->lout;
						break;
		default: break;
	}

	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosStorageAreaWrite_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:	Writes the given bytes to the specified storage area.		
 * 																		
 * Last Change: 20.04.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/ 
unsigned int CgosStorageAreaWrite_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned int storageArea;
	unsigned int areaIndex;
	unsigned char index = cdv->cin->pars[0];
	unsigned char length = cdv->cin->pars[1];
	unsigned int i;
	
	dbgsto("CgosStorageAreaWrite_BC called\n");

	if(CheckParameters(cdv, &areaIndex, &storageArea, index, length) == CGOS_ERROR)
	{
		return CGOS_ERROR;
	}

	switch(storageArea)
	{
		case CGOS_STORAGE_AREA_SDA:
						if(cdv->brd->sto[areaIndex & 0x0F].info.flags == 0x01) //Area is locked
						{
							cdv->cout->status = CGOS_ERROR;
							return CGOS_ERROR;
						}
						else
						{
							for(i = 0; i < length; i++)
							{
								CgosI2CWriteRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_SDA_EEP_I2C_ADDR,index+i,*((unsigned char*)cdv->pin + i));
								msleep(1);	//ToDo: Check if this is neccessery
							}
						}							
							break;
		case CGOS_STORAGE_AREA_EEPROM:
						if(cdv->brd->sto[areaIndex & 0x0F].info.flags == 0x01) //Area is locked
						{
							cdv->cout->rets[0] = 0; //FALSE
							cdv->cout->status = CGOS_ERROR;
							return CGOS_ERROR;
						}
						else
						{
							for(i = 0; i < length; i++)
							{
								CgosI2CWriteRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_USER_EEP_I2C_ADDR,index+i,*((unsigned char*)cdv->pin + i));
								msleep(1);  //ToDo: Check if this is neccessery
							}
						}
						break;
		case CGOS_STORAGE_AREA_RAM:	
						if(cdv->brd->sto[areaIndex & 0x0F].info.flags == 0x01) //Area is locked
						{
							cdv->cout->status = CGOS_ERROR;
							return CGOS_ERROR;
						}
						else
						{
							for(i = 0; i < length; i++)
							{
								CgosI2CWriteRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_USER_RAM_I2C_ADDR,index+i,*((unsigned char*)cdv->pin + i));
							}
						}
						break;
		case CGOS_STORAGE_AREA_EEPROM_BIOS:	
						if(cdv->brd->sto[areaIndex & 0x0F].info.flags == 0x01) //Area is locked
						{
							cdv->cout->status = CGOS_ERROR;
							return CGOS_ERROR;
						}
						else
						{
							for(i = 0; i < length; i++)
							{
								CgosI2CWriteRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_BIOS_EEP_I2C_ADDR,index+i,*((unsigned char*)cdv->pin + i));
								msleep(1);  //ToDo: Check if this is neccessery
							}
						}
							break;
		case CGOS_STORAGE_AREA_RAM_BIOS:
						if(cdv->brd->sto[areaIndex & 0x0F].info.flags == 0x01) //Area is locked
						{
							cdv->cout->status = CGOS_ERROR;
							return CGOS_ERROR;
						}
						else
						{	
							for(i = 0; i < length; i++)
							{
								CgosI2CWriteRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_BIOS_RAM_I2C_ADDR,index+i,*((unsigned char*)cdv->pin + i));
							}
						}
						break;
		default: break;
	}

	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosStorageAreaErase_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:	Writes 0xFF to the given area to indicate that it has 
 *              erased.		
 * 																		
 * Last Change: 20.04.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/ 
unsigned int CgosStorageAreaErase_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned int storageArea;
	unsigned int areaIndex;
	unsigned char index = cdv->cin->pars[0];
	unsigned char length = cdv->cin->pars[1];
	unsigned int i;

	dbgsto("CgosStorageAreaErase_BC called\n");

	if(CheckParameters(cdv, &areaIndex, &storageArea, index, length) == CGOS_ERROR)
	{
		return CGOS_ERROR;
	}

	switch(storageArea)
	{
		case CGOS_STORAGE_AREA_SDA:
						if(cdv->brd->sto[areaIndex & 0x0F].info.flags == 0x01) //Area is locked
						{
							cdv->cout->status = CGOS_ERROR;
							return CGOS_ERROR;
						}
						else
						{
							for(i = 0; i < length; i++)
							{
								CgosI2CWriteRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_SDA_EEP_I2C_ADDR,index+i,0xff);
								msleep(1);	//ToDo: Check if this is neccessery
							}
						}
						break;
		case CGOS_STORAGE_AREA_EEPROM:
						if(cdv->brd->sto[areaIndex & 0x0F].info.flags == 0x01) //Area is locked
						{
							cdv->cout->status = CGOS_ERROR;
							return CGOS_ERROR;
						}
						else
						{
							for(i = 0; i < length; i++)
							{
								CgosI2CWriteRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_USER_EEP_I2C_ADDR,index+i,0xff);
								msleep(1);  //ToDo: Check if this is neccessery
							}
						}
						break;
		case CGOS_STORAGE_AREA_RAM:	
						if(cdv->brd->sto[areaIndex & 0x0F].info.flags == 0x01) //Area is locked
						{
							cdv->cout->status = CGOS_ERROR;
							return CGOS_ERROR;
						}
						else
						{
							for(i = 0; i < length; i++)
							{
								CgosI2CWriteRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_USER_RAM_I2C_ADDR,index+i,0xff);
							}
						}
						break;
		case CGOS_STORAGE_AREA_EEPROM_BIOS:	
						if(cdv->brd->sto[areaIndex & 0x0F].info.flags == 0x01) //Area is locked
						{
							cdv->cout->status = CGOS_ERROR;
							return CGOS_ERROR;
						}
						else
						{
							for(i = 0; i < length; i++)
							{
								CgosI2CWriteRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_BIOS_EEP_I2C_ADDR,index+i,0xff);
								msleep(1);  //ToDo: Check if this is neccessery
							}
						}
						break;
		case CGOS_STORAGE_AREA_RAM_BIOS:
						if(cdv->brd->sto[areaIndex & 0x0F].info.flags == 0x01) //Area is locked
						{
							cdv->cout->status = CGOS_ERROR;
							return CGOS_ERROR;
						}
						else
						{	
							for(i = 0; i < length; i++)
							{
								CgosI2CWriteRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_BIOS_RAM_I2C_ADDR,index+i,0xff);
							}
						}
						break;
		default: break;
	}


	return CGOS_SUCCESS;	
  }

/***********************************************************************
 * unsigned int CgosStorageAreaEraseStatus_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:	Reads the specified storage area, checks if it's set to
 *              0xFF which would indicate that it's been erased. Returns
 *              0 if the storage area was erased and 2 if it wasn't. 
 *
 * 																		
 * Last Change: 20.04.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/ 
unsigned int CgosStorageAreaEraseStatus_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned int storageArea;
	unsigned int areaIndex;
	unsigned char index = cdv->cin->pars[0];
	unsigned char length = cdv->cin->pars[1];
	unsigned char rbuf;
	unsigned int i;
	unsigned char eraseStatus = 0;

	dbgsto("CgosStorageAreaEraseStatus_BC called\n");

	if(CheckParameters(cdv, &areaIndex, &storageArea, index, length) == CGOS_ERROR)
	{
		return CGOS_ERROR;
	}


	switch(storageArea)
	{
		case CGOS_STORAGE_AREA_SDA: 	
						for(i = 0; i < length; i++)
						{
							CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_SDA_EEP_I2C_ADDR,index+i,&rbuf);
							if(rbuf != 0xff)
							{	
								eraseStatus = 1;
							}
						}
						if(eraseStatus == 0)
						{
							cdv->cout->rets[0] = 0; //return Area erased
						}
						else
						{
							cdv->cout->rets[0] = 2; //return Area not erased
						}
						break;
		case CGOS_STORAGE_AREA_EEPROM:
						for(i = 0; i < length; i++)
						{
							CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_USER_EEP_I2C_ADDR,index+i,&rbuf);
							if(rbuf != 0xff)
							{	
								eraseStatus = 1;
							}
						}
						if(eraseStatus == 0)
						{
							cdv->cout->rets[0] = 0; //return Area erased
						}
						else
						{
							cdv->cout->rets[0] = 2; //return Area not erased
						}
						break;
		case CGOS_STORAGE_AREA_RAM:	
						for(i = 0; i < length; i++)
						{
							CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_USER_RAM_I2C_ADDR,index+i,&rbuf);
							if(rbuf != 0xff)
							{	
								eraseStatus = 1;
							}
						}
						if(eraseStatus == 0)
						{
							cdv->cout->rets[0] = 0; //return Area erased
						}
						else
						{
							cdv->cout->rets[0] = 2; //return Area not erased
						}
						break;
		case CGOS_STORAGE_AREA_EEPROM_BIOS:	
						for(i = 0; i < length; i++)
						{
							CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_BIOS_EEP_I2C_ADDR,index+i,&rbuf);
							if(rbuf != 0xff)
							{	
								eraseStatus = 1;
							}
						}
						if(eraseStatus == 0)
						{
							cdv->cout->rets[0] = 0; //return Area erased
						}
						else
						{
							cdv->cout->rets[0] = 2; //return Area not erased
						}
						break;
		case CGOS_STORAGE_AREA_RAM_BIOS:	
						for(i = 0; i < length; i++)
						{
							CgosI2CReadRegisterRaw_BC(CGBC_I2C_BUS_INTERNAL,CG_BIOS_RAM_I2C_ADDR,index+i,&rbuf);
							if(rbuf != 0xff)
							{	
								eraseStatus = 1;
							}
						}
						if(eraseStatus == 0)
						{
							cdv->cout->rets[0] = 0; //return Area erased
						}
						else
						{
							cdv->cout->rets[0] = 2; //return Area not erased
						}
						break;
		default: break;
	}

	return CGOS_SUCCESS;		
  }

/***********************************************************************
 * unsigned int CgosStorageAreaLock_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:	Write locks a storage area.			
 * 																		
 * Last Change: 20.04.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/   
unsigned int CgosStorageAreaLock_BC(CGOS_DRV_VARS *cdv)					
  {
	unsigned int storageArea;
	unsigned int index;
	unsigned char writeBuf[8] = {0};
	unsigned char lockConfig;
	unsigned char length = cdv->cin->pars[1];
	unsigned char stat;

	dbgsto("CgosStorageAreaLock_BC called\n");

	//See if cdv->cin->type is a index or a storage area type
	if(IsAreaType(cdv->cin->type))	// area type
	{
		index = IsAreaPresent(cdv->cin->type, cdv);
		if(index >= 0x20) //private storage area
		{
			if((index & 0x0F) < cdv->brd->stoPrivCount)
			{
				storageArea = cdv->brd->stoPriv[index & 0x0F].info.type;
			}
			else
			{
				cdv->cout->status = CGOS_ERROR;
				return CGOS_ERROR;
			}		
		}
		else if(index >= 0x10) //public storage area
		{
			if((index & 0x0F) < cdv->brd->stoCount)
			{
				storageArea = cdv->brd->sto[index & 0x0F].info.type;
			}
			else
			{
				cdv->cout->status = CGOS_ERROR;
				return CGOS_ERROR;
			}	
		}
		else
		{
			cdv->cout->status = CGOS_ERROR;
			return CGOS_ERROR;
		}
		index = index & 0x0F;
	}
	else	// index
	{
		index = cdv->cin->type;
		if(index < cdv->brd->stoCount)
		{
			storageArea = cdv->brd->sto[index].info.type;
	
		}
		else
		{
			cdv->cout->status = CGOS_ERROR;
			return CGOS_ERROR;
		}
	}

	//Check length of key string, max 6 bytes
	if(length > 6)
	{
		cdv->cout->status = CGOS_ERROR;
		return CGOS_ERROR;
	}

	switch(storageArea)
	{
		case CGOS_STORAGE_AREA_EEPROM:
						writeBuf[0] = 0x92; //CGBC_CMD_ADV_USER_LOCK for BC4
								    //CGBC_CMD_ACCESS_LOCK for GEN5
								    
						// Lock Configuration Byte, Bit set to 1 means locked, 0 unlocked.
						// Bit |  7   6   5   4   3  |      2     |    1    |      0      |
						//     | Reserved, must be 0 |read access |reserved |write access |
						writeBuf[1] = 0x01; 
						
						OsaMemCpy(&writeBuf[2],(unsigned char*)cdv->pin,length);
						if(!bcCommand(&writeBuf[0],8,&lockConfig,1,&stat))
						{
							cdv->brd->sto[index].info.flags = 0x01;
						}
						else
						{
							cdv->cout->status = CGOS_ERROR;
							return CGOS_ERROR;
						}
						break;	
		default: 	cdv->cout->status = CGOS_ERROR;
				return CGOS_ERROR;
	}
	return CGOS_SUCCESS;
  }
  
/***********************************************************************
 * unsigned int CgosStorageAreaUnlock_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:	Unlocks a storage area.	
 * 																		
 * Last Change: 20.04.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/   
unsigned int CgosStorageAreaUnlock_BC(CGOS_DRV_VARS *cdv)					
  {
	unsigned int storageArea;
	unsigned int index;
	unsigned char writeBuf[8] = {0};
	unsigned char length = cdv->cin->pars[1];
	unsigned char lockConfig;
	unsigned char stat;

	dbgsto("CgosStorageAreaUnlock_BC called\n");

	//See if cdv->cin->type is a index or a storage area type
	if(IsAreaType(cdv->cin->type))
	{
		index = IsAreaPresent(cdv->cin->type, cdv);
		if(index >= 0x20) //private storage area
		{
			if((index & 0x0F) < cdv->brd->stoPrivCount)
			{
				storageArea = cdv->brd->stoPriv[index & 0x0F].info.type;
			}
			else
			{
				cdv->cout->status = CGOS_ERROR;
				return CGOS_ERROR;
			}
		}
		else if(index >= 0x10) //public storage area
		{
			if((index & 0x0F) < cdv->brd->stoCount)
			{
				storageArea = cdv->brd->sto[index & 0x0F].info.type;
			}
			else
			{
				cdv->cout->status = CGOS_ERROR;
				return CGOS_ERROR;
			}
		}
		else
		{
			cdv->cout->status = CGOS_ERROR;
			return CGOS_ERROR;
		}
		index = index & 0x0F;
	}
	else
	{
		if(cdv->cin->type < cdv->brd->stoCount)
		{
			index = cdv->cin->type;
			storageArea = cdv->brd->sto[index].info.type;
		}
		else
		{
			cdv->cout->status = CGOS_ERROR;
			return CGOS_ERROR;
		}	
	}

	//Check length of key string, max 6 bytes
	if(length > 6)
	{
		cdv->cout->status = CGOS_ERROR;
		return CGOS_ERROR;
	}

	switch(storageArea)
	{
		case CGOS_STORAGE_AREA_EEPROM:
						writeBuf[0] = 0x92; //CGBC_CMD_ADV_USER_LOCK for BC4
								    //CGBC_CMD_ACCESS_LOCK for GEN5
						
						// Lock Configuration Byte, Bit set to 1 means locked, 0 unlocked.
						// Bit |  7   6   5   4   3  |      2     |    1    |      0      |
						//     | Reserved, must be 0 |read access |reserved |write access |
						writeBuf[1] = 0x00; 
						OsaMemCpy(&writeBuf[2],(unsigned char*)cdv->pin,length);
						if(!bcCommand(&writeBuf[0],8,&lockConfig,1,&stat))
						{
							cdv->brd->sto[index].info.flags = 0x00;
		 				}
						else
						{
							cdv->cout->status = CGOS_ERROR;
							return CGOS_ERROR;
						}
						break;	
		default: 	cdv->cout->status = CGOS_ERROR;
				return CGOS_ERROR;
	}	
	return CGOS_SUCCESS;		
  }
  
/***********************************************************************
 * unsigned int CgosStorageAreaIsLocked_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:	Returns if the given area is locked or not.		
 * 																		
 * Last Change: 20.04.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/ 
unsigned int CgosStorageAreaIsLocked_BC(CGOS_DRV_VARS *cdv)					
  {
	unsigned int index;
	dbgsto("CgosStorageAreaIsLocked_BC called\n");

	if(IsAreaType(cdv->cin->type)) //input is storage area type
	{
		index = IsAreaPresent(cdv->cin->type, cdv);
		if(index >= 0x20) //private storage area
		{
			if((index & 0x0F) < cdv->brd->stoPrivCount)
			{
				if(cdv->brd->stoPriv[index & 0x0F].info.flags == 0x01)
				{
					cdv->cout->rets[0] = 1; //TRUE
					return CGOS_SUCCESS;
				}
				else
				{
					cdv->cout->rets[0] = 0; //FALSE
					return CGOS_SUCCESS;
				}
			}
			else
			{
				return CGOS_ERROR;
			}
		}
		else if(index >= 0x10) //public storage area
		{
			if((index & 0x0F) < cdv->brd->stoCount)
			{
				if(cdv->brd->sto[index & 0x0F].info.flags == 0x01)
				{
					cdv->cout->rets[0] = 1; //TRUE
					return CGOS_SUCCESS;
				}
				else
				{
					cdv->cout->rets[0] = 0; //FALSE
					return CGOS_SUCCESS;
				}
			}
			else
			{
				return CGOS_ERROR;
			}
		}
		else
		{
			return CGOS_ERROR;
		}
	}
	else	//input is index
	{
		if(cdv->cin->type < cdv->brd->stoCount)
		{
			if(cdv->brd->sto[cdv->cin->type].info.flags == 0x01)
			{
				cdv->cout->rets[0] = 1; //TRUE
				return CGOS_SUCCESS;
			}
			else
			{
				cdv->cout->rets[0] = 0; //FALSE
				return CGOS_SUCCESS;
			}
		}
		else
		{
			return CGOS_ERROR;
		}
	}
  }

/***********************************************************************
 * unsigned int exitStoSubModule(CGOS_DRV_VARS *cdv) 							    
 * 																		
 ***********************************************************************
 * Description: This function is called during driver close and should	
 * 				free allocated resources.								
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int exitStoSubModule_BC(CGOS_DRV_VARS *cdv)
{
	dbgsto("exitStoSubModule called\n");
	return CGOS_SUCCESS;
}
