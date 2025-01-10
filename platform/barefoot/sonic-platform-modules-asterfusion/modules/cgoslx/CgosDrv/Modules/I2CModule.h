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
 
#ifndef _I2CMOD_H_
#define _I2CMOD_H_

//************************ Driver Internal Functions ************************	
extern unsigned int initI2CModule(CGOS_DRV_VARS *cdv);
extern unsigned int I2CModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv);
extern void exitI2CModule(CGOS_DRV_VARS *cdv);

extern unsigned int I2CBusGetStatus(unsigned char bus, unsigned char *pSts);

//************************ SubModule Handling ************************
extern unsigned int I2CGetCurrentSubModule(unsigned char *ModuleType);
extern unsigned int I2CChangeSubModule(CGOS_DRV_VARS *cdv, unsigned char NewSubModule);

//************************ Cgos Functions ************************
extern unsigned int zCgosI2CCount(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosI2CIsAvailable(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosI2CType(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosI2CRead(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosI2CWrite(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosI2CReadRegister(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosI2CWriteRegister(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosI2CWriteReadCombined(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosI2CGetMaxFrequency(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosI2CGetFrequency(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosI2CSetFrequency(CGOS_DRV_VARS *cdv);	

#endif
