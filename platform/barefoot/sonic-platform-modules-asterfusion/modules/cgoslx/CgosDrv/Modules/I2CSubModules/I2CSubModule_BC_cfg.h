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
 
#ifndef _I2CSUBMOD_BC_CFG_H_
#define _I2CSUBMOD_BC_CFG_H_

#define I2C_COUNT 6

CGOS_DRV_I2C_ENTRY I2C_TABLE[I2C_COUNT] = {	{CGOS_I2C_TYPE_PRIMARY,	CGBC_I2C_BUS_EXTERNAL},
						{CGOS_I2C_TYPE_SMB, 	CGBC_I2C_BUS_SMB1},
						{0x00030000,  0x00030000},
						{0x00040000,  0x00040000},
						{0x00020000,  0x00020000},
						{0x00050000,  0x00050000}};
#endif
