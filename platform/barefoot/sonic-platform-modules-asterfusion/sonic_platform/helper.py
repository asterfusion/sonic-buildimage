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

    from .constants import *
    from .pltfm_mgr_rpc.pltfm_mgr_rpc import Client as MgrClient
    from .pltfm_pm_rpc.pltfm_pm_rpc import Client as PmClient

    from sonic_py_common.logger import Logger
    from thrift.transport import TSocket
    from thrift.transport import TTransport
    from thrift.protocol import TBinaryProtocol
    from thrift.protocol import TMultiplexedProtocol

    from typing import Literal
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")

THRIFT_SERVER = "127.0.0.1"
THRIFT_PORT = 9090
THRIFT_RETRIES_TIMES = 10
THRIFT_RETRY_TIMEOUT = 15
THRIFT_WAIT_TIMEOUT = 15
THRIFT_DUMMY_DEVID = 0


class APIHelper(object):
    def __init__(self):
        # type: () -> None
        self._logger = Logger()
        self.wait_for_thrift_server_setup()

    def get_device_info_by_board_type(self):
        # type: () -> tuple[int, str, str, int, int, int, int, int, int]
        # HWVER(Board Type), HWSKU, Platform
        hwver = self.get_board_type()
        hwsku = (
            HWSKU_X308PT
            if hwver in BOARD_X308PT
            else HWSKU_X312PT
            if hwver in BOARD_X312PT
            else HWSKU_X532PT
            if hwver in BOARD_X532PT
            else HWSKU_X564PT
            if hwver in BOARD_X564PT
            else None
        )
        platform = (
            PLTFM_X308PT
            if hwver in BOARD_X308PT
            else PLTFM_X312PT
            if hwver in BOARD_X312PT
            else PLTFM_X532PT
            if hwver in BOARD_X532PT
            else PLTFM_X564PT
            if hwver in BOARD_X564PT
            else None
        )
        assert hwsku is not None
        assert platform is not None
        # Component
        component = COMPONENT.get(hwsku)
        assert component is not None
        num_component = component.get("NUM")
        # Peripheral
        peripheral = PERIPHERAL.get(hwver)
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
            hwver,
            hwsku,
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
        with self.thrift_connect(type="pltfm_pm_rpc", client=PmClient) as client:
            return client.pltfm_pm_board_type_get()

    def get_x86_thermal_num(self):
        # type: () -> int
        if self.check_if_host():
            sensor_result = self.get_cmd_output(HOST_TEMP_GET_CMD).split("\n")
        else:
            sensor_result = self.get_cmd_output(PMON_TEMP_GET_CMD).split("\n")
        num = len(
            list(
                filter(
                    lambda output: re.search(
                        r"\+?-?[0-9]+\.[0-9]*|\+?-?[0-9]+ C", output
                    )
                    is not None,
                    sensor_result,
                )
            )
        )
        return num

    def get_x86_thermal_names(self):
        # type: () -> list[str]
        if self.check_if_host():
            sensor_result = self.get_cmd_output(HOST_TEMP_GET_CMD).split("\n")
        else:
            sensor_result = self.get_cmd_output(PMON_TEMP_GET_CMD).split("\n")
        names = list(
            map(
                lambda output: output.split(": ")[0].title(),
                filter(
                    lambda output: re.search(
                        r"\+?-?[0-9]+\.[0-9]*|\+?-?[0-9]+ C", output
                    )
                    is not None,
                    sensor_result,
                ),
            )
        )
        for index in range(0, len(names)):
            if names[index].lower() == "temp1":
                names[index] = "CPU 0"
        return names

    def get_x86_thermal_values(self):
        # type: () -> list[dict[str, int]]
        if self.check_if_host():
            sensor_result = self.get_cmd_output(HOST_TEMP_GET_CMD).split("\n")
        else:
            sensor_result = self.get_cmd_output(PMON_TEMP_GET_CMD).split("\n")
        filtered_result = list(
            filter(
                lambda output: re.search(r"\+?-?[0-9]+\.[0-9]*|\+?-?[0-9]+ C", output)
                is not None,
                sensor_result,
            )
        )
        keys = list(
            map(
                lambda output: list(
                    itertools.chain(["temp"], re.findall(r"high|crit", output))
                ),
                filtered_result,
            )
        )
        values = list(
            map(
                lambda output: list(
                    map(
                        lambda value: float(value),
                        re.findall(r"\+?-?[0-9]+\.[0-9]*|\+?-?[0-9]+ C", output),
                    )
                ),
                filtered_result,
            )
        )
        assert len(keys) == len(values), "unmatched thermal keys and values num"
        result = []
        for index in range(0, len(keys)):
            result.append(dict(zip(keys[index], values[index])))
        return result

    def get_component_version(self, exec_type, exec_target):
        # type: (str, tuple[str]) -> str | None
        if exec_type == "cmd":
            return self.get_cmd_output(exec_target, trigger_time=-1)
        elif exec_type == "file":
            return self.get_file_content(exec_target, trigger_time=-1)
        elif exec_type == "mgr" or exec_type == "pm":
            client_class = MgrClient if exec_type == "mgr" else PmClient
            client_type = "pltfm_mgr_rpc" if exec_type == "mgr" else "pltfm_pm_rpc"
            with self.thrift_connect(type=client_type, client=client_class) as target:
                for attr in exec_target:
                    target = getattr(target, attr)
                    if callable(target):
                        target = target()
            return target

    # Create a global dict to store cmd result or file content.
    # Using debounce algorithm, min trigger time is 5s.
    # If trigger time set to -1, never re-execute again.
    def get_cmd_output(self, cmd, trigger_time=5):
        # type: (str | tuple[str, ...], float) -> str
        global __RESULT__
        try:
            __RESULT__
        except Exception as err:
            __RESULT__ = (
                {}
            )  # type: dict[str | tuple[str, ...], dict[str, str | float]]
        if __RESULT__.get(cmd) is None:
            __RESULT__[cmd] = {
                "result": subprocess.check_output(cmd).decode().strip(),
                "timestamp": time.time(),
            }
        if trigger_time == -1:
            return __RESULT__[cmd].get("result", None)
        if (
            time.time() - __RESULT__.get(cmd).get("timestamp", time.time())
            >= trigger_time
        ):
            __RESULT__[cmd]["result"] = (
                subprocess.check_output(cmd).decode().strip()
            )
            __RESULT__[cmd]["timestamp"] = time.time()
        return __RESULT__[cmd].get("result")

    def get_file_content(self, path, trigger_time=5):
        # type: (str, int) -> str
        global __RESULT__
        try:
            __RESULT__
        except Exception as err:
            __RESULT__ = (
                {}
            )  # type: dict[str | tuple[str, ...], dict[str, str | float]]
        if __RESULT__.get(path) is None:
            with open(path, "r") as file:
                content = file.read().strip()
            __RESULT__[path] = {
                "result": content,
                "timestamp": time.time(),
            }
        if trigger_time == -1:
            return __RESULT__[path].get("result", None)
        if (
            time.time() - __RESULT__.get(path).get("timestamp", time.time())
            >= trigger_time
        ):
            with open(path, "r") as file:
                content = file.read().strip()
            __RESULT__[path]["result"] = content
            __RESULT__[path]["timestamp"] = time.time()
        return __RESULT__[path].get("result")

    def validate_mac_addr(self, mac_addr):
        # type: (str) -> Literal[True]
        assert bool(
            re.match("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$", mac_addr)
        ), "invalid mac address"
        return True

    def check_if_host(self):
        # type: () -> bool
        # A process cannot be both in host and docker container.
        # Thus using a global var to indicate in where the process is.
        global __IS_HOST__
        try:
            return __IS_HOST__
        except Exception as err:
            try:
                subprocess.call(
                    HOST_CHK_CMD, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
                )
                __IS_HOST__ = True  # type: bool
            except FileNotFoundError:
                __IS_HOST__ = False
        return __IS_HOST__

    def wait_for_thrift_server_setup(self):
        # type: () -> None
        connectable = False
        invokable = False
        retries = 0
        while retries < THRIFT_RETRIES_TIMES:
            self.log_info("(Re)trying to connect to thrift server...\n")
            # First check if host is connectable
            while not connectable:
                connector = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                try:
                    connector.connect(("127.0.0.1", 9095))
                    connector.shutdown(2)
                    connectable = True
                except Exception as err:
                    time.sleep(THRIFT_WAIT_TIMEOUT)
            # Second check if server API is invokable
            while not invokable:
                try:
                    with self.thrift_connect() as client:
                        client.pltfm_mgr_dummy(THRIFT_DUMMY_DEVID)
                    invokable = True
                except Exception as err:
                    time.sleep(THRIFT_WAIT_TIMEOUT)
            if connectable and invokable:
                break
            retries += 1
        if not invokable and retries >= THRIFT_RETRIES_TIMES:
            self.log_error("Failed to connect to thrift server:( Exiting...\n")
            sys.exit(1)

    @property
    def log_error(self):
        return self._logger.log_error

    @property
    def log_warning(self):
        return self._logger.log_warning

    @property
    def log_notice(self):
        return self._logger.log_notice

    @property
    def log_info(self):
        return self._logger.log_info

    @property
    def log_debug(self):
        return self._logger.log_debug

    @property
    def thrift_connect(self):
        return ThriftConnect


class ThriftConnect(object):
    def __init__(
        self,
        host=THRIFT_SERVER,
        port=THRIFT_PORT,
        type="pltfm_mgr_rpc",
        client=MgrClient,
    ):
        self._host = host
        self._port = port
        self._type = type
        self._client = client
        self._transport = None

    def __enter__(self):
        self._transport = TSocket.TSocket(self._host, self._port)
        self._transport = TTransport.TBufferedTransport(self._transport)
        bprotocol = TBinaryProtocol.TBinaryProtocol(self._transport)
        mprotocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, self._type)
        client = self._client(mprotocol)
        for retry in range(0, THRIFT_RETRIES_TIMES):
            try:
                self._transport.open()
                if retry:
                    time.sleep(THRIFT_RETRY_TIMEOUT)
                break
            except TTransport.TTransportException as err:
                if err.type != TTransport.TTransportException.NOT_OPEN:
                    raise err
                time.sleep(THRIFT_RETRY_TIMEOUT)
        return client

    def __exit__(self, exc_type, exc_value, exc_traceback):
        try:
            if self._transport is not None:
                self._transport.close()
        except Exception as err:
            pass
