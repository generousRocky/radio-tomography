"""
This simple python codes read and print lines from SensorTag which connected with your test pc.

After read a line from SensorTag, it print the line at the consol and text file which name is the time of running this code

16 Jan 2017


Heerak Lim (rocky).

rocky92.lim@gmail.com
Rocky.Lim@data61.csiro.au

"""


import serial
import time

# the device name and number can be vary up to the device
# before run this code, check the /dev/
ser= serial.Serial('/dev/tty.usbmodemL3000641',115200 ,timeout=30)


now = time.localtime()
nowTime = "%02d:%02d:%02d " % (now.tm_hour, now.tm_min, now.tm_sec)
f = open(nowTime, 'w')


def start():

    while ser.is_open:
        now = time.localtime()
        nowTime = "%02d:%02d:%02d " % (now.tm_hour, now.tm_min, now.tm_sec)

        line = ser.readline()

        if (line[0] == "N") | (line[0] == "T"):
            line2 = nowTime+line
            f.write(line2,)
            print line2

        # end if
    # end while


start()











