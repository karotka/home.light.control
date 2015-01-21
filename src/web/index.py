import os
import sqlite3
import json
import datetime
import logging
import threading

import time

from flask import Flask
from flask_sockets import Sockets
from logging.handlers import RotatingFileHandler

from gevent.pywsgi import WSGIServer
from geventwebsocket.handler import WebSocketHandler
from geventwebsocket import WebSocketError

app = Flask(__name__)
sockets = Sockets(app)
log = logging.getLogger(__name__)

s = []

def wsgi_app(environ, start_response):
    path = environ["PATH_INFO"]
    if path == "/":
        return app(environ, start_response)
    elif path == "/ws":
        s.append(environ["wsgi.websocket"])
        log.info(">>>>>> %s" % (s))
        global t
        t = threading.Thread(target = workerWebsocket, args=(environ["wsgi.websocket"], ))
        t.start()
        handle_websocket(environ["wsgi.websocket"])
    else:
        return app(environ, start_response)

def workerWebsocket(ws):
    log.info("Run update worker ...")

    conn = getConnection()
    c = conn.cursor()

    while True:
        res = getNewItem()
        res["action"] = "update"
        c.execute("SELECT id, lastUpdate, value FROM light WHERE state=1")
        for row in c.fetchall():
            data = {
                "id" : row[0],
                "lastUpdate" : row[1],
                "value" : row[2]
            }
            res["rows"].append(data)

        if len(res["rows"]):
            log.info("Sockets %s" % (s))
            for ss in s:
                log.info("Update to web-> %s %s" % (res, ss))
                ss.emit("message", json.dumps(res))
            c.execute("UPDATE light SET state=0")
        conn.commit()

        time.sleep(0.5)

@sockets.route('/ws')
def handle_websocket(ws):
    log.info("Handle set websocket ...")

    conn = getConnection()
    c = conn.cursor()

    send(ws, json.dumps(findAll(c)))

    while True:
        # waiting for message
        m = ws.receive()
        log.info("receive <%s> on socket <%s>" % (m, ws))

        if m:
            item = json.loads(m)
            c.execute("UPDATE light SET lastUpdate=DATETIME('now'), "
                      "value=?, state=1 WHERE id=?", (item["value"], item["id"]) )
            #log.info("%s" % item)
            conn.commit()

@app.route('/')
def index():
    log.info("Serving page.html")
    page = open('templates/index.html').read()
    page = page.\
            replace("__WS__", app.config.get("WS"))
    return page

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
    log.info("Update web-> %s", res)
    return res

def getConnection():
    return sqlite3.connect(app.config.get("DBFILE"))

def getNewItem():
    res = dict()
    res["action"] = "load"
    res["rows"] = list()
    now = datetime.datetime.now()
    res["lastUpdate"] = "%04d-%02d-%02d %02d:%02d:%02d" % (
            now.year, now.month, now.day, now.hour, now.minute, now.second)
    return res

def send(socket, data):
    try:
        socket.send(data)
    except WebSocketError, e:
        log.info("Sending data to closed socked")

def start():
    app.config.from_envvar('APPSETTINGS')

    handler = RotatingFileHandler('logs/debug_log', maxBytes = 10000, backupCount = 1)
    handler.setLevel(logging.INFO)
    formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s %(name)s')
    handler.setFormatter(formatter)
    log.addHandler(handler)
    logging.basicConfig(level = logging.INFO)

    server, port = app.config.get('SERVER'), app.config.get('PORT')
    httpServer = WSGIServer((server, port),
            wsgi_app, handler_class = WebSocketHandler)

    log.info("Server started at %s:%s" % (server, port))

    try:
        httpServer.serve_forever()
    except KeyboardInterrupt:
        exit()

if __name__ == "__main__":
    start()
