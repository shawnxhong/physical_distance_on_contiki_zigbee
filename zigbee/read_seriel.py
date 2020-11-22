# -*- coding: utf-8 -*-

import serial
import sys
import datetime

bytes_to_read = 10
#FILE = "record.txt"

ser = serial.Serial('/dev/ttyACM0') #, timeout = 10)
print(ser.name)

bytes_to_read = 10
#-----------------------------------------------------------------
#ser.flushInput()
#print("---done flushing")
#f = open(FILE, "w+")
#f.write("Records\r\n")
#f.close()

while(1):
#    print("---start reading")
    data = ser.readline()
    
    if data:
        data = data[1:]
        time_now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M")
        print(time_now)
        print(data)
        tag_id = data[:3]
        print("tag id", tag_id)
        f = open('%s.txt' % tag_id, "a+")
        f.write(time_now+": ")            
        f.write(data+"\n")
        
        f.close()
    else:
        print("not reading")

    
#    print("---done reading\n")
#    decoded_bytes = ser_bytes[0:len(ser_bytes)-2].decode("utd-8")
#    print(decoded_byted)
#    serial.Serial.reset_input_buffer()

#-----------------------------------------------------------------
        
#ser_bytes = ser.readline()

#-----------------------------------------------------------------
#with serial.Serial('/dev/ttyACM0', timeout = 1) as ser:
#    x = ser.read()
#    s = ser.read(100)
#    line = ser.readline()
    
#print(x)
#print(s)
#print(line)
    
#ser.close
#-----------------------------------------------------------------
print("---end")
