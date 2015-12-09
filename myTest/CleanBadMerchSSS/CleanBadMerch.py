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

t = time.localtime()
localDate = (('%04d%02d') % (t.tm_year,t.tm_mon))
#Fliter repeat element
def FliterList( listSource):
	listDest = []
	for i in listSource:
		if not i in listDest:
			listDest.append(i)
	return listDest
def CleanNotThistMonthData():
	ThisMonthData = []
	try:
		with open(logDone,'a+',encoding = 'GBK' ) as FileLogDone:
			FileLogDone.seek(0)
			LogDoneFile = FileLogDone.readlines()
			print('localDate = %s ' % localDate)
			print('type of LogDoneFile = ',type(LogDoneFile))
			if len(LogDoneFile) == 0:
				print('%s is null' % FileLogDone.name)
				return 0
			for line in LogDoneFile:
				#print('line = (%s),len(line) = [%d]' % (line,len(line)))
				if (len(line) == 0) or (line == '\n'):
					print('this line is empty')
					continue
				mylist = re.findall(r'\[(.{1,15}?)\]' ,line)
				#print(mylist[3][0:6])
				if mylist[3][0:6] == localDate:
					ThisMonthData.append(line.replace('\n',''))
			FileLogDone.seek(0)
			FileLogDone.truncate()
			FileLogDone.writelines(['%s\n' % item for item in ThisMonthData])
		return 0
	except FileNotFoundError as err:
		print(err) 
		return -1

	pass


def GetBadMerch():
	listtmp = []
	listtmp2 = []
	try:
		with open(logDone,'r',encoding='GBK') as FileLogDone, open(badMerchFile,'a+') as tmpF:
			tmpF.seek(0)
			#print(os.getcwd())
			f = tmpF.readlines()
			#print(f)
			if len(f) == 0:
				print('[%s] is null' % tmpF.name)
			else:
				l = tmpF.readlines()
				for line in l:
					mylist = re.findall(r'\'(.{1,15}?)\'' ,line)
					list1 = sorted((set(mylist[0:3])),key=mylist.index )
					listtmp.append( sorted(set(mylist[0:3]),key=mylist.index))
			l = FileLogDone.readlines()
			sum = len(l)
			#print('%s has %d lines' % (logDone,sum))
			for line in l:
				if (len(line) == 0) or (line == '\n'):
					print('this line is empty')
					continue
				mylist = re.findall(r'\[(.{1,15}?)\]' ,line)
				if mylist[0] in errorCode:
					listtmp.append( sorted(set(mylist[0:3]),key=mylist.index))
			listtmp =  (reduce(func,[[],]+listtmp))
			#2.4 realse
			#listtmp = FliterList(listtmp)
			print('there are  [%d] bad merchs' % len(listtmp))
			# Empty the tmp.txt
			tmpF.seek(0)
			tmpF.truncate()
			#write the bad merch data into tmp.txt
			tmpF.writelines(['%s\n' % item for item in listtmp])
	except FileNotFoundError as err:
		print(err)
		return -1
	return 1 
	
def CleanGoodMerch():
	listBadMerch = []
	listGoodMerch = []
	try:
		with open(goodMerch,'a+',encoding='GBK') as GoodMerchFile, open(badMerchFile,'r') as BadMerchFile:
			#Get bad merch number list
			BadF = BadMerchFile.readlines()
			if len(BadF) <= 0:
				print('%s is NULL,there is no bad merch number'%BadMerchFile.name )
				return 0
			BadMerchFile.seek(0)
			BadF = BadMerchFile.readlines()
			for line in BadF:
				mylist = re.findall(r'\'(.{1,15}?)\'' ,line)
			#	list1 = sorted((set(mylist[0:3])),key=mylist.index )
				listBadMerch.append( sorted(set(mylist[1:3]),key=mylist.index))
			#print(listBadMerch )

			#Get good merch list
			GoodMerchFile.seek(0)
			GoodF = GoodMerchFile.readlines()
			if len(GoodF) <= 0:
				print('%s is NULL,there is no good merch number'%GoodMerchFile.name )
				return 0
			for lineGood in GoodF:
				#fliter \n
				lineGood = lineGood.strip('\n')
				listGoodMerch.append(lineGood.split(','))
				#print(list(lineGood))
				#listGoodMerch.append( sorted(set(lineGood[1:3]),key=lineGood.index))
			#print(listGoodMerch)

			for badMerch in listBadMerch:
				while (badMerch in listGoodMerch):
					#print(badMerch)
					listGoodMerch.remove(badMerch)
			#print(listGoodMerch)
			GoodMerchFile.seek(0)
			GoodMerchFile.truncate()
			for item in listGoodMerch:
				itemtmp = str(item)
				itemtmp = itemtmp.replace('\'','')
				itemtmp = itemtmp.replace('[','')
				itemtmp = itemtmp.replace(']','')
				itemtmp = itemtmp.replace(' ','')
				#print(itemtmp)
				GoodMerchFile.writelines(itemtmp )
				GoodMerchFile.writelines('\n' )
	except FileNotFoundError as err:
		print(err)
		return -1
	return 0

if __name__ == '__main__':
	#clean logdone.txt : clean last month data
	if CleanNotThistMonthData() < 0 :
		print('Clean last month logdone data error')
		sys.exit(-1)
	else:
		print('Clean last month logdone data OK!')

	#Get bad merch without Deduplication and put data into file 
	if GetBadMerch() < 0 :
		print('Get bad merch error\n' )
		sys.exit(-1)
	else:
		print('Get bad merch OK')
	#Clean good merch 
	if CleanGoodMerch() < 0:
		print('Clean Good Merch File fail\n')
		sys.exit(-1)
	else:
		print('Clean Good Merch File OK')
	print('done')
	sys.exit(0)
