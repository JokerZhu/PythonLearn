#!/usr/bin/python3.4

from wsgiref.simple_server import make_server
from hello import application


httpd = make_server('',9000,application)
print('Server http om port 9000...')

httpd.serve_forever()
