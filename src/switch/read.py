#!/usr/bin/env python

import serial

#SERIAL  = "/dev/tty.NoZAP-PL2303-000013FA"
SERIAL  = "/dev/tty.NoZAP-PL2303-000012FD"

ser = serial.Serial(SERIAL, 9600)

def readData():
    print ord(ser.read())

while 1:
    readData()
