import os
import socket
import threading
import time
def th(port,send_bytes):
	s = socket.socket()
	#s.setblocking(False)
	s.connect(("localhost",port))
	send_bytes_left = send_bytes
	while(send_bytes_left > 0):
		if(send_bytes_left >= 100):
			send_bytes = s.send(b"\0"*100)
		else:
			send_bytes = s.send(b"\0"*send_bytes_left)
		buf = s.recv(65536)
		send_bytes_left -= send_bytes
	s.close()
	print("exit")
if __name__ == "__main__":
	port = int(input("port_number:"))
	th_num = int(input("thread num:"))
	send_bytes = int(input("send_bytes:"))
	for i in range(th_num):
	#    print(threading.enumerate())
	    th1  = threading.Thread(target=th,args=(port,send_bytes))
	    th1.start()
	    time.sleep(1)
	while(True):
	    time.sleep(1)
	#    print(threading.enumerate())

