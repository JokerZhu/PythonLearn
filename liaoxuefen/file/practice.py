#!/usr/bin/python3.4
#-*- coding = utf-8 -*-

#1、利用os模块编写一个能实现dir -l输出的程序。

import os
import os.path

def cmdll(path):
	return os.listdir(path)




print(cmdll('/home/zhujingwei/git/PythonLearn'))
#2、编写一个程序，能在当前目录以及当前目录的所有子目录下查找文件名包含指定字符串的文件，并打印出相对路径
'''
def cmdlR(path,str):
	for x in os.listdir(path):
		print(x)
		if os.path.isfile(os.path.join(path,x)):
			with open(os.path.join(path,x),'r') as f:
				print(f.readline())
#			pass


print(cmdlR('/home/zhujingwei/git/PythonLearn','hello'))
'''
'''
def cmdlR(path):
	for x in os.listdir(path):
		print(x)


print(cmdlR('.'))
'''
