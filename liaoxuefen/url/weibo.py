#!/usr/bin/python3.4
from urllib import request,parse
from http import cookiejar
import getpass
import json
import time

global email
DefaultHeaders = {
            "Accept":"*/*",
            "User-Agent":"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET4.0C; .NET4.0E)",
            "Accept-Language":"zh-cn",
            "Accept-Encoding":"gzip;deflate",
            "Connection":"keep-alive",
            "Referer":"http://weibo.com/"
}
cj = cookiejar.MozillaCookieJar("cookies.txt")
opener  = request.build_opener(request.HTTPCookieProcessor(cj))
def Post(url,postdata,charset="utf-8",headers=DefaultHeaders):
	if postdata:
		postdata = parse.urlencode(postdata).encode("utf-8")
	rr = request.Request(url=url,headers=headers,data=postdata)
	with opener.open(rr) as fp:
		if fp.info().get("Content-Encoding") == "gzip":
			f = gzip.decompress(fp.read())
			res = f.decode(charset)
		else:
			res = fp.read().decode(charset)
	return res

#登陆微博
def LoginWeibo():
	print('login to weibo.cn...')
	email = input('Email :')
	passwd = getpass.getpass('Passwd:')
	login_data = parse.urlencode([
	('username',email),
	('password',passwd),
	('entry','mweibo'),
	('client_id',''),
	('savestate','1'),
	('ec',''),
	('pagerrefer','https://passport.weibo.cn/signin/welcome?entry=mweibo&r=http%3A%2F%2Fm.weibo.cn%2F')

	])
	req = request.Request('https://passport.weibo.cn/sso/login')
	req.add_header('Origin','https://passport.weibo.cn')
	req.add_header('User-Agent', 'Mozilla/5.0 (Windows NT 6.1; WOW64; rv:38.0) Gecko/20100101 Firefox/38.0')
	#req.add_header('Referer', 'https://passport.weibo.cn/signin/login?entry=mweibo&res=wel&wm=3349&r=http%3A%2F%2Fm.weibo.cn%2F')
	req.add_header('Referer', 'http://weibo.com/u/1998993445/home?wvr=5')
	with request.urlopen(req,data=login_data.encode('utf-8')) as f:
		print('Status:', f.status, f.reason)
		result = f.read().decode('utf-8')
		print(result)
		response = json.loads(result)
		if response['retcode'] == 20000000:
			print('登陆成功') 
			return True
		else:
			print('登陆失败，%s' % response['msg'])
			return False

def FaWeiBo():
	NewWeibo = input('请输入你要发的微博:\n')
	weiboData = parse.urlencode([
	('_t' , '0'),
	('appkey' , ''),
	('location' , '	v6_content_home'),
	('module' , 'stissue'),
	('pdetail' , ''),
	('pic_id' , ''),
	('pub_type' , 'dialog'),
	('rank' , '0'),
	('rankid' , ''),
	('style_type' , ''),
	('text',NewWeibo)
	])
#						   http://weibo.com/aj/mblog/add?ajwvr=6&__rnd=1437037030287
	req = request.Request("http://weibo.com/aj/mblog/add?ajwvr=6&__rnd=%s" % (int(time.time()*1000)))
	req.add_header('User-Agent', 'Mozilla/6.0 (iPhone; CPU iPhone OS 8_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/8.0 Mobile/10A5376e Safari/8536.25')
	req.add_header('User-Agent', 'Mozilla/5.0 (Windows NT 6.1; WOW64; rv:38.0) Gecko/20100101 Firefox/38.0')
	req.add_header('Referer','http://weibo.com' )
	with request.urlopen(req,data=weiboData.encode('utf-8')) as f:
		print('Status:', f.status, f.reason)
		#result = f.read().decode('utf-8')
		result = f.read()
		print(result)


if __name__ == "__main__":
	if LoginWeibo() == True:
		FaWeiBo()	
	else:
		exit(0)
