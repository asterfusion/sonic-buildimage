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
 
#ifndef _BOARDMOD_CFG_H_
#define _BOARDMOD_CFG_H_

#define BOARD_COUNT 1

CGOSBOARDINFO BOARD_INFO[BOARD_COUNT] = { {	sizeof(CGOSBOARDINFO),	//dwSize	
										0,							//dwFlags
										{0},						//szReserved[CGOS_BOARD_MAX_SIZE_ID_STRING]
										{0},						//szBoard[CGOS_BOARD_MAX_SIZE_ID_STRING]
										{0},						//szBoardSub[CGOS_BOARD_MAX_SIZE_ID_STRING]
										{0},						//szManufacturer[CGOS_BOARD_MAX_SIZE_ID_STRING]
										{0},						//stManufacturingDate
										{0},						//stLastRepairDate
										{0},						//szSerialNumber[CGOS_BOARD_MAX_SIZE_ID_STRING]
										0,							//wProductRevision
										0,							//wSystemBiosRevision
										0,							//wBiosInterfaceRevision
										0,							//wBiosInterfaceBuildRevision
										0,							//dwClasses
										0,							//dwPrimaryClass
										0,							//dwRepairCounter
										{0},						//szPartNumber[CGOS_BOARD_MAX_SIZE_PART_STRING
										{0},						//szEAN[CGOS_BOARD_MAX_SIZE_EAN_STRING]
										0,							//dwManufacturer
									} };

#endif
