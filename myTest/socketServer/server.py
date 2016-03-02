#!/usr/bin/python3.4
#创建SocketServerTCP服务器：
import socketserver
from socketserver import (StreamRequestHandler as SRH,ForkingMixIn as FMI ,TCPServer as TCP)
from time import ctime
import os

host = 'localhost'
port = 9989
addr = (host,port)

class Servers(SRH):
	def handle(self):
		print('got connection from ',self.client_address)
		#self.wfile.write('connection %s:%s at %s succeed!' % (host,port,ctime()))
		while True:
			data = self.request.recv(1024)
			if not data: 
				break
			print (data.decode())
			print (os.getpid())
			#print("RECV from %s " % self.client_address[0])
			self.request.send(data)

class ServerFMI(FMI,TCP):
	pass
class MyRequestHandler(SRH):
	def handle(self):
		data = self.request.recv(1024)
		print (data.decode())
		print (os.getpid())
		#self.wfile.write('connection %s:%s at %s succeed!' % (host,port,ctime()))
		self.request.send(data)

print('server is running....')
#server = socketserver.ThreadingTCPServer(addr,Servers)
server = ServerFMI(addr,MyRequestHandler)
server.serve_forever()
