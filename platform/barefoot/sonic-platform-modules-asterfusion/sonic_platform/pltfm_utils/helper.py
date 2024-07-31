####################################################################
# Asterfusion CX-T Devices Chassis Helper API                      #
#                                                                  #
# Module contains an implementation of SONiC Platform Base API and #
# provides the helper api                                          #
#                                                                  #
####################################################################

try:
    import itertools
    import re
    import socket
    import subprocess
    import sys
    import time

    from functools import lru_cache
    from subprocess import Popen
    from typing import Literal

    from .constants import *
    from .logger import Logger
    from .thrift import ThriftClient

    from .pltfm_mgr_rpc.pltfm_mgr_rpc import Client as MgrClient
    from .pltfm_pm_rpc.pltfm_pm_rpc import Client as PmClient

    from sonic_py_common.device_info import get_hwsku
    from sonic_py_common.device_info import get_platform
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


class APIHelper(Logger):
    def __init__(self):
        # type: () -> None
        super().__init__()
        self.thrift_client = ThriftClient
        self._wait_for_thrift_server_setup()

    def _wait_for_thrift_server_setup(self):
        # type: () -> None
        connectable = False
        invokable = False
        retries = 0
        while True:
            timediff = time.time()
            retries += 1
            if retries <= 1:
                self.log_debug(
                    "Trying to connect to thrift server({}:{})...".format(
                        THRIFT_RPC_HOST, THRIFT_RPC_PORT
                    )
                )
            else:
                self.log_debug(
                    "Retrying to connect to thrift server({} out of {})...".format(
                        retries, THRIFT_RETRY_TIMES
                    )
                )
            # First check if host is connectable
            if not connectable and retries < THRIFT_RETRY_TIMES:
                connector = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                try:
                    connector.connect((THRIFT_RPC_HOST, 9095))
                    connector.shutdown(2)
                    connectable = True
                except Exception as err:
                    self.log_debug("No response from thrift server yet...")
            # Second check if server API is invokable
            if connectable and not invokable and retries < THRIFT_RETRY_TIMES:
                try:
                    with self.thrift_client() as client:
                        client.pltfm_mgr_dummy(THRIFT_DUMMY_DEVID)
                    invokable = True
                except Exception as err:
                    self.log_debug("Thrift server has not been setup yet...")
            if connectable and invokable or retries >= THRIFT_RETRY_TIMES:
                break
            time.sleep(max(0, THRIFT_WAIT_TIMEOUT + timediff - time.time()))
        if not connectable or not invokable:
            self.log_error("Failed in connecting to thrift server:( Exiting...")
            sys.exit(1)
        self.log_debug("Succeeded in connecting to thrift server")

    def get_device_info_by_board_type(self):
        # type: () -> tuple[int, str, str, int, int, int, int, int, int]
        # BD ID(Board Type), Device, Platform, ...
        bdid = self.get_board_type()
        # fmt: off
        device = (
            DEVICE_X308PT
            if bdid in HWSKU_X308PT
            else DEVICE_X312PT
            if bdid in HWSKU_X312PT
            else DEVICE_X532PT
            if bdid in HWSKU_X532PT
            else DEVICE_X564PT
            if bdid in HWSKU_X564PT
            else DEVICE_X732QT
            if bdid in HWSKU_X732QT
            else None
        )
        platform = (
            PLTFM_X308PT
            if bdid in HWSKU_X308PT
            else PLTFM_X312PT
            if bdid in HWSKU_X312PT
            else PLTFM_X532PT
            if bdid in HWSKU_X532PT
            else PLTFM_X564PT
            if bdid in HWSKU_X564PT
            else PLTFM_X732QT
            if bdid in HWSKU_X732QT
            else None
        )
        # fmt: on
        assert device is not None
        assert device == get_hwsku()
        assert platform is not None
        assert platform == get_platform()
        # Component
        component = COMPONENT.get(device)
        assert component is not None
        num_component = component.get("NUM")
        # Peripheral
        peripheral = PERIPHERAL.get(bdid)
        assert peripheral is not None
        # Fan
        fan = peripheral.get("FAN")
        assert fan is not None
        num_fan_drawer = fan.get("DRAWER")
        num_fan_per_drawer = fan.get("NUM")
        # Psu
        psu = peripheral.get("PSU")
        assert psu is not None
        num_psu = psu.get("NUM")
        # Sfp
        sfp = peripheral.get("SFP")
        assert sfp is not None
        num_sfp = sfp.get("NUM")
        # Thermal
        thermal = peripheral.get("THERMAL")
        assert thermal is not None
        num_thermal = thermal.get("NUM")
        num_core_x86_thermal = self.get_x86_thermal_num()
        num_thermal += num_core_x86_thermal
        return (
            bdid,
            device,
            platform,
            num_component,
            num_fan_drawer,
            num_fan_per_drawer,
            num_psu,
            num_sfp,
            num_thermal,
        )

    def get_board_type(self):
        # type: () -> int
        with self.thrift_client(rpc_type="pltfm_pm_rpc", rpc_client=PmClient) as client:
            return client.pltfm_pm_board_type_get()

    @lru_cache(maxsize=16)
    def get_x86_thermal_info(self, _=None):
        # type: (int | None) -> dict[str, dict[str, float]]
        if self.check_if_host():
            sensor_result = self.get_pmon_result(
                PMON_SENSORS_CMD, round(time.time() / LRU_CACHE_TTL)
            ).splitlines()
        else:
            sensor_result = self.get_cmd_result(
                PMON_SENSORS_CMD, round(time.time() / LRU_CACHE_TTL)
            ).splitlines()
        filter_pattern = r"\+?-?[0-9]+\.[0-9]*|\+?-?[0-9]+ C"
        filtered_result = tuple(
            filter(
                lambda output: re.search(filter_pattern, output) is not None,
                sensor_result,
            )
        )
        keys = tuple(
            map(
                lambda output: tuple(
                    itertools.chain(["temp"], re.findall(r"high|crit", output))
                ),
                filtered_result,
            )
        )
        values = tuple(
            map(
                lambda output: tuple(
                    map(lambda value: float(value), re.findall(filter_pattern, output))
                ),
                filtered_result,
            )
        )
        assert len(keys) == len(values), "unmatched thermal keys and values num"
        thermal_names = list(
            map(lambda output: output.split(": ")[0].title(), filtered_result)
        )
        thermal_values = (
            dict(zip(keys[index], values[index])) for index in range(0, len(keys))
        )
        for index in range(0, len(thermal_names)):
            if thermal_names[index].lower() == "temp1":
                thermal_names[index] = "CPU Package"
        return dict(zip(thermal_names, thermal_values))

    def get_x86_thermal_num(self):
        # type: () -> int
        thermal_info_dict = self.get_x86_thermal_info(
            round(time.time() / LRU_CACHE_TTL)
        )
        return len(thermal_info_dict)

    def get_x86_thermal_names(self):
        # type: () -> list[str]
        thermal_info_dict = self.get_x86_thermal_info(
            round(time.time() / LRU_CACHE_TTL)
        )
        return list(thermal_info_dict.keys())

    def get_x86_thermal_values(self):
        # type: () -> list[dict[str, float]]
        thermal_info_dict = self.get_x86_thermal_info(
            round(time.time() / LRU_CACHE_TTL)
        )
        return list(thermal_info_dict.values())

    def get_component_version(self, exec_type, exec_target):
        # type: (str, str | tuple[str, ...]) -> str | None
        timestamp = time.time()
        if exec_type == "cmd":
            return self.get_cmd_result(exec_target, round(timestamp / LRU_CACHE_TTL))
        if exec_type == "file":
            return self.get_file_content(exec_target, round(timestamp / LRU_CACHE_TTL))
        if exec_type == "mgr" or exec_type == "pm":
            return self.get_thrift_result(
                exec_type, exec_target, round(timestamp / LRU_CACHE_TTL)
            )

    @lru_cache(maxsize=16)
    def get_pmon_result(self, cmd, _=None):
        # type: (tuple[str, ...], int | None) -> str
        command = " ".join(cmd)
        try:
            import docker

            client = docker.from_env()
            pmon = client.containers.get("pmon")
            if pmon.status != "running":
                self.log_error(
                    "Failed in running command '{}' in pmon since it's not running".format(
                        command
                    )
                )
                return ""
            result = pmon.exec_run(cmd)
            output = result.output.decode().strip()
            if result.exit_code != 0:
                self.log_error(
                    "Failed in running command '{}' in pmon due to {}".format(
                        command, output
                    )
                )
                return ""
            return output
        except ImportError as err:
            self.log_error(
                "Failed in running command '{}' in pmon due to {}".format(command, err)
            )
            return self.get_cmd_result(cmd, _)

    @lru_cache(maxsize=16)
    def get_cmd_result(self, cmd, _=None):
        # type: (tuple[str, ...], int | None) -> str
        try:
            process = Popen(cmd, universal_newlines=True, stdout=subprocess.PIPE)
            output, error = process.communicate()
            return output.strip()
        except Exception as err:
            self.log_error(
                "Failed in running command '{}' in pmon due to {}".format(
                    " ".join(cmd), err
                )
            )
            return ""

    @lru_cache(maxsize=16)
    def get_file_content(self, path, _=None):
        # type: (str, int | None) -> str
        with open(path) as file:
            return file.read().strip()

    @lru_cache(maxsize=16)
    def get_thrift_result(self, exec_type, exec_target, _=None):
        # type: (str, str | tuple[str, ...], int | None) -> str | None
        client_class = MgrClient if exec_type == "mgr" else PmClient
        client_type = "pltfm_mgr_rpc" if exec_type == "mgr" else "pltfm_pm_rpc"
        with self.thrift_client(
            rpc_type=client_type, rpc_client=client_class
        ) as target:
            for attr in exec_target:
                target = getattr(target, attr)
                if callable(target):
                    target = target()
            return target

    def validate_mac_addr(self, mac_addr):
        # type: (str) -> Literal[True]
        assert bool(
            re.match("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$", mac_addr)
        ), "invalid mac address"
        return True

    @lru_cache(maxsize=16)
    def check_if_host(self):
        # type: () -> bool
        # A process cannot be both in host and docker container.
        # Thus caching the result to accelerate this method.
        try:
            subprocess.call(
                HOST_CHK_CMD, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
            )
            return True  # type: bool
        except FileNotFoundError as err:
            return False
