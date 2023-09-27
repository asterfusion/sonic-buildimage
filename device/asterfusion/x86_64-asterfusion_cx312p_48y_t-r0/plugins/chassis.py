#!/usr/bin/env python

########################################################################
# Asterfusion CX312P-48Y-T Chassis API
#
# Module contains an implementation of SONiC Platform Base API and
# provides the Chassis information which are available in the platform
#
########################################################################

try:
    import os
    import sys
    from sonic_platform_base.chassis_base import ChassisBase

    sys.path.append(os.path.dirname(__file__))
    from helper import ThriftConnect
    from helper import wait_for_thrift_server_setup

except ImportError as e:
    raise ImportError(str(e) + "- required module not found")


class Chassis(ChassisBase):
    """Platform-specific Chassis class"""

    def __init__(self):
        ChassisBase.__init__(self)
        wait_for_thrift_server_setup()

    def get_model(self):
        """
        Retrieves the model number (or part number) of the device

        Returns:
            string: Model/part number of device
        """
        with ThriftConnect() as thrift_client:
            eeprom = thrift_client.pltfm_mgr_sys_eeprom_get()
        return eeprom.prod_part_num

    def get_serial(self):
        """
        Retrieves the serial number of the device

        Returns:
            string: Serial number of device
        """
        with ThriftConnect() as thrift_client:
            eeprom = thrift_client.pltfm_mgr_sys_eeprom_get()
        return eeprom.prod_ser_num

    def get_revision(self):
        """
        Retrieves the hardware revision of the device

        Returns:
            string: Revision value of device
        """
        with ThriftConnect() as thrift_client:
            eeprom = thrift_client.pltfm_mgr_sys_eeprom_get()
        return eeprom.prod_sub_ver
