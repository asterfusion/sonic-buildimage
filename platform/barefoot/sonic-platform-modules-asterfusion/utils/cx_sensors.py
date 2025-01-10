#!/usr/bin/env python
#
# main.py
#
# Command-line utility for interacting with environment sensors within SONiC
#
from __future__ import print_function

try:
    import click
    import os
    import sys
    import tabulate

    from sonic_platform.platform import Platform
    from sonic_platform.pltfm_utils.constants import *
except ImportError as e:
    raise ImportError("%s - required module not found" % str(e))

UTIL_VERSION = "1.0"
NOT_AVAILABLE = "N/A"
RPM_UNIT = "r/min"
CUR_UNIT = "A"
VOL_UNIT = "V"
PWR_UNIT = "W"
TMP_UNIT = "C"


class UnsupportedPlatformException(Exception):
    pass


class Sensors(object):
    platform = Platform()
    chassis = platform.get_chassis()
    helper = chassis.get_helper()

    @property
    def all(self):
        return self.fans, self.psus, self.temperature, self.voltage

    @property
    def fans(self):
        fan_status_dict = {
            # tuple[presence, status]
            (True, True): "Normal",
            (True, False): "Stuck",
            (False, True): "Absent",
            (False, False): "Absent",
        }
        table_header = ("Fan Units", "Status", "Speed", "Direction", "Model", "Serial")
        table_data = []
        for fan in self.chassis.get_all_fans():
            fan_name = fan.get_name().title()
            fan_status = fan_status_dict[(fan.get_presence(), fan.get_status())]
            fan_speed = fan.get_speed()
            fan_direction = fan.get_direction().title()
            fan_model = fan.get_model() or NOT_AVAILABLE
            fan_serial = fan.get_serial() or NOT_AVAILABLE
            # Post processing
            if fan_speed != NOT_AVAILABLE:
                fan_speed = "{} {}".format(fan.get_speed(), RPM_UNIT)
            # Appending data
            table_data.append((fan_name, fan_status, fan_speed, fan_direction, fan_model, fan_serial))
        return tabulate.tabulate(tabular_data=table_data, headers=table_header)

    @property
    def psus(self):
        psu_status_dict = {
            # tuple[presence, status]
            (True, True): "Normal",
            (True, False): "Powerloss",
            (False, True): "Absent",
            (False, False): "Absent",
        }
        table_header = ("Power Supply Units", "Status",
                        "Current In", "Current Out",
                        "Voltage In", "Voltage Out",
                        "Power In", "Power Out",
                        "Temperature", "Model", "Serial")
        table_data = []
        for psu in self.chassis.get_all_psus():
            psu_name = psu.get_name()
            psu_status = psu_status_dict[(psu.get_presence(), psu.get_status())]
            psu_current_in = psu.get_current_in()
            psu_current_out = psu.get_current_out()
            psu_voltage_in = psu.get_voltage_in()
            psu_voltage_out = psu.get_voltage_out()
            psu_power_in = psu.get_power_in()
            psu_power_out = psu.get_power_out()
            psu_temperature = psu.get_temperature() or NOT_AVAILABLE
            psu_model = psu.get_model() or NOT_AVAILABLE
            psu_serial = psu.get_serial() or NOT_AVAILABLE
            # Post processing
            if psu_current_in != NOT_AVAILABLE:
                psu_current_in = "{} {}".format(psu.get_current_in(), CUR_UNIT)
            if psu_current_out != NOT_AVAILABLE:
                psu_current_out = "{} {}".format(psu.get_current_out(), CUR_UNIT)
            if psu_voltage_in != NOT_AVAILABLE:
                psu_voltage_in = "{} {}".format(psu.get_voltage_in(), VOL_UNIT)
            if psu_voltage_out != NOT_AVAILABLE:
                psu_voltage_out = "{} {}".format(psu.get_voltage_out(), VOL_UNIT)
            if psu_power_in != NOT_AVAILABLE:
                psu_power_in = "{} {}".format(psu.get_power_in(), PWR_UNIT)
            if psu_power_out != NOT_AVAILABLE:
                psu_power_out = "{} {}".format(psu.get_power_out(), PWR_UNIT)
            if psu_temperature != NOT_AVAILABLE:
                psu_temperature = "{} {}".format(psu.get_temperature(), TMP_UNIT)
            # Appending data
            table_data.append((psu_name, psu_status,
                               psu_current_in, psu_current_out,
                               psu_voltage_in, psu_voltage_out,
                               psu_power_in, psu_power_out,
                               psu_temperature, psu_model, psu_serial))
        return tabulate.tabulate(tabular_data=table_data, headers=table_header)

    @property
    def temperature(self):
        thermal_status_dict = {
            # tuple[status, warning]
            (True, False): "Normal",
            (True, True): "Overheat",
            (False, False): "Abormal",
            (False, True): "Abormal",
        }
        table_header = ("Thermal Sensors", "Status", "Temperature",
                        "High Threshold", "Critical High Threshold",
                        "Low Threshold", "Critical Low Threshold")
        table_data = []
        for thermal in self.chassis.get_all_thermals():
            thermal_name = thermal.get_name()
            thermal_status = thermal_status_dict[
                (thermal.get_status(), thermal.get_warning())
            ]
            thermal_temperature = thermal.get_temperature()
            if thermal_temperature == NOT_AVAILABLE: continue
            thermal_temperature = "{} {}".format(thermal_temperature, TMP_UNIT)
            thermal_temperature_high_threshold = thermal.get_high_threshold()
            thermal_temperature_critical_high_threshold = thermal.get_high_critical_threshold()
            thermal_temperature_low_threshold = thermal.get_low_threshold()
            thermal_temperature_critical_low_threshold = thermal.get_low_critical_threshold()
            # Post processing
            if thermal_temperature_high_threshold != NOT_AVAILABLE:
                thermal_temperature_high_threshold = "{} {}".format(thermal.get_high_threshold(), TMP_UNIT)
            if thermal_temperature_critical_high_threshold != NOT_AVAILABLE:
                thermal_temperature_critical_high_threshold = "{} {}".format(thermal.get_high_critical_threshold(), TMP_UNIT)
            if thermal_temperature_low_threshold != NOT_AVAILABLE:
                thermal_temperature_low_threshold = "{} {}".format(thermal.get_low_threshold(), TMP_UNIT)
            if thermal_temperature_critical_low_threshold != NOT_AVAILABLE:
                thermal_temperature_critical_low_threshold = "{} {}".format(thermal.get_low_critical_threshold(), TMP_UNIT)
            # Appending data
            table_data.append((thermal_name, thermal_status, thermal_temperature,
                               thermal_temperature_high_threshold, thermal_temperature_critical_high_threshold,
                               thermal_temperature_low_threshold, thermal_temperature_critical_low_threshold))
        return tabulate.tabulate(tabular_data=table_data, headers=table_header)

    @property
    def voltage(self):
        device_platform = self.chassis.get_name()
        if device_platform in (
            DEVICE_X308PT,
            DEVICE_X532PT,
            DEVICE_X564PT,
        ):
            vrail_name_dict = {
                "1": "Barefoot Core 0.9V",
                "2": "Barefoot AVDD 0.9V",
                "3": "Payload 12.0V",
                "4": "Payload 3.3V",
                "5": "Payload 5.0V",
                "6": "Payload 2.5V",
                "7": "88E6131 1.9V",
                "8": "88E6131 1.2V",
            }
        elif device_platform in (
            DEVICE_X732QT,
        ):
            vrail_name_dict = {
                "1": "Barefoot VDDA 1.8V",
                "2": "Barefoot VDDA 1.7V",
                "3": "Payload 12.0V",
                "4": "Payload 3.3V",
                "5": "Payload 5.0V",
                "6": "Barefoot VDD 1.8V",
                "7": "Barefoot Core",
                "8": "Barefoot VDDT 0.86V",
            }
        elif device_platform in (
            DEVICE_X312PT,
        ):
            vrail_name_dict = {
                "1": "Barefoot Core 0.9V",
                "2": "Barefoot AVDD 0.9V",
                "6": "Payload 2.5V",
            }
        else:
            raise UnsupportedPlatformException("unknown platform: {}".format(device_platform))
        table_header = ("Voltage Rails", "Voltage")
        table_data = []
        with self.helper.thrift_client() as thrift_client:
            voltage_data = thrift_client.pltfm_mgr_pwr_rail_info_get(0)
        for index, vrail_name in vrail_name_dict.items():
            vrail_voltage = getattr(voltage_data, "vrail{}".format(index)) / 1000
            if vrail_voltage != NOT_AVAILABLE:
                vrail_voltage = "{} {}".format(vrail_voltage, VOL_UNIT)
            table_data.append((vrail_name, vrail_voltage))
        return tabulate.tabulate(tabular_data=table_data, headers=table_header)


# ==================== CLI commands and groups ====================


# This is our main entrypoint - the main 'environment' command
@click.group()
def cli():
    """environment - Command line utility for fans, power, temperature, voltage reading"""
    pass


# 'version' subcommand
@cli.command()
def version():
    """Display version info"""
    click.echo("environment version {0}".format(UTIL_VERSION))


# 'show' subgroup
@cli.group()
@click.pass_context
def show(ctx):
    # type: (click.Context) -> None
    """Display status of platform environment"""

    if os.geteuid() != 0:
        print("Root privileges are required for this operation")
        sys.exit(1)

    ctx.ensure_object(Sensors)


# 'environment' subcommand
@show.command()
@click.pass_context
def all(ctx):
    # type: (click.Context) -> None
    """Display Platform environment data"""
    sensors = ctx.obj # type: Sensors
    all_sensors = sensors.all
    splitter = "=" * max(map(len, "\n".join(all_sensors).splitlines()))
    click.echo(("\n{}\n".format(splitter)).join(all_sensors))

@show.command()
@click.pass_context
def fans(ctx):
    # type: (click.Context) -> None
    """Display Platform environment fans"""
    sensors = ctx.obj # type: Sensors
    click.echo(sensors.fans)

@show.command()
@click.pass_context
def power(ctx):
    # type: (click.Context) -> None
    """Display Platform environment power"""
    sensors = ctx.obj # type: Sensors
    click.echo(sensors.psus)

@show.command()
@click.pass_context
def temperature(ctx):
    # type: (click.Context) -> None
    """Display Platform environment temperature"""
    sensors = ctx.obj # type: Sensors
    click.echo(sensors.temperature)

@show.command()
@click.pass_context
def voltage(ctx):
    # type: (click.Context) -> None
    """Display Platform environment voltage"""
    sensors = ctx.obj # type: Sensors
    click.echo(sensors.voltage)

if __name__ == "__main__":
    cli()
