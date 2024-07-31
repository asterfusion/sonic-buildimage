#!/usr/bin/env python

#############################################################################
# Asterfusion CX308P-48Y-T Fan Plugin
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
    from helper import get_board_type
    from helper import wait_for_thrift_server_setup

except ImportError as e:
    raise ImportError (str(e) + "- required module not found")


class FanUtil(FanBase):
    """Platform-specific FanUtil class"""

    def __init__(self):
        FanBase.__init__(self)
        wait_for_thrift_server_setup()
        board_type = get_board_type()
        if board_type in (0x3080, 0x3081,):
            self.index_to_fan_mapping = {
                1: 'FAN 1F',
                2: 'FAN 1R',
                3: 'FAN 2F',
                4: 'FAN 2R',
                5: 'FAN 3F',
                6: 'FAN 3R',
                7: 'FAN 4F',
                8: 'FAN 4R',
                9: 'FAN 5F',
                10: 'FAN 5R',
                11: 'FAN 6F',
                12: 'FAN 6R',
            }
        else:
            self.index_to_fan_mapping = {
                1: 'FAN 1F',
                2: 'FAN 1R',
                3: 'FAN 2F',
                4: 'FAN 2R',
                5: 'FAN 3F',
                6: 'FAN 3R',
                7: 'FAN 4F',
                8: 'FAN 4R',
                9: 'FAN 5F',
                10: 'FAN 5R',
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
