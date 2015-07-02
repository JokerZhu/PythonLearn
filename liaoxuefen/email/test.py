#!/usr/bin/python3.4
#SMTP

from email.mime.text import MIMEText
from email import encoders
from email.header import Header
from email.utils import parseaddr, formataddr
import smtplib
def _format_addr(s):
	name,addr = parseaddr(s)
	return formataddr((Header(name,'utf-8').encode(),addr))



msg = MIMEText('hello,send by python..','plain','utf-8')

#输入email的地址和密码
#from_addr = input('input your email addr :')
#passwd = input('input your passwd: ')
from_addr = 'z_j_W_13@163.com'
passwd = 'KJL12240203ZJW'

to_add =  'z_jing_wei_10@163.com'

#输入SMTP服务器的地址

#smtp_server = input('SMTP server addr :')
smtp_server = 'smtp.163.com' 


msg['From'] = _format_addr('Python爱好者 <%s>' % from_addr)
msg['To'] = _format_addr('管理员 <%s>' % to_add)
msg['Subject'] = Header('from SMTP....','utf-8').encode()
server = smtplib.SMTP(smtp_server,25)

server.starttls()

server.set_debuglevel(1)
server.login(from_addr,passwd)
server.sendmail(from_addr,[to_add],msg.as_string())
server.quit()
