#####################################################################
# Asterfusion CX-T Devices Platform Constants                       #
#                                                                   #
# Module contains an implementation of SONiC Platform Constants and #
# provides the platform information                                 #
#                                                                   #
#####################################################################

try:
    import copy as _copy

    from pathlib import Path as _Path
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")

################################
#             MISC             #
################################
LRU_CACHE_TTL = 5
LOGGING_CONFIG_NAME = "logging.conf"
LOGGING_STACK_INFO = False
DEVICE_ROOT = "/usr/share/sonic/device"
HWSKU_ROOT = "/usr/share/sonic/hwsku"
NOT_AVAILABLE = "N/A"
PMON_SENSORS_CMD = ("sensors", "-j")


################################
#            THRIFT            #
################################
def __get_thrift_rpc_host_from_config():
    # type: () -> str
    import fcntl
    import re
    import socket
    import struct
    from sonic_py_common.device_info import get_path_to_platform_dir

    # type: () -> str
    platform_config_file = _Path(get_path_to_platform_dir(), "platform.conf")
    listen_point = ""
    if not platform_config_file.exists():
        return "127.0.0.1"
    for line in platform_config_file.read_text().splitlines():
        if line.strip().startswith("rpc-listen-point:"):
            listen_point = line.split(":")[1].strip()
            break
    if not listen_point:
        return "127.0.0.1"
    if re.match(
        "^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9]).){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])$",
        listen_point,
    ):
        return listen_point
    try:
        sc = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        return socket.inet_ntoa(
            fcntl.ioctl(
                sc.fileno(),
                0x8915,
                struct.pack("256s", listen_point[:15].encode()),
            )[20:24]
        )
    except OSError as err:
        return "127.0.0.1"


THRIFT_RPC_HOST = __get_thrift_rpc_host_from_config()
THRIFT_RPC_PORT = 9090
THRIFT_RETRY_TIMES = 60
THRIFT_RETRY_TIMEOUT = 3
THRIFT_WAIT_TIMEOUT = 3
THRIFT_DUMMY_DEVID = 0


# fmt: off
################################
#            DEVICE            #
################################
DEVICE_X308PT = "CX308P-48Y-T"
DEVICE_X312PT = "CX312P-48Y-T"
DEVICE_X532PT = "CX532P-T"
DEVICE_X564PT = "CX564P-T"
DEVICE_X732QT = "CX732Q-T"


################################
#           PLATFORM           #
################################
PLTFM_X308PT = "x86_64-asterfusion_cx308p_48y_t-r0"
PLTFM_X312PT = "x86_64-asterfusion_cx312p_48y_t-r0"
PLTFM_X532PT = "x86_64-asterfusion_cx532p_t-r0"
PLTFM_X564PT = "x86_64-asterfusion_cx564p_t-r0"
PLTFM_X732QT = "x86_64-asterfusion_cx732q_t-r0"


################################
#            BOARD             #
################################
BOARD_ID_X308PT_V1DOT0 = 0x3080  # AFN_BD_ID_X308PT_V1P0
BOARD_ID_X308PT_V1DOT1 = 0x3081  # AFN_BD_ID_X308PT_V1P1
BOARD_ID_X308PT_V2DOT0 = 0x3081  # AFN_BD_ID_X308PT_V2P0
BOARD_ID_X308PT_V3DOT0 = 0x3083  # AFN_BD_ID_X308PT_V3P0

BOARD_ID_X312PT_V1DOT0 = 0x3120  # AFN_BD_ID_X312PT_V1P0
BOARD_ID_X312PT_V1DOT1 = 0x3120  # AFN_BD_ID_X312PT_V1P1
BOARD_ID_X312PT_V1DOT2 = 0x3122  # AFN_BD_ID_X312PT_V1P2
BOARD_ID_X312PT_V2DOT0 = 0x3122  # AFN_BD_ID_X312PT_V2P0
BOARD_ID_X312PT_V1DOT3 = 0x3123  # AFN_BD_ID_X312PT_V1P3
BOARD_ID_X312PT_V3DOT0 = 0x3123  # AFN_BD_ID_X312PT_V3P0
BOARD_ID_X312PT_V4DOT0 = 0x3124  # AFN_BD_ID_X312PT_V4P0
BOARD_ID_X312PT_V5DOT0 = 0x3125  # AFN_BD_ID_X312PT_V5P0

BOARD_ID_X532PT_V1DOT0 = 0x5320  # AFN_BD_ID_X532PT_V1P0
BOARD_ID_X532PT_V1DOT1 = 0x5321  # AFN_BD_ID_X532PT_V1P1
BOARD_ID_X532PT_V2DOT0 = 0x5322  # AFN_BD_ID_X532PT_V2P0
BOARD_ID_X532PT_V3DOT0 = 0x5323  # AFN_BD_ID_X532PT_V3P0

BOARD_ID_X564PT_V1DOT0 = 0x5640  # AFN_BD_ID_X564PT_V1P0
BOARD_ID_X564PT_V1DOT1 = 0x5641  # AFN_BD_ID_X564PT_V1P1
BOARD_ID_X564PT_V1DOT2 = 0x5642  # AFN_BD_ID_X564PT_V1P2
BOARD_ID_X564PT_V2DOT0 = 0x5643  # AFN_BD_ID_X564PT_V2P0

BOARD_ID_X732QT_V1DOT0 = 0x7320  # AFN_BD_ID_X732QT_V1P0


################################
#            HWSKU             #
################################
HWSKU_X308PT = (
    BOARD_ID_X308PT_V1DOT0,
    BOARD_ID_X308PT_V1DOT1,
    BOARD_ID_X308PT_V2DOT0,
    BOARD_ID_X308PT_V3DOT0,
)

HWSKU_X312PT = (
    BOARD_ID_X312PT_V1DOT0,
    BOARD_ID_X312PT_V1DOT1,
    BOARD_ID_X312PT_V1DOT2,
    BOARD_ID_X312PT_V2DOT0,
    BOARD_ID_X312PT_V1DOT3,
    BOARD_ID_X312PT_V3DOT0,
    BOARD_ID_X312PT_V4DOT0,
    BOARD_ID_X312PT_V5DOT0,
)

HWSKU_X532PT = (
    BOARD_ID_X532PT_V1DOT0,
    BOARD_ID_X532PT_V1DOT1,
    BOARD_ID_X532PT_V2DOT0,
    BOARD_ID_X532PT_V3DOT0,
)

HWSKU_X564PT = (
    BOARD_ID_X564PT_V1DOT0,
    BOARD_ID_X564PT_V1DOT1,
    BOARD_ID_X564PT_V1DOT2,
    BOARD_ID_X564PT_V2DOT0,
)

HWSKU_X732QT = (
    BOARD_ID_X732QT_V1DOT0,
)


################################
#           CHASSIS            #
################################
# Possible reboot causes
REBOOT_CAUSE_POWER_LOSS = "Power Loss"
REBOOT_CAUSE_THERMAL_OVERLOAD_CPU = "Thermal Overload: CPU"
REBOOT_CAUSE_THERMAL_OVERLOAD_ASIC = "Thermal Overload: ASIC"
REBOOT_CAUSE_THERMAL_OVERLOAD_OTHER = "Thermal Overload: Other"
REBOOT_CAUSE_INSUFFICIENT_FAN_SPEED = "Insufficient Fan Speed"
REBOOT_CAUSE_WATCHDOG = "Watchdog"
REBOOT_CAUSE_HARDWARE_OTHER = "Hardware - Other"
REBOOT_CAUSE_NON_HARDWARE = "Non-Hardware"


################################
#          COMPONENT           #
################################
# Updating firmware is not supported yet!
COMPONENT_INFO = {}  # type: dict[str, dict[int, dict[str, int|list[dict[str, str]]]]]

###########
# X308P-T #
###########
COMPONENT_INFO[PLTFM_X308PT] = {}
COMPONENT_INFO[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0] = {}
COMPONENT_INFO[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0]["NUM"] = 4
COMPONENT_INFO[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0]["INFO"] = [
    {
        "name": "BIOS",
        "desc": "Basic Input/Output System",
        "type": "file",
        "target": "/sys/class/dmi/id/bios_version",
    },
    {
        "name": "BMC",
        "desc": "Baseboard Management Controller",
        "type": "mgr",
        "target": ("pltfm_mgr_bmc_version_get",),
    },
    {
        "name": "CPLD1",
        "desc": "SFP+ Ports (Y1 - Y48), QSFP+ Ports (C1 - C8) Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld1_ver",
        ),
    },
    {
        "name": "CPLD2",
        "desc": "Hardware Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld2_ver",
        ),
    },
]
COMPONENT_INFO[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT1] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
COMPONENT_INFO[PLTFM_X308PT][BOARD_ID_X308PT_V2DOT0] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
COMPONENT_INFO[PLTFM_X308PT][BOARD_ID_X308PT_V3DOT0] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])

###########
# X312P-T #
###########
COMPONENT_INFO[PLTFM_X312PT] = {}
COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0] = {}
COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0]["NUM"] = 7
COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0]["INFO"] = [
    {
        "name": "BIOS",
        "desc": "Basic Input/Output System",
        "type": "file",
        "target": "/sys/class/dmi/id/bios_version",
    },
    {
        "name": "BMC",
        "desc": "Baseboard Management Controller",
        "type": "mgr",
        "target": ("pltfm_mgr_bmc_version_get",),
    },
    {
        "name": "CPLD1",
        "desc": "QSFP+ Ports (C1 - C12) Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld1_ver",
        ),
    },
    {
        "name": "CPLD2",
        "desc": "Hardware Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld2_ver",
        ),
    },
    {
        "name": "CPLD3",
        "desc": "SFP+ Ports (Y1 - Y15, Y49 - Y50) Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld3_ver",
        ),
    },
    {
        "name": "CPLD4",
        "desc": "SFP+ Ports (Y16 - Y32) Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld4_ver",
        ),
    },
    {
        "name": "CPLD5",
        "desc": "SFP+ Ports (Y33 - Y48) Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld5_ver",
        ),
    },
]
COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT1] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT2] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V2DOT0] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT3] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V3DOT0] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V4DOT0] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V5DOT0] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])

###########
# X532P-T #
###########
COMPONENT_INFO[PLTFM_X532PT] = {}
COMPONENT_INFO[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0] = {}
COMPONENT_INFO[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0]["NUM"] = 4
COMPONENT_INFO[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0]["INFO"] = [
    {
        "name": "BIOS",
        "desc": "Basic Input/Output System",
        "type": "file",
        "target": "/sys/class/dmi/id/bios_version",
    },
    {
        "name": "BMC",
        "desc": "Baseboard Management Controller",
        "type": "mgr",
        "target": ("pltfm_mgr_bmc_version_get",),
    },
    {
        "name": "CPLD1",
        "desc": "QSFP+ Ports (C31 - C32) Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld1_ver",
        ),
    },
    {
        "name": "CPLD2",
        "desc": "SFP+ Ports (Y1 - Y2), QSFP+ Ports (C1 - C30) Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld2_ver",
        ),
    },
]
COMPONENT_INFO[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT1] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
COMPONENT_INFO[PLTFM_X532PT][BOARD_ID_X532PT_V2DOT0] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
COMPONENT_INFO[PLTFM_X532PT][BOARD_ID_X532PT_V3DOT0] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])

###########
# X564P-T #
###########
COMPONENT_INFO[PLTFM_X564PT] = {}
COMPONENT_INFO[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0] = {}
COMPONENT_INFO[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0]["NUM"] = 5
COMPONENT_INFO[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0]["INFO"] = [
    {
        "name": "BIOS",
        "desc": "Basic Input/Output System",
        "type": "file",
        "target": "/sys/class/dmi/id/bios_version",
    },
    {
        "name": "BMC",
        "desc": "Baseboard Management Controller",
        "type": "mgr",
        "target": ("pltfm_mgr_bmc_version_get",),
    },
    {
        "name": "CPLD1",
        "desc": "Hardware Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld1_ver",
        ),
    },
    {
        "name": "CPLD2",
        "desc": "SFP+ Ports (Y1 - Y2), QSFP+ Ports (C1 - C14, C16, C33 - C46, C48) Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld2_ver",
        ),
    },
    {
        "name": "CPLD3",
        "desc": "QSFP+ Ports (C15, C17 - C32, C47, C49 - C64) Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld3_ver",
        ),
    },
]
COMPONENT_INFO[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT1] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
COMPONENT_INFO[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT2] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
COMPONENT_INFO[PLTFM_X564PT][BOARD_ID_X564PT_V2DOT0] = _copy.deepcopy(COMPONENT_INFO[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])

###########
# X732Q-T #
###########
COMPONENT_INFO[PLTFM_X732QT] = {}
COMPONENT_INFO[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0] = {}
COMPONENT_INFO[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0]["NUM"] = 4
COMPONENT_INFO[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0]["INFO"] = [
    {
        "name": "BIOS",
        "desc": "Basic Input/Output System",
        "type": "file",
        "target": "/sys/class/dmi/id/bios_version",
    },
    {
        "name": "BMC",
        "desc": "Baseboard Management Controller",
        "type": "mgr",
        "target": ("pltfm_mgr_bmc_version_get",),
    },
    {
        "name": "CPLD1",
        "desc": "SFP+ Ports (Y1 - Y2), QSFP+ Ports (QC1 - QC32) Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld1_ver",
        ),
    },
    {
        "name": "CPLD2",
        "desc": "Hardware Management",
        "type": "mgr",
        "target": (
            "pltfm_mgr_cpld_info_get",
            "cpld2_ver",
        ),
    },
]


################################
#            EEPROM            #
################################
EEPROM_FIELD_CODE_MAP = [
    # TlvInfo Header:
    #    Id String:    TlvInfo
    #    Version:      1
    #    Total Length: 169
    #                                # TLV Name            Code Len Value
    # Key                   Code     # ------------------- ---- --- -----
    ("prod_name",         b"\x21"),  # Product Name        0x21  11 CX564P-T-S
    ("prod_part_num",     b"\x22"),  # Part Number         0x22   7 ONBP2U-T-2Y64C-S
    ("prod_ser_num",      b"\x23"),  # Serial Number       0x23   8 F01262BA086
    ("ext_mac_addr",      b"\x24"),  # Base MAC Address    0x24   6 60:EB:5A:00:56:88
    ("sys_mfg_date",      b"\x25"),  # Manufacture Date    0x25  19 27/11/2022 12:08:33
    ("prod_ver",          b"\x26"),  # Device Version      0x26   1 1
    ("prod_sub_ver",      b"\x27"),  # Label Revision      0x27   1 0
    ("prod_arch",         b"\x28"),  # Platform Name       0x28  30 x86_64-asterfusion_x564p_t-r0
    ("onie_version",      b"\x29"),  # ONIE Version        0x29  10 2019.05_V1.0.6
    ("ext_mac_addr_size", b"\x2a"),  # MAC Addresses       0x2A   2 1
    ("sys_mfger",         b"\x2b"),  # Manufacturer        0x2B  11 Asterfusion
    ("country_code",      b"\x2c"),  # Manufacture Country 0x2C   2 CN
    ("vendor_name",       b"\x2d"),  # Vendor Name         0x2D  11 Asterfusion
    ("diag_version",      b"\x2e"),  # Diag Version        0x2E   3 1.0
    ("serv_tag",          b"\x2f"),  # Service Tag         0x2F   1 X
    ("asic_vendor",       b"\x30"),  # Switch ASIC Vendor  0x30   1 Intel-bf                      * IGNORED CODE *
    ("main_bd_version",   b"\x31"),  # Main Board Version  0x31   1 APNS640T-A1-V2.0-221100179    * IGNORED CODE *
    ("come_version",      b"\x32"),  # COME Version        0x32   1 CME5008-16GB-HH-CGT           * IGNORED CODE *
    ("ghc_bd0",           b"\x33"),  # GHC-0 Board Version 0x33   1                               * IGNORED CODE *
    ("ghc_bd1",           b"\x34"),  # GHC-1 Board Version 0x34   1                               * IGNORED CODE *
    ("crc32",             b"\xfe"),  # CRC-32              0xFE   4 0x416F0FBB
]
# EEPROM codes that we should skip
EEPROM_IGNORED_CODE_LIST = (
    b"\x30",
    b"\x31",
    b"\x32",
    b"\x33",
    b"\x34",
)

CACHE_DIR = "/var/cache/sonic/decode-syseeprom"
CACHE_FILE = "syseeprom_cache"
CACHE_PATH = _Path(CACHE_DIR, CACHE_FILE).as_posix()


################################
#        PERIPHERAL NUM        #
################################
PERIPHERAL_NUM = {}  # type: dict[str, dict[int, dict[str, dict[str, int]]]]

###########
# X308P-T #
###########
PERIPHERAL_NUM[PLTFM_X308PT] = {}
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0] = {}
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0]["FAN"] = {}
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0]["FAN"]["DRAWER_NUM"] = 6
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0]["FAN"]["NUM"] = 2
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0]["PSU"] = {}
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0]["PSU"]["NUM"] = 2
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0]["SFP"] = {}
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0]["SFP"]["NUM"] = 56
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0]["THERMAL"] = {}
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0]["THERMAL"]["NUM"] = 10
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT1] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V2DOT0] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V3DOT0] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X308PT][BOARD_ID_X308PT_V3DOT0]["FAN"]["DRAWER_NUM"] = 5

###########
# X312P-T #
###########
PERIPHERAL_NUM[PLTFM_X312PT] = {}
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0] = {}
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0]["FAN"] = {}
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0]["FAN"]["DRAWER_NUM"] = 5
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0]["FAN"]["NUM"] = 2
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0]["PSU"] = {}
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0]["PSU"]["NUM"] = 2
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0]["SFP"] = {}
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0]["SFP"]["NUM"] = 66
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0]["THERMAL"] = {}
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0]["THERMAL"]["NUM"] = 10
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT1] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT2] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V2DOT0] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT3] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V3DOT0] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V4DOT0] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V5DOT0] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])

###########
# X532P-T #
###########
PERIPHERAL_NUM[PLTFM_X532PT] = {}
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0] = {}
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0]["FAN"] = {}
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0]["FAN"]["DRAWER_NUM"] = 5
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0]["FAN"]["NUM"] = 2
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0]["PSU"] = {}
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0]["PSU"]["NUM"] = 2
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0]["SFP"] = {}
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0]["SFP"]["NUM"] = 34
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0]["THERMAL"] = {}
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0]["THERMAL"]["NUM"] = 6
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT1] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V2DOT0] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V3DOT0] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])

###########
# X564P-T #
###########
PERIPHERAL_NUM[PLTFM_X564PT] = {}
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0] = {}
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0]["FAN"] = {}
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0]["FAN"]["DRAWER_NUM"] = 2
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0]["FAN"]["NUM"] = 2
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0]["PSU"] = {}
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0]["PSU"]["NUM"] = 2
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0]["SFP"] = {}
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0]["SFP"]["NUM"] = 66
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0]["THERMAL"] = {}
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0]["THERMAL"]["NUM"] = 6
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT1] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT2] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V2DOT0] = _copy.deepcopy(PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
PERIPHERAL_NUM[PLTFM_X564PT][BOARD_ID_X564PT_V2DOT0]["THERMAL"]["NUM"] = 8

###########
# X732Q-T #
###########
PERIPHERAL_NUM[PLTFM_X732QT] = {}
PERIPHERAL_NUM[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0] = {}
PERIPHERAL_NUM[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0]["FAN"] = {}
PERIPHERAL_NUM[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0]["FAN"]["DRAWER_NUM"] = 6
PERIPHERAL_NUM[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0]["FAN"]["NUM"] = 2
PERIPHERAL_NUM[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0]["PSU"] = {}
PERIPHERAL_NUM[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0]["PSU"]["NUM"] = 2
PERIPHERAL_NUM[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0]["SFP"] = {}
PERIPHERAL_NUM[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0]["SFP"]["NUM"] = 34
PERIPHERAL_NUM[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0]["THERMAL"] = {}
PERIPHERAL_NUM[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0]["THERMAL"]["NUM"] = 6


################################
#             FAN              #
################################
FAN_SPEED_TOLERANCE = 10
FAN_DRAWER_NAME = {}  # type: dict[str, dict[int, list[str]]]
FAN_UNIT_NAME = {}  # type: dict[str, dict[int, list[str]]]
FAN_RPM_LIMIT = {}  # type: dict[str, dict[int, dict[str, int]]]

###########
# X308P-T #
###########
FAN_DRAWER_NAME[PLTFM_X308PT] = {}
FAN_DRAWER_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0] = [
    "FAN DRAWER 1", "FAN DRAWER 2", "FAN DRAWER 3", "FAN DRAWER 4", "FAN DRAWER 5", "FAN DRAWER 6",
]
FAN_DRAWER_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT1] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
FAN_DRAWER_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V2DOT0] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
FAN_DRAWER_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V3DOT0] = [
    "FAN DRAWER 1", "FAN DRAWER 2", "FAN DRAWER 3", "FAN DRAWER 4", "FAN DRAWER 5",
]
FAN_UNIT_NAME[PLTFM_X308PT] = {}
FAN_UNIT_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0] = [
    "FAN 1F", "FAN 1R", "FAN 2F", "FAN 2R", "FAN 3F", "FAN 3R", "FAN 4F", "FAN 4R", "FAN 5F", "FAN 5R", "FAN 6F", "FAN 6R",
]
FAN_UNIT_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT1] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V2DOT0] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V3DOT0] = [
    "FAN 1F", "FAN 1R", "FAN 2F", "FAN 2R", "FAN 3F", "FAN 3R", "FAN 4F", "FAN 4R", "FAN 5F", "FAN 5R",
]
FAN_RPM_LIMIT[PLTFM_X308PT] = {}
FAN_RPM_LIMIT[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0] = {
    "MAX_INLET": 18500, "MAX_OUTLET": 15500,
}
FAN_RPM_LIMIT[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT1] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X308PT][BOARD_ID_X308PT_V2DOT0] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X308PT][BOARD_ID_X308PT_V3DOT0] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])

###########
# X312P-T #
###########
FAN_DRAWER_NAME[PLTFM_X312PT] = {}
FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0] = [
    "FAN DRAWER 1", "FAN DRAWER 2", "FAN DRAWER 3", "FAN DRAWER 4", "FAN DRAWER 5",
]
FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT1] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT2] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V2DOT0] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT3] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V3DOT0] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V4DOT0] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V5DOT0] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X312PT] = {}
FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0] = [
    "FAN 1F", "FAN 1R", "FAN 2F", "FAN 2R", "FAN 3F", "FAN 3R", "FAN 4F", "FAN 4R", "FAN 5F", "FAN 5R",
]
FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT1] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT2] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V2DOT0] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT3] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V3DOT0] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V4DOT0] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V5DOT0] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X312PT] = {}
FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0] = {
    "MAX_INLET": 30000, "MAX_OUTLET": 27000,
}
FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT1] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT2] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V2DOT0] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT3] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V3DOT0] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V4DOT0] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V5DOT0] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])

###########
# X532P-T #
###########
FAN_DRAWER_NAME[PLTFM_X532PT] = {}
FAN_DRAWER_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0] = [
    "FAN DRAWER 1", "FAN DRAWER 2", "FAN DRAWER 3", "FAN DRAWER 4", "FAN DRAWER 5",
]
FAN_DRAWER_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT1] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
FAN_DRAWER_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V2DOT0] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
FAN_DRAWER_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V3DOT0] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X532PT] = {}
FAN_UNIT_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0] = [
    "FAN 1F", "FAN 1R", "FAN 2F", "FAN 2R", "FAN 3F", "FAN 3R", "FAN 4F", "FAN 4R", "FAN 5F", "FAN 5R",
]
FAN_UNIT_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT1] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V2DOT0] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V3DOT0] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X532PT] = {}
FAN_RPM_LIMIT[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0] = {
    "MAX_INLET": 18500, "MAX_OUTLET": 15500,
}
FAN_RPM_LIMIT[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT1] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X532PT][BOARD_ID_X532PT_V2DOT0] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X532PT][BOARD_ID_X532PT_V3DOT0] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])

###########
# X564P-T #
###########
FAN_DRAWER_NAME[PLTFM_X564PT] = {}
FAN_DRAWER_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0] = [
    "FAN DRAWER 1", "FAN DRAWER 2", "FAN DRAWER 3", "FAN DRAWER 4",
]
FAN_DRAWER_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT1] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
FAN_DRAWER_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT2] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
FAN_DRAWER_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V2DOT0] = _copy.deepcopy(FAN_DRAWER_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X564PT] = {}
FAN_UNIT_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0] = [
    "FAN 1", "FAN 2", "FAN 3", "FAN 4",
]
FAN_UNIT_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT1] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT2] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
FAN_UNIT_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V2DOT0] = _copy.deepcopy(FAN_UNIT_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X564PT] = {}
FAN_RPM_LIMIT[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0] = {
    "MAX_INLET": 10000, "MAX_OUTLET": 10000,
}
FAN_RPM_LIMIT[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT1] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT2] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
FAN_RPM_LIMIT[PLTFM_X564PT][BOARD_ID_X564PT_V2DOT0] = _copy.deepcopy(FAN_RPM_LIMIT[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])

###########
# X732Q-T #
###########
FAN_DRAWER_NAME[PLTFM_X732QT] = {}
FAN_DRAWER_NAME[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0] = [
    "FAN DRAWER 1", "FAN DRAWER 2", "FAN DRAWER 3", "FAN DRAWER 4", "FAN DRAWER 5", "FAN DRAWER 6",
]
FAN_UNIT_NAME[PLTFM_X732QT] = {}
FAN_UNIT_NAME[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0] = [
    "FAN 1F", "FAN 1R", "FAN 2F", "FAN 2R", "FAN 3F", "FAN 3R", "FAN 4F", "FAN 4R", "FAN 5F", "FAN 5R", "FAN 6F", "FAN 6R",
]
FAN_RPM_LIMIT[PLTFM_X732QT] = {}
FAN_RPM_LIMIT[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0] = {
    "MAX_INLET": 30000, "MAX_OUTLET": 30000,
}


################################
#             PSU              #
################################
PSU_NAME = [
    "PSU 1", "PSU 2",
]


################################
#             SFP              #
################################
UNKNOWN_PORT_TYPE = "UNKNOWN"
SFP_PORT_TYPE = "SFP_PORT"
QSFP_PORT_TYPE = "QSFP_PORT"
QSFP_DD_PORT_TYPE = "QSFP_DD_PORT"
DPU_PORT_TYPE = "DPU_PORT"

UNKNOWN_SFP_TYPE = "UNKNOWN"
SFP_TYPE = "SFP"
QSFP_TYPE = "QSFP"
QSFP_DD_TYPE = "QSFP_DD"

DPU_PORT_POSITION = -255

LPMODE_UNSUPPORTED = 2
LPMODE_FAILURE = 1
LPMODE_SUCCESS = 0
LPMODE_OFF = 1
LPMODE_ON = 0

RESET_UNSUPPORTED = 2
RESET_FAILURE = 1
RESET_SUCCESS = 0

ERROR_DESCRIPTION_OK = "OK"
ERROR_DESCRIPTION_UNPLUGGED = "Unplugged"

PORT_INDEX_POS_TYPE = {}  # type: dict[str, dict[int, list[tuple[int, int, int, int, str]]]]

###########
# X308P-T #
###########
PORT_INDEX_POS_TYPE[PLTFM_X308PT] = {}
PORT_INDEX_POS_TYPE[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0] = [
    (0, 48, 1, 49, SFP_PORT_TYPE), (48, 56, 1, 9, QSFP_PORT_TYPE),
]
PORT_INDEX_POS_TYPE[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT1] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
PORT_INDEX_POS_TYPE[PLTFM_X308PT][BOARD_ID_X308PT_V2DOT0] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
PORT_INDEX_POS_TYPE[PLTFM_X308PT][BOARD_ID_X308PT_V3DOT0] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])

###########
# X312P-T #
###########
PORT_INDEX_POS_TYPE[PLTFM_X312PT] = {}
PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0] = [
    (0, 48, 1, 49, SFP_PORT_TYPE), (48, 60, 1, 9, QSFP_PORT_TYPE), (60, 62, 49, 51, SFP_PORT_TYPE), (62, 66, 9, 13, DPU_PORT_TYPE),
]
PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT1] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT2] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V2DOT0] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT3] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V3DOT0] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V4DOT0] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V5DOT0] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])

###########
# X532P-T #
###########
PORT_INDEX_POS_TYPE[PLTFM_X532PT] = {}
PORT_INDEX_POS_TYPE[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0] = [
    (0, 32, 1, 33, QSFP_PORT_TYPE), (32, 34, 1, 3, SFP_PORT_TYPE),
]
PORT_INDEX_POS_TYPE[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT1] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
PORT_INDEX_POS_TYPE[PLTFM_X532PT][BOARD_ID_X532PT_V2DOT0] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
PORT_INDEX_POS_TYPE[PLTFM_X532PT][BOARD_ID_X532PT_V3DOT0] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])

###########
# X564P-T #
###########
PORT_INDEX_POS_TYPE[PLTFM_X564PT] = {}
PORT_INDEX_POS_TYPE[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0] = [
    (0, 64, 1, 65, QSFP_PORT_TYPE), (64, 66, 1, 3, SFP_PORT_TYPE),
]
PORT_INDEX_POS_TYPE[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT1] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
PORT_INDEX_POS_TYPE[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT2] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
PORT_INDEX_POS_TYPE[PLTFM_X564PT][BOARD_ID_X564PT_V2DOT0] = _copy.deepcopy(PORT_INDEX_POS_TYPE[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])

###########
# X732Q-T #
###########
PORT_INDEX_POS_TYPE[PLTFM_X732QT] = {}
PORT_INDEX_POS_TYPE[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0] = [
    (0, 32, 1, 33, QSFP_DD_PORT_TYPE), (32, 34, 1, 3, SFP_PORT_TYPE),
]


################################
#           THERMAL            #
################################
TEMP_THRESHOLD_WARNING = 71.0
TEMP_THRESHOLD_CRITICAL_WARNING = 91.0

THERMAL_NAME = {}  # type: dict[str, dict[int, list[str]]]

###########
# X308P-T #
###########
THERMAL_NAME[PLTFM_X308PT] = {}
THERMAL_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0] = [
    "Mainboard Front Left", "Mainboard Front Right",
    "Fan 1",
    "GHC-1 Junction", "GHC-1 Ambient",
    "GHC-2 Junction", "GHC-2 Ambient",
    "Barefoot Junction", "Barefoot Ambient",
    "Fan 2",
]
THERMAL_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT1] = _copy.deepcopy(THERMAL_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
THERMAL_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V2DOT0] = _copy.deepcopy(THERMAL_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])
THERMAL_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V3DOT0] = _copy.deepcopy(THERMAL_NAME[PLTFM_X308PT][BOARD_ID_X308PT_V1DOT0])

###########
# X312P-T #
###########
THERMAL_NAME[PLTFM_X312PT] = {}
THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0] = [
    "LM75", "LM63",
    "Not Defined-1",
    "GHC-1 Junction", "GHC-1 Ambient",
    "GHC-2 Junction", "GHC-2 Ambient",
    "Barefoot Junction", "Barefoot Ambient",
    "Not Defined-2",
]
THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT1] = _copy.deepcopy(THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT2] = _copy.deepcopy(THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V2DOT0] = _copy.deepcopy(THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT0])
THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT3] = [
    "LM75", "LM63", "LM86",
    "GHC-1 Junction", "GHC-1 Ambient",
    "GHC-2 Junction", "GHC-2 Ambient",
    "Barefoot Junction", "Barefoot Ambient",
    "Not Defined",
]
THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V3DOT0] = _copy.deepcopy(THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT3])
THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V4DOT0] = _copy.deepcopy(THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT3])
THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V5DOT0] = _copy.deepcopy(THERMAL_NAME[PLTFM_X312PT][BOARD_ID_X312PT_V1DOT3])

###########
# X532P-T #
###########
THERMAL_NAME[PLTFM_X532PT] = {}
THERMAL_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0] = [
    "Mainboard Front Left", "Mainboard Front Right",
    "Barefoot Ambient", "Barefoot Junction",
    "Fan 1", "Fan 2",
]
THERMAL_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT1] = _copy.deepcopy(THERMAL_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
THERMAL_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V2DOT0] = _copy.deepcopy(THERMAL_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])
THERMAL_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V3DOT0] = _copy.deepcopy(THERMAL_NAME[PLTFM_X532PT][BOARD_ID_X532PT_V1DOT0])

###########
# X564P-T #
###########
THERMAL_NAME[PLTFM_X564PT] = {}
THERMAL_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0] = [
    "Mainboard Front Left", "Mainboard Front Right",
    "Barefoot Ambient", "Barefoot Junction",
    "Fan 1", "Fan 2",
]
THERMAL_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT1] = _copy.deepcopy(THERMAL_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
THERMAL_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT2] = _copy.deepcopy(THERMAL_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V1DOT0])
THERMAL_NAME[PLTFM_X564PT][BOARD_ID_X564PT_V2DOT0] = [
    "Mainboard Front Left", "Mainboard Front Right",
    "Barefoot Ambient", "Barefoot Junction",
    "Fan 1", "Fan 2",
    "Mainboard Rear Left", "Mainboard Rear Right",
]

###########
# X732Q-T #
###########
THERMAL_NAME[PLTFM_X732QT] = {}
THERMAL_NAME[PLTFM_X732QT][BOARD_ID_X732QT_V1DOT0] = [
    "Mainboard Front Left", "Mainboard Front Right",
    "Fan 1", "Fan 2",
    "Barefoot Ambient", "Barefoot Junction",
]
# fmt: on
