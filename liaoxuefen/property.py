#!/usr/bin/python3.4
#-*- coding:utf-8 -*-


#请利用@property给一个Screen对象加上width和height属性，以及一个只读属性resolution：
class Screen(object):
	@property
	def width(self):
		return self._width
	@width.setter
	def width(self,value):
		self._width = value

	@property
	def hight(self):
		return self._hight
	@hight.setter
	def hight(self,value):
		self._hight = value
	@property
	def resolution(self):
		return self._hight 






#test
s = Screen()
s.width = 1024
s.hight = 768
print('s.width = %s,s.hight = %s' % (s.width,s.hight))
print(s.resolution)
assert s.resolution == 786432,'1024 * 768 ?' % s.resolution
