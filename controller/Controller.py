import os
import errno
import threading
import time
import serial
from audio_talker import audio_effect

pipe = '/tmp/mypipe.fifo'
ser = '/dev/ttyACM0'

num_people = 0

def clean():
    try:
        os.remove(pipe)
    except OSError:
        pass

def openposeReceiver():
    global num_people
    clean()
    os.mkfifo(pipe)

    print("Opening openpose receiver")
    with open(pipe, 'rb') as fifo:
        print("Openpose receiver opened")
        while True:
            data = fifo.read(4)
            if len(data) > 0:
                num_people = int.from_bytes(data, byteorder='little')


def buttonListener(arduino):
    print("Button listener opened")
    global num_people
    while True:
        line = arduino.readline()
        # print(line)
        if line and line == b'read\r\n':
            print("play audio")
            audio_effect(num_people)


if __name__ == '__main__':
    arduino = serial.Serial(ser, 9600)
    threading.Thread(target=openposeReceiver).start()
    threading.Thread(target=buttonListener, kwargs={'arduino': arduino}).start()
    while True:
        # print(num_people)
        if num_people >= 4:
            level = '4\n'
        else:
            level = str(num_people) + '\n'
        arduino.write(level.encode('ascii'))
        time.sleep(0.3)