####################################################################
# Asterfusion CX-T Devices Power Supply Unit                       #
#                                                                  #
# Module contains an implementation of SONiC Platform Base API and #
# provides the PSUs status which are available in the platform     #
#                                                                  #
####################################################################

try:
    import copy

    from .constants import *
    from .helper import APIHelper

    from sonic_platform_base.psu_base import PsuBase
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


class Psu(PsuBase):
    """Platform-specific Psu class"""

    def __init__(self, psu_index, helper, hwver, hwsku, platform):
        PsuBase.__init__(self)
        self._api_helper = helper  # type: APIHelper
        # Init device
        self.hwver = hwver
        self.hwsku = hwsku
        self.platform = platform
        # Init index
        self.psu_index = psu_index + 1
        self.psu_presence = False
        self.psu_data = {}
        self._init_psu_name_list()
        self._update_psu_data()

    def _init_psu_name_list(self):
        self.psu_name_list = copy.deepcopy(PSU_NAME_LIST)

    def _update_psu_data(self):
        with self._api_helper.thrift_connect() as client:
            self.psu_presence = bool(
                client.pltfm_mgr_pwr_supply_present_get(self.psu_index)
            )
            if self.psu_presence:
                self.psu_data = vars(
                    client.pltfm_mgr_pwr_supply_info_get(self.psu_index)
                )

    def get_num_fans(self):
        """
        Retrieves the number of fan modules available on this PSU

        Returns:
            An integer, the number of fan modules available on this PSU
        """
        # Need support from BMC. Not implemented yet.
        return len(self._fan_list)

    def get_all_fans(self):
        """
        Retrieves all fan modules available on this PSU

        Returns:
            A list of objects derived from FanBase representing all fan
            modules available on this PSU
        """
        # Need support from BMC. Not implemented yet.
        return self._fan_list

    def get_fan(self, index=0):
        """
        Retrieves fan module represented by (0-based) index <index>

        Args:
            index: An integer, the index (0-based) of the fan module to
            retrieve

        Returns:
            An object dervied from FanBase representing the specified fan
            module
        """
        try:
            return self._fan_list[index]
        except IndexError as err:
            self._api_helper.log_debug(
                "Fan index {} out of range (0-{})\n".format(
                    index, len(self._fan_list) - 1
                )
            )
            # Need support from BMC. Not implemented yet.
            return None

    def get_num_thermals(self):
        """
        Retrieves the number of thermals available on this PSU

        Returns:
            An integer, the number of thermals available on this PSU
        """
        # Need support from BMC. Not implemented yet.
        return len(self._thermal_list)

    def get_all_thermals(self):
        """
        Retrieves all thermals available on this PSU

        Returns:
            A list of objects derived from ThermalBase representing all thermals
            available on this PSU
        """
        # Need support from BMC. Not implemented yet.
        return self._thermal_list

    def get_thermal(self, index):
        """
        Retrieves thermal unit represented by (0-based) index <index>

        Args:
            index: An integer, the index (0-based) of the thermal to
            retrieve

        Returns:
            An object dervied from ThermalBase representing the specified thermal
        """
        # Need support from BMC. Not implemented yet.
        try:
            return self._thermal_list[index]
        except IndexError as err:
            self._api_helper.log_debug(
                "THERMAL index {} out of range (0-{})\n".format(
                    index, len(self._thermal_list) - 1
                )
            )
            return None

    def get_voltage(self):
        """
        Retrieves current PSU voltage output

        Returns:
            A float number, the output voltage in volts,
            e.g. 12.1
        """
        self._update_psu_data()
        return float(self.psu_data.get("vout", 0.0)) / 1000.0

    def get_current(self):
        """
        Retrieves present electric current supplied by PSU

        Returns:
            A float number, the electric current in amperes, e.g 15.4
        """
        self._update_psu_data()
        return float(self.psu_data.get("iout", 0.0)) / 1000.0

    def get_power(self):
        """
        Retrieves current energy supplied by PSU

        Returns:
            A float number, the power in watts, e.g. 302.6
        """
        self._update_psu_data()
        return float(self.psu_data.get("pwr_out", 0.0)) / 1000.0

    def get_powergood_status(self):
        """
        Retrieves the powergood status of PSU

        Returns:
            A boolean, True if PSU has stablized its output voltages and passed all
            its internal self-tests, False if not.
        """
        self._update_psu_data()
        return bool(self.psu_data.get("pwr_out", 0.0))

    def set_status_led(self, color):
        """
        Sets the state of the PSU status LED

        Args:
            color: A string representing the color with which to set the
                   PSU status LED

        Returns:
            bool: True if status LED state is set successfully, False if not
        """
        # Need support from BMC. Not implemented yet.
        return False

    def get_status_led(self):
        """
        Gets the state of the PSU status LED

        Returns:
            A string, one of the predefined STATUS_LED_COLOR_* strings above
        """
        self._update_psu_data()
        if self.psu_presence and bool(self.psu_data.get("pwr_out", 0.0)):
            return self.STATUS_LED_COLOR_GREEN
        elif self.psu_presence and not bool(self.psu_data.get("pwr_out", 0.0)):
            return self.STATUS_LED_COLOR_AMBER
        else:
            return self.STATUS_LED_COLOR_RED

    def get_temperature(self):
        """
        Retrieves current temperature reading from PSU

        Returns:
            A float number of current temperature in Celsius up to nearest thousandth
            of one degree Celsius, e.g. 30.125
        """
        # Need support from BMC. Not implemented yet.
        return "N/A"

    def get_temperature_high_threshold(self):
        """
        Retrieves the high threshold temperature of PSU

        Returns:
            A float number, the high threshold temperature of PSU in Celsius
            up to nearest thousandth of one degree Celsius, e.g. 30.125
        """
        # Need support from BMC. Not implemented yet.
        return "N/A"

    def get_voltage_high_threshold(self):
        """
        Retrieves the high threshold PSU voltage output

        Returns:
            A float number, the high threshold output voltage in volts,
            e.g. 12.1
        """
        # Need support from BMC. Not implemented yet.
        return "N/A"

    def get_voltage_low_threshold(self):
        """
        Retrieves the low threshold PSU voltage output

        Returns:
            A float number, the low threshold output voltage in volts,
            e.g. 12.1
        """
        # Need support from BMC. Not implemented yet.
        return "N/A"

    def get_maximum_supplied_power(self):
        """
        Retrieves the maximum supplied power by PSU

        Returns:
            A float number, the maximum power output in Watts.
            e.g. 1200.1
        """
        # Need support from BMC. Not implemented yet.
        return "N/A"

    @classmethod
    def get_status_master_led(cls):
        """
        Gets the state of the Master status LED for a given device-type

        Returns:
            A string, one of the predefined STATUS_LED_COLOR_* strings.
        """
        return cls._psu_master_led_color

    @classmethod
    def set_status_master_led(cls, color):
        """
        Gets the state of the Master status LED for a given device-type

        Returns:
            bool: True if status LED state is set successfully, False if
                  not
        """
        cls._psu_master_led_color = color
        return True

    def get_name(self):
        """
        Retrieves the name of the device

        Returns:
            string: The name of the device
        """
        return self.psu_name_list[self.psu_index - 1]

    def get_presence(self):
        """
        Retrieves the presence of the device

        Returns:
            bool: True if device is present, False if not
        """
        self._update_psu_data()
        return self.psu_presence

    def get_warning(self):
        """
        Retrieves PSU's warning status

        Returns:
            A boolean, literally True or False
        """
        self._update_psu_data()
        return not self.psu_presence

    def get_model(self):
        """
        Retrieves the model number (or part number) of the device

        Returns:
            string: Model/part number of device
        """
        self._update_psu_data()
        return str(self.psu_data.get("model", "N/A"))

    def get_serial(self):
        """
        Retrieves the serial number of the device

        Returns:
            string: Serial number of device
        """
        self._update_psu_data()
        return str(self.psu_data.get("serial", "N/A"))

    def get_revision(self):
        """
        Retrieves the hardware revision of the device

        Returns:
            string: Revision value of device
        """
        self._update_psu_data()
        return str(self.psu_data.get("rev", "N/A"))

    def get_status(self):
        """
        Retrieves the operational status of the device

        Returns:
            A boolean value, True if device is operating properly, False if not
        """
        self._update_psu_data()
        return self.psu_presence and bool(self.psu_data.get("pwr_out", 0.0))

    def is_replaceable(self):
        """
        Indicate whether this device is replaceable.
        Returns:
            bool: True if it is replaceable.
        """
        return True
