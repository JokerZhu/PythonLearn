#!/usr/bin/python3.4
from socket import *

host = 'localhost'
#host = '192.168.1.90'
port = 9989
bufsize = 1024
addr = (host,port)


#003c600718000060220000000008000020000000c00012827628333331353030323238313233333135343531313030313400110000000100300003303031

#while True:
for i in range(0,100):
	client = socket(AF_INET,SOCK_STREAM)
	client.connect(addr)
	#data = '003c600718000060220000000008000020000000c00012827628333331353030323238313233333135343531313030313400110000000100300003303031'
	data = b'600718000060220000000008000020000000c00012827628333331353030323238313233333135343531313030313400110000000100300003303031'
	if not data or data=='exit':
		break
	#client.send(data.encode())
	#client.send(('%c%c' %(chr(int(len(data)/256)),chr(int(len(data)%256))) ).encode() + data.encode() )
	client.send(('%c%c' %(chr(int(len(data)/256)),chr(int(len(data)%256))) ).encode() + bytes(data) )

		#ret = self.request.send(('%c%c' %(chr(int(len(backData)/256)),chr(int(len(backData)%256))) ).encode() + bytes(backData))
	data = client.recv(bufsize)
	if not data:
		continue
	print(data)
	client.close() 
