######################################################################
# Asterfusion CX-T Devices Transceiver                               #
#                                                                    #
# Sfp contains an implementation of SONiC Platform Base API and      #
# provides the sfp device status which are available in the platform #
#                                                                    #
######################################################################

try:
    import os
    import sys

    if sys.version_info.major == 3:
        from io import StringIO
    elif sys.version_info.major == 2:
        from cStringIO import StringIO  # type: ignore

    from .constants import *
    from .helper import APIHelper

    from sonic_platform_base.sfp_base import SfpBase
    from sonic_platform_base.sonic_sfp.sff8472 import sff8472InterfaceId
    from sonic_platform_base.sonic_sfp.sff8472 import sff8472Dom
    from sonic_platform_base.sonic_sfp.sff8436 import sff8436InterfaceId
    from sonic_platform_base.sonic_sfp.sff8436 import sff8436Dom
    from sonic_platform_base.sonic_sfp.inf8628 import inf8628InterfaceId
    from sonic_platform_base.sonic_sfp.sfputilhelper import SfpUtilHelper
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")

INFO_OFFSET = 128
DOM_OFFSET = 0

# definitions of the offset and width for values in XCVR info eeprom
XCVR_INTFACE_BULK_OFFSET = 0
XCVR_INTFACE_BULK_WIDTH_QSFP = 20
XCVR_INTFACE_BULK_WIDTH_SFP = 21
XCVR_TYPE_OFFSET = 0
XCVR_TYPE_WIDTH = 1
XCVR_EXT_TYPE_OFFSET = 1
XCVR_EXT_TYPE_WIDTH = 1
XCVR_CONNECTOR_OFFSET = 2
XCVR_CONNECTOR_WIDTH = 1
XCVR_COMPLIANCE_CODE_OFFSET = 3
XCVR_COMPLIANCE_CODE_WIDTH = 8
XCVR_ENCODING_OFFSET = 11
XCVR_ENCODING_WIDTH = 1
XCVR_NBR_OFFSET = 12
XCVR_NBR_WIDTH = 1
XCVR_EXT_RATE_SEL_OFFSET = 13
XCVR_EXT_RATE_SEL_WIDTH = 1
XCVR_CABLE_LENGTH_OFFSET = 14
XCVR_CABLE_LENGTH_WIDTH_QSFP = 5
XCVR_CABLE_LENGTH_WIDTH_SFP = 6
XCVR_VENDOR_NAME_OFFSET = 20
XCVR_VENDOR_NAME_WIDTH = 16
XCVR_VENDOR_OUI_OFFSET = 37
XCVR_VENDOR_OUI_WIDTH = 3
XCVR_VENDOR_PN_OFFSET = 40
XCVR_VENDOR_PN_WIDTH = 16
XCVR_HW_REV_OFFSET = 56
XCVR_HW_REV_WIDTH_QSFP = 2
XCVR_HW_REV_WIDTH_SFP = 4
XCVR_VENDOR_SN_OFFSET = 68
XCVR_VENDOR_SN_WIDTH = 16
XCVR_VENDOR_DATE_OFFSET = 84
XCVR_VENDOR_DATE_WIDTH = 8
XCVR_DOM_CAPABILITY_OFFSET = 92
XCVR_DOM_CAPABILITY_WIDTH = 2

XCVR_INTERFACE_DATA_START = 0
XCVR_INTERFACE_DATA_SIZE = 92

QSFP_DOM_BULK_DATA_START = 22
QSFP_DOM_BULK_DATA_SIZE = 36
SFP_DOM_BULK_DATA_START = 96
SFP_DOM_BULK_DATA_SIZE = 10

# Offset for values in QSFP eeprom
QSFP_DOM_REV_OFFSET = 1
QSFP_DOM_REV_WIDTH = 1
QSFP_TEMPE_OFFSET = 22
QSFP_TEMPE_WIDTH = 2
QSFP_VOLT_OFFSET = 26
QSFP_VOLT_WIDTH = 2
QSFP_VERSION_COMPLIANCE_OFFSET = 1
QSFP_VERSION_COMPLIANCE_WIDTH = 2
QSFP_CHANNL_MON_OFFSET = 34
QSFP_CHANNL_MON_WIDTH = 16
QSFP_CHANNL_MON_WITH_TX_POWER_WIDTH = 24
QSFP_CHANNL_DISABLE_STATUS_OFFSET = 86
QSFP_CHANNL_DISABLE_STATUS_WIDTH = 1
QSFP_CHANNL_RX_LOS_STATUS_OFFSET = 3
QSFP_CHANNL_RX_LOS_STATUS_WIDTH = 1
QSFP_CHANNL_TX_FAULT_STATUS_OFFSET = 4
QSFP_CHANNL_TX_FAULT_STATUS_WIDTH = 1
QSFP_CONTROL_OFFSET = 86
QSFP_CONTROL_WIDTH = 8
QSFP_MODULE_MONITOR_OFFSET = 0
QSFP_MODULE_MONITOR_WIDTH = 9
QSFP_POWEROVERRIDE_OFFSET = 93
QSFP_POWEROVERRIDE_WIDTH = 1
QSFP_POWEROVERRIDE_BIT = 0
QSFP_POWERSET_BIT = 1
QSFP_OPTION_VALUE_OFFSET = 192
QSFP_OPTION_VALUE_WIDTH = 4
QSFP_MODULE_UPPER_PAGE3_START = 384
# Added by SunZheng on 2023.3.22
# Here we should be aware that current BSP is adopting
# A non-normative approach so some offset is biased.
# Therefore, we need to fix them manually with some calcs.
QSFP_MODULE_THRESHOLD_OFFSET = 128 - 128
QSFP_MODULE_THRESHOLD_WIDTH = 24
QSFP_CHANNL_THRESHOLD_OFFSET = 176 - 128
QSFP_CHANNL_THRESHOLD_WIDTH = 24

SFP_MODULE_ADDRA2_OFFSET = 256
SFP_MODULE_THRESHOLD_OFFSET = 0
SFP_MODULE_THRESHOLD_WIDTH = 56
SFP_CHANNL_THRESHOLD_OFFSET = 112
SFP_CHANNL_THRESHOLD_WIDTH = 2

SFP_TEMPE_OFFSET = 96
SFP_TEMPE_WIDTH = 2
SFP_VOLT_OFFSET = 98
SFP_VOLT_WIDTH = 2
SFP_CHANNL_MON_OFFSET = 100
SFP_CHANNL_MON_WIDTH = 6
SFP_CHANNL_STATUS_OFFSET = 110
SFP_CHANNL_STATUS_WIDTH = 1


QSFP_CABLE_LENGTH_TUP_KEYS = (
    "Length(km)",
    "Length OM3(2m)",
    "Length OM2(m)",
    "Length OM1(m)",
    "Length Cable Assembly(m)",
)

QSFP_COMPLIANCE_CODE_TUP_KEYS = (
    "10/40G Ethernet Compliance Code",
    "SONET Compliance codes",
    "SAS/SATA compliance codes",
    "Gigabit Ethernet Compliant codes",
    "Fibre Channel link length/Transmitter Technology",
    "Fibre Channel transmission media",
    "Fibre Channel Speed",
)

SFP_CABLE_LENGTH_TUP_KEYS = (
    "LengthSMFkm-UnitsOfKm",
    "LengthSMF(UnitsOf100m)",
    "Length50um(UnitsOf10m)",
    "Length62.5um(UnitsOfm)",
    "LengthCable(UnitsOfm)",
    "LengthOM3(UnitsOf10m)",
)

SFP_COMPLIANCE_CODE_TUP_KEYS = (
    "10GEthernetComplianceCode",
    "InfinibandComplianceCode",
    "ESCONComplianceCodes",
    "SONETComplianceCodes",
    "EthernetComplianceCodes",
    "FibreChannelLinkLength",
    "FibreChannelTechnology",
    "SFP+CableTechnology",
    "FibreChannelTransmissionMedia",
    "FibreChannelSpeed",
)

SFP_INFO_DICT_KEYS = (
    "type",
    "vendor_rev",
    "serial",
    "manufacturer",
    "model",
    "connector",
    "encoding",
    "ext_identifier",
    "ext_rateselect_compliance",
    "cable_type",
    "cable_length",
    "nominal_bit_rate",
    "specification_compliance",
    "vendor_date",
    "vendor_oui",
)

SFP_DOM_INFO_DICT_KEYS = (
    "rx_los",
    "tx_fault",
    "reset_status",
    "power_lpmode",
    "tx_disable",
    "tx_disable_channel",
    "temperature",
    "voltage",
    "rx1power",
    "rx2power",
    "rx3power",
    "rx4power",
    "tx1bias",
    "tx2bias",
    "tx3bias",
    "tx4bias",
    "tx1power",
    "tx2power",
    "tx3power",
    "tx4power",
)

SFP_THRESHOLD_INFO_DICT_KEYS = (
    "temphighalarm",
    "temphighwarning",
    "templowalarm",
    "templowwarning",
    "vcchighalarm",
    "vcchighwarning",
    "vcclowalarm",
    "vcclowwarning",
    "rxpowerhighalarm",
    "rxpowerhighwarning",
    "rxpowerlowalarm",
    "rxpowerlowwarning",
    "txpowerhighalarm",
    "txpowerhighwarning",
    "txpowerlowalarm",
    "txpowerlowwarning",
    "txbiashighalarm",
    "txbiashighwarning",
    "txbiaslowalarm",
    "txbiaslowwarning",
)


class Sfp(SfpBase):
    """Platform-specific Sfp class"""

    def __init__(self, sfp_index, helper, hwver, hwsku, platform):
        SfpBase.__init__(self)
        self._api_helper = helper  # type: APIHelper
        # Init device
        self.hwver = hwver
        self.hwsku = hwsku
        self.platform = platform
        # Init index
        self.sfp_index = sfp_index
        self.sfp_presence = False
        self._init_sfp_type()
        self._init_port_num()
        self._init_port_name()
        self._update_sfp_presence()
        self._update_sfp_eeprom()

    def _init_sfp_type(self):
        self.sfp_type = "Unknown"
        xsfp_index_type_list = None
        if self.hwsku == HWSKU_X308PT:
            xsfp_index_type_list = X308PT_XSFP_INDEX_TYPE
        elif self.hwsku == HWSKU_X312PT:
            xsfp_index_type_list = X312PT_XSFP_INDEX_TYPE
        elif self.hwsku == HWSKU_X532PT:
            xsfp_index_type_list = X532PT_XSFP_INDEX_TYPE
        elif self.hwsku == HWSKU_X564PT:
            xsfp_index_type_list = X564PT_XSFP_INDEX_TYPE
        assert xsfp_index_type_list is not None, "invalid index type list"
        for start, end, type in xsfp_index_type_list:
            if self.sfp_index in range(start, end):
                self.sfp_type = type
        assert self.sfp_type != "Unknown", "unknown sfp type"

    def _init_port_num(self):
        self.port_num = 0
        xsfp_index_type_list = None
        xsfp_index_seg_diff = 0
        if self.hwsku == HWSKU_X308PT:
            xsfp_index_type_list = X308PT_XSFP_INDEX_TYPE
        elif self.hwsku == HWSKU_X312PT:
            xsfp_index_type_list = X312PT_XSFP_INDEX_TYPE
        elif self.hwsku == HWSKU_X532PT:
            xsfp_index_type_list = X532PT_XSFP_INDEX_TYPE
        elif self.hwsku == HWSKU_X564PT:
            xsfp_index_type_list = X564PT_XSFP_INDEX_TYPE
        assert xsfp_index_type_list is not None, "invalid index type list"
        for start, end, _ in xsfp_index_type_list:
            if self.sfp_index in range(start, end):
                if self.sfp_type == DPU_PORT_TYPE:
                    self.port_num = DPU_PORT_NUM
                else:
                    self.port_num = self.sfp_index + 1 - xsfp_index_seg_diff
            xsfp_index_seg_diff = end - start
        assert self.port_num != 0, "invalid sfp port num"

    def _init_port_name(self):
        if self._api_helper.check_if_host():
            platform_json_path = os.path.join(
                DEVICE_ROOT, self.platform, self.hwsku, "platform.json"
            )
            port_config_path = os.path.join(
                DEVICE_ROOT, self.platform, self.hwsku, "port_config.ini"
            )
        else:
            platform_json_path = os.path.join(PMON_HWSKU, "platform.json")
            port_config_path = os.path.join(PMON_HWSKU, "port_config.ini")
        sfputil_helper = SfpUtilHelper()
        if os.path.exists(platform_json_path):
            sfputil_helper.read_porttab_mappings(platform_json_path)
        elif os.path.exists(port_config_path):
            sfputil_helper.read_porttab_mappings(port_config_path)
        self.port_name = copy.deepcopy(sfputil_helper.logical)

    def _update_sfp_presence(self):
        with self._api_helper.thrift_connect() as client:
            if self.sfp_type == SFP_TYPE:
                self.sfp_presence = bool(
                    client.pltfm_mgr_sfp_presence_get(self.port_num)
                )
            elif self.sfp_type == QSFP_TYPE:
                self.sfp_presence = bool(
                    client.pltfm_mgr_qsfp_presence_get(self.port_num)
                )
            elif self.sfp_type == DPU_PORT_TYPE:
                self.sfp_presence = False

    def _update_sfp_eeprom(self):
        self.sfp_data = bytearray.fromhex("").decode("unicode-escape")
        with self._api_helper.thrift_connect() as client:
            try:
                if self.sfp_type == SFP_TYPE:
                    if self.sfp_presence:
                        self.sfp_data = bytearray.fromhex(
                            client.pltfm_mgr_sfp_info_get(self.port_num)
                        ).decode("unicode-escape")
                elif self.sfp_type == QSFP_TYPE:
                    if self.sfp_presence:
                        self.sfp_data = bytearray.fromhex(
                            client.pltfm_mgr_qsfp_info_get(self.port_num)
                        ).decode("unicode-escape")
                elif self.sfp_type == DPU_PORT_TYPE:
                    pass
            except Exception as err:
                self._api_helper.log_debug(
                    "Failed to update transceiver EEPROM due to {}\n".format(err)
                )

    def _dom_capability_detect(self):
        self.dom_supported = False
        if not self.sfp_presence:
            self.dom_supported = False
            self.dom_temp_supported = False
            self.dom_volt_supported = False
            self.dom_rx_power_supported = False
            self.dom_tx_power_supported = False
            self.calibration = 0
            return

        if self.sfp_type == QSFP_TYPE:
            self.calibration = 1
            sfpi_obj = sff8436InterfaceId()
            if sfpi_obj is None:
                self.dom_supported = False
            offset = 128

            # QSFP capability byte parse, through this byte can know whether it support tx_power or not.
            # TODO: in the future when decided to migrate to support SFF-8636 instead of SFF-8436,
            # need to add more code for determining the capability and version compliance
            # in SFF-8636 dom capability definitions evolving with the versions.
            qsfp_dom_capability_raw = self._read_eeprom_specific_bytes(
                (offset + XCVR_DOM_CAPABILITY_OFFSET), XCVR_DOM_CAPABILITY_WIDTH
            )
            if qsfp_dom_capability_raw is not None:
                qsfp_version_compliance_raw = self._read_eeprom_specific_bytes(
                    QSFP_VERSION_COMPLIANCE_OFFSET, QSFP_VERSION_COMPLIANCE_WIDTH
                )
                qsfp_version_compliance = int(qsfp_version_compliance_raw[0], 16)
                dom_capability = sfpi_obj.parse_dom_capability(
                    qsfp_dom_capability_raw, 0
                )
                if qsfp_version_compliance > 0x08:
                    self.dom_temp_supported = (
                        dom_capability["data"]["Temp_support"]["value"] == "On"
                    )
                    self.dom_volt_supported = (
                        dom_capability["data"]["Voltage_support"]["value"] == "On"
                    )
                    self.dom_rx_power_supported = (
                        dom_capability["data"]["Rx_power_support"]["value"] == "On"
                    )
                    self.dom_tx_power_supported = (
                        dom_capability["data"]["Tx_power_support"]["value"] == "On"
                    )
                else:
                    self.dom_temp_supported = True
                    self.dom_volt_supported = True
                    self.dom_rx_power_supported = (
                        dom_capability["data"]["Rx_power_support"]["value"] == "On"
                    )
                    self.dom_tx_power_supported = True

                self.dom_supported = True
                self.calibration = 1
                sfpd_obj = sff8436Dom()
                if sfpd_obj is None:
                    return None
                qsfp_option_value_raw = self._read_eeprom_specific_bytes(
                    QSFP_OPTION_VALUE_OFFSET, QSFP_OPTION_VALUE_WIDTH
                )
                if qsfp_option_value_raw is not None:
                    optional_capability = sfpd_obj.parse_option_params(
                        qsfp_option_value_raw, 0
                    )
                    self.dom_tx_disable_supported = (
                        optional_capability["data"]["TxDisable"]["value"] == "On"
                    )
                dom_status_indicator = sfpd_obj.parse_dom_status_indicator(
                    qsfp_version_compliance_raw, 1
                )
                self.qsfp_page3_available = (
                    dom_status_indicator["data"]["FlatMem"]["value"] == "Off"
                )
            else:
                self.dom_supported = False
                self.dom_temp_supported = False
                self.dom_volt_supported = False
                self.dom_rx_power_supported = False
                self.dom_tx_power_supported = False
                self.calibration = 0
                self.qsfp_page3_available = False

        elif self.sfp_type == SFP_TYPE:
            sfpi_obj = sff8472InterfaceId()
            if sfpi_obj is None:
                return None
            sfp_dom_capability_raw = self._read_eeprom_specific_bytes(
                XCVR_DOM_CAPABILITY_OFFSET, XCVR_DOM_CAPABILITY_WIDTH
            )
            if sfp_dom_capability_raw is not None:
                sfp_dom_capability = int(sfp_dom_capability_raw[0], 16)
                self.dom_supported = sfp_dom_capability & 0x40 != 0
                if self.dom_supported:
                    self.dom_temp_supported = True
                    self.dom_volt_supported = True
                    self.dom_rx_power_supported = True
                    self.dom_tx_power_supported = True
                    if sfp_dom_capability & 0x20 != 0:
                        self.calibration = 1
                    elif sfp_dom_capability & 0x10 != 0:
                        self.calibration = 2
                    else:
                        self.calibration = 0
                else:
                    self.dom_temp_supported = False
                    self.dom_volt_supported = False
                    self.dom_rx_power_supported = False
                    self.dom_tx_power_supported = False
                    self.calibration = 0
                self.dom_tx_disable_supported = (
                    int(sfp_dom_capability_raw[1], 16) & 0x40 != 0
                )
        else:
            self.dom_supported = False
            self.dom_temp_supported = False
            self.dom_volt_supported = False
            self.dom_rx_power_supported = False
            self.dom_tx_power_supported = False

    def _convert_string_to_num(self, value_str):
        if "-inf" in value_str:
            return "N/A"
        elif "Unknown" in value_str:
            return "N/A"
        elif "dBm" in value_str:
            t_str = value_str.rstrip("dBm")
            return float(t_str)
        elif "mA" in value_str:
            t_str = value_str.rstrip("mA")
            return float(t_str)
        elif "C" in value_str:
            t_str = value_str.rstrip("C")
            return float(t_str)
        elif "Volts" in value_str:
            t_str = value_str.rstrip("Volts")
            return float(t_str)
        else:
            return "N/A"

    def _read_eeprom_specific_bytes(self, offset, num_bytes):
        sysfsfile_eeprom = None
        eeprom_raw = []
        for i in range(0, num_bytes):
            eeprom_raw.append("0x00")

        try:
            sysfsfile_eeprom = StringIO(self.sfp_data)
            sysfsfile_eeprom.seek(offset)
            raw = sysfsfile_eeprom.read(num_bytes)
            for n in range(0, num_bytes):
                eeprom_raw[n] = hex(ord(raw[n]))[2:].zfill(2)
        except Exception as err:
            self._api_helper.log_debug(
                "Failed to decode transceiver EEPROM due to {}\n".format(err)
            )
        finally:
            if sysfsfile_eeprom:
                sysfsfile_eeprom.close()

        return eeprom_raw

    def get_transceiver_info(self):
        """
        Retrieves transceiver info of this SFP
        Returns:
            A dict which contains following keys/values :
        ========================================================================
        keys                       |Value Format   |Information
        ---------------------------|---------------|----------------------------
        type                       |1*255VCHAR     |type of SFP
        vendor_rev                 |1*255VCHAR     |vendor revision of SFP
        serial                     |1*255VCHAR     |serial number of the SFP
        manufacturer               |1*255VCHAR     |SFP vendor name
        model                      |1*255VCHAR     |SFP model name
        connector                  |1*255VCHAR     |connector information
        encoding                   |1*255VCHAR     |encoding information
        ext_identifier             |1*255VCHAR     |extend identifier
        ext_rateselect_compliance  |1*255VCHAR     |extended rateSelect compliance
        cable_length               |INT            |cable length in m
        nominal_bit_rate           |INT            |nominal bit rate by 100Mbs
        specification_compliance   |1*255VCHAR     |specification compliance
        vendor_date                |1*255VCHAR     |vendor date
        vendor_oui                 |1*255VCHAR     |vendor OUI
        ========================================================================
        """
        self._update_sfp_presence()
        self._update_sfp_eeprom()
        compliance_code_dict = {}
        transceiver_info_dict = dict.fromkeys(SFP_INFO_DICT_KEYS, "N/A")
        if not self.sfp_presence:
            return transceiver_info_dict

        if self.sfp_type == QSFP_TYPE:
            offset = 128
            vendor_rev_width = XCVR_HW_REV_WIDTH_QSFP
            interface_info_bulk_width = XCVR_INTFACE_BULK_WIDTH_QSFP

            sfpi_obj = sff8436InterfaceId()
            if sfpi_obj is None:
                self._api_helper.log_debug("Error: sfp_object open failed")
                return None

        else:
            offset = 0
            vendor_rev_width = XCVR_HW_REV_WIDTH_SFP
            interface_info_bulk_width = XCVR_INTFACE_BULK_WIDTH_SFP

            sfpi_obj = sff8472InterfaceId()
            if sfpi_obj is None:
                self._api_helper.log_debug("Error: sfp_object open failed")
                return None
        sfp_interface_bulk_raw = self._read_eeprom_specific_bytes(
            offset + XCVR_INTERFACE_DATA_START, XCVR_INTERFACE_DATA_SIZE
        )
        if sfp_interface_bulk_raw is None:
            return None

        start = XCVR_INTFACE_BULK_OFFSET - XCVR_INTERFACE_DATA_START
        end = start + interface_info_bulk_width
        sfp_interface_bulk_data = sfpi_obj.parse_sfp_info_bulk(
            sfp_interface_bulk_raw[start:end], 0
        )

        start = XCVR_VENDOR_NAME_OFFSET - XCVR_INTERFACE_DATA_START
        end = start + XCVR_VENDOR_NAME_WIDTH
        sfp_vendor_name_data = sfpi_obj.parse_vendor_name(
            sfp_interface_bulk_raw[start:end], 0
        )

        start = XCVR_VENDOR_PN_OFFSET - XCVR_INTERFACE_DATA_START
        end = start + XCVR_VENDOR_PN_WIDTH
        sfp_vendor_pn_data = sfpi_obj.parse_vendor_pn(
            sfp_interface_bulk_raw[start:end], 0
        )

        start = XCVR_HW_REV_OFFSET - XCVR_INTERFACE_DATA_START
        end = start + vendor_rev_width
        sfp_vendor_rev_data = sfpi_obj.parse_vendor_rev(
            sfp_interface_bulk_raw[start:end], 0
        )

        start = XCVR_VENDOR_SN_OFFSET - XCVR_INTERFACE_DATA_START
        end = start + XCVR_VENDOR_SN_WIDTH
        sfp_vendor_sn_data = sfpi_obj.parse_vendor_sn(
            sfp_interface_bulk_raw[start:end], 0
        )

        start = XCVR_VENDOR_OUI_OFFSET - XCVR_INTERFACE_DATA_START
        end = start + XCVR_VENDOR_OUI_WIDTH
        sfp_vendor_oui_data = sfpi_obj.parse_vendor_oui(
            sfp_interface_bulk_raw[start:end], 0
        )

        start = XCVR_VENDOR_DATE_OFFSET - XCVR_INTERFACE_DATA_START
        end = start + XCVR_VENDOR_DATE_WIDTH
        sfp_vendor_date_data = sfpi_obj.parse_vendor_date(
            sfp_interface_bulk_raw[start:end], 0
        )

        transceiver_info_dict["type"] = sfp_interface_bulk_data["data"]["type"]["value"]
        transceiver_info_dict["type_abbrv_name"] = sfp_interface_bulk_data["data"][
            "type_abbrv_name"
        ]["value"]
        transceiver_info_dict["manufacturer"] = sfp_vendor_name_data["data"][
            "Vendor Name"
        ]["value"]
        transceiver_info_dict["model"] = sfp_vendor_pn_data["data"]["Vendor PN"][
            "value"
        ]
        transceiver_info_dict["vendor_rev"] = sfp_vendor_rev_data["data"][
            "Vendor Rev"
        ]["value"]
        transceiver_info_dict["serial"] = sfp_vendor_sn_data["data"]["Vendor SN"][
            "value"
        ]
        transceiver_info_dict["vendor_oui"] = sfp_vendor_oui_data["data"]["Vendor OUI"][
            "value"
        ]
        transceiver_info_dict["vendor_date"] = sfp_vendor_date_data["data"][
            "VendorDataCode(YYYY-MM-DD Lot)"
        ]["value"]
        transceiver_info_dict["connector"] = sfp_interface_bulk_data["data"][
            "Connector"
        ]["value"]
        transceiver_info_dict["encoding"] = sfp_interface_bulk_data["data"][
            "EncodingCodes"
        ]["value"]
        transceiver_info_dict["ext_identifier"] = sfp_interface_bulk_data["data"][
            "Extended Identifier"
        ]["value"]
        transceiver_info_dict["ext_rateselect_compliance"] = sfp_interface_bulk_data[
            "data"
        ]["RateIdentifier"]["value"]
        transceiver_info_dict["cable_type"] = "Unknown"
        transceiver_info_dict["cable_length"] = "Unknown"
        transceiver_info_dict["application_advertisement"] = "N/A"

        if self.sfp_type == QSFP_TYPE:
            for key in QSFP_CABLE_LENGTH_TUP_KEYS:
                if key in sfp_interface_bulk_data["data"]:
                    transceiver_info_dict["cable_type"] = key
                    transceiver_info_dict["cable_length"] = str(
                        sfp_interface_bulk_data["data"][key]["value"]
                    )

            for key in QSFP_COMPLIANCE_CODE_TUP_KEYS:
                if (
                    key
                    in sfp_interface_bulk_data["data"]["Specification compliance"][
                        "value"
                    ]
                ):
                    compliance_code_dict[key] = sfp_interface_bulk_data["data"][
                        "Specification compliance"
                    ]["value"][key]["value"]
            transceiver_info_dict["specification_compliance"] = str(
                compliance_code_dict
            )

            transceiver_info_dict["nominal_bit_rate"] = str(
                sfp_interface_bulk_data["data"]["Nominal Bit Rate(100Mbs)"]["value"]
            )
        else:
            for key in SFP_CABLE_LENGTH_TUP_KEYS:
                if key in sfp_interface_bulk_data["data"]:
                    transceiver_info_dict["cable_type"] = key
                    transceiver_info_dict["cable_length"] = str(
                        sfp_interface_bulk_data["data"][key]["value"]
                    )

            for key in SFP_COMPLIANCE_CODE_TUP_KEYS:
                if (
                    key
                    in sfp_interface_bulk_data["data"]["Specification compliance"][
                        "value"
                    ]
                ):
                    compliance_code_dict[key] = sfp_interface_bulk_data["data"][
                        "Specification compliance"
                    ]["value"][key]["value"]
            transceiver_info_dict["specification_compliance"] = str(
                compliance_code_dict
            )

            transceiver_info_dict["nominal_bit_rate"] = str(
                sfp_interface_bulk_data["data"]["NominalSignallingRate(UnitsOf100Mbd)"][
                    "value"
                ]
            )

        return transceiver_info_dict

    def get_transceiver_bulk_status(self):
        """
        Retrieves transceiver bulk status of this SFP
        Returns:
            A dict which contains following keys/values :
        ========================================================================
        keys                       |Value Format   |Information
        ---------------------------|---------------|----------------------------
        rx_los                     |BOOLEAN        |RX loss-of-signal status, True if has RX los, False if not.
        tx_fault                   |BOOLEAN        |TX fault status, True if has TX fault, False if not.
        reset_status               |BOOLEAN        |reset status, True if SFP in reset, False if not.
        lp_mode                    |BOOLEAN        |low power mode status, True in lp mode, False if not.
        tx_disable                 |BOOLEAN        |TX disable status, True TX disabled, False if not.
        tx_disabled_channel        |HEX            |disabled TX channels in hex, bits 0 to 3 represent channel 0
                                   |               |to channel 3.
        temperature                |INT            |module temperature in Celsius
        voltage                    |INT            |supply voltage in mV
        tx<n>bias                  |INT            |TX Bias Current in mA, n is the channel number,
                                   |               |for example, tx2bias stands for tx bias of channel 2.
        rx<n>power                 |INT            |received optical power in mW, n is the channel number,
                                   |               |for example, rx2power stands for rx power of channel 2.
        tx<n>power                 |INT            |TX output power in mW, n is the channel number,
                                   |               |for example, tx2power stands for tx power of channel 2.
        ========================================================================
        """
        self._update_sfp_presence()
        self._update_sfp_eeprom()
        self._dom_capability_detect()
        transceiver_dom_info_dict = dict.fromkeys(SFP_DOM_INFO_DICT_KEYS, "N/A")

        if not self.sfp_presence:
            return transceiver_dom_info_dict

        if self.sfp_type == QSFP_TYPE:
            if not self.dom_supported:
                return transceiver_dom_info_dict

            offset = 0
            sfpd_obj = sff8436Dom()
            if sfpd_obj is None:
                return transceiver_dom_info_dict

            dom_data_raw = self._read_eeprom_specific_bytes(
                (offset + QSFP_DOM_BULK_DATA_START), QSFP_DOM_BULK_DATA_SIZE
            )
            if dom_data_raw is None:
                return transceiver_dom_info_dict

            if self.dom_temp_supported:
                start = QSFP_TEMPE_OFFSET - QSFP_DOM_BULK_DATA_START
                end = start + QSFP_TEMPE_WIDTH
                dom_temperature_data = sfpd_obj.parse_temperature(
                    dom_data_raw[start:end], 0
                )
                temp = self._convert_string_to_num(
                    dom_temperature_data["data"]["Temperature"]["value"]
                )
                if temp is not None:
                    transceiver_dom_info_dict["temperature"] = temp

            if self.dom_volt_supported:
                start = QSFP_VOLT_OFFSET - QSFP_DOM_BULK_DATA_START
                end = start + QSFP_VOLT_WIDTH
                dom_voltage_data = sfpd_obj.parse_voltage(dom_data_raw[start:end], 0)
                volt = self._convert_string_to_num(
                    dom_voltage_data["data"]["Vcc"]["value"]
                )
                if volt is not None:
                    transceiver_dom_info_dict["voltage"] = volt

            start = QSFP_CHANNL_MON_OFFSET - QSFP_DOM_BULK_DATA_START
            end = start + QSFP_CHANNL_MON_WITH_TX_POWER_WIDTH
            dom_channel_monitor_data = (
                sfpd_obj.parse_channel_monitor_params_with_tx_power(
                    dom_data_raw[start:end], 0
                )
            )

            if self.dom_tx_power_supported:
                transceiver_dom_info_dict["tx1power"] = self._convert_string_to_num(
                    dom_channel_monitor_data["data"]["TX1Power"]["value"]
                )
                transceiver_dom_info_dict["tx2power"] = self._convert_string_to_num(
                    dom_channel_monitor_data["data"]["TX2Power"]["value"]
                )
                transceiver_dom_info_dict["tx3power"] = self._convert_string_to_num(
                    dom_channel_monitor_data["data"]["TX3Power"]["value"]
                )
                transceiver_dom_info_dict["tx4power"] = self._convert_string_to_num(
                    dom_channel_monitor_data["data"]["TX4Power"]["value"]
                )

            if self.dom_rx_power_supported:
                transceiver_dom_info_dict["rx1power"] = self._convert_string_to_num(
                    dom_channel_monitor_data["data"]["RX1Power"]["value"]
                )
                transceiver_dom_info_dict["rx2power"] = self._convert_string_to_num(
                    dom_channel_monitor_data["data"]["RX2Power"]["value"]
                )
                transceiver_dom_info_dict["rx3power"] = self._convert_string_to_num(
                    dom_channel_monitor_data["data"]["RX3Power"]["value"]
                )
                transceiver_dom_info_dict["rx4power"] = self._convert_string_to_num(
                    dom_channel_monitor_data["data"]["RX4Power"]["value"]
                )

            transceiver_dom_info_dict["tx1bias"] = dom_channel_monitor_data["data"][
                "TX1Bias"
            ]["value"]
            transceiver_dom_info_dict["tx2bias"] = dom_channel_monitor_data["data"][
                "TX2Bias"
            ]["value"]
            transceiver_dom_info_dict["tx3bias"] = dom_channel_monitor_data["data"][
                "TX3Bias"
            ]["value"]
            transceiver_dom_info_dict["tx4bias"] = dom_channel_monitor_data["data"][
                "TX4Bias"
            ]["value"]

        else:
            if not self.dom_supported:
                return transceiver_dom_info_dict

            offset = 256
            sfpd_obj = sff8472Dom()
            if sfpd_obj is None:
                return transceiver_dom_info_dict
            sfpd_obj._calibration_type = self.calibration

            dom_data_raw = self._read_eeprom_specific_bytes(
                (offset + SFP_DOM_BULK_DATA_START), SFP_DOM_BULK_DATA_SIZE
            )
            if dom_data_raw is None:
                return transceiver_dom_info_dict

            start = SFP_TEMPE_OFFSET - SFP_DOM_BULK_DATA_START
            end = start + SFP_TEMPE_WIDTH
            dom_temperature_data = sfpd_obj.parse_temperature(
                dom_data_raw[start:end], 0
            )

            start = SFP_VOLT_OFFSET - SFP_DOM_BULK_DATA_START
            end = start + SFP_VOLT_WIDTH
            dom_voltage_data = sfpd_obj.parse_voltage(dom_data_raw[start:end], 0)

            start = SFP_CHANNL_MON_OFFSET - SFP_DOM_BULK_DATA_START
            end = start + SFP_CHANNL_MON_WIDTH
            dom_channel_monitor_data = sfpd_obj.parse_channel_monitor_params(
                dom_data_raw[start:end], 0
            )

            transceiver_dom_info_dict["temperature"] = self._convert_string_to_num(
                dom_temperature_data["data"]["Temperature"]["value"]
            )
            transceiver_dom_info_dict["voltage"] = self._convert_string_to_num(
                dom_voltage_data["data"]["Vcc"]["value"]
            )
            transceiver_dom_info_dict["rx1power"] = self._convert_string_to_num(
                dom_channel_monitor_data["data"]["RXPower"]["value"]
            )
            transceiver_dom_info_dict["tx1bias"] = self._convert_string_to_num(
                dom_channel_monitor_data["data"]["TXBias"]["value"]
            )
            transceiver_dom_info_dict["tx1power"] = self._convert_string_to_num(
                dom_channel_monitor_data["data"]["TXPower"]["value"]
            )

        transceiver_dom_info_dict["rx_los"] = self.get_rx_los()
        transceiver_dom_info_dict["tx_fault"] = self.get_tx_fault()
        transceiver_dom_info_dict["reset_status"] = self.get_reset_status()
        transceiver_dom_info_dict["lp_mode"] = self.get_lpmode()

        return transceiver_dom_info_dict

    def get_transceiver_threshold_info(self):
        """
        Retrieves transceiver threshold info of this SFP
        Returns:
            A dict which contains following keys/values :
        ========================================================================
        keys                       |Value Format   |Information
        ---------------------------|---------------|----------------------------
        temphighalarm              |FLOAT          |High Alarm Threshold value of temperature in Celsius.
        templowalarm               |FLOAT          |Low Alarm Threshold value of temperature in Celsius.
        temphighwarning            |FLOAT          |High Warning Threshold value of temperature in Celsius.
        templowwarning             |FLOAT          |Low Warning Threshold value of temperature in Celsius.
        vcchighalarm               |FLOAT          |High Alarm Threshold value of supply voltage in mV.
        vcclowalarm                |FLOAT          |Low Alarm Threshold value of supply voltage in mV.
        vcchighwarning             |FLOAT          |High Warning Threshold value of supply voltage in mV.
        vcclowwarning              |FLOAT          |Low Warning Threshold value of supply voltage in mV.
        rxpowerhighalarm           |FLOAT          |High Alarm Threshold value of received power in dBm.
        rxpowerlowalarm            |FLOAT          |Low Alarm Threshold value of received power in dBm.
        rxpowerhighwarning         |FLOAT          |High Warning Threshold value of received power in dBm.
        rxpowerlowwarning          |FLOAT          |Low Warning Threshold value of received power in dBm.
        txpowerhighalarm           |FLOAT          |High Alarm Threshold value of transmit power in dBm.
        txpowerlowalarm            |FLOAT          |Low Alarm Threshold value of transmit power in dBm.
        txpowerhighwarning         |FLOAT          |High Warning Threshold value of transmit power in dBm.
        txpowerlowwarning          |FLOAT          |Low Warning Threshold value of transmit power in dBm.
        txbiashighalarm            |FLOAT          |High Alarm Threshold value of tx Bias Current in mA.
        txbiaslowalarm             |FLOAT          |Low Alarm Threshold value of tx Bias Current in mA.
        txbiashighwarning          |FLOAT          |High Warning Threshold value of tx Bias Current in mA.
        txbiaslowwarning           |FLOAT          |Low Warning Threshold value of tx Bias Current in mA.
        ========================================================================
        """
        self._update_sfp_presence()
        self._update_sfp_eeprom()
        self._dom_capability_detect()
        transceiver_dom_threshold_info_dict = dict.fromkeys(
            SFP_THRESHOLD_INFO_DICT_KEYS, "N/A"
        )

        if not self.sfp_presence:
            return transceiver_dom_threshold_info_dict

        if self.sfp_type == QSFP_TYPE:
            # Dom Threshold data starts from offset 384
            # Revert offset back to 0 once data is retrieved
            offset = QSFP_MODULE_UPPER_PAGE3_START

            if not self.dom_supported or not self.qsfp_page3_available:
                return transceiver_dom_threshold_info_dict

            sfpd_obj = sff8436Dom()
            if sfpd_obj is None:
                return transceiver_dom_threshold_info_dict

            dom_module_threshold_raw = self._read_eeprom_specific_bytes(
                (offset + QSFP_MODULE_THRESHOLD_OFFSET), QSFP_MODULE_THRESHOLD_WIDTH
            )
            if dom_module_threshold_raw is None:
                return transceiver_dom_threshold_info_dict

            dom_module_threshold_data = sfpd_obj.parse_module_threshold_values(
                dom_module_threshold_raw, 0
            )

            dom_channel_threshold_raw = self._read_eeprom_specific_bytes(
                (offset + QSFP_CHANNL_THRESHOLD_OFFSET), QSFP_CHANNL_THRESHOLD_WIDTH
            )
            if dom_channel_threshold_raw is None:
                return transceiver_dom_threshold_info_dict

            dom_channel_threshold_data = sfpd_obj.parse_channel_threshold_values(
                dom_channel_threshold_raw, 0
            )

            # Threshold Data
            # Module Threshold
            transceiver_dom_threshold_info_dict[
                "temphighalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["TempHighAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "temphighwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["TempHighWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "templowalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["TempLowAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "templowwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["TempLowWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "vcchighalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["VccHighAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "vcchighwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["VccHighWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "vcclowalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["VccLowAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "vcclowwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["VccLowWarning"]["value"]
            )
            # Channel Threshold
            transceiver_dom_threshold_info_dict[
                "rxpowerhighalarm"
            ] = self._convert_string_to_num(
                dom_channel_threshold_data["data"]["RxPowerHighAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "rxpowerhighwarning"
            ] = self._convert_string_to_num(
                dom_channel_threshold_data["data"]["RxPowerHighWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "rxpowerlowalarm"
            ] = self._convert_string_to_num(
                dom_channel_threshold_data["data"]["RxPowerLowAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "rxpowerlowwarning"
            ] = self._convert_string_to_num(
                dom_channel_threshold_data["data"]["RxPowerLowWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txbiashighalarm"
            ] = self._convert_string_to_num(
                dom_channel_threshold_data["data"]["TxBiasHighAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txbiashighwarning"
            ] = self._convert_string_to_num(
                dom_channel_threshold_data["data"]["TxBiasHighWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txbiaslowalarm"
            ] = self._convert_string_to_num(
                dom_channel_threshold_data["data"]["TxBiasLowAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txbiaslowwarning"
            ] = self._convert_string_to_num(
                dom_channel_threshold_data["data"]["TxBiasLowWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txpowerhighalarm"
            ] = self._convert_string_to_num(
                dom_channel_threshold_data["data"]["TxPowerHighAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txpowerhighwarning"
            ] = self._convert_string_to_num(
                dom_channel_threshold_data["data"]["TxPowerHighWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txpowerlowalarm"
            ] = self._convert_string_to_num(
                dom_channel_threshold_data["data"]["TxPowerLowAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txpowerlowwarning"
            ] = self._convert_string_to_num(
                dom_channel_threshold_data["data"]["TxPowerLowWarning"]["value"]
            )

        else:
            offset = SFP_MODULE_ADDRA2_OFFSET

            if not self.dom_supported:
                return transceiver_dom_threshold_info_dict

            sfpd_obj = sff8472Dom(None, self.calibration)
            if sfpd_obj is None:
                return transceiver_dom_threshold_info_dict

            dom_module_threshold_raw = self._read_eeprom_specific_bytes(
                (offset + SFP_MODULE_THRESHOLD_OFFSET), SFP_MODULE_THRESHOLD_WIDTH
            )
            if dom_module_threshold_raw is None:
                return transceiver_dom_threshold_info_dict

            dom_module_threshold_data = sfpd_obj.parse_alarm_warning_threshold(
                dom_module_threshold_raw, 0
            )

            # Module Threshold Data
            transceiver_dom_threshold_info_dict[
                "temphighalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["TempHighAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "templowalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["TempLowAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "temphighwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["TempHighWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "templowwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["TempLowWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "vcchighalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["VoltageHighAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "vcclowalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["VoltageLowAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "vcchighwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["VoltageHighWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "vcclowwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["VoltageLowWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txbiashighalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["BiasHighAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txbiaslowalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["BiasLowAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txbiashighwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["BiasHighWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txbiaslowwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["BiasLowWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txpowerhighalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["TXPowerHighAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txpowerlowalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["TXPowerLowAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txpowerhighwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["TXPowerHighWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "txpowerlowwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["TXPowerLowWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "rxpowerhighalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["RXPowerHighAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "rxpowerlowalarm"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["RXPowerLowAlarm"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "rxpowerhighwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["RXPowerHighWarning"]["value"]
            )
            transceiver_dom_threshold_info_dict[
                "rxpowerlowwarning"
            ] = self._convert_string_to_num(
                dom_module_threshold_data["data"]["RXPowerLowWarning"]["value"]
            )

        return transceiver_dom_threshold_info_dict

    def get_reset_status(self):
        """
        Retrieves the reset status of SFP
        Returns:
            A Boolean, True if reset enabled, False if disabled
        """
        self._update_sfp_presence()
        self._update_sfp_eeprom()
        self._dom_capability_detect()

        if not self.sfp_presence:
            return False

        if not self.dom_supported:
            return False

        if self.sfp_type == QSFP_TYPE:
            offset = 0
            dom_module_monitor_raw = self._read_eeprom_specific_bytes(
                (offset + QSFP_MODULE_MONITOR_OFFSET), QSFP_MODULE_MONITOR_WIDTH
            )

            if dom_module_monitor_raw is not None:
                return True
            else:
                return False
        elif self.sfp_type == SFP_TYPE:
            offset = 0
            dom_channel_monitor_raw = self._read_eeprom_specific_bytes(
                (offset + SFP_CHANNL_STATUS_OFFSET), SFP_CHANNL_STATUS_WIDTH
            )

            if dom_channel_monitor_raw is not None:
                return True
            else:
                return False

    def get_rx_los(self):
        """
        Retrieves the RX LOS (lost-of-signal) status of SFP
        Returns:
            A Boolean, True if SFP has RX LOS, False if not.
            Note : RX LOS status is latched until a call to get_rx_los or a reset.
        """
        self._update_sfp_presence()
        self._update_sfp_eeprom()
        self._dom_capability_detect()

        if not self.sfp_presence:
            return None

        if not self.dom_supported:
            return None

        rx_los_list = []
        if self.sfp_type == QSFP_TYPE:
            offset = 0
            dom_channel_monitor_raw = self._read_eeprom_specific_bytes(
                (offset + QSFP_CHANNL_RX_LOS_STATUS_OFFSET),
                QSFP_CHANNL_RX_LOS_STATUS_WIDTH,
            )
            if dom_channel_monitor_raw is not None:
                rx_los_data = int(dom_channel_monitor_raw[0], 16)
                rx_los_list.append(rx_los_data & 0x01 != 0)
                rx_los_list.append(rx_los_data & 0x02 != 0)
                rx_los_list.append(rx_los_data & 0x04 != 0)
                rx_los_list.append(rx_los_data & 0x08 != 0)
        else:
            offset = 256
            dom_channel_monitor_raw = self._read_eeprom_specific_bytes(
                (offset + SFP_CHANNL_STATUS_OFFSET), SFP_CHANNL_STATUS_WIDTH
            )
            if dom_channel_monitor_raw is not None:
                rx_los_data = int(dom_channel_monitor_raw[0], 16)
                rx_los_list.append(rx_los_data & 0x02 != 0)
            else:
                return None
        return rx_los_list

    def get_tx_fault(self):
        """
        Retrieves the TX fault status of SFP
        Returns:
            A Boolean, True if SFP has TX fault, False if not
            Note : TX fault status is lached until a call to get_tx_fault or a reset.
        """
        self._update_sfp_presence()
        self._update_sfp_eeprom()
        self._dom_capability_detect()

        if not self.sfp_presence:
            return None

        if not self.dom_supported:
            return None

        tx_fault_list = []
        if self.sfp_type == QSFP_TYPE:
            offset = 0
            dom_channel_monitor_raw = self._read_eeprom_specific_bytes(
                (offset + QSFP_CHANNL_TX_FAULT_STATUS_OFFSET),
                QSFP_CHANNL_TX_FAULT_STATUS_WIDTH,
            )
            if dom_channel_monitor_raw is not None:
                tx_fault_data = int(dom_channel_monitor_raw[0], 16)
                tx_fault_list.append(tx_fault_data & 0x01 != 0)
                tx_fault_list.append(tx_fault_data & 0x02 != 0)
                tx_fault_list.append(tx_fault_data & 0x04 != 0)
                tx_fault_list.append(tx_fault_data & 0x08 != 0)
        else:
            offset = 256
            dom_channel_monitor_raw = self._read_eeprom_specific_bytes(
                (offset + SFP_CHANNL_STATUS_OFFSET), SFP_CHANNL_STATUS_WIDTH
            )
            if dom_channel_monitor_raw is not None:
                tx_fault_data = int(dom_channel_monitor_raw[0], 16)
                tx_fault_list.append(tx_fault_data & 0x04 != 0)
            else:
                return None
        return tx_fault_list

    def get_tx_disable(self):
        """
        Retrieves the tx_disable status of this SFP
        Returns:
            A Boolean, True if tx_disable is enabled, False if disabled
        """
        self._update_sfp_presence()
        self._update_sfp_eeprom()
        self._dom_capability_detect()

        if not self.sfp_presence:
            return None

        if not self.dom_supported:
            return None

        tx_disable_list = []
        if self.sfp_type == QSFP_TYPE:
            offset = 0
            dom_channel_monitor_raw = self._read_eeprom_specific_bytes(
                (offset + QSFP_CHANNL_DISABLE_STATUS_OFFSET),
                QSFP_CHANNL_DISABLE_STATUS_WIDTH,
            )
            if dom_channel_monitor_raw is not None:
                tx_disable_data = int(dom_channel_monitor_raw[0], 16)
                tx_disable_list.append(tx_disable_data & 0x01 != 0)
                tx_disable_list.append(tx_disable_data & 0x02 != 0)
                tx_disable_list.append(tx_disable_data & 0x04 != 0)
                tx_disable_list.append(tx_disable_data & 0x08 != 0)
        else:
            offset = 256
            dom_channel_monitor_raw = self._read_eeprom_specific_bytes(
                (offset + SFP_CHANNL_STATUS_OFFSET), SFP_CHANNL_STATUS_WIDTH
            )
            if dom_channel_monitor_raw is not None:
                tx_disable_data = int(dom_channel_monitor_raw[0], 16)
                tx_disable_list.append(tx_disable_data & 0xC0 != 0)
            else:
                return None
        return tx_disable_list

    def get_tx_disable_channel(self):
        """
        Retrieves the TX disabled channels in this SFP
        Returns:
            A hex of 4 bits (bit 0 to bit 3 as channel 0 to channel 3) to represent
            TX channels which have been disabled in this SFP.
            As an example, a returned value of 0x5 indicates that channel 0
            and channel 2 have been disabled.
        """
        self._update_sfp_presence()
        self._update_sfp_eeprom()
        if not self.sfp_presence:
            return 0
        tx_disable_list = self.get_tx_disable()
        if tx_disable_list is None:
            return 0
        tx_disabled = 0
        for i in range(0, len(tx_disable_list)):
            if tx_disable_list[i]:
                tx_disabled |= 1 << i
        return tx_disabled

    def get_lpmode(self):
        """
        Retrieves the lpmode (low power mode) status of this SFP
        Returns:
            A Boolean, True if lpmode is enabled, False if disabled
        """
        if self.sfp_type == QSFP_TYPE:
            with self._api_helper.thrift_connect() as client:
                return client.pltfm_mgr_qsfp_lpmode_get(self.port_num)
        return False

    def get_power_override(self):
        """
        Retrieves the power-override status of this SFP
        Returns:
            A Boolean, True if power-override is enabled, False if disabled
        """
        self._update_sfp_presence()
        self._update_sfp_eeprom()
        if not self.sfp_presence:
            return False
        if self.sfp_type == QSFP_TYPE:
            sfpd_obj = sff8436Dom()
            if sfpd_obj is None:
                return False

            dom_control_raw = self._read_eeprom_specific_bytes(
                QSFP_CONTROL_OFFSET, QSFP_CONTROL_WIDTH
            )
            dom_control_data = sfpd_obj.parse_control_bytes(dom_control_raw, 0)
            power_override = "On" == dom_control_data["data"]["PowerOverride"]["value"]
            return power_override
        else:
            return False

    def get_temperature(self):
        """
        Retrieves the temperature of this SFP
        Returns:
            An integer number of current temperature in Celsius
        """
        return self.get_transceiver_bulk_status().get("temperature", "N/A")

    def get_voltage(self):
        """
        Retrieves the supply voltage of this SFP
        Returns:
            An integer number of supply voltage in mV
        """
        return self.get_transceiver_bulk_status().get("voltage", "N/A")

    def get_tx_bias(self):
        """
        Retrieves the TX bias current of this SFP
        Returns:
            A list of four integer numbers, representing TX bias in mA
            for channel 0 to channel 4.
            Ex. ["110.09", "111.12", "108.21", "112.09"]
        """
        transceiver_bulk_status = self.get_transceiver_bulk_status()
        return [
            transceiver_bulk_status.get("tx1bias", "N/A"),
            transceiver_bulk_status.get("tx2bias", "N/A"),
            transceiver_bulk_status.get("tx3bias", "N/A"),
            transceiver_bulk_status.get("tx4bias", "N/A"),
        ]

    def get_rx_power(self):
        """
        Retrieves the received optical power for this SFP
        Returns:
            A list of four integer numbers, representing received optical
            power in mW for channel 0 to channel 4.
            Ex. ["1.77", "1.71", "1.68", "1.70"]
        """
        self._update_sfp_presence()
        self._update_sfp_eeprom()
        self._dom_capability_detect()
        if not self.sfp_presence:
            return None
        rx_power_list = []
        if self.sfp_type == QSFP_TYPE:
            offset = 0

            sfpd_obj = sff8436Dom()
            if sfpd_obj is None:
                return None

            if self.dom_rx_power_supported:
                dom_channel_monitor_raw = self._read_eeprom_specific_bytes(
                    (offset + QSFP_CHANNL_MON_OFFSET),
                    QSFP_CHANNL_MON_WITH_TX_POWER_WIDTH,
                )
                if dom_channel_monitor_raw is not None:
                    dom_channel_monitor_data = (
                        sfpd_obj.parse_channel_monitor_params_with_tx_power(
                            dom_channel_monitor_raw, 0
                        )
                    )
                    rx_power_list.append(
                        self._convert_string_to_num(
                            dom_channel_monitor_data["data"]["RX1Power"]["value"]
                        )
                    )
                    rx_power_list.append(
                        self._convert_string_to_num(
                            dom_channel_monitor_data["data"]["RX2Power"]["value"]
                        )
                    )
                    rx_power_list.append(
                        self._convert_string_to_num(
                            dom_channel_monitor_data["data"]["RX3Power"]["value"]
                        )
                    )
                    rx_power_list.append(
                        self._convert_string_to_num(
                            dom_channel_monitor_data["data"]["RX4Power"]["value"]
                        )
                    )
                else:
                    return None
            else:
                return None
        else:
            offset = 256

            sfpd_obj = sff8472Dom()
            if sfpd_obj is None:
                return None

            if self.dom_supported:
                sfpd_obj._calibration_type = self.calibration

                dom_channel_monitor_raw = self._read_eeprom_specific_bytes(
                    (offset + SFP_CHANNL_MON_OFFSET), SFP_CHANNL_MON_WIDTH
                )
                if dom_channel_monitor_raw is not None:
                    dom_channel_monitor_data = sfpd_obj.parse_channel_monitor_params(
                        dom_channel_monitor_raw, 0
                    )
                    rx_power_list.append(
                        self._convert_string_to_num(
                            dom_channel_monitor_data["data"]["RXPower"]["value"]
                        )
                    )
                else:
                    return None
            else:
                return None

        return rx_power_list

    def get_tx_power(self):
        """
        Retrieves the TX power of this SFP
        Returns:
            A list of four integer numbers, representing TX power in mW
            for channel 0 to channel 4.
            Ex. ["1.86", "1.86", "1.86", "1.86"]
        """
        self._update_sfp_presence()
        self._update_sfp_eeprom()
        self._dom_capability_detect()
        if not self.sfp_presence:
            return None
        tx_power_list = []
        if self.sfp_type == QSFP_TYPE:
            offset = 0

            sfpd_obj = sff8436Dom()
            if sfpd_obj is None:
                return None

            if self.dom_tx_power_supported:
                dom_channel_monitor_raw = self._read_eeprom_specific_bytes(
                    (offset + QSFP_CHANNL_MON_OFFSET),
                    QSFP_CHANNL_MON_WITH_TX_POWER_WIDTH,
                )
                if dom_channel_monitor_raw is not None:
                    dom_channel_monitor_data = (
                        sfpd_obj.parse_channel_monitor_params_with_tx_power(
                            dom_channel_monitor_raw, 0
                        )
                    )
                    tx_power_list.append(
                        self._convert_string_to_num(
                            dom_channel_monitor_data["data"]["TX1Power"]["value"]
                        )
                    )
                    tx_power_list.append(
                        self._convert_string_to_num(
                            dom_channel_monitor_data["data"]["TX2Power"]["value"]
                        )
                    )
                    tx_power_list.append(
                        self._convert_string_to_num(
                            dom_channel_monitor_data["data"]["TX3Power"]["value"]
                        )
                    )
                    tx_power_list.append(
                        self._convert_string_to_num(
                            dom_channel_monitor_data["data"]["TX4Power"]["value"]
                        )
                    )
                else:
                    return None
            else:
                return None
        else:
            offset = 256
            sfpd_obj = sff8472Dom()
            if sfpd_obj is None:
                return None

            if self.dom_supported:
                sfpd_obj._calibration_type = self.calibration

                dom_channel_monitor_raw = self._read_eeprom_specific_bytes(
                    (offset + SFP_CHANNL_MON_OFFSET), SFP_CHANNL_MON_WIDTH
                )
                if dom_channel_monitor_raw is not None:
                    dom_channel_monitor_data = sfpd_obj.parse_channel_monitor_params(
                        dom_channel_monitor_raw, 0
                    )
                    tx_power_list.append(
                        self._convert_string_to_num(
                            dom_channel_monitor_data["data"]["TXPower"]["value"]
                        )
                    )
                else:
                    return None
            else:
                return None
        return tx_power_list

    def reset(self):
        """
        Reset SFP and return all user module settings to their default srate.
        Returns:
            A boolean, True if successful, False if not
        """
        if self.sfp_type == QSFP_TYPE:
            result = -1
            with self._api_helper.thrift_connect() as client:
                result = client.pltfm_mgr_qsfp_reset(
                    self.port_num, True
                ) | client.pltfm_mgr_qsfp_reset(self.port_num, False)
            return result == 0
        return False

    def tx_disable(self, tx_disable):
        """
        Disable SFP TX for all channels
        Args:
            tx_disable : A Boolean, True to enable tx_disable mode, False to disable
                         tx_disable mode.
        Returns:
            A boolean, True if tx_disable is set successfully, False if not
        """
        # Not Supported
        return False

    def tx_disable_channel(self, channel, disable):
        """
        Sets the tx_disable for specified SFP channels
        Args:
            channel : A hex of 4 bits (bit 0 to bit 3) which represent channel 0 to 3,
                      e.g. 0x5 for channel 0 and channel 2.
            disable : A boolean, True to disable TX channels specified in channel,
                      False to enable
        Returns:
            A boolean, True if successful, False if not
        """
        # Not Supported
        return False

    def set_lpmode(self, lpmode):
        """
        Sets the lpmode (low power mode) of SFP
        Args:
            lpmode: A Boolean, True to enable lpmode, False to disable it
            Note  : lpmode can be overridden by set_power_override
        Returns:
            A boolean, True if lpmode is set successfully, False if not
        """
        if self.sfp_type == QSFP_TYPE:
            result = -1
            with self._api_helper.thrift_connect() as client:
                result = client.pltfm_mgr_qsfp_lpmode_set(self.port_num, lpmode)
            return result == 0
        return False

    def set_power_override(self, power_override, power_set):
        """
        Sets SFP power level using power_override and power_set
        Args:
            power_override :
                    A Boolean, True to override set_lpmode and use power_set
                    to control SFP power, False to disable SFP power control
                    through power_override/power_set and use set_lpmode
                    to control SFP power.
            power_set :
                    Only valid when power_override is True.
                    A Boolean, True to set SFP to low power mode, False to set
                    SFP to high power mode.
        Returns:
            A boolean, True if power-override and power_set are set successfully,
            False if not
        """
        # Not supported
        return False

    def get_transceiver_status(self):
        """
        Retrieves the operational status of the device
        Returns:
            A boolean value, True if device is operating properly, False if not
        """
        raise NotImplementedError

    def get_transceiver_pm(self):
        """
        Retrieves transceiver info of this SFP
        Returns:
            A dict which contains following keys/values :
        ========================================================================
        keys                       |Value Format   |Information
        ---------------------------|---------------|----------------------------
        type                       |1*255VCHAR     |type of SFP
        vendor_rev                 |1*255VCHAR     |vendor revision of SFP
        serial                     |1*255VCHAR     |serial number of the SFP
        manufacturer               |1*255VCHAR     |SFP vendor name
        model                      |1*255VCHAR     |SFP model name
        connector                  |1*255VCHAR     |connector information
        encoding                   |1*255VCHAR     |encoding information
        ext_identifier             |1*255VCHAR     |extend identifier
        ext_rateselect_compliance  |1*255VCHAR     |extended rateSelect compliance
        cable_length               |INT            |cable length in m
        nominal_bit_rate           |INT            |nominal bit rate by 100Mbs
        specification_compliance   |1*255VCHAR     |specification compliance
        vendor_date                |1*255VCHAR     |vendor date
        vendor_oui                 |1*255VCHAR     |vendor OUI
        ========================================================================
        """
        raise NotImplementedError

    def get_xcvr_api(self):
        """
        Retrieves the xcvr API of the device
            Returns:
            object: The xcvr API obj of the device
        """
        return None

    def get_name(self):
        """
        Retrieves the name of the device
            Returns:
            string: The name of the device
        """
        return self.port_name[self.sfp_index] or "Unknown"

    def get_presence(self):
        """
        Retrieves the presence of the SFP
        Returns:
            bool: True if SFP is present, False if not
        """
        self._update_sfp_presence()
        return self.sfp_presence

    def get_model(self):
        """
        Retrieves the model number (or part number) of the device
        Returns:
            string: Model/part number of device
        """
        return (self.get_transceiver_info() or {}).get("model", "N/A")

    def get_serial(self):
        """
        Retrieves the serial number of the device
        Returns:
            string: Serial number of device
        """
        return (self.get_transceiver_info() or {}).get("serial", "N/A")

    def get_status(self):
        """
        Retrieves the operational status of the device
        Returns:
            A boolean value, True if device is operating properly, False if not
        """
        self._update_sfp_presence()
        return self.sfp_presence

    def is_replaceable(self):
        """
        Indicate whether this device is replaceable.
        Returns:
            bool: True if it is replaceable.
        """
        return True
