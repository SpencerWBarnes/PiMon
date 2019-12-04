from threading import Thread
from time import sleep

import redis
import serial

PORT_NAME = "/dev/ttyACM0"

red = redis.StrictRedis()


def serialReader(portName):
    port = serial.Serial(portName, 115200)
    while True:
        readSerial = port.readline().decode('utf-8')
        red.set('msg', readSerial)
        print(readSerial)
        sleep(0.05)


def triggerSensorData(portName):
    port = serial.Serial(portName, 115200)
    command = "get sensors"
    command = command.encode()
    while True:
        port.write(command)
        sleep(0.4)


def setupSerialHandlers():
    Thread(target=serialReader, args=(PORT_NAME,), daemon=True).start()
    triggerSensorData(PORT_NAME)
