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
 
#ifndef _BCSUBMOD_GEN5_CFG_H_
#define _BCSUBMOD_GEN5_CFG_H_

#define GEN5_LDN_REG           0x07
#define GEN5_DEV_ID_REG        0x20
#define    GEN5_DEV_ID_MEC1701    0x2D
#define    GEN5_DEV_ID_MEC1703    0x2E
#define GEN5_DEV_REV_REG       0x21

#define GEN5_LDN_SER0          0x09
#define GEN5_LDN_SER1          0x0A
#define GEN5_LDN_LPC           0x0C
#define GEN5_LDN_ESPI          0x0D
#define GEN5_LDN_EMI0          0x10
#define GEN5_LDN_EMI1          0x11
#define GEN5_LDN_EMI2          0x12

#define GEN5_LPC_ENA_REG       0x30
#define    GEN5_LPC_ENA_MSK       0x01
#define GEN5_LPC_CFG_BAR       0x60
#define GEN5_LPC_SER0_BAR      0x88
#define GEN5_LPC_SER1_BAR      0x8C
#define GEN5_LPC_EMI0_BAR      0x90
#define GEN5_LPC_EMI1_BAR      0x94
#define GEN5_LPC_EMI2_BAR      0x98

#define GEN5_ESPI_ENA_REG      0x30
#define    GEN5_ESPI_ENA_MSK      0x01
#define GEN5_ESPI_CFG_BAR      0x34
#define GEN5_ESPI_SER0_BAR     0x60
#define GEN5_ESPI_SER1_BAR     0x64
#define GEN5_ESPI_EMI0_BAR     0x68
#define GEN5_ESPI_EMI1_BAR     0x6C
#define GEN5_ESPI_EMI2_BAR     0x70

#define GEN5_HCC_STROBE 0x00
#define GEN5_HCC_INDEX  0x02
#define     GEN5_HCC_INDEX_CBI_MSK    0xFC
#define     GEN5_HCC_INDEX_CBM_MSK    0x03
#define     GEN5_HCC_INDEX_CBM_MAN8   0x00
#define     GEN5_HCC_INDEX_CBM_AUTO32 0x03
#define GEN5_HCC_DATA   0x04
#define GEN5_HCC_ACCESS 0x0C

#define GEN5_HCNM_COMMAND 0x00
#define     GEN5_HCNM_IDLE    0x00
#define     GEN5_HCNM_REQUEST 0x01
#define GEN5_HCNM_DATA    0x01
#define GEN5_HCNM_STATUS  0x02
#define     GEN5_HCNM_FREE    0x0003
#define GEN5_HCNM_ACCESS  0x04
#define     GEN5_HCNM_GAINED  0x00000000			

#define HCIO_BASE 0x0E3E
#define HCIO_SIZE 0x0002

#define HCC0_BASE 0x0E00
#define HCC0_SIZE 0x0010

#define HCC1_BASE 0x0E10
#define HCC1_SIZE 0x0010

#define HCNM_BASE 0x0E20
#define HCNM_SIZE 0x0010

#define CGBC_TIMEOUT_VALUE (0x2000)

#endif
