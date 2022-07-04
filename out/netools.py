from dataclasses import dataclass
import socket
import sys
from ping3 import ping


def netcat():
    if len(sys.argv) == 1:
        sys.argv.append(input("Host:"))
        sys.argv.append(input("Port:"))
        sys.argv.append(input("recvBufflen:"))

    response = ping(str(sys.argv[2]))
    print(str(response)+"ms")

    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((str(sys.argv[2]), int(sys.argv[3])))

    while True:
        msg = "POST /index.php HTTP/1.1\r\nHost: localhost\r\nUser-Agent: Mozilla/5.0 (Windows NT 5.1; rv:10.0.2) Gecko/20100101 Firefox/10.0.2\r\n\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,/;q=0.8\r\nAccept-Language: zh-cn,zh;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\nReferer: http://localhost/\r\nContent-Length: 25\r\nContent-Type: application/x-www-form-urlencoded\r\nusername=aa&password=1234"
        client.sendall(msg.encode('utf-8'))
        data = client.recv(int(sys.argv[4]))
        print(data)
    client.close()


if sys.argv[1] == '-nc':
    netcat()
elif sys.argv[1] == '-cli':
    UDPclientSend()
elif sys.argv[1] == '-server':
    Server()
else:
    print("no this mode")
    print("argv format: -mode host port bufflen/data (Server socketTypes:STREAM/DGRAM)")
