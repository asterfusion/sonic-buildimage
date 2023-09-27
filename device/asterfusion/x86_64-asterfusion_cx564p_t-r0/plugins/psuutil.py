#!/usr/bin/env python

#############################################################################
# Asterfusion CX564P-T PSU Plugin
#
# Module contains an implementation of SONiC PSU Base API and
# provides the PSUs status which are available in the platform
#
#################################
############################################

try:
    import os
    import sys
    from sonic_psu.psu_base import PsuBase

    sys.path.append(os.path.dirname(__file__))
    from helper import ThriftConnect
    from helper import wait_for_thrift_server_setup

except ImportError as e:
    raise ImportError (str(e) + "- required module not found")


class PsuUtil(PsuBase):
    """Platform-specific PSUutil class"""

    def __init__(self):
        PsuBase.__init__(self)
        wait_for_thrift_server_setup()

    def get_num_psus(self):
        return 2

    def get_psu_status(self, index):
        """
        Retrieves the oprational status of power supply unit (PSU) defined
                by 1-based index <index>
        :param index: An integer, 1-based index of the PSU of which to query status
        :return: Boolean, True if PSU is operating properly, False if PSU is faulty
        """
        if index is None:
            return False
        with ThriftConnect() as thrift_client:
            status = thrift_client.pltfm_mgr_pwr_supply_present_get(index)
        return status == 1

    def get_psu_presence(self, index):
        """
        Retrieves the presence status of power supply unit (PSU) defined
                by 1-based index <index>
        :param index: An integer, 1-based index of the PSU of which to query status
        :return: Boolean, True if PSU is plugged, False if not
        """
        if index is None:
            return False
        with ThriftConnect() as thrift_client:
            status = thrift_client.pltfm_mgr_pwr_supply_present_get(index)
        return status == 1

    def get_psu_iout(self, index):
        """
        Retrieves the output current in milli amperes of current supply unit (PSU) defined
                by 1-based index <index>
        :param index: An integer, 1-based index of the PSU of which to query status
        :return: Interger, Output current in milli amperes
        """
        if index is None:
            return False
        with ThriftConnect() as thrift_client:
            current_data = thrift_client.pltfm_mgr_pwr_supply_info_get(index)
        if current_data.pwr_out > 0 :
            return str(current_data.iout/1000.00)
        return "0"

    def get_psu_vout(self, index):
        """
        Retrieves the output voltage in milli volts of voltage supply unit (PSU) defined
                by 1-based index <index>
        :param index: An integer, 1-based index of the PSU of which to query status
        :return: Interger, Output voltage in milli volts
        """
        if index is None:
            return False
        with ThriftConnect() as thrift_client:
            voltage_data = thrift_client.pltfm_mgr_pwr_supply_info_get(index)
        if voltage_data.pwr_out > 0:
            return str(voltage_data.vout/1000.00)
        return "0"

    def get_psu_pout(self, index):
        """
        Retrieves the output power in milli watts of power supply unit (PSU) defined
                by 1-based index <index>
        :param index: An integer, 1-based index of the PSU of which to query status
        :return: Interger, Output power in milli watts
        """
        if index is None:
            return False
        with ThriftConnect() as thrift_client:
            power_data = thrift_client.pltfm_mgr_pwr_supply_info_get(index)
        if power_data.pwr_out > 0 :
            return str(power_data.pwr_out/1000.00)
        return "0"

    def get_psu_temp(self, index):
        """
        Retrieves the output temperature in Celsius of temperature supply unit (PSU) defined
                by 1-based index <index>
        :param index: An integer, 1-based index of the PSU of which to query status
        :return: Interger, Output power in Celsius
        """
        return 'N/A'

    def get_psu_warning(self, index):
        """
        Retrieves the warning status of power supply unit defined by 1-based index <index>
        :param index: An integer, 1-based index of the PSU of which to query status
        :return: Boolean, Warning status
        """
        if index is None:
            return False
        with ThriftConnect() as thrift_client:
            status = thrift_client.pltfm_mgr_pwr_supply_present_get(index)
        return status == 0

    def get_psu_direction(self, index):
        """
        Retrieves the fan direction of power supply unit defined by 1-based index <index>
        :param index: An integer, 1-based index of the PSU of which to query status
        :return: String, Literal "intake" or "exhaust"
        """
        # "intake"
        return "exhaust"

    def get_psu_direction_warning(self, index):
        """
        Retrieves the warning status of fan of power supply unit defined by 1-based index <index>
        :param index: An integer, 1-based index of the PSU of which to query status
        :return: Boolean, Warning status
        """
        return False
