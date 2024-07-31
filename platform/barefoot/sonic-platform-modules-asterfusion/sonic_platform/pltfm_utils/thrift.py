####################################################################
# Asterfusion CX-T Devices Thrift API                              #
#                                                                  #
# Module contains an implementation of SONiC Platform Base API and #
# provides the thrift api                                          #
#                                                                  #
####################################################################

try:
    import time

    from typing import overload, Any, Literal

    from .constants import *
    from .pltfm_mgr_rpc.pltfm_mgr_rpc import Client as MgrClient
    from .pltfm_pm_rpc.pltfm_pm_rpc import Client as PmClient

    from thrift.transport import TSocket
    from thrift.transport import TTransport
    from thrift.protocol import TBinaryProtocol
    from thrift.protocol import TMultiplexedProtocol
except ImportError as err:
    raise ImportError(str(err) + "- required module not found")


class ThriftClient(object):
    @overload
    def __init__(self, host=..., port=..., rpc_type=..., rpc_client=...):
        # type: (str, int, Literal["pltfm_mgr_rpc"], type[MgrClient]) -> None
        ...

    @overload
    def __init__(self, host=..., port=..., rpc_type=..., rpc_client=...):
        # type: (str, int, Literal["pltfm_pm_rpc"], type[PmClient]) -> None
        ...

    def __init__(
        self,
        host=THRIFT_RPC_HOST,
        port=THRIFT_RPC_PORT,
        rpc_type="pltfm_mgr_rpc",
        rpc_client=MgrClient,
    ):
        # type: (str, int, Literal["pltfm_mgr_rpc"] | Literal["pltfm_pm_rpc"], type[MgrClient] | type[PmClient]) -> None
        self._host = host
        self._port = port
        self._rpc_type = rpc_type
        self._rpc_client = rpc_client
        self._rpc_transport = None
        self._rpc_connected = False

    def __del__(self):
        # type: () -> None
        self.try_close()

    def __enter__(self):
        # type: () -> MgrClient | PmClient
        self.try_connect()
        return self._client

    def __exit__(self, *_):
        # type: (Any) -> None
        self.try_close()

    def __getattr__(self, attr_name):
        # type: (str) -> Any
        if attr_name.startswith("pltfm_"):
            self.try_connect()
            return getattr(self._client, attr_name)
        return getattr(self, attr_name)

    def try_connect(self):
        # type: () -> None
        if self._rpc_connected:
            return
        self._rpc_connected = True
        self._rpc_transport = TSocket.TSocket(self._host, self._port)
        self._rpc_transport = TTransport.TBufferedTransport(self._rpc_transport)
        transport_protocol = TBinaryProtocol.TBinaryProtocol(self._rpc_transport)
        service_protocol = TMultiplexedProtocol.TMultiplexedProtocol(
            transport_protocol, self._rpc_type
        )
        self._client = self._rpc_client(service_protocol)
        retries = 0
        while True:
            timediff = time.time()
            retries += 1
            try:
                self._rpc_transport.open()
                self._rpc_connected = True
                break
            except TTransport.TTransportException as err:
                self._rpc_connected = False
                if err.type != TTransport.TTransportException.NOT_OPEN:
                    raise err
            if retries >= THRIFT_RETRY_TIMES:
                raise TTransport.TTransportException(
                    type=TTransport.TTransportException.TIMED_OUT,
                    message="connect timeout",
                )
            time.sleep(max(0, THRIFT_RETRY_TIMEOUT + timediff - time.time()))

    def try_close(self):
        # type: () -> None
        if not self._rpc_connected:
            return
        if self._rpc_transport is not None:
            self._rpc_transport.close()
        self._rpc_connected = False
