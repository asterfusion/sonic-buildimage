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
 
#ifndef _VGAMOD_H_
#define _VGAMOD_H_

//************************ Driver Internal Functions ************************
extern unsigned int initVgaModule(CGOS_DRV_VARS *cdv);
extern unsigned int VgaModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv);
extern void exitVgaModule(CGOS_DRV_VARS *cdv);

//************************ SubModule Handling ************************
extern unsigned int VgaGetCurrentSubModule(unsigned char *ModuleType);
extern unsigned int VgaChangeSubModule(CGOS_DRV_VARS *cdv, unsigned char NewSubModule);

//************************ Cgos Functions ************************	
extern unsigned int zCgosVgaCount(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosVgaGetInfo(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosVgaGetContrast(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosVgaSetContrast(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosVgaGetContrastEnable(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosVgaSetContrastEnable(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosVgaGetBacklight(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosVgaSetBacklight(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosVgaGetBacklightEnable(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosVgaSetBacklightEnable(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosVgaEndDarkBoot(CGOS_DRV_VARS *cdv);

#endif
