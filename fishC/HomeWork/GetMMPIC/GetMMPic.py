import urllib.request
import os
import random
import re
##http://jandan.net/ooxx/page-1591#comments

USEPROXY = 1
def OpenUrl(url):
    if 1 == USEPROXY:
        proxylist = ['62.201.200.17:80','218.200.66.200:8080','124.66.115.137:8090','222.88.236.234:80']
        #增加代理
        i = random.randint(0,len(proxylist)-1)
        print('当前第[%d]个代理 [%s] ' % (i,proxylist[i]))
        req  = urllib.request.build_opener(urllib.request.ProxyHandler({'http':proxylist[i] }))
        #req = urllib.request.Request(url)
        req.addheaders = [('User-Agent','Mozilla/5.0 (Windows NT 5.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.80 Safari/537.36')]
        response = req.open(url)
        html = response.read().decode('utf-8')
    else:
        print('不适用代理')
        req = urllib.request.Request(url)
        req.add_header('User-Agent','Mozilla/5.0 (Windows NT 5.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.80 Safari/537.36')
        response = urllib.request.urlopen(req)
        html = response.read().decode('utf-8')
    return(html)

def GetMainNo():
    url = 'http://jandan.net/ooxx/'
    html = OpenUrl(url)
    return int(html[html.find('current-comment-page')+23:html.find('current-comment-page')+23+4 ])
    pass
def GetImgUrl(eachUrl):
    currentList=[]
    l = []
    html = OpenUrl(eachUrl)
    #print(html)
    #print('抓到一个妹纸图:',html[html.find('.jpg') - 60 : html.find('.jpg')+4])
    #l = html[html.find('.jpg') - 60 : html.find('.jpg')+4]
    p = re.compile(r'(http):\\\\/\\\\/[\\\\w \\\\-_]+(\\\\.[\\\\w\\\\-_]+)+([\\\\w\\\\-\\\\.,@?^=%&amp;:/~\\\\+#]*[\\\\w\\\\-\\\\@?^=%&amp;/~\\\\+#]).jpg')
    Match =  p.match(html)
    if Match:
        print(Match.group())
    print('抓到一个妹纸图:',l)
    currentList.append(l)
    return currentList
    pass


def DownLoadImg(Number=10):
    #1、获取主页的编号
    no = GetMainNo()
    print('index if no is :',no)
    #2、逐页找到对应的img地址
    imgaddresslist = []
    for i in range(Number):
        eachurl = 'http://jandan.net/ooxx/page-'+str(no-i)+'#comments'
        #print('eachurl = ',eachurl)
        imgaddresslist.append(GetImgUrl(eachurl))
    print(len(imgaddresslist))
    #3、保存到文件
    pass
if __name__ == '__main__':
    #DownLoadImg()
    html = OpenUrl('http://jandan.net/ooxx/')
