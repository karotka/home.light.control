#!/usr/bin/env python

import serial

SERIAL  = "/dev/tty.NoZAP-PL2303-00001014"
#SERIAL  = "/dev/tty.NoZAP-PL2303-000012FD"
#SERIAL = "/dev/tty.usbserial-A9IDXBNR"
#SERIAL = "/dev/tty.usbserial-A9SRV5D5"

ser = serial.Serial(SERIAL, 1200, stopbits=2)


def readData():
    buff = []
    while 1:
        v = ser.read()
        if ord(v) == 10:
            print "". join(buff)
            return
        else:
            buff.append(v)

def readData1():
    print ord(ser.read())

while 1:
    readData1()
