import os
import errno
import threading
import time

FIFO = '/tmp/mypipe.fifo'
num_people = 0

def clean():
    try:
        os.remove(FIFO)
    except OSError:
        pass

def openposeReceiver():
    global num_people
    clean()
    os.mkfifo(FIFO)

    print("Opening openpose receiver")
    with open(FIFO, 'rb') as fifo:
        print("FIFO opened")
        while True:
            data = fifo.read(4)
            if len(data) > 0:
                num_people = int.from_bytes(data, byteorder='little')
                # print(num_people)


if __name__ == '__main__':
    try:
        threading.Thread(target=openposeReceiver).start()
        while True:
            print(num_people)
            time.sleep(1)
    except KeyboardInterrupt:
        print("Interrupted")