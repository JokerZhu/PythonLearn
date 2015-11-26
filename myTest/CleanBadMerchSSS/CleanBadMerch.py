#!/usr/bin/python3.4
import os 
import sys
import re

badMerchFile = 'bad_merch.txt'
logDone = 'logdone.txt'
goodMerch = 'good_merch.txt'
errorCode = ['97','F4','F3']


def GetBadMerch():
	with open(logDone,'r',encoding='GBK') as FileLogDone:
		l = FileLogDone.readlines()
		sum = len(l)
		for line in l:
			print(line)
			line = line.strip('][')	
			print('line = ',line)
			lines = line.split('[')
			print('lines = ',lines)
		print('%s has %d lines' % (logDone,sum))

	return sum

def CleanGoodMerch():
	return 0

if __name__ == '__main__':
	#Get bad merch without Deduplication and put data into file 
	if GetBadMerch() < 0 :
		print('Get bad merch error\n' )
		sys.exit(-1)
	#Clean good merch 
	
	if CleanGoodMerch() < 0:
		print('Clean Good Merch File fail\n')


	print('done')
	sys.exit(0)
