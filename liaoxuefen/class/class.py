#!/usr/bin/python3.4
# -*- coding:utf-8 -*-
'''
class Student(object):
	def __init__(self,name,score):
		self.__name = name
		self.__score = score
	def print_socre(self):
		print('%s : %s' % (self.__name, self.__score))
	def get_grade(self):
		if self.__score >= 90:
			return 'A'
		elif self.__score >= 80:
			return 'B'
		else:
			return 'C'


a = Student('zhu',90)
b = Student('jing',81)
c = Student('wei',60)

print(b.get_grade())
print(b.__score)
'''

class Animal(object):
	def run(self):
		print('animal is running...')

class Dog(Animal):
	def run(self):
		print('Dog is running...')

class Cat(Animal):
	def run(self):
		print('Cat is running...')

def run_twice(animal):
	animal.run()
	animal.run()
	


dog = Dog()
dog.run()
cat = Cat()
cat.run()

run_twice(Animal())
run_twice(Dog())
