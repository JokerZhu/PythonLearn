#!/usr/local/bin/python3.4
#-*- coding:utf8 -*-
import sys
import re
import os


def loadFile(fileName):
	resultList = []
	#print(fileName)
	with open(fileName,'r') as fileCfg:
		AllLines = fileCfg.readlines()
		for line in AllLines:
			#print(line)
			resultList.append(re.findall(r'\[(.{1,30}?)\]',line))
	return resultList
	pass




if __name__ == '__main__':
	#轮数
	times = 0
	#每轮的次数
	eachTimes = 0
	#总交易数
	sum = 0
	#总用时
	tmp = 0
	#总成功数
	OKSum = 0
	#平均用时
	aveTime = 0
	#每轮平均用时
	eachAveTime = 0
	#超时次数
	timeOutTimes = 0
	#通讯成功，交易失败
	transErrorTimes = 0
	#最短响应用时
	shotestTime = 0
	#最长响应用时
	longestTime = 0

	


	#print ("脚本名：", len(sys.argv))
	if len(sys.argv) != 2:
		print('error format:%s filename' % (sys.argv[0]) )
		exit(0)

	resultList =  loadFile(sys.argv[1] )
	#print(resultList)
	sum = len(resultList)
	for each in resultList:
		#轮数
		if each[4] == '0':
			eachTimes +=1
		#每轮交易次数
		if int(each[4]) > times:
			times = int(each[4])
		#成功数
		if(each[5]) == 'OK' and each[9] == '00':
			#最长响应用时
			if longestTime < float(each[7]):
				longestTime = float(each[7])
			#最短响应用时
			if shotestTime > float(each[7]) or shotestTime == 0:
				shotestTime = float(each[7])
			OKSum += 1
		elif each[5] == 'TO':
			timeOutTimes += 1
		elif (each[5]) == 'OK' and each[9] != '00':
			transErrorTimes += 1
		#平均用时
		tmp += float(each[7])

	print("共做了[%d]笔交易" % sum )
	print("共做了[%d]轮交易" % (times + 1 ))
	print("每轮  [%d]笔交易" % eachTimes)
	print("成功: [%d]笔交易" % OKSum)
	print("失败: [%d]笔交易" % (sum - OKSum))
	print("总成功率: [%.4f%%]" % (OKSum/sum * 100))
	print("平均用时: [%.6f]" % (tmp/sum))
	print("最短响应用时: [%.6f]" % (shotestTime))
	print("最长响应用时: [%.6f]" % (longestTime))
	
	
	pass

