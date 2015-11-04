#!/usr/bin/python3.4
# -*- coding:utf-8 -*-
import os,os.path,io

'''
#0、统计当前目录下每个文件类型的文件数
def GetFileTypeAndSum():
	l = list()
	for n in os.listdir(os.curdir):
		if os.path.isfile(os.path.join(os.curdir,n)): 
			l.append(os.path.splitext(n)[1])
		elif os.path.isdir(os.path.join(os.curdir,n)):
			l.append('文件夹')
	for item in set(l):
		print('该文件夹下共有类型为【%s】的文件 %d 个'  % (item,l.count(item) )) 


GetFileTypeAndSum()
'''
'''
#1、计算当前文件夹所有文件的大小


def GetCurtDirFileSize():
	for n in os.listdir(os.curdir):
		if os.path.isfile(os.path.join(os.curdir,n)):
			print( '%s 【%d Bytes】' % (n,os.path.getsize(n)) )
	pass

GetCurtDirFileSize()
'''
'''
#2、用户输入目录以及开始搜索的文件名，搜索该文件是否存在该目录内。如果遇到文件夹则进入文件夹继续搜索

def FindFile(FileName,Path = '.'):
	for n in os.listdir(Path):
		a = os.path.join(Path,n)
		if os.path.isfile(a) and  FileName == n :
			print(a)
		elif os.path.isdir(a):
			FindFile(FileName,a)
	pass

path = input('请输入待查找的初始目录:')
if not os.path.isdir(path):
	print('输入的目录有误！' )
	exit(0)

name = input('请输入待查找的文件名:')
FindFile(name,path)
'''
#3、用户输入关键字，查找当前文件夹内所有包含该关键字的文本文件(.txt),要求显示该文件的位置以及关键字在文件中的位置(第几行第几个字符)

def FindStrInFile(str,Path = '.'):
	for n in os.listdir(Path):
		a = os.path.join(Path,n)
		if os.path.isfile(a):
			print('当前文件:',a)
			f = open(a,'rb')
			for line in f.readlines():
				print('在文件[%s]中找到关键字[%s]' % (a,str))
				#if
		#		print('line = %s' % (line))
		elif os.path.isdir(a):
			FindStrInFile(str,a)
	pass

str = input('请将该脚本放于待查找的文件夹内,请输入关键字:')

FindStrInFile(str)
