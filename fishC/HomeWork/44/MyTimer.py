#!/usr/bin/python3.4
import time as t


class MyTimer:

	def start(self):
		print('开始计时.')
		self.start = t.perf_counter()
	def stop(self):
		self.stop = t.pro()
		self._getTime()
		print('停止计时.')

	def _getTime(self):
		self.time = self.start - self.stop
		print('计时器运行了 %s 秒' % (self.time) )

