

SOF = 0xAA

class ProtocolParser:
    def __init__(self, on_frame):
        self.on_frame = on_frame
        self.reset()

    def reset(self):
        self.state = "WAIT_SOF"
        self.buf = bytearray()
        self.length = 0

    def feed(self, b: int):
        if self.state == "WAIT_SOF":
            if b == SOF:
                self.state = "LEN"
                self.buf.clear()
        
        elif self.state == "LEN":
            self.length = b
            self.buf.append(b)
            self.state = "BODY"
        
        elif self.state == "BODY":
            self.bof.append(b)
            if len(self.bof) == self.length + 1:
                self.state = "CRC"

        elif self.state == "CRC":
            crc = 0
            for x in self.buf:
                crc ^= x

            if crc == b:
                length = self.buf[0]
                src = self.buf[1]
                cmd = self.buf[2]
                payload = self.buf[3:]
                self.on_frame(src,cmd,payload)
            
            self.reset()

