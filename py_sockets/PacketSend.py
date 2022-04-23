import socket
import struct
import sys

s = socket.socket(socket.AF_PACKET, socket.SOCK_RAW)

multicast_group = ('224.0.0.251', 5353)
ttl = struct.pack('b', 1)
s.setsockopt(socket.IPPROTO_IP, socket.IP_TTL, ttl)

msg = 'Miele-001D63FFFE3C2C5Elocal'
msg_bytes = bytearray(msg, 'utf-8')
host = "224.0.0.251"
port = 5353

sent = s.send(msg_bytes)

'''
s.connect((host, port))
s.send(s_bytes)
s.close()
'''
