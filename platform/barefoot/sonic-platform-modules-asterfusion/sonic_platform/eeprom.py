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
    import struct

    from pathlib import Path

    from .pltfm_utils.constants import *
    from .pltfm_utils.helper import APIHelper

    from sonic_platform_base.sonic_eeprom.eeprom_tlvinfo import TlvInfoDecoder
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


class Tlv(TlvInfoDecoder):
    """Platform-specific EEPROM class"""

    def __init__(self, helper, bdid, device, platform):
        super(Tlv, self).__init__(CACHE_PATH, 0, "", True)
        self._api_helper = helper  # type: APIHelper
        # Init device
        self._bdid = bdid
        self._device = device
        self._platform = platform
        # Init eeprom
        self._eeprom = {}
        self._init_eeprom()
        self._cache_eeprom()

    def _init_eeprom(self):
        with self._api_helper.thrift_client() as client:
            self._eeprom = vars(
                client.pltfm_mgr_sys_eeprom_get()
            )  # type: dict[str, str]

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

        self._api_helper.log_debug("Retrieved EEPROM data from BSP")

    def _cache_eeprom(self):
        eeprom_cache_dir = Path(CACHE_DIR)
        if not eeprom_cache_dir.exists():
            eeprom_cache_dir.mkdir(parents=True, exist_ok=True)

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
                value = binascii.unhexlify("".join(eeprom_value[:17].split(":")))
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
        crc_raw = binascii.crc32(eeprom_no_crc) & 0xFFFFFFFF
        crc_data = (
            binascii.unhexlify(format(int((crc_raw & 0xFF000000) >> 24), "02x"))
            + binascii.unhexlify(format(int((crc_raw & 0x00FF0000) >> 16), "02x"))
            + binascii.unhexlify(format(int((crc_raw & 0x0000FF00) >> 8), "02x"))
            + binascii.unhexlify(format(int((crc_raw & 0x000000FF)), "02x"))
        )
        eeprom_raw = eeprom_no_crc + crc_data

        try:
            with open(CACHE_PATH, "wb") as eeprom_cache:
                eeprom_cache.write(eeprom_raw)
        except PermissionError as err:
            pass

        self._api_helper.log_debug("Saved EEPROM data to {}".format(CACHE_PATH))

    def get_eeprom(self):
        return self._eeprom

    def get_serial(self):
        return self._eeprom.get("prod_ser_num", NOT_AVAILABLE)

    def get_mac(self):
        return self._eeprom.get("ext_mac_addr", NOT_AVAILABLE)
