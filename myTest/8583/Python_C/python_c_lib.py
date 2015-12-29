#!/usr/local/bin/python3.4


from ctypes import *


import os
libtest = cdll.LoadLibrary(os.getcwd() + '/test.so')

#print(libtest.multiply(2, 2))
#str2 = c_char_p(b"Hello, World")
str2 = create_string_buffer(b"zhujingwei",100)
#str2 = create_unicode_buffer('zhujingwei')

#print(len(str2))
libtest.UppleToUpperCase(str2)

print(str2.value)

