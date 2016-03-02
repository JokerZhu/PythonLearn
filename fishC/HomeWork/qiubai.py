#!/usr/bin/python3.4
import urllib.request
import random
import re
from bs4 import BeautifulSoup
import os

url = 'http://www.qiushibaike.com'

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

def ReadQiubai(tail = ''):
	l = []
#	url = 'http://www.qiushibaike.com/'
	#打开糗百首页
	currentURL = url+tail
	print('current page = %s' % currentURL)
	html = OpenUrl(currentURL).decode('utf-8')
	#with open('qiubaiIndex.html','r') as filehtml:
	#with open('qiubai2.html','r') as filehtml:
	#	html = filehtml.read()
	#	return GetPageUrl(html)
	#从糗百返回的网页中找到段子
	p = re.compile(r'class="content">([^"]+)<!')
	l = p.findall(html) 
	for each in l:
		print(each)
	#从糗百返回的网页中找到8小时内的链接
	#soup = BeautifulSoup(html)
	return GetPageUrl(html)
	
def GetPageUrl(html,number = 10):
	result = {}
	soup = BeautifulSoup(html)
	#linklist = soup.find_all('li')
	for each in soup.find_all('li'):
		a = each.find('a')
		span = each.find('span')
		#当前页
		if span is not None :
			if span.get('class') == ['current']:
				result['current'] = span.string.strip('\n')
		#其他页的URL
		if a is None:
			continue
		href = a.get('href')
		if isinstance(href,str) and len(href) > 0 and len(re.findall(r'/8hr',href)) > 0 :
			result[span.string.strip('\n')] = href.strip('\n')
	#print(result)
	#print(sorted(result.items(),key=lambda d:d[1]))
	return result

def DisplayDuanzi(tail = ''):
	return ReadQiubai(tail)

	


if __name__ == '__main__':
	os.system('clear')
	choicePage = {1:'下一页',2:'<'} 
	print('看段子咯')
	urlTail = DisplayDuanzi()
	while(True):
		input()
		print('-' * 20 ,'请选择:','-' * 20)
		print('1.下一页')
		print('2.上一页')
		try:
			choice = int(input('请选择:'))
		except ValueError as e:
			print('输入错误')
			continue
		
		urlTail = DisplayDuanzi(urlTail[choicePage[choice]])
		os.system('clear')
		
