import socket
import sys
from ping3 import ping

msg = "GET /home/postTest HTTP/1.1\r\n"
pingList = []


def netcat():
    i = 0
    while i < 1:
        i += 1
        if len(sys.argv) == 1:
            sys.argv.append(input("Host:"))
            sys.argv.append(input("Port:"))
            sys.argv.append(input("recvBufflen:"))
        response = ping(str(sys.argv[2]))
        pingList.append(response)
        print(str(response)+"ms")

        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect((str(sys.argv[2]), int(sys.argv[3])))
        client.sendall(msg.encode('utf-8'))
        data = client.recv(int(sys.argv[4]))
        print(data)
        client.close()
        print("No.%d connect." % i)
    print("%f",sum(pingList)/i)
    

if sys.argv[1] == '-nc':
    netcat()
else:
    print("argv format: -mode host port bufflen/data (Server socketTypes:STREAM/DGRAM)")
