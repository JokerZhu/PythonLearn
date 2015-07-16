#!/usr/bin/python3.4

#import urllib.request
from urllib import request
import urllib.parse
import json
import os
import os.path

global VerImge 
global Vercode
global QQNumber 
global VerifySession
VerImge = 'verImge.jepg'

VerifySession = '123'

#向后台查询是否需要验证码
def SearhVerInfo():
	#url = 'https://ssl.ptlogin2.qq.com/check?pt_tea=1&uin=3305244186&appid=501004106&js_ver=10128&js_type=0&login_sig=&u1=http%3A%2F%2Fw.qq.com%2Fproxy.html&r=0.15705769786868795'
	url = 'https://ssl.ptlogin2.qq.com/check?pt_tea=1&uin='+QQNumber+'&appid=501004106&js_ver=10128&js_type=0&login_sig=&u1=http%3A%2F%2Fw.qq.com%2Fproxy.html&r=0.15705769786868795'
	response = request.Request(url)
	response.add_header('Accept','image/png,image/*;q=0.8,*/*;q=0.5' )
	response.add_header('Accept-Encoding','Accept-Encoding')
	response.add_header('Accept-Language','Accept-Language')
	response.add_header('Connection','keep-alive')
	response.add_header('Cookie','pgv_pvid=2085574404; pgv_info=ssid=s3807416464; pt_clientip=2a747f000001f049; pt_serverip=67040a82584b91c5; ptisp=ctc')
	response.add_header('Host','ssl.captcha.qq.com')
	response.add_header('Referer','https://ui.ptlogin2.qq.com/cgi-bin/login?daid=164&target=self&style=16&mibao_css=m_webqq&appid=501004106&enable_qlogin=0&no_verifyimg=1&s_url=http%3A%2F%2Fw.qq.com%2Fproxy.html&f_url=loginerroralert&strong_login=1&login_state=10&t=20131024001')
	response.add_header('User-Agent','Mozilla/5.0 (Windows NT 6.1; WOW64; rv:38.0) Gecko/20100101 Firefox/38.0')

	with request.urlopen(response) as f:
		for k,v in f.getheaders():
			print(' %s : %s ' %(k,v))
		result = f.read()
	return(result.decode('utf-8')[13:-2].replace('\'','' ).split(',') )

#获取验证码图片
def GetImge(VerInfo):
#	url = 'https://ssl.captcha.qq.com/getimage?aid=501004106&r=0.22845868623730392&uin=3305244186&cap_cd=lwnoDurX1pwZekm6hRt0FSGH3dc8rUv3DzojFZCG_kVKGPXTzAn_vA**'
	url = 'https://ssl.captcha.qq.com/getimage?aid=501004106&r=0.22845868623730392&uin='+QQNumber+'&cap_cd='+VerInfo[1]
	#先删除本地验证码
	if os.path.exists(VerImge):
		os.remove(VerImge)
		
	response = request.urlopen(url)
	with open(VerImge,'wb') as f:
		f.write(response.read() )

#判断是否需要输入验证码,如果需要，则去下载验证码
def GetVerCode(VerInfo):
	print(VerInfo[0])
	if VerInfo[0] == '0':
#		VerifySession = VerInfo[1]
		Vercode = VerInfo[1]
		return 0
	elif VerInfo[0] == '1':
#		print('need ver code')
		GetImge(VerInfo)
		return 1


def login():
	url = 'https://ssl.ptlogin2.qq.com/login?u='+QQNumber+'&p=gAm2TmUrzYU1c7SW4kDvcBOGtc*jFVDY3NAR9vhK25*yKSFi0ai1nOKFaQP5VQ4zGY*Mx26ioWhRUP878586VQsxuxaQlGQSO*UD9G7fTQkHjWQRVtU3CLqVVzos4Xv7Ye4d-Ai4uIenp5lgBC0ATafpMNlcJrSA4YLPpTLiigEESp1xpuEXwUPpUdEG2IQ4LRNC-P*n1GEOD*VgXQl4bg__&verifycode='+Vercode+'&webqq_type=10&remember_uin=1&login2qq=1&aid=501004106&u1=http%3A%2F%2Fw.qq.com%2Fproxy.html%3Flogin2qq%3D1%26webqq_type%3D10&h=1&ptredirect=0&ptlang=2052&daid=164&from_ui=1&pttype=1&dumy=&fp=loginerroralert&action=0-19-3429484&mibao_css=m_webqq&t=2&g=1&js_type=0&js_ver=10128&login_sig=&pt_randsalt=0&pt_vcode_v1=0&pt_verifysession_v1=' + VerifySession
	with  request.urlopen(url) as f:
		res = f.read().decode('utf-8')
	print(res)



#第一步,获取是否需要验证码的标识
#QQNumber = input('请输入QQ号码:')
QQNumber = '3305244186'
VerInfo =  list(SearhVerInfo())
print(VerInfo)
#第二步,如果需要验证码则去下载验证码
if GetVerCode(VerInfo) == 1:
	Vercode = input('请输入验证码:')
	login()
else:
	login()
