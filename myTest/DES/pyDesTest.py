#!/usr/bin/python3.4


from binascii import unhexlify as unhex
from pyDes import *
import sys
import os 
import re
import readline

def tripleDesEncrypt(sourceData='',tripleDesKey = '1234567890ABCDEF1234567890ABCDEF'):
	result = ''
	print('source len = ',len(sourceData))	
	if(len(sourceData) <= 0 ):
		print('input data len error')
		return -1
	#将数据分成8个字节进行3DES加密，不足8个字节填充0x00补齐8个字节
	t1 = triple_des(unhex(tripleDesKey),CBC,b'\0\0\0\0\0\0\0\0',pad=None ,padmode=PAD_NORMAL)
	n = len(sourceData) / 8
	m = len(sourceData) % 8
	for i in range(0,int(n)+1):
		#result = t1.encrypt(tmp)
		#print(i,n)
		if int(n) == i:
			tmp = sourceData[i * 8 : ]
			tmp += '\0' * (8 - m) 
		else:
			tmp = sourceData[i * 8 : (i + 1)*8]
		#print('tmp len : ',len(tmp))
		d = t1.encrypt(tmp)
		result += bcdhexToaschex(d)
		print('d = %r' % d)
		print(t1.decrypt(d))
		#result += bcdhexToaschex(t1.encrypt(tmp))
		#print('result = %s' % result)
	return result

def tripleDesDecrypt(data,tripleDesKey = '1234567890ABCDEF1234567890ABCDEF' ):
	t1 = triple_des(unhex(tripleDesKey),CBC,b'\0\0\0\0\0\0\0\0',pad=None ,padmode=PAD_NORMAL)
	#print('input :', data)
	#bcd =  ascTobcdhex(data[0:16])
	bcd = bytes(data.encode())
	print('bdc = %r' % bcd)
	#print(bcd)
	result =  t1.decrypt(bcd)
	print(result)
	pass
def bcdhexToaschex(bcdHex ):
	ascHex =  ''.join(map(lambda x : '%02X' % ord(chr(x)),list(bcdHex)))
#	print('ascHex = ',ascHex)
	return ascHex
def ascTobcdhex(ascHex ):
	#bcdHex =  ''.join(map(lambda x : '%c' % int(x,16),re.findall(r'.{2}', ascHex) ))
	#bcdHex =  ''.join(map(lambda x : bytes(x.encode()),re.findall(r'.{2}', ascHex) ))
	
	print('bcdHex = %r' % bcdHex)
	return bcdHex


def tripleDes():
	while(True):
		result = ''
		#输入key
		'''
		key = input('please input your 3 des key(q/Q for quit) : ')
		if key == 'q' or key == 'Q':
			break
		#print('len of your input key : %d' % len(key))

		if len(key) == 32 or len(key) == 48:
			pass
		else:
			print('input data len error\n')
			continue
		'''
		data = input('please input your data : ')
		if(len(data) <= 0 ):
			print('input data len error')
			break
		
		#result = tripleDesEncrypt(data,'31323334353637383930414243444546')
		result = tripleDesEncrypt(data)
		if len(result) < 0:
			print('triple des fail\n ')
		else:
			#print('triple des succes \n len = %d resule = [%s]' % (len(hexresult),hexresult))
			print('result = ', result)
		#tripleDesDecrypt(result)
		tripleDesDecrypt(result,'31323334353637383930414243444546')
		

if __name__ == '__main__':
	while(True):
		os.system('clear')
		print('=====================================================')
		#Input = input('please input what you want to do :' )
		tripleDes()
	pass





