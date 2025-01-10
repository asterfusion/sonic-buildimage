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
 
#ifndef _BOARDMOD_H_
#define _BOARDMOD_H_

//************************ Driver Internal Functions ************************	
extern unsigned int initBoardModule(CGOS_DRV_VARS *cdv);
extern unsigned int getBoardInfoFromBC(CGOS_DRV_VARS *cdv);
extern unsigned int BoardModuleHandler(CGOS_DRV_FCT *df, CGOS_DRV_VARS *cdv);
extern void exitBoardModule(CGOS_DRV_VARS *cdv);

//************************ Cgos Functions ************************				  
extern unsigned int zCgosBoardClose(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosBoardCount(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosBoardOpen(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosBoardOpenByNameA(CGOS_DRV_VARS *cdv);

extern unsigned int zCgosBoardGetNameA(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosBoardGetInfoA(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosBoardGetBootCounter(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosBoardGetRunningTimeMeter(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosBoardGetOption(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosBoardSetOption(CGOS_DRV_VARS *cdv);
extern unsigned int zCgosBoardGetBootErrorLog(CGOS_DRV_VARS *cdv);


#endif
