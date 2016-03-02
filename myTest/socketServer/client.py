#!/usr/bin/python3.4
from socket import *

host = 'localhost'
port = 9989
bufsize = 1024
addr = (host,port)

#while True:
for i in range(0,100):
	client = socket(AF_INET,SOCK_STREAM)
	client.connect(addr)
	data = '123'
	if not data or data=='exit':
		break
	#client.send(data.encode())
	#client.send(('%c%c' %(chr(int(len(data)/256)),chr(int(len(data)%256))) ).encode() + data.encode() )
	client.send(('%c%c' %(chr(int(len(data)/256)),chr(int(len(data)%256))) ).encode()  )
	data = client.recv(bufsize)
	if not data:
		continue
	print(data)
	client.close() 
