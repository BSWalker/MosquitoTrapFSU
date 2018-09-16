#!/usr/bin/env python
import RPi.GPIO as GPIO
from datetime import datetime
import time
import sys

# verify this collection mode is enabled
mode_ena = open("/home/pi/mosquito/config/cron_ena.mosq","r")
state = mode_ena.readline()
state = state.strip() #remove trailing newline char
mode_ena.close()

if state != "enabled":
    errlog = open("/home/pi/mosquito/logs/errlog.txt","a")
    timeStamp = datetime.now().strftime("%Y-%m%d %H:%M:%S")
    entry = timeStamp + " || StartCollection.py Failed: Mode Not Enabled\n"
    errlog.write(entry)
    errlog.close()
    raise SystemExit # terminate script

# write to event log
collectionLog = open("/home/pi/mosquito/logs/collectionlog.txt","a")
timeStamp = datetime.now().strftime("%Y-%m%d %H:%M:%S")
entry = timeStamp + " || Starting Collection\n"
collectionLog.write(entry)
collectionLog.close()

# variables for pins to make code more readable
fan1 = 7
fan2 = 11
solenoid = 13

# setting up gpio pins
GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)         # supress channel in use warnings 
GPIO.setup(fan1, GPIO.OUT)      # from using multiple scripts
GPIO.setup(fan2, GPIO.OUT)
GPIO.setup(solenoid, GPIO.OUT)
GPIO.output(fan1, 1)
GPIO.output(fan2, 1)
GPIO.output(solenoid, 1)


