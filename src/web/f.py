#!/usr/bin/python

from flask import Flask, render_template
from flask.ext.socketio import SocketIO, emit
import logging
import sqlite3
import datetime
import json


app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)
log = logging.getLogger(__name__)

@app.route('/')
def index():
    return open('templates/index.html').read()

@socketio.on('my event', namespace='/ws')
def test_message(message):
    emit('my response', {'data': message['data']})

@socketio.on('my broadcast event', namespace='/test')
def test_message(message):
    emit('my response', {'data': message['data']}, broadcast=True)

def findAll(c):
    res = getNewItem()
    c.execute("SELECT id, lastUpdate, value FROM light")
    for row in c.fetchall():
        data = {
            "id" : row[0],
            "lastUpdate" : row[1],
            "value" : row[2]
        }
        res["rows"].append(data)
    #log.info("Update web-> %s", res)
    return res

@socketio.on('connect', namespace='/ws')
def test_connect():
    conn = getConnection()
    c = conn.cursor()
    res = json.dumps(findAll(c))
    print res
    emit('my response', res)

@socketio.on('disconnect', namespace='/test')
def test_disconnect():
    print('Client disconnected')

def getConnection():
    return sqlite3.connect("data/sql.db")

def getNewItem():
    res = dict()
    res["action"] = "load"
    res["rows"] = list()
    now = datetime.datetime.now()
    res["lastUpdate"] = "%04d-%02d-%02d %02d:%02d:%02d" % (
            now.year, now.month, now.day, now.hour, now.minute, now.second)
    return res

if __name__ == '__main__':
    app.debug = True
    socketio.run(app, '127.0.0.1', 8080)
