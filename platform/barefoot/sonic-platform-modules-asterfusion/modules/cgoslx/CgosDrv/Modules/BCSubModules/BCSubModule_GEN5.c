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
#define outl(x,y) __outdword(y,x)
#define outw(x,y) __outword(y,x)
#define inb(x) __inbyte(x)
#define inw(x) __inword(x)
#define inl(x) __indword(x)
#else
#include <intrin.h>
#define outb(x,y) __outbyte(y,x)
#define outl(x,y) __outdword(y,x)
#define outw(x,y) __outword(y,x)
#define inb(x) __inbyte(x)
#define inw(x) __inword(x)
#define inl(x) __indword(x)
#endif
#endif

#include "CgosDrv.h"
#include "CGBC.h"
#include "BCSubModule_GEN5_cfg.h"

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

/*------------------------                                                      
 * Global and static data
 *------------------------
 */                                                                                                                                  
static unsigned int gen5HcnmBaseAddr = HCNM_BASE; 
static unsigned int gen5ChBase = HCC0_BASE;
static unsigned char *gen5ClientNum;				  // 0x01: Reserved client access number for the BIOS.
												  // ToDo: check if there is a different number for Cgos Driver

//***************************************************************************

//#define dbg_gen5_on

#ifdef dbg_gen5_on
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

unsigned int initSubModule_GEN5(CGOS_DRV_VARS *cdv);
unsigned int Probe_GEN5(CGOS_DRV_VARS *cdv);
unsigned int bcCommand_GEN5(unsigned char *cmdDataBPtr, unsigned char  cmdByteCount,
					   unsigned char *rcvDataBPtr, unsigned char  rcvByteCount,
					   unsigned char *retValueBPtr );
unsigned int CgosCgbcGetInfo_GEN5(CGOS_DRV_VARS *cdv);
unsigned int CgosCgbcSetControl_GEN5(CGOS_DRV_VARS *cdv);					
unsigned int CgosCgbcReadWrite_GEN5(CGOS_DRV_VARS *cdv);
unsigned int exitSubModule_GEN5(CGOS_DRV_VARS *cdv);
					
//***************************************************************************
//		Helper Functions for Gen5 bcCommand

unsigned int OccupyGen5Hcnm(void) //MOD HMI
{
	unsigned int i = 0; //MOD HMI
	dbgbc("OccupyGen5Hcnm called\n");
	for (;;)
	{
		while (inw(gen5HcnmBaseAddr + GEN5_HCNM_STATUS) != GEN5_HCNM_FREE)
		{
			//MOD HMI v
			if (i > 100000)
			{
				return CGOS_ERROR;
			}
			i++;
			//MOD HMI ^
			//bsy
		}
		if (inl(gen5HcnmBaseAddr + GEN5_HCNM_ACCESS) == GEN5_HCNM_GAINED)
		{
			return CGOS_SUCCESS; //MOD HMI
		}
		else
		{
			return CGOS_ERROR; //MOD HMI
		}
	}
}

unsigned int Gen5HcnmCommand(unsigned int ui8Cmd)
{
	dbgbc("Gen5HcnmCommand called\n");
	while (inb(gen5HcnmBaseAddr + GEN5_HCNM_COMMAND) != GEN5_HCNM_IDLE)
	{
		//bsy
	}
	outb(ui8Cmd, gen5HcnmBaseAddr + GEN5_HCNM_COMMAND);
	while (inb(gen5HcnmBaseAddr + GEN5_HCNM_COMMAND) != GEN5_HCNM_IDLE)
	{
		//bsy
	}
	return (inb(gen5HcnmBaseAddr + GEN5_HCNM_DATA));
}

unsigned int ObtainGen5ClientNumber(void)
{
	unsigned int retSuccess = CGOS_SUCCESS;	//MOD HMI
	dbgbc("ObtainGen5ClientNumber called\n");
	retSuccess = OccupyGen5Hcnm();

	if (retSuccess == CGOS_SUCCESS)
	{
		gen5ClientNum = OsaMemAlloc(sizeof(unsigned int));
		*gen5ClientNum = Gen5HcnmCommand(GEN5_HCNM_REQUEST);
		outw(GEN5_HCNM_FREE, gen5HcnmBaseAddr + GEN5_HCNM_STATUS);

		if ((*gen5ClientNum < 0x02) || (*gen5ClientNum > 0xFE))
		{
			dbgbc("CGBC_ERR: noHcnMsg\n");
			retSuccess = CGOS_ERROR;	//MOD HMI
		}
	}
	return retSuccess; //MOD HMI
}

void ReleaseGen5ClientNumber(void)
{
	unsigned char result;

	dbgbc("ReleaseGen5ClientNumber called\n");
	OccupyGen5Hcnm();
	result = Gen5HcnmCommand(*gen5ClientNum);
	outw(GEN5_HCNM_FREE, gen5HcnmBaseAddr + GEN5_HCNM_STATUS);
	if (result != *gen5ClientNum)
	{
		dbgbc("CGBC_ERR: release client number failed\n");
	}
	OsaMemFree(gen5ClientNum);
}
//***************************************************************************

/***********************************************************************
 * unsigned int RegisterBCSubModule_GEN5									
 * 																		
 ***********************************************************************
 * Description: This function uses the RegisterSubModule callback to	
 * 			  	register the SubModule functions.						
 * 																		
 * Last Change: 20.09.2019 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int RegisterBCSubModule_GEN5(unsigned int (*RegisterSubModule)(	CgosFunctionPointer initSubModule,
                                                CgosFunctionPointer Probe,
										        CgosCommandPointer bcCommand,
 								                CgosFunctionPointer BCGetInfo,
								                CgosFunctionPointer BCSetControl,
 								                CgosFunctionPointer BCReadWrite,
 								                CgosFunctionPointer exitSubModule,
										unsigned char ModuleNumber),
								                unsigned char Number)
{
	return RegisterSubModule(   initSubModule_GEN5,
								Probe_GEN5,
								bcCommand_GEN5,
								CgosCgbcGetInfo_GEN5,
								NULL, //CgosCgbcSetControl_GEN5,
								NULL, //CgosCgbcReadWrite_GEN5,
								exitSubModule_GEN5,
								Number);
}

/***********************************************************************
 * unsigned int initSubModule_GEN5(CGOS_DRV_VARS *cdv) 
 *
 * Inputs:
 * 	CGOS_DRV_VARS * cdv
 *
 * Outputs:
 * 	-
 *
 * Return:
 * 	CGOS_SUCCESS or CGOS_ERROR
 *
 ***********************************************************************
 * Description: This function is used to initialize this module.
 *
 * Last Change: 21.10.2019 HMI
 *
 * Modifications:
 ***********************************************************************/
unsigned int initSubModule_GEN5(CGOS_DRV_VARS *cdv)
{
	unsigned int retSuccess = CGOS_SUCCESS;
	dbgbc("initBCSubModule_GEN5 called\n");

	retSuccess = ObtainGen5ClientNumber();
	return retSuccess;
}

/***********************************************************************
 * unsigned int Probe_GEN5(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description: This function can be used to see if GEN5 BC is
 *              present.		
 * 																		
 * Last Change: 09.09.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int Probe_GEN5(CGOS_DRV_VARS *cdv)
{
	return CGOS_ERROR;
}

/***********************************************************************
 * unsigned int bcCommand_GEN5(unsigned char *cmdDataBPtr,					
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
 * Last Change: 20.09.2019 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
 unsigned int bcCommand_GEN5(unsigned char *cmdDataBPtr, unsigned char  cmdByteCount,
						   unsigned char *rcvDataBPtr, unsigned char  rcvByteCount,
					       unsigned char *retValueBPtr )
 { 
	unsigned int retSuccess = CGOS_SUCCESS;
    unsigned char   sts = 0x00;
    volatile unsigned char   chk, chksum;
    unsigned int  uiTmp;
    unsigned int  i, uiModChgIdx=0;

	unsigned int Timeout = CGBC_TIMEOUT_VALUE;

	dbgbc("bcCommand_GEN5 called\n");

    // Request access.
    do
    {       
        outb(*gen5ClientNum, gen5ChBase + GEN5_HCC_ACCESS);
        // TODO:  Check whether back-to-back write and read back is okay.
        uiTmp = inb( gen5ChBase + GEN5_HCC_ACCESS );

		Timeout--;
    } while( (uiTmp != *gen5ClientNum) && (Timeout > 0) );
	if (Timeout <= 0)
	{
		dbgbc("ERR: Timeout 1\n");
		return CGOS_ERROR;
	}
	Timeout = CGBC_TIMEOUT_VALUE;

    // Write command packet.
	while ((inb(gen5ChBase + GEN5_HCC_STROBE) != 0) && (Timeout > 0))
	{
		Timeout--;
	}
	if (Timeout <= 0)
	{
		dbgbc("ERR: Timeout 2\n");
		return CGOS_ERROR;
	}

    uiModChgIdx = -1;
    if( cmdByteCount <= 2 )
    {
        outb(0x00 | GEN5_HCC_INDEX_CBM_MAN8, gen5ChBase + GEN5_HCC_INDEX );
    }
    else
    {
        outb(0x00 | GEN5_HCC_INDEX_CBM_AUTO32 , gen5ChBase + GEN5_HCC_INDEX);
        if( (cmdByteCount % 4) != 0x03 )
        {
            uiModChgIdx = (cmdByteCount & 0xFFFC) - 1;
        }
    }
    for( chksum=0, i=0; i<cmdByteCount; i++ )
    {
        outb(cmdDataBPtr[i] , gen5ChBase + GEN5_HCC_DATA + (i % 4));
        chksum ^= cmdDataBPtr[i];
        if( i == uiModChgIdx )
        {
            outb((i + 1) | GEN5_HCC_INDEX_CBM_MAN8, gen5ChBase + GEN5_HCC_INDEX );
        }
    }

    // Append checksum byte.
    outb(chksum, gen5ChBase + GEN5_HCC_DATA + (i % 4) );

    // Perform command strobe.
    outb(*gen5ClientNum , gen5ChBase + GEN5_HCC_STROBE);

    // Rewind HCC buffer index.
    outb(0x00 | GEN5_HCC_INDEX_CBM_AUTO32, gen5ChBase + GEN5_HCC_INDEX );

    // Wait for command complete.
	Timeout = CGBC_TIMEOUT_VALUE;
    while(( inb( gen5ChBase + GEN5_HCC_STROBE ) != 0) && (Timeout > 0))
	{
		Timeout--;
	}
	if (Timeout <= 0)
	{
		return CGOS_ERROR;
	}

    // Determine command status.  Note!  It is not necessary to rewind the
    // GEN5_HCC_INDEX here because this is automatically done by the BC before
    // notifying 'command complete'. */
    sts = inb( gen5ChBase + GEN5_HCC_DATA );
    chksum = sts;
  
    switch( sts & CGBC_STAT_MSK )
    {
        case CGBC_DAT_STAT: 
			dbgbc("case dat stat\n");
            if( sts > rcvByteCount )
            {
                sts = rcvByteCount;
            }
            for( i=0; i<sts; i++ )
            {
                rcvDataBPtr[i] = inb( gen5ChBase + GEN5_HCC_DATA + ((i+1) % 4) );
                chksum ^= rcvDataBPtr[i];
            }
            chk = inb( gen5ChBase + GEN5_HCC_DATA + ((i+1) % 4) );
            sts = sts & CGBC_DAT_CNT_MSK;
            break;

      case CGBC_RDY_STAT:
		  dbgbc("case ready stat\n");
          chk = inb( gen5ChBase + GEN5_HCC_DATA + 1 );
          sts = sts & CGBC_ERR_COD_MSK;
          break;
  
      case CGBC_ERR_STAT:
		  dbgbc("case err stat\n");
          chk = inb( gen5ChBase + GEN5_HCC_DATA + 1 );
          sts = sts & CGBC_ERR_COD_MSK;
          retSuccess = CGOS_ERROR;
          break;
  
      default:
		  dbgbc("case default\n");
          chk = inb( gen5ChBase + GEN5_HCC_DATA + 1 );
          sts = sts & CGBC_ERR_COD_MSK;
          retSuccess = CGOS_ERROR;
          break;
    }

    // Release HCC.
    outb(*gen5ClientNum , gen5ChBase + GEN5_HCC_ACCESS);

    // Final checksum verification.
    if( (retSuccess == CGOS_SUCCESS) && (chksum != chk) )
    {
		dbgbc("ERR: bcCommand_GEN5 failed\n");
        retSuccess = CGOS_ERROR;
    }
      
    // Return board controller command status
    *retValueBPtr = sts;
    return( retSuccess ); 
}
 
/***********************************************************************
 * unsigned int CgosCgbcGetInfo_GEN5(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 20.09.2019 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosCgbcGetInfo_GEN5(CGOS_DRV_VARS *cdv)						
  {    
	CGOSBCINFO bcInfo = {0};
	bcInfo.size = sizeof(CGOSBCINFO);

	dbgbc("CgosCgbcGetInfo_GEN5 called\n");
	OsaMemCpy(cdv->pout, &bcInfo, sizeof(CGOSBCINFO));
	cdv->retcnt += sizeof(CGOSBCINFO); 

	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosCgbcSetControl_GEN5(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 20.09.2019 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/    
unsigned int CgosCgbcSetControl_GEN5(CGOS_DRV_VARS *cdv)					
  { 
	dbgbc("CgosCgbcSetControl_GEN5 called\n");	
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosCgbcReadWrite_GEN5(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description:				
 * 																		
 * Last Change: 20.09.2019 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosCgbcReadWrite_GEN5(CGOS_DRV_VARS *cdv)					
  { 
	dbgbc("CgosCgbcReadWrite_GEN5 called\n");
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int exitSubModule_GEN5(CGOS_DRV_VARS *cdv)
 *
 * Inputs:
 * 	CGOS_DRV_VARS * cdv
 *
 * Outputs:
 * 	-
 *
 * Return:
 * 	CGOS_SUCCESS or CGOS_ERROR
 *
 ***********************************************************************
 * Description: This function is used to initialize this module.
 *
 * Last Change: 21.10.2019 HMI
 *
 * Modifications:
 ***********************************************************************/
unsigned int exitSubModule_GEN5(CGOS_DRV_VARS *cdv)
{
	dbgbc("exitSubModule_GEN5 called\n");

	ReleaseGen5ClientNumber();
	return CGOS_SUCCESS;
}
