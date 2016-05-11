#!/bin/bash 
ps -ef |grep simuPOSP.py |grep -v grep |grep -v 'sh' |grep -v 'vim' |grep -v 'vi'|grep -v 'ps' |awk '{if(NR) print "kill -9",  $2 }'|sh
