import logging

from Pab import (
    opcode, recordLength,
    EOPATTR, EFILERR, SUCCESS,
    OPEN, CLOSE, READ, WRITE,
)
from TiSocket import BAD, TiSocket

logger = logging.getLogger(__name__)


class TcpFile(object):

    @staticmethod
    def filename():
        # open file in "append" mode, such as:
        #   PI.TCP=<hostname/ipv4-addr>:<port>
        return "TCP="

    def __init__(self, tipi_io):
        self.tipi_io = tipi_io
        self.handles = {}
        self.tisockets = TiSocket()

    def handle(self, pab, devname):
        op = opcode(pab)
        if op == OPEN:
            self.open(pab, devname)
        elif op == CLOSE:
            self.close(pab, devname)
        elif op == READ:
            self.read(pab, devname)
        elif op == WRITE:
            self.write(pab, devname)
        else:
            self.tipi_io.send([EOPATTR])

    def close(self, pab, devname):
        logger.debug("close devname: %s", devname)
        try:
            if self.handles[devname]:
                msg = bytearray(3)
                msg[0] = 0x22
                msg[1] = self.handles[devname]
                msg[2] = 0x02
                self.tisockets.processRequest(msg)
                del(self.handles[devname])
        except BaseException:
            pass
        self.tipi_io.send([SUCCESS])

    def open(self, pab, devname):
        logger.debug("open devname: %s", devname)
        try:
            server = self.parseDev(devname)
            logger.debug("host %s, port %s", server[0], server[1])
            handleId = self.tisockets.allocateHandleId()
            address = server[0] + ':' + server[1]
            msg = bytearray(len(address) + 3)
            msg[0] = 0x22
            msg[1] = handleId
            msg[2] = 0x01
            msg[3:] = address
            res = self.tisockets.processRequest(msg)
            if res == BAD:
                raise Exception("error opening socket")
            self.handles[devname] = handleId
        except Exception as e:
            logger.error(e, exc_info=True)
            self.tipi_io.send([EFILERR])
            return

        recLen = recordLength(pab)
        if recLen == 0:
            recLen = 80
        self.tipi_io.send([SUCCESS])
        self.tipi_io.send([recLen])
        return

    def read(self, pab, devname):
        logger.debug("read devname: %s", devname)

        handleId = self.handles[devname]
        msg = bytearray(5)
        buf_len = recordLength(pab)
        msg[0] = 0x22
        msg[1] = handleId
        msg[2] = 0x04
        msg[3] = (buf_len & 0xFF00) >> 8
        msg[4] = buf_len & 0xFF
        res = self.tisockets.processRequest(msg)
        if res == BAD:
            raise Exception('error reading socket')
        self.tipi_io.send([SUCCESS])
        self.tipi_io.send(res)
        return

    def write(self, pab, devname):
        logger.debug("write devname: %s", devname)

        handleId = self.handles[devname]
        self.tipi_io.send([SUCCESS])
        data = self.tipi_io.receive()

        msg = bytearray(len(data) + 3)
        msg[0] = 0x22
        msg[1] = handleId
        msg[2] = 0x03
        msg[3:] = data
        res = self.tisockets.processRequest(msg)
        if res == BAD:
            raise Exception('failed to write to sokcet')
        self.tipi_io.send([SUCCESS])

    def parseDev(self, devname):
        parts = str(devname).split("=")[1].split(":")
        return (parts[0], parts[1])
