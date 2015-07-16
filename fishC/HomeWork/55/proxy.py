#!/usr/bin/python3.4

#import urllib.request
from urllib import request


url = 'http://whatismyip.com.tw'

proxy = {'http':'115.159.50.56:808'}
#proxy_support = request.ProxyHandler(proxy)

#opener = request.build_opener(proxy_support)
#request.install_opener(opener)
response = request.Request(url)

response.add_header('User-Agent','Mozilla/5.0 (Windows NT 6.1; WOW64; rv:38.0) Gecko/20100101 Firefox/38.0')

with request.urlopen(response) as f:
	for k,v in f.getheaders():
		print(' %s : %s ' %(k,v))
	result = f.read()


print(result.decode('utf-8')) 
