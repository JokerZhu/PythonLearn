#!/usr/bin/python3.4
import urllib.request
import random
import re



USEPROXY = 0
def OpenUrl(url):
	if 1 == USEPROXY:
		proxylist = ['62.201.200.17:80','218.200.66.200:8080','124.66.115.137:8090','222.88.236.234:80']
		#增加代理
		i = random.randint(0,len(proxylist)-1)
		print('当前第[%d]个代理 [%s] ' % (i,proxylist[i]))
		req  = urllib.request.build_opener(urllib.request.ProxyHandler({'http':proxylist[i] }))
		#req = urllib.request.Request(url)
		req.addheaders = [('User-Agent','Mozilla/5.0 (Windows NT 5.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.80 Safari/537.36')]
		response = req.open(url)
		html = response.read()
	else:
		print('不适用代理')
		req = urllib.request.Request(url)
		req.add_header('User-Agent','Mozilla/5.0 (Windows NT 5.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.80 Safari/537.36')
		response = urllib.request.urlopen(req)
		html = response.read()
	return(html)

def GetDuanZi(html,no=10 ):
	pass


def GetNextUrl():
	pass

def ReadQiubai(number = 10):
	l = []
	url = 'http://www.qiushibaike.com/'
	#打开糗百首页
	html = OpenUrl(url).decode('utf-8')
	#从糗百返回的网页中找到段子
	p = re.compile(r'class="content">([^"]+)<!')
	l = p.findall(html) 
	for each in l:
		print(each)
	#从糗百返回的网页中找到8小时内的链接


def DisplayDuanzi(number= 10):
	ReadQiubai(number);

	pass
	


if __name__ == '__main__':
	nu = 0
	'''
	try:
		nu = int(input('请输入你想一页显示段子的数量(1~50):'))
	except ValueError as e:
		print('输入错误,请输入数字')
		exit(0)
	print('看段子咯')
	'''
	DisplayDuanzi(nu)


