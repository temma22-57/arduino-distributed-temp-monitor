
import argparse
from protocol.parser import ProtocolParser
from serial_link.transport import SerialTransport
from serial_link.reader import SerialReader
from cli.monitor import handle_frame
from cli.interact import repl
from cli.setup import repl_setup
from database.db import init_db

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("mode", choices=["monitor", "interact", "setup"])
    ap.add_argument("-p", "--port", required=True)
    args = ap.parse_args()

    transport = SerialTransport(args.port)
    parser = ProtocolParser(handle_frame)
    reader = SerialReader(transport, parser)
    reader.start()

    if args.mode == "interact":
        init_db()
        repl(transport, 1)
    elif args.mode == "setup":
        repl_setup(transport)
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
