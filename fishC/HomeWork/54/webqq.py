#!/usr/bin/python3.4

#import urllib.request
from urllib import request
import urllib.parse
import json



def SearVerInfo():
	#向后台查询是否需要验证码
	url = 'https://ssl.ptlogin2.qq.com/check?pt_tea=1&uin=&appid=244912654&js_ver=10129&js_type=0&login_sig=&u1=http%3A%2F%2Fw.qq.com%2Fproxy.html&r=0.629730636728759'
	response = request.Request(url )
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

	return result.decode('utf-8')[12:-1]




l =  list(SearVerInfo())

