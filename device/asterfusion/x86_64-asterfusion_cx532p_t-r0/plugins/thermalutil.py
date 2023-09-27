#!/usr/bin/env python

#############################################################################
# Asterfusion CX532P-T Thermal Plugin
#
# Module contains an implementation of SONiC Thermal Base API and
# provides the Thermal status which are available in the platform
#
#############################################################################

try:
    import os
    import sys
    from sonic_platform_base.thermal_base import ThermalBase

    sys.path.append(os.path.dirname(__file__))
    from helper import ThriftConnect
    from helper import (
        get_x86_thermal_names,
        get_x86_thermal_values,
        wait_for_thrift_server_setup,
    )

except ImportError as e:
    raise ImportError (str(e) + "- required module not found")


class ThermalUtil(ThermalBase):
    """Platform-specific ThermalUtil class"""

    def __init__(self):
        ThermalBase.__init__(self)
        self.x86_thermal_names = get_x86_thermal_names()
        self.x86_thermal_num = len(self.x86_thermal_names)
        self.dev_thermal_names = [
            'Mainboard Front Left',
            'Mainboard Front Right',
            'Barefoot Ambient',
            'Barefoot Junction',
            'Fan 1',
            'Fan 2',
        ]
        self.dev_thermal_num = len(self.dev_thermal_names)
        self.index_to_temp_mapping = {}
        for index, thermal_name in enumerate(self.x86_thermal_names):
            self.index_to_temp_mapping[index] = thermal_name
        for index, thermal_name in enumerate(self.dev_thermal_names):
            self.index_to_temp_mapping[index + self.x86_thermal_num] = thermal_name
        wait_for_thrift_server_setup()

    def get_num_thermal(self):
        return len(self.index_to_temp_mapping)

    def get_thermal_info_key(self, index):
        if index is None:
            return False
        return self.index_to_temp_mapping[index]

    def get_thermal_info_dict(self, index):
        if index is None:
            return False
        thermal_info_dict = {}
        thermal_info_dict['key'] = self.index_to_temp_mapping[index]
        thermal_info_dict['temperature'] = 'N/A'
        thermal_info_dict['high_threshold'] = 'N/A'
        thermal_info_dict['critical_high_threshold'] = 'N/A'
        thermal_info_dict['low_threshold'] = 'N/A'
        thermal_info_dict['critical_low_threshold'] = 'N/A'
        thermal_info_dict['warning_status'] = 'false'
        if index < self.x86_thermal_num:
            x86_thermal_values = get_x86_thermal_values()
            thermal_info_dict['temperature'] = x86_thermal_values[index].get('temp', 'N/A')
            thermal_info_dict['high_threshold'] = x86_thermal_values[index].get('high', 'N/A')
            thermal_info_dict['critical_high_threshold'] = x86_thermal_values[index].get('crit', 'N/A')
            try:
                thermal_info_dict['warning_status'] = 'True' \
                    if float(thermal_info_dict['temperature']) > float(thermal_info_dict['high_threshold']) \
                    else 'False'
            except ValueError:
                thermal_info_dict['warning_status'] = 'False'
            if thermal_info_dict['temperature'] != 'N/A':
                thermal_info_dict['temperature'] = '%.1f' % thermal_info_dict['temperature']
            if thermal_info_dict['high_threshold'] != 'N/A':
                thermal_info_dict['high_threshold'] = '%.1f' % thermal_info_dict['high_threshold']
            if thermal_info_dict['critical_high_threshold'] != 'N/A':
                thermal_info_dict['critical_high_threshold'] = '%.1f' % thermal_info_dict['critical_high_threshold']
        else:
            with ThriftConnect() as thrift_client:
                data = thrift_client.pltfm_mgr_sys_tmp_get()
            thermal_info_dict['temperature'] = '%.1f' % \
                (float(getattr(data, 'tmp{0}'.format(index-self.x86_thermal_num+1))))
            if thermal_info_dict['temperature'] == '-100.0':
                thermal_info_dict['temperature'] = 'N/A'
        return thermal_info_dict
