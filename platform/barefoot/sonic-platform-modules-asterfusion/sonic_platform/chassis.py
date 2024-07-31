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

    from functools import cached_property
    from itertools import product

    from .pltfm_utils.constants import *
    from .pltfm_utils.helper import APIHelper

    from sonic_platform_base.chassis_base import ChassisBase
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


class Chassis(ChassisBase):
    """Platform-specific Chassis class"""

    def __init__(self):
        ChassisBase.__init__(self)
        self._api_helper = APIHelper()
        self._init_device_info()

    def _init_device_info(self):
        (
            self._board_id,
            self._device_name,
            self._platform_name,
            self._num_component,
            self._num_fan_drawer,
            self._num_fan_per_drawer,
            self._num_psu,
            self._num_sfp,
            self._num_thermal,
        ) = self._api_helper.get_device_info_by_board_type()
        # fmt: off
        self._api_helper.log_info("Initialized Device Information:")
        self._api_helper.log_info("Board ID               : {}".format(hex(self._board_id)))
        self._api_helper.log_info("Device                 : {}".format(self._device_name))
        self._api_helper.log_info("Platform               : {}".format(self._platform_name))
        self._api_helper.log_info("Component Count        : {}".format(self._num_component))
        self._api_helper.log_info("Fan Drawer Count       : {}".format(self._num_fan_drawer))
        self._api_helper.log_info("Fan Count per Drawer   : {}".format(self._num_fan_per_drawer))
        self._api_helper.log_info("Fan Count in Total     : {}".format(self._num_fan_drawer * self._num_fan_per_drawer))
        self._api_helper.log_info("Power Supply Unit Count: {}".format(self._num_psu))
        self._api_helper.log_info("Front Panel Port Count : {}".format(self._num_sfp))
        self._api_helper.log_info("Thermal Sensor Count   : {}".format(self._num_thermal))
        # fmt: on

    ###################
    # LAZY attributes #
    ###################

    @cached_property
    def __eeprom(self):
        from sonic_platform.eeprom import Tlv

        self._eeprom = Tlv(
            self._api_helper,
            self._board_id,
            self._device_name,
            self._platform_name,
        )
        self._api_helper.log_debug("EEPROM has been initialized")
        return self._eeprom

    @cached_property
    def __component_list(self):
        from sonic_platform.component import Component

        self._component_list = list(
            map(
                lambda component_index: Component(
                    component_index,
                    self._api_helper,
                    self._board_id,
                    self._device_name,
                    self._platform_name,
                ),
                range(self._num_component),
            )
        )
        self._api_helper.log_debug("Component list has been initialized")
        return self._component_list

    @cached_property
    def __fan_list(self):
        from sonic_platform.fan import Fan

        self._fan_list = list(
            map(
                lambda fand_fan_index: Fan(
                    fand_fan_index[0],
                    fand_fan_index[1],
                    self._api_helper,
                    self._board_id,
                    self._device_name,
                    self._platform_name,
                ),
                product(
                    range(self._num_fan_drawer),
                    range(self._num_fan_per_drawer),
                ),
            )
        )
        self._api_helper.log_debug("Fan list has been initialized")
        return self._fan_list

    @cached_property
    def __fan_drawer_list(self):
        from sonic_platform.fan_drawer import FanDrawer

        self._fan_drawer_list = list(
            map(
                lambda fand_index: FanDrawer(
                    fand_index,
                    self._api_helper,
                    self._board_id,
                    self._device_name,
                    self._platform_name,
                ),
                range(self._num_fan_drawer),
            )
        )
        self._api_helper.log_debug("Fan drawer list has been initialized")
        return self._fan_drawer_list

    @cached_property
    def __psu_list(self):
        from sonic_platform.psu import Psu

        self._psu_list = list(
            map(
                lambda psu_index: Psu(
                    psu_index,
                    self._api_helper,
                    self._board_id,
                    self._device_name,
                    self._platform_name,
                ),
                range(self._num_psu),
            )
        )
        self._api_helper.log_debug("PSU list has been initialized")
        return self._psu_list

    @cached_property
    def __sfp_list(self):
        from sonic_platform.sfp import Sfp

        self._sfp_list = list(
            map(
                lambda sfp_index: Sfp(
                    sfp_index,
                    self._api_helper,
                    self._board_id,
                    self._device_name,
                    self._platform_name,
                ),
                range(self._num_sfp),
            )
        )
        self._api_helper.log_debug("SFP list has been initialized")
        return self._sfp_list

    @cached_property
    def __thermal_list(self):
        from sonic_platform.thermal import Thermal

        self._thermal_list = list(
            map(
                lambda thermal_index: Thermal(
                    thermal_index,
                    self._api_helper,
                    self._board_id,
                    self._device_name,
                    self._platform_name,
                ),
                range(self._num_thermal),
            )
        )
        self._api_helper.log_debug("Thermal list has been initialized")
        return self._thermal_list

    @property
    def __device_presence_dict(self):
        if not hasattr(self, "_device_presence_dict"):
            self._device_presence_dict = {}
            self._device_presence_dict.setdefault(
                "fan_drawer",
                dict.fromkeys(map(str, range(len(self.__fan_drawer_list))), "0"),
            )
            self._device_presence_dict.setdefault(
                "fan", dict.fromkeys(map(str, range(len(self.__fan_list))), "0")
            )
            self._device_presence_dict.setdefault(
                "psu", dict.fromkeys(map(str, range(len(self.__psu_list))), "0")
            )
            self._device_presence_dict.setdefault(
                "sfp", dict.fromkeys(map(str, range(len(self.__sfp_list))), "0")
            )
            self._api_helper.log_debug("Device presence dict has been initialized")
        return self._device_presence_dict

    @__device_presence_dict.setter
    def __device_presence_dict(self, device_presence_dict):
        self._device_presence_dict = device_presence_dict

    @__device_presence_dict.deleter
    def __device_presence_dict(self):
        del self._device_presence_dict

    ###################
    # CHASSIS methods #
    ###################

    def get_base_mac(self):
        """
        Retrieves the base MAC address for the chassis
        Returns:
            A string containing the MAC address in the format
            "XX:XX:XX:XX:XX:XX"
        """
        return self.__eeprom.get_mac()

    def get_system_eeprom_info(self):
        """
        Retrieves the full content of system EEPROM information for the chassis
        Returns:
            A dictionary where keys are the type code defined in
            OCP ONIE TlvInfo EEPROM format and values are their corresponding
            values.
        """
        return self.__eeprom.get_eeprom()

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
        # Since CX-T devices don't support IPMI, we can't get reboot-cause
        # directly through `ipmitool`. Thus we just return "Non-Hardware".
        return (REBOOT_CAUSE_NON_HARDWARE, "")

    #####################
    # COMPONENT methods #
    #####################

    def get_num_components(self):
        """
        Retrieves the number of components available on this chassis
        Returns:
            An integer, the number of components available on this chassis
        """
        return len(self.__component_list)

    def get_all_components(self):
        """
        Retrieves all components available on this chassis
        Returns:
            A list of objects derived from ComponentBase representing all components
            available on this chassis
        """
        return self.__component_list

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
            component = self.__component_list[index]
        except IndexError as err:
            self._api_helper.log_warning(
                "Component index {} out of range (0-{})".format(
                    index, len(self.__component_list) - 1
                )
            )
        return component

    ###############
    # FAN methods #
    ###############

    def get_num_fan(self):
        """
        Retrieves the number of fans available on this chassis
        Returns:
            An integer, the number of fan modules available on this chassis
        """
        return len(self.__fan_list)

    def get_num_fans(self):
        """
        Retrieves the number of fans available on this chassis
        Returns:
            An integer, the number of fan modules available on this chassis
        """
        return len(self.__fan_list)

    def get_all_fans(self):
        """
        Retrieves all fan modules available on this chassis
        Returns:
            A list of objects derived from FanBase representing all fan
            modules available on this chassis
        """
        return self.__fan_list

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
            fan = self.__fan_list[index]
        except IndexError as err:
            self._api_helper.log_warning(
                "Fan index {} out of range (0-{})".format(
                    index, len(self.__fan_list) - 1
                )
            )
        return fan

    def get_num_fan_drawers(self):
        """
        Retrieves the number of fan drawers available on this chassis
        Returns:
            An integer, the number of fan drawers available on this chassis
        """
        return len(self.__fan_drawer_list)

    def get_all_fan_drawers(self):
        """
        Retrieves all fan drawers available on this chassis
        Returns:
            A list of objects derived from FanDrawerBase representing all fan
            drawers available on this chassis
        """
        return self.__fan_drawer_list

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
            fan_drawer = self.__fan_drawer_list[index]
        except IndexError as err:
            self._api_helper.log_warning(
                "Fan drawer index {} out of range (0-{})".format(
                    index, len(self.__fan_drawer_list) - 1
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
        return len(self.__psu_list)

    def get_all_psus(self):
        """
        Retrieves all power supply units available on this chassis
        Returns:
            A list of objects derived from PsuBase representing all power
            supply units available on this chassis
        """
        return self.__psu_list

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
            psu = self.__psu_list[index]
        except IndexError as err:
            self._api_helper.log_warning(
                "PSU index {} out of range (0-{})".format(
                    index, len(self.__psu_list) - 1
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
        return len(self.__sfp_list)

    def get_all_sfps(self):
        """
        Retrieves all sfps available on this chassis
        Returns:
            A list of objects derived from SfpBase representing all sfps
            available on this chassis
        """
        return [sfp for sfp in self.__sfp_list if sfp is not None]

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
            sfp = self.__sfp_list[index]
        except IndexError as err:
            self._api_helper.log_warning(
                "SFP index {} out of range (0-{})".format(
                    index, len(self.__sfp_list) - 1
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
        return len(self.__thermal_list)

    def get_num_thermals(self):
        """
        Retrieves the number of thermals available on this chassis
        Returns:
            An integer, the number of thermals available on this chassis
        """
        return len(self.__thermal_list)

    def get_all_thermals(self):
        """
        Retrieves all thermals available on this chassis
        Returns:
            A list of objects derived from ThermalBase representing all thermals
            available on this chassis
        """
        return self.__thermal_list

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
            thermal = self.__thermal_list[index]
        except IndexError as err:
            self._api_helper.log_warning(
                "Thermal index {} out of range (0-{})".format(
                    index, len(self.__thermal_list) - 1
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
    # DEVICE methods #
    ##################

    def get_model(self):
        """
        Retrieves the model number (or part number) of the device

        Returns:
            string: Model/part number of device
        """
        return str(self.get_eeprom().get_eeprom().get("prod_part_num", NOT_AVAILABLE))

    def get_serial(self):
        """
        Retrieves the serial number of the device

        Returns:
            string: Serial number of device
        """
        return str(self.get_eeprom().get_eeprom().get("prod_ser_num", NOT_AVAILABLE))

    def get_revision(self):
        """
        Retrieves the hardware revision of the device

        Returns:
            string: Revision value of device
        """
        return str(self.get_eeprom().get_eeprom().get("prod_sub_ver", NOT_AVAILABLE))

    #################
    # OTHER methods #
    #################

    def get_helper(self):
        """
        Retrieves the helper of the chassis
            Returns:
            string: The helper object of the chassis
        """
        return self._api_helper

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
        return self.__eeprom

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
        # Create a back-up device presence dict to avoid detection failure
        device_presence_dict = copy.deepcopy(self.__device_presence_dict)
        change_event_dict = {"fan_drawer": {}, "fan": {}, "psu": {}, "sfp": {}}
        start = time.time()
        while True:
            timediff = time.time()
            succeeded = True
            detected = False
            presence = False
            reason = None
            try:
                # Walk throught all fan drawers and get current fan drawer presence
                for index, fan_drawer in enumerate(self.__fan_drawer_list):
                    fan_drawer_index = str(index)
                    presence = fan_drawer.get_presence()
                    if (
                        str(int(presence))
                        != device_presence_dict["fan_drawer"][fan_drawer_index]
                    ):
                        device_presence_dict["fan_drawer"][fan_drawer_index] = str(
                            int(presence)
                        )
                        change_event_dict["fan_drawer"][fan_drawer_index] = str(
                            int(presence)
                        )
                        detected = True
                        self._api_helper.log_info(
                            "Detected {} {} event".format(
                                fan_drawer.get_name(),
                                "inserted" if presence else "removed",
                            )
                        )
                # Walk throught all fans and get current fan presence
                for index, fan in enumerate(self.__fan_list):
                    fan_index = str(index)
                    presence = fan.get_presence()
                    if str(int(presence)) != device_presence_dict["fan"][fan_index]:
                        device_presence_dict["fan"][fan_index] = str(int(presence))
                        change_event_dict["fan"][fan_index] = str(
                            int(presence)
                        )
                        detected = True
                        self._api_helper.log_info(
                            "Detected {} {} event".format(
                                fan.get_name(), "inserted" if presence else "removed"
                            )
                        )
                # Walk throught all psus and get current psu presence
                for index, psu in enumerate(self.__psu_list):
                    psu_index = str(index)
                    presence = psu.get_presence()
                    if str(int(presence)) != device_presence_dict["psu"][psu_index]:
                        device_presence_dict["psu"][psu_index] = str(int(presence))
                        change_event_dict["psu"][psu_index] = str(
                            int(presence)
                        )
                        detected = True
                        self._api_helper.log_info(
                            "Detected {} {} event".format(
                                psu.get_name(), "inserted" if presence else "removed"
                            )
                        )
                # Walk throught all sfps and get current sfp presence
                for index, sfp in enumerate(self.__sfp_list):
                    sfp_index = str(index)
                    presence = sfp.get_presence()
                    if str(int(presence)) != device_presence_dict["sfp"][sfp_index]:
                        device_presence_dict["sfp"][sfp_index] = str(int(presence))
                        change_event_dict["sfp"][sfp_index] = str(
                            int(presence)
                        )
                        detected = True
                        self._api_helper.log_info(
                            "Detected {} {} event".format(
                                sfp.get_name(), "inserted" if presence else "removed"
                            )
                        )
            except Exception as err:
                succeeded = False
                reason = err
            # Four conditions(OR gate) are concerned here:
            # 1) When succeeded is False. In other words, this method cannot run well. So we shouldn`t let the loop go on.
            if succeeded:  # If call successful, update the device presence dict
                self.__device_presence_dict = copy.deepcopy(device_presence_dict)
                self._api_helper.log_debug("Succeeded in updating device presence dict")
            else:  # Otherwise break the loop immediately
                self._api_helper.log_debug(
                    "Failed in updating device presence dict: {}".format(reason)
                )
                return succeeded, change_event_dict
            # 2) When a change event is detected. Always break the loop as long as a change event is detected.
            # 3) When timeout is 0. Under this condition we should never break the loop until a change event is detected.
            if detected or (timeout == 0 and detected):
                self._api_helper.log_debug("Succeeded in updating device presence dict")
                return succeeded, change_event_dict
            # 4) When time elapsed is longer than timeout. It doesn`t matter if there`s any change event detected,
            #    As long as it has taken long enough on running this method, we just break the loop.
            if timeout > 0 and time.time() - start > timeout / 1000:
                self._api_helper.log_debug(
                    "Failed in updating device presence dict due to time out"
                )
                return succeeded, change_event_dict
            time.sleep(max(0, 1 + timediff - time.time()))
