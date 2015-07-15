#!/usr/bin/python3.4



class CountList:
	def __init__(self,*args):
		self.values = [x for x in args]
		self.count = {}.fromkeys(range(len(self.values)),0)

	def __len__(self):
			return len(self.values)
	def
