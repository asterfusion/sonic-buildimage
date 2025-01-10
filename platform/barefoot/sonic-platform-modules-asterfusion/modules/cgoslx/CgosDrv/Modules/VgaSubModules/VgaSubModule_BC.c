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
#include "VgaSubModule_BC_cfg.h"

//***************************************************************************

//#define dbg_vga_sub_bc_on

#ifdef dbg_vga_sub_bc_on
#ifdef __linux__
#define dbgvga(x) printk(x)
#elif _WIN64
#define dbgvga(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#elif _WIN32
#define dbgvga(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#endif
#else
#define dbgvga(x)
#endif
//***************************************************************************
// Function prototypes for register function

unsigned int initVgaSubModule_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVgaCount_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVgaGetInfo_BC(CGOS_DRV_VARS *cdv);	
unsigned int CgosVgaGetContrast_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVgaSetContrast_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVgaGetContrastEnable_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVgaSetContrastEnable_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVgaGetBacklight_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVgaSetBacklight_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVgaGetBacklightEnable_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVgaSetBacklightEnable_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVgaEndDarkBoot_BC(CGOS_DRV_VARS *cdv);
unsigned int exitVgaSubModule_BC(CGOS_DRV_VARS *cdv);

//***************************************************************************

/***********************************************************************
 * unsigned int RegisterVgaSubModule_BC									
 * 																		
 ***********************************************************************
 * Description: This function uses the RegisterSubModule callback to	
 * 			  	register the SubModule functions.						
 * 																		
 * Last Change: 25.06.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int RegisterVgaSubModule_BC(unsigned int (*RegisterSubModule)(	CgosFunctionPointer initSubModule,				    
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
																		unsigned char ModuleNumber),
								                    unsigned char Number)
{
	return RegisterSubModule(	initVgaSubModule_BC,
								NULL, //CgosVgaCount_BC,
								NULL, //CgosVgaGetInfo_BC,
								NULL, //CgosVgaGetContrast_BC,
								NULL, //CgosVgaSetContrast_BC,
								NULL, //CgosVgaGetContrastEnable_BC,
								NULL, //CgosVgaSetContrastEnable_BC,
								NULL, //CgosVgaGetBacklight_BC,
								NULL, //CgosVgaSetBacklight_BC,
								NULL, //CgosVgaGetBacklightEnable_BC,
								NULL, //CgosVgaSetBacklightEnable_BC,
								NULL, //CgosVgaEndDarkBoot_BC,
								exitVgaSubModule_BC,
								Number);
}

/***********************************************************************
 * unsigned int initVgaSubModule_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description: This function is called to copy information from		 
 * 				VgaModule_cfg.h into the cdv struct.					
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initVgaSubModule_BC(CGOS_DRV_VARS *cdv)
  {
	int i = 0;
	unsigned int retSuccess = CGOS_SUCCESS;
	dbgvga("initVgaModule called\n");
	//Only writes dummy values from VgaSubModule_BC_cfg.h at the moment!!!
	if(VGA_COUNT <= CGOS_DRV_VGA_MAX)		//CGOS_DRV_VGA_MAX defined in DrvVars.h
	{
		cdv->brd->vgaCount = VGA_COUNT;
		for(i = 0; i < VGA_COUNT; i++)
		{
			OsaMemCpy(&cdv->vga[i].info,&VGA_INFO[i],sizeof(CGOSVGAINFO));
		}
	}
	else
	{
		retSuccess = CGOS_ERROR;
	}
	return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosVgaCount_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/    
unsigned int CgosVgaCount_BC(CGOS_DRV_VARS *cdv)
  {
	dbgvga("zCgosVgaCount called\n");
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int CgosVgaGetInfo_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/    
unsigned int CgosVgaGetInfo_BC(CGOS_DRV_VARS *cdv)						
  {
	dbgvga("zCgosVgaGetInfo called\n");
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int CgosVgaGetContrast_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosVgaGetContrast_BC(CGOS_DRV_VARS *cdv)
  {
	dbgvga("zCgosVgaGetContrast called\n");
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int CgosVgaSetContrast_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosVgaSetContrast_BC(CGOS_DRV_VARS *cdv)
  {
	dbgvga("zCgosVgaSetContrast called\n");
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int CgosVgaGetContrastEnable_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosVgaGetContrastEnable_BC(CGOS_DRV_VARS *cdv)
  {
	dbgvga("zCgosVgaGetContrastEnable called\n");
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int CgosVgaSetContrastEnable_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosVgaSetContrastEnable_BC(CGOS_DRV_VARS *cdv)
  {
	dbgvga("zCgosVgaSetContrastEnable called\n");
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int CgosVgaGetBacklight_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosVgaGetBacklight_BC(CGOS_DRV_VARS *cdv)
  {
	dbgvga("zCgosVgaGetBacklight called\n");
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int CgosVgaSetBacklight_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosVgaSetBacklight_BC(CGOS_DRV_VARS *cdv)
  {
	dbgvga("zCgosVgaSetBacklight called\n");	
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int CgosVgaGetBacklightEnable_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosVgaGetBacklightEnable_BC(CGOS_DRV_VARS *cdv)
  {
	dbgvga("zCgosVgaGetBacklightEnable called\n");
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int CgosVgaSetBacklightEnable_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosVgaSetBacklightEnable_BC(CGOS_DRV_VARS *cdv)
  {
	dbgvga("zCgosVgaSetBacklightEnable called\n");  
	return CGOS_ERROR;
  }

/***********************************************************************
 * unsigned int CgosVgaEndDarkBoot_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosVgaEndDarkBoot_BC(CGOS_DRV_VARS *cdv)
  {
	dbgvga("zCgosVgaEndDarkBoot called\n");
	return CGOS_ERROR;
  }


/***********************************************************************
 * void exitVgaSubModule_BC(CGOS_DRV_VARS *cdv) 							    
 * 																		
 ***********************************************************************
 * Description: This function is called during driver close and should	
 * 				free allocated resources.							
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int exitVgaSubModule_BC(CGOS_DRV_VARS *cdv)
{
	dbgvga("exitVgaModule called\n");
	return CGOS_SUCCESS;
}
