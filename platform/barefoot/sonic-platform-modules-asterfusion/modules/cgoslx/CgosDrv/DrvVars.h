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

//***************************************************************************

#ifndef _DRVVARS_H_
#define _DRVVARS_H_

//***************************************************************************

#define CGOS_DRV_BOARD_MAX 4
#define CGOS_DRV_STO_MAX 16
#define CGOS_DRV_WDOG_MAX 4
#define CGOS_DRV_VGA_MAX 4
#define CGOS_DRV_IO_MAX 8

#define CGOS_DRV_TEMP_MAX 16
#define CGOS_DRV_VOLT_MAX 16
#define CGOS_DRV_FAN_MAX 16
#define CGOS_DRV_I2C_TABLE_MAX 32

//MOD24 v
#define BC_TYPE_GEN5 0x01
#define BC_TYPE_BC4_BASE_E 0x02
#define BC_TYPE_BC4_BASE_C 0x03
//MOD24 ^
//***************************************************************************

typedef struct {
  CGEB_STORAGEAREA_INFO info;
  } CGOS_DRV_STO;
  
typedef struct {
  CGOSWDCONFIG config;
  CGOSWDINFO info;
  } CGOS_DRV_WDOG;

typedef struct {
  unsigned int value;
  unsigned int valueValid;
  unsigned int enabled;
  unsigned int enabledValid;
  } CGOS_DRV_DAC;

typedef struct {
  CGOSVGAINFO info;
  CGOS_DRV_DAC backlight;
  CGOS_DRV_DAC contrast;
  } CGOS_DRV_VGA;

typedef struct {
  unsigned int num;
  unsigned int flags;
  unsigned int outmask;
  unsigned int inmask;
  unsigned int shift;
  unsigned int value;
  unsigned int valueValid;
  } CGOS_DRV_IO;
  
typedef struct {
	unsigned long in;
	unsigned long out;
}CGOS_DRV_IO_DIR_CAPS;
  
//***************************************************************************

typedef struct {
	CGOSTEMPERATUREINFO tempinfo;
	unsigned char BCUnitNumber;
}CGOS_DRV_TEMP_ENTRY;

typedef struct {
	CGOSVOLTAGEINFO voltinfo;
	unsigned char BCUnitNumber;
}CGOS_DRV_VOLT_ENTRY;

typedef struct {
	CGOSFANINFO faninfo;
	unsigned char BCUnitNumber;
}CGOS_DRV_FAN_ENTRY;

typedef struct {
	CGOS_DRV_TEMP_ENTRY tempsensors[CGOS_DRV_TEMP_MAX];
	CGOS_DRV_VOLT_ENTRY voltsensors[CGOS_DRV_VOLT_MAX];
	CGOS_DRV_FAN_ENTRY  fansensors[CGOS_DRV_FAN_MAX];
}CGOS_DRV_HWM;

typedef struct{
	unsigned int tempCount;
	unsigned int voltCount;
	unsigned int fanCount;
}CGOS_HWM_COUNT;

//***************************************************************************

typedef struct {
  unsigned char *entry;
  unsigned char *code;
  unsigned char *data;
  unsigned short ds;
  void *mapMem;
  unsigned char *hiDescStart;
  unsigned int hiDescLen;
  } CGOS_DRV_CGEB;
  
//***************************************************************************

typedef struct {
	unsigned int cgostype;
	unsigned int cgbctype;
}CGOS_DRV_I2C_ENTRY;

//***************************************************************************

typedef struct {
  CGOSBOARDINFOA info;
  unsigned char BC_Type;
  char BoardName[4];
  unsigned char BoardGroup;
  unsigned int validFlags;
  unsigned int stoCount;
  unsigned int stoPrivCount;
  CGOS_DRV_STO sto[CGOS_DRV_STO_MAX];
  CGOS_DRV_STO stoPriv[CGOS_DRV_STO_MAX];
  unsigned int i2cCount;  
  CGOS_DRV_I2C_ENTRY i2cTable[CGOS_DRV_I2C_TABLE_MAX];
  unsigned int wdogCount;
  CGOS_DRV_WDOG wdog[CGOS_DRV_WDOG_MAX];
  unsigned int vgaCount;
  CGOS_DRV_VGA vga[CGOS_DRV_VGA_MAX];
  unsigned int ioCount;
  CGOS_DRV_IO io[CGOS_DRV_IO_MAX];
  CGOS_DRV_IO_DIR_CAPS iocaps[CGOS_DRV_IO_MAX];
  CGOS_HWM_COUNT hwmCount;
  CGOS_DRV_HWM hwm;  
  CGOS_DRV_CGEB cgeb;
  } CGOS_DRV_BOARD;

//***************************************************************************/

typedef struct {
  // back pointer to OS specific vars
  void *osDrvVars;

  // boards
  unsigned int boardCount;
  CGOS_DRV_BOARD boards[CGOS_DRV_BOARD_MAX];

  // parameter passing
  CGOSIOCTLIN *cin;
  CGOSIOCTLOUT *cout;
  unsigned int nInBufferSize;
  unsigned int nOutBufferSize;
  unsigned int retcnt;
  unsigned int status;

  // pure data buffers
  void *pin;
  void *pout;
  unsigned int lin;
  unsigned int lout;
  // translated type
  unsigned int unit;

  // translated pointers
  CGOS_DRV_BOARD *brd;
  CGOS_DRV_STO *sto;
  CGOS_DRV_WDOG *wdog;
  CGOS_DRV_VGA *vga;
  CGOS_DRV_IO *io;
  CGOS_DRV_HWM *hwm;
  CGOS_DRV_I2C_ENTRY *i2c;

  //  unsigned int stotype,stosize;

  } CGOS_DRV_VARS;


//***************************************************************************

typedef unsigned int(*CgosFunctionPointer)(CGOS_DRV_VARS *cdv);
typedef unsigned int(*CgosCommandPointer)(unsigned char *cmdDataBPtr, unsigned char  cmdByteCount,
					   unsigned char *rcvDataBPtr, unsigned char  rcvByteCount,
					   unsigned char *retValueBPtr );

//***************************************************************************
//Enumerations for SubModule handling:

//BCSubModules:
enum{BC_SUBMODULE_BC4_BASE_E, BC_SUBMODULE_BC4_BASE_C, BC_SUBMODULE_GEN5};

//IOSubModules:
enum{IO_SUBMODULE_BC, IO_SUBMODULE_CSAS};

//I2CSubModules:
enum{I2C_SUBMODULE_BC};

//WDSubModules:
enum{WD_SUBMODULE_BC};

//HWMSubModules:
enum{HWM_SUBMODULE_BC, HWM_SUBMODULE_CSAS};

//VgaSubModules:
enum{VGA_SUBMODULE_BC};

//StoSubModules:
enum{STO_SUBMODULE_BC};
//***************************************************************************
//Board Groups

enum{GROUP_DEFAULT=0, GROUP_CSAS};

//***************************************************************************
#endif
