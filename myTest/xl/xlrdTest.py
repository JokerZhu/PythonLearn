#!/usr/bin/python3.4
# -*- coding:utf8 -*-
import xlrd

def testXlrd(filename):
	book=xlrd.open_workbook(filename)
	sh=book.sheet_by_index(0)
	print ("Worksheet name(s): ",book.sheet_names()[0])
	print ('book.nsheets',book.nsheets)
	print ('sh.name:',sh.name,'sh.nrows:',sh.nrows,'sh.ncols:',sh.ncols)
	print ('A1:',sh.cell_value(rowx=0,colx=1))
	#如果A3的内容为中文
	print ('A2:',sh.cell_value(0,2).encode('utf-8'))







if __name__ == '__main__':
	testXlrd(u'你好.xls')
	


