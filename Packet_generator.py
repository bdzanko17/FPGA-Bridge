"""Demonstrates how to construct and send raw Ethernet packets on the
network.
You probably need root privs to be able to bind to the network interface,
e.g.:
    $ sudo python sendeth.py
"""

from socket import *


def sendeth(src, dst, eth_type, payload, interface = "eno1"):
  """Send raw Ethernet packet on interface."""

  assert(len(src) == len(dst) == 6) # 48-bit ethernet addresses
  assert(len(eth_type) == 2) # 16-bit ethernet type

  s = socket(AF_PACKET, SOCK_RAW)
  # From the docs: "For raw packet
  # sockets the address is a tuple (ifname, proto [,pkttype [,hatype]])"
  #AF_PACKET is a low-level interface directly to network devices. The packets are represented by the tuple (ifname, proto[, pkttype[, hatype[, addr]]]) where:
  s.bind((interface, 0))
  return s.send( src+dst + eth_type + payload)

if __name__ == "__main__":
  print("Sent %d-byte Ethernet packet on eth0" %
    sendeth("\x98\x28\xA6\x32\xEC\xBF",
            "\xEC\xF4\xBB\x64\x55\x3A",
            "\x7A\x05",
            "OVO JE NAJBOLJA GRUPA"))

