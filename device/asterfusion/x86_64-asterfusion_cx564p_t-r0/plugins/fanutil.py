#!/usr/bin/env python

#############################################################################
# Asterfusion CX564P-T Fan Plugin
#
# Module contains an implementation of SONiC FAN Base API and
# provides the FAN status which are available in the platform
#
#############################################################################

try:
    import os
    import sys
    from sonic_platform_base.fan_base import FanBase

    sys.path.append(os.path.dirname(__file__))
    from helper import ThriftConnect
    from helper import wait_for_thrift_server_setup

except ImportError as e:
    raise ImportError (str(e) + "- required module not found")


class FanUtil(FanBase):
    """Platform-specific FanUtil class"""

    def __init__(self):
        FanBase.__init__(self)
        wait_for_thrift_server_setup()
        self.index_to_fan_mapping = {
            1: 'FAN 1',
            2: 'FAN 2',
            3: 'FAN 3',
            4: 'FAN 4',
        }


    def get_num_fan(self):
        return len(self.index_to_fan_mapping)

    def get_fan_info_dict(self, index):
        if index is None:
            return False
        with ThriftConnect() as thrift_client:
            fan = thrift_client.pltfm_mgr_fan_info_get(index)
        fan_info_dict = {}
        fan_info_dict['status'] = 'false' if fan.front_rpm == 0 else \
                                    'true' if fan.front_rpm > 0 else \
                                    'N/A'
        fan_info_dict['presence'] = 'false' if fan.front_rpm == 0 else \
                                    'true' if fan.front_rpm > 0 else \
                                    'true'
        fan_info_dict['direction'] = 'no fan' if fan.front_rpm == 0 else \
                                     'exhaust' if fan.front_rpm > 0 else \
                                     'intake'
        fan_info_dict['speed'] = str(fan.front_rpm)
        return fan_info_dict
