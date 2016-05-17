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



msg = MIMEText('该邮件是可能的短账交易明细，详细请查看附件','plain','utf-8')

#输入email的地址和密码
#from_addr = input('input your email addr :')
#passwd = input('input your passwd: ')
#from_addr = 'z_jing_wei_10@163.com'
from_addr = 'zhujingwei@unionwebpay.com'
passwd = 'KJL12240203'

to_add =  'zhujingwei@unionwebpay.com'
#to_add =  'z_jing_wei_10@163.com'
#smtp_server = 'smtp.163.com' 
smtp_server = 'mail.unionwebpay.com' 

#输入SMTP服务器的地址

#smtp_server = input('SMTP server addr :')

#msg=mail.Message.Message()    #一个实例 
#msg['from']='z_j_w_13@163.com'       #自己的邮件地址 
#msg['subject']='hello world'    #邮件主题 
#msg['From'] = _format_addr('短账交易报警 <%s>' % from_addr)
msg['From'] = _format_addr('<%s>' % from_addr)
msg['To'] = _format_addr('<%s>' % to_add)
msg['Subject'] = Header('error....','utf-8').encode()
server = smtplib.SMTP(smtp_server,25)
#server.starttls()
#server.ehlo()

server.set_debuglevel(1)
server.login(from_addr,passwd)
server.sendmail(from_addr,[to_add],msg.as_string())
server.quit()
