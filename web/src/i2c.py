#!/usr/bin/python
import sqlite3
import smbus
import time

bus = smbus.SMBus(1)    # 0 = /dev/i2c-0 (port I2C0), 1 = /dev/i2c-1 (port I2C1)

WRITE = 0x10
READ = 0x20

#Write a single register
def getConnection():
    return sqlite3.connect("/home/pi/ws/data/sql.db")

def scannBus():
    l = list()
    for i in xrange(1, 127):
        try:
            bus.write_quick(i)
            l.append(i)
        except IOError, e:
            pass
    return l

def addDevices(addrs):
    conn = getConnection()
    c = conn.cursor()

    c.execute("DELETE FROM light")
    conn.commit()
    for a in addrs:
        c.execute(
            "INSERT INTO light (id, lastUpdate, value, address, state) "
            "VALUES"
            "(null, DATETIME('now'), 0, ?, 0)", (a, )
        )
    conn.commit()


addrs = scannBus()
addDevices(addrs)

while True:
    conn = getConnection()
    c = conn.cursor()

    c.execute("SELECT value, address, state FROM light")
    for row in c.fetchall():
        value = row[0]
        addr = row[1]
        state = row[2]

        print "Processing addr: %d, state: %s, value: %s" % (addr, state, value)
        if state in (0, 2):
            # first read all
            bus.write_byte_data(addr, 0, READ)
            newValue = bus.read_byte(addr)
            print "Value: %d newValue: %d" % (value, newValue)
            if newValue != value:
                c.execute("UPDATE light SET value=?, state=2 WHERE address=?", (newValue, addr))
                conn.commit()

        if state == 1:
            bus.write_byte_data(addr, value, WRITE)
            c.execute("UPDATE light SET state=0 WHERE address=?", (addr, ))
            conn.commit()

        time.sleep(0.5)

