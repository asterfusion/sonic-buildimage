#!/usr/bin/env python
#
# main.py
#
# Command-line utility for interacting with environment sensors within SONiC
#
from __future__ import print_function

try:
    import importlib
    import sys
    import os
    import click

    from pltfm_mgr_rpc.ttypes import *
    from pltfm_pm_rpc.ttypes import *

    from thrift.transport import TSocket
    from thrift.transport import TTransport
    from thrift.protocol import TBinaryProtocol
    from thrift.protocol import TMultiplexedProtocol

    from pltfm_mgr_rpc.pltfm_mgr_rpc import wait_for_thrift_server
except ImportError as e:
    raise ImportError("%s - required module not found" % str(e))

VERSION = '1.0'
thrift_server = '127.0.0.1'
transport = None
pltfm_mgr = None
pltfm_pm = None

def thrift_setup():
    global thrift_server, transport, pltfm_mgr
    transport = TSocket.TSocket(thrift_server, 9090)

    transport = TTransport.TBufferedTransport(transport)
    bprotocol = TBinaryProtocol.TBinaryProtocol(transport)

    pltfm_mgr_client_module = importlib.import_module(".".join(["pltfm_mgr_rpc", "pltfm_mgr_rpc"]))
    pltfm_pm_client_module = importlib.import_module(".".join(["pltfm_pm_rpc", "pltfm_pm_rpc"]))

    pltfm_mgr_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "pltfm_mgr_rpc")
    pltfm_pm_protocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, "pltfm_pm_rpc")

    pltfm_mgr = pltfm_mgr_client_module.Client(pltfm_mgr_protocol)
    pltfm_pm = pltfm_pm_client_module.Client(pltfm_pm_protocol)

    transport.open()

def thrift_teardown():
    global transport
    transport.close()

def get_psu_status():

    wait_for_thrift_server()

    global pltfm_mgr

    psu_status = [0, 0]
    psu_power = [0, 0]

    try:
       thrift_setup()
       psu_status[0] = pltfm_mgr.pltfm_mgr_pwr_supply_present_get(1)
       power_data = pltfm_mgr.pltfm_mgr_pwr_supply_info_get(1)
       psu_power[0] = power_data.pwr_out / 1000
       psu_status[1] = pltfm_mgr.pltfm_mgr_pwr_supply_present_get(2)
       power_data = pltfm_mgr.pltfm_mgr_pwr_supply_info_get(2)
       psu_power[1] = power_data.pwr_out / 1000
       thrift_teardown()
    except:
        print("Could not setup thrift ! Please check the bsp thrift status !")
        return False

    print("Power Supply Unit")

    try:
        print("PSU 1:")
        print("    Status:   Normal" if psu_status[0] else \
              "    Status:   No Psu")
        print("    Power:    %d W\n" % psu_power[0])

        print("PSU 2:")
        print("    Status:   Normal" if psu_status[1] else \
              "    Status:   No Psu")
        print("    Power:    %d W\n" % psu_power[1])
    except:
        pass

def get_core_voltage():

    wait_for_thrift_server()

    global pltfm_mgr

    try:
       thrift_setup()
       data = pltfm_mgr.pltfm_mgr_pwr_rail_info_get(1)
       thrift_teardown()
    except:
        print("Could not setup thrift ! Please check the bsp thrift status !")
        return False

    print("Voltage")

    try:
        print("Barefoot Core Voltage      : %.2f V\n" % (float(data.vrail1)/1000))
        print("Barefoot AVDD 0.9V Voltage : %.2f V\n" % (float(data.vrail2)/1000))
        print("Payload 12V Voltage        : %.2f V\n" % (float(data.vrail3)/1000))
        print("Payload 3.3V Voltage       : %.2f V\n" % (float(data.vrail4)/1000))
        print("Payload 5V Voltage         : %.2f V\n" % (float(data.vrail5)/1000))
        print("Payload 2.5V Voltage       : %.2f V\n" % (float(data.vrail6)/1000))
        print("88E6131 1.9V Voltage       : %.2f V\n" % (float(data.vrail7)/1000))
        print("88E6131 1.2V Voltage       : %.2f V\n" % (float(data.vrail8)/1000))
    except:
        pass

def get_fan_info():

    wait_for_thrift_server()

    try:
        thrift_setup()
        fan_data = []
        for fan_index in range(0, 10):
            fan_data.append(pltfm_mgr.pltfm_mgr_fan_info_get(fan_index+1))
        thrift_teardown()
    except:
        print("Could not setup thrift ! Please check the bsp thrift status !")
        return False

    print("Fan Unit")

    try:
        print("FAN 1F:")
        print("    Status:   Normal" if fan_data[0].front_rpm > 0 else \
              "    Status:   No Fan")
        print("    Speed:    %d r/min\n" % fan_data[0].front_rpm)

        print("FAN 1R:")
        print("    Status:   Normal" if fan_data[1].front_rpm > 0 else \
              "    Status:   No Fan")
        print("    Speed:    %d r/min\n" % fan_data[1].front_rpm)

        print("FAN 2F:")
        print("    Status:   Normal" if fan_data[2].front_rpm > 0 else \
              "    Status:   No Fan")
        print("    Speed:    %d r/min\n" % fan_data[2].front_rpm)

        print("FAN 2R:")
        print("    Status:   Normal" if fan_data[3].front_rpm > 0 else \
              "    Status:   No Fan")
        print("    Speed:    %d r/min\n" % fan_data[3].front_rpm)

        print("FAN 3F:")
        print("    Status:   Normal" if fan_data[4].front_rpm > 0 else \
              "    Status:   No Fan")
        print("    Speed:    %d r/min\n" % fan_data[4].front_rpm)

        print("FAN 3R:")
        print("    Status:   Normal" if fan_data[5].front_rpm > 0 else \
              "    Status:   No Fan")
        print("    Speed:    %d r/min\n" % fan_data[5].front_rpm)

        print("FAN 4F:")
        print("    Status:   Normal" if fan_data[6].front_rpm > 0 else \
              "    Status:   No Fan")
        print("    Speed:    %d r/min\n" % fan_data[6].front_rpm)

        print("FAN 4R:")
        print("    Status:   Normal" if fan_data[7].front_rpm > 0 else \
              "    Status:   No Fan")
        print("    Speed:    %d r/min\n" % fan_data[7].front_rpm)

        print("FAN 5F:")
        print("    Status:   Normal" if fan_data[8].front_rpm > 0 else \
              "    Status:   No Fan")
        print("    Speed:    %d r/min\n" % fan_data[8].front_rpm)

        print("FAN 5R:")
        print("    Status:   Normal" if fan_data[9].front_rpm > 0 else \
              "    Status:   No Fan")
        print("    Speed:    %d r/min\n" % fan_data[9].front_rpm)
    except:
        pass

def get_temps_and_alarm():

    wait_for_thrift_server()

    global pltfm_mgr

    try:
       thrift_setup()
       data = pltfm_mgr.pltfm_mgr_sys_tmp_get()
       thrift_teardown()
    except:
        print("Could not setup thrift ! Please check the bsp thrift status !")
        return False

    try:
        print("Mainboard Front Left")
        print("Adapter: cgos adapter")
        print("M/B:          +%.1f C\n" % (float(data.tmp1)))

        print("Mainboard Front Right")
        print("Adapter: cgos adapter")
        print("M/B:          +%.1f C\n" % (float(data.tmp2)))

        print("Barefoot Ambient")
        print("Adapter: cgos adapter")
        print("M/B:          +%.1f C\n" % (float(data.tmp3)))

        print("Barefoot Junction")
        print("Adapter: cgos adapter")
        print("M/B:          +%.1f C\n" % (float(data.tmp4)))

        print("Fan 1")
        print("Adapter: cgos adapter")
        print("M/B:          +%.1f C\n" % (float(data.tmp5)))

        print("Fan 2")
        print("Adapter: cgos adapter")
        print("M/B:          +%.1f C\n" % (float(data.tmp6)))
    except:
        pass


# ==================== CLI commands and groups ====================

# This is our main entrypoint - the main 'environment' command
@click.group()
def cli():
    """environment - Command line utility for power voltage fans temps read set"""

    if os.geteuid() !=0:
        print("Root privileges are required for this operation")
        sys.exit(1)

# 'show' subgroup
@cli.group()
def show():
    """Display status of platform environment"""
    pass

# 'environment' subcommand
@show.command()
def powers():
    """Display Platform environment powers"""
    get_psu_status()

@show.command()
def voltages():
    """Display Platform environment voltages"""
    get_core_voltage()

@show.command()
def fans():
    """Display Platform environment fans"""
    get_fan_info()

@show.command()
def temps():
    """Display Platform environment temps"""
    get_temps_and_alarm()

# 'version' subcommand
@cli.command()
def version():
    """Display version info"""
    click.echo("environment version {0}".format(VERSION))

if __name__ == '__main__':
    cli()

