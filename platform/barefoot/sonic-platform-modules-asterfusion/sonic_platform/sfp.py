######################################################################
# Asterfusion CX-T Devices Transceiver                               #
#                                                                    #
# Sfp contains an implementation of SONiC Platform Base API and      #
# provides the sfp device status which are available in the platform #
#                                                                    #
######################################################################

try:
    from pathlib import Path

    from .pltfm_utils.constants import *
    from .pltfm_utils.helper import APIHelper

    from sonic_platform_base.sonic_sfp.sfputilhelper import SfpUtilHelper
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


try:
    from sonic_platform_base.sonic_xcvr.sfp_optoe_base import SfpOptoeBase
except ImportError:
    from .pltfm_utils.sonic_xcvr.sfp_optoe_base import SfpOptoeBase


class Sfp(SfpOptoeBase):
    """Platform-specific Sfp class"""

    def __init__(self, sfp_index, helper, bdid, device, platform):
        SfpOptoeBase.__init__(self)
        self._api_helper = helper  # type: APIHelper
        # Init device
        self._bdid = bdid
        self._device = device
        self._platform = platform
        # Init index
        self._sfp_index = sfp_index
        self._sfp_presence = False
        # Init port type
        self._init_port_type()
        # Init port position
        self._init_port_pos()
        # Init sfp data
        self._init_port_name()
        self._update_sfp_presence()
        self._update_sfp_data()

    @property
    def index(self):
        return self._sfp_index

    @property
    def sfp_type(self):
        if not self._sfp_presence:
            return UNKNOWN_SFP_TYPE
        sfp_id = self.read_eeprom(0, 1)
        if not sfp_id:
            return UNKNOWN_SFP_TYPE
        if sfp_id[0] in (0x03,):
            return SFP_TYPE
        if sfp_id[0] in (0x0D, 0x11,):
            return QSFP_TYPE
        if sfp_id[0] in (0x18, 0x19, 0x1E,):
            return QSFP_DD_TYPE
        return UNKNOWN_SFP_TYPE

    def _init_port_type(self):
        self._port_type = UNKNOWN_PORT_TYPE
        sfp_index_type_list = None
        if self._device == DEVICE_X308PT:
            sfp_index_type_list = X308PT_PORT_INDEX_TYPE
        elif self._device == DEVICE_X312PT:
            sfp_index_type_list = X312PT_PORT_INDEX_TYPE
        elif self._device == DEVICE_X532PT:
            sfp_index_type_list = X532PT_PORT_INDEX_TYPE
        elif self._device == DEVICE_X564PT:
            sfp_index_type_list = X564PT_PORT_INDEX_TYPE
        elif self._device == DEVICE_X732QT:
            sfp_index_type_list = X732QT_PORT_INDEX_TYPE
        assert sfp_index_type_list is not None, "invalid index type list"
        for start, end, type in sfp_index_type_list:
            if self._sfp_index in range(start, end):
                self._port_type = type
        assert self._port_type != UNKNOWN_PORT_TYPE, "unknown port type"

    def _init_port_pos(self):
        self._port_pos = 0
        sfp_index_type_list = None
        sfp_index_seg_diff = 0
        if self._device == DEVICE_X308PT:
            sfp_index_type_list = X308PT_PORT_INDEX_TYPE
        elif self._device == DEVICE_X312PT:
            sfp_index_type_list = X312PT_PORT_INDEX_TYPE
        elif self._device == DEVICE_X532PT:
            sfp_index_type_list = X532PT_PORT_INDEX_TYPE
        elif self._device == DEVICE_X564PT:
            sfp_index_type_list = X564PT_PORT_INDEX_TYPE
        elif self._device == DEVICE_X732QT:
            sfp_index_type_list = X732QT_PORT_INDEX_TYPE
        assert sfp_index_type_list is not None, "invalid index type list"
        for start, end, _ in sfp_index_type_list:
            if self._sfp_index in range(start, end):
                if self._port_type == DPU_PORT_TYPE:
                    self._port_pos = DPU_PORT_POSITION
                else:
                    self._port_pos = self._sfp_index + 1 - sfp_index_seg_diff
            sfp_index_seg_diff = end - start
        assert self._port_pos != 0, "invalid port position"

    def _init_port_name(self):
        if self._api_helper.check_if_host():
            platform_json_path = Path(
                DEVICE_ROOT, self._platform, self._device, "platform.json"
            )
            port_config_path = Path(
                DEVICE_ROOT, self._platform, self._device, "port_config.ini"
            )
        else:
            platform_json_path = Path(HWSKU_ROOT, "platform.json")
            port_config_path = Path(HWSKU_ROOT, "port_config.ini")
        sfputil_helper = SfpUtilHelper()
        if platform_json_path.exists():
            sfputil_helper.read_porttab_mappings(platform_json_path.as_posix())
        elif port_config_path.exists():
            sfputil_helper.read_porttab_mappings(port_config_path.as_posix())
        self._port_name = sfputil_helper.logical
        assert self._port_name != [], "invalid port name"

    def _update_sfp_presence(self):
        self._sfp_presence = False
        with self._api_helper.thrift_client() as client:
            if self._port_type == SFP_PORT_TYPE:
                self._sfp_presence = bool(
                    client.pltfm_mgr_sfp_presence_get(self._port_pos)
                )
            elif self._port_type == QSFP_PORT_TYPE:
                self._sfp_presence = bool(
                    client.pltfm_mgr_qsfp_presence_get(self._port_pos)
                )
            elif self._port_type == QSFP_DD_PORT_TYPE:
                self._sfp_presence = bool(
                    client.pltfm_mgr_qsfp_presence_get(self._port_pos)
                )
            # Code below is a temporary hack to fix the issue
            # that transceivers' memory page could be unready
            # shortly after it's plugged into the port cage.
            # Making its' presence False is enough to help.
            try:
                if self._port_type == SFP_PORT_TYPE:
                    client.pltfm_mgr_sfp_info_get(self._port_pos)
                elif self._port_type == QSFP_PORT_TYPE:
                    client.pltfm_mgr_qsfp_info_get(self._port_pos)
                elif self._port_type == QSFP_DD_PORT_TYPE:
                    client.pltfm_mgr_qsfp_info_get(self._port_pos)
            except Exception as err:
                self._sfp_presence = False
            if self._port_type != DPU_PORT_TYPE:
                self._api_helper.log_debug(
                    "Updated transceiver presence of port {}".format(self.get_name())
                )

    def _update_sfp_data(self):
        self._sfp_data = bytearray()
        if not self._sfp_presence:
            return
        with self._api_helper.thrift_client() as client:
            try:
                if self._port_type == SFP_PORT_TYPE:
                    self._sfp_data = bytearray.fromhex(
                        client.pltfm_mgr_sfp_info_get(self._port_pos)
                    )
                elif self._port_type == QSFP_PORT_TYPE:
                    self._sfp_data = bytearray.fromhex(
                        client.pltfm_mgr_qsfp_info_get(self._port_pos)
                    )
                elif self._port_type == QSFP_DD_PORT_TYPE:
                    self._sfp_data = bytearray.fromhex(
                        client.pltfm_mgr_qsfp_info_get(self._port_pos)
                    )
            except Exception as err:
                self._api_helper.log_debug(
                    "Failed in updating transceiver EEPROM of port {} due to {}".format(
                        self.get_name(), err
                    )
                )
            else:
                if self._port_type != DPU_PORT_TYPE:
                    self._api_helper.log_debug(
                        "Updated transceiver EEPROM of port {}".format(self.get_name())
                    )
                else:
                    self._api_helper.log_debug(
                        "Skipped updating transceiver EEPROM of DPU port {}".format(
                            self.get_name()
                        )
                    )

    def read_eeprom(self, offset, num_bytes):
        """
        read eeprom specfic bytes beginning from a random offset with size as num_bytes
        Args:
             offset :
                     Integer, the offset from which the read transaction will start
             num_bytes:
                     Integer, the number of bytes to be read
        Returns:
            bytearray, if raw sequence of bytes are read correctly from the offset of size num_bytes
            None, if the read_eeprom fails
        """
        self._update_sfp_presence()
        if not self._sfp_presence:
            return bytearray(num_bytes)
        self._update_sfp_data()
        self._api_helper.log_debug(
            "Read {} bytes at offset {}".format(num_bytes, offset)
        )
        return self._sfp_data[offset : offset + num_bytes]

    def write_eeprom(self, offset, num_bytes, write_buffer):
        """
        write eeprom specfic bytes beginning from a random offset with size as num_bytes
        and write_buffer as the required bytes
        Args:
             offset :
                     Integer, the offset from which the read transaction will start
             num_bytes:
                     Integer, the number of bytes to be written
             write_buffer:
                     bytearray, raw bytes buffer which is to be written beginning at the offset
        Returns:
            a Boolean, true if the write succeeded and false if it did not succeed.
        """
        self._update_sfp_presence()
        if not self._sfp_presence:
            self._api_helper.log_debug(
                "Failed in writing transceiver EEPROM of port {} due to transceiver absence".format(
                    self.get_name()
                )
            )
            return False
        self._update_sfp_data()
        self._api_helper.log_debug(
            "Fakely wrote data \"{}\" of {} bytes at offset {}".format(
                " ".join(map(hex, write_buffer)), num_bytes, offset
            )
        )
        return True

    def get_reset_status(self):
        """
        Retrieves the reset status of SFP
        Returns:
            A Boolean, True if reset enabled, False if disabled
        """
        self._update_sfp_presence()
        if not self._sfp_presence:
            return False
        if self._port_type == SFP_PORT_TYPE:
            return False
        if self._port_type == QSFP_PORT_TYPE:
            return False
        if self._port_type == QSFP_DD_PORT_TYPE:
            return False
        return False

    def get_lpmode(self):
        """
        Retrieves the lpmode (low power mode) status of this SFP
        Returns:
            A Boolean, True if lpmode is enabled, False if disabled
        """
        self._update_sfp_presence()
        if not self._sfp_presence:
            return LPMODE_UNSUPPORTED
        if self._port_type == SFP_PORT_TYPE:
            return LPMODE_UNSUPPORTED
        if self._port_type == QSFP_PORT_TYPE:
            with self._api_helper.thrift_client() as client:
                return (
                    LPMODE_ON
                    if client.pltfm_mgr_qsfp_lpmode_get(self._port_pos)
                    else LPMODE_OFF
                )
        if self._port_type == QSFP_DD_PORT_TYPE:
            with self._api_helper.thrift_client() as client:
                return (
                    LPMODE_ON
                    if client.pltfm_mgr_qsfp_lpmode_get(self._port_pos)
                    else LPMODE_OFF
                )
        return LPMODE_UNSUPPORTED

    def reset(self):
        """
        Reset SFP and return all user module settings to their default srate.
        Returns:
            A boolean, True if successful, False if not
        """
        self._update_sfp_presence()
        if not self._sfp_presence:
            return False
        if self._port_type == SFP_PORT_TYPE:
            return False
        if self._port_type == QSFP_PORT_TYPE:
            result = -1
            with self._api_helper.thrift_client() as client:
                result = client.pltfm_mgr_qsfp_reset(
                    self._port_pos, True
                ) | client.pltfm_mgr_qsfp_reset(self._port_pos, False)
            return result == 0
        if self._port_type == QSFP_DD_PORT_TYPE:
            result = -1
            with self._api_helper.thrift_client() as client:
                result = client.pltfm_mgr_qsfp_reset(
                    self._port_pos, True
                ) | client.pltfm_mgr_qsfp_reset(self._port_pos, False)
            return result == 0
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
        self._update_sfp_presence()
        if not self._sfp_presence:
            return LPMODE_UNSUPPORTED
        if self._port_type == SFP_PORT_TYPE:
            return LPMODE_UNSUPPORTED
        if self._port_type == QSFP_PORT_TYPE:
            result = -1
            with self._api_helper.thrift_client() as client:
                result = client.pltfm_mgr_qsfp_lpmode_set(self._port_pos, lpmode)
            return LPMODE_SUCCESS if result == 0 else LPMODE_FAILURE
        if self._port_type == QSFP_DD_PORT_TYPE:
            result = -1
            with self._api_helper.thrift_client() as client:
                result = client.pltfm_mgr_qsfp_lpmode_set(self._port_pos, lpmode)
            return LPMODE_SUCCESS if result == 0 else LPMODE_FAILURE
        return LPMODE_UNSUPPORTED

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

    def get_xcvr_api(self):
        """
        Retrieves the XcvrApi associated with this SFP
        Returns:
            An object derived from XcvrApi that corresponds to the SFP
        """
        self._update_sfp_presence()
        if self._sfp_presence:
            self.refresh_xcvr_api()
        return self._xcvr_api

    def get_name(self):
        """
        Retrieves the name of the device
            Returns:
            string: The name of the device
        """
        return self._port_name[self._sfp_index] or "Unknown"

    def get_presence(self):
        """
        Retrieves the presence of the SFP
        Returns:
            bool: True if SFP is present, False if not
        """
        self._update_sfp_presence()
        return self._sfp_presence

    def get_status(self):
        """
        Retrieves the operational status of the device
        Returns:
            A boolean value, True if device is operating properly, False if not
        """
        self._update_sfp_presence()
        return self._sfp_presence

    def get_revision(self):
        """
        Retrieves the hardware revision of the device

        Returns:
            string: Revision value of device
        """
        return self.get_transceiver_info().get("vendor_rev", NOT_AVAILABLE)

    def get_position_in_parent(self):
        """
        Retrieves 1-based relative physical position in parent device. If the agent cannot determine the parent-relative position
        for some reason, or if the associated value of entPhysicalContainedIn is '0', then the value '-1' is returned
        Returns:
            integer: The 1-based relative physical position in parent device or -1 if cannot determine the position
        """
        return self._port_pos

    def is_replaceable(self):
        """
        Indicate whether this device is replaceable.
        Returns:
            bool: True if it is replaceable.
        """
        return True
