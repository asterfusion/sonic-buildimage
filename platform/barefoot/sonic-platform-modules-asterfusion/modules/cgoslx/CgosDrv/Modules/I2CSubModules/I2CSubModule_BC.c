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
//MOD23 v
#include "I2CSubModule_BC_cfg.h"
#include "I2CSubModule_GEN5_BC_cfg.h"
//MOD23 ^

//***************************************************************************

//#define dbg_i2c_sub_bc_on

#ifdef dbg_i2c_sub_bc_on
#ifdef __linux__
#define dbgi2c(x) printk(x)
#elif _WIN64
#define dbgi2c(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#elif _WIN32
#define dbgi2c(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#endif
#else
#define dbgi2c(x)
#endif

//***************************************************************************
// Function prototypes for register function

unsigned int initI2CSubModule_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosI2CCount_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosI2CIsAvailable_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosI2CType_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosI2CRead_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosI2CWrite_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosI2CReadRegister_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosI2CWriteRegister_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosI2CWriteReadCombined_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosI2CGetMaxFrequency_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosI2CGetFrequency_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosI2CSetFrequency_BC(CGOS_DRV_VARS *cdv); 
unsigned int exitI2CSubModule_BC(CGOS_DRV_VARS *cdv);

//***************************************************************************

/***********************************************************************
 * unsigned int RegisterI2CSubModule_BC									
 * 																		
 ***********************************************************************
 * Description: This function uses the RegisterSubModule callback to	
 * 			  	register the SubModule functions.						
 * 																		
 * Last Change: 03.04.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int RegisterI2CSubModule_BC(unsigned int (*RegisterSubModule)(	CgosFunctionPointer initSubModule,
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
																		unsigned char ModuleNumber),
								    unsigned char Number)
{
	return RegisterSubModule(	initI2CSubModule_BC,
								CgosI2CCount_BC,
								CgosI2CType_BC,
								CgosI2CIsAvailable_BC,
								CgosI2CRead_BC,
								CgosI2CWrite_BC,
								CgosI2CReadRegister_BC,
								CgosI2CWriteRegister_BC,
								CgosI2CWriteReadCombined_BC,
								CgosI2CGetMaxFrequency_BC,
								CgosI2CGetFrequency_BC,
								CgosI2CSetFrequency_BC,
								exitI2CSubModule_BC,
								Number);
}

/***********************************************************************
 * unsigned int initI2CSubModule_BC(CGOS_DRV_VARS *cdv)			
 * 																		
 ***********************************************************************
 * Description: This function initializes this SubModule.				
 * 																		
 * Last Change: 03.04.2018 HMI										
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initI2CSubModule_BC(CGOS_DRV_VARS *cdv)
{
	unsigned int retSuccess = CGOS_SUCCESS;
    dbgi2c("initI2CModule called\n");
	if ((cdv->brd->BC_Type == BC_TYPE_BC4_BASE_C)||(cdv->brd->BC_Type == BC_TYPE_BC4_BASE_E))
	{
		if (I2C_COUNT <= CGOS_DRV_I2C_TABLE_MAX)     //CGOS_DRV_I2C_TABLE_MAX defined in DrvVars.h
		{
			cdv->brd->i2cCount = I2C_COUNT;
			OsaMemCpy(cdv->i2c, &I2C_TABLE, I2C_COUNT * sizeof(CGOS_DRV_I2C_ENTRY));    //May cause alignment problems
		}
		else
		{
			retSuccess = CGOS_ERROR;
		}
	}
	else if (cdv->brd->BC_Type == BC_TYPE_GEN5)
	{
		if (I2C_GEN5_COUNT <= CGOS_DRV_I2C_TABLE_MAX)     //CGOS_DRV_I2C_TABLE_MAX defined in DrvVars.h
		{
			cdv->brd->i2cCount = I2C_GEN5_COUNT;
			OsaMemCpy(cdv->i2c, &I2C_GEN5_TABLE, I2C_GEN5_COUNT * sizeof(CGOS_DRV_I2C_ENTRY));    //May cause alignment problems
		}
		else
		{
			retSuccess = CGOS_ERROR;
		}
	}
	else
	{
		retSuccess = CGOS_ERROR;
	}
    return retSuccess;
}

/***********************************************************************
 * unsigned int CgosI2CCount_BC(CGOS_DRV_VARS *cdv)                       
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
unsigned int CgosI2CCount_BC(CGOS_DRV_VARS *cdv)
  {
    dbgi2c("CgosI2CCount_BC called\n");
    cdv->cout->rets[0]=cdv->brd->i2cCount;
    return CGOS_SUCCESS;
  }
 
/***********************************************************************
 * unsigned int CgosI2CIsAvailable_BC(CGOS_DRV_VARS *cdv)                 
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
unsigned int CgosI2CIsAvailable_BC(CGOS_DRV_VARS *cdv)
  {
    dbgi2c("CgosI2CIsAvailable_BC called\n");
    cdv->cout->rets[0]=cdv->cin->type<cdv->brd->i2cCount;
    return CGOS_SUCCESS;
  } 

/***********************************************************************
 * I2CBusGetStatus(unsigned char bus, unsigned char *pSts)              
 *                                                                     
 * Inputs:                                                              
 *   bus                                                                
 *   pSts                                                               
 *                                                                      
 * Output:                                                              
 *   Writes status of bus to pSts                                       
 *                                                                      
 ***********************************************************************
 * Description: Gets status of the requested i2c bus.                   
 *                                                                      
 * Last Change: 05-Apr-2022 HMI                                          
 *                                                                      
 * Modifications: -MOD24: Removed one value array                                                       
 ***********************************************************************/ 
unsigned int I2CBusGetStatus(unsigned char bus, unsigned char *pSts)
  {
    unsigned char wbuf; //MOD24
    dbgi2c("I2CBusGetStatus called\n");
    if( bus & ~CGBC_I2C_BUS_MSK )
    {
        // Invalid bus number
        return CGOS_ERROR;
    }
    wbuf = CGBC_CMD_I2C_STAT | bus; //MOD24
    return bcCommand(&wbuf, 1, NULL, 0, pSts); //MOD24
  }

/***********************************************************************
 * unsigned int CgosI2CType_BC(CGOS_DRV_VARS *cdv)                        
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
unsigned int CgosI2CType_BC(CGOS_DRV_VARS *cdv)
  { 
    unsigned int retSuccess = CGOS_SUCCESS;
    dbgi2c("CgosI2CType_BC called\n");
    if(cdv->cin->type >= cdv->brd->i2cCount)
    {
        retSuccess = CGOS_ERROR;
    }
    else
    {
        cdv->cout->rets[0] = cdv->i2c[cdv->cin->type].cgostype;
    }
    return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosI2CRead_BC(CGOS_DRV_VARS *cdv)                        
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
 * Last Change: 10-Feb-2022 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/ 
 
unsigned int CgosI2CRead_BC(CGOS_DRV_VARS *cdv)
  {
    unsigned char wbuf[5];
    unsigned char sts;
    unsigned char retSuccess = CGOS_SUCCESS;
    unsigned char splitBlockSize;
    unsigned char i;
    unsigned char cnt = cdv->lout;
    unsigned char addr = cdv->cin->pars[0];
    unsigned char bus = cdv->i2c[cdv->cin->type].cgbctype;

    dbgi2c("CgosI2CRead_BC called\n");
        
    if(cdv->cin->type >= cdv->brd->i2cCount)
    {
        retSuccess = CGOS_ERROR;
    }
    else
    {   
        wbuf[0] = CGBC_CMD_I2C_START | bus;
        wbuf[1] = CGBC_I2C_SEND_START | 1;
        wbuf[2] = 0;
        wbuf[3] = addr & 0xFE; //MOD23
        retSuccess = bcCommand( &wbuf[0], 4, NULL, 0, &sts );
        if(retSuccess == CGOS_SUCCESS)
        {
            do
            {   
                retSuccess = I2CBusGetStatus(bus, &sts );
            }while((retSuccess == CGOS_SUCCESS) && (sts == CGBC_I2C_STAT_BSY));
                    
            if(sts  == CGBC_I2C_STAT_IDL)
            {
                splitBlockSize = 31;
                // Limitation due to CGBC Command Status Byte, 31 is the maximum read value the 
                // Board Controller can return in RC[4:0].
                //    ___________________________________
                //   | 7   6  |  5  |  4   3   2   1   0 |
                //   |________|_____|____________________|
                //   |ST[1:0] | RES |      RC[4:0]       |
                //   |________|_____|____________________|
                // 
                // ST[1:0]	command state: 00 = DATA
                //                             01 = READY
                //                             10 = BUSY
                //                             11 = ERROR
                // RES		reserved/normally return 0/maybe used as RC[5] with GEN5 cBC
                // RC[4:0]	command return code command state is READY
                //              data count when command state is DATA
                //              error code when command state is ERROR
                
#ifdef __linux__
                for(i = cnt; (retSuccess == CGOS_SUCCESS) && i; /*(unsigned char*)*/cdv->pout += splitBlockSize)
#else
		for (i = cnt; (retSuccess == CGOS_SUCCESS) && i; (unsigned char*)cdv->pout += splitBlockSize)
#endif
                {
                    wbuf[0] = CGBC_CMD_I2C_START | bus;
                    if(i == cnt)
                    {
                        wbuf[1] = CGBC_I2C_SEND_START | 1;
                        wbuf[3] = addr | 0x01;
                    }
                    else
                    {
                        wbuf[1] = 0;
                    }
                            
                    if(i <= splitBlockSize) 
                    {
                        wbuf[1] |= CGBC_I2C_SEND_STOP;
                        wbuf[2] = i;
                    }
                    else
                    {
                        wbuf[2] = CGBC_I2C_SEND_LAST_ACK | splitBlockSize;
                    }
                    retSuccess = bcCommand(&wbuf[0], (unsigned char)((cnt == i)?4:3), NULL, 0, &sts);
                    if(retSuccess == CGOS_SUCCESS)
                    {
                        do
                        {
                            retSuccess = I2CBusGetStatus( bus, &sts );
                        } while( (retSuccess == CGOS_SUCCESS) && (sts == CGBC_I2C_STAT_BSY) );
                        if( sts == CGBC_I2C_STAT_DAT )
                        {
                            wbuf[0] = CGBC_CMD_I2C_DATA | bus;
                            retSuccess = bcCommand(&wbuf[0], 1, cdv->pout, (unsigned char)(wbuf[2] & 0x3F), &sts);
                            if(retSuccess == CGOS_SUCCESS)
                            {
                                if(sts == (wbuf[2] & 0x3F))
                                {
                                    i -= wbuf[2] & 0x3F;
                                }
                                else
                                {
                                    retSuccess = CGOS_ERROR;
                                }
                            }
                        }
                    }
                }
            }       
        }
    }
    cdv->retcnt += cdv->lout;
    return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosI2CWrite_BC(CGOS_DRV_VARS *cdv)                       
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
 * Last Change: 10-Feb-2022 HMI                                          
 *                                                                      
 * Modifications:                                                       
 ***********************************************************************/ 
unsigned int CgosI2CWrite_BC(CGOS_DRV_VARS *cdv)
  {
    unsigned char wbuf[4+32];
    unsigned char inputBuf[900] = {0};
    unsigned int  index=0;
    unsigned char sts;
    unsigned char retSuccess = CGOS_SUCCESS;
    unsigned char splitBlockSize;
    unsigned char thisCnt;
    unsigned char i,j;
    unsigned char len = cdv->lin;

    dbgi2c("CgosI2CWrite_BC called\n");
    if(cdv->cin->type >= cdv->brd->i2cCount)
    {
        retSuccess = CGOS_ERROR;
    }
    else
    {
    	splitBlockSize = 32;	//MOD24
        if( len <= splitBlockSize )
        {
            thisCnt = len;
        }
        else
        {
            thisCnt = splitBlockSize;
        }
        wbuf[0] = CGBC_CMD_I2C_START | cdv->i2c[cdv->cin->type].cgbctype;
        wbuf[1] = CGBC_I2C_SEND_START | (1 + thisCnt);
        if( thisCnt == len)
        {
            wbuf[1] |= CGBC_I2C_SEND_STOP;
        }
        wbuf[2] = 0;
        wbuf[3] = cdv->cin->pars[0];
    
    	//v MOD24
    	if(len > 900)
    	{
    		return CGOS_ERROR;
    	}
    	OsaMemCpy(&inputBuf[0],cdv->pin,len);
        for (i=0;i<thisCnt;i++)
        {
        	wbuf[4+i]=inputBuf[i];
        }
        //^ MOD24
        
        retSuccess = bcCommand( &wbuf[0], (unsigned char)(4+thisCnt), NULL, 0, &sts );

        if( retSuccess==CGOS_SUCCESS )
        {
            do
            {
                retSuccess = I2CBusGetStatus( cdv->i2c[cdv->cin->type].cgbctype, &sts );
            } while( (retSuccess == CGOS_SUCCESS) && (sts == CGBC_I2C_STAT_BSY) );

            if( sts == CGBC_I2C_STAT_IDL )
            {
                index += thisCnt; //MOD24
		for (i = (cdv->lin) - thisCnt; (retSuccess == CGOS_SUCCESS) && i; index += splitBlockSize) //MOD24
                {
                    if( i > splitBlockSize )
                    {
                        thisCnt = splitBlockSize;
                        wbuf[1] = splitBlockSize;
                    }
                    else
                    {
                        thisCnt = i;
                        wbuf[1] = i | CGBC_I2C_SEND_STOP;
                    }
                    //v MOD24
                    for(j = 0 ; j < thisCnt; j++)
                    {
                    	wbuf[3+j]=inputBuf[index+j];
                    }
                    //^ MOD24

                    retSuccess = bcCommand( &wbuf[0], (unsigned char)(3+thisCnt), NULL, 0, &sts ); 
                    if( retSuccess==CGOS_SUCCESS )
                    {
                        do
                        {
                            retSuccess = I2CBusGetStatus( cdv->i2c[cdv->cin->type].cgbctype, &sts );
                        } while( (retSuccess == CGOS_SUCCESS) && (sts == CGBC_I2C_STAT_BSY) );
                        if( sts == CGBC_I2C_STAT_IDL )
                        {
                            i = i - thisCnt;
                            retSuccess = CGOS_SUCCESS;
                        }
                        else
                        {
                            retSuccess = CGOS_ERROR;
                        }
                    }
                }
            }
            else
            {
                retSuccess =  CGOS_ERROR;
            }
        } 
    }
    return retSuccess;
  }
/***********************************************************************
 * unsigned int CgosI2CReadRegisterRaw_BC(unsigned long dwUnit,
 *                                        unsigned char bAddr,
 *                                        unsigned char wReg,
 *                                        unsigned char *PDataByte)
 *
 ***********************************************************************
 * Cgos Function: CgosI2CReadRegister(HCGOS hCgos,
 *                                    unsigned long dwUnit,
 *                                    unsigned char bAddr,
 *                                    unsigned short wReg,
 *                                    unsigned char *pDataByte
 * Inputs:
 * dwUnit  
 * bAddr       
 * wReg      
 *
 * Output:
 * pData Byte  
 *
 ***********************************************************************
 * Description: Reads one byte from a register, for Driver internal use
 *              only, not accessable from upper layers.
 *
 * Last Change:\B408.10.2019 HMI
 *
 * Modifications:
 ***********************************************************************/
  unsigned int CgosI2CReadRegisterRaw_BC(unsigned long dwUnit,
	                                     unsigned char bAddr,
	                                     unsigned char wReg,
	                                     unsigned char *PDataByte)
  {
	  unsigned char wbuf[5];
	  unsigned char sts;
	  unsigned char retSuccess = CGOS_SUCCESS;
	  unsigned char ret;

	  dbgi2c("CgosI2CReadRegisterRaw_BC called\n");
		  wbuf[0] = CGBC_CMD_I2C_START | dwUnit;
		  wbuf[1] = CGBC_I2C_SEND_START | 2;
		  wbuf[2] = 0;
		  wbuf[3] = bAddr & 0xFE;
		  wbuf[4] = wReg;
		  retSuccess = bcCommand(&wbuf[0], 5, NULL, 0, &sts);
		  if (retSuccess == CGOS_SUCCESS)
		  {
			  do
			  {
				  retSuccess = I2CBusGetStatus(dwUnit, &sts);
			  } while ((retSuccess == CGOS_SUCCESS) && (sts == CGBC_I2C_STAT_BSY));
			  if (sts == CGBC_I2C_STAT_IDL)
			  {
				  wbuf[0] = CGBC_CMD_I2C_START | dwUnit;
				  wbuf[1] = CGBC_I2C_SEND_START | CGBC_I2C_SEND_STOP | 1;
				  wbuf[2] = 1;
				  wbuf[3] = bAddr | 0x01;
				  retSuccess = bcCommand(&wbuf[0], 4, NULL, 0, &sts);
				  if (retSuccess == CGOS_SUCCESS)
				  {
					  do
					  {
						  retSuccess = I2CBusGetStatus(dwUnit, &sts);
					  } while ((retSuccess == CGOS_SUCCESS) && (sts == CGBC_I2C_STAT_BSY));
					  if (sts == CGBC_I2C_STAT_DAT)
					  {
						  wbuf[0] = CGBC_CMD_I2C_DATA | dwUnit;
						  retSuccess = bcCommand(&wbuf[0], 1, &ret, 1 & 0x3F, &sts);
						  *PDataByte = ret;
					  }
				  }
			  }
		  }
	  return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosI2CWriteRegisterRaw_BC(unsigned long dwUnit,
 *                                        unsigned char bAddr,
 *                                        unsigned char wReg,
 *                                        unsigned char bData)
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
 * Description: Writes one byte from a register, for Driver internal use
 *              only, not accessable from upper layers.
 *
 * Last Change:\B408.10.2019 HMI
 *

 * Modifications:
 ***********************************************************************/
unsigned int CgosI2CWriteRegisterRaw_BC(unsigned long dwUnit, unsigned char bAddr, unsigned short wReg, unsigned char bData)
  { 
    unsigned char wbuf[6];
    unsigned char sts = 0;
    unsigned char retSuccess = CGOS_SUCCESS;

    dbgi2c("CgosI2CWriteRegister_BC called\n");

        wbuf[0] = CGBC_CMD_I2C_START | dwUnit;
        wbuf[1] = CGBC_I2C_SEND_START | CGBC_I2C_SEND_STOP | 3; 
	wbuf[2] = 0;
        wbuf[3] = bAddr;
        wbuf[4] = wReg;
	wbuf[5] = bData;
        retSuccess = bcCommand( &wbuf[0], 6, NULL, 0, &sts );
    
    return retSuccess;
  }


/***********************************************************************
 * unsigned int CgosI2CReadRegister_BC(CGOS_DRV_VARS *cdv)                
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
unsigned int CgosI2CReadRegister_BC(CGOS_DRV_VARS *cdv)
  { 
    unsigned char wbuf[5];
    unsigned char sts;
    unsigned char retSuccess = CGOS_SUCCESS;
    unsigned char ret;

    dbgi2c("CgosI2CReadRegister_BC called\n"); 
    if(cdv->cin->type >= cdv->brd->i2cCount)
    {
        retSuccess = CGOS_ERROR;
    }
    else
    {
        wbuf[0] = CGBC_CMD_I2C_START | cdv->i2c[cdv->cin->type].cgbctype;
        wbuf[1] = CGBC_I2C_SEND_START | 2;
        wbuf[2] = 0;
		wbuf[3] = cdv->cin->pars[0] & 0xFE;
		wbuf[4] = cdv->cin->pars[1];
        retSuccess = bcCommand( &wbuf[0], 5, NULL, 0, &sts );
        if(retSuccess == CGOS_SUCCESS)
        {
            do
            {
                retSuccess = I2CBusGetStatus( cdv->i2c[cdv->cin->type].cgbctype, &sts );
            } while( (retSuccess == CGOS_SUCCESS) && (sts == CGBC_I2C_STAT_BSY) );
            if( sts == CGBC_I2C_STAT_IDL )
            {
                wbuf[0] = CGBC_CMD_I2C_START | cdv->i2c[cdv->cin->type].cgbctype;
                wbuf[1] = CGBC_I2C_SEND_START | CGBC_I2C_SEND_STOP | 1;
                wbuf[2] = 1;
				wbuf[3] = cdv->cin->pars[0] | 0x01;
                retSuccess = bcCommand( &wbuf[0], 4, NULL, 0, &sts );
                if(retSuccess == CGOS_SUCCESS)
                {
                    do
                    {
                        retSuccess = I2CBusGetStatus( cdv->i2c[cdv->cin->type].cgbctype, &sts );
                    } while( (retSuccess == CGOS_SUCCESS) && (sts == CGBC_I2C_STAT_BSY) );
                    if( sts == CGBC_I2C_STAT_DAT )
                    {
                        wbuf[0] = CGBC_CMD_I2C_DATA | cdv->i2c[cdv->cin->type].cgbctype;
                        retSuccess = bcCommand( &wbuf[0], 1, &ret, 1 & 0x3F, &sts );
                        cdv->cout->rets[0] = ret;
                    }
                }
            }
        }
    }
    return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosI2CWriteRegister_BC(CGOS_DRV_VARS *cdv)               
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
unsigned int CgosI2CWriteRegister_BC(CGOS_DRV_VARS *cdv)
  { 
    unsigned char wbuf[6];
    unsigned char sts = 0;
    unsigned char retSuccess = CGOS_SUCCESS;

    dbgi2c("CgosI2CWriteRegister_BC called\n");
    
    if(cdv->cin->type >= cdv->brd->i2cCount)
    {
        retSuccess = CGOS_ERROR;
    }
    else
    {
        wbuf[0] = CGBC_CMD_I2C_START | cdv->i2c[cdv->cin->type].cgbctype;
        wbuf[1] = CGBC_I2C_SEND_START | CGBC_I2C_SEND_STOP | 3; 
        wbuf[2] = 0;
        wbuf[3] = cdv->cin->pars[0];
        wbuf[4] = cdv->cin->pars[1];
        wbuf[5] = cdv->cin->pars[2];
        retSuccess = bcCommand( &wbuf[0], 6, NULL, 0, &sts );
    }
    return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosI2CWriteReadCombined_BC(CGOS_DRV_VARS *cdv)           
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
 * pBytesWrite  <->     cdv->pin                                        
 * dwLenWrite   <->     cdv->lin                                        
 * dwLenRead    <->     cdv->lout                                      
 *                                                                      
 * Output:                                                              
 * pBytesRead   <->     (unsigned char*)cdv->pout                       
 *                                                                      
 ***********************************************************************
 * Description: Execute a combined write and read action.               
 *                                                                      
 * Last Change: 05-Apr-2022 HMI                                          
 *                                                                      
 * Modifications: - MOD24: Instead of using cBC WriteReadCombined command
 *                         which was limited to 34 Bytes write and 32 
 *                         Bytes read this function now uses the 
 *                         implementation of CgosI2CWrite without stop 
 *                         condition, followed by CgosI2CRead.                                              
 ***********************************************************************/  
unsigned int CgosI2CWriteReadCombined_BC(CGOS_DRV_VARS *cdv)
  { 
    unsigned char wbuf[4+32];
    unsigned char inputBuf[900] = {0};
    unsigned int  index=0;
    unsigned char sts;
    unsigned char retSuccess = CGOS_SUCCESS;
    unsigned char splitBlockSize;
    unsigned char thisCnt;
    unsigned char i,j;
    unsigned char len = cdv->lin;
    unsigned char rbuf[5];
    unsigned char cnt = cdv->lout;
    unsigned char addr = cdv->cin->pars[0];
    unsigned char bus;

    dbgi2c("CgosI2CWriteReadCombined_BC called\n");
    //Write Part
    if(len != 0) //In case 0 Bytes are written, go directly to read part
    {	   
   	    if(cdv->cin->type >= cdv->brd->i2cCount)
    	{
        	retSuccess = CGOS_ERROR;
    	}
    	else
    	{
        	bus = cdv->i2c[cdv->cin->type].cgbctype;
    		splitBlockSize = 32;	
        	if( len <= splitBlockSize )
        	{
        	    thisCnt = len;
        	}
        	else
        	{
        	    thisCnt = splitBlockSize;
        	}
        	wbuf[0] = CGBC_CMD_I2C_START | bus;
        	wbuf[1] = CGBC_I2C_SEND_START | (1 + thisCnt);
        	if( (thisCnt == len) && (cnt == 0)) //In case 0 Bytes are going to be read a stop condition has to be added here
        	{
        		wbuf[1] |= CGBC_I2C_SEND_STOP;
        	}
        	wbuf[2] = 0;
        	wbuf[3] = addr & 0xFE;
    	
    		if(len > 900)
    		{
    			return CGOS_ERROR;
    		}
    		OsaMemCpy(&inputBuf[0],cdv->pin,len);
        	for (i=0;i<thisCnt;i++)
        	{
        		wbuf[4+i]=inputBuf[i];
        	}
        	
        	retSuccess = bcCommand( &wbuf[0], (unsigned char)(4+thisCnt), NULL, 0, &sts );
        	if( retSuccess==CGOS_SUCCESS )
        	{
        	    do
        	    {
        	        retSuccess = I2CBusGetStatus( cdv->i2c[cdv->cin->type].cgbctype, &sts );
        	    } while( (retSuccess == CGOS_SUCCESS) && (sts == CGBC_I2C_STAT_BSY) );
        	    if( sts == CGBC_I2C_STAT_IDL )
        	    {
        	        index += thisCnt;
			for (i = (cdv->lin) - thisCnt; (retSuccess == CGOS_SUCCESS) && i; index += splitBlockSize) 
        	        {
        	            if( i > splitBlockSize )
        	            {
        	                thisCnt = splitBlockSize;
        	                wbuf[1] = splitBlockSize;
        	            }
        	            else
        	            {
        	                thisCnt = i;
        	                if( cnt == 0) //In case 0 Bytes are going to be read a stop condition has to be added here
        	                {
        	                	wbuf[1] = i | CGBC_I2C_SEND_STOP;
        	                }
        	                else
        	                {
        	                	wbuf[1] = i;
        	            	}
        	            }
        	            for(j = 0 ; j < thisCnt; j++)
        	            {
        	            	wbuf[3+j]=inputBuf[index+j];
        	            }
	
        	            retSuccess = bcCommand( &wbuf[0], (unsigned char)(3+thisCnt), NULL, 0, &sts ); 
        	            if( retSuccess==CGOS_SUCCESS )
        	            {
        	                do
        	                {
        	                    retSuccess = I2CBusGetStatus( cdv->i2c[cdv->cin->type].cgbctype, &sts );
        	                } while( (retSuccess == CGOS_SUCCESS) && (sts == CGBC_I2C_STAT_BSY) );
        	                if( sts == CGBC_I2C_STAT_IDL )
        	                {
        	                    i = i - thisCnt;
        	                    retSuccess = CGOS_SUCCESS;
        	                }
        	                else
        	                {
        	                    retSuccess = CGOS_ERROR;
        	                }
        	            }
        	        }
        	    }
        	    else
        	    {
        	        retSuccess =  CGOS_ERROR;
        	    }
        	} 
        }
    }	
//Read Part

    if(cdv->cin->type >= cdv->brd->i2cCount)
    {
        retSuccess = CGOS_ERROR;
    }
    else
    {
    	if(len == 0) //In case 0 Bytes are written, this has to be called first
    	{
    		wbuf[0] = CGBC_CMD_I2C_START | bus;
        	wbuf[1] = CGBC_I2C_SEND_START | 1;
        	wbuf[2] = 0;
        	wbuf[3] = addr & 0xFE; //MOD23
        	retSuccess = bcCommand( &wbuf[0], 4, NULL, 0, &sts );
    	} 
    	  
        if(retSuccess == CGOS_SUCCESS)
        {
            do
            {   
                retSuccess = I2CBusGetStatus(bus, &sts );
            }while((retSuccess == CGOS_SUCCESS) && (sts == CGBC_I2C_STAT_BSY));
                    
            if(sts  == CGBC_I2C_STAT_IDL)
            {
                splitBlockSize = 31;
                // Limitation due to CGBC Command Status Byte, 31 is the maximum read value the 
                // Board Controller can return in RC[4:0].
                //    ___________________________________
                //   | 7   6  |  5  |  4   3   2   1   0 |
                //   |________|_____|____________________|
                //   |ST[1:0] | RES |      RC[4:0]       |
                //   |________|_____|____________________|
                // 
                // ST[1:0]	command state: 00 = DATA
                //                             01 = READY
                //                             10 = BUSY
                //                             11 = ERROR
                // RES		reserved/normally return 0/maybe used as RC[5] with GEN5 cBC
                // RC[4:0]	command return code command state is READY
                //              data count when command state is DATA
                //              error code when command state is ERROR
                
#ifdef __linux__
                for(i = cnt; (retSuccess == CGOS_SUCCESS) && i; /*(unsigned char*)*/cdv->pout += splitBlockSize)
#else
		for (i = cnt; (retSuccess == CGOS_SUCCESS) && i; (unsigned char*)cdv->pout += splitBlockSize)
#endif
                {
                    rbuf[0] = CGBC_CMD_I2C_START | bus;
                    if(i == cnt)
                    {
                        rbuf[1] = CGBC_I2C_SEND_START | 1;
                        rbuf[3] = addr | 0x01;
                    }
                    else
                    {
                        rbuf[1] = 0;
                    }
                            
                    if(i <= splitBlockSize) 
                    {
                        rbuf[1] |= CGBC_I2C_SEND_STOP;
                        rbuf[2] = i;
                    }
                    else
                    {
                        rbuf[2] = CGBC_I2C_SEND_LAST_ACK | splitBlockSize;
                    }
                    retSuccess = bcCommand(&rbuf[0], (unsigned char)((cnt == i)?4:3), NULL, 0, &sts);
                    
                    if(retSuccess == CGOS_SUCCESS)
                    {
                        do
                        {
                            retSuccess = I2CBusGetStatus( bus, &sts );
                        } while( (retSuccess == CGOS_SUCCESS) && (sts == CGBC_I2C_STAT_BSY) );
                        if( sts == CGBC_I2C_STAT_DAT )
                        {
                            rbuf[0] = CGBC_CMD_I2C_DATA | bus;
                            retSuccess = bcCommand(&rbuf[0], 1, cdv->pout, (unsigned char)(rbuf[2] & 0x3F), &sts);
                            if(retSuccess == CGOS_SUCCESS)
                            {
                                if(sts == (rbuf[2] & 0x3F))
                                {
                                    i -= rbuf[2] & 0x3F;
                                }
                                else
                                {
                                    retSuccess = CGOS_ERROR;
                                }
                            }
                        }
                    }
                }
            }       
        }
    cdv->retcnt += cdv->lout;
    }
    return retSuccess;
  }
  
/***********************************************************************
 * unsigned int CgosI2CGetMaxFrequency_BC(CGOS_DRV_VARS *cdv)             
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
 * Last Change: 05-Apr-2022 HMI                                          
 *                                                                      
 * Modifications: - MOD24: Added additional capabilities                                                       
 ***********************************************************************/  
unsigned int CgosI2CGetMaxFrequency_BC(CGOS_DRV_VARS *cdv)
  {
    unsigned char stat;
    unsigned char unit;
    unsigned char bus;
    unsigned char value;
    unsigned char writeBuf;
    unsigned char readBuf[14];
    unsigned int retSuccess = CGOS_SUCCESS;
    
    dbgi2c("CgosI2CGetMaxFrequency_BC called\n");    
    //MOD24 v
    if(cdv->cin->type >= cdv->brd->i2cCount)
    {
        retSuccess = CGOS_ERROR;
    }
    else
    {
    	bus = cdv->i2c[cdv->cin->type].cgbctype; //MOD24 ^
    	writeBuf = CGBC_CMD_INFO_1;

    	if(!bcCommand(&writeBuf,sizeof(writeBuf),&readBuf[0],sizeof(readBuf),&stat)) //Get Info from cBC
    	{
    		//MOD24 v
    		if(bus == CGBC_I2C_BUS_EXTERNAL)
    		{
       			unit = readBuf[3] & CGBC_I2C_FREQ_UNIT_MASK;
       			value = readBuf[3] & CGBC_I2C_FREQ_VALUE_MSK;
       		}
       		else if((bus == CGBC_I2C_BUS_SMB0) || (bus == CGBC_I2C_BUS_SMB1) || (bus == CGBC_I2C_BUS_SMB2))
       		{
       			unit = readBuf[4] & CGBC_I2C_FREQ_UNIT_MASK;
       			value = readBuf[4] & CGBC_I2C_FREQ_VALUE_MSK;
       		}
       		else if(bus == CGBC_I2C_BUS_EPI)
       		{
       		        unit = readBuf[5] & CGBC_I2C_FREQ_UNIT_MASK;
       			value = readBuf[5] & CGBC_I2C_FREQ_VALUE_MSK;
       		}
       		else
       		{
       			return CGOS_ERROR;
       		}
       		//MOD24 ^
       		if(unit == CGBC_I2C_FREQ_UNIT_100KHZ)
       		{
       		    cdv->cout->rets[0] = value * 100000;
       		}
       		else if(unit == CGBC_I2C_FREQ_UNIT_10KHZ)
       		{
       		    cdv->cout->rets[0] = value * 10000;
       		}
       		else if(unit == CGBC_I2C_FREQ_UNIT_1KHZ)
       		{
       		    cdv->cout->rets[0] = value * 1000;
       		}
       		else if(unit == CGBC_I2C_FREQ_UNIT_100HZ)
       		{
       		    cdv->cout->rets[0] = value * 100;
       		}
       		else
       		{
       		    cdv->cout->rets[0] = value;
       		}
    	}	
    	else
    	{
        	retSuccess = CGOS_ERROR;
    	}
    }
    return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosI2CGetFrequency_BC(CGOS_DRV_VARS *cdv)                
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
 * Last Change: 05-Apr-2022 HMI                                          
 *                                                                      
 * Modifications: -MOD24:  Fixed error in bcCommand function call                                                      
 ***********************************************************************/  
unsigned int CgosI2CGetFrequency_BC(CGOS_DRV_VARS *cdv)               
  {
    unsigned char stat;
    unsigned char erg;
    unsigned char unit; 
    unsigned char  bus;
    unsigned char value;
    unsigned char writeBuf[2];
    unsigned int retSuccess = CGOS_SUCCESS;
    
    dbgi2c("CgosI2CGetFrequency_BC called\n");
    
    if(cdv->cin->type >= cdv->brd->i2cCount)
    {
        retSuccess = CGOS_ERROR;
    }
    else
    {
    	bus = cdv->i2c[cdv->cin->type].cgbctype;
    	if(cdv->cin->type >= cdv->brd->i2cCount)
    	{
        	retSuccess = CGOS_ERROR;
    	}
    	else
    	{
        	writeBuf[0] = CGBC_CMD_I2C_SPEED | bus;
        	writeBuf[1] = 0x00;
        	if(!bcCommand(&writeBuf[0],2, &erg, 1,&stat))	//MOD24
        	{
        	    unit = erg & CGBC_I2C_FREQ_UNIT_MASK;
        	    value = erg & CGBC_I2C_FREQ_VALUE_MSK;
        	    if(unit == CGBC_I2C_FREQ_UNIT_100KHZ)
        	    {
        	        cdv->cout->rets[0] = value * 100000;
        	    }
        	    else if(unit == CGBC_I2C_FREQ_UNIT_10KHZ)
        	    {
        	        cdv->cout->rets[0] = value * 10000;
        	    }
        	    else if(unit == CGBC_I2C_FREQ_UNIT_1KHZ)
        	    {
        	        cdv->cout->rets[0] = value * 1000;
        	    }
        	    else if(unit == CGBC_I2C_FREQ_UNIT_100HZ)
        	    {
        	        cdv->cout->rets[0] = value * 100;
        	    }
        	    else
        	    {
        	        cdv->cout->rets[0] = value;
        	    }
        	}
        	else 
        	{
        	    retSuccess = CGOS_ERROR;
        	} 
    	}
    }
    return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosI2CSetFrequency_BC(CGOS_DRV_VARS *cdv)                
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
 * Last Change: 05-Apr-2022 HMI                                          
 *                                                                      
 * Modifications: -MOD24: +Fixed error in bcCommand function call
 *                        +Added 6100000 as maximum frequency value, the
 *                         board controller command can't send a higher
 *                         value.                                                 
 ***********************************************************************/  
unsigned int CgosI2CSetFrequency_BC(CGOS_DRV_VARS *cdv)                   
  {
    unsigned char stat;
    unsigned char bus;
    unsigned char ret = 0;
    unsigned int value = cdv->cin->pars[0];
    unsigned char dataByte = 0x00;
    unsigned char writeBuf[2];
    unsigned int retSuccess = CGOS_SUCCESS;
    
    dbgi2c("CgosI2CSetFrequency_BC called\n");
    
    if(cdv->cin->type >= cdv->brd->i2cCount)
    {
        retSuccess = CGOS_ERROR;
    }
    else
    {
    	bus = cdv->i2c[cdv->cin->type].cgbctype;
    	if(cdv->cin->type >= cdv->brd->i2cCount)
    	{
        	retSuccess = CGOS_ERROR;
    	}
    	else
    	{
        	writeBuf[0] = CGBC_CMD_I2C_SPEED | bus;        
        	if((value >= 100000)&&(value <= 6100000))	//MOD24
        	{
        		dataByte |= CGBC_I2C_FREQ_UNIT_100KHZ;
        		value /= 100000;
        	}
        	else if((value >= 10000)&&(value < 100000))
        	{
        		dataByte |= CGBC_I2C_FREQ_UNIT_10KHZ;
       			value /= 10000;
        	}
        	else if((value >= 1000)&&(value < 10000))
        	{
        		dataByte |= CGBC_I2C_FREQ_UNIT_1KHZ;
        		value /= 1000;
        	}
        	else if((value >= 100)&&(value < 1000))
        	{
        		dataByte |= CGBC_I2C_FREQ_UNIT_100HZ;
        		value /= 100;
        	}
        	else
        	{
        		retSuccess = CGOS_ERROR;
        	}
        	
        	if(retSuccess == CGOS_SUCCESS)
        	{
        		writeBuf[1] = dataByte | (CGBC_I2C_FREQ_VALUE_MSK & value);
        		if(!bcCommand(&writeBuf[0],2, &ret, 1,&stat))	//MOD24
        		{   
        	    		cdv->cout->rets[0] = ret;
        	    		retSuccess = CGOS_SUCCESS;
        		} 
        		else
        		{
        	    		retSuccess = CGOS_ERROR;
        		}
    		}
    	}
    }
    return retSuccess;
  }

/***********************************************************************
 * unsigned int exitI2CSubModule_BC(CGOS_DRV_VARS *cdv)					
 * 																	
 ***********************************************************************
 * Description: This function exits this SubModule.						
 * 																	
 * Last Change: 03.04.2018 HMI											
 * 																		
 * Modifications:													
 ***********************************************************************/
unsigned int exitI2CSubModule_BC(CGOS_DRV_VARS *cdv)
{
	dbgi2c("exitI2CModule called\n");
	return CGOS_SUCCESS;
}
