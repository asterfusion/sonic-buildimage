/*---------------------------------------------------------------------------
 *
 * Copyright (c) 2022, congatec GmbH. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the BSD 2-clause license which 
 * accompanies this distribution. 
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the BSD 2-clause license for more details.
 *
 * The full text of the license may be found at:        
 * http://opensource.org/licenses/BSD-2-Clause   
 *
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 *
 * Contents: Board controller common definitions.
 *
 *---------------------------------------------------------------------------
 *
 *                      R E V I S I O N   H I S T O R Y
 *
 * MOD071: Added support for a 32-bit boot time/system up time meter with a 1ms
 *         granularity.
 * MOD070: Revised the definition and usage of standard data types to support
 *         64-bit environments properly.
 * MOD069: Added logical pin function for BRD_PWR_EN.
 * MOD068: Added missing logical pin functions for SUSPWRDNACK, SLP_M#,
 *         SLP_LAN# and SLP_WLAN#.
 * MOD067: Added support for logging special system events such as eSPI events.
 * MOD066: Added CGBC_POST_SEC_DISPLAY_DIS bit definition.
 *
 * MOD065: Added support for dedicated SPD SMBuses CGBC_I2C_BUS_DDR0 and
 *         CGBC_I2C_BUS_DDR1.
 * MOD064: Added support for a power control event logging group and for event
 *         logging of virtual pins, eSPI virtual wires and HWM values.
 * MOD063: Added support for general purpose SPI (GSPI).
 *
 * MOD062: Changed congatec board controller type name for the MEC170x based
 *         congatec Board Controllers.
 * MOD061: Added new CGBC API command CGBC_CMD_AVR_SPM_EXT.
 * MOD060: Added support for the new pin function CGBC_PIN_FNC_SPD_SMB_MUX.
 * MOD059: Added support for extended cBC I2C API.
 * MOD058: Added the ui8Usage field in the HWM control unit configuration
 *         parameters used to identify specific HWM control units.
 * MOD057: Improved definitions of the HWM unit configuration parameter
 *         structures
 * MOD056: Added support for MCU GPIO pins being used as binary HWM sensors and
 *         actuators.
 * MOD055: Added support for UART_MUX_PCH pin.
 * MOD054: Added support for a voltage tolerant open-drain output pin mode.
 * MOD053: Added support for enabling/disabling DHCC.
 * MOD052: Added support for COM-HPC (new I2C/SMBuses I2C1, IPMB, UPDB, new
 *         pin functions CGBC_PIN_FNC_PMBUS_MUX, CGBC_PIN_FNC_TESTn,
 *         CGBC_PIN_FNC_VIN_PWROK, CGBC_PIN_FNC_RAPID_SHUTDOWN
 *         CGBC_PIN_FNC_UART_MUX_PCHn, CGBC_PIN_FNC_THRMn and
 *         CGBC_PIN_FNC_PROCHOTn, new logical HWM sensor types.
 *
 * MOD051: Changes to allow a clear identification of two DIMM temperature
 *         sensors on the top side of the PCB.
 *
 * MOD050: Added Support for MEC1705 battery mode
 *
 * MOD049: Added support for G781 temperature sensor
 *
 * *    Rev 1.35   Feb 12 2020 13:40:16   ess
 * MOD048: Added new pin functions V5A_CTL and HOST_RST_ACK.
 * 
 *    Rev 1.34   Dec 12 2019 15:46:40   ess
 * MOD047: Added support for POST code event logging and repeated trigger
 *         mode.
 * 
 *    Rev 1.33   Dec 10 2019 09:51:20   ess
 * MOD046: Update to latest GEN5 cBC functionality and minor cleanup.
 * 
 *    Rev 1.32   Mar 18 2019 15:56:12   ess
 * MOD045: added latest CM41 enhancments and corrected actuator stepping
 *         width parameters
 * 
 *    Rev 1.31   Mar 18 2019 13:07:34   ess
 * MOD044: Update to latest GEN5 cBC functionality
 * 
 *    Rev 1.30   Sep 18 2018 14:42:52   ess
 * MOD043: put CGBC_HWM_DATA type definition back in which had been commented
 *         out by mistake in rev 1.28 and rev 1.29
 * 
 *    Rev 1.29   Apr 03 2018 08:53:28   ess
 * MOD042: added support for MEC1705
 * 
 *    Rev 1.28   Sep 06 2016 15:45:56   congatec
 * Added BSD header.
 * 
 *    Rev 1.27   Jun 03 2016 11:17:24   ess
 * MOD041: added new host commands for second LFP control, diagnostic output
 *         interface configuration and GPI IRQ support
 * 
 *    Rev 1.26   Apr 05 2016 16:55:28   ess
 * MOD040: update for latest BC functionality including POST code redirection,
 *          LFP backlight delay and HWM configuration
 * 
 *    Rev 1.25   Feb 26 2014 10:20:44   ess
 * MOD039: more changes concerning 64bit porting
 * MOD038: added support for new commands CGBC_CMD_AVR_SPM,
 *         CGBC_CMD_DEVICE_ID and CGBC_CMD_HWM_SENSOR
 * 
 *    Rev 1.24   Nov 05 2013 16:38:54   ess
 * MOD037: HIF_ID corrected
 * MOD036: added CGBC_CMD_BLT_PWM
 * 
 *    Rev 1.23   Oct 18 2013 12:06:32   ess
 * MOD035: added conditional #defines for uint8_t, uint16_t and uint32_t in
 *         case they are not defined yet
 * MOD034: added support for battery systems on SMBus
 * MOD033: added more SMBus numbers
 * MOD032: added error code CGBC_ERR_ABORT
 * 
 *    Rev 1.22   Aug 19 2013 23:14:56   ess
 * MOD031: added CGBC_CMD_COMX_CFG
 * 
 *    Rev 1.21   Jun 26 2013 10:51:08   ess
 * MOD030: added support for TM4E1231H6ZRB
 * MOD029: added CGBC_CMD_AVR_SPM
 * 
 *    Rev 1.20   Mar 25 2013 14:49:34   ess
 * MOD028: added conditional #defines for UINT8 and UINT32 in case they are
 *         not defined yet
 * MOD027: added CM41 BC family declarations
 * 
 *    Rev 1.19   Jun 27 2012 11:06:26   ess
 * MOD026: more changes concerning 64bit porting
 * MOD025: added support for system configuration pins
 * MOD024: added support for SMBus gate
 * 
 *    Rev 1.18   May 31 2012 15:45:34   gartner
 * Updated variable definition to ease 64bit porting.
 * 
 *    Rev 1.16   Mar 21 2011 09:25:54   ess
 * MOD023: added PECI support
 * MOD022: added RTC support
 * 
 *    Rev 1.15   Dec 02 2010 11:32:04   ess
 * MOD021: completed board controller command parameter union
 * MOD020: added fan control support
 * MOD019: added support for hot S5
 * MOD018: added support for STM32F100R8
 * 
 *    Rev 1.14   Jul 22 2009 16:33:52   ess
 * MOD017: update according to the latest CGBC functionality including
 *         button control support
 * 
 *    Rev 1.13   May 12 2009 09:29:42   ess
 * MOD016: added GPIO commands
 * 
 *    Rev 1.12   Dec 18 2008 15:46:34   ess
 * MOD015: added support for ATmega325P
 * 
 *    Rev 1.11   Aug 26 2008 21:56:00   ess
 * MOD014: update according to the latest CGBC functionality including
 *           support for ATmega165P
 * MOD013: changes to allow compilation in the AVR GCC environment
 * 
 *    Rev 1.10   Aug 07 2007 11:21:24   ess
 * MOD012: added feature byte 1 and the SBSM emulation flag
 * 
 *    Rev 1.9   Apr 13 2007 15:55:58   ess
 * MOD011: added RTC_PRESENT bit in power loss parameter
 * MOD010: added support for CGBC_CMD_SCI_STOP command
 * 
 *    Rev 1.8   Oct 30 2006 21:54:10   ess
 * MOD009: added declaration for CGBC_CMD_CPU_TEMP
 * 
 *    Rev 1.7   Mar 02 2006 20:52:40   ess
 * MOD008: changed watchdog parameter declarations from integer to short to
 *         avoid wrong structure in WIN32 environment
 * 
 *    Rev 1.6   Mar 01 2006 19:18:08   ess
 * MOD007: further corrections on battery management declarations
 * 
 *    Rev 1.5   Jan 19 2006 15:06:02   ess
 * MOD006: corrected declaration of battery flags
 * 
 *    Rev 1.4   Jan 03 2006 17:20:30   ess
 * MOD005: declarations updated for latest board controller enhancements
 * 
 *    Rev 1.3   Dec 23 2005 01:57:20   ess
 * MOD004: declarations updated for latest board controller enhancements
 * 
 *    Rev 1.2   Nov 25 2005 18:30:16   ess
 * MOD003: declarations updated for latest board controller enhancements
 * 
 *    Rev 1.1   Oct 20 2005 11:26:32   ess
 * MOD002: updates from latest board controller firmware revision CGBCP121
 * MOD001: added pragma pack(1) to ensure correct structure offsets
 * 
 *    Rev 1.0   Sep 30 2005 16:29:08   gartner
 * Initial revision.
 *
 *---------------------------------------------------------------------------
 */



#ifndef CGBC_H_INCLUDED
#define CGBC_H_INCLUDED                 /* This avoids multiple inclusions. */



#ifndef __AVR__                                                      //MOD013
#pragma pack(1)                                                      //MOD001
#endif //!__AVR__                                                    //MOD013

#ifdef _MSC_VER                                                    /*MOD070 */
                                                                   /*MOD039v*/
#ifndef  SINT16
#define  SINT16 signed short
#endif /*SINT16*/
                                                                   /*MOD039^*/
                                                                   /*MOD026v*/
#ifndef  UINT16
#define  UINT16 unsigned short
#endif /*UINT16*/
                                                                   /*MOD026^*/
                                                                   /*MOD028v*/
#ifndef  UINT8
#define  UINT8 unsigned char
#endif /*UINT8*/

#ifndef  UINT32
#define  UINT32 unsigned long
#endif /*UINT32*/
                                                                   /*MOD028^*/
                                                                   /*MOD035v*/
#ifndef  uint8_t
#define  uint8_t unsigned char
#endif /*uint8_t*/

#ifndef  uint16_t
#define  uint16_t unsigned short
#endif /*uint16_t*/

#ifndef  uint32_t
#define  uint32_t unsigned long
#endif /*uint32_t*/
                                                                   /*MOD035^*/
                                                                   /*MOD046v*/
#ifndef  int8_t
#define  int8_t signed char
#endif /*int8_t*/

#ifndef  int16_t
#define  int16_t signed short
#endif /*int16_t*/

#ifndef  int32_t
#define  int32_t signed long
#endif /*int32_t*/
                                                                     /*MOD070v*/

//MOD24 v not needed, typedef int wchar_t in DrvOsHdr.h

// #else //#ifdef _MSC_VER

//#include <stdint.h> 
//MOD24 ^ 
              
#endif //#ifdef _MSC_VER
                                                                   /*MOD070^*/ 
                                                                   /*MOD046v*/
/*------------------------------------
 * congatec Board Controller families
 *------------------------------------
 */

#define CGBC_FAMILY_AVR1 1                   /* Atmel AVR  / ATmega48/88/168 */
#define CGBC_FAMILY_AVR2 2                   /* Atmel AVR  / ATmega165P/325P */
#define CGBC_FAMILY_CM31 3                   /* Cortex-M3  / STM32F100R8     */
#define CGBC_FAMILY_CM41 4                   /* Cortex-M4F / TM4E1231H6ZRB   */
#define CGBC_FAMILY_GEN5 5                   /* Cortex-M4F / MEC170x         */

                                                                   /*MOD044^*/

/*--------------------
 * CGBC host commands
 *--------------------
 */

#define CGBC_CMD_GET_STATUS      0x00
#define CGBC_CMD_GET_DATA_1      0x01
#define CGBC_CMD_GET_DATA_2      0x02
#define CGBC_CMD_GET_DATA_3      0x03
#define CGBC_CMD_GET_DATA_4      0x04
#define CGBC_CMD_GET_DATA_5      0x05
#define CGBC_CMD_GET_DATA_6      0x06
#define CGBC_CMD_GET_DATA_7      0x07
#define CGBC_CMD_GET_DATA_8      0x08
#define CGBC_CMD_GET_DATA_9      0x09
#define CGBC_CMD_GET_DATA_10     0x0A
#define CGBC_CMD_GET_DATA_11     0x0B
#define CGBC_CMD_GET_DATA_12     0x0C
#define CGBC_CMD_GET_DATA_13     0x0D
#define CGBC_CMD_GET_DATA_14     0x0E
#define CGBC_CMD_GET_DATA_15     0x0F
#define CGBC_CMD_GET_DATA_16     0x10
#define CGBC_CMD_GET_DATA_17     0x11
#define CGBC_CMD_GET_DATA_18     0x12
#define CGBC_CMD_GET_DATA_19     0x13
#define CGBC_CMD_GET_DATA_20     0x14
#define CGBC_CMD_GET_DATA_21     0x15
#define CGBC_CMD_GET_DATA_22     0x16
#define CGBC_CMD_GET_DATA_23     0x17
#define CGBC_CMD_GET_DATA_24     0x18
#define CGBC_CMD_GET_DATA_25     0x19
#define CGBC_CMD_GET_DATA_26     0x1A
#define CGBC_CMD_GET_DATA_27     0x1B
#define CGBC_CMD_GET_DATA_28     0x1C
#define CGBC_CMD_GET_DATA_29     0x1D
#define CGBC_CMD_GET_DATA_30     0x1E
#define CGBC_CMD_GET_DATA_31     0x1F
#define CGBC_CMD_GET_DATA_32     0x20
#define CGBC_CMD_GET_FW_REV      0x21
#define CGBC_CMD_AUX_INIT        0x22
#define CGBC_CMD_AUX_OUT         0x23
#define CGBC_CMD_AUX_IN          0x24
#define CGBC_CMD_AUX_STAT        0x25
#define CGBC_CMD_WDOG_EVENT      0x26                                //MOD046
#define CGBC_CMD_WD_EVENT        0x26 /* Deprecated */
#define CGBC_CMD_WDOG_TRIGGER    0x27                                //MOD046
#define CGBC_CMD_WD_TRIGGER      0x27 /* Deprecated */
#define CGBC_CMD_WDOG_INIT       0x28                                //MOD046
#define CGBC_CMD_WD_INIT         0x28 /* Deprecated */
#define CGBC_CMD_WDOG_INIT_POST  0x29                                //MOD046
#define CGBC_CMD_WD_INIT_POST    0x29 /* Deprecated */
#define CGBC_CMD_WDOG_INIT_EARLY 0x2A                                //MOD046
#define CGBC_CMD_WD_INIT_EARLY   0x2A /* Deprecated */
#define CGBC_CMD_SYS_FLAGS_1     0x2B
#define CGBC_CMD_SCI_EVT_QUERY   0x2C
#define CGBC_CMD_STACK_INFO      0x2D
#define CGBC_CMD_INFO_1          0x2E
#define CGBC_CMD_LFP0_CONTROL    0x2F                                //MOD044
#define CGBC_CMD_LFP_CONTROL     0x2F /* Deprecated */        //MOD040 MOD044
#define CGBC_CMD_VID_CONTROL     0x2F /* Deprecated */               //MOD040
#define CGBC_CMD_TEST_COUNTER    0x30
#define CGBC_CMD_BOOT_TIME       0x31
#define CGBC_CMD_SPI_TIMING      0x32
#define CGBC_CMD_DUMP            0x33                                //MOD002
#define CGBC_CMD_RESET_INFO      0x34                                //MOD002
#define CGBC_CMD_POST_CODE       0x35                                //MOD002
#define CGBC_CMD_PERFORMANCE     0x36                                //MOD002
#define CGBC_CMD_POWER_LOSS      0x37                                //MOD003
#define CGBC_CMD_XINT_CONTROL    0x38                                //MOD004
#define CGBC_CMD_USER_LOCK       0x39                                //MOD004
#define CGBC_CMD_OSCCAL          0x3A                                //MOD005
#define CGBC_CMD_CPU_TEMP        0x3B                                //MOD009
#define CGBC_CMD_SBSM_DATA       0x3C                         //MOD004 MOD005
#define CGBC_CMD_I2C_START       0x40
#define CGBC_CMD_I2C_STAT        0x48
#define CGBC_CMD_I2C_DATA        0x50
#define CGBC_CMD_I2C_SPEED       0x58                                //MOD002
#define CGBC_CMD_SCI_STOP        0x60                                //MOD010
#define CGBC_CMD_BTN_CONTROL     0x61                                //MOD017
#define CGBC_CMD_CPU_FAN_CONTROL 0x62                                //MOD046
#define CGBC_CMD_FAN_CONTROL     0x62 /* Deprecated */               //MOD020
#define CGBC_CMD_RTC             0x63                                //MOD022
#define CGBC_CMD_GPIO_DAT_RD     0x64                                //MOD016
#define CGBC_CMD_GPIO_DAT_WR     0x65                                //MOD016
#define CGBC_CMD_GPIO_CFG_RD     0x66                                //MOD016
#define CGBC_CMD_GPIO_CFG_WR     0x67                                //MOD016
#define CGBC_CMD_I2C_COMBINED    0x68                                //MOD014
#define CGBC_CMD_PECI            0x70                                //MOD023
#define CGBC_CMD_I2C_GATE        0x71                                //MOD024
#define CGBC_CMD_CFG_PINS        0x72                                //MOD025
#define CGBC_CMD_AVR_SPM         0x73                                //MOD029
#define CGBC_CMD_COMX_CFG        0x74                                //MOD031
#define CGBC_CMD_BLT0_PWM        0x75                                //MOD044
#define CGBC_CMD_BLT_PWM         0x75 /* Deprecated */        //MOD036 MOD044
#define CGBC_CMD_DEVICE_ID       0x76                                //MOD038
#define CGBC_CMD_HWM_SENSOR      0x77                                //MOD038
#define CGBC_CMD_POST_CFG        0x78                                //MOD040
#define CGBC_CMD_LFP0_DELAY      0x79                                //MOD044
#define CGBC_CMD_LFP_DELAY       0x79 /* Deprecated */        //MOD040 MOD044
#define CGBC_CMD_TEST_MODE       0x7A                                //MOD040
#define CGBC_CMD_HWM_CONFIG      0x7B                                //MOD040
#define CGBC_CMD_LFP1_CONTROL    0x7C                                //MOD044
#define CGBC_CMD_LF2_CONTROL     0x7C /* Deprecated */      //MOD041 MOD044
#define CGBC_CMD_LFP1_DELAY      0x7D                                //MOD044
#define CGBC_CMD_LF2_DELAY       0x7D /* Deprecated */        //MOD041 MOD044
#define CGBC_CMD_BLT1_PWM        0x7E                                //MOD044
#define CGBC_CMD_BL2_PWM         0x7E /* Deprecated */        //MOD041 MOD044
#define CGBC_CMD_DIAG_CFG        0x7F                                //MOD041
#define CGBC_CMD_GPIO_IER_RD     0x80                                //MOD041
#define CGBC_CMD_GPIO_IER_WR     0x81                                //MOD041
#define CGBC_CMD_GPIO_IRQ_RD     0x82                                //MOD041
#define CGBC_CMD_GPIO_IRQ_WR     0x83                                //MOD041
                                                                     //MOD044v
#define CGBC_CMD_GPIO_ENA_RD     0x84
#define CGBC_CMD_GPIO_ENA_WR     0x85
#define CGBC_CMD_PWROK_DELAY     0x86
#define CGBC_CMD_LOG_CFG         0x87
#define CGBC_CMD_LOG_DUMP        0x88
#define CGBC_CMD_LOG_READ        0x89
#define CGBC_CMD_LOG_WRITE       0x8A
#define CGBC_CMD_LOG_GPIO        0x8B
#define CGBC_CMD_API_CONFIG      0x8C
#define CGBC_CMD_COND_RTM        0x8D
#define CGBC_CMD_BLT0_PWM_DELAY  0x8E
#define CGBC_CMD_BLT1_PWM_DELAY  0x8F
#define CGBC_CMD_PIN_FEAT        0x90
#define CGBC_CMD_DIAG_MODE       0x91
#define CGBC_CMD_ACCESS_LOCK     0x92                                //MOD046
#define CGBC_CMD_ADV_USER_LOCK   0x92 /* Deprecated */
                                                                     //MOD044^
                                                                     //MOD046v
#define CGBC_CMD_BAT_DYN         0x93                                //MOD050
#define CGBC_CMD_BAT_STA         0x94                                //MOD050
#define CGBC_CMS_SYS_FAN_CONTROL 0x95
#define CGBC_CMD_GPIO_PIN_RD     0x96
#define CGBC_CMD_GPIO_PIN_WR     0x97
#define CGBC_CMD_WDOG_RD         0x98
#define CGBC_CMD_GPIO_IRQ_TBL    0x99
                                                                     //MOD046^
                                                                     //MOD059v
#define CGBC_CMD_I2C_START_EXT   0x9A
#define CGBC_CMD_I2C_STAT_EXT    0x9B
#define CGBC_CMD_I2C_DATA_EXT    0x9C
#define CGBC_CMD_I2C_SPEED_EXT   0x9D
#define CGBC_CMD_I2C_COMBI_EXT   0x9E
                                                                     //MOD059^
#define CGBC_CMD_AVR_SPM_EXT     0x9F                                //MOD061
#define CGBC_CMD_GSPI_CFG        0xA0                                //MOD063
#define CGBC_CMD_SYNC            0xFE                                //MOD014



/*---------------------------------------------------
 * CGBC status byte / AVR1, AVR2 and CM31 cBC family
 *---------------------------------------------------
 */

#define CGBC_BSY_BIT     7                             /* busy flag         */
#define CGBC_RDY_BIT     6                             /* ready flag        */
#define CGBC_STAT_MSK    ((1<<CGBC_BSY_BIT)|(1<<CGBC_RDY_BIT)) /* state msk */
#define CGBC_IDL_STAT    ((0<<CGBC_BSY_BIT)|(0<<CGBC_RDY_BIT)) /* IDLE      */
#define CGBC_BSY_STAT    ((1<<CGBC_BSY_BIT)|(0<<CGBC_RDY_BIT)) /* BUSY      */
#define CGBC_RDY_STAT    ((0<<CGBC_BSY_BIT)|(1<<CGBC_RDY_BIT)) /* READY     */
#define CGBC_ERR_STAT    ((1<<CGBC_BSY_BIT)|(1<<CGBC_RDY_BIT)) /* ERROR     */

#define CGBC_DAT_BIT     5                             /* data pending flag */
#define CGBC_DAT_PENDING (1<<CGBC_DAT_BIT)

#define CGBC_DAT_CNT_MSK 0x1F                          /* data count        */
#define CGBC_RET_COD_MSK 0x1F                          /* return code       */
#define CGBC_ERR_COD_MSK 0x1F                          /* error code        */
#define CGBC_IDL_FLG_MSK 0x1F                          /* idle flags        */


                                                                    /*MOD027v*/
/*-----------------------------------------------------
 * CGBC command status byte / CM41 and GEN5 cBC family
 *-----------------------------------------------------
 */
typedef uint8_t CGBC_STATUS;                                         //MOD046

#define CGBC_ERR_BIT     7                                     /* error flag */
/*efine CGBC_RDY_BIT     6                                        ready flag */
/*efine CGBC_STAT_MSK    ((1<<CGBC_ERR_BIT)|(1<<CGBC_RDY_BIT))    state mask */
#define CGBC_DAT_STAT    ((0<<CGBC_ERR_BIT)|(0<<CGBC_RDY_BIT)) /* DATA READY */
/*efine CGBC_RDY_STAT    ((0<<CGBC_ERR_BIT)|(1<<CGBC_RDY_BIT))    CMD READY  */
/*efine CGBC_ERR_STAT    ((1<<CGBC_ERR_BIT)|(1<<CGBC_RDY_BIT))    ERROR      */
                                                                    /*MOD027^*/


/*-------------
 * Error codes
 *-------------
 */

#define CGBC_ERR_UNKNOWN       0x00     /* unexpected or unknown error       */
#define CGBC_ERR_BAD_COMMAND   0x01     /* command not supported             */
#define CGBC_ERR_OVERFLOW      0x02     /* buffer overflow                   */
#define CGBC_ERR_OUT_OF_SYNC   0x03     /* protocol out of sync              */
#define CGBC_ERR_NO_DATA       0x04     /* no data available                 */
#define CGBC_ERR_BAD_PROTOCOL  0x05     /* protocol not supported            */
#define CGBC_ERR_DEVICE_NAK    0x06     /* device address not acknowledged   */
#define CGBC_ERR_DATA_NAK      0x07     /* data byte not acknowledged        */
#define CGBC_ERR_BAD_INTERRUPT 0x08     /* unexpected or unknown interrupt   */
#define CGBC_ERR_ARBITRATION   0x09     /* device arbitration failed         */
#define CGBC_ERR_DEVICE_BUSY   0x0A     /* device is busy and cannot respond */
#define CGBC_ERR_NOT_SUPPORTED 0x0B     /* feature or function not supported */
#define CGBC_ERR_BAD_PARAMETER 0x0C     /* command parameter invalid         */
#define CGBC_ERR_TIMEOUT       0x0D     /* timeout condition       */ //MOD002
#define CGBC_ERR_ABORT         0x0E     /* command aborted         */ //MOD032
#define CGBC_ERR_BAD_CHECKSUM  0x0F     /* checksum error          */ //MOD040
                                                                    /*MOD044v*/
#define CGBC_ERR_INACTIVE      0x10     /* device or service inactive        */
#define CGBC_ERR_WRITE         0x11     /* write operation failed            */
                                                                    /*MOD044^*/
                                                                    /*MOD027v*/
#define CGBC_ERR_HIF_RESET     0x1E     /* host interface was reset          */
#define CGBC_ERR_HIF_DOWN      0x1F     /* host interface is down            */
                                                                    /*MOD027^*/

                                                                     //MOD014v
/*-------------------------------
 * Info returned from GET_FW_REV
 *-------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_FW_REV_STRUCT
   {
    unsigned char feat;                         /* firmware feature number  */
    unsigned char majv;                         /* major revision number    */
    unsigned char minv;                         /* minor revision number    */
    unsigned char cid;                          /* compatibility ID         */
   } CGBC_FW_REV, *P_CGBC_FW_REV;

typedef struct CGBC_CMD_FW_REV_OUT_STRUCT
   {
    unsigned char sts;                          /* command status           */
    CGBC_FW_REV   rev;                          /* revision information     */
   } CGBC_CMD_FW_REV_OUT, *P_CGBC_CMD_FW_REV_OUT;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^
                                                                     
                                                                     
/*------------------------------------
 * Info returned from CGBC_CMD_INFO_1
 *------------------------------------
 */

#ifndef __ASSEMBLER__                                                //MOD013
                                                                     //MOD002v
typedef struct CGBC_INFO_1_STRUCT
   {
    unsigned char cgbcType;          /* congatec board controller type      */
    unsigned char wdInfo;            /* watchdog information                */
    unsigned char cgbcFeatures;      /* supported features                  */
    unsigned char i2cFreq;           /* S/F-mode I2C bus frequency          */
    unsigned char smbFreq;           /* ASM-bus frequency                   */
    unsigned char ddcFreq;           /* EPI-bus frequency                   */
    unsigned char sdaEepromInfo;     /* secure data area EEPROM information */
    unsigned char cgbcEepromInfo;    /* CGBC parameter EEPROM information   */
    unsigned char userEepromInfo;    /* user data EEPROM information        */
    unsigned char biosEepromInfo;    /* BIOS parameter EEPROM information   */
    unsigned char cgbcRamInfo;       /* CGBC parameter RAM information      */
    unsigned char userRamInfo;       /* user data RAM information           */
    unsigned char biosRamInfo;       /* BIOS parameter RAM information      */
    unsigned char reserved;                                          //MOD014
   } CGBC_INFO_1, *P_CGBC_INFO_1;
                                                                     //MOD002^
                                                                     //MOD014v
typedef struct CGBC_CMD_INFO_1_OUT_STRUCT
   {
    unsigned char sts;                          /* command status           */
    CGBC_INFO_1   info1;                        /* info 1 structure         */
   } CGBC_CMD_INFO_1_OUT, *P_CGBC_CMD_INFO_1_OUT;
                                                                     //MOD014^
#endif /*!__ASSEMBLER__*/                                            //MOD013

/* congatec board controller types */

#define CGBC_ATMEGA48      1
#define CGBC_ATMEGA88      2
#define CGBC_ATMEGA168     3
#define CGBC_ATMEGA165P    4                                         //MOD014
#define CGBC_ATMEGA325P    5                                         //MOD015
#define CGBC_STM32F100R8   6                                         //MOD018
#define CGBC_TM4E1231H6ZRB 7                                         //MOD030
#define CGBC_MEC170x       8                                  //MOD042 MOD062

/* watchdog information */

#define CGBC_WD_TYPE_MSK           0x0F    /* mask for controller type field */
 #define CGBC_WD_TYPE_NONE          0
 #define CGBC_WD_TYPE_STANDARD      1

#define CGBC_WD_EVENT_SUPPORT_MSK  0xF0    /* mask for supported watchdog events */
 #define CGBC_WD_INT_SUPPORT        0x10    /* NMI/IRQ supported */
 #define CGBC_WD_SCI_SUPPORT        0x20    /* SMI/SCI supported */
 #define CGBC_WD_RST_SUPPORT        0x40    /* system reset supported */
 #define CGBC_WD_BTN_SUPPORT        0x80    /* power button supported */

/* supported features */

#define CGBC_BACKLIGHT_SUPPORT     0x01    /* LCD backlight control supported */
#define CGBC_LFP_BACKLIGHT_SUPPORT 0x01    /* LFP backlight control */ //MOD040
#define CGBC_LCD_POWER_SUPPORT     0x02    /* LCD power control supported */
#define CGBC_LFP_POWER_SUPPORT     0x02    /* LFP power control */     //MOD040
#define CGBC_BOOT_COUNTER_SUPPORT  0x04    /* boot counter supported */
#define CGBC_RUNNIG_TIME_SUPPORT   0x08    /* running time meter supported */
#define CGBC_BOOT_TIME_SUPPORT     0x10    /* boot time meter supported */
#define CGBC_AUX_SUPPORT           0x20    /* aux. serial port supported */

/* I2C bus frequency */

#define CGBC_I2C_FREQ_VALUE_MSK    0x3F    /* mask for I2C bus frequency value */
#define CGBC_I2C_FREQ_UNIT_MASK    0xC0    /* mask for I2C bus frequency unit */
 #define CGBC_I2C_FREQ_UNIT_100HZ   0x00    /* unit of frequency value is 100Hz */
 #define CGBC_I2C_FREQ_UNIT_1KHZ    0x40    /* unit of frequency value is 1kHz */
 #define CGBC_I2C_FREQ_UNIT_10KHZ   0x80    /* unit of frequency value is 10kHz */
 #define CGBC_I2C_FREQ_UNIT_100KHZ  0xC0    /* unit of frequency value is 100kHz */

/* I2C device information */

#define CGBC_I2C_DEV_SIZE_MASK     0x1F    /* I2C device size coded in power of 2 */
#define CGBC_I2C_DEV_TYPE_MASK     0xE0    /* mask for I2C device type */
 #define CGBC_I2C_DEV_TYPE_EEP8     0x00    /* I2C EEPROM with 8-bit index */
 #define CGBC_I2C_DEV_TYPE_EEP16    0x20    /* I2C EEPROM with 16-bit index */
 #define CGBC_I2C_DEV_TYPE_RAM8     0x40    /* I2C RAM with 8-bit index */
 #define CGBC_I2C_DEV_TYPE_RAM16    0x60    /* I2C RAM with 16-bit index */


                                                                     //MOD014v
/*---------------------------------------
 * Info returned from CGBC_CMD_BOOT_TIME
 *---------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_BOOT_TIME_OUT_STRUCT
   {
    unsigned char  sts;                    /* command status                */
    uint16_t       systemUpTime;           /* system up time in 250ms units */ //MOD070
    uint16_t       lastBootTime;           /* last boot time in 250ms units */ //MOD070
   } CGBC_CMD_BOOT_TIME_OUT, *P_CGBC_CMD_BOOT_TIME_OUT;
                                                                     //MOD071v
typedef struct CGBC_CMD_BOOT_TIME_OUT_32_STRUCT
   {
    unsigned char  sts;                      /* command status              */
    uint32_t       systemUpTime;             /* system up time in 1ms units */
    uint32_t       lastBootTime;             /* last boot time in 1ms units */
   } CGBC_CMD_BOOT_TIME_OUT_32, *P_CGBC_CMD_BOOT_TIME_OUT_32;
                                                                     //MOD071^
#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^
                                                                     

/*----------------
 * System flags 1
 *----------------
 */
                                                                     //MOD021v
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_SYS_FLAGS_1_IN_STRUCT   // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_SYS_FLAGS_1
    unsigned char andMask;                      // system flags 1 AND mask
    unsigned char orMask;                       // system flags 1 OR mask
   } CGBC_CMD_SYS_FLAGS_1_IN, *P_CGBC_CMD_SYS_FLAGS_1_IN;

typedef struct CGBC_CMD_SYS_FLAGS_1_OUT_STRUCT  // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char flags;                        // current system flags 1
   } CGBC_CMD_SYS_FLAGS_1_OUT, *P_CGBC_CMD_SYS_FLAGS_1_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD021^
                                                                     //MOD044v
#define CGBC_SYS_S5E_ENABLE_MASK    0xE0  /* S5e enabled when non-zero */
#define CGBC_SYS_S5E_SLPBTNn_WAKE   0x80  /* 1 - S5e enabled, SLEEP# wake */
#define CGBC_SYS_S5E_SMBALERTn_WAKE 0x40  /* 1 - S5e enabled, SMBALERT# wake */
#define CGBC_SYS_S5E_PWRBTNn_WAKE   0x20  /* 1 - S5e enabled, PWRBTN# wake */
#define CGBC_SYS_S5E_SUPPORT        0x10  /* 1 - S5e supported */
                                                                     //MOD044^
#define CGBC_SYS_PWR_AT             0x08  /* 1 = AT power supply */  //MOD003
#define CGBC_SYS_POST_END           0x04  /* 1 = POST complete */
#define CGBC_SYS_POST_START         0x02  /* 1 = system has started POST */
#define CGBC_SYS_PWR_ON             0x01  /* 1 = system power is on */



/*---------------------------------------------------------------------------
 * CGBC_CMD_VID_CONTROL 
 *
 * Input:  bits 7-6 - reserved
 *         bit  5   - 1 = enable LCD power control
 *         bit  4   - 1 = enable LCD backlight control
 *         bits 3-2 - reserved
 *         bit  1   - new LCD power state if bit 5 = 1 (1=on, 0=off)
 *         bit  0   - new LCD backlight state if bit 4 = 1 (1=on, 0=off)
 *
 * Output: bits 7-2 - reserved
 *         bit  1   - current LCD power state (1=on, 0=off)
 *         bit  0   - current LCD backlight state (1=on, 0=off)
 */
                                                                     //MOD021v
#ifndef __ASSEMBLER__
                                                                     //MOD040v
typedef struct CGBC_CMD_LFP_CONTROL_IN_STRUCT   // cmd packet struct
   {
    unsigned char cmd;                          // CGBC_CMD_LFP_CONTROL
    unsigned char parm;                         // video control parameter
   } CGBC_CMD_LFP_CONTROL_IN, *P_CGBC_CMD_LFP_CONTROL_IN;

typedef struct CGBC_CMD_LFP_CONTROL_OUT_STRUCT  // response pkt struct
   {
    unsigned char sts;                          // command status
    unsigned char flags;                        // current video ctrl flags
   } CGBC_CMD_LFP_CONTROL_OUT, *P_CGBC_CMD_LFP_CONTROL_OUT;

typedef struct CGBC_CMD_VID_CONTROL_IN_STRUCT   // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_VID_CONTROL
    unsigned char parm;                         // video control parameter
   } CGBC_CMD_VID_CONTROL_IN, *P_CGBC_CMD_VID_CONTROL_IN; /* Deprecated */

typedef struct CGBC_CMD_VID_CONTROL_OUT_STRUCT  // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char flags;                        // current video ctrl flags
   } CGBC_CMD_VID_CONTROL_OUT, *P_CGBC_CMD_VID_CONTROL_OUT; /* Deprecated */
                                                                     //MOD040^
#endif //!__ASSEMBLER__
                                                                     //MOD021^
                                                                     //MOD040v
/* Local flat panel control register */
#define CGBC_LFP_VDD_INP_FORCED  0x80
#define CGBC_LFP_BLT_INP_FORCED  0x40
#define CGBC_LFP_VDD_CTL_UPDATE  0x20
#define CGBC_LFP_BLT_CTL_UPDATE  0x10
#define CGBC_LFP_VDD_INP_STATUS  0x08
#define CGBC_LFP_VDD_INP_ON      0x08
#define CGBC_LFP_VDD_INP_OFF     0x00
#define CGBC_LFP_BLT_INP_STATUS  0x04
#define CGBC_LFP_BLT_INP_ON      0x04
#define CGBC_LFP_BLT_INP_OFF     0x00
#define CGBC_LFP_VDD_GATE_STATUS 0x02
#define CGBC_LFP_VDD_GATE_OPEN   0x02
#define CGBC_LFP_VDD_GATE_CLOSED 0x00
#define CGBC_LCD_POWER_ON        0x02
#define CGBC_LFP_BLT_GATE_STATUS 0x01
#define CGBC_LFP_BLT_GATE_OPEN   0x01
#define CGBC_LFP_BLT_GATE_CLOSED 0x00
#define CGBC_LCD_BACKLIGHT_ON    0x01

/* Deprecated definitions */
#define CGBC_LCD_BACKLIGHT_OFF  0x00
#define CGBC_LCD_POWER_OFF      0x00
#define CGBC_SET_LCD_BACKLIGHT  0x10
#define CGBC_SET_LCD_POWER      0x20
                                                                     //MOD040^
                                                                     //MOD002v
/*-----------------
 * I2C bus numbers
 *-----------------
 */

#define CGBC_I2C_BUS_MSK      0x07  /* bus mask for CGBC commands           */
#define CGBC_I2C_BUS_I2C0     0x00  /* primary I2C-bus 0                    */ //MOD044
#define CGBC_I2C_BUS_EXTERNAL 0x00  /* physical 400kHz multi-master I2C bus */
                                                                               //MOD052 MOD059
#define CGBC_I2C_BUS_SMB1     0x01  /* secondary SMBus                      */ //MOD033
#define CGBC_I2C_BUS_SMB0     0x02  /* primary SMBus                        */ //MOD033
#define CGBC_I2C_BUS_SMB      0x02  /* system management bus                */
#define CGBC_I2C_BUS_ASM      0x02  /* auxiliary system management bus      */ //MOD004
#define CGBC_I2C_BUS_I2CV     0x03  /* virtual I2C-bus                      */ //MOD044
#define CGBC_I2C_BUS_INTERNAL 0x03  /* virtual I2C bus for internal devices */
#define CGBC_I2C_BUS_DDC0     0x04  /* display data channel 0               */ //MOD044
#define CGBC_I2C_BUS_DDC      0x04  /* video DDC bus                        */
#define CGBC_I2C_BUS_EPI      0x04  /* embedded panel interface bus         */ //MOD004
#define CGBC_I2C_BUS_EOOB     0x05  /* eSPI OOB channel                     */ //MOD046
#define CGBC_I2C_BUS_IPMB     0x06  /* intelligent platform management bus  */ //MOD052
                                                                               //MOD052 MOD059
#define CGBC_I2C_BUS_SMB2     0x07  /* auxiliary SMBus                      */ //MOD033
#define CGBC_I2C_BUS_I2C1     0x08  /* secondary I2C-bus 1                  */ //MOD059
#define CGBC_I2C_BUS_UPDB     0x09  /* USB PD controller bus                */ //MOD059
#define CGBC_I2C_BUS_DDR0     0x0A  /* dedicated DDR SMBus 0                */ //MOD065
#define CGBC_I2C_BUS_DDR1     0x0B  /* dedicated DDR SMBus 1                */ //MOD065
#define CGBC_I2C_BUS_MAX      CGBC_I2C_BUS_DDR1                                //MOD065
                                                                     //MOD002^


/*--------------------------------------
 * I2C_START/I2C_COMBINED command flags
 *--------------------------------------
 */
                                                                     //MOD014v
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_I2C_START_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char wrCnt;
                   unsigned char rdCnt;
                   unsigned char i2cData[35];
                  } CGBC_CMD_I2C_START_IN,
                 *P_CGBC_CMD_I2C_START_IN;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^
#define CGBC_I2C_SEND_START     0x80    /* send START condition */
#define CGBC_I2C_SEND_STOP      0x40    /* send STOP condition */
#define CGBC_I2C_SEND_LAST_ACK  0x80    /* send ACK on last read byte */
#define CGBC_I2C_SEND_ALL_NAK   0x40    /* send NAK on all read bytes */
#define CGBC_I2C_CNT_MSK        0x3F    /* write/read count mask */  //MOD014
                                                                     //MOD044v
/* I2C_COMBINED flag for SMBus block protocol in the device address field
 * (i2cData[0]) */
#define CGBC_I2C_SMBUS_BLK     0x01
                                                                     //MOD044^


/*-----------------------
 * I2C_STAT return codes
 *-----------------------
 */

#define CGBC_I2C_STAT_IDL       0x00    /* I2C bus is idle. */
#define CGBC_I2C_STAT_DAT       0x01    /* I2C bus controller data pending. */
#define CGBC_I2C_STAT_BSY       0x02    /* I2C bus is busy. */


                                                                     //MOD014v
/*---------------------------
 * I2C_DATA return structure
 *---------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_I2C_DATA_OUT_STRUCT
                  {
                   unsigned char sts;
                   unsigned char i2cData[32];
                  } CGBC_CMD_I2C_DATA_OUT, *P_CGBC_CMD_I2C_DATA_OUT;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^

                                                                     //MOD021v
/*------------------------------------
 * I2C_SPEED command/response packets
 *------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_I2C_SPEED_IN_STRUCT
   {
    unsigned char cmd;                          // command code
    unsigned char speed;                        // desired I2C-bus frequency
   } CGBC_CMD_I2C_SPEED_IN, *P_CGBC_CMD_I2C_SPEED_IN;

typedef struct CGBC_CMD_I2C_SPEED_OUT_STRUCT
   {
    unsigned char sts;                          // command status
    unsigned char speed;                        // current I2C-bus frequency
   } CGBC_CMD_I2C_SPEED_OUT, *P_CGBC_CMD_I2C_SPEED_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD021^

                                                                     //MOD059v
/* -----------------------------------------------------------------------
 * CGBC_CMD_I2C_START_EXT/CGBC_CMD_I2C_COMBINED_EXT command definitions
 * -------------------------------------------------------------------- */

typedef struct CGBC_CMD_I2C_START_EXT_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char bus;
                   unsigned char wrCnt;
                   unsigned char rdCnt;
                   unsigned char i2cData[35];
                  } CGBC_CMD_I2C_START_EXT_IN,
                 *P_CGBC_CMD_I2C_START_EXT_IN;

/* --------------------------------------------
 * CGBC_CMD_I2C_STAT_EXT command definitions
 * ----------------------------------------- */

typedef struct CGBC_CMD_I2C_STAT_EXT_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char bus;
                  } CGBC_CMD_I2C_STAT_EXT_IN,
                 *P_CGBC_CMD_I2C_STAT_EXT_IN;

/* --------------------------------------------
 * CGBC_CMD_I2C_DATA_EXT command definitions
 * ----------------------------------------- */

typedef struct CGBC_CMD_I2C_DATA_EXT_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char bus;
                  } CGBC_CMD_I2C_DATA_EXT_IN,
                 *P_CGBC_CMD_I2C_DATA_EXT_IN;

/* -----------------------------------------
 * CGBC_CMD_I2C_SPEED_EXT command definitions
 * -------------------------------------- */

typedef struct CGBC_CMD_I2C_SPEED_EXT_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char bus;
                   unsigned char speed;
                  } CGBC_CMD_I2C_SPEED_EXT_IN,
                 *P_CGBC_CMD_I2C_SPEED_EXT_IN;
                                                                     //MOD059^


/*--------------------
 * I2C device numbers
 *--------------------
 */

#define CG_SDA_EEP_I2C_ADDR     0x80    /* secure data area EEPROM */
#define CG_BC_EEP_I2C_ADDR      0x90    /* board controller parameter EEPROM */
#define CG_USER_EEP_I2C_ADDR    0xA0    /* user data EEPROM */
#define CG_BIOS_EEP_I2C_ADDR    0xB0    /* BIOS control parameter EEPROM */
#define CG_BC_RAM_I2C_ADDR      0xC0    /* board controller parameter RAM */
#define CG_USER_RAM_I2C_ADDR    0xD0    /* user scratch RAM */
#define CG_BIOS_RAM_I2C_ADDR    0xE0    /* BIOS scratch RAM */


                                                                     //MOD002v
/*-------------------------------------------------
 * Layout of the board controller parameter EEPROM
 *-------------------------------------------------
 */

#ifndef __ASSEMBLER__                                                //MOD013

typedef struct CG_BC_EEP_STRUCT
   {
    unsigned char  i2cExtSpeed;       /* I2C-bus power-up default frequency */
    uint16_t       i2cExtTimeout;     /* I2C-bus power-up default timeout   */ //MOD070
    unsigned char  i2cSmbSpeed;       /* ASM-bus power-up default frequency */
    uint16_t       i2cSmbTimeout;     /* ASM-bus power-up default timeout   */ //MOD070
    unsigned char  i2cDdcSpeed;       /* EPI-bus power-up default frequency */
    uint16_t       i2cDdcTimeout;     /* EPI-bus power-up default timeout   */ //MOD070
    unsigned char  s5eDelay250;       /* default S5e delay       */  //MOD044
                                                                     //MOD004v
    unsigned char  vidControl;        /* video control power-up default     */
    unsigned char  vidResetDelay;     /* video signal suppression time      */
                                                                     //MOD004^
    unsigned char  pwrOffDelay;       /* LastStateOff saving dly */  //MOD003
    unsigned char  featByte1;         /* feature byte 1          */  //MOD012
    unsigned char  lf2Control;        /* LF2 ctrl at pwr-up dflt */  //MOD041
    unsigned char  lf2ResetDelay;     /* LF2 signal suppr. time  */  //MOD041
   } CG_BC_EEP, *P_CG_BC_EEP;
                                                                     //MOD002^
#endif /*!__ASSEMBLER__*/                                            //MOD013


                                                                     //MOD014v
/*----------------------------------------------
 * Layout of the board controller parameter RAM
 *----------------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CG_BC_RAM_STRUCT
   {
    unsigned char  reserved[4];                         /* reserved         */
    unsigned char  unused;                                           //MOD044
                                                                     //MOD017v
    unsigned char  flagsByte1;                          /* flags byte 1     */
    unsigned char  padding[2];
                                                                     //MOD017^
   } CG_BC_RAM, *P_CG_BC_RAM;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^
                                                                     //MOD017v
/* CG_BC_RAM.flagsByte1 */

#define CGBC_COLD_RESET_REQUEST      0x10                            //MOD040
#define CGBC_RSMRST_REQUEST          0x20                            //MOD040
#define CGBC_HARD_OFF_INHIBIT        0x40
#define CGBC_BOOTBLOCK_UPDATE_ENABLE 0x80
                                                                     //MOD017^

                                                                     //MOD012v
/*----------------
 * Feature byte 1
 *----------------
 */

#define CGBC_FORCE_SBSM_EMULATION 0x01          /* 1 = force SBSM emulation */
                                                                     //MOD012^
#define CGBC_HOT_S5_ENABLE        0x02          // 1 = hot S5 enabled//MOD019
#define CGBC_FORCE_BAT_ON_SMBUS   0x04          // 1 = bat on SMBus  //MOD040
#define CGBC_MPP_FLAG             0x08          // multi purpose     //MOD040
                                                // power flag        //MOD040
                                                                     //MOD017v
#define CGBC_RST_BTN_INHIBIT      0x10          // 1 = reset button disabled
#define CGBC_LID_BTN_INHIBIT      0x20          // 1 = LID button disabled
#define CGBC_SLP_BTN_INHIBIT      0x40          // 1 = sleep button disabled
#define CGBC_PWR_BTN_INHIBIT      0x80          // 1 = power button disabled
                                                                     //MOD017^


/*------------------------------------
 * CGBC_CMD_WD_INIT command structure
 *------------------------------------
 */

#ifndef __ASSEMBLER__                                                //MOD013
                                                                     //MOD002v
typedef struct CGBC_WD_PARMS_STRUCT
   {
    unsigned char  wdInitCmd;                /* CGBC_CMD_WD_INIT             */
    unsigned char  wdMode;                   /* watchdog mode                */
    unsigned char  wdCtrl;                   /* watchdog control byte        */
    uint16_t       wdSt1TimL;                /* watchdog stage 1 time (msec) */ //MOD008 MOD070
    unsigned char  wdSt1TimH;
    uint16_t       wdSt2TimL;                /* watchdog stage 2 time (msec) */ //MOD008 MOD070
    unsigned char  wdSt2TimH;
    uint16_t       wdSt3TimL;                /* watchdog stage 3 time (msec) */ //MOD008 MOD070
    unsigned char  wdSt3TimH;
    uint16_t       wdDlyTimL;                /* watchdog delay time   (msec) */ //MOD008 MOD070
    unsigned char  wdDlyTimH;
   } CGBC_WD_PARMS, *P_CGBC_WD_PARMS;
                                                                     //MOD002^
#endif /*!__ASSEMBLER__*/                                            //MOD013



/*--------------------
 * Watchdog mode byte
 *--------------------
 */

#define CGBC_WD_DISABLED           0x00    /* watchdog disabled */
#define CGBC_WD_ONETIME_TRIG_MODE  0x01    /* watchdog one-time-trigger mode */
#define CGBC_WD_SINGLE_EVENT_MODE  0x02    /* watchdog single event mode */
#define CGBC_WD_EVENT_REPEAT_MODE  0x03    /* watchdog event repeat mode */
#define CGBC_WD_MODE_MSK           0x03                              //MOD014



/*-----------------
 * Watchdog events
 *-----------------
 */

#define CGBC_WD_EVENT_INT       0x00    /* NMI/IRQ */
#define CGBC_WD_EVENT_SCI       0x01    /* SMI/SCI */
#define CGBC_WD_EVENT_RST       0x02    /* system reset */
#define CGBC_WD_EVENT_BTN       0x03    /* power button */



/*-----------------------
 * Watchdog control byte
 *-----------------------
 */

#define CGBC_WD_STAGES_MSK      0x03
#define CGBC_WD_EVENT1_MSK      0x0C
#define CGBC_WD_EVENT2_MSK      0x30
#define CGBC_WD_EVENT3_MSK      0xC0


                                                                     //MOD014v
/*---------------------------------
 * CGBC_CMD_WD_INIT_POST parameter
 *---------------------------------
 */

#define CGBC_POST_WATCHDOG_EVENT_MSK   0x8000
 #define CGBC_POST_WATCHDOG_EVENT_RST   0x0000
 #define CGBC_POST_WATCHDOG_EVENT_BTN   0x8000

#define CGBC_POST_WATCHDOG_TIMEOUT_MSK 0x7FFF



/*----------------------------------
 * CGBC_CMD_WD_INIT_EARLY parameter
 *----------------------------------
 */

#define CGBC_EARLY_WATCHDOG_EVENT_MSK   0x80
 #define CGBC_EARLY_WATCHDOG_EVENT_RST   0x00
 #define CGBC_EARLY_WATCHDOG_EVENT_BTN   0x80

#define CGBC_EARLY_WATCHDOG_TIMEOUT_MSK 0x7F
                                                                     //MOD014^

                                                                     //MOD002v
/*---------------------
 * AUX_INIT parameters
 *---------------------
 */
                                                                     //MOD014v
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_AUX_INIT_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char parm;
                  } CGBC_CMD_AUX_INIT_IN,
                 *P_CGBC_CMD_AUX_INIT_IN;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^
#define CGBC_UART_BAUDRATE_MSK 0x07
#define CGBC_UART_BAUDRATE_S   0                                     //MOD040
#define CGBC_UART_DISABLE      0x00
#define CGBC_UART_1200         0x01
#define CGBC_UART_2400         0x02
#define CGBC_UART_4800         0x03
#define CGBC_UART_9600         0x04
#define CGBC_UART_19200        0x05
#define CGBC_UART_38400        0x06
#define CGBC_UART_115200       0x07                                  //MOD040

#define CGBC_UART_DATABITS_MSK 0x18
#define CGBC_UART_DATABITS_S   3                                     //MOD040
#define CGBC_UART_DATA_5       0x00
#define CGBC_UART_DATA_6       0x08
#define CGBC_UART_DATA_7       0x10
#define CGBC_UART_DATA_8       0x18

#define CGBC_UART_STOPBITS_MSK 0x20
#define CGBC_UART_STOPBITS_S   5                                     //MOD040
#define CGBC_UART_STOP_1       0x00
#define CGBC_UART_STOP_2       0x20

#define CGBC_UART_PARITY_MSK   0xC0
#define CGBC_UART_PARITY_S     6                                     //MOD040
#define CGBC_UART_PARITY_NONE  0x00
#define CGBC_UART_PARITY_EVEN  0x80
#define CGBC_UART_PARITY_ODD   0xC0
                                                                     //MOD002^
#define CGBC_AUX_62500_N_6_1   0x48                                  //MOD014



/*-----------------------
 * AUX_STAT return code
 *-----------------------
 */

#define CGBC_UART_IBF   0x01    /* RX data input buffer filled */
#define CGBC_UART_OBF   0x02    /* TX data output buffer full */
#define CGBC_UART_OBE   0x04    /* TX data output buffer empty */
#define CGBC_UART_ERR   0x08    /* transmission error */             //MOD002



/*------------
 * SCI events
 *------------
 */

#define CGBC_SCI_EVT_NONE               0
#define CGBC_SCI_EVT_WATCHDOG           1
#define CGBC_SCI_EVT_TEMPERATURE        2
#define CGBC_SCI_EVT_XINT               3                            //MOD004
#define CGBC_SCI_EVT_BAT_0              4                            //MOD005
#define CGBC_SCI_EVT_BAT_1              5                            //MOD005
#define CGBC_SCI_EVT_BAT_2              6                            //MOD005
#define CGBC_SCI_EVT_BAT_3              7                            //MOD005
#define CGBC_SCI_EVT_LID_OPEN           8                            //MOD017
#define CGBC_SCI_EVT_LID_CLOSE          9                            //MOD017
#define CGBC_SCI_EVT_SLP_BTN            10                           //MOD017
#define CGBC_SCI_EVT_PWR_BTN            11                           //MOD045



/*----------------------------------
 * CGBC_CMD_TEST_COUNTER parameters
 *----------------------------------
 */
                                                                     //MOD021v
#ifndef __ASSEMBLER__

typedef struct CGBC_TCNT_STRUCT
   {
    unsigned char good;
    unsigned char bad;
   } CGBC_TCNT, *P_CGBC_TCNT;

typedef struct CGBC_CMD_TCNT_IN_STRUCT          // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_TEST_COUNTER
    unsigned char parm;                         // test counter parameter
   } CGBC_CMD_TCNT_IN, *P_CGBC_CMD_TCNT_IN;

typedef struct CGBC_CMD_TCNT_OUT_STRUCT         // response packet structure
   {
    unsigned char sts;                          // command status
    CGBC_TCNT     tcnt;                         // test counters
   } CGBC_CMD_TCNT_OUT, *P_CGBC_CMD_TCNT_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD021^
#define CGBC_TEST_PASSED        0x55
#define CGBC_TEST_FAILED        0xAA

#define CGBC_LAST_FAILED        0x80


                                                                     //MOD002v
/*--------------------------
 * CGBC_CMD_DUMP parameters
 *--------------------------
 */
                                                                     //MOD014v
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_DUMP_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char typeCnt;
                   unsigned char addrLow;
                   unsigned char addrMid;
                   unsigned char addrHigh;
                  } CGBC_CMD_DUMP_IN, *P_CGBC_CMD_DUMP_IN;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^
#define CGBC_DUMP_COUNT_MSK   0xF8
#define CGBC_MEM_TYPE_MSK     0x07
#define CGBC_MEM_TYPE_FLASH   0x00
#define CGBC_MEM_TYPE_EEPROM  0x01
#define CGBC_MEM_TYPE_RAM     0x02


                                                                     //MOD014v
/*---------------------------------------
 * CGBC_CMD_RESET_INFO return parameters
 *---------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_RESET_INFO_STRUCT
   {
    unsigned char latestReset;
    unsigned char externalResetCounter;
    unsigned char brownoutResetCounter;
    unsigned char watchdogResetCounter;
    unsigned char softwareResetCounter;
   } CGBC_RESET_INFO, *P_CGBC_RESET_INFO;
                                                                     //MOD021v
typedef struct CGBC_CMD_RESET_INFO_OUT_STRUCT
   {
    unsigned char   sts;                        // command status
    CGBC_RESET_INFO info;                       // reset information
   } CGBC_CMD_RESET_INFO_OUT, *P_CGBC_CMD_RESET_INFO_OUT;
                                                                     //MOD021^
#endif /*!__ASSEMBLER__*/

/* bit declarations within latestResetSource */
                                                                     //MOD014^
#define CGBC_RESET_SWR  0x10
#define CGBC_RESET_WDG  0x08
#define CGBC_RESET_BOD  0x04
#define CGBC_RESET_EXT  0x02
#define CGBC_RESET_POR  0x01
                                                                     //MOD002^
                                                                     //MOD040v
/*-------------------------------
 * CGBC_CMD_POST_CODE parameters
 *-------------------------------
 */
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_POST_CODE_IN_STRUCT     // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_POST_CODE
    unsigned char postCode;                     // POST code
   } CGBC_CMD_POST_CODE_IN, *P_CGBC_CMD_POST_CODE_IN;

#endif //!__ASSEMBLER__
                                                                     //MOD040^

                                                                     //MOD003v
/*--------------------------------
 * CGBC_CMD_POWER_LOSS parameters
 *--------------------------------
 */

#define CGBC_POWER_LOSS_MSK   0x03
#define  CGBC_POWER_LOSS_OFF   0x00
#define  CGBC_POWER_LOSS_ON    0x01
#define  CGBC_POWER_LOSS_LAST  0x02
                                                                     //MOD003^
#define CGBC_RTC_PRESENT      0x80                                   //MOD011
                                                                     //MOD044v
#define CGBC_BATLOWn_INIT_MSK 0x80
#define  CGBC_BATLOWn_INIT_LOW 0x00
#define  CGBC_BATLOWn_INIT_TRI 0x80
                                                                     //MOD044^

                                                                     //MOD004v
/*----------------------------------
 * CGBC_CMD_XINT_CONTROL parameters
 *----------------------------------
 */
                                                                     //MOD021v
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_XINT_CONTROL_IN_STRUCT  // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_XINT_CONTROL
    unsigned char parm;                         // control parameter
   } CGBC_CMD_XINT_CONTROL_IN, *P_CGBC_CMD_XINT_CONTROL_IN;

typedef struct CGBC_CMD_XINT_CONTROL_OUT_STRUCT // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char parm;                         // current control parameter
   } CGBC_CMD_XINT_CONTROL_OUT, *P_CGBC_CMD_XINT_CONTROL_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD021^
#define  CGBC_XINT_GET_STATUS 0x00           
#define  CGBC_XINT_SET_MODE   0x01           // 1 = set parms / 0 = get status
#define  CGBC_XINT_NMI        0x02           // Generate IRQ/NMI on EXT_INT.
#define  CGBC_XINT_SCI        0x04           // Generate SCI/SMI on EXT_INT.
#define  CGBC_XINT_POLLING    0x08           // EXT_INT polling mode.
#define  CGBC_XINT_MODE_MSK   (CGBC_XINT_NMI|CGBC_XINT_SCI|CGBC_XINT_POLLING)
#define  CGBC_XINT_ACTIVE     0x10           // 1 = EXT_INT was active.



/*----------------------------------
 * CGBC_CMD_USER_LOCK return status
 *----------------------------------
 */
                                                                     //MOD021v
#define CGBC_USER_LOCK_KEY_SIZE 6

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_USER_LOCK_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char key[CGBC_USER_LOCK_KEY_SIZE];
                  } CGBC_CMD_USER_LOCK_IN, *P_CGBC_CMD_USER_LOCK_IN;

#endif //!__ASSEMBLER__
                                                                     //MOD021^
#define  CGBC_LOCKED    0x01                // User lock is closed.
#define  CGBC_TAMPERED  0x02                // Someone tampered with the lock.
                                                                     //MOD004^

                                                                     //MOD017v
/*---------------------------------------------------------------------------
 * CGBC_CMD_BTN_CONTROL 
 *
 * Input:  bit  7   - power button inhibit
 *         bit  6   - sleep button inhibit
 *         bit  5   -   LID button inhibit
 *         bit  4   - reset button inhibit
 *         bits 3-0 - reserved
 *
 * Output: bit  7   - 1 = power button disabled / 0 = power button enabled
 *         bit  6   - 1 = sleep button disabled / 0 = sleep button enabled
 *         bit  5   - 1 = LID button disabled   / 0 = LID button enabled
 *         bit  4   - 1 = reset button disabled / 0 = reset button enabled
 *         bit  3   - current power button state (1=inactive, 0=active)
 *         bit  2   - current sleep button state (1=inactive, 0=active)
 *         bit  1   - current LID state          (1=open,     0=closed)
 *         bit  0   - current reset button state (1=inactive, 0=active)
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_BTN_CONTROL_IN_STRUCT   // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_BTN_CONTROL
    unsigned char parm;                         // button control parameter
   } CGBC_CMD_BTN_CONTROL_IN, *P_CGBC_CMD_BTN_CONTROL_IN;
                                                                      //MOD044v
#endif //!__ASSEMBLER__

/* button control parameter bit mapping */
#define CGBC_RST_BTN_DESELECT 0x01   /* reset button control update disabled */
#define CGBC_LID_BTN_DESELECT 0x02   /*   LID button control update disabled */
#define CGBC_SLP_BTN_DESELECT 0x04   /* sleep button control update disabled */
#define CGBC_PWR_BTN_DESELECT 0x08   /* power button control update disabled */
#if 0                                           /* same as in feature byte 1 */
#define CGBC_RST_BTN_INHIBIT  0x10              /* 1 = reset button disabled */
#define CGBC_LID_BTN_INHIBIT  0x20              /* 1 = LID button disabled   */
#define CGBC_SLP_BTN_INHIBIT  0x40              /* 1 = sleep button disabled */
#define CGBC_PWR_BTN_INHIBIT  0x80              /* 1 = power button disabled */
#endif

#ifndef __ASSEMBLER__
                                                                      //MOD044^
typedef struct CGBC_CMD_BTN_CONTROL_OUT_STRUCT  // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char flags;                        // current button ctrl flags
   } CGBC_CMD_BTN_CONTROL_OUT, *P_CGBC_CMD_BTN_CONTROL_OUT;

#endif //!__ASSEMBLER__

/*
 * Button control bit mapping
 */
#define CGBC_RST_BTN_INACTIVE 0x01              // 0 = reset button pressed
#define CGBC_LID_OPEN         0x02              // 0 = LID closed
#define CGBC_SLP_BTN_INACTIVE 0x04              // 0 = sleep button pressed
#define CGBC_PWR_BTN_INACTIVE 0x08              // 0 = power button pressed
#if 0                                           // same as in feature byte 1
#define CGBC_RST_BTN_INHIBIT  0x10              // 1 = reset button disabled
#define CGBC_LID_BTN_INHIBIT  0x20              // 1 = LID button disabled
#define CGBC_SLP_BTN_INHIBIT  0x40              // 1 = sleep button disabled
#define CGBC_PWR_BTN_INHIBIT  0x80              // 1 = power button disabled
#endif


                                                                     //MOD020v
/*-----------------------------------------------
 * CGBC_CMD_FAN_CONTROL command/response packets
 *-----------------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_FAN_CONTROL_IN_STRUCT   // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_FAN_CONTROL
    unsigned char fanCmd;                       // fan command
   } CGBC_CMD_FAN_CONTROL_IN, *P_CGBC_CMD_FAN_CONTROL_IN;

typedef struct CGBC_CMD_FAN_CONTROL_OUT_STRUCT  // response packet structure
   {
    unsigned char  sts;                         // command status
    uint16_t       speed;                       // current fan speed count MOD070
   } CGBC_CMD_FAN_CONTROL_OUT, *P_CGBC_CMD_FAN_CONTROL_OUT;

#endif //!__ASSEMBLER__

/*
 * Fan commands
 */
#define CGBC_FAN_SET_PWM      0x00 // - 0x64    // set PWM on/off ratio 0-100
#define CGBC_FAN_ON           0x65              // turn fan on
#define CGBC_FAN_OFF          0x66              // turn fan off
                                                                     //MOD040v
#define CGBC_FAN_SET_FREQ_HI  0x80 // - 0xBF    // set high PWM frequency
#define CGBC_FAN_FREQ_HI_MASK 0x3F
#define CGBC_FAN_FREQ_HI_MIN  0x01
#define CGBC_FAN_FREQ_1kHz    0x01
#define CGBC_FAN_FREQ_63kHz   0x3F
#define CGBC_FAN_FREQ_HI_MAX  0x3F
#define CGBC_FAN_SET_FREQ_LO  0xC0 // - 0xC7    // set low PWM frequency
#define CGBC_FAN_FREQ_LO_MASK 0x07
#define CGBC_FAN_FREQ_LO_MIN  0x00
#define CGBC_FAN_FREQ_11p0Hz  0x00
#define CGBC_FAN_FREQ_14p7Hz  0x01
#define CGBC_FAN_FREQ_22p1Hz  0x02
#define CGBC_FAN_FREQ_29p4Hz  0x03
#define CGBC_FAN_FREQ_35p3Hz  0x04
#define CGBC_FAN_FREQ_44p1Hz  0x05
#define CGBC_FAN_FREQ_58p8Hz  0x06
#define CGBC_FAN_FREQ_88p2Hz  0x07
#define CGBC_FAN_FREQ_LO_MAX  0x07
                                                                     //MOD040^
#define CGBC_FAN_GET_SPEED    0xFF              // get fan speed count
                                                                     //MOD020^

                                                                     //MOD022v
/*---------------------------------------
 * CGBC_CMD_RTC command/response packets
 *---------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_RTC_IN_STRUCT           // command packet structure
   {
    unsigned char  cmd;                         // CGBC_CMD_RTC
    uint16_t       year;                        // year to be set or 0 MOD070
    unsigned char  month;                       // month to be set
    unsigned char  day;                         // day to be set
    unsigned char  hour;                        // hour to be set
    unsigned char  min;                         // minute to be set
    unsigned char  sec;                         // second to be set
   } CGBC_CMD_RTC_IN, *P_CGBC_CMD_RTC_IN;

typedef struct CGBC_CMD_RTC_OUT_STRUCT          // response packet structure
   {
    unsigned char  sts;                         // command status
    uint16_t       yearSts;                     // current year and flags MOD070
    unsigned char  month;                       // current month
    unsigned char  day;                         // current day
    unsigned char  hour;                        // current hour
    unsigned char  min;                         // current minute
    unsigned char  sec;                         // current second
   } CGBC_CMD_RTC_OUT, *P_CGBC_CMD_RTC_OUT;

#endif //!__ASSEMBLER__

/*
 * RTC year and status flags
 */
#define CGBC_RTC_LOST_POWER   0x8000            // RTC lost power flag
#define CGBC_RTC_G3           0x4000            // first RTC read since G3
#define CGBC_RTC_YEAR_MSK     0x0FFF            // RTC year date
                                                                     //MOD022^


/*------------------------------------------
 * CGBC_CMD_GPIO_x command/response packets
 *------------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_GPIO_IN_STRUCT
   {
    unsigned char cmd;                          // command code
    unsigned char adr;                          // GPIO byte address
    unsigned char dat;                          // data/direction byte
   } CGBC_CMD_GPIO_IN, *P_CGBC_CMD_GPIO_IN;

typedef struct CGBC_CMD_GPIO_OUT_STRUCT
   {
    unsigned char sts;                          // command status
    unsigned char dat;                          // data/direction byte
   } CGBC_CMD_GPIO_OUT, *P_CGBC_CMD_GPIO_OUT;

#endif //!__ASSEMBLER__



/*-----------------------------------
 * CGBC_CMD_CPU_TEMP response packet
 *-----------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_CPU_TEMP_OUT_STRUCT
   {
    unsigned char sts;                          // command status
      signed char temp;                         // latest CPU temperature
   } CGBC_CMD_CPU_TEMP_OUT, *P_CGBC_CMD_CPU_TEMP_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD017^


/*-------------------------------
 * CGBC_CMD_SBSM_DATA parameters
 *-------------------------------
 */

#define  CGBC_SBSM_BAT_MSK 0x03
#define  CGBC_SBSM_BAT_0   0
#define  CGBC_SBSM_BAT_1   1
#define  CGBC_SBSM_BAT_2   2
#define  CGBC_SBSM_BAT_3   3
                                                                     //MOD007
#define  CGBC_BAT_PRESENT      0x01                                  //MOD006
#define  CGBC_BAT_CHARGING     0x02
#define  CGBC_BAT_DISCHARGING  0x04                                  //MOD007
#define  CGBC_BAT_ON_SMBUS     0x08                                  //MOD034
#define  CGBC_SBSM_REDIRECTION 0x10                                  //MOD017
#define  CGBC_SBSM_EMULATION   0x20                                  //MOD012
#define  CGBC_AC_ONLINE        0x40                           //MOD006 MOD007
#define  CGBC_SBSM_PRESENT     0x80                                  //MOD007
                                                                     //MOD005^
                                                                     //MOD021v
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_SBSM_DATA_OUT_STRUCT
   {
    unsigned char sts;                          // command status
    unsigned char pwrSts;                       // AC and battery status
   } CGBC_CMD_SBSM_DATA_OUT, *P_CGBC_CMD_SBSM_DATA_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD021^

                                                                   /*MOD038v*/
/*------------------------------------
 * Hardware monitoring data structure
 *------------------------------------
 */

/* HWM data header structure */
typedef struct CGBC_HWM_DATA_HDR_STRUCT
    {
     uint8_t sts;                       /* hardware monitoring status byte  */
     uint8_t siz;                       /* size of HWM data space in bytes  */
     uint8_t rev;                       /* BC firmware revision byte        */
     uint8_t res;                       /* reserved                         */
    } CGBC_HWM_DATA_HDR, *P_CGBC_HWM_DATA_HDR;

/* sensor info structure */
typedef struct CGBC_HWM_SENSOR_STRUCT
    {
     uint8_t typ;                       /* hardware monitoring sensor type  */
     int16_t val;                       /* hardware monitoring sensor value */ //MOD070
     uint8_t res;                       /* reserved                         */
    } CGBC_HWM_SENSOR, *P_CGBC_HWM_SENSOR;

                                                                   /*MOD043v*/
/* complete HWM data space  */
typedef struct CGBC_HWM_DATA_STRUCT
    {
     CGBC_HWM_DATA_HDR hdr;             /* HWM data space header            */
     CGBC_HWM_SENSOR   sensorTbl[];     /* hardware monitoring sensor table */
    } CGBC_HWM_DATA, *P_CGBC_HWM_DATA;
                                                                   /*MOD043^*/
                                                                   /*MOD038^*/

                                                                     //MOD014v
/*--------------------------------------------
 * Hardware monitoring status byte (HWM_STAT)
 *--------------------------------------------
 */

#define CGBC_HWM_ACTIVE   0x80
#define CGBC_HWM_FEAT_MSK 0x0F



/*------------------------------------------------------
 * Board controller firmware revision byte (HWM_FW_REV)
 *------------------------------------------------------
 */

#define CGBC_HWM_MAJ_REV_MSK 0xF0
#define CGBC_HWM_MIN_REV_MSK 0x0F



/*-----------------------------------------------------
 * Hardware monitoring sensor type field (HWM_Sx_TYPE)
 *-----------------------------------------------------
 */

#define CGBC_SENSOR_ACTIVE      0x80

#define CGBC_SENSOR_GROUP_MSK   0x60
 #define CGBC_NO_SENSOR          0x00
 #define CGBC_TEMP_SENSOR        0x20
 #define CGBC_VOLTAGE_SENSOR     0x40
 #define CGBC_CURRENT_SENSOR     0x40                               /*MOD038*/
 #define CGBC_FAN_SENSOR         0x60

#define CGBC_SENSOR_TYPE_MSK    0x1F
 #define CGBC_SENSOR_RESERVED    0x00

 #define CGBC_TEMP_CPU           0x01
 #define CGBC_TEMP_BOX           0x02
 #define CGBC_TEMP_ENV           0x03
 #define CGBC_TEMP_BOARD         0x04
 #define CGBC_TEMP_BACKPLANE     0x05
 #define CGBC_TEMP_CHIPSETS      0x06
 #define CGBC_TEMP_VIDEO         0x07
 #define CGBC_TEMP_OTHER         0x08
 #define CGBC_TEMP_DIMM_TOP      0x09
 #define CGBC_TEMP_DIMM_TOP_0    0x09                              /* MOD051 v*/
 #define CGBC_TEMP_DIMM_BOTTOM   0x0A
 #define CGBC_TEMP_DIMM_BOT_0    0x0A                              /* MOD052 */
 #define CGBC_TEMP_BOARD_ALT     0x0B                              /* MOD049 */
 #define CGBC_TEMP_DIMM_TOP_1    0x0C                              /* MOD051 ^*/
 #define CGBC_TEMP_DIMM_TOP_2    0x0D                              /* MOD052 */
 #define CGBC_TEMP_DIMM_TOP_3    0x0E                              /* MOD052 */
 #define CGBC_TEMP_DIMM_TOP_4    0x0F                              /* MOD052 */
 #define CGBC_TEMP_DIMM_TOP_5    0x10                              /* MOD052 */
 #define CGBC_TEMP_DIMM_TOP_6    0x11                              /* MOD052 */
 #define CGBC_TEMP_DIMM_TOP_7    0x12                              /* MOD052 */
 #define CGBC_TEMP_DIMM_BOT_1    0x13                              /* MOD052 */

 #define CGBC_VOLTAGE_CPU        0x01
 #define CGBC_VOLTAGE_DC         0x02
 #define CGBC_VOLTAGE_DC_STANDBY 0x03
 #define CGBC_VOLTAGE_BAT_CMOS   0x04
 #define CGBC_VOLTAGE_BAT_POWER  0x05
 #define CGBC_VOLTAGE_AC         0x06
 #define CGBC_VOLTAGE_OTHER      0x07
 #define CGBC_VOLTAGE_5V_S0      0x08
 #define CGBC_VOLTAGE_5V_S5      0x09
 #define CGBC_VOLTAGE_33V_S0     0x0A
 #define CGBC_VOLTAGE_33V_S5     0x0B
 #define CGBC_VOLTAGE_VCOREA     0x0C
 #define CGBC_VOLTAGE_VCOREB     0x0D
 #define CGBC_VOLTAGE_12V_S0     0x0E
 #define CGBC_VOLTAGE_12V_S5     0x0F                              /* MOD040 */
                                                                   /*MOD038v*/
 #define CGBC_CURRENT_DC         0x12
 #define CGBC_CURRENT_5V_S0      0x18
 #define CGBC_CURRENT_12V_S0     0x1E
                                                                   /*MOD038^*/
 #define CGBC_FAN_CPU            0x01
 #define CGBC_FAN_BOX            0x02
 #define CGBC_FAN_ENV            0x03
 #define CGBC_FAN_CHIPSET        0x04
 #define CGBC_FAN_VIDEO          0x05
 #define CGBC_FAN_OTHER          0x06
                                                                   /*MOD038v*/
/*---------------------------------------------------------------------
 * Hardware monitoring error codes in sensor value field (HWM_Sx_VALUE)
 *---------------------------------------------------------------------
 */

#define CGBC_HWM_NOT_PRESENT     0x00              /* sensor not present    */
#define CGBC_HWM_NOT_SUPPORTED   0x01              /* sensor not supported  */
#define CGBC_HWM_INACTIVE        0x02              /* sensor inactive       */
#define CGBC_HWM_BROKEN          0x03              /* sensor broken or open */
#define CGBC_HWM_SHORTCUT        0x04              /* sensor shortcut       */
                                                                   /*MOD038^*/
                                                                   /*MOD040v*/
/*------------------------
 * HWM reporting commands
 *------------------------
 */
#define HWM_CMD_BYTE_PON 0x02                            /* Power ON request */
#define HWM_CMD_BYTE_RSD 0x01                            /* ReSenD request   */

/*---------------------------------------------
 * CGBC_CMD_HWM_CONFIG command/response packet
 *---------------------------------------------
 */
typedef struct
CGBC_HWM_UNIT_COMMON_CONFIG_STRUCT /* This structure defines the common part of
 * the HWM unit config parameters supported by all types of HWM units. */
{
  uint8_t   ui8UnitMode;             /* see HWM unit mode byte               */
  uint8_t   ui8ErrorStatus;          /* see hardware monitoring error codes  */
  uint16_t ui16SampleRateMs;         /* HWM unit sample rate in milliseconds */
  uint32_t ui32Reserved;             /* reserved for future extensions       */
}
CGBC_HU_CFG, *P_CGBC_HU_CFG;

/* Bit definitions for the HWM unit mode byte (CGBC_HU_CFG.ui8UnitMode) */
#define CGBC_HU_ACTIVE            0x80   /* activated and operating          */
#define CGBC_HU_RUNTIME           0x40   /* S0 runtime power state supported */
#define CGBC_HU_STANDBY           0x20   /* Sx standby power state supported */
#define CGBC_HU_HOST_CONTROLLED   0x10   /* host controlled, no auto-on      */
#define CGBC_HU_FILTER_DEPTH_MASK 0x07   /* sensor input filter depth        */
#define CGBC_HU_FILTER_DEPTH_128  0x07
#define CGBC_HU_FILTER_DEPTH_64   0x06
#define CGBC_HU_FILTER_DEPTH_32   0x05
#define CGBC_HU_FILTER_DEPTH_16   0x04
#define CGBC_HU_FILTER_DEPTH_8    0x03
#define CGBC_HU_FILTER_DEPTH_4    0x02
#define CGBC_HU_FILTER_DEPTH_2    0x01
#define CGBC_HU_FILTER_OFF        0x00
                                                                      //MOD044v
#define CGBC_HU_STEP_WIDTH_MASK   0x07   /* maximum actuator set point       */
                                                                      //MOD045v
#define CGBC_HU_STEP_WIDTH_1      0x07   /* stepping width in percentage of  */
#define CGBC_HU_STEP_WIDTH_2      0x06   /* the actuator set point range     */
#define CGBC_HU_STEP_WIDTH_4      0x05   /* (ui16SetMax - ui16SetMin)        */
#define CGBC_HU_STEP_WIDTH_8      0x04
#define CGBC_HU_STEP_WIDTH_16     0x03
#define CGBC_HU_STEP_WIDTH_32     0x02
#define CGBC_HU_STEP_WIDTH_64     0x01
#define CGBC_HU_STEP_WIDTH_100    0x00
                                                                      //MOD045^
                                                                      //MOD044^
typedef struct
CGBC_HWM_SENSOR_UNIT_CONFIG_STRUCT /* This is the structure of the config
 * parameters for HWM sensor units. */
{
  CGBC_HU_CFG xCmn;                       /* see common config parameters    */
  uint8_t   ui8LogType;                   /* logical sensor type             */
  uint8_t   ui8PhyType;                   /* physical sensor type            */
  uint8_t   ui8BusNum;                    /* bus on which the sensor resides */
  uint8_t   ui8DevAddr;                   /* sensor device address           */
  uint8_t  aui8Reserved[0];               /* reserved for extensions  MOD057 */
}
CGBC_HSU_CFG, *P_CGBC_HSU_CFG;

/* Values for the logical sensor type (CGBC_HSU_CFG.ui8LogType) */
/* See sensor groups and types as for HWM_Sx_TYPE. */

/* Values for the physical sensor type (CGBC_HSU_CFG.ui8PhyType) */
#define CGBC_HSU_UNKNOWN    0x00
#define CGBC_HSU_INTEL_PECI 0x01
#define CGBC_HSU_AMD_SBTSI  0x02
#define CGBC_HSU_MCU_INT    0x03
#define CGBC_HSU_DIMM_SMB   0x04
#define CGBC_HSU_ISL28022   0x05
#define CGBC_HSU_G781_SMB   0x06                                      //MOD044
#define CGBC_HSU_INTEL_ESPI 0x07                                      //MOD046
#define CGBC_HSU_NONE       0xFF

typedef struct
CGBC_HWM_ACTUATOR_UNIT_CONFIG_STRUCT /* This is the structure of the config
 * parameters for HWM actuator units. */
{
  CGBC_HU_CFG xCmn;                     /* see common config parameters      */
  uint8_t   ui8LogType;                 /* logical actuator type             */
  uint8_t   ui8PhyType;                 /* physical actuator type            */
  uint8_t   ui8BusNum;                  /* bus on which the actuator resides */
  uint8_t   ui8DevAddr;                 /* actuator device address           */
  uint16_t ui16SetMin;                  /* set point minimum                 */
  uint16_t ui16SetMax;                  /* set point maximum                 */
  uint16_t ui16SetDflt;                 /* default set point                 */
  uint16_t ui16SetCur;                  /* current set point                 */
  uint8_t  aui8Reserved[0];             /* reserved for extensions    MOD057 */
}
CGBC_HAU_CFG, *P_CGBC_HAU_CFG;

/* Values for the logical actuator type (CGBC_HAU_CFG.ui8LogType) */
#define CGBC_HAU_RESERVED 0x00
#define CGBC_HAU_FAN_CPU  0x01
#define CGBC_HAU_FAN_SYS  0x02                                        //MOD046

/* Values for the physical actuator type (CGBC_HAU_CFG.ui8PhyType) */
#define CGBC_HAU_UNKNOWN    0x00
#define CGBC_HAU_FAN_EC_100 0x01
#define CGBC_HAU_FAN_EC_RPM 0x02
#define CGBC_HAU_NONE       0xFF

typedef struct
CGBC_HWM_CONTROL_UNIT_CONFIG_STRUCT /* This is the structure of the config
 * parameters for HWM control units. */
{
  CGBC_HU_CFG xCmn;             /* see common config parameters              */
  uint16_t ui16LimitHi;         /* upper sensor limit                        */
  uint16_t ui16LimitLo;         /* lower sensor limit                        */
  uint8_t   ui8SetHi;           /* set point when sensor above upper limit   */
  uint8_t   ui8SetMidMax;       /* set point when sensor at mid range top    */
  uint8_t   ui8SetMidMin;       /* set point when sensor at mid range bottom */
  uint8_t   ui8SetLo;           /* set point when sensor below lower limit   */
  uint8_t   ui8SenSel;          /* HWM sensor selector                       */
  uint8_t   ui8ActSel;          /* HWM actuator selector                     */
  uint8_t   ui8SenPin;          /* pin number of MCU pin sensor       MOD056 */
  uint8_t   ui8ActPin;          /* pin number of MCU pin actuator     MOD056 */
  uint8_t   ui8Usage;           /* intended HWM control unit usage    MOD058 */
  uint8_t  aui8Reserved[0];     /* reserved for future extensions     MOD057 */
}
CGBC_HCU_CFG, *P_CGBC_HCU_CFG;

typedef union
CGBC_HWM_UNIT_CONFIG_UNION /* This is the overall union of config parameters
 * for all types of HWM units. */
{
  CGBC_HU_CFG  xCmn;           /* see common config parameters               */
  CGBC_HSU_CFG xSen;           /* HWM sensor unit config parameters          */
  CGBC_HAU_CFG xAct;           /* HWM actuator unit config parameters        */
  CGBC_HCU_CFG xCtl;           /* HWM control unit config parameters         */
  uint8_t aui8RawData[24];     /* maximum structure size              MOD057 */
}
CGBC_HWM_UNIT_CFG, *P_CGBC_HWM_UNIT_CFG;

typedef struct
CGBC_CMD_HWM_CFG_IN_STRUCT /* command packet structure for CGBC_CMD_HWM_CFG */
{
  unsigned char   ui8Cmd;                      /* CGBC_CMD_HWM_CONFIG        */
  unsigned char   ui8Sel;                      /* HWM unit selector          */
  CGBC_HWM_UNIT_CFG xCfg;                      /* HWM unit config parameters */
} CGBC_CMD_HWM_CFG_IN, *P_CGBC_CMD_HWM_CFG_IN;

typedef struct
CGBC_CMD_HWM_CFG_OUT_STRUCT /* response data structure for CGBC_CMD_HWM_CFG */
{
  unsigned char   ui8Sts;                      /* command status             */
  unsigned char   ui8Sel;                      /* HWM unit selector          */
  CGBC_HWM_UNIT_CFG xCfg;                      /* HWM unit config parameters */
} CGBC_CMD_HWM_CFG_OUT, *P_CGBC_CMD_HWM_CFG_OUT;

/* Bit definitions for the HWM unit selector (CGBC_CMD_HWM_CFG_IO.ui8Sel) */
#define CGBC_HWM_CUP      0x80  /* configuration update request/status flag */

#define CGBC_HUT_MASK     0x60  /* HWM unit type */
#define CGBC_HUT_RESERVED 0x00
#define CGBC_HUT_SENSOR   0x20
#define CGBC_HUT_ACTUATOR 0x40
#define CGBC_HUT_CONTROL  0x60
#define CGBC_HUT_MASK_S   5

#define CGBC_HUN_MASK     0x1F  /* HWM unit number */
                                                                     //MOD040^
                                                                     //MOD056v
/* Special HWM unit selectors */
#define CGBC_HUS_RESERVED_1F  (CGBC_HUT_RESERVED | (CGBC_HUN_MASK - 0))
#define CGBC_HUS_MCU_GPIL_SEN (CGBC_HUT_RESERVED | (CGBC_HUN_MASK - 1))
#define CGBC_HUS_MCU_GPIH_SEN (CGBC_HUT_RESERVED | (CGBC_HUN_MASK - 2))
#define CGBC_HUS_MCU_GPOL_ACT (CGBC_HUT_RESERVED | (CGBC_HUN_MASK - 3))
#define CGBC_HUS_MCU_GPOH_ACT (CGBC_HUT_RESERVED | (CGBC_HUN_MASK - 4))
                                                                     //MOD056^
                                                                     //MOD058v
/* intended HWM control unit usage codes */
#define CGBC_HCU_USAGE_GENERAL  0x00               /* general purpose        */
#define CGBC_HCU_USAGE_CPU_TEMP 0x01               /* CPU thermal control    */
#define CGBC_HCU_USAGE_SYS_TEMP 0x02               /* system thermal control */
#define CGBC_HCU_USAGE_CPU_THRT 0x03               /* CPU throttling control */
                                                                     //MOD058^
                                                                     //MOD023v
/*----------------------------------------
 * CGBC_CMD_PECI command/response packets
 *----------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_PECI_IN_STRUCT          // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_PECI
    unsigned char addr;                         // PECI client address
    unsigned char wrCnt;                        // PECI write count
    unsigned char rdCnt;                        // PECI read count
    unsigned char wrDat[32];                    // TxD
   } CGBC_CMD_PECI_IN, *P_CGBC_CMD_PECI_IN;

typedef struct CGBC_CMD_PECI_OUT_STRUCT         // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char rdDat[32];                    // RxD
   } CGBC_CMD_PECI_OUT, *P_CGBC_CMD_PECI_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD023^

                                                                   /*MOD024v*/
/*--------------------------------------------
 * CGBC_CMD_I2C_GATE command/response packets
 *--------------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_I2C_GATE_IN_STRUCT      // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_I2C_GATE
    unsigned char parm;                         // I2C gate control parameter
   } CGBC_CMD_I2C_GATE_IN, *P_CGBC_CMD_I2C_GATE_IN;

typedef struct CGBC_CMD_I2C_GATE_OUT_STRUCT     // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char parm;                         // I2C gate control parameter
   } CGBC_CMD_I2C_GATE_OUT, *P_CGBC_CMD_I2C_GATE_OUT;

#endif //!__ASSEMBLER__

/*
 * I2C gate control parameter bit mapping
 */
#define CGBC_I2C_GATE_GSB     0x80
#define CGBC_I2C_GATE_GS0     0x40
#define CGBC_I2C_GATE_GBT     0x20
#define CGBC_I2C_GATE_GAT     0x10
#define CGBC_I2C_GATE_GUP     0x08
#define CGBC_I2C_GATE_BUS_MSK 0x07
                                                                   /*MOD024^*/

                                                                   /*MOD025v*/
/*---------------------------------------------------------------------------
 * CGBC_CMD_CFG_PINS
 *
 * Input:  bits 7-6 - control mode
 *                    00 = no change / read current pin output states only
 *                    01 = update current pin states only
 *                    10 = update EEPROM content only
 *                    11 = update current pin states and EEPROM content
 *         bit  5-2 - reserved for more config pins
 *         bit  1   - new CONFIG_PIN_1 state
 *         bit  0   - new CONFIG_PIN_0 state
 *
 * Output: bits 7-6 - reserved
 *         bit  5-2 - reserved for more config pins
 *         bit  1   - current CONFIG_PIN_1 output state
 *         bit  0   - current CONFIG_PIN_0 output state
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_CFG_PINS_IN_STRUCT      // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_CFG_PINS
    unsigned char parm;                         // config pins parameter
   } CGBC_CMD_CFG_PINS_IN, *P_CGBC_CMD_CFG_PINS_IN;

typedef struct CGBC_CMD_CFG_PINS_OUT_STRUCT     // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char state;                        // current pin output state
   } CGBC_CMD_CFG_PINS_OUT, *P_CGBC_CMD_CFG_PINS_OUT;

#endif //!__ASSEMBLER__

/*
 * CGBC_CMD_CFG_PINS parameter/response bit mapping
 */
#define CGBC_CFG_PINS_MODE_MSK     0xC0
#define CGBC_CFG_PINS_MODE_EEP_CUR 0xC0
#define CGBC_CFG_PINS_MODE_EEP     0x80
#define CGBC_CFG_PINS_MODE_CUR     0x40
#define CGBC_CFG_PINS_MODE_READ    0x00

#define CGBC_CFG_PINS_MSK          0x3F
#define CGBC_CFG_PIN_5             0x20
#define CGBC_CFG_PIN_4             0x10
#define CGBC_CFG_PIN_3             0x08
#define CGBC_CFG_PIN_2             0x04
#define CGBC_CFG_PIN_1             0x02
#define CGBC_CFG_PIN_0             0x01
                                                                   /*MOD025^*/

                                                                    /*MOD038v*/
/*------------------
 * CGBC_CMD_AVR_SPM
 *------------------
 */
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_AVR_SPM_IN_STRUCT       /* command packet structure  */
  {
    uint8_t  cmd;                               /* CGBC_CMD_AVR_SPM          */
    uint32_t spmCmd;                            /* AVR SPM command           */
  } CGBC_CMD_AVR_SPM_IN, *P_CGBC_CMD_AVR_SPM_IN;

typedef struct CGBC_CMD_AVR_SPM_OUT_STRUCT      /* response packet structure */
   {
    uint8_t  sts;                               /* command status            */
    uint32_t spmRes;                            /* AVR SPM command response  */
   } CGBC_CMD_AVR_SPM_OUT, *P_CGBC_CMD_AVR_SPM_OUT;

#endif /* not __ASSEMBLER__ */
                                                                    /*MOD038^*/

                                                                   /*MOD031v*/
/*-------------------
 * CGBC_CMD_COMX-CFG
 *-------------------
 */
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_COMX_CFG_IN_STRUCT      /* command packet structure  */
  {
    uint8_t  cmd;                               /* CGBC_CMD_COMX_CFG         */
    uint8_t  parm;                              /* COMX config parameter     */
    uint16_t portBase;                          /* LPC I/O base address      */
    uint32_t baudrate;                          /* baudrate                  */
    uint8_t  lineCtrl;                          /* line control              */
    uint8_t  modemCtrl;                         /* modem control             */
  } CGBC_CMD_COMX_CFG_IN, *P_CGBC_CMD_COMX_CFG_IN;

typedef struct CGBC_CMD_COMX_CFG_OUT_STRUCT     /* response packet structure */
  {
    uint8_t  sts;                               /* command status            */
    uint8_t  parm;                              /* COMX config parameter     */
    uint16_t portBase;                          /* LPC I/O base address      */
    uint32_t baudrate;                          /* baudrate                  */
    uint8_t  lineCtrl;                          /* line control              */
    uint8_t  modemCtrl;                         /* modem control             */
  } CGBC_CMD_COMX_CFG_OUT, *P_CGBC_CMD_COMX_CFG_OUT;

#endif /* not __ASSEMBLER__ */

/* COMX configuration parameter */
#define COMX_CTRL_MODE_M       0xC0             /* control mode              */
#define COMX_CTRL_MODE_READ    0x00             /* read configuration only   */
#define COMX_CTRL_MODE_DISABLE 0x40             /* disable COM port          */
#define COMX_CTRL_MODE_ENABLE  0xC0             /* enable COM port           */
#define COMX_CTRL_MODE_S       6

#define COMX_PORT_SELECT_M     0x20             /* COM port select           */
#define COMX_PORT_SELECT_0     0x00             /* COM port 0                */
#define COMX_PORT_SELECT_1     0x20             /* COM port 1                */
#define COMX_PORT_SELECT_S     5

#define COMX_IRQ_MODE_M        0x10             /* IRQ mode                  */
#define COMX_IRQ_MODE_DISABLE  0x00             /* no IRQ allocated          */
#define COMX_IRQ_MODE_ENABLE   0x10             /* enabled / IRQ allocated   */
#define COMX_IRQ_MODE_S        4

#define COMX_IRQ_SELECT_M      0x0F             /* IRQ select                */
#define COMX_IRQ_SELECT_IRQ0   0
#define COMX_IRQ_SELECT_IRQ1   1
#define COMX_IRQ_SELECT_IRQ2   2
#define COMX_IRQ_SELECT_IRQ3   3
#define COMX_IRQ_SELECT_IRQ4   4
#define COMX_IRQ_SELECT_IRQ5   5
#define COMX_IRQ_SELECT_IRQ6   6
#define COMX_IRQ_SELECT_IRQ7   7
#define COMX_IRQ_SELECT_IRQ8   8
#define COMX_IRQ_SELECT_IRQ9   9
#define COMX_IRQ_SELECT_IRQ10  10
#define COMX_IRQ_SELECT_IRQ11  11
#define COMX_IRQ_SELECT_IRQ12  12
#define COMX_IRQ_SELECT_IRQ13  13
#define COMX_IRQ_SELECT_IRQ14  14
#define COMX_IRQ_SELECT_IRQ15  15
#define COMX_IRQ_SELECT_S      0
                                                                   /*MOD031^*/
                                                                   /*MOD040v*/
/*---------------------------------------------
 * Second COM port 1 from host LPC perspective
 *---------------------------------------------
 */
/* register offsets from the LPC base address BC_COM1_BASE */
#define BC_COM1_INDEX  0                       /* COM port 1 index register  */
#define BC_COM1_DATA   1                       /* COM port 1 data register   */
#define BC_COM1_STROBE 3                       /* COM port 1 strobe register */

/* COM port 1 index register BC_COM1_IDX */
#define BC_COM1_IDX_CMD_M 0xF8       /* command field */
#define BC_COM1_IDX_CMD_S 3

#define BC_COM1_IDX_IDX_M 0x07       /* index field */
#define BC_COM1_IDX_IDX_S 0

/* COM port 1 commands */
#define BC_COM1_CMD_READ_NS16550_REG  0x00
#define BC_COM1_CMD_WRITE_NS16550_REG 0x08
#define BC_COM1_CMD_READ_ID           0x10
                                                                   /*MOD040^*/

                                                                   /*MOD036v*/
/*------------------
 * CGBC_CMD_BLT_PWM
 *------------------
 */
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_BLT_PWM_IN_STRUCT       /* command packet structure  */
  {
    uint8_t  cmd;                               /* CGBC_CMD_COMX_CFG         */
    uint8_t  pwmDuty;                           /* backlight PWM duty cycle  */
    uint16_t pwmFreq;                           /* backlight PWM frequency   */
  } CGBC_CMD_BLT_PWM_IN, *P_CGBC_CMD_BLT_PWM_IN;

typedef struct CGBC_CMD_BLT_PWM_OUT_STRUCT      /* response packet structure */
  {
    uint8_t  sts;                               /* command status            */
    uint8_t  pwmDuty;                           /* backlight PWM duty cycle  */
    uint16_t pwmFreq;                           /* backlight PWM frequency   */
  } CGBC_CMD_BLT_PWM_OUT, *P_CGBC_CMD_BLT_PWM_OUT;

#endif /* not __ASSEMBLER__ */

/* backlight PWM duty cycle parameter bit masks */
#define BLT_PWM_DUTY_M      0x7F
#define BLT_PWM_DUTY_S      0

#define BLT_PWM_INVERTED_M  0x80
#define BLT_PWM_INVERTED_S  7
                                                                   /*MOD036^*/

                                                                   /* MOD038 */
                                                                   /*      v */
/*--------------------
 * CGBC_CMD_DEVICE_ID
 *--------------------
 */
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_DEVICE_ID_OUT_STRUCT    /* response packet structure */
  {
    uint8_t  sts;                               /* command status            */
    uint32_t did0;                              /* device identification 0   */
    uint32_t did1;                              /* device identification 1   */
  } CGBC_CMD_DEVICE_ID_OUT, *P_CGBC_CMD_DEVICE_ID_OUT;

#endif /* not __ASSEMBLER__ */

/*---------------------
 * CGBC_CMD_HWM_SENSOR
 *---------------------
 */
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_HWM_SENSOR_IN_STRUCT    /* command packet structure  */
  {
    uint8_t cmd;                                /* CGBC_CMD_AVR_SPM          */
    uint8_t sensorNum;                          /* sensor number             */
  } CGBC_CMD_HWM_SENSOR_IN, *P_CGBC_CMD_HWM_SENSOR_IN;

typedef struct CGBC_CMD_HWM_SENSOR_OUT_STRUCT   /* response packet structure */
   {
    uint8_t         sts;                        /* command status            */
    uint8_t         sensorCount;                /* # of available sensors    */
    CGBC_HWM_SENSOR sensorDat;                  /* sensor data               */
   } CGBC_CMD_HWM_SENSOR_OUT, *P_CGBC_CMD_HWM_SENSOR_OUT;

#endif /* not __ASSEMBLER__ */
                                                                   /*      ^ */
                                                                   /* MOD038 */
                                                                   /* MOD040 */
                                                                   /*      v */
/*------------------------------
 * CGBC_CMD_POST_CFG parameters
 *------------------------------
 */
#ifndef __ASSEMBLER__

typedef struct
CGBC_POST_CFG_PARMS_STRUCT         /* POST Code Feature parameter structure  */
{
  uint8_t  ctrl;                             /* control parameter            */
  uint16_t ch1Addr;                          /* debug port address parameter */
  uint8_t  relayDevAddr;                     /* relay device address         */
} CGBC_POST_CFG_PARMS, *P_CGBC_POST_CFG_PARMS;

typedef struct
CGBC_CMD_POST_CFG_IN_STRUCT                     /* command packet structure  */
{
  uint8_t             cmd;                      /* CGBC_CMD_POST_CFG         */
  CGBC_POST_CFG_PARMS parms;                    /* feature parameters        */
} CGBC_CMD_POST_CFG_IN, *P_CGBC_CMD_POST_CFG_IN;

typedef struct
CGBC_CMD_POST_CFG_OUT_STRUCT                    /* response packet structure */
{
  uint8_t  sts;                                 /* command status            */
  CGBC_POST_CFG_PARMS parms;                    /* feature parameters        */
} CGBC_CMD_POST_CFG_OUT, *P_CGBC_CMD_POST_CFG_OUT;

#endif //!__ASSEMBLER__

/* CGBC_CMD_POST_CFG control parameter bit mapping */
#define CGBC_POST_CFG_UPDATE    0x80   /* current config update request      */
#define CGBC_POST_EEP_UPDATE    0x40   /* EEPROM parameter update request    */
#define CGBC_POST_RAW_MODE      0x20   /* raw relay data mode                */

#define CGBC_POST_RELAY_IF_MSK  0x18   /* relay interface selection          */
#define  CGBC_POST_RELAY_OFF     0x00  /*   - relay off                      */
#define  CGBC_POST_RELAY_DIAG    0x08  /*   - diagnostic console             */
#define  CGBC_POST_RELAY_I2C     0x10  /*   - I2C-bus                        */
#define  CGBC_POST_RELAY_SMB     0x18  /*   - SMBus                          */

#define CGBC_POST_CH3_ENABLE    0x04   /* 3rd POST code input channel enable */
#define CGBC_POST_CH2_ENABLE    0x02   /* 2nd POST code input channel enable */
#define CGBC_POST_CH1_ENABLE    0x01   /* 1st POST code input channel enable */
                                                                      //MOD044v
/* debug port address parameter bit mapping */
#define CGBC_POST_CH1_ADDR_MSK 0xFFF8       /* primary debug port address    */
#define CGBC_POST_CH2_ADDR_MSK 0x0003       /* secondary debug port decoding */
#define  CGBC_POST_CH2_ADDR_4   0x0000      /*   - primary port address + 4  */
#define  CGBC_POST_CH2_ADDR_1   0x0001      /*   - primary port address + 1  */
#define  CGBC_POST_CH2_ADDR_2   0x0010      /*   - primary port address + 2  */
                                                                      //MOD044^
                                                                      //MOD066v
#define CGBC_POST_SEC_DISPLAY_DIS 0x0004 /* Actively disable the secondary POST
                                          * code display which are the lower
										  * two digits. */
                                                                      //MOD066^


/*-------------------------------
 * CGBC_CMD_LFP_DELAY parameters
 *-------------------------------
 */
typedef struct
CGBC_CMD_LFP_DELAY_IN_STRUCT                    /* command packet structure  */
{
  uint8_t cmd;                                  /* CGBC_CMD_LFP_DELAY        */
  uint8_t ui8RstDelay250ms;                     /* delay in units of 250ms   */
  uint8_t ui8BltDelay25ms;                      /* delay in units of 25ms    */
} CGBC_CMD_LFP_DELAY_IN, *P_CGBC_CMD_LFP_DELAY_IN;

typedef struct
CGBC_CMD_LFP_DELAY_OUT_STRUCT                   /* response packet structure */
{
  uint8_t sts;                                  /* command status            */
  uint8_t ui8RstDelay250ms;                     /* delay in units of 250ms   */
  uint8_t ui8BltDelay25ms;                      /* delay in units of 25ms    */
} CGBC_CMD_LFP_DELAY_OUT, *P_CGBC_CMD_LFP_DELAY_OUT;

#define CGBC_LFP_DELAY_READ 0xFF   /* reserved delay value for read function */
                                                                   /*      ^ */
                                                                   /* MOD040 */
                                                                   /* MOD041 */
                                                                   /*      v */
/*------------------------------
 * CGBC_CMD_DIAG_CFG parameters
 *------------------------------
 */
#ifndef __ASSEMBLER__

typedef struct
CGBC_DIAG_CON_CFG_STRUCT       /* diagnostic console configuration structure */
{
  uint8_t ui8Ctrl;                              /* control byte              */
  uint8_t ui8Parm;                              /* interface parameter       */
} CGBC_DIAG_CON_CFG, *P_CGBC_DIAG_CON_CFG;

typedef struct
CGBC_CMD_DIAG_CFG_IN_STRUCT                     /* command packet structure  */
{
  uint8_t         ui8Cmd;                       /* CGBC_CMD_DIAG_CFG         */
  CGBC_DIAG_CON_CFG xCfg;                       /* diagnostic console config */
} CGBC_CMD_DIAG_CFG_IN, *P_CGBC_CMD_DIAG_CFG_IN;

typedef struct
CGBC_CMD_DIAG_CFG_OUT_STRUCT                    /* response packet structure */
{
  uint8_t  sts;                                 /* command status            */
  CGBC_DIAG_CON_CFG xCfg;                       /* diagnostic console config */
} CGBC_CMD_DIAG_CFG_OUT, *P_CGBC_CMD_DIAG_CFG_OUT;

#endif //!__ASSEMBLER__

/* Diagnostic console control byte bit mapping */
#define CGBC_DIAG_CFG_UPDATE    0x80   /* current config update request      */
#define CGBC_DIAG_EEP_UPDATE    0x40   /* EEPROM parameter update request    */

#define CGBC_DIAG_UART_MUX_M    0x20     /* UART mux config bit     */ //MOD055
#define  CGBC_DIAG_UART_MUX_cBC  0x00    /*  - cBC UART selected    */ //MOD055
#define  CGBC_DIAG_UART_MUX_SoC  0x20    /*  - SoC UART selected    */ //MOD055

#define CGBC_DIAG_DHCC_ENA      0x10     /* DHCC enable             */ //MOD053

#define CGBC_DIAG_IF_MSK        0x0F     /* diag console I/F select */ //MOD044
#define  CGBC_DIAG_CON_OFF       0x00    /*   - diagnostic console disabled  */
#define  CGBC_DIAG_CON_AUX       0x01    /*   - auxiliary serial port        */
#define  CGBC_DIAG_CON_SER0      0x02    /*   - system serial port 0         */
#define  CGBC_DIAG_CON_SER1      0x03    /*   - system serial port 1         */
#define  CGBC_DIAG_CON_I2C0      0x08    /*   - primary I2C-bus     */ //MOD044
#define  CGBC_DIAG_CON_SMB0      0x0A    /*   - primary SMBus       */ //MOD044
                                                                   /*      ^ */
                                                                   /* MOD041 */
                                                                      //MOD045v
/*-------------------------------
 * CGBC_CMD_PWROK_DELAY parameters
 *-------------------------------
 */
typedef struct
CGBC_CMD_PWROK_DELAY_IN_STRUCT                  /* command packet structure  */
{
  uint8_t cmd;                                  /* CGBC_CMD_PWROK_DELAY      */
  uint8_t ui8PwrokDelay25ms;                    /* delay in units of 25ms    */
} CGBC_CMD_PWROK_DELAY_IN, *P_CGBC_CMD_PWROK_DELAY_IN;

typedef struct
CGBC_CMD_PWROK_DELAY_OUT_STRUCT                 /* response packet structure */
{
  uint8_t sts;                                  /* command status            */
  uint8_t ui8PwrokDelay25ms;                    /* delay in units of 25ms    */
} CGBC_CMD_PWROK_DELAY_OUT, *P_CGBC_CMD_PWROK_DELAY_OUT;

#define CGBC_PWROK_DELAY_READ 0xFF /* reserved delay value for read function */
                                                                      //MOD045^

                                                                      //MOD044v
/* --------------------------------------------
 * Event logging tags and log buffer entries
 * ----------------------------------------- */

#define CGBC_LOG_TAG_SYST   0x10        /* log entry with system time stamp  */
#define CGBC_LOG_TAG_LOGT   0x08        /* log entry with logging time stamp */
#define CGBC_LOG_TAG_NOTS   0x00        /* log entry without time stamp      */
#define CGBC_LOG_TAG_SIZE_M 0x07        /* log entry data size mask          */

/* single tag entries */
#define CGBC_LOG_TAG_HOST_EV0    (( 0 << 5) | CGBC_LOG_TAG_NOTS | 0) /* no additional data */

/* tags for events with one data byte */
#define CGBC_LOG_TAG_HOST_EV1    (( 0 << 5) | CGBC_LOG_TAG_NOTS | 1) /* followed by event byte */
#define CGBC_LOG_TAG_PIN_HI      (( 1 << 5) | CGBC_LOG_TAG_NOTS | 1) /* followed by pin # */
#define CGBC_LOG_TAG_PIN_LO      (( 2 << 5) | CGBC_LOG_TAG_NOTS | 1) /* followed by pin # */
                                                                   /* MOD047 */
                                                                   /*      v */
#define CGBC_LOG_TAG_POST_CH1    (( 3 << 5) | CGBC_LOG_TAG_NOTS | 1) /* followed by POST code */
#define CGBC_LOG_TAG_POST_CH2    (( 4 << 5) | CGBC_LOG_TAG_NOTS | 1) /* followed by POST code */
#define CGBC_LOG_TAG_POST_CH3    (( 5 << 5) | CGBC_LOG_TAG_NOTS | 1) /* followed by POST code */
                                                                   /*      ^ */
                                                                   /* MOD047 */
                                                                   /* MOD064 */
                                                                   /*      v */
#define CGBC_LOG_TAG_VIR_HI      (( 6 << 5) | CGBC_LOG_TAG_NOTS | 1) /* followed by logical pin function */
#define CGBC_LOG_TAG_VIR_LO      (( 7 << 5) | CGBC_LOG_TAG_NOTS | 1) /* followed by logical pin function */
                                                                   /*      ^ */
                                                                   /* MOD064 */
/* tags for events with two data bytes */
#define CGBC_LOG_TAG_HOST_EV2    (( 0 << 5) | CGBC_LOG_TAG_NOTS | 2) /* followed by event word */
#define CGBC_LOG_TAG_ANALOG      (( 1 << 5) | CGBC_LOG_TAG_NOTS | 2) /* followed by ADC channel/value */
                                                                   /* MOD067 */
                                                                   /*      v */
#define CGBC_LOG_TAG_SYS_EV      (( 2 << 5) | CGBC_LOG_TAG_NOTS | 2) /* followed by group/event */
                                                                   /*      ^ */
                                                                   /* MOD067 */
/* tags for events with three data bytes */
#define CGBC_LOG_TAG_HOST_EV3    (( 0 << 5) | CGBC_LOG_TAG_NOTS | 3) /* followed by three event bytes */
                                                                   /* MOD064 */
                                                                   /*      v */
#define CGBC_LOG_TAG_HWM_EV      (( 1 << 5) | CGBC_LOG_TAG_NOTS | 3) /* followed by HWM log ID/value */
                                                                   /*      ^ */
                                                                   /* MOD064 */
/* tags for events with four data bytes */
#define CGBC_LOG_TAG_HOST_EV4    (( 0 << 5) | CGBC_LOG_TAG_NOTS | 4) /* followed by event dword */
#define CGBC_LOG_TAG_HOST_EV0_ST (( 0 << 5) | CGBC_LOG_TAG_SYST | 4) /* followed by system time stamp */
#define CGBC_LOG_TAG_HOST_EV0_LT (( 0 << 5) | CGBC_LOG_TAG_LOGT | 4) /* followed by logging time stamp */

/* tags for events with five data bytes */
#define CGBC_LOG_TAG_HOST_EV5    (( 0 << 5) | CGBC_LOG_TAG_NOTS | 5) /* followed by five event bytes */
#define CGBC_LOG_TAG_HOST_EV1_ST (( 0 << 5) | CGBC_LOG_TAG_SYST | 5) /* followed by event byte and system time stamp */
#define CGBC_LOG_TAG_HOST_EV1_LT (( 0 << 5) | CGBC_LOG_TAG_LOGT | 5) /* followed by event byte and logging time stamp */
#define CGBC_LOG_TAG_PIN_HI_ST   (( 1 << 5) | CGBC_LOG_TAG_SYST | 5) /* followed by pin # and system time stamp */
#define CGBC_LOG_TAG_PIN_HI_LT   (( 1 << 5) | CGBC_LOG_TAG_LOGT | 5) /* followed by pin # and logging time stamp */
#define CGBC_LOG_TAG_PIN_LO_ST   (( 2 << 5) | CGBC_LOG_TAG_SYST | 5) /* followed by pin # and system time stamp */
#define CGBC_LOG_TAG_PIN_LO_LT   (( 2 << 5) | CGBC_LOG_TAG_LOGT | 5) /* followed by pin # and logging time stamp */
                                                                   /* MOD047 */
                                                                   /*      v */
#define CGBC_LOG_TAG_POST_CH1_ST (( 3 << 5) | CGBC_LOG_TAG_SYST | 5) /* followed by POST code and system time stamp */
#define CGBC_LOG_TAG_POST_CH1_LT (( 3 << 5) | CGBC_LOG_TAG_LOGT | 5) /* followed by POST code and logging time stamp */
#define CGBC_LOG_TAG_POST_CH2_ST (( 4 << 5) | CGBC_LOG_TAG_SYST | 5) /* followed by POST code and system time stamp */
#define CGBC_LOG_TAG_POST_CH2_LT (( 4 << 5) | CGBC_LOG_TAG_LOGT | 5) /* followed by POST code and logging time stamp */
#define CGBC_LOG_TAG_POST_CH3_ST (( 5 << 5) | CGBC_LOG_TAG_SYST | 5) /* followed by POST code and system time stamp */
#define CGBC_LOG_TAG_POST_CH3_LT (( 5 << 5) | CGBC_LOG_TAG_LOGT | 5) /* followed by POST code and logging time stamp */
                                                                   /*      ^ */
                                                                   /* MOD047 */
                                                                   /* MOD064 */
                                                                   /*      v */
#define CGBC_LOG_TAG_VIR_HI_ST   (( 6 << 5) | CGBC_LOG_TAG_SYST | 5) /* followed by ogical pin function and system time stamp */
#define CGBC_LOG_TAG_VIR_HI_LT   (( 6 << 5) | CGBC_LOG_TAG_LOGT | 5) /* followed by ogical pin function and logging time stamp */
#define CGBC_LOG_TAG_VIR_LO_ST   (( 7 << 5) | CGBC_LOG_TAG_SYST | 5) /* followed by ogical pin function and system time stamp */
#define CGBC_LOG_TAG_VIR_LO_LT   (( 7 << 5) | CGBC_LOG_TAG_LOGT | 5) /* followed by ogical pin function and logging time stamp */
                                                                   /*      ^ */
                                                                   /* MOD064 */
/* tags for events with six data bytes */
#define CGBC_LOG_TAG_HOST_EV6    (( 0 << 5) | CGBC_LOG_TAG_NOTS | 6) /* followed by six event bytes */
#define CGBC_LOG_TAG_HOST_EV2_ST (( 0 << 5) | CGBC_LOG_TAG_SYST | 6) /* followed by event word and system time stamp */
#define CGBC_LOG_TAG_HOST_EV2_LT (( 0 << 5) | CGBC_LOG_TAG_LOGT | 6) /* followed by event word and logging time stamp */
#define CGBC_LOG_TAG_ANALOG_ST   (( 1 << 5) | CGBC_LOG_TAG_SYST | 6) /* followed by ADC channel/value and system time stamp */
#define CGBC_LOG_TAG_ANALOG_LT   (( 1 << 5) | CGBC_LOG_TAG_LOGT | 6) /* followed by ADC channel/value and logging time stamp */
                                                                   /* MOD067 */
                                                                   /*      v */
#define CGBC_LOG_TAG_SYS_EV_ST   (( 2 << 5) | CGBC_LOG_TAG_SYST | 6) /* followed by group/event and logging time stamp */
#define CGBC_LOG_TAG_SYS_EV_LT   (( 2 << 5) | CGBC_LOG_TAG_LOGT | 6) /* followed by group/event and logging time stamp */
                                                                   /*      ^ */
                                                                   /* MOD067 */
/* tags for events with seven data bytes */
#define CGBC_LOG_TAG_HOST_EV7    (( 0 << 5) | CGBC_LOG_TAG_NOTS | 7) /* followed by seven event bytes */
#define CGBC_LOG_TAG_HOST_EV3_ST (( 0 << 5) | CGBC_LOG_TAG_SYST | 7) /* followed by three event bytes and system time stamp */
#define CGBC_LOG_TAG_HOST_EV3_LT (( 0 << 5) | CGBC_LOG_TAG_LOGT | 7) /* followed by three event bytes and logging time stamp */
                                                                   /* MOD064 */
                                                                   /*      v */
#define CGBC_LOG_TAG_HWM_EV_ST   (( 1 << 5) | CGBC_LOG_TAG_SYST | 7) /* followed by HWM log ID/value and system time stamp */
#define CGBC_LOG_TAG_HWM_EV_LT   (( 1 << 5) | CGBC_LOG_TAG_LOGT | 7) /* followed by HWM log ID/value and logging time stamp */

/* HWM log IDs */
#define CGBC_LOG_HWM_VIN 0 /* voltage on the runtime power input rail */
#define CGBC_LOG_HWM_VSB 1 /* voltage on the standby power input rail */
                                                                   /*      ^ */
                                                                   /* MOD064 */
                                                                   /* MOD067 */
                                                                   /*      v */
/* system event groups */
#define CGBC_LOG_ESPI_EV 0

/* eSPI events */
#define CGBC_LOG_ESPI_RST_HI       0x00
#define CGBC_LOG_ESPI_RST_LO       0x01
#define CGBC_LOG_ESPI_VW_ON        0x02
#define CGBC_LOG_ESPI_OOB_OFF      0x03
#define CGBC_LOG_ESPI_OOB_ON       0x04
#define CGBC_LOG_ESPI_FLS_OFF      0x05
#define CGBC_LOG_ESPI_FLS_ON       0x06
#define CGBC_LOG_ESPI_PC_OFF       0x07
#define CGBC_LOG_ESPI_PC_ON        0x08
#define CGBC_LOG_ESPI_OOBRSTWRN_HI 0x09
#define CGBC_LOG_ESPI_OOBRSTWRN_LO 0x0A
#define CGBC_LOG_ESPI_OOBRSTACK_HI 0x0B
#define CGBC_LOG_ESPI_OOBRSTACK_LO 0x0C
#define CGBC_LOG_ESPI_HSTRSTWRN_HI 0x0D
#define CGBC_LOG_ESPI_HSTRSTWRN_LO 0x0E
#define CGBC_LOG_ESPI_HSTRSTACK_HI 0x0F
#define CGBC_LOG_ESPI_HSTRSTACK_LO 0x10
#define CGBC_LOG_ESPI_LPCPDn_HI    0x11
#define CGBC_LOG_ESPI_LPCPDn_LO    0x12
                                                                   /*      ^ */
                                                                   /* MOD067 */
/* log buffer entry structures */
typedef struct
CGBC_EVENT_B_STRUCT   { uint8_t   ui8Tag;
                        uint8_t   ui8Dat;     } cgbc_event_b_t;
typedef struct
CGBC_EVENT_W_STRUCT   { uint8_t   ui8Tag;
                        uint16_t ui16Dat;     } cgbc_event_w_t;
                                                                   /* MOD067 */
                                                                   /*      v */
typedef struct
CGBC_EVENT_B2_STRUCT  { uint8_t   ui8Tag;
                        uint8_t  aui8Dat[2];  } cgbc_event_b2_t;
                                                                   /*      ^ */
                                                                   /* MOD067 */
                                                                   /* MOD064 */
                                                                   /*      v */
typedef struct
CGBC_EVENT_B3_STRUCT  { uint8_t   ui8Tag;
                        uint8_t  aui8Dat[3];  } cgbc_event_b3_t;
                                                                   /*      ^ */
                                                                   /* MOD064 */
typedef struct
CGBC_EVENT_D_STRUCT   { uint8_t   ui8Tag; 
                        uint32_t ui32Dat;     } cgbc_event_d_t;
typedef struct
CGBC_EVENT_B7_STRUCT  { uint8_t   ui8Tag;
                        uint8_t  aui8Dat[7];  } cgbc_event_b7_t;
typedef struct
CGBC_EVENT_BT_STRUCT  { uint8_t   ui8Tag;
                        uint8_t   ui8Dat;
                        uint32_t ui32Tim;     } cgbc_event_bt_t;
typedef struct
CGBC_EVENT_WT_STRUCT  { uint8_t   ui8Tag;
                        uint16_t ui16Dat;
                        uint32_t ui32Tim;     } cgbc_event_wt_t;
                                                                   /* MOD067 */
                                                                   /*      v */
typedef struct
CGBC_EVENT_B2T_STRUCT { uint8_t   ui8Tag;
                        uint8_t  aui8Dat[2];
                        uint32_t ui32Tim;     } cgbc_event_b2t_t;
                                                                   /*      ^ */
                                                                   /* MOD067 */
typedef struct
CGBC_EVENT_B3T_STRUCT { uint8_t   ui8Tag;
                        uint8_t  aui8Dat[3];
                        uint32_t ui32Tim;     } cgbc_event_b3t_t;
typedef union
CGBC_EVENT_UNION      { cgbc_event_b_t   b;
                        cgbc_event_w_t   w;
                        cgbc_event_d_t   d;
                        cgbc_event_b7_t  b7;
                        cgbc_event_bt_t  bt;
                        cgbc_event_wt_t  wt;
                        cgbc_event_b3t_t b3t; } cgbc_event_t;

/* ---------------------------------------
 * CGBC_CMD_LOG_CFG command definitions
 * ------------------------------------ */

/* event logging configuration structure */
typedef struct
CGBC_LOG_CFG_STRUCT
{
  uint8_t   ui8CtlSts;                  /* event logging control/status byte */
  uint32_t ui32EventSelect;             /* event selection flags             */
  uint32_t ui32SysTimEna;               /* system time stamp enable flags    */
  uint32_t ui32LogTimEna;               /* logging time stamp enable flags   */
  uint32_t ui32StartTriggerSelect;      /* start trigger selection flags     */
  uint32_t ui32StopTriggerSelect;       /* stop trigger selection flags      */
  uint32_t ui32LogTimFrq;               /* logging timer frequency in Hz     */
} CGBC_LOG_CFG, *P_CGBC_LOG_CFG;

/* command package structure */
typedef struct
CGBC_CMD_LOG_CFG_IN_STRUCT
{
  uint8_t    ui8Cmd;                          /* CGBC_CMD_LOG_CFG            */
  CGBC_LOG_CFG xCfg;                          /* event logging configuration */
} CGBC_CMD_LOG_CFG_IN, *P_CGBC_CMD_LOG_CFG_IN;

/* result package structure */
typedef struct
CGBC_CMD_LOG_CFG_OUT_STRUCT
{
  CGBC_STATUS stsCmdSts;                      /* CGBC status byte            */
  CGBC_LOG_CFG  xCfg;                         /* event logging configuration */
} CGBC_CMD_LOG_CFG_OUT, *P_CGBC_CMD_LOG_CFG_OUT;

/* Event logging control byte bit mapping */
#define CGBC_LOG_CFG_UPDATE 0x80         /* current config update request    */
#define CGBC_LOG_EEP_UPDATE 0x40         /* EEPROM parameter update request  */
#define CGBC_LOG_START_TRIG 0x20         /* start logging upon start trigger */
                                                                   /* MOD047 */
                                                                   /*      v */
#define CGBC_LOG_RETAIN_BUF 0x10      /* retain buffer upon repeated trigger */
                                                                   /*      ^ */
                                                                   /* MOD047 */
#define CGBC_LOG_REP_MODE   0x08         /* repeat mode                      */
#define CGBC_LOG_CIR_MODE   0x04         /* circular mode                    */
#define CGBC_LOG_RES_M      0x02         /* reserved / must be zero          */
#define CGBC_LOG_ENABLE     0x01         /* event logging enable switch      */

/* Event logging status byte bit mapping */
#define CGBC_LOG_STS_RES_M     0xFC           /* reserved / must be ignored  */
#define CGBC_LOG_STATE_M       0x03           /* event logging state mask    */
#define  CGBC_LOG_STATE_OFF     0x00          /*   - off                     */
#define  CGBC_LOG_STATE_ARMED   0x01          /*   - armed, awaiting trigger */
#define  CGBC_LOG_STATE_STOPPED 0x02          /*   - stopped                 */
#define  CGBC_LOG_STATE_ON      0x03          /*   - on, recording events    */

/* Event logging flag parameter bit mapping */
                                                                   /* MOD047 */
                                                                   /*      v */
                                                                   /* MOD064 */
                                                                   /*      v */
#define CGBC_LOG_FLG_RES_M   0xFF000030    /* reserved / must be zero        */

#define CGBC_LOG_VIRT_SIG_M  0x00FF0000    /* specific virtual signal mask   */
#define CGBC_LOG_VIRT_SIG_S  16            /* specific virt. signal shft cnt */
                                                                   /*      ^ */
                                                                   /* MOD064 */
#define CGBC_LOG_POST_CODE_M 0x0000FF00    /* specific POST code mask        */
#define CGBC_LOG_POST_CODE_S 8             /* specific POST code shift count */

#define CGBC_LOG_POST_SEL_M  0x00000080    /* POST code select mask          */
#define CGBC_LOG_POST_SEL_ALL 0x00000000   /* select all POST codes          */
#define CGBC_LOG_POST_SEL_ONE 0x00000080   /* select a specific POST code    */
                                                                   /*      ^ */
                                                                   /* MOD047 */
                                                                   /* MOD064 */
                                                                   /*      v */
#define CGBC_LOG_VIRT_SEL_M  0x00000040    /* virtual signal select mask     */
#define CGBC_LOG_VIRT_SEL_ALL 0x00000000   /* select all virtual signals     */
#define CGBC_LOG_VIRT_SEL_ONE 0x00000040   /* select a specific virt. signal */

#define CGBC_LOG_VIRT_ENABLE 0x00000008    /* virtual signals logging enable */
                                                                   /*      ^ */
                                                                   /* MOD064 */
#define CGBC_LOG_PCTL_ENABLE 0x00000004    /* power control logging enable   */ /* MOD064 */
#define CGBC_LOG_POST_ENABLE 0x00000002    /* POST code logging enable       */
#define CGBC_LOG_GPIO_ENABLE 0x00000001    /* MCU pin logging enable         */

/* ----------------------------------------
 * CGBC_CMD_LOG_READ command definitions
 * ------------------------------------- */

/* command package structure */
typedef struct
CGBC_CMD_LOG_READ_IN_STRUCT
{
  uint8_t ui8Cmd;                            /* CGBC_CMD_LOG_READ            */
  uint8_t ui8First;                          /* 1 = 1st read / 0 = next read */
} CGBC_CMD_LOG_READ_IN, *P_CGBC_CMD_LOG_READ_IN;

/* result package structure */
typedef struct
CGBC_CMD_LOG_READ_OUT_STRUCT
{
  CGBC_STATUS stsCmdSts;                                 /* CGBC status byte */
  uint8_t    aui8Data[8];                                /* log entry        */
} CGBC_CMD_LOG_READ_OUT, *P_CGBC_CMD_LOG_READ_OUT;

/* -----------------------------------------
 * CGBC_CMD_LOG_WRITE command definitions
 * -------------------------------------- */

/* command package structure */
typedef struct
CGBC_CMD_LOG_WRITE_IN_STRUCT
{
  uint8_t ui8Cmd;                                  /* CGBC_CMD_LOG_WRITE     */
  uint8_t ui8Ctrl;                                 /* log write control byte */
  uint8_t aui8Data[7];                             /* log write data         */
} CGBC_CMD_LOG_WRITE_IN, *P_CGBC_CMD_LOG_WRITE_IN;
                                                                      //MOD046v
/* log write control byte bit mapping */
#define CGBC_LOG_WRITE_RES_M    0xE0      /* reserved / must be zero         */

#define CGBC_LOG_WRITE_TSE_M    0x18      /* time stamp enable mask          */
#define  CGBC_LOG_WRITE_TSE_NONE 0x00     /* no time stamp                   */
#define  CGBC_LOG_WRITE_TSE_LOG  0x08     /* logging time stamp              */
#define  CGBC_LOG_WRITE_TSE_SYS  0x10     /* system time stamp               */

#define CGBC_LOG_WRITE_SIZE_M   0x07      /* log write data size mask        */
#define CGBC_LOG_WRITE_SIZE_S   0         /* log write data size shift count */
                                                                      //MOD046^
/* ----------------------------------------
 * CGBC_CMD_LOG_GPIO command definitions
 * ------------------------------------- */

/* command package structure */
typedef struct
CGBC_CMD_LOG_GPIO_IN_STRUCT
{
  uint8_t ui8Cmd;                    /* CGBC_CMD_LOG_GPIO                    */
  uint8_t ui8CtlSts;                 /* GPIO pin logging control/status byte */
  uint8_t ui8PinNum;                 /* target GPIO pin number (0-255)       */
} CGBC_CMD_LOG_GPIO_IN, *P_CGBC_CMD_LOG_GPIO_IN;

/* result package structure */
typedef struct
CGBC_CMD_LOG_GPIO_OUT_STRUCT
{
  CGBC_STATUS stsCmdSts;             /* CGBC status byte                     */
  uint8_t     ui8CtlSts;             /* GPIO pin logging control/status byte */
  uint8_t     ui8PinNum;             /* target GPIO pin number (0-255)       */
} CGBC_CMD_LOG_GPIO_OUT, *P_CGBC_CMD_LOG_GPIO_OUT;

/* GPIO pin logging control/status byte bit mapping */
#define CGBC_LOG_GPIO_CUP   0x80          /* current config update request   */
#define CGBC_LOG_GPIO_EUP   0x40          /* EEPROM parameter update request */
#define CGBC_LOG_GPIO_RES_M 0x20          /* reserved / must be zero         */
#define CGBC_LOG_GPIO_ENA   0x10          /* logging enable                  */
#define CGBC_LOG_GPIO_STT   0x08          /* logging start trigger           */
#define CGBC_LOG_GPIO_SPT   0x04          /* logging stop trigger            */
#define CGBC_LOG_GPIO_FEE   0x02          /* falling edge enable             */
#define CGBC_LOG_GPIO_REE   0x01          /* rising edge enable              */

/* ------------------------------------------
 * CGBC_CMD_API_CONFIG command definitions
 * --------------------------------------- */

/* command package structure */
typedef struct
CGBC_CMD_API_CONFIG_IN_STRUCT
{    
  uint8_t ui8Cmd;                                   /* CGBC_CMD_API_CONFIG   */
  uint8_t ui8SubCmd;                                /* sub-command           */
  uint8_t ui8Parm;                                  /* sub-command parameter */
} CGBC_CMD_API_CONFIG_IN, *P_CGBC_CMD_API_CONFIG_IN;

/* CGBC_CMD_API_CONFIG sub-commands */
#define CGBC_CMD_API_CMD_DISABLE     0x00
#define CGBC_CMD_API_SET_RESET_EVENT 0x01

/* CGBC_CMD_API_SET_RESET_EVENT sub-command parameter */
#define CGBC_API_RESET_MSK     0x03
#define CGBC_API_RESET_SYS_RST  0x00
#define CGBC_API_RESET_S5       0x01
#define CGBC_API_RESET_G3       0x02
#define CGBC_API_RESET_NEVER    0x03

/*---------------------------------------
 * CGBC_CMD_COND_RTM command definitions
 *---------------------------------------
 */
                                                                      //MOD045v
#ifndef __ASSEMBLER__

typedef struct
CON_RTM_PARAM_STRUCT          /* conditional parameter structure             */
{
 uint32_t conRunTimMaxTemp;   /* conditional running time meter max temp     */
 uint32_t conRunTimMinTemp;   /* conditional running time meter min temp     */
 uint32_t conRunTimMaxVolt;   /* conditional running time meter max volt     */
 uint32_t conRunTimMinVolt;   /* conditional running time meter min volt     */
 uint32_t conRunTimBiosSet;   /* conditional running time meter BIOS setting */
}CON_RTM_PARAM;
 
typedef struct
CGBC_CMD_CON_RTM_IN_STRUCT           /* command in packet structure          */
{
  uint8_t cmd;                       /* CGBC_CMD_CON_RTM                     */
  uint8_t subCmd;                    /* Sub command:
                                      * Stop:  0x00
                                      * Start: 0x01
                                      * Read:  0x02
                                      * Reset: 0x03                          
                                      * Burn:  0x04                          */
  CON_RTM_PARAM param;               /* parameters under which condition the
                                      * running time should be logged        */
} CGBC_CMD_CON_RTM_IN, *P_CGBC_CMD_CON_RTM_IN;

typedef struct
CGBC_CMD_CON_RTM_OUT_STRUCT                  /* command out packet structure */
{
  uint8_t  sts;                              /* CGBC_CMD_CON_RTM             */
  uint8_t  conRunTimSts;                     /* Timer status                 */
  uint16_t conRunTimVolt;                    /* running time over under volt */
  uint16_t conRunTimTemp;                    /* running time over under temp */
  uint16_t conRunTimSet;                     /* running time setup node act  */
  CON_RTM_PARAM param;                       /* parameter out                */
} CGBC_CMD_CON_RTM_OUT, *P_CGBC_CMD_CON_RTM_OUT;

#endif //!__ASSEMBLER__
                                                                      //MOD045^


/* ----------------------------------------------
 * CGBC_CMD_BLTx_PWM_DELAY command definitions
 * ------------------------------------------- */

/* command package structure  */
typedef struct
CGBC_CMD_BLT_PWM_DELAY_IN_STRUCT
{
  uint8_t ui8Cmd;                                 /* CGBC_CMD_BLTx_PWM_DELAY */
  int8_t   i8Dly5ms;                              /* delay in units of 5ms   */
} CGBC_CMD_BLT_PWM_DELAY_IN, *P_CGBC_CMD_BLT_PWM_DELAY_IN;

/* result package structure */
typedef struct
CGBC_CMD_BLT_PWM_DELAY_OUT_STRUCT
{
  CGBC_STATUS stsCmdSts;                          /* CGBC status byte        */
   int8_t      i8Dly5ms;                          /* delay in units of 5ms   */
} CGBC_CMD_BLT_PWM_DELAY_OUT, *P_CGBC_CMD_BLT_PWM_DELAY_OUT;

#define CGBC_BLT_PWM_DELAY_READ -128     /* reserved value for read function */

/* ----------------------------------------
 * CGBC_CMD_PIN_FEAT command definitions
 * ------------------------------------- */
                                                                      //MOD046v
/* command package structure  */
typedef struct
CGBC_CMD_PIN_FEAT_IN_STRUCT
{
  uint8_t   ui8Cmd;                   /* CGBC_CMD_PIN_FEAT                   */
  uint8_t   ui8PhyPinVirSta;          /* phys. pin number or virt. pin state */
  uint16_t ui16PinCfgLogFnc;          /* pin config and/or logical pin func  */
} CGBC_CMD_PIN_FEAT_IN, *P_CGBC_CMD_PIN_FEAT_IN;

/* result package structure */
typedef struct
CGBC_CMD_PIN_FEAT_OUT_STRUCT
{
  CGBC_STATUS stsCmdSts;              /* CGBC status byte                    */
  uint8_t     ui8PhyPinVirSta;        /* phys. pin number or virt. pin state */
  uint16_t   ui16PinCfgLogFnc;        /* pin config and/or logical pin func  */
} CGBC_CMD_PIN_FEAT_OUT, *P_CGBC_CMD_PIN_FEAT_OUT;

/* pin configuration / detailed physical pin characteristics */
#define CGBC_PIN_CFG_MSK 0xFF00
#define CGBC_PIN_CFG_S   8
                                                                      //MOD046^
#define CGBC_PIN_PWR_MSK 0xC000
#define  CGBC_PIN_PWR_DSW 0xC000 /* deep standby powered */
#define  CGBC_PIN_PWR_STB 0x8000 /* standby powered */
#define  CGBC_PIN_PWR_S0  0x4000 /* runtime powered */
#define  CGBC_PIN_PWR_RES 0x0000 /* reserved */

#define CGBC_PIN_TYP_MSK 0x3000
#define  CGBC_PIN_TYP_RES 0x3000 /* reserved */
#define  CGBC_PIN_TYP_ANA 0x2000 /* analog pad */
#define  CGBC_PIN_TYP_DIG 0x1000 /* digital pad */
#define  CGBC_PIN_TYP_OFF 0x0000 /* pad off or tri-stated */

#define CGBC_PIN_LOW_MSK 0x0800
#define  CGBC_PIN_LOW_ENA 0x0800 /* output pin driven low upon logical 0 */
#define  CGBC_PIN_LOW_DIS 0x0000 /* output pin tri-stated upon logical 0 */

#define CGBC_PIN_PUP_MSK 0x0800
#define  CGBC_PIN_PUP_ENA 0x0800 /* pull-up enabled */
#define  CGBC_PIN_PUP_DIS 0x0000 /* pull-up disabled */
                                                                      //MOD054v
#define CGBC_PIN_ODV_MSK 0x0800
#define CGBC_PIN_ODV_FIX  0x0800 /* fixed voltage open-drain output pin */
#define CGBC_PIN_ODV_TOL  0x0000 /* voltage tolerant open-drain output pin */
                                                                      //MOD054^
#define CGBC_PIN_MOD_MSK 0x0400
#define  CGBC_PIN_MOD_OD  0x0400 /* open-drain mode */
#define  CGBC_PIN_MOD_PP  0x0000 /* push-pull mode */

#define CGBC_PIN_PDN_MSK 0x0400
#define  CGBC_PIN_PDN_ENA 0x0400 /* pull-down enabled */
#define  CGBC_PIN_PDN_DIS 0x0000 /* pull-down disabled */

#define CGBC_PIN_DIR_MSK 0x0200
#define  CGBC_PIN_DIR_OUT 0x0200 /* output pin */
#define  CGBC_PIN_DIR_INP 0x0000 /* input pin */

#define CGBC_PIN_LVL_MSK 0x0100
#define  CGBC_PIN_LVL_HI  0x0100 /* initial output level high */
#define  CGBC_PIN_LVL_LO  0x0000 /* initial output level low */
                                                                      //MOD046v
/* pin configuration / logical pin functions */
#define CGBC_PIN_FNC_MSK 0x00FF
#define CGBC_PIN_FNC_S   0
#define  CGBC_PIN_FNC_NONE           0x00 /* unused pin                      */
#define  CGBC_PIN_FNC_EC_NMIn        0x01 /* EC NMI output                   */
#define  CGBC_PIN_FNC_EC_SMIn        0x02 /* EC SMI output                   */
#define  CGBC_PIN_FNC_EC_SCIn        0x03 /* EC SCI output                   */
#define  CGBC_PIN_FNC_SER0_RXD       0x04 /* system serial port 0 RXD        */
#define  CGBC_PIN_FNC_SER0_TXD       0x05 /* system serial port 0 TXD        */
#define  CGBC_PIN_FNC_SER0_RTS       0x06 /* system serial port 0 RTS        */
#define  CGBC_PIN_FNC_SER0_CTS       0x07 /* system serial port 0 CTS        */
#define  CGBC_PIN_FNC_SER1_RXD       0x08 /* system serial port 1 RXD        */
#define  CGBC_PIN_FNC_SER1_TXD       0x09 /* system serial port 1 TXD        */
#define  CGBC_PIN_FNC_SER1_RTS       0x0A /* system serial port 1 RTS        */
#define  CGBC_PIN_FNC_SER1_CTS       0x0B /* system serial port 1 CTS        */
#define  CGBC_PIN_FNC_SERA_RXD       0x0C /* auxiliary serial port RXD       */
#define  CGBC_PIN_FNC_SERA_TXD       0x0D /* auxiliary serial port TXD       */
#define  CGBC_PIN_FNC_SERA_RTS       0x0E /* auxiliary serial port RTS       */
#define  CGBC_PIN_FNC_SERA_CTS       0x0F /* auxiliary serial port CTS       */
#define  CGBC_PIN_FNC_USER_GPIO_0    0x10 /* user GPIO pin 0                 */
#define  CGBC_PIN_FNC_USER_GPIO_1    0x11 /* user GPIO pin 1                 */
#define  CGBC_PIN_FNC_USER_GPIO_2    0x12 /* user GPIO pin 2                 */
#define  CGBC_PIN_FNC_USER_GPIO_3    0x13 /* user GPIO pin 3                 */
#define  CGBC_PIN_FNC_USER_GPIO_4    0x14 /* user GPIO pin 4                 */
#define  CGBC_PIN_FNC_USER_GPIO_5    0x15 /* user GPIO pin 5                 */
#define  CGBC_PIN_FNC_USER_GPIO_6    0x16 /* user GPIO pin 6                 */
#define  CGBC_PIN_FNC_USER_GPIO_7    0x17 /* user GPIO pin 7                 */
#define  CGBC_PIN_FNC_USER_GPIO_8    0x18 /* user GPIO pin 8                 */
#define  CGBC_PIN_FNC_USER_GPIO_9    0x19 /* user GPIO pin 9                 */
#define  CGBC_PIN_FNC_USER_GPIO_10   0x1A /* user GPIO pin 10                */
#define  CGBC_PIN_FNC_USER_GPIO_11   0x1B /* user GPIO pin 11                */
#define  CGBC_PIN_FNC_USER_GPIO_12   0x1C /* user GPIO pin 12                */
#define  CGBC_PIN_FNC_USER_GPIO_13   0x1D /* user GPIO pin 13                */
#define  CGBC_PIN_FNC_USER_GPIO_14   0x1E /* user GPIO pin 14                */
#define  CGBC_PIN_FNC_USER_GPIO_15   0x1F /* user GPIO pin 15                */
#define  CGBC_PIN_FNC_USER_GPIO_16   0x20 /* user GPIO pin 16                */
#define  CGBC_PIN_FNC_USER_GPIO_17   0x21 /* user GPIO pin 17                */
#define  CGBC_PIN_FNC_USER_GPIO_18   0x22 /* user GPIO pin 18                */
#define  CGBC_PIN_FNC_USER_GPIO_19   0x23 /* user GPIO pin 19                */
#define  CGBC_PIN_FNC_USER_GPIO_20   0x24 /* user GPIO pin 20                */
#define  CGBC_PIN_FNC_USER_GPIO_21   0x25 /* user GPIO pin 21                */
#define  CGBC_PIN_FNC_USER_GPIO_22   0x26 /* user GPIO pin 22                */
#define  CGBC_PIN_FNC_USER_GPIO_23   0x27 /* user GPIO pin 23                */
#define  CGBC_PIN_FNC_USER_GPIO_24   0x28 /* user GPIO pin 24                */
#define  CGBC_PIN_FNC_USER_GPIO_25   0x29 /* user GPIO pin 25                */
#define  CGBC_PIN_FNC_USER_GPIO_26   0x2A /* user GPIO pin 26                */
#define  CGBC_PIN_FNC_USER_GPIO_27   0x2B /* user GPIO pin 27                */
#define  CGBC_PIN_FNC_USER_GPIO_28   0x2C /* user GPIO pin 28                */
#define  CGBC_PIN_FNC_USER_GPIO_29   0x2D /* user GPIO pin 29                */
#define  CGBC_PIN_FNC_USER_GPIO_30   0x2E /* user GPIO pin 30                */
#define  CGBC_PIN_FNC_USER_GPIO_31   0x2F /* user GPIO pin 31                */
#define  CGBC_PIN_FNC_DDC0_SDA       0x30 /* display data channel 0 data     */
#define  CGBC_PIN_FNC_DDC0_SCL       0x31 /* display data channel 0 clock    */
#define  CGBC_PIN_FNC_DDC0_ALERTn    0x32 /* display data channel 0 alert    */
#define  CGBC_PIN_FNC_I2C0_SDA       0x33 /* primary I2C-bus 0 data          */
#define  CGBC_PIN_FNC_I2C0_SCL       0x34 /* primary I2C-bus 0 clock         */
#define  CGBC_PIN_FNC_I2C0_ALERTn    0x35 /* primary I2C-bus 0 alert         */
#define  CGBC_PIN_FNC_SMB0_SDA       0x36 /* primary SMBus 0 data            */
#define  CGBC_PIN_FNC_SMB0_SCL       0x37 /* primary SMBus 0 clock           */
#define  CGBC_PIN_FNC_SMB0_ALERTn    0x38 /* primary SMBus 0 alert           */
#define  CGBC_PIN_FNC_SMB1_SDA       0x39 /* secondary SMBus 1 data          */
#define  CGBC_PIN_FNC_SMB1_SCL       0x3A /* secondary SMBus 1 clock         */
#define  CGBC_PIN_FNC_SMB1_ALERTn    0x3B /* secondary SMBus 1 alert         */
#define  CGBC_PIN_FNC_SMB2_SDA       0x3C /* auxiliary SMBus 2 data          */
#define  CGBC_PIN_FNC_SMB2_SCL       0x3D /* auxiliary SMBus 2 clock         */
#define  CGBC_PIN_FNC_SMB2_ALERTn    0x3E /* auxiliary SMBus 2 alert         */
#define  CGBC_PIN_FNC_SMB_ISOn       0x3F /* SMBus isolation control         */
#define  CGBC_PIN_FNC_LIDBTNn        0x40 /* lid button                      */
#define  CGBC_PIN_FNC_PWRBTNn_INP    0x41 /* power button input              */
#define  CGBC_PIN_FNC_PWRBTNn_OUT    0x42 /* power button output             */
#define  CGBC_PIN_FNC_RSTBTNn_INP    0x43 /* reset button input              */
#define  CGBC_PIN_FNC_RSTBTNn_OUT    0x44 /* reset button output             */
#define  CGBC_PIN_FNC_SLPBTNn        0x45 /* sleep button                    */
#define  CGBC_PIN_FNC_FP_LED_0       0x46 /* front panel LED 0               */
#define  CGBC_PIN_FNC_FP_LED_1       0x47 /* front panel LED 1               */
#define  CGBC_PIN_FNC_IINA_INP       0x48 /* analog current sensor input     */
#define  CGBC_PIN_FNC_HI_TEMPA_INP   0x49 /* analog high temp. sensor input  */
#define  CGBC_PIN_FNC_LO_TEMPA_INP   0x4A /* analog low temp. sensor input   */
#define  CGBC_PIN_FNC_VINA_INP       0x4B /* analog runtime voltage input    */
#define  CGBC_PIN_FNC_VSBA_INP       0x4C /* analog standby voltage input    */
#define  CGBC_PIN_FNC_GPA_INP        0x4D /* general purpose analog input    */
#define  CGBC_PIN_FNC_WDG_EVENT      0x4E /* watchdog event output pin       */
#define  CGBC_PIN_FNC_WDG_STROBEn    0x4F /* watchdog strobe input pin       */
#define  CGBC_PIN_FNC_LFP0_BLT_INP   0x50 /* LFP0 backlight ctrl input pin   */
#define  CGBC_PIN_FNC_LFP0_BLT_OUT   0x51 /* LFP0 backlight ctrl output pin  */
#define  CGBC_PIN_FNC_LFP0_PWM_OUT   0x52 /* LFP0 backlight PWM output pin   */
#define  CGBC_PIN_FNC_LFP0_VDD_INP   0x53 /* LFP0 power ctrl input pin       */
#define  CGBC_PIN_FNC_LFP0_VDD_OUT   0x54 /* LFP0 power ctrl output pin      */
#define  CGBC_PIN_FNC_LFP1_BLT_INP   0x55 /* LFP1 backlight ctrl input pin   */
#define  CGBC_PIN_FNC_LFP1_BLT_OUT   0x56 /* LFP1 backlight ctrl output pin  */
#define  CGBC_PIN_FNC_LFP1_PWM_OUT   0x57 /* LFP1 backlight PWM output pin   */
#define  CGBC_PIN_FNC_LFP1_VDD_INP   0x58 /* LFP1 power ctrl input pin       */
#define  CGBC_PIN_FNC_LFP1_VDD_OUT   0x59 /* LFP1 power ctrl output pin      */
#define  CGBC_PIN_FNC_SYS_CFG0       0x5A /* system configuration pin 0      */
#define  CGBC_PIN_FNC_SYS_CFG1       0x5B /* system configuration pin 1      */
#define  CGBC_PIN_FNC_SYS_CFG2       0x5C /* system configuration pin 2      */
#define  CGBC_PIN_FNC_SYS_CFG3       0x5D /* system configuration pin 3      */
#define  CGBC_PIN_FNC_SYS_CFG4       0x5E /* system configuration pin 4      */
#define  CGBC_PIN_FNC_SYS_CFG5       0x5F /* system configuration pin 5      */
#define  CGBC_PIN_FNC_CPU_FAN_TACH   0x60 /* CPU fan tachometer input        */
#define  CGBC_PIN_FNC_CPU_FAN_PWM    0x61 /* CPU fan PWM output              */
#define  CGBC_PIN_FNC_SYS_FAN_TACH   0x62 /* system fan tachometer input     */
#define  CGBC_PIN_FNC_SYS_FAN_PWM    0x63 /* system fan PWM output           */
#define  CGBC_PIN_FNC_PECI_DAT       0x64 /* bidirectional PECI pin          */
#define  CGBC_PIN_FNC_PECI_RXD       0x65 /* PECI input pin                  */
#define  CGBC_PIN_FNC_PECI_TXD       0x66 /* PECI output pin                 */
#define  CGBC_PIN_FNC_VREF_VTT       0x67 /* CPU VTT voltage reference pin   */
#define  CGBC_PIN_FNC_CHARGINGn      0x68 /* battery charging                */
#define  CGBC_PIN_FNC_CHARGER_PRSNTn 0x69 /* charger present                 */
#define  CGBC_PIN_FNC_BATLOWn        0x6A /* BATLOW# pin                     */
#define  CGBC_PIN_FNC_ACPRESENT      0x6B /* ACPRESENT pin                   */
#define  CGBC_PIN_FNC_SLP_SUSn       0x6C /* SLP_SUS# pin                    */
#define  CGBC_PIN_FNC_SUS_PGD        0x6D /* SUS_PGD pin                     */
#define  CGBC_PIN_FNC_SUSWARNn       0x6E /* SUSWARN# pin                    */
#define  CGBC_PIN_FNC_SUSACKn        0x6F /* SUSACK# pin                     */
#define  CGBC_PIN_FNC_RSMRSTn        0x70 /* RSMRST# pin                     */
#define  CGBC_PIN_FNC_SLP_S5n        0x71 /* SLP_S3# pin                     */
#define  CGBC_PIN_FNC_SLP_S4n        0x72 /* SLP_S4# pin                     */
#define  CGBC_PIN_FNC_SLP_S3n        0x73 /* SLP_S5# pin                     */
#define  CGBC_PIN_FNC_SYS_PGD_INP    0x74 /* SYS_PGD_INP pin                 */
#define  CGBC_PIN_FNC_SYS_PGD_OUT    0x75 /* SYS_PGD_OUT pin                 */
#define  CGBC_PIN_FNC_PLTRSTn        0x76 /* PLTRST# pin                     */
                                                                      //MOD048v
#define  CGBC_PIN_FNC_V5A_CTL        0x77 /* V5A_CTL pin                     */
#define  CGBC_PIN_FNC_HOST_RST_ACK   0x78 /* HOST_RST_ACK pin                */
                                                                      //MOD052v
#define  CGBC_PIN_FNC_I2C1_SDA       0x79 /* secondary I2C-bus 1 data        */
#define  CGBC_PIN_FNC_I2C1_SCL       0x7A /* secondary I2C-bus 1 clock       */
#define  CGBC_PIN_FNC_I2C1_ALERTn    0x7B /* secondary I2C-bus 1 alert       */
#define  CGBC_PIN_FNC_IPMB_SDA       0x7C /* int. platform mgmt bus data     */
#define  CGBC_PIN_FNC_IPMB_SCL       0x7D /* int. platform mgmt bus clock    */
#define  CGBC_PIN_FNC_IPMB_ALERTn    0x7E /* int. platform mgmt bus alert    */
#define  CGBC_PIN_FNC_UPDB_SDA       0x7F /* USB PD controller bus data      */
#define  CGBC_PIN_FNC_UPDB_SCL       0x80 /* USB PD controller bus clock     */
#define  CGBC_PIN_FNC_UPDB_ALERTn    0x81 /* USB PD controller bus alert     */
#define  CGBC_PIN_FNC_PMBUS_MUX      0x82 /* PMBUS multiplexer control pin   */
#define  CGBC_PIN_FNC_TESTn          0x83 /* system test mode pin            */
#define  CGBC_PIN_FNC_VIN_PWROK      0x84 /* VIN power good pin              */
#define  CGBC_PIN_FNC_RAPID_SHUTDOWN 0x85 /* rapid shutdown pin              */
#define  CGBC_PIN_FNC_UART_MUX_PCHn  0x86 /* cBC/PCH UART mux control pin    */
#define  CGBC_PIN_FNC_THRMn          0x87 /* THRM# pin                       */
#define  CGBC_PIN_FNC_PROCHOTn       0x88 /* PROCHOT# pin                    */
                                                                      //MOD060v
#define  CGBC_PIN_FNC_SPD_SMB_MUX    0x89 /* SPD_SMB_MUX pin                 */
                                                                      //MOD063v
#define  CGBC_PIN_FNC_GSPI_INTn      0x8A /* GSPI client interrupt pin       */
#define  CGBC_PIN_FNC_GSPI_DSEL      0x8B /* GSPI device selection pin       */
                                                                      //MOD068v
#define  CGBC_PIN_FNC_SUSPWRDNACK    0x8C /* SUSPWRDNACK pin                 */
#define  CGBC_PIN_FNC_SLP_Mn         0x8D /* SLP_M# pin                      */
#define  CGBC_PIN_FNC_SLP_LANn       0x8E /* SLP_LAN# pin                    */
#define  CGBC_PIN_FNC_SLP_WLANn      0x8F /* SLP_WLAN# pin                   */
                                                                      //MOD068^
                                                                      //MOD069v
#define  CGBC_PIN_FNC_BRD_PWR_EN     0x90 /* Board power enable pin          */
#define  CGBC_PIN_FNC_MAX            CGBC_PIN_FNC_BRD_PWR_EN
                                          //MOD048 MOD052 MOD060 MOD063 MOD069^
/* reserved logical pin functions */
#define  CGBC_PIN_FNC_RES0           0xE0 /* reserved logical pin function 0 */
#define  CGBC_PIN_FNC_RES1           0xE1 /* reserved logical pin function 1 */
#define  CGBC_PIN_FNC_RES2           0xE2 /* reserved logical pin function 2 */
#define  CGBC_PIN_FNC_RES3           0xE3 /* reserved logical pin function 3 */
#define  CGBC_PIN_FNC_RES4           0xE4 /* reserved logical pin function 4 */
#define  CGBC_PIN_FNC_RES5           0xE5 /* reserved logical pin function 5 */
#define  CGBC_PIN_FNC_RES6           0xE6 /* reserved logical pin function 6 */
#define  CGBC_PIN_FNC_RES7           0xE7 /* reserved logical pin function 0 */
#define  CGBC_PIN_FNC_RES8           0xE8 /* reserved logical pin function 0 */
#define  CGBC_PIN_FNC_RES9           0xE9 /* reserved logical pin function 0 */
#define  CGBC_PIN_FNC_RES10          0xEA /* reserved logical pin function 0 */
#define  CGBC_PIN_FNC_RES11          0xEB /* reserved logical pin function 0 */
#define  CGBC_PIN_FNC_RES12          0xEC /* reserved logical pin function 0 */
#define  CGBC_PIN_FNC_RES13          0xED /* reserved logical pin function 0 */
#define  CGBC_PIN_FNC_RES14          0xEE /* reserved logical pin function 0 */
#define  CGBC_PIN_FNC_RES15          0xEF /* reserved logical pin function 0 */

/* logical pin function groups */
#define  CGBC_PIN_FNC_LPC            0xF0 /* LPC pin function group          */
#define  CGBC_PIN_FNC_ESPI           0xF1 /* eSPI pin function group         */
#define  CGBC_PIN_FNC_GPSPI_0        0xF2 /* GPSPI 0 function group          */
#define  CGBC_PIN_FNC_QSPI_PVT       0xF3 /* private QSPI pin function group */
#define  CGBC_PIN_FNC_QSPI_SHD       0xF4 /* shared QSPI pin function group  */

/* virtual pin states */
#define  CGBC_PIN_VW_LOW             0xFA /* eSPI virtual wire in low state  */
#define  CGBC_PIN_VW_HIGH            0xFB /* eSPI virtual wire in high state */
#define  CGBC_PIN_VP_LOW             0xFC /* virtual pin in low state        */
#define  CGBC_PIN_VP_HIGH            0xFD /* virtual pin in high state       */
#define  CGBC_PIN_NONE               0xFF /* unavailable pin                 */
                                                                      //MOD046^
/* combined pin features */
                                                                      //MOD054v
#define CGBC_PIN_PAD_MSK        (CGBC_PIN_TYP_MSK|CGBC_PIN_ODV_MSK|CGBC_PIN_LOW_MSK|CGBC_PIN_PUP_MSK|CGBC_PIN_PDN_MSK|CGBC_PIN_DIR_MSK|CGBC_PIN_MOD_MSK|CGBC_PIN_LVL_MSK)
#define CGBC_PIN_TRI            (CGBC_PIN_TYP_OFF|        0       |        0       |        0       |        0       |        0       |        0       |        0       )
#define CGBC_PIN_ANA_INP        (CGBC_PIN_TYP_ANA|        0       |        0       |        0       |        0       |        0       |        0       |        0       )
#define CGBC_PIN_DIG_INP_FLOAT  (CGBC_PIN_TYP_DIG|        0       |        0       |CGBC_PIN_PUP_DIS|CGBC_PIN_PDN_DIS|CGBC_PIN_DIR_INP|        0       |        0       )
#define CGBC_PIN_DIG_INP_PD     (CGBC_PIN_TYP_DIG|        0       |        0       |CGBC_PIN_PUP_DIS|CGBC_PIN_PDN_ENA|CGBC_PIN_DIR_INP|        0       |        0       )
#define CGBC_PIN_DIG_INP_PU     (CGBC_PIN_TYP_DIG|        0       |        0       |CGBC_PIN_PUP_ENA|CGBC_PIN_PDN_DIS|CGBC_PIN_DIR_INP|        0       |        0       )
#define CGBC_PIN_DIG_OUT_OD_HI  (CGBC_PIN_TYP_DIG|CGBC_PIN_ODV_FIX|        0       |        0       |        0       |CGBC_PIN_DIR_OUT|CGBC_PIN_MOD_OD |CGBC_PIN_LVL_HI )
#define CGBC_PIN_DIG_OUT_OD_LO  (CGBC_PIN_TYP_DIG|CGBC_PIN_ODV_FIX|        0       |        0       |        0       |CGBC_PIN_DIR_OUT|CGBC_PIN_MOD_OD |CGBC_PIN_LVL_LO )
#define CGBC_PIN_DIG_OUT_ODT_HI (CGBC_PIN_TYP_DIG|CGBC_PIN_ODV_TOL|        0       |        0       |        0       |CGBC_PIN_DIR_OUT|CGBC_PIN_MOD_OD |CGBC_PIN_LVL_HI )
#define CGBC_PIN_DIG_OUT_ODT_LO (CGBC_PIN_TYP_DIG|CGBC_PIN_ODV_TOL|        0       |        0       |        0       |CGBC_PIN_DIR_OUT|CGBC_PIN_MOD_OD |CGBC_PIN_LVL_LO )
#define CGBC_PIN_DIG_OUT_PP_HI  (CGBC_PIN_TYP_DIG|        0       |CGBC_PIN_LOW_ENA|        0       |        0       |CGBC_PIN_DIR_OUT|CGBC_PIN_MOD_PP |CGBC_PIN_LVL_HI )
#define CGBC_PIN_DIG_OUT_PP_LO  (CGBC_PIN_TYP_DIG|        0       |CGBC_PIN_LOW_ENA|        0       |        0       |CGBC_PIN_DIR_OUT|CGBC_PIN_MOD_PP |CGBC_PIN_LVL_LO )
#define CGBC_PIN_DIG_OUT_LZ_HI  (CGBC_PIN_TYP_DIG|        0       |CGBC_PIN_LOW_DIS|        0       |        0       |CGBC_PIN_DIR_OUT|CGBC_PIN_MOD_PP |CGBC_PIN_LVL_HI )
#define CGBC_PIN_DIG_OUT_LZ_LO  (CGBC_PIN_TYP_DIG|        0       |CGBC_PIN_LOW_DIS|        0       |        0       |CGBC_PIN_DIR_OUT|CGBC_PIN_MOD_PP |CGBC_PIN_LVL_LO )
                                                                      //MOD054^
                                                                      //MOD063v
/* ----------------------------------------
 * CGBC_CMD_GSPI_CFG command definitions
 * ------------------------------------- */

/* command package structure */
typedef struct
CGBC_CMD_GSPI_CFG_IN_STRUCT
{
  uint8_t   ui8Cmd;         /* CGBC_CMD_GSPI_CFG                             */
  uint8_t   ui8Cfg;         /* GSPI configuration byte. Including byte order,
                             * operation mode, and duplex                    */
  uint8_t   ui8SerIrqNum;   /* SERIRQ number which shall be allocated for the
                             * GSPI transfer finished or GSPI client interrupt
                             * SERIRQ to the host.                           */
  uint32_t ui32Freq;        /* GSPI clock frequency to be configured. Any
                             * frequency in Hz can be used. The configuration
                             * routine automatically configures the next lowest
                             * frequency which is supported by the controller*/
  uint32_t ui32GspiSramBar;
} CGBC_CMD_GSPI_CFG_IN, *P_CGBC_CMD_GSPI_CFG_IN;

#define CGBC_GSPI_CFG_DUPLEX_POS                       0
#define CGBC_GSPI_CFG_DUPLEX_MASK                   0x03
#define CGBC_GSPI_CFG_DUPLEX_FULL                   0x00
#define CGBC_GSPI_CFG_DUPLEX_HALF                   0x01
#define CGBC_GSPI_CFG_DUPLEX_DUAL                   0x02

#define CGBC_GSPI_GFG_MODE_POS                         2
#define CGBC_GSPI_GFG_MODE_MASK                     0x0C
#define CGBC_GSPI_CFG_MODE_00                       0x00
#define CGBC_GSPI_CFG_MODE_01                       0x04
#define CGBC_GSPI_CFG_MODE_10                       0x08
#define CGBC_GSPI_CFG_MODE_11                       0x0C

#define CGBC_GSPI_CFG_ENDIAN_POS                       4
#define CGBC_GSPI_CFG_ENDIAN_MASK                   0x10
#define CGBC_GSPI_CFG_ENDIAN_LIT                    0x00
#define CGBC_GSPI_CFG_ENDIAN_BIG                    0x10

#define CGBC_GSPI_CFG_UPD_POS                          5
#define CGBC_GSPI_CFG_UPD_MASK                      0x20

/* result package structure */
typedef struct
CGBC_CMD_GSPI_CFG_OUT_STRUCT
{
  CGBC_STATUS stsCmdSts;    /* CGBC status byte                              */
  uint8_t   ui8Cfg;         /* Actually configured byte order, operation mode
                             * and duplex in the GSPI controller             */
  uint8_t   ui8SerIrqNum;   /* SERIRQ number allocated for the GSPI support  */
  uint32_t ui32Freq;        /* Actually configured GSPI frequency in the GSPI
                             * controller. It can deviate from the value in
                             * the command because the controller only supports
                             * certain frequencies. The configuration routine
                             * will configure the next lowest frequency to the
                             * desired one and returns iter_swap             */
  uint32_t ui32GspiSramBar;
} CGBC_CMD_GSPI_CFG_OUT, *P_CGBC_CMD_GSPI_CFG_OUT;

#define CGBC_GSPI_CFG_MASK                          0x3F

#define CGBC_GSPI_CLK_FREQ_MASK                     0x0F
#define CGBC_GSPI_CLK_FREQ_15_9kHz                  0x00
#define CGBC_GSPI_CLK_FREQ_333kHz                   0x01
#define CGBC_GSPI_CLK_FREQ_500kHz                   0x02
#define CGBC_GSPI_CLK_FREQ_1MHz                     0x03
#define CGBC_GSPI_CLK_FREQ_381kHz                   0x04
#define CGBC_GSPI_CLK_FREQ_6MHz                     0x05
#define CGBC_GSPI_CLK_FREQ_12MHz                    0x06
#define CGBC_GSPI_CLK_FREQ_24MHz                    0x07
#define CGBC_GSPI_CLK_FREQ_48MHz                    0x08

#define CGBC_GSPI_BYTE_ORDER_MASK                   0x10
#define CGBC_GSPI_BYTE_ORDER_LITTLE_ENDIAN          0x00
#define CGBC_GSPI_BYTE_ORDER_BIG_ENDIAN             0x10

#define CGBC_GSPI_MODE_MASK                         0x20
#define CGBC_GSPI_MODE_00                           0x00
#define CGBC_GSPI_MODE_11                           0x20
                                                                      //MOD063^
/* -----------------------------------------
 * CGBC_CMD_DIAG_MODE command definitions
 * -------------------------------------- */

/* command package structure  */
typedef struct
CGBC_CMD_DIAG_MODE_IN_STRUCT
{
  uint8_t   ui8Cmd;                /* CGBC_CMD_DIAG_MODE                     */
  uint8_t   ui8DiagMode;           /* desired diagnostic console output mode */
  uint16_t ui16Res;                /* reserved / must be 0                   */
  uint32_t ui32FeatId;             /* BC firmware feature ID                 */
} CGBC_CMD_DIAG_MODE_IN, *P_CGBC_CMD_DIAG_MODE_IN;

/* diagnostic console output mode settings */
#define CGBC_DIAG_MODE_OFF     0
#define CGBC_DIAG_MODE_SPECIAL 1
#define CGBC_DIAG_MODE_NORMAL  2
#define CGBC_DIAG_MODE_VERBOSE 3
                                                                      //MOD044^
                                                                      //MOD046 
                                                                      //MOD045v
/*--------------------------------------
 * CGBC_CMD_ADV_USER_LOCK return status
 *--------------------------------------
 */

/* This is done to reuse the definition of the standard user lock if it is
 * de-featured and completely replaced by the advanced user lock.
 */
#ifndef CGBC_USER_LOCK_KEY_SIZE
#define CGBC_USER_LOCK_KEY_SIZE 6
#endif /* CGBC_USER_LOCK_KEY_SIZE */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_ADV_USER_LOCK_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char subCmd;        /* Can configure if read, write
                                                 * or both shall be locked,
                                                 * unlocked or tested
                                                 */
                   unsigned char key[CGBC_USER_LOCK_KEY_SIZE];
                  } CGBC_CMD_ADV_USER_LOCK_IN, *P_CGBC_CMD_ADV_USER_LOCK_IN;

#endif //!__ASSEMBLER__

#ifndef  CGBC_LOCKED
#define  CGBC_LOCKED                0x01        // user lock write is closed
#endif /* !CGBC_LOCKED */

#ifndef  CGBC_TAMPERED
#define  CGBC_TAMPERED              0x02        // someone tampered with lock
#endif /* !CGBC_TAMPERED */

#define CGBC_USER_WRITE_LOCKED      CGBC_LOCKED // redefined for better readability
#define CGBC_USER_READ_LOCKED       0x04        // user lock read is closed

/* Bit mask to store only read and write lock bits in EEPROM but no tampered bit */
#define USER_LOCK_STATUS_EEP_MASK   0x05


                                                                      //MOD046v
/* ------------------------------------------------------------
 * CGBC_CMD_BAT_DYN and CGBC_CMD_BAT_STA command definitions
 * --------------------------------------------------------- */

/* command package structure  */
typedef struct
CGBC_CMD_BAT_IN_STRUCT
{
    uint8_t     ui8Cmd;                                 /* cmd               */
    uint8_t     ui8Battery;                             /* battery to select */
} CGBC_CMD_BAT_IN, *P_CGBC_CMD_BAT_IN;

/* result package structure */
typedef struct
CGBC_CMD_BAT_DYN_DATA_OUT_STRUCT
{
  CGBC_STATUS stsCmdSts;                         /* CGBC status byte         */
   int16_t      i16Current;                      /* Current                  */
  uint16_t     ui16Voltage;                      /* Voltage                  */
  uint16_t     ui16RemainingCapacity;            /* Remaining Capacity       */
  uint16_t     ui16FullChargeCapacity;           /* Full Charge Capacity     */
  uint16_t     ui16Temperature;
  uint16_t     ui16RelStateOfCharge;
  uint16_t     ui16AbsStateOfCharge;
} CGBC_CMD_BAT_DYN_DATA_OUT, *P_CGBC_CMD_BAT_DYN_DATA_OUT;

/* result package structure */
typedef struct
CGBC_CMD_BAT_STA_DATA_OUT_STRUCT
{
  CGBC_STATUS stsCmdSts;                /* CGBC status byte                  */
  uint16_t     ui16StaticValue;         /* Design Cap./Voltage/serialn0      */
  uint16_t     ui16StaticValue2;        /* second value                      */
  char         caStringName[32];        /* Chemistry/Manufacturer/DeviceName */
} CGBC_CMD_BAT_STA_DATA_OUT, *P_CGBC_CMD_BAT_STA_DATA_OUT;



/* ---------------------------------------
 * CGBC_CMD_WDOG_RD command definitions
 * ------------------------------------ */

typedef struct
CGBC_CMD_WDOG_RD_IN_STRUCT
{
  uint8_t ui8Cmd;                   /* CGBC_CMD_WDOG_RD                      */
  uint8_t ui8WdgType;               /* type of watchdog which should be read */
} CGBC_CMD_WDOG_RD_IN, *P_CGBC_CMD_WDOG_RD_IN;

#define CGBC_WDOG                   0x01
#define CGBC_WDOG_POST              0x02
#define CGBC_WDOG_EARLY             0x03



/* --------------------------------------------
 * CGBC_CMD_GPIO_IRQ_TBL command definitions
 * ----------------------------------------- */

/* command package structure  */
typedef struct
CGBC_CMD_GPIO_IRQ_TBL_IN_STRUCT
{
  uint8_t   ui8Cmd;                        /* CGBC_CMD_GPIO_IRQ_TBL          */
  uint8_t   aui8Tbl[32];                   /* user GPIO IRQ table (32 bytes) */
} CGBC_CMD_GPIO_IRQ_TBL_IN, *P_CGBC_CMD_GPIO_IRQ_TBL_IN;

/* special user GPIO IRQ table entries */
#define CGBC_GPIO_IRQ_NONE 0x00
#define CGBC_GPIO_IRQ_READ 0xFF

/* result package structure */
typedef struct
CGBC_CMD_GPIO_IRQ_TBL_OUT_STRUCT
{
  CGBC_STATUS stsCmdSts;                   /* CGBC status byte               */
  uint8_t   aui8Tbl[32];                   /* user GPIO IRQ table (32 bytes) */
} CGBC_CMD_GPIO_IRQ_TBL_OUT, *P_CGBC_CMD_GPIO_IRQ_TBL_OUT;
                                                                      //MOD046^

                                                                      //MOD061v
/* ------------------------------------
 * CGBC_CMD_AVR_SPM_EXT sub-commands
 * --------------------------------- */

#define CGBC_CMD_AVR_SPM_FLS_ADDR 0x00
#define CGBC_CMD_AVR_SPM_FLS_STAT 0x01
#define CGBC_CMD_AVR_SPM_FLS_RD32 0x02
#define CGBC_CMD_AVR_SPM_FLS_WR32 0x03

/* ------------------------------------------------
 * CGBC_CMD_AVR_SPM_FLS_ADDR command definitions
 * --------------------------------------------- */

/* command package structure  */
typedef struct
CGBC_CMD_AVR_SPM_FLS_ADDR_IN_STRUCT
{
  uint8_t   ui8Cmd;                             /* CGBC_CMD_AVR_SPM_EXT      */
  uint8_t   ui8ExtCmd;                          /* CGBC_CMD_AVR_SPM_FLS_ADDR */
  uint32_t ui32FlsAddr;                         /* target flash page address */
} CGBC_CMD_AVR_SPM_FLS_ADDR_IN, *P_CGBC_CMD_AVR_SPM_FLS_ADDR_IN;

/* ------------------------------------------------
 * CGBC_CMD_AVR_SPM_FLS_STAT command definitions
 * --------------------------------------------- */

/* command package structure  */
typedef struct
CGBC_CMD_AVR_SPM_FLS_STAT_IN_STRUCT
{
  uint8_t   ui8Cmd;                             /* CGBC_CMD_AVR_SPM_EXT      */
  uint8_t   ui8ExtCmd;                          /* CGBC_CMD_AVR_SPM_FLS_STAT */
} CGBC_CMD_AVR_SPM_FLS_STAT_IN, *P_CGBC_CMD_AVR_SPM_FLS_STAT_IN;

/* result package structure */
typedef struct
CGBC_CMD_AVR_SPM_FLS_STAT_OUT_STRUCT
{
  CGBC_STATUS stsCmdSts;                      /* CGBC status byte            */
  uint8_t     ui8FlsCtlrSts;                  /* cBC flash controller status */
  uint32_t   ui32FlsPageSiz;                  /* flash page size             */
} CGBC_CMD_AVR_SPM_FLS_STAT_OUT, *P_CGBC_CMD_AVR_SPM_FLS_STAT_OUT;

/* flash controller status definitions */
#define CGBC_AVR_SPM_FLS_IDL 0x00      /* cBC flash controller is idle.      */
#define CGBC_AVR_SPM_FLS_BSY 0x01      /* cBC flash controller is busy.      */
#define CGBC_AVR_SPM_FLS_ERR 0x02      /* cBC flash controller has an error. */

/* ------------------------------------------------
 * CGBC_CMD_AVR_SPM_FLS_RD32 command definitions
 * --------------------------------------------- */

/* command package structure  */
typedef struct
CGBC_CMD_AVR_SPM_FLS_RD32_IN_STRUCT
{
  uint8_t   ui8Cmd;                             /* CGBC_CMD_AVR_SPM_EXT      */
  uint8_t   ui8ExtCmd;                          /* CGBC_CMD_AVR_SPM_FLS_RD32 */
} CGBC_CMD_AVR_SPM_FLS_RD32_IN, *P_CGBC_CMD_AVR_SPM_FLS_RD32_IN;

/* result package structure */
typedef struct
CGBC_CMD_AVR_SPM_FLS_RD32_OUT_STRUCT
{
  CGBC_STATUS stsCmdSts;                                 /* CGBC status byte */
  uint8_t    aui8RdDat[32];                              /* read data        */
} CGBC_CMD_AVR_SPM_FLS_RD32_OUT, *P_CGBC_CMD_AVR_SPM_FLS_RD32_OUT;

/* ------------------------------------------------
 * CGBC_CMD_AVR_SPM_FLS_WR32 command definitions
 * --------------------------------------------- */

/* command package structure  */
typedef struct
CGBC_CMD_AVR_SPM_FLS_WR32_IN_STRUCT
{
  uint8_t   ui8Cmd;                           /* CGBC_CMD_AVR_SPM_EXT        */
  uint8_t   ui8ExtCmd;                        /* CGBC_CMD_AVR_SPM_FLS_WR32 */
  uint8_t  aui8WrDat[32];                     /* write data                  */
} CGBC_CMD_AVR_SPM_FLS_WR32_IN, *P_CGBC_CMD_AVR_SPM_FLS_WR32_IN;
                                                                      //MOD061^
																	  

/*-------------------
 * SYNC return codes
 *-------------------
 */

#define CGBC_SYNC_BYTE0 0xAC
#define CGBC_SYNC_BYTE1 0x53

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_SYNC_OUT_STRUCT
   {
    unsigned char sts;                          // command status
    unsigned char byte0;                        // CGBC_SYNC_BYTE0
    unsigned char byte1;                        // CGBC_SYNC_BYTE1
   } CGBC_CMD_SYNC_OUT, *P_CGBC_CMD_SYNC_OUT;

#endif //!__ASSEMBLER__
                                                                      //MOD045^


/*--------------------
 * CGBD command union
 *--------------------
 */

#define MAX_HST_CMD_SIZ 38

#ifndef __ASSEMBLER__

typedef union CGBC_CMD_STRUCT
               {
                unsigned char              cmd;
                unsigned char              sts;
                unsigned char              cmdPkt[MAX_HST_CMD_SIZ];
                unsigned char              resPkt[CGBC_DAT_CNT_MSK+2];
                CGBC_CMD_FW_REV_OUT        revOut;
                CGBC_CMD_AUX_INIT_IN       auxInitIn;
                CGBC_WD_PARMS              wdParms;
                CGBC_CMD_SYS_FLAGS_1_IN    sysFlags1In;              //MOD021
                CGBC_CMD_SYS_FLAGS_1_OUT   sysFlags1Out;             //MOD021
                CGBC_CMD_INFO_1_OUT        info1Out;
                CGBC_CMD_LFP_CONTROL_IN    lfpControlIn;             //MOD040
                CGBC_CMD_VID_CONTROL_IN    vidControlIn;             //MOD021
                CGBC_CMD_LFP_CONTROL_OUT   lfpControlOut;            //MOD040
                CGBC_CMD_VID_CONTROL_OUT   vidControlOut;            //MOD021
                CGBC_CMD_TCNT_IN           tcntIn;                   //MOD021
                CGBC_CMD_TCNT_OUT          tcntOut;                  //MOD021
                CGBC_CMD_BOOT_TIME_OUT     btimOut;
                CGBC_CMD_BOOT_TIME_OUT_32  btimOut32;                //MOD071
                CGBC_CMD_DUMP_IN           dumpParms;
                CGBC_CMD_RESET_INFO_OUT    resetOut;                 //MOD021
                CGBC_CMD_POST_CODE_IN      postCodeIn;               //MOD040
                CGBC_CMD_XINT_CONTROL_IN   xintControlIn;            //MOD021
                CGBC_CMD_XINT_CONTROL_OUT  xintControlOut;           //MOD021
                CGBC_CMD_USER_LOCK_IN      userLockIn;               //MOD021
                CGBC_CMD_CPU_TEMP_OUT      cpuTempOut;               //MOD021
                CGBC_CMD_SBSM_DATA_OUT     sbsmDataOut;              //MOD021
                CGBC_CMD_I2C_START_IN      i2cStartIn;
                CGBC_CMD_I2C_DATA_OUT      i2cDataOut;
                CGBC_CMD_I2C_SPEED_IN      i2cSpeedIn;               //MOD021
                CGBC_CMD_I2C_SPEED_OUT     i2cSpeedOut;              //MOD021
                CGBC_CMD_BTN_CONTROL_IN    btnControlIn;             //MOD017
                CGBC_CMD_BTN_CONTROL_OUT   btnControlOut;            //MOD017
                CGBC_CMD_FAN_CONTROL_IN    fanControlIn;             //MOD020
                CGBC_CMD_FAN_CONTROL_OUT   fanControlOut;            //MOD020
                CGBC_CMD_RTC_IN            rtcIn;                    //MOD022
                CGBC_CMD_RTC_OUT           rtcOut;                   //MOD022
                CGBC_CMD_GPIO_IN           gpioIn;                   //MOD021
                CGBC_CMD_GPIO_OUT          gpioOut;                  //MOD021
                CGBC_CMD_PECI_IN           peciIn;                   //MOD023
                CGBC_CMD_PECI_OUT          peciOut;                  //MOD023
                CGBC_CMD_I2C_GATE_IN       i2cGateIn;                //MOD024
                CGBC_CMD_I2C_GATE_OUT      i2cGateOut;               //MOD024
                CGBC_CMD_CFG_PINS_IN       cfgPinsIn;                //MOD025
                CGBC_CMD_CFG_PINS_OUT      cfgPinsOut;               //MOD025
                CGBC_CMD_AVR_SPM_IN        avrSpmIn;                 //MOD038
                CGBC_CMD_AVR_SPM_OUT       avrSpmOut;                //MOD038
                CGBC_CMD_COMX_CFG_IN       comxCfgIn;                //MOD031
                CGBC_CMD_COMX_CFG_OUT      comxCfgOut;               //MOD031
                CGBC_CMD_BLT_PWM_IN        bltPwmIn;                 //MOD036
                CGBC_CMD_BLT_PWM_OUT       bltPwmOut;                //MOD036
                CGBC_CMD_DEVICE_ID_OUT     deviceIdOut;              //MOD038
                CGBC_CMD_HWM_SENSOR_IN     hwmSensorIn;              //MOD038
                CGBC_CMD_HWM_SENSOR_OUT    hwmSensorOut;             //MOD038
                CGBC_CMD_POST_CFG_IN       postCfgIn;                //MOD040
                CGBC_CMD_POST_CFG_OUT      postCfgOut;               //MOD040
                CGBC_CMD_LFP_DELAY_IN      bltDelayIn;               //MOD040
                CGBC_CMD_LFP_DELAY_OUT     bltDelayOut;              //MOD040
                CGBC_CMD_HWM_CFG_IN        hwmCfgIn;                 //MOD040
                CGBC_CMD_HWM_CFG_OUT       hwmCfgOut;                //MOD040
                CGBC_CMD_DIAG_CFG_IN       diagCfgIn;                //MOD041
                CGBC_CMD_DIAG_CFG_OUT      diagCfgOut;               //MOD041
                CGBC_CMD_LOG_CFG_IN        xLogCfgIn;                //MOD044
                CGBC_CMD_LOG_CFG_OUT       xLogCfgOut;               //MOD044
                CGBC_CMD_LOG_READ_IN       xLogReadIn;               //MOD044
                CGBC_CMD_LOG_READ_OUT      xLogReadOut;              //MOD044
                CGBC_CMD_LOG_WRITE_IN      xLogWriteIn;              //MOD044
                CGBC_CMD_LOG_GPIO_IN       xLogGpioIn;               //MOD044
                CGBC_CMD_LOG_GPIO_OUT      xLogGpioOut;              //MOD044
                CGBC_CMD_PWROK_DELAY_IN    xPwrokDelayIn;            //MOD045
                CGBC_CMD_PWROK_DELAY_OUT   xPwrokDelayOut;           //MOD045
                CGBC_CMD_API_CONFIG_IN     xApiConfigIn;             //MOD044
                CGBC_CMD_CON_RTM_IN        xConRunTimIn;             //MOD045
                CGBC_CMD_CON_RTM_OUT       xConRunTimOut;            //MOD045
                CGBC_CMD_ADV_USER_LOCK_IN  xAdvUserLockIn;           //MOD045
                CGBC_CMD_BLT_PWM_DELAY_IN  xBltPwmDelayIn;           //MOD044
                CGBC_CMD_BLT_PWM_DELAY_OUT xBltPwmDelayOut;          //MOD044
                CGBC_CMD_PIN_FEAT_IN       xPinFeatIn;               //MOD046
                CGBC_CMD_PIN_FEAT_OUT      xPinFeatOut;              //MOD046
                CGBC_CMD_DIAG_MODE_IN      xDiagModeIn;              //MOD044
                CGBC_CMD_BAT_IN            xBatIn;                   //MOD046
                CGBC_CMD_BAT_DYN_DATA_OUT  xBatDynData;              //MOD046
                CGBC_CMD_BAT_STA_DATA_OUT  xBatStaData;              //MOD046
                CGBC_CMD_WDOG_RD_IN        xWdogRdIn;                //MOD046
                CGBC_CMD_GPIO_IRQ_TBL_IN   xGpioIrqTblIn;            //MOD046
                CGBC_CMD_GPIO_IRQ_TBL_OUT  xGpioIrqTblOut;           //MOD046
                CGBC_CMD_I2C_START_EXT_IN  xI2cStartExtIn;           //MOD059
                CGBC_CMD_I2C_STAT_EXT_IN   xI2cStatExtIn;            //MOD059
                CGBC_CMD_I2C_DATA_EXT_IN   xI2cDataExtIn;            //MOD059
                CGBC_CMD_I2C_SPEED_EXT_IN  xI2cSpeedExtIn;           //MOD059
                CGBC_CMD_AVR_SPM_FLS_ADDR_IN  xAvrSpmFlsAddr;        //MOD061
                CGBC_CMD_AVR_SPM_FLS_STAT_IN  xAvrSpmFlsStatIn;      //MOD061
                CGBC_CMD_AVR_SPM_FLS_STAT_OUT xAvrSpmFlsStatOut;     //MOD061
                CGBC_CMD_AVR_SPM_FLS_RD32_IN  xAvrSpmFlsRd32In;      //MOD061
                CGBC_CMD_AVR_SPM_FLS_RD32_OUT xAvrSpmFlsRd32Out;     //MOD061
                CGBC_CMD_AVR_SPM_FLS_WR32_IN  xAvrSpmFlsWr32;        //MOD061
                CGBC_CMD_GSPI_CFG_IN       xGspiCfgIn;               //MOD063
                CGBC_CMD_GSPI_CFG_OUT      xGspiCfgOut;              //MOD063
                CGBC_CMD_SYNC_OUT          sync;
               } CGBC_CMD, *P_CGBC_CMD;

#endif //!__ASSEMBLER__
                                                                     //MOD014^

                                                                     /*MOD027v*/
/*----------------------------------------------------
 * CM41 host interface information structure HIF_INFO
 *----------------------------------------------------
 */
#ifndef __ASSEMBLER__

typedef struct HIF_INFO_STRUCT
{
                                                                     //MOD070v
  uint32_t    id;                  /* HIF_ID                                 */
  uint8_t     infoRev;             /* info structure revision                */
  uint8_t     infoSize;            /* structure size in bytes                */
  uint8_t     flags;               /* host interface control flags           */
  uint8_t     res7;                /* reserved (offset 0x07)                 */
  uint32_t    ctrlBase;            /* control block base address             */
  uint32_t    shBase;              /* shared command channel base address    */
  uint32_t    exBase;              /* exclusive command channel base address */
  uint8_t     shSize;              /* shared command channel size            */
  uint8_t     shIrq;               /* shared command channel IRQ control     */
  uint8_t     exSize;              /* exclusive command channel size         */
  uint8_t     exIrq;               /* exclusive command channel IRQ control  */
  CGBC_FW_REV fwRev;               /* CGBC firmware revision information     */
  uint32_t    res1C;               /* reserved (offset 0x1C)                 */
                                                                     //MOD070^
} HIF_INFO, *P_HIF_INFO;

#endif //!__ASSEMBLER__

/* host interface identifier */
#define HIF_ID 0x434263DE       /* CGBC_ERR_HIF_RESET, 'c', 'B', 'C' **MOD037*/

/* Info structure revision */
#define HIF_INFO_REV_MAJ_MASK  0xF0                 /* major revision number */
#define HIF_INFO_REV_MAJ_SHIFT 4
#define HIF_INFO_REV_MIN_MASK  0x0F                 /* minor revision number */
#define HIF_INFO_REV_MIN_SHIFT 0
#define HIF_INFO_REV ((0<<HIF_INFO_REV_MAJ_SHIFT)|(1<<HIF_INFO_REV_MIN_SHIFT))

/* Host interface information structure size */
#define HIF_INFO_SIZE (sizeof (HIF_INFO))

/* host interface control flags */
#define HIF_FLAGS_INV 0x80                 /* control flags invalid          */
#define HIF_UPD       0x01                 /* request to update HIF defaults */

/* default LPC base addresses */
#define HIF_CTRL_BASE_DFLT 0x0C38       /* HIF control block base address    */
#define HIF_SH_BASE_DFLT   0x0C40       /* shared command channel base addr. */
#define HIF_EX_BASE_DFLT   0x0C80       /* excl. command channel base addr.  */

/* supported command channel sizes */
#define HIF_CH_SIZE_64  6
#define HIF_CH_SIZE_128 7
#define HIF_CH_SIZE_256 8
#define HIF_CH_SIZE_MIN HIF_CH_SIZE_64
#define HIF_CH_SIZE_MAX HIF_CH_SIZE_256

/* default command channel sizes */
#define HIF_SH_SIZE_DFLT HIF_CH_SIZE_64
#define HIF_EX_SIZE_DFLT HIF_CH_SIZE_64

/* IRQ control parameter */
#define HIF_IRQ_INV      0x80              /* IRQ control parameter invalid  */
#define HIF_IRQ_ENA      0x10              /* 0: IRQ enabled / 1: IRQ masked */
#define HIF_IRQ_NUM_MASK 0x0F              /* IRQ0 - IRQ15                   */

/*------------------------------
 * Host interface control block
 *------------------------------
 */
/* register offsets from control block base address HIF_CTRL_BASE */
#define HIF_ACCESS  0              /* host interface access control register */
#define HIF_RELEASE 0              /* host interface release register        */
#define HIF_STATUS  4              /* host interface status register         */

/* host interface status register HIF_ACCESS */
#define HIF_OCC_SHFT 4                    /* shared command channel occupied */
#define HIF_MIS_SHFT 5                    /* access number mismatch          */

#define HIF_OCC_MASK (1 << HIF_OCC_SHFT)
#define HIF_MIS_MASK (1 << HIF_OCC_SHFT)
                                                                     /*MOD027^*/


#ifndef __AVR__                                                      //MOD013
#pragma pack()                                                       //MOD001
#endif //!__AVR__                                                    //MOD013



#endif /*CGBC_H_INCLUDED*/
