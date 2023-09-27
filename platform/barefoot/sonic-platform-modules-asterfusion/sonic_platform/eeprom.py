##############################################################################
# Asterfusion CX-T Devices EEPROM                                            #
#                                                                            #
# Platform and model specific eeprom subclass, inherits from the base class, #
# and provides the followings:                                               #
# - the eeprom format definition                                             #
# - specific encoder/decoder if there is special need                        #
#                                                                            #
##############################################################################

try:
    import binascii
    import os
    import struct
    import sys

    from .constants import *
    from .helper import APIHelper

    from sonic_platform_base.sonic_eeprom.eeprom_tlvinfo import TlvInfoDecoder
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")

CACHE_ROOT = "/var/cache/sonic/decode-syseeprom"
CACHE_FILE = "syseeprom_cache"
EEPROM_CACHE = os.path.join(CACHE_ROOT, CACHE_FILE)
EEPROM_CACHE_DIR = os.path.dirname(EEPROM_CACHE)


class Tlv(TlvInfoDecoder):
    """Platform-specific EEPROM class"""

    def __init__(self, helper, hwver, hwsku, platform):
        super(Tlv, self).__init__(EEPROM_CACHE, 0, "", True)
        self._api_helper = helper  # type: APIHelper
        # Init device
        self.hwver = hwver
        self.hwsku = hwsku
        self.platform = platform
        # Init eeprom
        self._eeprom = {}
        self._init_eeprom()
        self._cache_eeprom()

    def _init_eeprom(self):
        with self._api_helper.thrift_connect() as client:
            self._eeprom = vars(client.pltfm_mgr_sys_eeprom_get())

        for key in self._eeprom:
            if self._eeprom[key] is None:
                self._eeprom[key] = 0
            if key == "ext_mac_addr":
                self._eeprom[key] = self._eeprom[key][:17]
                self._api_helper.validate_mac_addr(self._eeprom[key])
            elif key == "prod_ver" or key == "prod_sub_ver":
                self._eeprom[key] = int(self._eeprom[key])
            elif key == "ext_mac_addr_size":
                self._eeprom[key] = int(self._eeprom[key])
            elif key != "crc32":
                self._eeprom[key] = str(self._eeprom[key])

    def _cache_eeprom(self):
        eeprom_header = b"TlvInfo\x00\x01\x00"
        eeprom_length = 0
        eeprom_content = b""

        for key, code in EEPROM_FIELD_CODE_MAP:
            if key not in self._eeprom:
                continue
            if code in EEPROM_IGNORED_CODE_LIST:
                continue
            eeprom_value = self._eeprom[key]
            if eeprom_value is None:
                eeprom_value = 0
            if key == "ext_mac_addr":
                value = binascii.unhexlify("".join((eeprom_value[:17]).split(":")))
            elif key == "prod_ver" or key == "prod_sub_ver":
                value = binascii.unhexlify(format(int(eeprom_value), "02x"))
            elif key == "ext_mac_addr_size":
                value = b"\x00"
                value += binascii.unhexlify(format(int(eeprom_value), "02x"))
            elif key == "crc32":
                value = b"\x00\x00\x00\x00"
            else:
                if type(eeprom_value) is int:
                    eeprom_value = str(eeprom_value)
                value = b""
                for char in eeprom_value:
                    value += binascii.unhexlify(format(ord(char), "02x"))
            length = struct.pack("B", len(value))
            eeprom_content += code + length + value
            eeprom_length += len(value) + 2

        eeprom_length = struct.pack("B", eeprom_length)
        eeprom_raw = eeprom_header + eeprom_length + eeprom_content
        eeprom_no_crc = eeprom_raw[:-4]
        crc = binascii.crc32(eeprom_no_crc) & 0xFFFFFFFF
        if sys.version_info.major == 3:
            crc_data = (
                binascii.unhexlify(format(int((crc & 0xFF000000) >> 24), "02x"))
                + binascii.unhexlify(format(int((crc & 0x00FF0000) >> 16), "02x"))
                + binascii.unhexlify(format(int((crc & 0x0000FF00) >> 8), "02x"))
                + binascii.unhexlify(format(int((crc & 0x000000FF)), "02x"))
            )
        else:
            crc_data = (
                chr((crc & 0xFF000000) >> 24)
                + chr((crc & 0x00FF0000) >> 16)
                + chr((crc & 0x0000FF00) >> 8)
                + chr(crc & 0x000000FF)
            )
        eeprom_raw = eeprom_no_crc + crc_data

        try:
            if not os.path.exists(EEPROM_CACHE_DIR):
                os.makedirs(EEPROM_CACHE_DIR, exist_ok=True)
            with open(os.path.join(CACHE_ROOT, CACHE_FILE), "wb") as eeprom_cache:
                eeprom_cache.write(eeprom_raw)
        except Exception:
            pass

    def get_eeprom(self):
        return self._eeprom

    def get_serial(self):
        return self._eeprom.get("prod_ser_num", "N/A")

    def get_mac(self):
        return self._eeprom.get("ext_mac_addr", "N/A")
