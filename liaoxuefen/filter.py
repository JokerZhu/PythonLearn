#!/usr/local/bin/python3.4
#-*- coding:utf-8 -*-

#回数是指从左向右读和从右向左读都是一样的数，例如12321，909。请利用filter()滤掉非回数
def is_palindrome(n):
	i = 0
	l = len(str(n))
	if l > 1:
		list2 = list(str(n))	
		while i < l/2:
			if list2[i] == list2[l-i-1]:
				i=i+1
			else:
				return 0
		return 1
	else:
		return 1
	
#网上看到别人写的筛选回文的代码
def is_palindrome2(n):
	return str(n)==str(n)[::-1]




output = list(filter(is_palindrome, range(1, 10000)))
print(output)
output = list(filter(is_palindrome2, range(1, 10000)))
print(output)
