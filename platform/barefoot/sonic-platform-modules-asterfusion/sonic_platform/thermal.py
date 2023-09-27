##########################################################################
# Asterfusion CX-T Devices Thermal                                       #
#                                                                        #
# Thermal contains an implementation of SONiC Platform Base API and      #
# provides the thermal device status which are available in the platform #
#                                                                        #
##########################################################################

try:
    import copy

    from .constants import *
    from .helper import APIHelper

    from sonic_platform_base.thermal_base import ThermalBase
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


class Thermal(ThermalBase):
    """Platform-specific Thermal class"""

    def __init__(self, thermal_index, helper, hwver, hwsku, platform):
        ThermalBase.__init__(self)
        self._api_helper = helper  # type: APIHelper
        # Init device
        self.hwver = hwver
        self.hwsku = hwsku
        self.platform = platform
        # Init index
        self.thermal_index = thermal_index
        self.thermal_num = thermal_index + 1
        self.thermal_data = {}
        self._init_thermal_name_list()
        self._init_thermal_extremum()
        self._update_thermal_data()

    def _init_thermal_name_list(self):
        thermal_name_list = []
        if self.hwsku == HWSKU_X308PT:
            if self.hwver in (
                HWVER_X308PT_V1DOT0,
                HWVER_X308PT_V1DOT1,
                HWVER_X308PT_V2DOT0,
                HWVER_X308PT_V3DOT0,
            ):
                thermal_name_list = copy.deepcopy(X308PT_V123_THERMAL_NAME_LIST)
        elif self.hwsku == HWSKU_X312PT:
            if self.hwver in (
                HWVER_X312PT_V1DOT0,
                HWVER_X312PT_V1DOT1,
                HWVER_X312PT_V1DOT2,
                HWVER_X312PT_V2DOT0,
            ):
                thermal_name_list = copy.deepcopy(X312PT_V12_THERMAL_NAME_LIST)
            elif self.hwver in (
                HWVER_X312PT_V1DOT3,
                HWVER_X312PT_V3DOT0,
                HWVER_X312PT_V4DOT0,
                HWVER_X312PT_V5DOT0,
            ):
                thermal_name_list = copy.deepcopy(X312PT_V345_THERMAL_NAME_LIST)
        elif self.hwsku == HWSKU_X532PT:
            if self.hwver in (
                HWVER_X532PT_V1DOT0,
                HWVER_X532PT_V1DOT1,
                HWVER_X532PT_V2DOT0,
            ):
                thermal_name_list = copy.deepcopy(X532PT_V12_THERMAL_NAME_LIST)
        elif self.hwsku == HWSKU_X564PT:
            if self.hwver in (
                HWVER_X564PT_V1DOT0,
                HWVER_X564PT_V1DOT1,
                HWVER_X564PT_V1DOT2,
            ):
                thermal_name_list = copy.deepcopy(X564PT_V1_THERMAL_NAME_LIST)
            elif self.hwver in (HWVER_X564PT_V2DOT0,):
                thermal_name_list = copy.deepcopy(X564PT_V2_THERMAL_NAME_LIST)
        self.thermal_index_range = len(thermal_name_list)
        assert self.thermal_index_range != 0, "invalid thermal num"
        thermal_name_list += self._api_helper.get_x86_thermal_names()
        self.thermal_name_list = thermal_name_list

    def _init_thermal_extremum(self):
        self.minimum_recorded = "N/A"
        self.maximum_recorded = "N/A"

    def _update_thermal_data(self):
        with self._api_helper.thrift_connect() as client:
            self.thermal_data = dict(
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
        for index in range(self.thermal_index_range, len(self.thermal_name_list)):
            num = index + 1
            x86_index = index - self.thermal_index_range
            self.thermal_data["tmp{}".format(num)] = {}
            self.thermal_data["tmp{}".format(num)]["temp"] = x86_thermals_values[
                x86_index
            ].get("temp", "N/A")
            self.thermal_data["tmp{}".format(num)]["high"] = x86_thermals_values[
                x86_index
            ].get("high", "N/A")
            self.thermal_data["tmp{}".format(num)]["crit_high"] = x86_thermals_values[
                x86_index
            ].get("crit", "N/A")

    def get_temperature(self):
        """
        Retrieves current temperature reading from thermal

        Returns:
            A float number of current temperature in Celsius up to nearest thousandth
            of one degree Celsius, e.g. 30.125
        """
        self._update_thermal_data()
        temperature = self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "temp", "N/A"
        )
        if temperature == 0.0 or temperature == -100.0:
            temperature = "N/A"
        return temperature

    def get_high_threshold(self):
        """
        Retrieves the high threshold temperature of thermal

        Returns:
            A float number, the high threshold temperature of thermal in Celsius
            up to nearest thousandth of one degree Celsius, e.g. 30.125
        """
        self._update_thermal_data()
        return self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "high", "N/A"
        )

    def get_low_threshold(self):
        """
        Retrieves the low threshold temperature of thermal

        Returns:
            A float number, the low threshold temperature of thermal in Celsius
            up to nearest thousandth of one degree Celsius, e.g. 30.125
        """
        self._update_thermal_data()
        return self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "low", "N/A"
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
        return self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "crit_high", "N/A"
        )

    def get_low_critical_threshold(self):
        """
        Retrieves the low critical threshold temperature of thermal

        Returns:
            A float number, the low critical threshold temperature of thermal in Celsius
            up to nearest thousandth of one degree Celsius, e.g. 30.125
        """
        self._update_thermal_data()
        return self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "crit_low", "N/A"
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
        temperature = self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "temp", "N/A"
        )
        if temperature == 0.0 or temperature == -100.0:
            self.minimum_recorded = "N/A"
        else:
            if self.minimum_recorded == "N/A":
                self.minimum_recorded = temperature
            self.minimum_recorded = min(self.minimum_recorded, temperature)
        return self.minimum_recorded

    def get_maximum_recorded(self):
        """
        Retrieves the maximum recorded temperature of thermal

        Returns:
            A float number, the maximum recorded temperature of thermal in Celsius
            up to nearest thousandth of one degree Celsius, e.g. 30.125
        """
        self._update_thermal_data()
        temperature = self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "temp", "N/A"
        )
        if temperature == 0.0 or temperature == -100.0:
            self.maximum_recorded = "N/A"
        else:
            if self.maximum_recorded == "N/A":
                self.maximum_recorded = temperature
            self.maximum_recorded = max(self.maximum_recorded, temperature)
        return self.maximum_recorded

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
        temperature = self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "temp", "N/A"
        )
        high = self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "high", "N/A"
        )
        crit_high = self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "crit_high", "N/A"
        )
        low = self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "low", "N/A"
        )
        crit_low = self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "crit_low", "N/A"
        )
        high_warning = temperature >= high if high != "N/A" else False
        low_warning = temperature <= low if low != "N/A" else False
        warning = str(high_warning or low_warning)
        temperature = (
            "N/A" if temperature == 0.0 or temperature == -100.0 else temperature
        )
        temperature = (
            "{:.3f}".format(temperature) if temperature != "N/A" else temperature
        )
        high = "{:.3f}".format(high) if high != "N/A" else high
        crit_high = "{:.3f}".format(crit_high) if crit_high != "N/A" else crit_high
        low = "{:.3f}".format(low) if low != "N/A" else low
        crit_low = "{:.3f}".format(crit_low) if crit_low != "N/A" else crit_low
        return {
            "key": self.thermal_name_list[self.thermal_index],
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
        return self.thermal_name_list[self.thermal_index]

    def get_name(self):
        """
        Retrieves the name of the device

        Returns:
            string: The name of the device
        """
        return self.thermal_name_list[self.thermal_index]

    def get_presence(self):
        """
        Retrieves the presence of the device

        Returns:
            bool: True if device is present, False if not
        """
        # Must be presented
        return True

    def get_warning(self):
        """
        Retrieves the warning status the device

        Returns:
            bool: True if device is warning, False if not
        """
        self._update_thermal_data()
        temperature = self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "temp", "N/A"
        )
        high = self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "high", "N/A"
        )
        low = self.thermal_data.get("tmp{}".format(self.thermal_num), {}).get(
            "low", "N/A"
        )
        high_warning = temperature >= high if high != "N/A" else False
        low_warning = temperature <= low if low != "N/A" else False
        return high_warning or low_warning

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
        # Must be OK
        return True

    def is_replaceable(self):
        """
        Indicate whether this device is replaceable.
        Returns:
            bool: True if it is replaceable.
        """
        return True
