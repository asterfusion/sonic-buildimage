########################################################################
# Asterfusion CX-T Devices Chassis API                                 #
#                                                                      #
# Module contains an implementation of SONiC Platform Base API and     #
# provides the Chassis information which are available in the platform #
#                                                                      #
########################################################################

try:
    import copy
    import time

    from itertools import product

    from .constants import *
    from .helper import APIHelper

    from sonic_platform_base.chassis_base import ChassisBase
    from sonic_platform.component import Component
    from sonic_platform.eeprom import Tlv
    from sonic_platform.fan import Fan
    from sonic_platform.fan_drawer import FanDrawer
    from sonic_platform.psu import Psu
    from sonic_platform.sfp import Sfp
    from sonic_platform.thermal import Thermal
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


class Chassis(ChassisBase):
    """Platform-specific Chassis class"""

    def __init__(self):
        ChassisBase.__init__(self)
        self._api_helper = APIHelper()
        self._init_device_info()
        self._init_device_data()
        self._init_change_event()

    def _init_device_info(self):
        (
            self._board_type,
            self._device_name,
            self._platform_name,
            self._num_components,
            self._num_fan_drawer,
            self._num_fan_per_drawer,
            self._num_psu,
            self._num_sfp,
            self._num_thermal,
        ) = self._api_helper.get_device_info_by_board_type()

    def _init_device_data(self):
        self._eeprom = Tlv(
            self._api_helper,
            self._board_type,
            self._device_name,
            self._platform_name,
        )
        self._component_list = list(
            map(
                lambda component_index: Component(
                    component_index,
                    self._api_helper,
                    self._board_type,
                    self._device_name,
                    self._platform_name,
                ),
                range(self._num_components),
            )
        )
        self._fan_list = list(
            map(
                lambda fand_fan_index: Fan(
                    fand_fan_index[0],
                    fand_fan_index[1],
                    self._api_helper,
                    self._board_type,
                    self._device_name,
                    self._platform_name,
                ),
                product(
                    range(self._num_fan_drawer),
                    range(self._num_fan_per_drawer),
                ),
            )
        )
        self._fan_drawer_list = list(
            map(
                lambda fand_index: FanDrawer(
                    fand_index,
                    self._api_helper,
                    self._board_type,
                    self._device_name,
                    self._platform_name,
                ),
                range(self._num_fan_drawer),
            )
        )
        self._psu_list = list(
            map(
                lambda psu_index: Psu(
                    psu_index,
                    self._api_helper,
                    self._board_type,
                    self._device_name,
                    self._platform_name,
                ),
                range(self._num_psu),
            )
        )
        self._sfp_list = list(
            map(
                lambda sfp_index: Sfp(
                    sfp_index,
                    self._api_helper,
                    self._board_type,
                    self._device_name,
                    self._platform_name,
                ),
                range(self._num_sfp),
            )
        )
        self._thermal_list = list(
            map(
                lambda thermal_index: Thermal(
                    thermal_index,
                    self._api_helper,
                    self._board_type,
                    self._device_name,
                    self._platform_name,
                ),
                range(self._num_thermal),
            )
        )

    def _init_change_event(self):
        # Change event dict is a nested dictionary where key is a device type,
        # value is a dictionary with key:value pairs in the format of
        # {"device_id":"device_event"},
        # where device_id is the device ID for this device and
        # device_event="1" represents device inserted,
        # device_event="0" represents device removed.
        # {
        #     "fan": {
        #         "0": "1",
        #         "1": "1",
        #         ...
        #     },
        #     "psu": {
        #         "0": "1",
        #         "1": "0",
        #     },
        #     "sfp": {
        #         "0": "0",
        #         "1": "1",
        #         ...
        #     },
        # }
        #
        self._change_event_dict = {}
        self._change_event_dict.setdefault(
            "fan", dict.fromkeys(range(len(self._fan_list)), "0")
        )
        self._change_event_dict.setdefault(
            "psu", dict.fromkeys(range(len(self._psu_list)), "0")
        )
        self._change_event_dict.setdefault(
            "sfp", dict.fromkeys(range(len(self._sfp_list)), "0")
        )

    ###################
    # Chassis methods #
    ###################

    def get_base_mac(self):
        """
        Retrieves the base MAC address for the chassis
        Returns:
            A string containing the MAC address in the format
            "XX:XX:XX:XX:XX:XX"
        """
        return self._eeprom.get_mac()

    def get_system_eeprom_info(self):
        """
        Retrieves the full content of system EEPROM information for the chassis
        Returns:
            A dictionary where keys are the type code defined in
            OCP ONIE TlvInfo EEPROM format and values are their corresponding
            values.
        """
        return self._eeprom.get_eeprom()

    def get_reboot_cause(self):
        """
        Retrieves the cause of the previous reboot

        Returns:
            A tuple (string, string) where the first element is a string
            containing the cause of the previous reboot. This string must be
            one of the predefined strings in this class. If the first string
            is "REBOOT_CAUSE_HARDWARE_OTHER", the second string can be used
            to pass a description of the reboot cause.
        """
        # As CX-T devices don't support IPMI, we can't get reboot-cause
        # directly through `ipmitool`. Thus we just return "Unknown reason".
        return ("Hardware - Other", "Unknown reason")

    #####################
    # Component methods #
    #####################

    def get_num_components(self):
        """
        Retrieves the number of components available on this chassis
        Returns:
            An integer, the number of components available on this chassis
        """
        return len(self._component_list)

    def get_all_components(self):
        """
        Retrieves all components available on this chassis
        Returns:
            A list of objects derived from ComponentBase representing all components
            available on this chassis
        """
        return self._component_list

    def get_component(self, index):
        """
        Retrieves component represented by (0-based) index <index>
        Args:
            index: An integer, the index (0-based) of the component to retrieve
        Returns:
            An object dervied from ComponentBase representing the specified component
        """
        component = None
        try:
            component = self._component_list[index]
        except IndexError as err:
            self._api_helper.log_warning(
                "Component index {} out of range (0-{})\n".format(
                    index, len(self._component_list) - 1
                )
            )
        return component

    ###############
    # Fan methods #
    ###############

    def get_num_fan(self):
        """
        Retrieves the number of fans available on this chassis
        Returns:
            An integer, the number of fan modules available on this chassis
        """
        return len(self._fan_list)

    def get_num_fans(self):
        """
        Retrieves the number of fans available on this chassis
        Returns:
            An integer, the number of fan modules available on this chassis
        """
        return len(self._fan_list)

    def get_all_fans(self):
        """
        Retrieves all fan modules available on this chassis
        Returns:
            A list of objects derived from FanBase representing all fan
            modules available on this chassis
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
        except IndexError as err:
            self._api_helper.log_warning(
                "Fan index {} out of range (0-{})\n".format(
                    index, len(self._fan_list) - 1
                )
            )
        return fan

    def get_num_fan_drawers(self):
        """
        Retrieves the number of fan drawers available on this chassis
        Returns:
            An integer, the number of fan drawers available on this chassis
        """
        return len(self._fan_drawer_list)

    def get_all_fan_drawers(self):
        """
        Retrieves all fan drawers available on this chassis
        Returns:
            A list of objects derived from FanDrawerBase representing all fan
            drawers available on this chassis
        """
        return self._fan_drawer_list

    def get_fan_drawer(self, index):
        """
        Retrieves fan drawers represented by (0-based) index <index>
        Args:
            index: An integer, the index (0-based) of the fan drawer to
            retrieve
        Returns:
            An object dervied from FanDrawerBase representing the specified fan
            drawer
        """
        fan_drawer = None
        try:
            fan_drawer = self._fan_drawer_list[index]
        except IndexError as err:
            self._api_helper.log_warning(
                "Fan drawer index {} out of range (0-{})\n".format(
                    index, len(self._fan_drawer_list) - 1
                )
            )
        return fan_drawer

    ###############
    # PSU methods #
    ###############

    def get_num_psus(self):
        """
        Retrieves the number of power supply units available on this chassis
        Returns:
            An integer, the number of power supply units available on this
            chassis
        """
        return len(self._psu_list)

    def get_all_psus(self):
        """
        Retrieves all power supply units available on this chassis
        Returns:
            A list of objects derived from PsuBase representing all power
            supply units available on this chassis
        """
        return self._psu_list

    def get_psu(self, index):
        """
        Retrieves power supply unit represented by (0-based) index <index>
        Args:
            index: An integer, the index (0-based) of the power supply unit to
            retrieve
        Returns:
            An object dervied from PsuBase representing the specified power
            supply unit
        """
        psu = None
        try:
            psu = self._psu_list[index]
        except IndexError as err:
            self._api_helper.log_warning(
                "PSU index {} out of range (0-{})\n".format(
                    index, len(self._psu_list) - 1
                )
            )
        return psu

    ###############
    # SFP methods #
    ###############

    def get_num_sfps(self):
        """
        Retrieves the number of sfps available on this chassis
        Returns:
            An integer, the number of sfps available on this chassis
        """
        return len(self._sfp_list)

    def get_all_sfps(self):
        """
        Retrieves all sfps available on this chassis
        Returns:
            A list of objects derived from SfpBase representing all sfps
            available on this chassis
        """
        return [sfp for sfp in self._sfp_list if sfp is not None]

    def get_sfp(self, index):
        """
        Retrieves sfp corresponding to physical port <index>
        Args:
            index: An integer (>=0), the index of the sfp to retrieve.
                   The index should correspond to the physical port in a chassis.
                   For example:-
                   1 for Ethernet0, 2 for Ethernet4 and so on for one platform.
                   0 for Ethernet0, 1 for Ethernet4 and so on for another platform.
        Returns:
            An object dervied from SfpBase representing the specified sfp
        """
        sfp = None
        try:
            sfp = self._sfp_list[index]
        except IndexError as err:
            self._api_helper.log_warning(
                "SFP index {} out of range (0-{})\n".format(
                    index, len(self._sfp_list) - 1
                )
            )
        return sfp

    def get_port_or_cage_type(self, index):
        """
        Retrieves sfp port or cage type corresponding to physical port <index>
        Args:
            index: An integer (>=0), the index of the sfp to retrieve.
                   The index should correspond to the physical port in a chassis.
                   For example:-
                   1 for Ethernet0, 2 for Ethernet4 and so on for one platform.
                   0 for Ethernet0, 1 for Ethernet4 and so on for another platform.
        Returns:
            The masks of all types of port or cage that can be supported on the port
            Types are defined in sfp_base.py
            Eg.
                Both SFP and SFP+ are supported on the port, the return value should be 0x0a
                which is 0x02 | 0x08
        """
        raise NotImplementedError

    ###################
    # THERMAL methods #
    ###################

    def get_num_thermal(self):
        """
        Retrieves the number of thermals available on this chassis
        Returns:
            An integer, the number of thermals available on this chassis
        """
        return len(self._thermal_list)

    def get_num_thermals(self):
        """
        Retrieves the number of thermals available on this chassis
        Returns:
            An integer, the number of thermals available on this chassis
        """
        return len(self._thermal_list)

    def get_all_thermals(self):
        """
        Retrieves all thermals available on this chassis
        Returns:
            A list of objects derived from ThermalBase representing all thermals
            available on this chassis
        """
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
        thermal = None
        try:
            thermal = self._thermal_list[index]
        except IndexError as err:
            self._api_helper.log_warning(
                "THERMAL index {} out of range (0-{})\n".format(
                    index, len(self._thermal_list) - 1
                )
            )
        return thermal

    def get_thermal_manager(self):
        """
        Retrieves thermal manager class on this chassis
        :return: A class derived from ThermalManagerBase representing the
        specified thermal manager. ThermalManagerBase is returned as default
        """
        raise NotImplementedError

    ##################
    # Device methods #
    ##################

    def get_model(self):
        """
        Retrieves the model number (or part number) of the device

        Returns:
            string: Model/part number of device
        """
        return str(self.get_eeprom().get_eeprom().get("0x22", "Undefined."))

    def get_serial(self):
        """
        Retrieves the serial number of the device

        Returns:
            string: Serial number of device
        """
        return str(self.get_eeprom().get_eeprom().get("0x23", "Undefined."))

    def get_revision(self):
        """
        Retrieves the hardware revision of the device

        Returns:
            string: Revision value of device
        """
        return str(self.get_eeprom().get_eeprom().get("0x27", "Undefined."))

    #################
    # Other methods #
    #################

    def get_name(self):
        """
        Retrieves the name of the device
            Returns:
            string: The name of the device
        """
        return self._device_name

    def get_watchdog(self):
        """
        Retreives hardware watchdog device on this chassis

        Returns:
            An object derived from WatchdogBase representing the hardware
            watchdog device
        """
        return self._watchdog

    def get_eeprom(self):
        """
        Retreives eeprom device on this chassis

        Returns:
            An object derived from WatchdogBase representing the hardware
            eeprom device
        """
        return self._eeprom

    def get_change_event(self, timeout=0):
        """
        Returns a nested dictionary containing all devices which have
        experienced a change at chassis level

        Args:
            timeout: Timeout in milliseconds (optional). If timeout == 0,
                this method will block until a change is detected.

        Returns:
            (bool, dict):
                - True if call successful, False if not;
                - A nested dictionary where key is a device type,
                  value is a dictionary with key:value pairs in the format of
                  {"device_id":"device_event"},
                  where device_id is the device ID for this device and
                        device_event,
                             status="1" represents device inserted,
                             status="0" represents device removed.
                  Ex. {"fan":{"0":"0", "2":"1"}, "sfp":{"11":"0"}}
                      indicates that fan 0 has been removed, fan 2
                      has been inserted and sfp 11 has been removed.
                  Specifically for SFP event, besides SFP plug in and plug out,
                  there are some other error event could be raised from SFP, when
                  these error happened, SFP eeprom will not be avalaible, XCVRD shall
                  stop to read eeprom before SFP recovered from error status.
                      status="2" I2C bus stuck,
                      status="3" Bad eeprom,
                      status="4" Unsupported cable,
                      status="5" High Temperature,
                      status="6" Bad cable.
        """
        # Create a back-up change-event-dict which will be the return value only if call successful
        change_event_dict = copy.deepcopy(self._change_event_dict)
        start = time.time()
        while True:
            # We need to reset call status to False because we could get error during every loop
            succeeded = True
            detected = False
            presence = False
            try:
                # Walk throught all fans and get current fan presence
                for index, fan in enumerate(self._fan_list):
                    presence = fan.get_presence()
                    if str(int(presence)) != change_event_dict["fan"][index]:
                        change_event_dict["fan"][index] = str(int(presence))
                        detected = True
                # Walk throught all psus and get current psu presence
                for index, psu in enumerate(self._psu_list):
                    presence = psu.get_presence()
                    if str(int(presence)) != change_event_dict["psu"][index]:
                        change_event_dict["psu"][index] = str(int(presence))
                        detected = True
                # Walk throught all sfps and get current sfp presence
                for index, sfp in enumerate(self._sfp_list):
                    presence = sfp.get_presence()
                    if str(int(presence)) != change_event_dict["sfp"][index]:
                        change_event_dict["sfp"][index] = str(int(presence))
                        detected = True
            except Exception as err:
                succeeded = False
                self._api_helper.log_warning(
                    "Failed in getting device change event: {}\n".format(err)
                )
            # Four conditions(OR gate) are concerned here:
            # 1) When succeeded is False. In other words, this method cannot run well. So we shouldn`t let the loop go on.
            if succeeded:  # If call successful, update the change-event-dict
                self._change_event_dict = copy.deepcopy(change_event_dict)
            else:  # Otherwise break the loop immediately
                break
            # 2) When a change event is detected. Always break the loop as long as a change event is detected.
            if detected:
                break
            # 3) When timeout is 0. Under this condition we should never break the loop until a change event is detected.
            if timeout == 0 and detected:
                break
            # 4) When time elapsed is longer than timeout. It doesn`t matter if there`s any change event detected,
            #    As long as it has taken long enough on running this method, we just break the loop.
            if timeout > 0 and time.time() - start > timeout / 1000:
                break
            time.sleep(1)
        return succeeded, change_event_dict
