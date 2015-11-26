#!/usr/bin/python3.4
import os 
import sys
import re

badMerchFile = 'bad_merch.txt'
logDone = 'logdone.txt'
goodMerch = 'good_merch.txt'
errorCode = ['097','0F4','0F3']


def GetBadMerch():
	badMerchList = {}
	with open(logDone,'r',encoding='GBK') as FileLogDone:
		l = FileLogDone.readlines()
		sum = len(l)
		print('%s has %d lines' % (logDone,sum))
		for line in l:
			mylist = re.findall(r'\[(.{1,15}?)\]' ,line)
			#print('lines = ',(re.findall(r'\[\d{0,20}\]',line))
			#print(mylist)
			if mylist[0] in errorCode:
#				badMerchList.append(mylist)
				print(mylist)
	print(badMerchList)
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
