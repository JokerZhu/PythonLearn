from django.http import HttpResponse
from django.template.loader import get_template
from django.template import Context
import datetime

def hello(request):
	return HttpResponse("hello world")
def homePage(request):
	return HttpResponse("hello")
'''
def current_datetime(request,agrc=''):
	now = datetime.datetime.now()
	html = "<html><body>It is now %s. argc = %s</body></html>" % (now,agrc)
	return HttpResponse(html)
'''
def current_datetime(request):
	now = datetime.datetime.now()
	t = get_template('current_datetime.html')
	html = t.render(Context({'current_date':now}))
	return HttpResponse(html)
	
	
