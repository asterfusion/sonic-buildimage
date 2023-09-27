#!/usr/bin/env python

#############################################################################
# Asterfusion CX532P-T SFP Plugin
#
# Module contains an implementation of SONiC SFP Base API and
# provides the SFPs status which are available in the platform
#
#############################################################################

try:
    import errno
    import os
    import sys
    import time
    from sonic_sfp.sfputilbase import SfpUtilBase

    sys.path.append(os.path.dirname(__file__))
    from helper import ThriftConnect
    from helper import wait_for_thrift_server_setup

except ImportError as e:
    raise ImportError(str(e) + "- required module not found")


SFP_EEPROM_CACHE = "/var/run/platform/sfp/cache"


class SfpUtil(SfpUtilBase):
    """Platform-specific SfpUtil class"""

    PORT_START = 0
    PORT_END = 33
    PORTS_IN_BLOCK = 34
    QSFP_PORT_START = 0
    QSFP_PORT_END = 32
    EEPROM_OFFSET = 0
    QSFP_CHECK_INTERVAL = 4
    THRIFT_RETRIES = 5
    THRIFT_TIMEOUT = 5

    @property
    def port_start(self):
        self.update_port_info()
        return self.PORT_START

    @property
    def port_end(self):
        self.update_port_info()
        return self.PORT_END

    @property
    def qsfp_ports(self):
        self.update_port_info()
        return list(range(self.QSFP_PORT_START, self.QSFP_PORT_END))

    @property
    def port_to_eeprom_mapping(self):
        print("dependency on sysfs has been removed")
        raise Exception()

    def __init__(self):
        self.ready = False
        self.phy_port_dict = {'-1': 'system_not_ready'}
        self.phy_port_cur_state = {}
        self.qsfp_interval = self.QSFP_CHECK_INTERVAL
        if not os.path.exists(os.path.dirname(SFP_EEPROM_CACHE)):
            try:
                os.makedirs(os.path.dirname(SFP_EEPROM_CACHE))
            except OSError as e:
                if e.errno != errno.EEXIST:
                    raise
        open(SFP_EEPROM_CACHE, 'ab').close()
        SfpUtilBase.__init__(self)
        wait_for_thrift_server_setup()

    def update_port_info(self):
        if self.QSFP_PORT_END == 0:
            try:
                with ThriftConnect() as thrift_client:
                    self.QSFP_PORT_END = thrift_client.pltfm_mgr_qsfp_get_max_port()
                self.PORT_END = self.QSFP_PORT_END
                self.PORTS_IN_BLOCK = self.QSFP_PORT_END
            except Exception as e:
                print(e.__doc__)
                print(e.message)

    def get_presence(self, port_num):
        # Check for invalid port_num, which is 0 based index
        if port_num < self.port_start or port_num > self.port_end:
            return False
        presence = False
        with ThriftConnect() as thrift_client:
            if port_num < self.QSFP_PORT_END:
                presence = thrift_client.pltfm_mgr_qsfp_presence_get(port_num+1)
            else:
                presence = thrift_client.pltfm_mgr_sfp_presence_get(port_num + 1 - self.QSFP_PORT_END)
        return presence

    def get_eeprom(self, port_num):
        # Check for invalid port_num, which is 0 based index
        if port_num < self.port_start or port_num > self.port_end:
            return False
        eeprom_hex = bytearray(512)
        if not self.get_presence(port_num):
            return eeprom_hex
        with ThriftConnect() as thrift_client:
            if port_num < self.QSFP_PORT_END:
                eeprom_hex = thrift_client.pltfm_mgr_qsfp_info_get(port_num+1)
            else:
                eeprom_hex = thrift_client.pltfm_mgr_sfp_info_get(port_num + 1 - self.QSFP_PORT_END)
        return bytearray.fromhex(eeprom_hex)

    def get_low_power_mode(self, port_num):
        # Check for invalid port_num
        if port_num < self.QSFP_PORT_START or port_num >= self.QSFP_PORT_END:
            return False
        with ThriftConnect() as thrift_client:
            lpmode = thrift_client.pltfm_mgr_qsfp_lpmode_get(port_num + 1 - self.QSFP_PORT_START)
        return lpmode

    def set_low_power_mode(self, port_num, lpmode):
        # Check for invalid port_num
        if port_num < self.QSFP_PORT_START or port_num >= self.QSFP_PORT_END:
            return False
        with ThriftConnect() as thrift_client:
            status = thrift_client.pltfm_mgr_qsfp_lpmode_set(port_num + 1 - self.QSFP_PORT_START, lpmode)
        return (status == 0)

    def reset(self, port_num):
        # Check for invalid port_num
        if port_num < self.QSFP_PORT_START or port_num >= self.QSFP_PORT_END:
            return False
        with ThriftConnect() as thrift_client:
            status = thrift_client.pltfm_mgr_qsfp_reset(port_num + 1 - self.QSFP_PORT_START, True) | \
                     thrift_client.pltfm_mgr_qsfp_reset(port_num + 1 - self.QSFP_PORT_START, False)
        return (status == 0)

    def check_transceiver_change(self):
        if not self.ready:
            return
        self.phy_port_dict = {}
        try:
            # Get presence of each SFP
            for port in range(self.port_start, self.port_end + 1):
                sfp_state = '1' if self.get_presence(port) else '0'
                if port in self.phy_port_cur_state:
                    if self.phy_port_cur_state[port] != sfp_state:
                        self.phy_port_dict[port] = sfp_state
                else:
                    self.phy_port_dict[port] = sfp_state
                # Update port current state
                self.phy_port_cur_state[port] = sfp_state
        except:
            return

    def get_transceiver_change_event(self, timeout=0):
        forever = False
        if timeout == 0:
            forever = True
        elif timeout > 0:
            timeout = timeout / float(1000)  # Convert to secs
        else:
            print("get_transceiver_change_event:Invalid timeout value", timeout)
            return False, {}
        while forever or timeout > 0:
            if not self.ready:
                with ThriftConnect() as thrift_client:
                    pass
                self.ready = True
                self.phy_port_dict = {}
                break
            elif self.qsfp_interval == 0:
                self.qsfp_interval = self.QSFP_CHECK_INTERVAL
                # Process transceiver plug-in/out event
                self.check_transceiver_change()
                # Break if tranceiver state has changed
                if bool(self.phy_port_dict):
                    break
            if timeout:
                timeout -= 1
            if self.qsfp_interval:
                self.qsfp_interval -= 1
            time.sleep(1)
        return self.ready, self.phy_port_dict

    def _get_port_eeprom_path(self, port_num, devid):
        with open(SFP_EEPROM_CACHE, 'wb') as eeprom_cache:
            eeprom_cache.write(self.get_eeprom(port_num))
        return SFP_EEPROM_CACHE
