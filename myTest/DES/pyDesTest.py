from binascii import unhexlify as unhex
from pyDes import *


def tripleDesEncrypt(tripleDesKey = '1234567890ABCDEF1234567890ABCDEF',sourceData='' ):
	t1 = triple_des(unhex(tripleDesKey))
	result = t1.encrypt(sourceData)
	return result
	pass

def bcdhexToaschex(bcdHex ):
	ascHex =  ''.join(map(lambda x : '%02X' % ord(chr(x)),list(bcdHex)))
#	print('ascHex = ',ascHex)
	return ascHex



if __name__ == '__main__':
	print('==============3DES=================')
	while(True):
		result = ''
		#输入key
		key = input('please input your 3 des key : ')
		print('len of your input key : %d' % len(key))

		if len(key) == 32 or len(key) == 48:
			pass
		else:
			print('输入长度不正确\n')
			continue
		data = input('please input your data : ')
		hexresult = tripleDesEncrypt(key,data)
		if len(result) < 0:
			print('triple des fail\n ')
		else:
			#print('triple des succes \n len = %d resule = [%s]' % (len(hexresult),hexresult))
			result =  bcdhexToaschex(hexresult)
			print('result = ', result)
	





