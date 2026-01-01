
import threading
import time

class SerialReader(threading.Thread):
    def __init__(self, transport, parser):
        super().__init__(daemon=True)
        self.transport = transport
        self.parser = parser
        self.running = True

    def run(self):
        while self.running:
            data = self.transport.read()

            for b in data:
                self.parser.feed(b)

            time.sleep(0.01)

    def stop(self):
        self.running = False
