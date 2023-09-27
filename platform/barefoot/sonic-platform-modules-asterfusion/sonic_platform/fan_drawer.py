####################################################################
# Asterfusion CX-T Devices Fan Drawer                              #
#                                                                  #
# Module contains an implementation of SONiC Platform Base API and #
# provides the fan status which are available in the platform      #
#                                                                  #
####################################################################

try:
    import copy

    from .constants import *
    from .helper import APIHelper

    from sonic_platform_base.fan_drawer_base import FanDrawerBase
    from sonic_platform.fan import Fan
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


class FanDrawer(FanDrawerBase):
    """Platform-specific Fan Drawer class"""

    def __init__(self, fan_drawer_index, helper, hwver, hwsku, platform):
        FanDrawerBase.__init__(self)
        self._api_helper = helper  # type: APIHelper
        # Init device
        self.hwver = hwver
        self.hwsku = hwsku
        self.platform = platform
        # Init index
        self.fan_drawer_index = fan_drawer_index
        self._fan_list = []
        self._init_fan_drawer_name_list()
        self._init_fan_num_per_drawer()
        self._init_fan_list()

    def _init_fan_drawer_name_list(self):
        self.fan_drawer_name_list = []
        if self.hwsku == HWSKU_X308PT:
            if self.hwver in (
                HWVER_X308PT_V1DOT0,
                HWVER_X308PT_V1DOT1,
                HWVER_X308PT_V2DOT0,
            ):
                self.fan_drawer_name_list = copy.deepcopy(
                    X308PT_V12_FAN_DRAWER_NAME_LIST
                )
            elif self.hwver in (HWVER_X308PT_V3DOT0,):
                self.fan_drawer_name_list = copy.deepcopy(
                    X308PT_V3_FAN_DRAWER_NAME_LIST
                )
        elif self.hwsku == HWSKU_X312PT:
            if self.hwver in (
                HWVER_X312PT_V1DOT0,
                HWVER_X312PT_V1DOT1,
                HWVER_X312PT_V1DOT2,
                HWVER_X312PT_V2DOT0,
                HWVER_X312PT_V1DOT3,
                HWVER_X312PT_V3DOT0,
                HWVER_X312PT_V4DOT0,
                HWVER_X312PT_V5DOT0,
            ):
                self.fan_drawer_name_list = copy.deepcopy(
                    X312PT_V12345_FAN_DRAWER_NAME_LIST
                )
        elif self.hwsku == HWSKU_X532PT:
            if self.hwver in (
                HWVER_X532PT_V1DOT0,
                HWVER_X532PT_V1DOT1,
                HWVER_X532PT_V2DOT0,
            ):
                self.fan_drawer_name_list = copy.deepcopy(
                    X532PT_V12_FAN_DRAWER_NAME_LIST
                )
        elif self.hwsku == HWSKU_X564PT:
            if self.hwver in (
                HWVER_X564PT_V1DOT0,
                HWVER_X564PT_V1DOT1,
                HWVER_X564PT_V1DOT2,
                HWVER_X564PT_V2DOT0,
            ):
                self.fan_drawer_name_list = copy.deepcopy(
                    X564PT_V12_FAN_DRAWER_NAME_LIST
                )
        assert len(self.fan_drawer_name_list) != 0, "invalid fan drawer name list"

    def _init_fan_num_per_drawer(self):
        self.fan_num_per_drawer = PERIPHERAL[self.hwver]["FAN"]["NUM"]

    def _init_fan_list(self):
        for fan_index in range(0, self.fan_num_per_drawer):
            fan = Fan(
                self.fan_drawer_index,
                fan_index,
                self._api_helper,
                self.hwver,
                self.hwsku,
                self.platform,
            )
            self._fan_list.append(fan)

    def get_num_fans(self):
        """
        Retrieves the number of fans available on this fan drawer

        Returns:
            An integer, the number of fan modules available on this fan drawer
        """
        return len(self._fan_list)

    def get_all_fans(self):
        """
        Retrieves all fan modules available on this fan drawer

        Returns:
            A list of objects derived from FanBase representing all fan
            modules available on this fan drawer
        """
        return self._fan_list

    def get_fan(self, index):
        """
        Retrieves fan module represented by (0-based) index <index>

        Args:
            index: An integer, the index (0-based) of the fan module to
            retrieve

        Returns:
            An object dervied from FanBase representing the specified fan
            module
        """
        fan = None

        try:
            fan = self._fan_list[index]
        except IndexError:
            self._api_helper.log_debug(
                "Fan index {} out of range (0-{})\n".format(
                    index, len(self._fan_list) - 1
                )
            )

        return fan

    def set_status_led(self, color):
        """
        Sets the state of the fan drawer status LED

        Args:
            color: A string representing the color with which to set the
                   fan drawer status LED

        Returns:
            bool: True if status LED state is set successfully, False if not
        """
        return False

    def get_status_led(self):
        """
        Gets the state of the fan drawer LED

        Returns:
            A string, one of the predefined STATUS_LED_COLOR_* strings above
        """
        return "N/A"

    def get_maximum_consumed_power(self):
        """
        Retrives the maximum power drawn by Fan Drawer

        Returns:
            A float, with value of the maximum consumable power of the
            component.
        """
        return "N/A"

    def get_name(self):
        """
        Retrieves the name of the device
            Returns:
            string: The name of the device
        """
        # Need support from BMC. PSU-FAN Not implemented yet.
        # if not self.is_psu_fan else "PSU-{} FAN-{}".format(self.psu_index+1, self.fan_index+1)
        return self.fan_drawer_name_list[self.fan_drawer_index]

    def get_presence(self):
        """
        Retrieves the presence of the FAN
        Returns:
            bool: True if FAN is present, False if not
        """
        presence = True
        for fan in self._fan_list:
            presence = presence and fan.get_presence()
        return presence

    def get_model(self):
        """
        Retrieves the model number (or part number) of the device
        Returns:
            string: Model/part number of device
        """
        # Need support from BMC. Not implemented yet.
        return "N/A"

    def get_serial(self):
        """
        Retrieves the serial number of the device
        Returns:
            string: Serial number of device
        """
        # Need support from BMC. Not implemented yet.
        return "N/A"

    def get_revision(self):
        """
        Retrieves the hardware revision of the device

        Returns:
            string: Revision value of device
        """
        # Need support from BMC. Not implemented yet.
        return "N/A"

    def get_status(self):
        """
        Retrieves the operational status of the device
        Returns:
            A boolean value, True if device is operating properly, False if not
        """
        for fan in self._fan_list:
            if not fan.get_status():
                return False
        return True

    def is_replaceable(self):
        """
        Indicate whether this device is replaceable.
        Returns:
            bool: True if it is replaceable.
        """
        return True
