#!/usr/bin/python
import socket

#creat array of buffers from 20 to 2000 with increments of 20
buffer = ["A"]
counter = 20
while len(buffer) <= 100:
    buffer.append("A" * counter)
    counter=counter+20
#define the FTP commands to be fuzzed
commands = ["MKD", "CWD", "STOR"]


#run the fuzzing loop
for command in commands:
    for string in buffer:
        print('fuzzing' + command + ":" + str(len(string)))
        s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connect = s.connect(('127.0.0.1',21))
        s.recv(1024)
        s.send('USER ftp\r\n')
        s.recv(1024)
        s.send('PASS ftp\r\n')
        s.recv(1024)
        s.send(command + '' + string + '\r\n')
        s.recv(1024)
        s.send('QUIT\r\n')
        s.close()
