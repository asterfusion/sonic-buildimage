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
 
#ifndef _HWMMOD_H_
#define _HWMMOD_H_

//************************ Driver Internal Functions ************************	
extern unsigned int initHWMModule(CGOS_DRV_VARS *cdv);
extern unsigned int HWMModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv);
extern void exitHWMModule(CGOS_DRV_VARS *cdv);

//************************ SubModule Handling ************************
extern unsigned int HWMGetCurrentSubModule(unsigned char *ModuleType);
extern unsigned int HWMChangeSubModule(CGOS_DRV_VARS *cdv, unsigned char NewSubModule);

//************************ Cgos Functions ************************
extern unsigned int zCgosPerformanceGetCurrent(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosPerformanceSetCurrent(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosPerformanceGetPolicyCaps(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosPerformanceGetPolicy(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosPerformanceSetPolicy(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosTemperatureCount(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosTemperatureGetInfo(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosTemperatureGetCurrent(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosTemperatureSetLimits(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosFanCount(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosFanGetInfo(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosFanGetCurrent(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosFanSetLimits(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosVoltageCount(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosVoltageGetInfo(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosVoltageGetCurrent(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosVoltageSetLimits(CGOS_DRV_VARS *cdv);

#endif
