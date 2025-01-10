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
 
#ifndef _BCMOD_CFG_H_
#define _BCMOD_CFG_H_

typedef struct
{
	char Letters[4];
} BoardName;
			
#define BC4_BASE_E_COUNT 28

BoardName BC4_BASE_E_LIST[BC4_BASE_E_COUNT] = {
							{{'T','F','T','3'}},
							{{'Q','F','T','3'}},
							{{'T','C','S','L'}},
								{{'B','U','S','L'}},
								{{'B','V','S','L'}},
							{{'T','S','S','L'}},
								{{'B','H','S','L'}},
								{{'B','Q','S','L'}},
							{{'I','C','S','L'}},
								{{'I','U','S','L'}},
								{{'I','V','S','L'}},
							{{'I','C','K','L'}},
								{{'I','U','K','L'}},
								{{'I','V','K','L'}},
							{{'T','S','K','L'}},			
							    {{'B','H','K','L'}},
						    	{{'B','Q','K','L'}},
						    {{'T','C','K','L'}},
						    	{{'B','U','K','L'}},
						    	{{'B','V','K','L'}},
							{{'T','R','3','3'}},
							{{'Q','G','4','0'}},
							{{'T','S','D','E'}},
							{{'T','R','4','4'}},
							{{'D','S','A','C'}},
								{{'D','S','A','0'}},
								{{'D','S','A','1'}},
								{{'D','S','A','2'}}
						};

#define BC4_BASE_C_COUNT 39

BoardName BC4_BASE_C_LIST[BC4_BASE_C_COUNT] = {
							{{'T','U','8','7'}},
								{{'T','V','8','7'}},
							{{'Q','A','3','0'}},
								{{'Q','A','3','1'}},
								{{'Q','C','3','1'}},
								{{'Q','A','3','2'}},
								{{'Q','C','3','2'}},
							{{'T','A','3','0'}},
								{{'T','A','3','1'}},
								{{'T','C','3','1'}},
								{{'T','A','3','2'}},
								{{'T','C','3','2'}},
							{{'M','A','3','0'}},
								{{'M','A','3','1'}},
								{{'M','C','3','1'}},
								{{'M','A','3','2'}},
								{{'M','C','3','2'}},
							{{'I','U','9','7'}},
								{{'I','U','8','7'}},
								{{'I','V','8','7'}},
							{{'I','A','3','0'}},
								{{'I','A','3','2'}},
								{{'I','C','3','2'}},
							{{'P','A','3','0'}},
								{{'P','A','3','1'}},
								{{'P','C','3','1'}},
							{{'Q','A','4','0'}},
							{{'P','A','3','C'}},
								{{'P','A','C','1'}},
							{{'T','A','4','0'}},
							{{'M','A','4','0'}},
							{{'I','A','4','0'}},
							{{'Q','A','5','0'}},
							{{'T','A','5','0'}},
							{{'M','A','5','0'}},
							{{'I','A','5','0'}},
							{{'P','A','5','0'}},
							{{'S','A','5','0'}},
							{{'C','S','A','S'}}
						};
						
#define GEN5_COUNT 40

BoardName GEN5_LIST[GEN5_COUNT] = {
					{{'T','S','C','O'}},
						{{'B','H','C','O'}},
						{{'B','Q','C','O'}},
					{{'T','S','C','R'}},
					{{'T','C','T','L'}},
						{{'B','V','T','L'}},
						{{'B','U','T','L'}},
					{{'G','A','T','L'}},
						{{'G','U','T','L'}},
						{{'G','V','T','L'}},
					{{'T','C','T','O'}},
						{{'B','V','T','O'}},
						{{'B','U','T','O'}},
					{{'T','S','T','L'}},
						{{'B','H','T','L'}},
						{{'B','Q','T','L'}},
					{{'G','B','T','L'}},
						{{'G','Q','T','L'}},
						{{'G','H','T','L'}},
					{{'B','7','E','3'}},
					{{'B','7','E','4'}},
					{{'S','A','7','0'}},
					{{'P','A','7','0'}},
					{{'M','A','7','0'}},
					{{'Q','A','7','0'}},
					{{'T','A','7','0'}},
					{{'I','C','W','L'}},
						{{'I','U','W','L'}},
						{{'I','V','W','L'}},
					{{'J','C','W','L'}},
						{{'J','U','W','L'}},
						{{'J','V','W','L'}},
					{{'T','C','W','L'}},
						{{'B','U','W','L'}},
						{{'B','V','W','L'}},
					{{'H','S','I','L'}},
					{{'T','C','V','2'}},
					{{'D','I','C','L'}},
					{{'T','C','A','L'}},
					{{'H','E','I','H'}}
				   };									  
#endif
