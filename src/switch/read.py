#!/usr/bin/env python

import serial

SERIAL  = "/dev/tty.NoZAP-PL2303-000013FA"

ser = serial.Serial(SERIAL, 9600)

def readData():
    print ser.read()

while 1:
    readData()
