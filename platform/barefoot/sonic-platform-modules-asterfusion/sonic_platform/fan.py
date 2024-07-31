####################################################################
# Asterfusion CX-T Devices Fan                                     #
#                                                                  #
# Module contains an implementation of SONiC Platform Base API and #
# provides the fan status which are available in the platform      #
#                                                                  #
####################################################################

try:
    import copy

    from .pltfm_utils.constants import *
    from .pltfm_utils.helper import APIHelper

    from sonic_platform_base.fan_base import FanBase
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


class Fan(FanBase):
    """Platform-specific Fan class"""

    def __init__(self, fan_drawer_index, fan_index, helper, bdid, device, platform):
        FanBase.__init__(self)
        self._api_helper = helper  # type: APIHelper
        # Init device
        self._bdid = bdid
        self._device = device
        self._platform = platform
        # Init index
        self._fan_drawer_index = fan_drawer_index
        self._fan_index = fan_drawer_index * 2 + fan_index
        # Init position
        self._fan_pos = fan_index + 1
        # Init fan data
        self._fan_presence = False
        self._fan_data = {}
        self._init_fan_name_list()
        self._init_fan_limit_info()
        self._update_fan_presence()
        self._update_fan_data()

    def _init_fan_name_list(self):
        self._fan_name_list = []
        if self._device == DEVICE_X308PT:
            if self._bdid in (
                BOARD_ID_X308PT_V1DOT0,
                BOARD_ID_X308PT_V1DOT1,
                BOARD_ID_X308PT_V2DOT0,
                BOARD_ID_X308PT_V3DOT0,
            ):
                self._fan_name_list = copy.deepcopy(X308PT_V12_FAN_NAME_LIST)
            elif self._bdid in (BOARD_ID_X308PT_V3DOT0,):
                self._fan_name_list = copy.deepcopy(X308PT_V3_FAN_NAME_LIST)
        elif self._device == DEVICE_X312PT:
            if self._bdid in (
                BOARD_ID_X312PT_V1DOT0,
                BOARD_ID_X312PT_V1DOT1,
                BOARD_ID_X312PT_V1DOT2,
                BOARD_ID_X312PT_V2DOT0,
                BOARD_ID_X312PT_V1DOT3,
                BOARD_ID_X312PT_V3DOT0,
                BOARD_ID_X312PT_V4DOT0,
                BOARD_ID_X312PT_V5DOT0,
            ):
                self._fan_name_list = copy.deepcopy(X312PT_V12345_FAN_NAME_LIST)
        elif self._device == DEVICE_X532PT:
            if self._bdid in (
                BOARD_ID_X532PT_V1DOT0,
                BOARD_ID_X532PT_V1DOT1,
                BOARD_ID_X532PT_V2DOT0,
                BOARD_ID_X532PT_V3DOT0,
            ):
                self._fan_name_list = copy.deepcopy(X532PT_V12_FAN_NAME_LIST)
        elif self._device == DEVICE_X564PT:
            if self._bdid in (
                BOARD_ID_X564PT_V1DOT0,
                BOARD_ID_X564PT_V1DOT1,
                BOARD_ID_X564PT_V1DOT2,
                BOARD_ID_X564PT_V2DOT0,
            ):
                self._fan_name_list = copy.deepcopy(X564PT_V12_FAN_NAME_LIST)
        elif self._device == DEVICE_X732QT:
            if self._bdid in (BOARD_ID_X732QT_V1DOT0,):
                self._fan_name_list = copy.deepcopy(X732QT_V1_FAN_NAME_LIST)
        assert len(self._fan_name_list) != 0, "invalid fan name list"

    def _init_fan_limit_info(self):
        if self._device == DEVICE_X308PT:
            self._fan_max_inlet = X308PT_FAN_MAX_INLET
            self._fan_max_outlet = X308PT_FAN_MAX_OUTLET
        elif self._device == DEVICE_X312PT:
            self._fan_max_inlet = X312PT_FAN_MAX_INLET
            self._fan_max_outlet = X312PT_FAN_MAX_OUTLET
        elif self._device == DEVICE_X532PT:
            self._fan_max_inlet = X532PT_FAN_MAX_INLET
            self._fan_max_outlet = X532PT_FAN_MAX_OUTLET
        elif self._device == DEVICE_X564PT:
            self._fan_max_inlet = X564PT_FAN_MAX_INLET
            self._fan_max_outlet = X564PT_FAN_MAX_OUTLET
        elif self._device == DEVICE_X732QT:
            self._fan_max_inlet = X732QT_FAN_MAX_INLET
            self._fan_max_outlet = X732QT_FAN_MAX_OUTLET
        else:
            self._fan_max_inlet = 0
            self._fan_max_outlet = 0
        assert self._fan_max_inlet != 0, "invalid max fan inlet rpm"
        assert self._fan_max_outlet != 0, "invalid max fan outlet rpm"

    def _update_fan_presence(self):
        with self._api_helper.thrift_client() as client:
            fan_data = vars(client.pltfm_mgr_fan_info_get(self._fan_index + 1))
            self._fan_presence = int(fan_data.get("front_rpm", 0)) > 0
        self._api_helper.log_debug(
            "Updated presence of {}".format(self.get_name()),
        )

    def _update_fan_data(self):
        with self._api_helper.thrift_client() as client:
            self._fan_data = vars(client.pltfm_mgr_fan_info_get(self._fan_index + 1))
        self._api_helper.log_debug("Updated data of {}".format(self.get_name()))

    def get_direction(self):
        """
        Retrieves the direction of fan
        Returns:
            A string, either FAN_DIRECTION_INTAKE or FAN_DIRECTION_EXHAUST
            depending on fan direction
        """
        self._update_fan_data()
        return (
            self.FAN_DIRECTION_EXHAUST
            if int(self._fan_data.get("front_rpm", 0)) >= 0
            else self.FAN_DIRECTION_INTAKE
        )

    def get_speed(self):
        """
        Retrieves the speed of fan as a percentage of full speed
        Returns:
            An integer, the percentage of full fan speed, in the range 0 (off)
                 to 100 (full speed)
        """
        self._update_fan_data()
        max_rpm = (
            self._fan_max_outlet
            if (self._fan_index + 1) % 2 == 0
            else self._fan_max_inlet
        )
        rpm = int(self._fan_data.get("front_rpm", 0))

        return int(float(rpm) / max_rpm * 100)
        # return rpm

    def get_target_speed(self):
        """
        Retrieves the target (expected) speed of the fan
        Returns:
            An integer, the percentage of full fan speed, in the range 0 (off)
                 to 100 (full speed)
        """
        self._update_fan_data()
        max_rpm = (
            self._fan_max_outlet
            if (self._fan_index + 1) % 2 == 0
            else self._fan_max_inlet
        )
        rpm = int(self._fan_data.get("front_rpm", 0))

        return int(float(rpm) / max_rpm * 100)
        # return rpm

    def get_speed_tolerance(self):
        """
        Retrieves the speed tolerance of the fan
        Returns:
            An integer, the percentage of variance from target speed which is
                 considered tolerable
        """
        return FAN_SPEED_TOLERANCE

    def set_speed(self, speed):
        """
        Sets the fan speed
        Args:
            speed: An integer, the percentage of full fan speed to set fan to,
                   in the range 0 (off) to 100 (full speed)
        Returns:
            A boolean, True if speed is set successfully, False if not
        """
        # Need support from BMC. Not implemented yet.
        return False

    def set_status_led(self, color):
        """
        Sets the state of the fan module status LED
        Args:
            color: A string representing the color with which to set the
                   fan module status LED
        Returns:
            bool: True if status LED state is set successfully, False if not
        """
        # Need support from BMC. Not implemented yet.
        return False

    def get_status_led(self):
        """
        Gets the state of the fan status LED
        Returns:
            A string, one of the predefined STATUS_LED_COLOR_* strings above
        """
        # Need support from BMC. Not implemented yet.
        return NOT_AVAILABLE

    def get_fan_info_dict(self):
        """
        Retrieves the fan info dict of the fan

        Returns:
            dict: The fan info dict of the fan who has following keys:
                status: the status of the fan
                direction: the rotate direction of the fan
                speed: the speed of the fan
                presence: the presence status of the fan
        """
        self._update_fan_presence()
        self._update_fan_data()
        return {
            "status": str(self._fan_presence).lower(),
            "direction": (
                self.FAN_DIRECTION_EXHAUST
                if int(self._fan_data.get("front_rpm", 0)) >= 0
                else self.FAN_DIRECTION_INTAKE
            ),
            "speed": str(int(self._fan_data.get("front_rpm", 0))),
            "presence": str(self._fan_presence).lower(),
        }

    def get_warning(self):
        """
        Retrieves the warning status of the device
        Returns:
            A boolean value, True if device is warning, False if not
        """
        self._update_fan_presence()
        if not self._fan_presence:
            self._api_helper.log_warning(
                "Triggered warning status by {}: device absent!".format(self.get_name())
            )
        return not self._fan_presence

    def get_name(self):
        """
        Retrieves the name of the device
            Returns:
            string: The name of the device
        """
        # Need support from BMC. PSU-FAN Not implemented yet.
        return self._fan_name_list[self._fan_index]

    def get_presence(self):
        """
        Retrieves the presence of the FAN
        Returns:
            bool: True if FAN is present, False if not
        """
        self._update_fan_presence()
        # Need support from BMC. PSU-FAN Not implemented yet.
        return self._fan_presence

    def get_model(self):
        """
        Retrieves the model number (or part number) of the device
        Returns:
            string: Model/part number of device
        """
        return str(self._fan_data.get("model", NOT_AVAILABLE))

    def get_serial(self):
        """
        Retrieves the serial number of the device
        Returns:
            string: Serial number of device
        """
        return str(self._fan_data.get("serial", NOT_AVAILABLE))

    def get_revision(self):
        """
        Retrieves the hardware revision of the device

        Returns:
            string: Revision value of device
        """
        return str(self._fan_data.get("rev", NOT_AVAILABLE))

    def get_status(self):
        """
        Retrieves the operational status of the device
        Returns:
            A boolean value, True if device is operating properly, False if not
        """
        self._update_fan_presence()
        return self._fan_presence

    def get_position_in_parent(self):
        """
        Retrieves 1-based relative physical position in parent device. If the agent cannot determine the parent-relative position
        for some reason, or if the associated value of entPhysicalContainedIn is '0', then the value '-1' is returned
        Returns:
            integer: The 1-based relative physical position in parent device or -1 if cannot determine the position
        """
        return self._fan_drawer_index * 2 + self._fan_pos

    def is_replaceable(self):
        """
        Indicate whether this device is replaceable.
        Returns:
            bool: True if it is replaceable.
        """
        return True
