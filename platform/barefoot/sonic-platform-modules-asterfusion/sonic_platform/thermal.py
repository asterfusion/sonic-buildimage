##########################################################################
# Asterfusion CX-T Devices Thermal                                       #
#                                                                        #
# Thermal contains an implementation of SONiC Platform Base API and      #
# provides the thermal device status which are available in the platform #
#                                                                        #
##########################################################################

try:
    import copy

    from .pltfm_utils.constants import *
    from .pltfm_utils.helper import APIHelper

    from sonic_platform_base.thermal_base import ThermalBase
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


class Thermal(ThermalBase):
    """Platform-specific Thermal class"""

    def __init__(self, thermal_index, helper, bdid, device, platform):
        ThermalBase.__init__(self)
        self._api_helper = helper  # type: APIHelper
        # Init device
        self._bdid = bdid
        self._device = device
        self._platform = platform
        # Init index
        self._thermal_index = thermal_index
        self._thermal_pos = thermal_index + 1
        self._thermal_data = {}
        self._init_thermal_name_list()
        self._init_thermal_extremum()
        self._update_thermal_data()

    def _init_thermal_name_list(self):
        thermal_name_list = []
        if self._device == DEVICE_X308PT:
            if self._bdid in (
                BOARD_ID_X308PT_V1DOT0,
                BOARD_ID_X308PT_V1DOT1,
                BOARD_ID_X308PT_V2DOT0,
                BOARD_ID_X308PT_V3DOT0,
            ):
                thermal_name_list = copy.deepcopy(X308PT_V123_THERMAL_NAME_LIST)
        elif self._device == DEVICE_X312PT:
            if self._bdid in (
                BOARD_ID_X312PT_V1DOT0,
                BOARD_ID_X312PT_V1DOT1,
                BOARD_ID_X312PT_V1DOT2,
                BOARD_ID_X312PT_V2DOT0,
            ):
                thermal_name_list = copy.deepcopy(X312PT_V12_THERMAL_NAME_LIST)
            elif self._bdid in (
                BOARD_ID_X312PT_V1DOT3,
                BOARD_ID_X312PT_V3DOT0,
                BOARD_ID_X312PT_V4DOT0,
                BOARD_ID_X312PT_V5DOT0,
            ):
                thermal_name_list = copy.deepcopy(X312PT_V345_THERMAL_NAME_LIST)
        elif self._device == DEVICE_X532PT:
            if self._bdid in (
                BOARD_ID_X532PT_V1DOT0,
                BOARD_ID_X532PT_V1DOT1,
                BOARD_ID_X532PT_V2DOT0,
                BOARD_ID_X532PT_V3DOT0,
            ):
                thermal_name_list = copy.deepcopy(X532PT_V12_THERMAL_NAME_LIST)
        elif self._device == DEVICE_X564PT:
            if self._bdid in (
                BOARD_ID_X564PT_V1DOT0,
                BOARD_ID_X564PT_V1DOT1,
                BOARD_ID_X564PT_V1DOT2,
            ):
                thermal_name_list = copy.deepcopy(X564PT_V1_THERMAL_NAME_LIST)
            elif self._bdid in (BOARD_ID_X564PT_V2DOT0,):
                thermal_name_list = copy.deepcopy(X564PT_V2_THERMAL_NAME_LIST)
        elif self._device == DEVICE_X732QT:
            if self._bdid in (BOARD_ID_X732QT_V1DOT0,):
                thermal_name_list = copy.deepcopy(X732QT_V1_THERMAL_NAME_LIST)
        self._thermal_index_range = len(thermal_name_list)
        assert self._thermal_index_range != 0, "invalid thermal num"
        thermal_name_list += self._api_helper.get_x86_thermal_names()
        self._thermal_name_list = thermal_name_list

    def _init_thermal_extremum(self):
        self._minimum_recorded = NOT_AVAILABLE
        self._maximum_recorded = NOT_AVAILABLE

    def _update_thermal_data(self):
        with self._api_helper.thrift_client() as client:
            self._thermal_data = dict(
                map(
                    lambda kv: (
                        kv[0],
                        {
                            "temp": kv[1],
                            "high": TEMP_THRESHOLD_WARNING,
                            "crit_high": TEMP_THRESHOLD_CRITICAL_WARNING,
                        },
                    ),
                    vars(client.pltfm_mgr_sys_tmp_get()).items(),
                )
            )
        x86_thermals_values = self._api_helper.get_x86_thermal_values()
        for index in range(self._thermal_index_range, len(self._thermal_name_list)):
            num = index + 1
            x86_index = index - self._thermal_index_range
            self._thermal_data["tmp{}".format(num)] = {}
            self._thermal_data["tmp{}".format(num)]["temp"] = x86_thermals_values[
                x86_index
            ].get("temp", NOT_AVAILABLE)
            self._thermal_data["tmp{}".format(num)]["high"] = x86_thermals_values[
                x86_index
            ].get("high", NOT_AVAILABLE)
            self._thermal_data["tmp{}".format(num)]["crit_high"] = x86_thermals_values[
                x86_index
            ].get("crit", NOT_AVAILABLE)
        self._api_helper.log_debug("Updated data of {}".format(self.get_name()))

    def get_temperature(self):
        """
        Retrieves current temperature reading from thermal

        Returns:
            A float number of current temperature in Celsius up to nearest thousandth
            of one degree Celsius, e.g. 30.125
        """
        self._update_thermal_data()
        temperature = self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "temp", NOT_AVAILABLE
        )
        if temperature == 0.0 or temperature == -100.0:
            temperature = NOT_AVAILABLE
        return temperature

    def get_high_threshold(self):
        """
        Retrieves the high threshold temperature of thermal

        Returns:
            A float number, the high threshold temperature of thermal in Celsius
            up to nearest thousandth of one degree Celsius, e.g. 30.125
        """
        self._update_thermal_data()
        return self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "high", NOT_AVAILABLE
        )

    def get_low_threshold(self):
        """
        Retrieves the low threshold temperature of thermal

        Returns:
            A float number, the low threshold temperature of thermal in Celsius
            up to nearest thousandth of one degree Celsius, e.g. 30.125
        """
        self._update_thermal_data()
        return self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "low", NOT_AVAILABLE
        )

    def set_high_threshold(self, temperature):
        """
        Sets the high threshold temperature of thermal

        Args :
            temperature: A float number up to nearest thousandth of one degree Celsius,
            e.g. 30.125

        Returns:
            A boolean, True if threshold is set successfully, False if not
        """
        # Not supported.
        return False

    def set_low_threshold(self, temperature):
        """
        Sets the low threshold temperature of thermal

        Args :
            temperature: A float number up to nearest thousandth of one degree Celsius,
            e.g. 30.125

        Returns:
            A boolean, True if threshold is set successfully, False if not
        """
        # Not supported.
        return False

    def get_high_critical_threshold(self):
        """
        Retrieves the high critical threshold temperature of thermal

        Returns:
            A float number, the high critical threshold temperature of thermal in Celsius
            up to nearest thousandth of one degree Celsius, e.g. 30.125
        """
        self._update_thermal_data()
        return self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "crit_high", NOT_AVAILABLE
        )

    def get_low_critical_threshold(self):
        """
        Retrieves the low critical threshold temperature of thermal

        Returns:
            A float number, the low critical threshold temperature of thermal in Celsius
            up to nearest thousandth of one degree Celsius, e.g. 30.125
        """
        self._update_thermal_data()
        return self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "crit_low", NOT_AVAILABLE
        )

    def set_high_critical_threshold(self, temperature):
        """
        Sets the critical high threshold temperature of thermal

        Args :
            temperature: A float number up to nearest thousandth of one degree Celsius,
            e.g. 30.125

        Returns:
            A boolean, True if threshold is set successfully, False if not
        """
        # Not supported.
        return False

    def set_low_critical_threshold(self, temperature):
        """
        Sets the critical low threshold temperature of thermal

        Args :
            temperature: A float number up to nearest thousandth of one degree Celsius,
            e.g. 30.125

        Returns:
            A boolean, True if threshold is set successfully, False if not
        """
        # Not supported.
        return False

    def get_minimum_recorded(self):
        """
        Retrieves the minimum recorded temperature of thermal

        Returns:
            A float number, the minimum recorded temperature of thermal in Celsius
            up to nearest thousandth of one degree Celsius, e.g. 30.125
        """
        self._update_thermal_data()
        temperature = self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "temp", NOT_AVAILABLE
        )
        if temperature == 0.0 or temperature == -100.0:
            self._minimum_recorded = NOT_AVAILABLE
        else:
            if self._minimum_recorded == NOT_AVAILABLE:
                self._minimum_recorded = temperature
            self._minimum_recorded = min(self._minimum_recorded, temperature)
        return self._minimum_recorded

    def get_maximum_recorded(self):
        """
        Retrieves the maximum recorded temperature of thermal

        Returns:
            A float number, the maximum recorded temperature of thermal in Celsius
            up to nearest thousandth of one degree Celsius, e.g. 30.125
        """
        self._update_thermal_data()
        temperature = self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "temp", NOT_AVAILABLE
        )
        if temperature == 0.0 or temperature == -100.0:
            self._maximum_recorded = NOT_AVAILABLE
        else:
            if self._maximum_recorded == NOT_AVAILABLE:
                self._maximum_recorded = temperature
            self._maximum_recorded = max(self._maximum_recorded, temperature)
        return self._maximum_recorded

    def get_temp_info_dict(self):
        """
        Retrieves the temperature info dict of the device

        Returns:
            dict: The temperature info dict of the device who has following keys:
                key: the name of the device
                temperature: the temperature of the device
                high_threshold: the high threshild temperature of the device
                critical_high_threshold: the critical high threshild temperature of the device
                low_threshold: the critical low threshild temperature of the device
                critical_low_threshold: the critical low threshild temperature of the device
                warning_status: the warning status of the device
        """
        self._update_thermal_data()
        temperature = self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "temp", NOT_AVAILABLE
        )
        high = self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "high", NOT_AVAILABLE
        )
        crit_high = self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "crit_high", NOT_AVAILABLE
        )
        low = self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "low", NOT_AVAILABLE
        )
        crit_low = self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "crit_low", NOT_AVAILABLE
        )
        high_warning = temperature >= high if high != NOT_AVAILABLE else False
        low_warning = temperature <= low if low != NOT_AVAILABLE else False
        warning = str(high_warning or low_warning)
        temperature = (
            NOT_AVAILABLE
            if temperature == 0.0 or temperature == -100.0
            else temperature
        )
        temperature = (
            "{:.3f}".format(temperature)
            if temperature != NOT_AVAILABLE
            else temperature
        )
        high = "{:.3f}".format(high) if high != NOT_AVAILABLE else high
        crit_high = (
            "{:.3f}".format(crit_high) if crit_high != NOT_AVAILABLE else crit_high
        )
        low = "{:.3f}".format(low) if low != NOT_AVAILABLE else low
        crit_low = "{:.3f}".format(crit_low) if crit_low != NOT_AVAILABLE else crit_low
        return {
            "key": self._thermal_name_list[self._thermal_index],
            "temperature": temperature,
            "high_threshold": high,
            "critical_high_threshold": crit_high,
            "low_threshold": low,
            "critical_low_threshold": crit_low,
            "warning_status": warning,
        }

    def get_key(self):
        """
        Retrieves the name of the device

        Returns:
            string: The name of the device
        """
        return self._thermal_name_list[self._thermal_index]

    def get_warning(self):
        """
        Retrieves the warning status the device

        Returns:
            bool: True if device is warning, False if not
        """
        self._update_thermal_data()
        temperature = self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "temp", NOT_AVAILABLE
        )
        high = self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "high", NOT_AVAILABLE
        )
        low = self._thermal_data.get("tmp{}".format(self._thermal_pos), {}).get(
            "low", NOT_AVAILABLE
        )
        high_warning = temperature >= high if high != NOT_AVAILABLE else False
        low_warning = temperature <= low if low != NOT_AVAILABLE else False
        if high_warning:
            self._api_helper.log_warning(
                "Triggered warning status by {}: device overheated!".format(
                    self.get_name()
                )
            )
        if low_warning:
            self._api_helper.log_warning(
                "Triggered warning status by {}: device temperature too low!".format(
                    self.get_name()
                )
            )
        return high_warning or low_warning

    def get_name(self):
        """
        Retrieves the name of the device

        Returns:
            string: The name of the device
        """
        return self._thermal_name_list[self._thermal_index]

    def get_presence(self):
        """
        Retrieves the presence of the device

        Returns:
            bool: True if device is present, False if not
        """
        # Must be present
        return True

    def get_model(self):
        """
        Retrieves the model number (or part number) of the device

        Returns:
            string: Model/part number of device
        """
        # Need support from BMC. Not implemented yet.
        return NOT_AVAILABLE

    def get_serial(self):
        """
        Retrieves the serial number of the device

        Returns:
            string: Serial number of device
        """
        # Need support from BMC. Not implemented yet.
        return NOT_AVAILABLE

    def get_revision(self):
        """
        Retrieves the hardware revision of the device

        Returns:
            string: Revision value of device
        """
        # Need support from BMC. Not implemented yet.
        return NOT_AVAILABLE

    def get_status(self):
        """
        Retrieves the operational status of the device

        Returns:
            A boolean value, True if device is operating properly, False if not
        """
        # Must be OK
        return True

    def get_position_in_parent(self):
        """
        Retrieves 1-based relative physical position in parent device. If the agent cannot determine the parent-relative position
        for some reason, or if the associated value of entPhysicalContainedIn is '0', then the value '-1' is returned
        Returns:
            integer: The 1-based relative physical position in parent device or -1 if cannot determine the position
        """
        return self._thermal_pos

    def is_replaceable(self):
        """
        Indicate whether this device is replaceable.
        Returns:
            bool: True if it is replaceable.
        """
        return False
