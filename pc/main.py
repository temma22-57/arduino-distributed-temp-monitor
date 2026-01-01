
import argparse
from protocol.parser import ProtocolParser
from serial_link.transport import SerialTransport
from serial_link.reader import SerialReader
from cli.monitor import handle_frame
from cli.interact import repl

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("mode", choices=["monitor", "interact"])
    ap.add_argument("-p", "--port", required=True)
    args = ap.parse_args()

    transport = SerialTransport(args.port)
    parser = ProtocolParser(handle_frame)
    reader = SerialReader(transport, parser)
    reader.start()

    if args.mode == "interact":
        repl(transport)
    else:
        try:
            while True:
                pass
        except KeyboardInterrupt:
            pass

    reader.stop()
    transport.close()

if __name__ == "__main__":
    main()
