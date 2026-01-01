

from protocol.frame import build_frame

def repl(transport):
    while True:
        try:
            line = input("> ").strip().split()
            if not line:
                continue

            cmd = line[0]

            if cmd == "ping":
                transport.write(build_frame(0x00, 0x01))

            elif cmd == "temp":
                transport.write(build_frame(0x00, 0x10))

            elif cmd == "set_interval":
                sec = int(line[1])
                payload = bytes([(sec >> 8) & 0xFF, sec & 0xFF])
                transport.write(build_frame(0x00, 0x20, payload))

            elif cmd == "set_id":
                node_id = int(line[1])
                payload = bytes([node_id])
                transport.write(build_frame(0x00, 0x22, payload))

            elif cmd == "quit":
                break
            
            else:
                print("Commands: ping, temp, interval <sec>, node <id>, quit")

        except Exception as e:
            print("Error:", e)
