

import serial

class SerialTransport:
    def __init__(self, port, baud=9600):
        self.ser = serial.Serial(port, baud, timeout=0.1)

    def write(self, data: bytes):
        self.ser.write(data)

    def read(self) -> bytes:
        return self.ser.read(256)

    def close(self):
        self.ser.close()
