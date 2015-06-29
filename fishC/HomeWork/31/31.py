#!/usr/bin/python3.4
import pickle


'''
def CutFile():
	PickleFile = open('record.txt','rb')
	Myfile = pickle.load(PickleFile)
	print(Myfile)
	PickleFile.close()
	pass


CutFile()
'''

def save_file(boy, girl, count):
	file_name_boy = 'boy_' + str(count) + '.txt'
	file_name_girl = 'girl_' + str(count) + '.txt'

	boy_file = open(file_name_boy, 'wb') # 记得一定要加 b 吖
	girl_file = open(file_name_girl, 'wb') # 记得一定要加 b 吖

	pickle.dump(boy, boy_file)
	pickle.dump(girl, girl_file)

	boy_file.close()
	girl_file.close()

def split_file(file_name):
	count = 1
	boy = []
	girl = []

	f = open(file_name)

	for each_line in f:
		if each_line[:6] != '======':
			print('39', each_line)
			(role, line_spoken) = each_line.split(':')
			if role == '小甲鱼':
				print(line_spoken)
				boy.append(line_spoken)
			if role == '小客服':
				print(line_spoken)
				girl.append(line_spoken)
		else:
			print('46',each_line)
			
			save_file(boy, girl, count)

			boy = []
			girl = []
			count += 1 

	save_file(boy, girl, count)
	f.close()

split_file('record.txt')

