# Echo client program
import socket
import sys
try: 
    sys.dont_write_bytecode = True
except:
    pass

HOST = ''    # The remote host
#PORT = 50007              # The same port as used by the server
from port import PORT
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
s.sendall(' '.join(sys.argv[1:]))
while 1:
    data = s.recv(1024)
    if data:
        break
s.close()
print  (data)
