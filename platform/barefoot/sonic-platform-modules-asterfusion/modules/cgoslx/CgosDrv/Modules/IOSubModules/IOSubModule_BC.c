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
#include "IOSubModule_BC_cfg.h"

//***************************************************************************
//#define dbg_io_sub_bc_on

#ifdef dbg_io_sub_bc_on
#ifdef __linux__
#define dbgio(x) printk(x)
#elif _WIN64
#define dbgio(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#elif _WIN32
#define dbgio(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#endif
#else
#define dbgio(x)
#endif

//***************************************************************************
// Function prototypes for register function

unsigned int initIOSubModule_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosIOCount_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosIORead_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosIOWrite_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosIOGetDirection_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosIOSetDirection_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosIOIsAvailable_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosIOGetDirectionCaps_BC(CGOS_DRV_VARS *cdv);
unsigned int exitIOSubModule_BC(CGOS_DRV_VARS *cdv);

//***************************************************************************

/***********************************************************************
 * unsigned int RegisterIOSubModule_BC									
 * 																		
 ***********************************************************************
 * Description: This function uses the RegisterSubModule callback to	
 * 			  	register the SubModule functions.						
 * 																		
 * Last Change: 20.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int RegisterIOSubModule_BC(unsigned int (*RegisterSubModule)(	CgosFunctionPointer initSubModule,
																		CgosFunctionPointer Count,
																		CgosFunctionPointer Read,
																		CgosFunctionPointer Write,
																		CgosFunctionPointer GetDirection,
																		CgosFunctionPointer SetDirection,
																		CgosFunctionPointer IsAvailable,
																		CgosFunctionPointer GetDirectionCaps,
																		CgosFunctionPointer exitSubModule,
																		unsigned char ModuleNumber),
								    unsigned char Number)
{
	return RegisterSubModule(	initIOSubModule_BC,
								CgosIOCount_BC,
								CgosIORead_BC,
								CgosIOWrite_BC,
								CgosIOGetDirection_BC,
								CgosIOSetDirection_BC,
								CgosIOIsAvailable_BC,
								CgosIOGetDirectionCaps_BC,
								exitIOSubModule_BC,
								Number);
}

/***********************************************************************
 * unsigned int initIOSubModule_BC(CGOS_DRV_VARS *cdv)					
 * 																		
 ***********************************************************************
 * Description: This function initializes this SubModule.				
 * 																		
 * Last Change: 20.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initIOSubModule_BC(CGOS_DRV_VARS *cdv)
{
	unsigned int retSuccess = CGOS_SUCCESS;
	
	dbgio("initIOSubModule_BC called\n");

	if(IO_COUNT_BC <= CGOS_DRV_IO_MAX)		//CGOS_DRV_IO_MAX defined in DrvVars.h
	{
		cdv->brd->ioCount = IO_COUNT_BC;
		OsaMemCpy(&cdv->brd->iocaps[0],&IO_CAPS_BC[0],IO_COUNT_BC * sizeof(CGOS_DRV_IO_DIR_CAPS));	//May cause alignment problems
	}
	else
	{
		retSuccess = CGOS_ERROR;
	}
	return retSuccess;
}

/***********************************************************************
 * unsigned int CgosIOCount_BC(CGOS_DRV_VARS *cdv)						
 * 																		
 ***********************************************************************
 * Description: This function returns the number of IO Units.			
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosIOCount_BC(CGOS_DRV_VARS *cdv)
  {
	dbgio("CgosIOCount_BC called\n");
    cdv->cout->rets[0] = cdv->brd->ioCount;
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosIORead_BC(CGOS_DRV_VARS *cdv)						
 * 																		
 ***********************************************************************
 * Description: This function reads a IO Unit.							
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosIORead_BC(CGOS_DRV_VARS *cdv)
{
	unsigned char wbuf[3];
	unsigned char sts;
	unsigned char ret;
	unsigned char rets;
	
	dbgio("CgosIORead_BC called\n");

	if(cdv->cin->type < cdv->brd->ioCount)
	{	
		wbuf[0] = CGBC_CMD_GPIO_DAT_RD;
		wbuf[1] = cdv->cin->type;
		wbuf[2] = 0x00;
	
		ret = bcCommand(&wbuf[0],3,&rets,1,&sts);
		cdv->cout->rets[0] = rets;

		return ret;
	}
	else
	{
		return CGOS_ERROR;
	}
}

/***********************************************************************
 * unsigned int CgosIOWrite_BC(CGOS_DRV_VARS *cdv)						
 * 																		
 ***********************************************************************
 * Description: This function writes to a IO Unit.						
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosIOWrite_BC(CGOS_DRV_VARS *cdv)
{
	unsigned char wbuf[3];
	unsigned char sts;
	unsigned char ret;
	unsigned char rets;
	
	dbgio("CgosIOWrite_BC called\n");

	if(cdv->cin->type < cdv->brd->ioCount)
	{	
		wbuf[0] = CGBC_CMD_GPIO_DAT_WR;
		wbuf[1] = cdv->cin->type;
		wbuf[2] = cdv->cin->pars[0];

		ret = bcCommand(&wbuf[0],3,&rets,1,&sts); 	
		
		return ret;
	}
	else
	{
		return CGOS_ERROR;
	}
}

/***********************************************************************
 * unsigned int CgosIOGetDirection_BC(CGOS_DRV_VARS *cdv)				
 * 																		
 ***********************************************************************
 * Description: This function gets the current IO directions.			
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosIOGetDirection_BC(CGOS_DRV_VARS *cdv)
{
	unsigned char wbuf[3];
	unsigned char sts;
	unsigned char ret;
	unsigned char rets;
	
	dbgio("CgosIOGetDirection_BC called\n");

	if(cdv->cin->type < cdv->brd->ioCount)
	{
		wbuf[0] = CGBC_CMD_GPIO_CFG_RD;
		wbuf[1] = cdv->cin->type;
		wbuf[2] = 0x00;
	
		ret = bcCommand(&wbuf[0],3,&rets,1,&sts);
		cdv->cout->rets[0] = rets;
	 
		return ret;
	}
	else
	{
		return CGOS_ERROR;
	}
}   

/***********************************************************************
 * unsigned int CgosIOSetDirection_BC(CGOS_DRV_VARS *cdv)				
 * 																		
 ***********************************************************************
 * Description: This function sets the IO directions.					
 * 																		
 * Last Change: 24.01.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosIOSetDirection_BC(CGOS_DRV_VARS *cdv)
{
	unsigned char wbuf[3];
	unsigned char sts;
	unsigned char ret;
	unsigned char rets;

	dbgio("CgosIOSetDirection_BC called\n");

	if(cdv->cin->type < cdv->brd->ioCount)
	{
		wbuf[0] = CGBC_CMD_GPIO_CFG_WR;
		wbuf[1] = cdv->cin->type;
		wbuf[2] = cdv->cin->pars[0];

		ret = bcCommand(&wbuf[0],3,&rets,1,&sts);
			
		return ret;
	}
	else
	{
		return CGOS_ERROR;
	}
}

/***********************************************************************
 * unsigned int CgosIOIsAvailable_BC(CGOS_DRV_VARS *cdv)				
 * 																		
 ***********************************************************************
 * Description: This function checks if a Unit is available.			
 * 																		
 * Last Change: 20.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosIOIsAvailable_BC(CGOS_DRV_VARS *cdv)
  {	
	dbgio("CgosIOIsAvailable_BC called\n");
	cdv->cout->rets[0] = cdv->cin->type < cdv->brd->ioCount;
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosIOGetDirectionCaps_BC(CGOS_DRV_VARS *cdv)			
 * 																		
 ***********************************************************************
 * Description: This function gets the Unit's direction capabilities.	
 * 																		
 * Last Change: 20.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosIOGetDirectionCaps_BC(CGOS_DRV_VARS *cdv)
  {  
	unsigned int retSuccess = CGOS_SUCCESS;

	dbgio("CgosIOGetDirectionCaps_BC called\n");

	if(cdv->cin->type < cdv->brd->ioCount)
	{
		cdv->cout->rets[0] = cdv->brd->iocaps[cdv->cin->type].in;
		cdv->cout->rets[1] = cdv->brd->iocaps[cdv->cin->type].out;
	}
	else
	{
		retSuccess = CGOS_ERROR;
	}
	return retSuccess;
  }

/***********************************************************************
 * unsigned int exitIOSubModule_BC(CGOS_DRV_VARS *cdv)					
 * 																		
 ***********************************************************************
 * Description: This function exits this SubModule.						
 * 																		
 * Last Change: 20.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int exitIOSubModule_BC(CGOS_DRV_VARS *cdv)
{
	dbgio("exitIOSubModule_BC called\n");
	return CGOS_SUCCESS;
}
