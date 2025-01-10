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
 
#ifndef _BCMOD_H_
#define _BCMOD_H_

typedef struct {
  unsigned int xfct;
  unsigned int (*fct)(CGOS_DRV_VARS *cdv);
  unsigned int minin;
  unsigned int minout;
  unsigned int flags;
  } CGOS_DRV_FCT;

//************************ Driver Internal Functions ************************	
extern unsigned int initBCModule(CGOS_DRV_VARS *cdv);
extern unsigned int BCModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv);

extern unsigned int bcCommand(unsigned char *cmdDataBPtr,
					   unsigned char  cmdByteCount,
					   unsigned char *rcvDataBPtr,
					   unsigned char  rcvByteCount,
					   unsigned char *retValueBPtr );

//************************ SubModule Handling ************************
extern unsigned int BCGetCurrentSubModule(unsigned char *ModuleType);
extern unsigned int BCChangeSubModule(CGOS_DRV_VARS *cdv, unsigned char NewSubModule);

//************************ Cgos Functions ************************			  
extern unsigned int zCgosCgbcGetInfo(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosCgbcSetControl(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosCgbcReadWrite(CGOS_DRV_VARS *cdv);	
extern unsigned int zCgosCgbcHandleCommand(CGOS_DRV_VARS *cdv);	
															  
#endif
