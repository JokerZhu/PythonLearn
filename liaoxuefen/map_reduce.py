#!/usr/bin/python3.4
# -*- codin:utf-8 -*-
from functools import reduce

#---------------------------------
#第一题
#利用map()函数，把用户输入的不规范的英文名字，变为首字母大写，其他小写的规范名字。
#输入：['adam', 'LISA', 'barT']，输出：['Adam', 'Lisa', 'Bart']
def normalize1(name):
	return name.title()
def normalize(name):
	return name[0].upper() + name[1:].lower()
L1 =['adam','LISA','barT']
#L2 = list(map(normalize,L1))
#print(L2)


#---------------------------------
#第二题
#Python提供的sum()函数可以接受一个list并求和.
#请编写一个prod()函数，可以接受一个list并利用reduce()求积
def f(x,y):
	return x * y
def prod(L):
	return reduce(lambda x,y: x * y,L )
print('3 * 5 * 7 *9 =',prod([3,5,7,9]))
#---------------------------------
#第三题
#利用map和reduce编写一个str2float函数，把字符串'123.456'转换成浮点数123.456
def str2float(s):
	pass

print('str2float(\'123.456\') = ',str2float('123.456'))




