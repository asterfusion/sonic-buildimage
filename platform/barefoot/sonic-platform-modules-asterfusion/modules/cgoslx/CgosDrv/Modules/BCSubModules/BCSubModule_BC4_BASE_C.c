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
#include <asm/io.h>
#else
#ifdef _WIN64
#pragma intrinsic(__outbyte, __inbyte)
#include <intrin.h>
#define outb(x,y) __outbyte(y,x)
#define inb(x) __inbyte(x)
#else
#include <intrin.h>
#define outb(x,y) __outbyte(y,x)
#define inb(x) __inbyte(x)
#endif
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
#include "BCSubModule_BC4_cfg.h"
//***************************************************************************

//#define dbg_bc4_on

#ifdef dbg_bc4_on
#ifdef __linux__
#define dbgbc(x) printk(x)
#elif _WIN64
#define dbgbc(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#elif _WIN32
#define dbgbc(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#endif
#else
	#define dbgbc(x)
#endif

//***************************************************************************
// Function prototypes for register function

//unsigned int initSubModule_BC4_BASE_C(CGOS_DRV_VARS *cdv);
unsigned int Probe_BC4_BASE_C(CGOS_DRV_VARS *cdv);
unsigned int bcCommand_BC4_BASE_C(unsigned char *cmdDataBPtr, unsigned char  cmdByteCount,
					   unsigned char *rcvDataBPtr, unsigned char  rcvByteCount,
					   unsigned char *retValueBPtr );
unsigned int CgosCgbcGetInfo_BC4_BASE_C(CGOS_DRV_VARS *cdv);
unsigned int CgosCgbcSetControl_BC4_BASE_C(CGOS_DRV_VARS *cdv);					
unsigned int CgosCgbcReadWrite_BC4_BASE_C(CGOS_DRV_VARS *cdv);
//unsigned int exitSubModule_BC4_BASE_C(CGOS_DRV_VARS *cdv);
					
//***************************************************************************

/***********************************************************************
 * unsigned int RegisterBCSubModule_BC4_BASE_C								
 * 																		
 ***********************************************************************
 * Description: This function uses the RegisterSubModule callback to	
 * 			  	register the SubModule functions.						
 * 																		
 * Last Change: 25.06.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int RegisterBCSubModule_BC4_BASE_C(unsigned int (*RegisterSubModule)(	CgosFunctionPointer initSubModule,
		                                                                CgosFunctionPointer Probe,
																		CgosCommandPointer bcCommand,
 								                                        CgosFunctionPointer BCGetInfo,
								                                        CgosFunctionPointer BCSetControl,
 								                                        CgosFunctionPointer BCReadWrite,
 								                                        CgosFunctionPointer exitSubModule,
																		unsigned char ModuleNumber),
								                    unsigned char Number)
{
	return RegisterSubModule(	NULL, //initSubModule
	                            Probe_BC4_BASE_C,
								bcCommand_BC4_BASE_C,
								CgosCgbcGetInfo_BC4_BASE_C,
								NULL, //CgosCgbcSetControl_BC4_BASE_C,
								NULL, //CgosCgbcReadWrite_BC4_BASE_C,
								NULL, //exitSubModule
								Number);
}

/***********************************************************************
 * unsigned int Probe_BC4_BASE_C(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description: This function can be used to see if BC4 Base C is
 *              present.		
 * 																		
 * Last Change: 09.09.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int Probe_BC4_BASE_C(CGOS_DRV_VARS *cdv)
{
	/*unsigned int BASE = 0x0C38;
	unsigned char rbuf[136];
	unsigned int i;
	
	printk("CgosDrv: Probe_BC4_BASE_C called\n");
	
	for(i = 0; i < 136; i++)
	{
		rbuf[i] = inb(BASE + i);
		printk("C%d: %x\n",i,rbuf[i]);
	}
	
	if(((rbuf[16] == 0x0C) && (rbuf[17] == 0x80)) || ((rbuf[18] == 0x0C)  && (rbuf[19] == 0x80)))
	{
		return CGOS_SUCCESS;
	}
	*/
	return CGOS_ERROR;
}

/***********************************************************************
 * unsigned int bcCommand_BC4_BASE_C(unsigned char *cmdDataBPtr,					
 *						 	  unsigned char  cmdByteCount,					
 *					      	  unsigned char *rcvDataBPtr,					
 *					     	  unsigned char  rcvByteCount,					
 *					     	  unsigned char *retValueBPtr )	  				
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
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int bcCommand_BC4_BASE_C(unsigned char *cmdDataBPtr, unsigned char  cmdByteCount,
						   unsigned char *rcvDataBPtr, unsigned char  rcvByteCount,
					       unsigned char *retValueBPtr )
{
	unsigned char accessNumber = 1;
    unsigned int retSuccess = 0;
    unsigned char sts = 0x00;
    unsigned char i=0;
    unsigned char checksum = 0;
    unsigned int BASE = 0x0C80;
	unsigned char ret = 0;
    	unsigned int curStrobe;
	unsigned int curChBase;

	curStrobe = BASE + 0x40 - 1; // HIF_EX_BASE_DFLT + 0x40 - 1;
	curChBase = BASE; // HIF_EX_BASE_DFLT;
    
	dbgbc("bcCommand_BC4 called\n");
    while(inb(curStrobe))
    {
		i++;
		if(i == 1000000)
        {
			dbgbc("CGBC timeout error (0)\n");
			return CGOS_ERROR;
		}
	}
	for( i=0; i<cmdByteCount; i++ )
    {
		outb(*(cmdDataBPtr + i), curChBase + i);
        checksum = checksum ^ (*(cmdDataBPtr + i));        				// Calculate checksum
    }
	outb(checksum, curChBase + cmdByteCount);							// Add checksum after data 
	outb(accessNumber, curStrobe);										// Send command.                 
	
	i = 0;
    do
    {
        sts = inb(curStrobe);
        i++;
        if(i == 1000000)
        {
			dbgbc("CGBC timeout error (1)\n");
			return CGOS_ERROR;
		}
    } while( sts != 0 );
    checksum = 0;                                              			// Clear checksum
    sts = inb(curChBase);
    checksum = checksum ^ sts;                                 			// Calculate incoming package checksum

    switch( sts & CGBC_STAT_MSK )
    {
        case CGBC_RDY_STAT:
            sts = sts & CGBC_ERR_COD_MSK;
            if(checksum != inb(curChBase+1))              				// Verify input package checksum
            {
                sts = CGBC_ERR_OUT_OF_SYNC;
				dbgbc("CGBC_ERR_OUT_OF_SYNC (0)");
                retSuccess = 1;
            }
            break;
       
        case CGBC_DAT_STAT: 
            // TODO: Check length  
            for( i=0; i<sts; i++ ) 
            {            
					ret = inb(curChBase+1+i);
					*(rcvDataBPtr+i) = ret;
					checksum = checksum ^ (*(rcvDataBPtr+i));      		// Calculate incoming package checksum	
            }
            sts = sts & 0x3F;       						
            if(checksum != inb(curChBase+sts+1))          				// Verify input package checksum
            {
                sts = CGBC_ERR_OUT_OF_SYNC;
				dbgbc("CGBC_ERR_OUT_OF_SYNC (1)");
                retSuccess = 1;
            }
            break;          

        case CGBC_ERR_STAT: 
			dbgbc("CGBC_ERR_STAT\n");
        default:
            sts = sts & CGBC_ERR_COD_MSK;
			dbgbc("CGBC default STAT");
            retSuccess = 1;    
            break;
    }

    if(retSuccess == 0) 
    {
		dbgbc("bcCommand_BC4 success\n");
		*retValueBPtr = sts;											
		//*retValueBPtr = (sts-1)|CGBC_RDY_STAT|CGBC_DAT_PENDING; 		// !!!! CGEB faked Status!!!!  				
		return CGOS_SUCCESS;
	}
    else 
    {
		dbgbc("bcCommand_BC4 error\n");
		*retValueBPtr = sts;
		return CGOS_ERROR;
	}
}

/***********************************************************************
 * unsigned int CgosCgbcGetInfo_BC4_BASE_C(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosCgbcGetInfo_BC4_BASE_C(CGOS_DRV_VARS *cdv)						
  {    
	CGOSBCINFO bcInfo = {0};
	bcInfo.size = sizeof(CGOSBCINFO);
	dbgbc("CgosCgbcGetInfo_BC4 called\n");
	
        OsaMemCpy(cdv->pout, &bcInfo, sizeof(CGOSBCINFO));
	cdv->retcnt += sizeof(CGOSBCINFO); 

	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosCgbcSetControl_BC4_BASE_C(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/    
unsigned int CgosCgbcSetControl_BC4_BASE_C(CGOS_DRV_VARS *cdv)					
  { 
	dbgbc("CgosCgbcSetControl_BC4 called\n");	
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosCgbcReadWrite_BC4_BASE_C(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 28.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosCgbcReadWrite_BC4_BASE_C(CGOS_DRV_VARS *cdv)					
  { 
	dbgbc("CgosCgbcReadWrite_BC4 called\n");
	return CGOS_SUCCESS;
  }

