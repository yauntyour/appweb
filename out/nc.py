from dataclasses import dataclass
import socket
import sys
from ping3 import ping
# -mode host port bufflen/data


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
        data = client.recv(int(sys.argv[4]))
        print(data)
        msg = input()
        if msg == "exit":
            break
        client.sendall(msg.encode('utf-8'))
    client.close()


def UDPclientSend():
    if len(sys.argv) == 1:
        sys.argv.append(input("Host:"))
        sys.argv.append(input("Port:"))
        sys.argv.append(input("data:"))
    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    client.connect((str(sys.argv[2]), int(sys.argv[3])))
    client.sendall(sys.argv[4].encode('utf-8'))
    print("send over")
    client.close()


def Server():
    if len(sys.argv) == 1:
        sys.argv.append(input("Host:"))
        sys.argv.append(input("Port:"))
        sys.argv.append(input("recvBufflen:"))
        sys.argv.append(input("Server socketTypes:"))

    if sys.argv.__len__() == 5:
        print("set socket type:STREAM")
        sys.argv.append("STREAM")

    if sys.argv[5] == "DGRAM":
        print("set socket type:DGRAM")
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind(("0.0.0.0", int(sys.argv[3])))
        #sock.listen()
        #c,addr = sock.accept()
        while True:
            #data = sock.recv(int(sys.argv[4]))
            #print(addr,"connect.")
            data = sock.recv(int(sys.argv[4]))
            print("DATA:",data)
            if not data:
                break
            #sock.sendall(data)

    elif sys.argv[5] == "STREAM":
        print("set socket type:STREAM")
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind(("0.0.0.0", int(sys.argv[3])))
        sock.listen()
        c, addr = sock.accept()
        with c:
            print(addr, "connect.")
            c.sendall(b"Hello, World\n")
            while True:
                data = c.recv(int(sys.argv[4]))
                if not data:
                    break
                print(data)
                c.sendall(data)
    sock.close()

def listen255():
    print("set socket type:DGRAM")
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.255",9995))
    #sock.listen()
    #c,addr = sock.accept()
    while True:
        #data = sock.recv(int(sys.argv[4]))
        #print(addr,"connect.")
        data = sock.recv(int(sys.argv[4]))
        print("DATA:",data)
        if not data:
            break
        #sock.sendall(data)
    sock.close()

if sys.argv[1] == '-nc':
    netcat()
elif sys.argv[1] == '-cli':
    UDPclientSend()
elif sys.argv[1] == '-server':
    Server()
else:
    print("no this mode")
    print("argv format: -mode host port bufflen/data (Server socketTypes:STREAM/DGRAM)")
