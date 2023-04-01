import socket
import sys
import time

from contextlib import contextmanager

pingList = []


@contextmanager
def timer(name):
    start = time.time()
    yield
    i = time.time() - start
    pingList.append(i)
    print(f'[{name}] done in {i:.4f} s\r\n')


#msg = "GET /home/postTest HTTP/1.1\r\n"

msg = """GET / HTTP/1.1\r\nHost: localhost:10000\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nsec-ch-ua: "Not_A Brand";v="99", "Microsoft Edge";v="109", "Chromium";v="109"\r\nsec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: "Windows"\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36 Edg/109.0.1518.55\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6\r\n"""


def netcat():
    i = 0
    while i < 10000:
        i += 1
        if len(sys.argv) == 1:
            sys.argv.append(input("Host:"))
            sys.argv.append(input("Port:"))
            sys.argv.append(input("recvBufflen:"))

        with timer('No.{0} connect'.format(i)):
            client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client.connect((str(sys.argv[2]), int(sys.argv[3])))
            client.sendall(msg.encode('utf-8'))
            j = 0;
            data = b''
            try:
                while j < int(sys.argv[4]):
                    data += client.recv(1)
                    j+=1
            except Exception as e:
                print(e.message)
                print(data)
            print(data)
            client.close()
    print("Total:%d Average:%f ms" % (i, sum(pingList)/i*1000))


if sys.argv[1] == '-nc':
    with timer('netcat'):
        netcat()
else:
    print("argv format: -mode host port bufflen/data (Server socketTypes:STREAM/DGRAM)")
