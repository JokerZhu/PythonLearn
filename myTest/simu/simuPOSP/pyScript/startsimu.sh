#!/bin/sh 

echo > nohup.out
nohup python3.4 $SIMUPOSP/pyScript/simuPOSP.py &


tailf nohup.out


