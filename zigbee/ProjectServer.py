#!/usr/bin/python

import socket
import time
import datetime
import struct
import StringIO
from threading import Thread
import sys

UDP_SEND_PORT = 3000 # port for sending packets
UDP_REPLY_PORT = 3001 # node listens for reply packets
FILE = "record.csv"

isRunning = True

def udpListenThread():
 # listen on UDP socket port UDP_SEND_PORT
  recvSocket = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
  recvSocket.bind(("aaaa::1", UDP_REPLY_PORT))
  recvSocket.settimeout(0.5)

  while isRunning:
    try:
    	data, addr = recvSocket.recvfrom( 1024 )
    	print "Reply from:", addr[0], "Data:", data
      # print "Reply from:", addr[0], "UTC[s]:", data, "Localtime:", utc.strftime("%Y-%m-%d %H:%M:%S")
    	f = open(FILE, "a+")
    	f.write(data+"\r\n")
    	f.close()
    
    except socket.timeout:
      pass
    
def udpSendThread():

  sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM, 0)

  while isRunning:
    timestamp = int(time.time())
    print "Sending", timestamp
    # send UDP packet to nodes
    # change the IP Address with your sensorTag accordingly.
    # you may start with one sensortag.
    sock.sendto(struct.pack("I", timestamp), ("aaaa::212:4b00:1205:2bcb", UDP_SEND_PORT))
    sock.sendto(struct.pack("I", timestamp), ("aaaa::212:4b00:1665:2280", UDP_SEND_PORT))
    sock.sendto(struct.pack("I", timestamp), ("aaaa::212:4b00:1665:b601", UDP_SEND_PORT))
  
    # sleep for some seconds
    # the frequency of sending the sych timestamps packet is very important
    # you will see how this affect the sych accuracy in your experiment
    time.sleep(2)

f = open(FILE, "w+")
f.write("Contact\r\n")
f.close()

# start UDP listener as a thread
t1 = Thread(target=udpListenThread)
t1.start()
print "Listening for incoming packets on UDP port", UDP_REPLY_PORT

time.sleep(1)

# start UDP timesync sender as a thread
t2 = Thread(target=udpSendThread)
t2.start()

#print "Sending timesync packets on UDP port", UDP_SEND_PORT
print "Exit application by pressing (CTRL-C)"

try:
  while True:
    # wait for application to finish (ctrl-c)
    time.sleep(1)
except KeyboardInterrupt:
  print "Keyboard interrupt received. Exiting."
  isRunning = False




