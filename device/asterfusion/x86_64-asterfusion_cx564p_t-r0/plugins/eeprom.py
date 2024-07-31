#!/usr/bin/env python

#############################################################################
# Asterfusion CX564P-T EEPROM Plugin
#
# Platform and model specific eeprom subclass, inherits from the base class,
# and provides the followings:
# - the eeprom format definition
# - specific encoder/decoder if there is special need
#############################################################################

try:
    import binascii
    import errno
    import io
    import os
    import struct
    import sys
    from sonic_eeprom import eeprom_tlvinfo

    sys.path.append(os.path.dirname(__file__))
    from helper import ThriftConnect
    from helper import wait_for_thrift_server_setup

except ImportError as e:
    raise ImportError (str(e) + "- required module not found")


EEPROM_CACHE = "/var/cache/sonic/decode-syseeprom/syseeprom_cache"


class board(eeprom_tlvinfo.TlvInfoDecoder):

    THRIFT_RETRIES = 5
    THRIFT_TIMEOUT = 5
    _TLV_INFO_HDR_LEN = 11

    def __init__(self, name, path, cpld_root, ro):
        if not os.path.exists(os.path.dirname(EEPROM_CACHE)):
            try:
                os.makedirs(os.path.dirname(EEPROM_CACHE))
            except OSError as e:
                if e.errno != errno.EEXIST:
                    raise
        wait_for_thrift_server_setup()
        self.eeprom_init()
        self.eeprom_path = EEPROM_CACHE
        super(board, self).__init__(self.eeprom_path, 0, '', True)

    def util_assemble_eeprom_raw(self, eeprom):
        eeprom_values = vars(eeprom)
        eeprom_codes = (
                                            # TlvInfo Header:
                                            #    Id String:    TlvInfo
                                            #    Version:      1
                                            #    Total Length: 169
                                            # TLV Name            Code Len Value
            # Key                 # Code    # ------------------- ---- --- -----
            ('prod_name',         b'\x21'), # Product Name        0x21  11 X312P-48Y-T
            ('prod_part_num',     b'\x22'), # Part Number         0x22   7 CME3000
            ('prod_ser_num',      b'\x23'), # Serial Number       0x23   8 18120001
            ('ext_mac_addr',      b'\x24'), # Base MAC Address    0x24   6 00:11:22:33:44:55
            ('sys_mfg_date',      b'\x25'), # Manufacture Date    0x25  19 12/31/2018 23:59:59
            ('prod_ver',          b'\x26'), # Device Version      0x26   1 0
            ('prod_sub_ver',      b'\x27'), # Label Revision      0x27   1 0
            ('prod_arch',         b'\x28'), # Platform Name       0x28  30 x86_64-asterfusion_cx312p_48y_
            ('onie_version',      b'\x29'), # ONIE Version        0x29  10 2018.07.31
            ('ext_mac_addr_size', b'\x2a'), # MAC Addresses       0x2A   2 1
            ('sys_mfger',         b'\x2b'), # Manufacturer        0x2B  11 Asterfusion
            ('country_code',      b'\x2c'), # Manufacture Country 0x2C   2 CN
            ('vendor_name',       b'\x2d'), # Vendor Name         0x2D  11 Asterfusion
            ('diag_version',      b'\x2e'), # Diag Version        0x2E   3 1.0
            ('serv_tag',          b'\x2f'), # Service Tag         0x2F   1 X
            ('crc32',             b'\xfe')  # CRC-32              0xFE   4 0x8AFC983D
        )
        eeprom_header = b'TlvInfo\x00\x01\x00'
        eeprom_length = 0
        eeprom_content = b''

        for key, code in eeprom_codes:
            if key in eeprom_values:
                eeprom_value = eeprom_values[key]
                if eeprom_value is None:
                    eeprom_value = 0

                if key == 'ext_mac_addr':
                    value = binascii.unhexlify(''.join((eeprom_value[:17]).split(':')))
                elif key == 'prod_ver' or key == 'prod_sub_ver':
                    value = binascii.unhexlify(format(int(eeprom_value), '02x'))
                elif key == 'ext_mac_addr_size':
                    value = b'\x00'
                    value += binascii.unhexlify(format(int(eeprom_value), '02x'))
                elif key == 'crc32':
                    value = b'\x00\x00\x00\x00'
                else:
                    if type(eeprom_value) is int:
                        eeprom_value = str(eeprom_value)
                    value = b''
                    for char in eeprom_value:
                        value += binascii.unhexlify(format(ord(char), '02x'))

                length = struct.pack('B', len(value))

                eeprom_content += code + length + value
                eeprom_length += len(value) + 2

        eeprom_length = struct.pack('B', eeprom_length)
        eeprom_raw = eeprom_header + eeprom_length + eeprom_content

        return eeprom_raw

    def util_open_eeprom(self):
        eeprom_file = EEPROM_CACHE
        try:
            F = io.open(eeprom_file, "rb")
        except:
            exit(1)
        return F

    def util_read_eeprom_bytes(self, byteCount, offset=0):
        F = self.util_open_eeprom()
        F.seek(offset)
        o = F.read(byteCount)
        F.close()
        return o

    def util_read_eeprom(self):
        offset = 0
        h = self.util_read_eeprom_bytes(self._TLV_INFO_HDR_LEN)
        offset = self._TLV_INFO_HDR_LEN
        if sys.version_info.major == 3:
            sizeof_tlvs = (h[9] << 8) | h[10] - 4
        else:
            sizeof_tlvs = ((ord(h[9]) << 8) | ord(h[10])) - 4
        t = self.util_read_eeprom_bytes(sizeof_tlvs, offset)
        return h + t

    def eeprom_init(self):
        with ThriftConnect() as thrift_client:
            eeprom = thrift_client.pltfm_mgr_sys_eeprom_get()
        try:
            if eeprom:
                eeprom_cache = open(EEPROM_CACHE, 'wb')
                eeprom_raw = self.util_assemble_eeprom_raw(eeprom)
                eeprom_cache.write(eeprom_raw)
                eeprom_cache.close()
                data = self.util_read_eeprom()
                crc = binascii.crc32(data) & 0xffffffff
                if sys.version_info.major == 3:
                    crc_data = (binascii.unhexlify(format(int((crc & 0xff000000) >> 24), '02x')) +
                                binascii.unhexlify(format(int((crc & 0x00ff0000) >> 16), '02x')) +
                                binascii.unhexlify(format(int((crc & 0x0000ff00) >> 8), '02x')) +
                                binascii.unhexlify(format(int((crc & 0x000000ff)), '02x')))
                else:
                    crc_data = (chr((crc & 0xff000000) >> 24) +
                                chr((crc & 0x00ff0000) >> 16) +
                                chr((crc & 0x0000ff00) >> 8) +
                                chr(crc & 0x000000ff))
                data = data + crc_data
                eeprom_cache = open(EEPROM_CACHE, 'wb')
                eeprom_cache.write(data)
                eeprom_cache.close()
        except:
            print("eeprom decode error")
            return False
        return True
