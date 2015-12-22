#!/usr/local/bin/python2.7
from ISO8583.ISO8583 import ISO8583
from ISO8583.ISOErrors import *
import socket
import sys
import traceback

serverIP = '183.63.103.90'
serverPort = 19183 

def PackLogonPackage():
	PackLogon = ISO8583()
	transation = "800"
	PackLogon.setTransationType(transation) 
	PackLogon.setBit(11,5768)
	PackLogon.setBit(41,'16688013')
	PackLogon.setBit(42,'000000000000001')
	PackLogon.setBit(60,'4629003')
	PackLogon.setBit(63,'01 ')
	#print( PackLogon.showRawIso())
	message = PackLogon.getNetworkISO()
	return message
	#print('message = ',message)
	pass

def Login():
	#pack 8583
	package = PackLogonPackage()

	
	#send to server
	sendRecvMessage(package)
	return 0 
	pass

def sendRecvMessage(message):
	s = None
	for res in socket.getaddrinfo(serverIP, serverPort, socket.AF_UNSPEC, socket.SOCK_STREAM):
		af, socktype, proto, canonname, sa = res
		try:
			s = socket.socket(af, socktype, proto)
		except socket.error, msg:
			s = None
			continue
		try:
			s.connect(sa)
		except socket.error, msg:
			s.close()
			s = None
			continue
		break
	if s is None:
		print ('Could not connect :(')
		sys.exit(1)
	print('sending message = [%s]' % message)
	s.send(message)
	ans = s.recv(2048)
	print('recv message = [%s]' % ans)
	s.close()
	return 0

if __name__ == '__main__':
	print('starting..')
	#login 
	if Login() < 0:
		print('login error ')
		sys.exit(-1)


