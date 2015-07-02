#!/usr/bin/python3.4
# -*- coding = utf-8 -*-

import hashlib

def get_md5(passwd):
	md5 = hashlib.md5()
	md5.update(passwd.encode('utf-8'))
	return (md5.hexdigest())


passwd = str(input('请输入密码:'))
md5 = get_md5(passwd)
print('md5 of passwd : ',md5)
