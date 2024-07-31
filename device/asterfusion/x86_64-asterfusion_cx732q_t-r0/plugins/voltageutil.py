#!/usr/bin/env python

#############################################################################
# Asterfusion CX532P-T Voltage Plugin
#
# Module contains Voltage states which are available in the platform
#
#############################################################################

try:
    import os
    import sys

    sys.path.append(os.path.dirname(__file__))
    from helper import ThriftConnect
    from helper import wait_for_thrift_server_setup

except ImportError as e:
    raise ImportError (str(e) + "- required module not found")


class VoltageUtil(object):
    """Platform-specific FanUtil class"""

    def __init__(self):
        self.index_to_voltage_mapping = {
            0: ['Barefoot VDDA 1.8V'],
            1: ['Barefoot VDDA 1.7V'],
            2: ['Payload 12V'],
            3: ['Payload 3.3V'],
            4: ['Payload 5V'],
            5: ['Barefoot VDD 1.8V'],
            6: ['Barefoot Core'],
            7: ['Barefoot VDDT 0.86V']
        }
        wait_for_thrift_server_setup()

    def get_num_voltage(self):
        return len(self.index_to_voltage_mapping)

    def get_voltage_info_key(self, index):
        if index is None:
            return False
        return self.index_to_voltage_mapping[index][0]

    def get_voltage_info_dict(self, index):
        if index is None:
            return False
        with ThriftConnect() as thrift_client:
            data = thrift_client.pltfm_mgr_pwr_rail_info_get(index)
        voltage_info_dict = {}
        voltage_info_dict['key'] = self.index_to_voltage_mapping[index][0]
        voltage_info_dict['core_voltage'] = '%.2f' % (float(getattr(data, 'vrail{0}'.format(index+1)))/1000)
        return voltage_info_dict
