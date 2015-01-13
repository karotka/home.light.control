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


class UpdateThread(threading.Thread):
    def run(ws):
        log.info("Update thread started <%s>." % ws)
        while True:
            time.sleep(5)
            log.info("---- update -----")


def wsgi_app(environ, start_response):
    path = environ["PATH_INFO"]
    if path == "/":
        return app(environ, start_response)
    elif path == "/ws":
        handle_websocket(environ["wsgi.websocket"])
    else:
        return app(environ, start_response)

def getConnection():
    return sqlite3.connect(app.config.get("DBFILE"))

def getNewItem():
    res = dict()
    res["rows"] = list()
    now = datetime.datetime.now()
    res["lastUpdate"] = "%04d-%02d-%02d %02d:%02d:%02d" % (
            now.year, now.month, now.day, now.hour, now.minute, now.second)
    return res

def send(socket, data):
    try:
        socket.send(data)
    except WebSocketError, e:
        log.info("KILL CNNECTION -------")

@sockets.route('/ws')
def handle_websocket(ws):
    conn = getConnection()
    c = conn.cursor()

    threadId = None
    while True:
        #if not threadId:
        #    threadId = thread.start_new_thread(update, (ws, ))
        #    log.info("Start update thread: %s", threadId)

        # waiting for message
        m = ws.receive()
        if m:
            item = json.loads(m)
            c.execute("UPDATE light SET lastUpdate=DATETIME('now'), "
                      "value=? WHERE id=?", (item["value"], item["id"]) )
            conn.commit()

        res = getNewItem()
        c.execute("SELECT id, lastUpdate, value FROM light")
        for row in c.fetchall():
            data = {
                "id" : row[0],
                "lastUpdate" : row[1],
                "value" : row[2]
            }
            res["rows"].append(data)

        send(ws, json.dumps(res))


@app.route('/')
def index():
    log.info("Serving page.html")
    page = open('templates/index.html').read()
    page = page.\
            replace("__WS__", app.config.get("WS"))
    return page


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

    httpServer.serve_forever()

if __name__ == "__main__":
    start()
