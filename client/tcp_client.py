#!/usr/bin/python
from plumbum import cli
import socket
import sys
import time
import struct

kMagicNumber = 0x53545259
kPing = 1
kGetStats = 2
kResetStats = 3
kCompress = 4

kHdrSize = 8
kStatSize = 9
class Application(cli.Application):
    port = cli.SwitchAttr(["p"], int, default=4000)
    host = cli.SwitchAttr(["h"], str, default="127.0.0.1")

    def sendHdr(self, packetType, payload = 0):
        data = struct.pack("!ihh", kMagicNumber, payload, packetType)
        self.s.send(data)

    def receiveResp(self):
        data = self.s.recv(kHdrSize)
        resp = struct.unpack("!ihh", data)
        return resp

    def receiveStats(self):
        data = self.s.recv(kStatSize)
        return struct.unpack("!iic", data)

    def main(self):
        self.getStats()
        self.ping()
        self.compress()

    def connect(f):
        def wrap(self):
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                print("Connecting to {0}".format(self.port))
                s.connect((self.host, self.port))
                print("connected")
                self.s = s
                f(self) 
            except socket.error as err:
                print("Error connecting to remote host: {0}".format(err))
                sys.exit(1)
        return wrap

    @connect
    def getStats(self):
        self.sendHdr(kGetStats)
        print (self.receiveStats())

    @connect
    def ping(self):
        self.sendHdr(kPing)
        print (self.receiveResp())

    @connect
    def compress(self):
        pl = 10 * "a"  + 40 * "b" + "cc" 
        self.sendHdr(kCompress, len(pl))
        self.s.send(pl)
        resp = self.receiveResp()
        print(resp)
        data = self.s.recv(resp[1])
        print(data)
        
if __name__ == "__main__":
    Application.run()

