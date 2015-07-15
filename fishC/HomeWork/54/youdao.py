#!/usr/bin/python3.4

import urllib.request
import urllib.parse
import json
url = 'http://fanyi.youdao.com/translate?smartresult=dict&smartresult=rule&smartresult=ugc&sessionFrom=null'
data = {}
data['type'] = 'AUTO'
data['i'] = input('请输入要翻译的词语:') 
data['doctype'] = 'json'
data['xmlVersion'] = ['1.8']
data['keyfrom'] = 'fanyi.web'
data['ue'] = 'UTF-8'
data['action']='FY_BY_CLICKBUTTON'
data['typoResult']='true'
data = urllib.parse.urlencode(data).encode('utf-8')

#Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.90 Safari/537.36

response = urllib.request.urlopen(url,data )


html = response.read().decode('utf-8')
print(html)

resrult =  json.loads(html)
print('结果是:',resrult['translateResult'][0][0]['tgt'])

