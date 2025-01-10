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
#include "HWMSubModule_BC_cfg.h"


//***************************************************************************

//#define dbg_hwm_sub_bc_on

#ifdef dbg_hwm_sub_bc_on
#ifdef __linux__
#define dbghwm(x) printk(x)
#elif _WIN64
#define dbghwm(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#elif _WIN32
#define dbghwm(x) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, x)
#endif
#else
#define dbghwm(x)
#endif


//***************************************************************************

static unsigned char tempcnt;
static unsigned char fancnt;
static unsigned char voltcnt;
static unsigned char sensorcnt;

static CGOS_DRV_TEMP_ENTRY TempList[CGOS_DRV_TEMP_MAX] = {0};
static CGOS_DRV_FAN_ENTRY FanList[CGOS_DRV_FAN_MAX] = {0};
static CGOS_DRV_VOLT_ENTRY VoltList[CGOS_DRV_VOLT_MAX] = {0};

//***************************************************************************
//Helper functions

unsigned int GetErrorFlag(unsigned char byte)
{
	switch (byte)
	{
		case 0x00:	//Sensor not present
					return CGOS_SENSOR_BROKEN;
					
		case 0x01:	//Sensor not supported
					return CGOS_SENSOR_BROKEN;
					
		case 0x02:	//Sensor inactive
					return CGOS_SENSOR_BROKEN;
					
		case 0x03:	//Sensor broken or open
					return CGOS_SENSOR_BROKEN;
					
		case 0x04:	//Sensor shortcut
					return CGOS_SENSOR_SHORTCIRCUIT;
					
		default: 	//reserved values
					return CGOS_SENSOR_BROKEN;
	}
	return CGOS_ERROR;
}

unsigned int GetTempSensorType(unsigned char byte)
{
	switch(byte)
	{
		case 0b00001:	//CPU temperature
						return CGOS_TEMP_CPU;
						
		case 0b00010:	//case temperature
						return CGOS_TEMP_BOX;
						
		case 0b00011:	//ambient temperature
						return CGOS_TEMP_ENV;
						
		case 0b00100:	//CPU board temperature
						return CGOS_TEMP_BOARD;
						
		case 0b00101:	//carrier board temperature
						return CGOS_TEMP_BACKPLANE;
						
		case 0b00110:	//chipset temperature
						return CGOS_TEMP_CHIPSETS;
						
		case 0b00111:	//viedeo controller/board temperature
						return CGOS_TEMP_VIDEO;
						
		case 0b01000:	//other temperature sensor type
						return CGOS_TEMP_OTHER;
						
		case 0b01001:	//top DIMM temperature
						return CGOS_TEMP_TOPDIMM_ENV;
						
		case 0b01010:	//bottom DIMM temperature 
						return CGOS_TEMP_BOTDIMM_ENV;
						
		default:		//reserved values
						return CGOS_TEMP_OTHER;
	}
	return CGOS_ERROR;
}

unsigned int GetVoltSensorType(unsigned char byte)
{
	switch(byte)
	{
		case 0b00001:	//CPU core voltage
						return CGOS_VOLTAGE_VCOREA;
		
		case 0b00010:	//DC runtime voltage
						return CGOS_VOLTAGE_DC;
						
		case 0b00011:	//DC standby voltage
						return CGOS_VOLTAGE_DC_STANDBY;
						
		case 0b00100:	//CMOS battery voltage
						return CGOS_VOLTAGE_BAT_CMOS;
						
		case 0b00101:	//battery supply voltage
						return CGOS_VOLTAGE_BAT_POWER;
						
		case 0b00110:	//AC voltage
						return CGOS_VOLTAGE_OTHER;
						
		case 0b00111:	//other voltage sensor type
						return CGOS_VOLTAGE_OTHER;
						
		case 0b01000:	//5V runtime voltage
						return CGOS_VOLTAGE_5V_S0;
						
		case 0b01001:	//5V standby voltage
						return CGOS_VOLTAGE_5V_S5;
						
		case 0b01010:	//3.3V runtime voltage
						return CGOS_VOLTAGE_33V_S0;
						
		case 0b01011:	//3.3V standby voltage
						return CGOS_VOLTAGE_33V_S5;
						
		case 0b01100:	//chipset core voltage A
						return CGOS_VOLTAGE_VCOREA;
						
		case 0b01101:	//chipset core voltage B
						return CGOS_VOLTAGE_VCOREB;
						
		case 0b01110:	//12V runtime voltage
						return CGOS_VOLTAGE_12V_S0;
						
		case 0b10010:	//DC runtime current
						return CGOS_VCURRENT_DC;
						
		case 0b11000:	//5V runtime current
						return CGOS_VCURRENT_5V_S0;
						
		case 0b11110:	//12V runtime current
						return CGOS_VCURRENT_12V_S0;
		
		default:		//reserved values
						return CGOS_VOLTAGE_OTHER;
	}
	return CGOS_ERROR;
}

unsigned int GetFanSensorType(unsigned char byte)
{
	switch(byte)
	{
		case 0b00001:	//CPU fan
						return CGOS_FAN_CPU;
						
		case 0b00010:	//case fan
						return CGOS_FAN_BOX;
						
		case 0b00011:	//ambient fan
						return CGOS_FAN_OTHER;
						
		case 0b00100:	//chipset fan
						return CGOS_FAN_CHIPSET;
						
		case 0b00101:	//video controller/board fan
						return CGOS_FAN_VIDEO;
						
		case 0b00110:	//other fan speed sensor type
						return CGOS_FAN_OTHER;
		
		default:		//reserved values
						return CGOS_FAN_OTHER;
	}
	return CGOS_ERROR;
}

//***************************************************************************
// Function prototypes for register function

unsigned int initHWMSubModule_BC(CGOS_DRV_VARS *cdv);
//Temperature
unsigned int CgosTemperatureCount_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosTemperatureGetInfo_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosTemperatureGetCurrent_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosTemperatureSetLimits_BC(CGOS_DRV_VARS *cdv);
//Fan
unsigned int CgosFanCount_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosFanGetInfo_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosFanGetCurrent_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosFanSetLimits_BC(CGOS_DRV_VARS *cdv);
//Voltage
unsigned int CgosVoltageCount_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVoltageGetInfo_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVoltageGetCurrent_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosVoltageSetLimits_BC(CGOS_DRV_VARS *cdv);
//Performance
unsigned int CgosPerformanceGetCurrent_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosPerformanceSetCurrent_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosPerformanceGetPolicyCaps_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosPerformanceGetPolicy_BC(CGOS_DRV_VARS *cdv);
unsigned int CgosPerformanceSetPolicy_BC(CGOS_DRV_VARS *cdv);
unsigned int exitHWMSubModule_BC(CGOS_DRV_VARS *cdv);

//***************************************************************************

//Prototypes for initHWMModule function
unsigned int initPerformance_BC(CGOS_DRV_VARS *cdv);
unsigned int initTemperature_BC(CGOS_DRV_VARS *cdv);
unsigned int initFan_BC(CGOS_DRV_VARS *cdv);
unsigned int initVoltage_BC(CGOS_DRV_VARS *cdv);

//***************************************************************************

/***********************************************************************
 * unsigned int RegisterHWMSubModule_BC									
 * 																		
 ***********************************************************************
 * Description: This function uses the RegisterSubModule callback to	
 * 			  	register the SubModule functions.						
 * 																		
 * Last Change: 20.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int RegisterHWMSubModule_BC(unsigned int (*RegisterSubModule)(	CgosFunctionPointer initSubModule,
																		CgosFunctionPointer TempCount,
																		CgosFunctionPointer TempGetInfo,
																		CgosFunctionPointer TempGetCurrent,
																		CgosFunctionPointer TempSetLimits,
																		CgosFunctionPointer FanCount,
																		CgosFunctionPointer FanGetInfo,
																		CgosFunctionPointer FanGetCurrent,
																		CgosFunctionPointer FanSetLimits,
																		CgosFunctionPointer VoltCount,
																		CgosFunctionPointer VoltGetInfo,
																		CgosFunctionPointer VoltGetCurrent,
																		CgosFunctionPointer VoltSetLimits,
																		CgosFunctionPointer PerfGetCurrent,
																		CgosFunctionPointer PerfSetCurrent,
																		CgosFunctionPointer PerfGetPolCaps,
																		CgosFunctionPointer PerfGetPol,
																		CgosFunctionPointer PerfSetPol,
																		CgosFunctionPointer exitSubModule,
																		unsigned char ModuleNumber),
								    unsigned char Number)
{
	return RegisterSubModule(	initHWMSubModule_BC,
								CgosTemperatureCount_BC,
								CgosTemperatureGetInfo_BC,
								CgosTemperatureGetCurrent_BC,
								NULL, //CgosTemperatureSetLimits_BC
								CgosFanCount_BC,
								CgosFanGetInfo_BC,
								CgosFanGetCurrent_BC,
								CgosFanSetLimits_BC,
								CgosVoltageCount_BC,
								CgosVoltageGetInfo_BC,
								CgosVoltageGetCurrent_BC,
								NULL, //CgosVoltageSetLimits_BC,
								NULL, //CgosPerformanceGetCurrent_BC,
								NULL, //CgosPerformanceSetCurrent_BC,
								NULL, //CgosPerformanceGetPolicyCaps_BC,
								NULL, //CgosPerformanceGetPolicy_BC,
								NULL, //CgosPerformanceSetPolicy_BC
								exitHWMSubModule_BC,
								Number);
}


/***********************************************************************
 * unsigned int initHWMSubModule_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description: This function calls all init functions of this module.	
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initHWMSubModule_BC(CGOS_DRV_VARS *cdv)
  {	
	unsigned char wbuf[2];
	unsigned char rbuf[5];
	unsigned char SensorStatus = 0;
	unsigned char sts;
	unsigned int type;
	int i;
	unsigned int retSuccess = CGOS_SUCCESS;
	dbghwm("initHWMSubModule_BC called\n");
	
	//init 
	tempcnt = 0;
	voltcnt = 0;
	fancnt = 0;
	sensorcnt = 0;
	
	//Get BC sensor count
	wbuf[0] = CGBC_CMD_HWM_SENSOR;
	wbuf[1] = 0;
	
	bcCommand(&wbuf[0],2,&rbuf[0],5,&sts);
	sensorcnt = rbuf[0];
	
	//Get sensor information
	for(i = 0; i < sensorcnt; i++)
	{
		//Get information of sensor i
		wbuf[1] = i;
		bcCommand(&wbuf[0],2,&rbuf[0],5,&sts);
		
		//evaluate status byte
		SensorStatus = rbuf[1];
		switch((SensorStatus & 0x60) >> 5)
		{
			case 0b01:	//Temperature Sensor
						TempList[tempcnt].BCUnitNumber = i;
						TempList[tempcnt].tempinfo.dwSize = sizeof(CGOSTEMPERATUREINFO);
						type = GetTempSensorType(SensorStatus & 0x1F);
						if(type != CGOS_ERROR)
						{
							TempList[tempcnt].tempinfo.dwType = type;
						}
						if(((SensorStatus & 0x80) >> 7) == 0)	//Sensor Status inactive
						{
							TempList[tempcnt].tempinfo.dwFlags = GetErrorFlag(rbuf[2]);
						}
						else 	//Sensor Status active
						{
							TempList[tempcnt].tempinfo.dwFlags = CGOS_SENSOR_ACTIVE;
						}
						tempcnt++;
						
						break;
			case 0b10:	//Voltage Sensor
						VoltList[voltcnt].BCUnitNumber = i;
						VoltList[voltcnt].voltinfo.dwSize = sizeof(CGOSVOLTAGEINFO);
						type = GetVoltSensorType(SensorStatus & 0x1F);
						if(type != CGOS_ERROR)
						{
							VoltList[voltcnt].voltinfo.dwType = type;
						}
						if(((SensorStatus & 0x80) >> 7) == 0)	//Sensor Status inactive
						{
							VoltList[voltcnt].voltinfo.dwFlags = GetErrorFlag(rbuf[2]);
						}
						else 	//Sensor Status active
						{
							VoltList[voltcnt].voltinfo.dwFlags = CGOS_SENSOR_ACTIVE;
						}
						voltcnt++;
						break;
			case 0b11:	//Fan Sensor
						FanList[fancnt].BCUnitNumber = i;
						FanList[fancnt].faninfo.dwSize = sizeof(CGOSFANINFO);
						type = GetFanSensorType(SensorStatus & 0x1F);
						if(type != CGOS_ERROR)
						{
							FanList[fancnt].faninfo.dwType = type;
						}
						if(((SensorStatus & 0x80) >> 7) == 0)	//Sensor Status inactive
						{
							FanList[fancnt].faninfo.dwFlags = GetErrorFlag(rbuf[2]);
						}
						else 	//Sensor Status active
						{
							FanList[fancnt].faninfo.dwFlags = CGOS_SENSOR_ACTIVE;
						}
						fancnt++;
						break;
			default:
						break;
		}
	}
	
	//Call init functions
	retSuccess |= initTemperature_BC(cdv);
	retSuccess |= initFan_BC(cdv);
	retSuccess |= initVoltage_BC(cdv);
	//retSuccess |= initPerformance_BC(cdv);		//Performance functions not implemented
	return (retSuccess == CGOS_SUCCESS) ? CGOS_SUCCESS : CGOS_ERROR;
  }

/* **********************************************************************
 *                                                                      *
 *                       Temperature Functions                          *
 *                                                                      *
 ************************************************************************/

/***********************************************************************
 * unsigned int initTemperature_BC(CGOS_DRV_VARS *cdv) 				    
 * 																		
 ***********************************************************************
 * Description: This function is called to copy temperature information 
 * 				from HWMModule_cfg.h into the cdv struct.				
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initTemperature_BC(CGOS_DRV_VARS *cdv)
  {	
	unsigned int retSuccess = CGOS_SUCCESS;
	int i;
	dbghwm("initTemperature_BC called\n");
	if(tempcnt <= CGOS_DRV_TEMP_MAX)			//CGOS_DRV_TEMP_MAX defined in DrvVars.h
	{
		cdv->brd->hwmCount.tempCount = tempcnt;
		for(i = 0; i < tempcnt; i++)
		{
			OsaMemCpy(&cdv->hwm->tempsensors[i],&TempList[i],sizeof(CGOS_DRV_TEMP_ENTRY));  
		}
	}
	else
	{
		retSuccess = CGOS_ERROR;
	}
	return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosTemperatureCount_BC(CGOS_DRV_VARS *cdv)			    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosTemperatureCount(HCGOS hCgos) 				    
 * 																	    
 * Inputs:															    
 *   -												    			    
 * 																	    
 * Outputs:															    
 *   cdv->cout->rets[0]												    
 * 																		
 ***********************************************************************
 * Description: Gets temperature sensor	count.							
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosTemperatureCount_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosTemperatureCount_BC called\n");
	cdv->cout->rets[0] = cdv->brd->hwmCount.tempCount;
	
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosTemperatureGetInfo_BC(CGOS_DRV_VARS *cdv)			    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosTemperatureGetInfo(HCGOS hCgos, 				    
 * 								 		 unsigned long dwUnit,			
 * 								  		 CGOSTEMPERATUREINFO *pInfo)	
 * Inputs:															    
 *   dwUnit <->		cdv->cin->type					    			    
 * 																	    
 * Outputs:															    
 *   pInfo 	<-> 	cdv->pout										    
 * 																	
 ***********************************************************************
 * Description: Gets info struct of the requested temperature sensor
 * 																		
 * Last Change: 06.12.2017 HMI										
 * 																	
 * Modifications:														
 ***********************************************************************/
unsigned int CgosTemperatureGetInfo_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosTemperatureGetInfo_BC called\n");
	if(cdv->cin->type < cdv->brd->hwmCount.tempCount)
	{
		OsaMemCpy(cdv->pout,&cdv->hwm->tempsensors[cdv->cin->type].tempinfo,sizeof(CGOSTEMPERATUREINFO));	
		return CGOS_SUCCESS;
	}
	else
	{
		return CGOS_ERROR;
	}
  }
  
/***********************************************************************
 * unsigned int CgosTemperatureGetCurrent_BC(CGOS_DRV_VARS *cdv)		    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosTemperatureGetCurrent(HCGOS hCgos, 			    
 * 								 	 		unsigned long dwUnit,		
 * 								  	 		unsigned long *pdwSetting,	
 * 											unsigned long *pdwStatus)	
 * Inputs:															    
 *   dwUnit 	<->		cdv->cin->type				    			    
 * 																	    
 * Outputs:															    
 *   pdwSetting 	<-> 	cdv->cout->rets[0]						    
 * 	 pdwStatus		<->		cdv->cout->rets[1]						
 * 																		
 ***********************************************************************
 * Description: Gets status and value/error code of the	requested		
 * 				temperature sensor.										
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosTemperatureGetCurrent_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned char wbuf[2];
	unsigned char rbuf[5];
	unsigned char SensorStatus = 0;
	unsigned char sts;

	dbghwm("CgosTemperatureGetCurrent_BC called\n");
	
	if(cdv->cin->type >= cdv->brd->hwmCount.tempCount)
	{
		return CGOS_ERROR;
	}
	else
	{
		wbuf[0] = CGBC_CMD_HWM_SENSOR;
		wbuf[1] = cdv->hwm->tempsensors[cdv->cin->type].BCUnitNumber;
	
		bcCommand(&wbuf[0],2,&rbuf[0],5,&sts);
		
		SensorStatus = rbuf[1];
		if(((SensorStatus & 0x80) >> 7) == 1)	//Sensor Status active
		{
			//rbuf[2] holds the low byte of the sensor value,
			//rbuf[3] the high byte.
		
			cdv->cout->rets[0] = ((rbuf[3]<<8)|rbuf[2]) * 100;	// *100 because board controller returns 
																// 0.1 degree centigrade and cgos expects 
																// 0.001 degree centigrade.
			cdv->cout->rets[1] = CGOS_SENSOR_ACTIVE;											
		}
		else //Sensor Status inactive
		{
			cdv->cout->rets[0] = 0;
			cdv->cout->rets[1] = GetErrorFlag(rbuf[2]);
		}
	}
	return CGOS_SUCCESS;
  }
  
unsigned int CgosTemperatureSetLimits_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosTemperatureSetLimits_BC called\n");
	return CGOS_SUCCESS;
  }

/* **********************************************************************
 *                                                                      *
 *                            Fan Functions                             *
 *                                                                      *
 ************************************************************************/


/***********************************************************************
 * unsigned int initFan_BC(CGOS_DRV_VARS *cdv) 						    
 * 																		
 ***********************************************************************
 * Description: This function is called to copy fan information from	
 * 				HWMModule_cfg.h	into the cdv struct.					
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initFan_BC(CGOS_DRV_VARS *cdv)
  {	
	unsigned int retSuccess = CGOS_SUCCESS;
	int i;
	dbghwm("initFan_BC called\n");
	if(fancnt <= CGOS_DRV_FAN_MAX)			//CGOS_DRV_TEMP_MAX defined in DrvVars.h
	{
		cdv->brd->hwmCount.fanCount = fancnt;
		for(i = 0; i < fancnt; i++)
		{
			OsaMemCpy(&cdv->hwm->fansensors[i],&FanList[i],sizeof(CGOS_DRV_FAN_ENTRY));  
		}
	}
	else
	{
		retSuccess = CGOS_ERROR;
	}
	return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosFanCount_BC(CGOS_DRV_VARS *cdv)					    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosFanCount(HCGOS hCgos) 						    
 * 																	    
 * Inputs:															    
 *   -												    			    
 * 																	    
 * Outputs:															    
 *   cdv->cout->rets[0]												    
 * 																		
 ***********************************************************************
 * Description: Gets fan sensor	count.									
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosFanCount_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosFanCount_BC called\n");
	cdv->cout->rets[0] = cdv->brd->hwmCount.fanCount;
	
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosFanGetInfo_BC(CGOS_DRV_VARS *cdv)					    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosFanGetInfo(HCGOS hCgos, 						    
 * 								 unsigned long dwUnit,				    
 * 								 CGOSFANINFO *pInfo)				    
 * Inputs:															    
 *   dwUnit <->		cdv->cin->type					    			    
 * 																	    
 * Outputs:															    
 *   pInfo 	<-> 	cdv->pout										    
 * 																		
 ***********************************************************************
 * Description: Gets info struct of the requested fan sensor			
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosFanGetInfo_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosFanGetInfo_BC called\n");
	if(cdv->cin->type < cdv->brd->hwmCount.fanCount)
	{
		OsaMemCpy(cdv->pout,&cdv->hwm->fansensors[cdv->cin->type].faninfo,sizeof(CGOSFANINFO));	
		return CGOS_SUCCESS;
	}
	else
	{
		return CGOS_ERROR;
	}
  }

/***********************************************************************
 * unsigned int CgosFanGetCurrent_BC(CGOS_DRV_VARS *cdv)				    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosFanGetCurrent(HCGOS hCgos, 			 			
 * 								 	unsigned long dwUnit,				
 * 								  	unsigned long *pdwSetting,			
 * 									unsigned long *pdwStatus)			
 * Inputs:															    
 *   dwUnit 	<->		cdv->cin->type				    			    
 * 																	    
 * Outputs:															    
 *   pdwSetting 	<-> 	cdv->cout->rets[0]						    
 * 	 pdwStatus		<->		cdv->cout->rets[1]							
 * 																		
 ***********************************************************************
 * Description: Gets status and value/error code of the					
 * 				requested fan sensor									
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosFanGetCurrent_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned char wbuf[2];
	unsigned char rbuf[5];
	unsigned char SensorStatus = 0;
	unsigned char sts;

	dbghwm("CgosFanGetCurrent_BC called\n");
	
	
	if(cdv->cin->type >= cdv->brd->hwmCount.fanCount)
	{
		return CGOS_ERROR;
	}
	else
	{
		wbuf[0] = CGBC_CMD_HWM_SENSOR;
		wbuf[1] = cdv->hwm->fansensors[cdv->cin->type].BCUnitNumber;
	
		bcCommand(&wbuf[0],2,&rbuf[0],5,&sts);
		SensorStatus = rbuf[1];
		
		if(((SensorStatus & 0x80) >> 7) == 1)	//Sensor Status active
		{
			//rbuf[2] holds the low byte of the sensor value,
			//rbuf[3] the high byte.
		
			cdv->cout->rets[0] = ((rbuf[3]<<8)|rbuf[2]);	
			cdv->cout->rets[1] = CGOS_SENSOR_ACTIVE;											
		}
		else //Sensor Status inactive
		{
			cdv->cout->rets[0] = 0;
			cdv->cout->rets[1] = GetErrorFlag(rbuf[2]);
		}
	}
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosFanSetLimits_BC(CGOS_DRV_VARS *cdv)				    
 * 																		
 ***********************************************************************
 * Description: Is used to set the fan speed.									
 * 																		
 * Last Change: 22.04.2020 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosFanSetLimits_BC(CGOS_DRV_VARS *cdv)
  {
	CGOSFANINFO fanInfo = {0};
	unsigned char wbuf[2];
	unsigned char rbuf[2];
	unsigned char sts;

	dbghwm("CgosFanSetLimits_BC called\n");

	OsaMemCpy(&fanInfo,(CGOSFANINFO*)cdv->pin,sizeof(CGOSFANINFO));
	if(fanInfo.dwType == CGOS_FAN_CPU)
	{	
		if(fanInfo.dwOutMax <= 100) //check if percentage
		{
			wbuf[0] = CGBC_CMD_CPU_FAN_CONTROL;
			wbuf[1] = CGBC_FAN_SET_PWM + fanInfo.dwOutMax;
			bcCommand(&wbuf[0],2,&rbuf[0],2,&sts);

			cdv->hwm->fansensors[cdv->cin->type].faninfo.dwOutMax = fanInfo.dwOutMax;
		}
		else
		{
			return CGOS_INVALID_PARAMETER;
		}
	}
	else
	{
		return CGOS_ERROR;
	}
	return CGOS_SUCCESS;
  }
  
/* **********************************************************************
 *                                                                      *
 *                         Voltage Functions                            *							    
 *                                                                      *
 ************************************************************************/  


/***********************************************************************
 * unsigned int initVoltage_BC(CGOS_DRV_VARS *cdv) 					    
 * 																		
 ***********************************************************************
 * Description: This function is called to copy voltage information 	
 * 				from HWMModule_cfg.h into the cdv struct.				
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initVoltage_BC(CGOS_DRV_VARS *cdv)
  {
	unsigned int retSuccess = CGOS_SUCCESS;
	int i;
	dbghwm("initVoltage_BC called\n");
	if(voltcnt <= CGOS_DRV_VOLT_MAX)			//CGOS_DRV_TEMP_MAX defined in DrvVars.h
	{
		cdv->brd->hwmCount.voltCount = voltcnt;
		for(i = 0; i < voltcnt; i++)
		{
			OsaMemCpy(&cdv->hwm->voltsensors[i],&VoltList[i],sizeof(CGOS_DRV_VOLT_ENTRY));  
		}
	} 
	else
	{
		retSuccess = CGOS_ERROR;
	}
	return retSuccess;
  }

/***********************************************************************
 * unsigned int CgosVoltageCount_BC(CGOS_DRV_VARS *cdv)				    
 * 																	
 ***********************************************************************
 * Cgos Function: CgosVoltageCount(HCGOS hCgos) 					   
 * 																	    
 * Inputs:															    
 *   -												    			    
 * 																	    
 * Outputs:															    
 *   cdv->cout->rets[0]												    
 * 																	
 ***********************************************************************
 * Description: Gets voltage sensor	count.								
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/  
unsigned int CgosVoltageCount_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosVoltageCount_BC called\n");	
	cdv->cout->rets[0] = cdv->brd->hwmCount.voltCount;
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosVoltageGetInfo_BC(CGOS_DRV_VARS *cdv)				    
 * 																		
 ***********************************************************************
 * Cgos Function: CgosVoltageGetInfo(HCGOS hCgos, 					    
 * 								 	 unsigned long dwUnit,			    
 * 								  	 CGOSVOLTAGEINFO *pInfo)		    
 * Inputs:															    
 *   dwUnit <->		cdv->cin->type					    			    
 * 																	    
 * Outputs:															    
 *   pInfo 	<-> 	cdv->pout										    
 * 																		
 ***********************************************************************
 * Description: Gets info struct of the requested voltage sensor		
 * 																		
 * Last Change: 06.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosVoltageGetInfo_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosVoltageGetInfo_BC called\n");
	if(cdv->cin->type < cdv->brd->hwmCount.voltCount)
	{	
		OsaMemCpy(cdv->pout,&cdv->hwm->voltsensors[cdv->cin->type].voltinfo,sizeof(CGOSVOLTAGEINFO));	
		return CGOS_SUCCESS;
	}
	else
	{
		return CGOS_ERROR;
	}
  }

/***********************************************************************
 * unsigned int CgosVoltageGetCurrent_BC(CGOS_DRV_VARS *cdv)			    
 * 																	
 ***********************************************************************
 * Cgos Function: CgosVoltageGetCurrent(HCGOS hCgos, 			   		
 * 								 	 	unsigned long dwUnit,			
 * 								  	 	unsigned long *pdwSetting,		
 * 										unsigned long *pdwStatus)		
 * Inputs:															    
 *   dwUnit		 <->		cdv->cin->type			    			    
 * 																	    
 * Outputs:															    
 *   pdwSetting 	<-> 	cdv->cout->rets[0]						   
 * 	 pdwStatus		<->		cdv->cout->rets[1]							
 * 																		
 ***********************************************************************
 * Description: Gets status and value/error code of the					
 * 				requested voltage sensor								
 * 																		
 * Last Change: 25.06.2018 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosVoltageGetCurrent_BC(CGOS_DRV_VARS *cdv)					
  {
	unsigned char wbuf[2];
	unsigned char rbuf[5];
	unsigned char SensorStatus = 0;
	unsigned char sts;

	dbghwm("CgosVoltageGetCurrent_BC called\n");
	
	
	if(cdv->cin->type >= cdv->brd->hwmCount.tempCount)
	{
		return CGOS_ERROR;
	}
	else
	{
		wbuf[0] = CGBC_CMD_HWM_SENSOR;
		wbuf[1] = cdv->hwm->voltsensors[cdv->cin->type].BCUnitNumber;
	
		bcCommand(&wbuf[0],2,&rbuf[0],5,&sts);
		SensorStatus = rbuf[1];
		
		if(((SensorStatus & 0x80) >> 7) == 1)	//Sensor Status active
		{
			//rbuf[2] holds the low byte of the sensor value,
			//rbuf[3] the high byte.
		
			cdv->cout->rets[0] = ((rbuf[3]<<8)|rbuf[2]);	
			cdv->cout->rets[1] = CGOS_SENSOR_ACTIVE;											
		}
		else //Sensor Status inactive
		{
			cdv->cout->rets[0] = 0;
			cdv->cout->rets[1] = GetErrorFlag(rbuf[2]);
		}
	}
	return CGOS_SUCCESS;
  }

unsigned int CgosVoltageSetLimits_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosVoltageSetLimits_BC called\n");
	return CGOS_SUCCESS;
  }
  
/* **********************************************************************
 *                                                                      *
 *                       Performance Functions                          *
 *                                                                      *
 ************************************************************************/

/***********************************************************************
 * unsigned int initPerformance_BC(CGOS_DRV_VARS *cdv) 				    
 * 																		
 ***********************************************************************
 * Description: Not Implemented											
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int initPerformance_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("initPerformance_BC called\n"); 
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosPerformanceGetCurrent_BC(CGOS_DRV_VARS *cdv) 		    
 * 																		
 ***********************************************************************
 * Description: Not Implemented											
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosPerformanceGetCurrent_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosPerformanceGetCurrent_BC called\n");
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosPerformanceSetCurrent_BC(CGOS_DRV_VARS *cdv) 		    
 * 																		
 ***********************************************************************
 * Description: Not Implemented											
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:													
 ***********************************************************************/
unsigned int CgosPerformanceSetCurrent_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosPerformanceSetCurrent_BC called\n");
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosPerformanceGetPolicyCaps_BC(CGOS_DRV_VARS *cdv) 	    
 * 																		
 ***********************************************************************
 * Description: Not Implemented											
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosPerformanceGetPolicyCaps_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosPerformanceGetPolicyCaps_BC called\n");
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosPerformanceGetPolicy_BC(CGOS_DRV_VARS *cdv) 		    
 * 																		
 ***********************************************************************
 * Description: Not Implemented											
 * 																		
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosPerformanceGetPolicy_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosPerformanceGetPolicy_BC called\n");
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * unsigned int CgosPerformanceSetPolicy_BC(CGOS_DRV_VARS *cdv) 		    
 * 																		
 ***********************************************************************
 * Description: Not Implemented											
 * 																	
 * Last Change: 05.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int CgosPerformanceSetPolicy_BC(CGOS_DRV_VARS *cdv)
  {
	dbghwm("CgosPerformanceSetPolicy_BC called\n");
	return CGOS_SUCCESS;
  }

/***********************************************************************
 * void exitHWMModule(CGOS_DRV_VARS *cdv) 							    
 * 																		
 ***********************************************************************
 * Description: This function is called during driver close and should	
 * 				free allocated resources.								
 * 																		
 * Last Change: 12.12.2017 HMI											
 * 																		
 * Modifications:														
 ***********************************************************************/
unsigned int exitHWMSubModule_BC(CGOS_DRV_VARS *cdv)
{
	dbghwm("exitHWMSubModule_BC called\n");
	return CGOS_SUCCESS;
}
