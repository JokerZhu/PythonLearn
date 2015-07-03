#!/usr/bin/python3.4

import logging;logging.basicConfig(level=logging.INFO)
import asyncio,os,json,time
from datetime import datetime

from aiohttp import web


'''
def index(request):
	return web.Response(body=b'<h1>Awesome</h1>')

def entrans(request):
	data = yield from request.post()
	print('server recive date :[%s]' % data)
	return web.Response(body=b'hello zhujingwei!')

@asyncio.coroutine
def init(loop):
	app = web.Application(loop=loop)
	app.router.add_route('GET','/',index)
	app.router.add_route('POST','/',entrans)
	srv = yield from loop.create_server(app.make_handler(),'127.0.0.1',9000)
	logging.info('server started at http://127.0.0.1:9000')
	return srv

loop = asyncio.get_event_loop()
loop.run_until_complete(init(loop))
loop.run_forever() 
'''
class Handler:
	def __init_(self):
		pass

	def index(self,request):
		return web.Response(body=b'<h1>Awesome</h1>')

	def entrans(self,request):
		data = yield from request.post()
		print('in entrans, server recive date :[%s],pid = [%d]' % (data,os.getpid()))
		return web.Response(body=b'hello zhujingwei!')

	def download(self,request):
		data = yield from request.post()
		print('in download,server recive date :[%s]' % data)
		return web.Response(body=b'hello zhujingwei!')

@asyncio.coroutine
def init(loop):
	handler = Handler()
	app = web.Application(loop=loop)
	app.router.add_route('GET','/',handler.index)
	app.router.add_route('POST','/entrans',handler.entrans)
	app.router.add_route('POST','/download',handler.download)
	srv = yield from loop.create_server(app.make_handler(),'127.0.0.1',9000)
	logging.info('server started at http://127.0.0.1:9000')
	return srv

loop = asyncio.get_event_loop()
loop.run_until_complete(init(loop))
loop.run_forever() 

