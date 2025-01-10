congatec Linux CGOS Package
============================

This package contains the following components:

CgosDrv		: congatec CGOS kernel mode driver for Linux.
CgosLib		: congatec CGOS interface library for Linux.
CgosDump	: Simple CGOS tool to verify correct CGOS installation.
CgosMon		: Simple CGOS hardware monitoring utility.
CgosTest	: Test tool and code reference for accessing all CGOS functions.


Installation:
-------------

- For Ubuntu, run

  sudo apt install build-essential

  Since Ubuntu 18.04, the package libelf-dev has to be installed, too:

  sudo apt install libelf-dev

- For Fedora, run

  sudo dnf install @development-tools elfutils-libelf-devel kernel-devel-$(uname -r)

- in the package root folder, compile the sources using 
  
  make

- after that, install with

  sudo make install
  
- If libcgos.so can not be found (e.g. on Fedora), you need to (re-)configure your dynamic linker
  run-time bindings by running 

  sudo ldconfig

- cgostest contains sample code for all CGOS functions. It is recommended to use it 
  for verifying functionality and as a reference for writing your own applications.

Notes:
------

When running make install, the udev rule 99-cgos.rules will be copied to
/lib/udev/rules.d if the file does not already exist there. It will set the permission
for accessing the CGOS driver to "0666" (r/w for everyone). You can change the permissions 
according to your needs either by modifying the rule file or by adding your custom user 
rule to /etc/udev/rules.d.

  permission to:  owner      group      other     
                  /¯¯¯\      /¯¯¯\      /¯¯¯\
  octal:            6          6          6
  binary:         1 1 0      1 1 0      1 1 0
  what to permit: r w x      r w x      r w x

  binary         - 1: enabled, 0: disabled

  what to permit - r: read, w: write, x: execute

  permission to  - owner: the user that created the file/folder
                   group: the users from the group where the owner is member
                   other: all other users

Additionally, a cgos.conf file will be copied to /usr/lib/modules-load.d
for having the cgosdrv kernel module loaded after each boot per default.


LICENSE:
--------
The congatec Linux CGOS package is free software.
You can redistribute it and/or modify it under the terms of the applicable
licenses that accompany this distribution.

The CGOS kernel mode driver (CgosDrv) ist distributed under the terms of the 
GNU General Public License version 2.
The CGOS interface library (CgosLib) and the included CGOS tools (CgosDump, CgosMon)
are distributed under the terms of the BSD 2-clause license.

Please refer to the license information in the source files and in the 
single COPYING_xxx file(s) for detailed information.



Implementation State:
---------------------
The current cgos_direct release has no software SMI based flash interface routines implemented
and thus no BIOS and cBC updates can be done. There is also some information that the driver cannot
get from the BIOS at the moment and some cgos function implementations are incomplete or missing
at the current state. The following shows the deviations from the cgos API manual.

========================================================================================================
CGOS Board Functions:

The Cgos Board capabilities rely on the following information stored in the CGOSBOARDINFO struct.

Parameter			Implementation State
---------------------------------------------------------------------------------------
dwSize				Implemented, simple sizeof(CGOSBOARDINFO)	
dwFlags				Not implemented
szReserved			Not implemented
szBoard				Implemented, derived from cBC
szBoardSub			Implemented, derived from cBC. Same Value as szBoard.
szManufacturer			Not implemented, a Manufacturing ID can be derived from 
				the cBC, see dwManufacturer, needs link from ID to a 
				Manufacturer Name.
stManufacturingDate		Implemented. Day, Month and Year taken from cBC.
stLastRepairDate		Implemented. Day, Month and Year taken from cBC.
szSerialNumber			Implemented, taken from cBC
wProductRevision		Implemented, taken from cBC
wSystemBIOSRevision		Implemented for Linux, this can be taken from SMBIOS.
wBIOSInterfaceRevision		Not implemented
wBIOSInterfaceBuildRevision	Not implemented
dwClasses			Not implemented
dwPrimaryClass			Not implemented
dwRepairCounter			Implemented, taken from cBC
szPartNumber			Implemented, taken from cBC
szEAN				Implemented, taken from cBC
dwManufacturer			Implemented, taken from cBC. Manufacturer ID, needs 
				link to manufacturer name
---------------------------------------------------------------------------------------

Cgos Function			Implementation State
---------------------------------------------------------------------------------------
CgosBoardClose			Implemented, taken from old driver	
CgosBoardCount			Implemented, taken from old driver	
CgosBoardOpen			Implemented, taken from old driver	
CgosBoardOpenByNameA		Implemented, taken from old driver	
CgosBoardGetNameA		Implemented, taken from old driver	
CgosBoardGetInfoA		Implemented
CgosBoardGetBootCounter		Implemented, feature might not be supported by the cBC	
CgosBoardGetRunningTimeMeter	Implemented, feature might not be supported by the cBC	
CgosBoardGetOption		Not Implemented	
CgosBoardSetOption		Not Implemented	
CgosBoardGetBootErrorLog	Not Implemented
---------------------------------------------------------------------------------------

========================================================================================================
Cgos Hardware Monitoring Functions:

The Cgos Direct driver only supports sensors that are managed by the cBC.

Cgos Function			Implementation State
---------------------------------------------------------------------------------------
CgosTemperatureCount		Implemented	
CgosTemperatureGetInfo		Implemented	
CgosTemperatureGetCurrent	Implemented	
CgosTemperatureSetLimits	Not implemented	
CgosFanCount			Implemented	
CgosFanInfo			Implemented	
CgosFanGetCurrent		Implemented	
CgosFanSetLimits		Implemented	
CgosVoltageCount		Implemented	
CgosVoltageInfo			Implemented	
CgosVoltageGetCurrent		Implemented	
CgosVoltageSetLimits		Not implemented	
CgosPerformanceGetCurrent	Neither implemented nor planed since not implemented 
				in the old driver either.	
CgosPerformanceSetCurrent	Neither implemented nor planed since not implemented 
				in the old driver either.
CgosPerformanceGetPolicyCaps	Neither implemented nor planed since not implemented 
				in the old driver either.	
CgosPerformanceGetPolicy	Neither implemented nor planed since not implemented 
				in the old driver either.	
CgosPerformanceSetPolicy	Neither implemented nor planed since not implemented 
				in the old driver either.
---------------------------------------------------------------------------------------

========================================================================================================
I2C Functions:

Cgos Function			Implementation State
---------------------------------------------------------------------------------------
I2CSubModule_BC	CgosI2CCount	Implemented, hardcoded to 6. Two hardcoded mappings 
				available, one for cBC4 and one for GEN5. No mapping
				for COM-HPC available yet.
CgosI2CIsAvailable		Implemented	
CgosI2CType			Implemented, hardcoded	
CgosI2CRead			Implemented
CgosI2CWrite			Implemented
CgosI2CReadRegister		Implemented	
CgosI2CWriteRegister		Implemented	
CgosI2CWriteReadCombined	Implemented
CgosI2CGetMaxFrequency		Implemented
CgosI2CGetFrequency		Implemented
CgosI2CSetFrequency		Implemented
---------------------------------------------------------------------------------------

========================================================================================================
IO Functions:

The IO Count is hardcoded to 1 and the capabilities for this one IO are hardcoded to

{0x000000ff, //in
 0x0000ff00  //out}

IO Module might show additional errors, since different behaviour to legacy driver was
experienced on some occasions, currently under investigation.

Cgos Function			Implementation State
---------------------------------------------------------------------------------------
CgosIOCount			Implemented, hardcoded to 1	
CgosIORead			Implemented for GPIOs handled by the cBC	
CgosIOWrite			Implemented for GPIOs handled by the cBC	
CgosIOGetDirection		Implemented for GPIOs handled by the cBC	
CgosIOSetDirection		Implemented for GPIOs handled by the cBC	
CgosIOIsAvailable		Implemented for GPIOs handled by the cBC	
CgosIOGetDirectionCaps		Implemented, info hardcoded
---------------------------------------------------------------------------------------

========================================================================================================
Storage Functions:

Some storage areas shown by the legacy Cgos driver like CMOS are not implemented and
at the moment there are no plans to do so.

Cgos Function			Implementation State
---------------------------------------------------------------------------------------
CgosStorageAreaCount		Implemented	
CgosStorageAreaType		Implemented
CgosStorageAreaSize		Implemented. Can check if dwUnit input is to be 
				interpreted as index or as storage Unit type.
CgosStorageAreaBlockSize	Implemented. Can check if dwUnit input is to be 
				interpreted as index or as storage Unit type.
CgosStorageAreaRead		Implemented. Can check if dwUnit input is to be 
				interpreted as index or as storage Unit type.
CgosStorageAreaWrite		Implemented. Can check if dwUnit input is to be 
				interpreted as index or as storage Unit type.
CgosStorageAreaErase		Implemented. Can check if dwUnit input is to be 
				interpreted as index or as storage Unit type.
CgosStorageAreaEraseStatus	Implemented. Can check if dwUnit input is to be 
				interpreted as index or as storage Unit type.
CgosStorageAreaLock		Implemented, works only for CGOS_STORAGE_AREA_EEPROM 
				as stated in the cgos manual. Can check if dwUnit input 
				is to be interpreted as index or as storage Unit type.
CgosStorageAreaUnlock		Implemented, works only for CGOS_STORAGE_AREA_EEPROM 
				as stated in the cgos manual. Can check if dwUnit input 
				is to be interpreted as index or as storage Unit type.
CgosStorageAreaIsLocked		Implemented
---------------------------------------------------------------------------------------

========================================================================================================
VGA Functions:

Cgos Function			Implementation State
---------------------------------------------------------------------------------------
CgosVgaCount 			Not implemented	
CgosVgaGetInfo			Not implemented	
CgosVgaGetContrast		Not implemented	
CgosVgaSetContrast		Not implemented	
CgosVgaGetContrastEnable	Not implemented	
CgosVgaSetContrastEnable	Not implemented	
CgosVgaGetBacklight		Not implemented	
CgosVgaSetBacklight		Not implemented	
CgosVgaGetBacklightEnable	Not implemented		
CgosVgaSetBacklightEnable	Not implemented	
CgosVgaEndDarkBoot		Not implemented
---------------------------------------------------------------------------------------

========================================================================================================
Watchdog Functions:

The Watchdog count is hardcoded to 1. There are two important information structures for the
watchdog module, the first one is CGOSWDCONFIG:

Parameter		Initialized with
---------------------------------------------------------------------------------------
dwSize			sizeof(CGOSWDCONFIG)
dwTimeout		0
dwDelay			0
dwMode			CGOS_WDOG_MODE_STAGED
dwOpMode		CGOS_WDOG_OPMODE_SINGLE_EVENT
dwStageCount		CGOS_WDOG_EVENT_MAX_STAGES
stStage[0]		{0,CGOS_WDOG_EVENT_RST}
stStage[1]		{0,CGOS_WDOG_EVENT_RST}
stStage[2]		{0,CGOS_WDOG_EVENT_RST}
---------------------------------------------------------------------------------------

The second structure is CGOSWDINFO:

Parameter		Initialized with
---------------------------------------------------------------------------------------
dwSize			sizeof(CGOSWDINFO)
dwFlags			0
dwMinTimeout		0
dwMaxTimeout		0xFFFFFF
dwMinDelay		0
dwMaxDelay		0xFFFFFF
dwOpModes		0xF
dwMaxStageCount		3
dwEvents		0xF
dwType			CGOS_WDOG_TYPE_BC
---------------------------------------------------------------------------------------

Cgos Function			Implementation State
---------------------------------------------------------------------------------------
CgosWDogGetInfo			Implemented	
CgosWDogCount			Implemented, hardcoded to 1	
CgosWDogIsAvailable		Implemented	
CgosWDogTrigger			Implemented	
CgosWDogGetTriggerCount		Not implemented	
CgosWDogSetTriggerCount		Not implemented	
CgosWDogGetConfigStruct		Implemented	
CgosWDogSetConfigStruct		Implemented	
CgosWDogSetConfig		Implemented	
CgosWDogDisable			Implemented
---------------------------------------------------------------------------------------


