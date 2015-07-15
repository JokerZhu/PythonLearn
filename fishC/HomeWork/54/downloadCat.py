#!/usr/bin/python3.4
import urllib.request
res = urllib.request.urlopen('http://placekitten.com/g/500/600')

cat_img = res.read()
with open('cat_500_600.jpg','wb')as f:
	f.write(cat_img)
