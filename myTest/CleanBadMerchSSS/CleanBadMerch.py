#!/usr/bin/python3.4
import os 
import sys
import re
from functools import reduce 
import time

badMerchFile = 'bad_merch.txt'
logDone = 'logdone.txt'
goodMerch = 'good_merch.txt'
tmpFile = 'tmp.txt'
errorCode = ['097','0F4','0F3']

func = lambda x,y:x if y in x else x + [y]

def GetBadMerch():
	listtmp = []
	listtmp2 = []
	#put bad merch number into tmpFile,where from logdon.txt
	with open(logDone,'r',encoding='GBK') as FileLogDone, open(tmpFile,'r+',encoding='GBK') as tmpF:
		tmpF.seek(0)
		f = tmpF.readline()
		if len(f) == 0:
			print('%s is null' % tmpFile)
		else:
			l = tmpF.readlines()
			#print('l = ' ,l)
			for line in l:
				mylist = re.findall(r'\'(.{1,15}?)\'' ,line)
				#print(mylist)
				#print(line)
				list1 = sorted((set(mylist[0:3])),key=mylist.index )
				listtmp.append( sorted(set(mylist[0:3]),key=mylist.index))
		#print(listtmp)
		print('len of listtmp [%d]' % len(listtmp))




		l = FileLogDone.readlines()
		sum = len(l)
		#print('%s has %d lines' % (logDone,sum))
		for line in l:
			mylist = re.findall(r'\[(.{1,15}?)\]' ,line)
			if mylist[0] in errorCode:
				#mylist = list(set(mylist[0:3]))
				#list2 = sorted((set(mylist[0:3])),key=mylist.index )
				listtmp.append( sorted(set(mylist[0:3]),key=mylist.index))
		
		#listtmp2 = sorted(set(listtmp),key=listtmp.index) 
	
		#print(listtmp)
		print('len of listtmp [%d], type of listtmp = [%s]' % (len(listtmp),type(listtmp)))
		for item in listtmp:
			print(item)
		listtmp =  (reduce(func,[[],]+listtmp))
		#listtmp2.append(sorted(set(listtmp)))
		#tmpF.writelines('\n')
#		CMD = ('echo >'' ' + tmpFile)
		CMD = 'cat /dev/null >' + tmpFile
		os.system(CMD)
#		time.sleep(1)
#		tmpF.writelines(['%s\n' % item for item in listtmp])
	with open(tmpFile,'a+',encoding='GBK') as tmpF:
		tmpF.writelines(['%s\n' % item for item in listtmp])
	'''
	#filter repeat data
	with open(tmpFile,'r+',encoding = 'GBK') as tmpF:
		f = tmpF.read()	
		if len(f) == 0:
			print('%s is null' % tmpFile)
		else:
			listtmp = f.split('\n') 
			listtmp = list(set(listtmp))
		print(listtmp)
		print(len(listtmp))
	'''
	return 1 
	
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
