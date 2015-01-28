#!/usr/bin/env python
# -*- coding: utf-8 -*-
from gevent import monkey
monkey.patch_all()
import ConfigParser
from myapp import baseApp
from flask import Flask, render_template
from flask.ext.socketio import SocketIO, emit
from logging.handlers import RotatingFileHandler
from threading import Thread

import sqlite3
import datetime
import json
import logging
import time

app = Flask(
    __name__,
    static_folder = baseApp.config.get("WebServer", "StaticDir"),
    static_url_path = "/static",
    template_folder = baseApp.config.get("WebServer", "TemplateDir")
)
socketio = SocketIO(app)
log = logging.getLogger(__name__)
clientsConnected = 0

def backgroundThread():
    log.info("Run update worker ...")

    conn = getConnection()
    c = conn.cursor()

    global clientsConnected
    while True and clientsConnected > 0:
        log.info("Running ....")

        res = getNewItem()
        c.execute("SELECT id, lastUpdate, value FROM light WHERE state=2")
        for row in c.fetchall():
            data = {
                "id" : row[0],
                "lastUpdate" : row[1],
                "value" : row[2]
            }
            res["rows"].append(data)

        if len(res["rows"]):
            socketio.emit("recv:updateAll", {"data" : res}, namespace = "/ws")
            c.execute("UPDATE light SET state=0")
        conn.commit()
        time.sleep(2)

@app.route('/')
def index():
    return open('templates/index.html').read()

@socketio.on('send:updateSlider', namespace='/ws')
def updateSlider(message):
    log.info("Handle set websocket ...")

    m = message["message"]
    conn = getConnection()
    c = conn.cursor()

    # waiting for message
    log.info("Receive: %s" % (m, ))

    if m:
        item = json.loads(m)
        c.execute("UPDATE light SET lastUpdate=DATETIME('now'), "
            "value=?, state=1 WHERE id=?", (item["value"], item["id"]) )
        conn.commit()
        emit("recv:updateOneSlider", {"data" : m}, broadcast = True)

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
def connect():
    global clientsConnected
    clientsConnected += 1
    #log.info("New client connected %d." % clientsConnected)
    if clientsConnected > 0:
        thread = Thread(target = backgroundThread)
        thread.start()

    conn = getConnection()
    c = conn.cursor()
    res = json.dumps(findAll(c))
    log.info("New client connected to the server. "
             "Total %d connected." % clientsConnected)
    emit('init', res)

@socketio.on('disconnect', namespace='/ws')
def disconnect():
    global clientsConnected
    clientsConnected -= 1
    log.info("Client disconnected from the server %d clients connected" % clientsConnected)

def getConnection():
    return baseApp.db

def getNewItem():
    res = dict()
    res["action"] = "load"
    res["rows"] = list()
    now = datetime.datetime.now()
    res["lastUpdate"] = "%04d-%02d-%02d %02d:%02d:%02d" % (
            now.year, now.month, now.day, now.hour, now.minute, now.second)
    return res

if __name__ == '__main__':
    app.debug = bool(baseApp.config.getint("Log", "AppDebug"))

    handler = RotatingFileHandler(baseApp.config.get("Log", "LogFile"), maxBytes = 10000, backupCount = 1)
    handler.setLevel(logging.INFO)
    formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s %(name)s')
    handler.setFormatter(formatter)
    log.addHandler(handler)
    logging.basicConfig(level = logging.INFO)

    socketio.run(
        app,
        baseApp.config.get("WebServer", "Host"),
        baseApp.config.getint("WebServer", "Port")
    )
