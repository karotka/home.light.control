#!/usr/bin/python


import socket

host = '127.0.0.1'
port = 8080
address = (host, port)

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((address))
server_socket.listen(5)

print "Listening for client . . ."

conn, address = server_socket.accept()

