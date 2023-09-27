#!/usr/bin/env python

#############################################################################
# Asterfusion CX308P-48Y-T Chassis Helper API
#
# Module contains an implementation of SONiC Platform Base API and
# provides the helper api
#
#############################################################################

try:
    import itertools
    import os
    import re
    import socket
    import subprocess
    import sys
    import time

    from thrift.transport import TSocket
    from thrift.transport import TTransport
    from thrift.protocol import TBinaryProtocol
    from thrift.protocol import TMultiplexedProtocol

    sys.path.append(os.path.dirname(__file__))
    from pltfm_mgr_rpc.pltfm_mgr_rpc import Client as MgrClient
    from pltfm_pm_rpc.pltfm_pm_rpc import Client as PmClient
except ImportError as e:
    raise ImportError(str(e) + "- required module not found")


HOST_CHK_CMD = "docker"
PMON_TEMP_GET_CMD = "sensors"
HOST_TEMP_GET_CMD = ("docker", "exec", "pmon", "sensors")
THRIFT_RETRIES_TIMES = 3
THRIFT_RETRY_TIMEOUT = 30
THRIFT_WAIT_TIMEOUT = 10
THRIFT_DUMMY = 0


def get_x86_thermal_names():
    if check_if_host():
        sensor_result = get_cmd_output(HOST_TEMP_GET_CMD).split("\n")
    else:
        sensor_result = get_cmd_output(PMON_TEMP_GET_CMD).split("\n")
    names = list(
        map(
            lambda output: output.split(": ")[0].title(),
            filter(
                lambda output: re.search(r"\+?-?[0-9]+\.[0-9]*|\+?-?[0-9]+ C", output)
                is not None,
                sensor_result,
            ),
        )
    )
    for index in range(0, len(names)):
        if names[index].lower() == "temp1":
            names[index] = "CPU 0"
    return names


def get_x86_thermal_values():
    if check_if_host():
        sensor_result = get_cmd_output(HOST_TEMP_GET_CMD).split("\n")
    else:
        sensor_result = get_cmd_output(PMON_TEMP_GET_CMD).split("\n")
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
    assert len(keys) == len(values)
    result = []
    for index in range(0, len(keys)):
        result.append(dict(zip(keys[index], values[index])))
    return result


def check_if_host():
    # A process cannot be both in host and docker container.
    # Thus using a global var to indicate in where the process is.
    global IS_HOST
    try:
        return IS_HOST
    except:
        try:
            subprocess.call(
                HOST_CHK_CMD, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
            )
            IS_HOST = True
        except FileNotFoundError:
            IS_HOST = False
    return IS_HOST


def get_cmd_output(cmd):
    # Create a global dict to store shell command result.
    # The dict is time-sensitive, which means only after certain time
    # will the dict update its data(shell command result).
    global CMD_RESULT
    try:
        CMD_RESULT
    except:
        CMD_RESULT = {}
    if CMD_RESULT.get(cmd) is None:
        CMD_RESULT[cmd] = {
            "result": subprocess.check_output(cmd).decode().strip(),
            "timestamp": time.time(),
        }
    if time.time() - CMD_RESULT.get(cmd).get("timestamp", time.time()) >= 5:
        CMD_RESULT[cmd]["result"] = subprocess.check_output(cmd).decode().strip()
        CMD_RESULT[cmd]["timestamp"] = time.time()
    return CMD_RESULT[cmd]["result"]


def get_board_type(self):
    with ThriftConnect(type="pltfm_pm_rpc", client=PmClient) as client:
        return client.pltfm_pm_board_type_get()


def wait_for_thrift_server_setup():
    connectable = False
    setup = False
    retries = 0
    while retries < THRIFT_RETRIES_TIMES:
        while not connectable:
            connector = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                connector.connect(("127.0.0.1", 9095))
                connector.shutdown(2)
                connectable = True
            except:
                time.sleep(THRIFT_WAIT_TIMEOUT)
        while not setup:
            try:
                with ThriftConnect() as client:
                    client.pltfm_mgr_dummy(THRIFT_DUMMY)
                setup = True
            except:
                time.sleep(THRIFT_WAIT_TIMEOUT)
        if connectable and setup:
            break
        retries += 1
    if not setup and retries >= THRIFT_RETRIES_TIMES:
        print("Unable to connect to thrift server! Exiting...")
        sys.exit(1201)


class ThriftConnect(object):
    def __init__(self, host="127.0.0.1", port=9090, type="pltfm_mgr_rpc", client=MgrClient):
        self._host = host
        self._port = port
        self._type = type
        self._client = client
        self._transport = None
        self._retries = 5
        self._timeout = 5

    def __enter__(self):
        self._transport = TSocket.TSocket(self._host, self._port)
        self._transport = TTransport.TBufferedTransport(self._transport)
        bprotocol = TBinaryProtocol.TBinaryProtocol(self._transport)
        mprotocol = TMultiplexedProtocol.TMultiplexedProtocol(bprotocol, self._type)
        client = self._client(mprotocol)
        for retry in range(0, self._retries):
            try:
                self._transport.open()
                if retry:
                    time.sleep(self._timeout)
                break
            except TTransport.TTransportException as err:
                if err.type != TTransport.TTransportException.NOT_OPEN:
                    raise err
                time.sleep(self._timeout)
        return client

    def __exit__(self, exc_type, exc_value, exc_traceback):
        try:
            if self._transport is not None:
                self._transport.close()
        except:
            pass
