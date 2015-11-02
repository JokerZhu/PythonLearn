#!/usr/bin/python3.4
import urllib.request
import os
import random
import re

USEPROXY = 0
def OpenUrl(url):
	if 1 == USEPROXY:
		proxylist = ['62.201.200.17:80','222.88.236.234:80']
		#增加代理
		i = random.randint(0,len(proxylist)-1)
#		print('当前第[%d]个代理 [%s] ' % (i,proxylist[i]))
		req  = urllib.request.build_opener(urllib.request.ProxyHandler({'http':proxylist[i] }))
		#req = urllib.request.Request(url)
		req.addheaders = [('User-Agent','Mozilla/5.0 (Windows NT 5.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.80 Safari/537.36')]
		response = req.open(url)
		html = response.read()
	else:
#		print('不使用用代理')
		req = urllib.request.Request(url)
		req.add_header('User-Agent','Mozilla/5.0 (Windows NT 5.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.80 Safari/537.36')
		response = urllib.request.urlopen(req)
		html = response.read()
	return(html)

def GetMainNo():
	url = 'http://jandan.net/ooxx/'
	html = OpenUrl(url).decode('utf-8')
	return int(html[html.find('current-comment-page')+23:html.find('current-comment-page')+23+4 ])
	pass
def GetImgUrl(eachUrl):
	currentList=[]
	l = []
	html = OpenUrl(eachUrl).decode('utf-8')
	a = html.find('img src=')
	while a !=-1:
		b = html.find('.jpg',a,a+80)
		if b != -1:
			l.append(html[a+9:b+4])
		else:
			b = a + 9
		a = html.find('img src=',b)
	return l 
	pass
def SaveImg(folder,img_addr):
	
	with open(img_addr.split('/')[-1],'wb')as f:
		img = OpenUrl(img_addr)
		f.write(img)
	pass

def DownLoadImg(Number=10):
	#默认读１０个网页的图
	#1、创建文件夹
	os.mkdir("Imgs")
	os.chdir("Imgs")
	#2、获取主页的编号
	no = GetMainNo()
	print('index if no is :',no)
	#3、逐页找到对应的img地址
	imgaddresslist = []
	for i in range(Number):
		eachurl = 'http://jandan.net/ooxx/page-'+str(no-i)+'#comments'
		#print('eachurl = ',eachurl)
		imgaddresslist += GetImgUrl(eachurl)
#		print(len(imgaddresslist))
#	print(imgaddresslist)
	#4、保存到文件
	for each in imgaddresslist:
#		print(each)
		SaveImg(os.getcwd(),each)
	pass
if __name__ == '__main__':
	DownLoadImg()
#	SaveImg(os.getcwd(),'http://ww3.sinaimg.cn/mw600/005vbOHfgw1exjf3132w9j30m80xctdf.jpg')
