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
 
#ifndef _WDMOD_H_
#define _WDMOD_H_

//************************ Driver Internal Functions ************************
extern unsigned int initWDModule(CGOS_DRV_VARS *cdv);
extern unsigned int WDModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv);
extern void exitWDModule(CGOS_DRV_VARS *cdv);

//************************ SubModule Handling ************************
extern unsigned int WDGetCurrentSubModule(unsigned char *ModuleType);
extern unsigned int WDChangeSubModule(CGOS_DRV_VARS *cdv, unsigned char NewSubModule);

//************************ Cgos Functions ************************
extern unsigned int zCgosWDogGetInfo(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosWDogCount(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosWDogIsAvailable(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosWDogTrigger(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosWDogGetTriggerCount(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosWDogSetTriggerCount(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosWDogGetConfigStruct(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosWDogSetConfigStruct(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosWDogSetConfig(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosWDogDisable(CGOS_DRV_VARS *cdv);

#endif
