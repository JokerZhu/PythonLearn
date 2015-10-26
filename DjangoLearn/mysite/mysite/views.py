from django.http import HttpResponse
import datetime

def hello(request):
	return HttpResponse("hello world")
def homePage(request):
	return HttpResponse("hello")

def current_datetime(request,agrc=''):
	now = datetime.datetime.now()
	html = "<html><body>It is now %s. argc = %s</body></html>" % (now,agrc)
	return HttpResponse(html)
