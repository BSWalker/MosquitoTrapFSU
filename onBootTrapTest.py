#!/usr/bin/env python
import RPi.GPIO as GPIO
import time
from datetime import datetime

#write timestamp to log
testlog = open("/home/pi/mosquito/logs/testlog.txt","a")
timeStamp = datetime.now().strftime("%Y-%m%d %H:%M:%S")
entry = timeStamp + " || Starting trap test on boot\n"
testlog.write(entry)

#variables for pins to make code more readable
fan1     = 7
fan2     = 11
solenoid = 13
#waiting  = 29 #blue LED
LEDgrn   = 31 #green LED
LEDred   = 33 #red LED

#setting up gpio pins
GPIO.setmode(GPIO.BOARD)
GPIO.setup(fan1, GPIO.OUT)
GPIO.setup(fan2, GPIO.OUT)
GPIO.setup(solenoid, GPIO.OUT)
GPIO.setup(LEDgrn, GPIO.OUT)
GPIO.setup(LEDred, GPIO.OUT)
GPIO.output(fan1, 0)
GPIO.output(fan2, 0)
GPIO.output(solenoid, 0)
GPIO.output(LEDgrn, 0)
GPIO.output(LEDred, 1)

#test routine 
#fan 1 for 4 seconds
GPIO.output(fan1, 1)
time.sleep(4)
GPIO.output(fan1 ,0)
time.sleep(2)

#fan 2 for 4 seconds
GPIO.output(fan2, 1)
time.sleep(4)
GPIO.output(fan2, 0)
time.sleep(2)

#click the solenoid on and off 3 times
GPIO.output(solenoid, 1)
time.sleep(1)
GPIO.output(solenoid, 0)
time.sleep(1)
GPIO.output(solenoid, 1)
time.sleep(1)
GPIO.output(solenoid, 0)
time.sleep(1)
GPIO.output(solenoid, 1)
time.sleep(1)
GPIO.output(solenoid, 0)
time.sleep(1)

#write completed message to log
timeStamp = datetime.now().strftime("%Y-%m%d %H:%M:%S")
entry = timeStamp + " || Startup test completed\n"
testlog.write(entry)
testlog.close()

#display reassuring green led for 30 seconds
GPIO.output(LEDred, 0)
GPIO.output(LEDgrn, 1)
time.sleep(30)
GPIO.output(LEDgrn, 0)

#gpio back to default values
GPIO.cleanup()


