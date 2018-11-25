#!/usr/bin/python

# test for suitability of python in detecting fan RPM

import RPi.GPIO as GPIO
import time
import threading

GPIO.setmode(GPIO.BOARD)

fan1 = 0;
fan2 = 0;
fan1_rev_count = 0;
fan2_rev_count = 0;
skipCount1 = true;
skipCount2 = true;

GPIO.setup([fan1, fan2], GPIO.IN, pull_up_down=GPIO.PUD_UP)

GPIO.add_event_detect([fan1, fan2], GPIO.FALLING, fan_irq_handler)

def fan_irq_handler(pin):
    if pin == fan1 and not skipCount1:
        fan1_rev_count += 1
        skipCount1 = true
    else:
        skipCount1 = false

    if pin == fan2 and not skipCount2:
        fan2_rev_count += 1
        skipCount2 = true
    else:
        skipCount2 = false
        

while true:
    print(fan1_rev_count, ' ', fan2_rev_count, '\n')
