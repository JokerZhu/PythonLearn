#!/usr/bin/python3.4
import sys,os
TMPFILE = 'tmp.txt'
DESTFILE = 'dest.txt'

import sys,os

def changeList(n = 0):
	l1 = ['00','OK']
	l2 = ['01','error']
	if n%2:
		return l1
	else:
		return l2

'''
with  open(TMPFILE,'r') as tmpFile,open(DESTFILE,'w') as destFile:
	l =  tmpFile.readlines()
	now = destFile.tell()
	for line in l:
		line = line.strip('\n')
		line = line.split('|')
		if line[5] != '00':
			destFile.seek(0,2)
		else:
			destFile.seek(now,0)
			now = destFile.tell()

		destFile.writelines(['%s|' % item for item in line])
		destFile.writelines('\n' )
'''

if __name__ == '__main__':
	path = sys.path[0]
	print(path)
