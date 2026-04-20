
from database.db import *
from protocol.frame import build_frame

def repl(transport, host_id):
    while True:
        try:
            line = input("> ").strip().split()
            if not line:
                continue

            cmd = line[0]

            if cmd == "ping":
                node_id = int(line[1])

                if node_id == host_id:
                    transport.write(build_frame(0x00, 0x01))
                else:
                    mac_address = get_mac_address(node_id)
                    
                    mac_array = bytes([int(b, 16) for b in mac_address.split(':')])
                    payload = mac_array + build_frame(0x00, 0x01)
                    transport.write(build_frame(0x00, 0x35, payload))

            elif cmd == "temp":
                node_id = int(line[1])

                if node_id == host_id:
                    transport.write(build_frame(0x00, 0x10))
                else:
                    mac_address = get_mac_address(node_id).replace(":", "")
                    
                    payload1 = bytes.fromhex(mac_address)
                    payload2 = [payload1, build_frame(0x00, 0x10)]
                    transport.write(build_frame(0x00, 0x35, payload2))

            elif cmd == "set_interval":
                node_id = int(line[1])
                sec = int(line[2])
                internal_payload = bytes([(sec >> 8) & 0xFF, sec & 0xFF])
                
                if node_id == host_id:
                    transport.write(build_frame(0x00, 0x20, internal_payload))
                else:
                    mac_address = get_mac_address(node_id).replace(":", "")
                    
                    payload1 = bytes.fromhex(mac_address)
                    payload2 = [payload1, build_frame(0x00, 0x20, internal_payload)]
                    transport.write(build_frame(0x00, 0x35, payload2))

            elif cmd == "mac":
                node_id = int(line[1])

                if node_id == host_id:
                    transport.write(build_frame(0x00, 0x15))
                else:
                    mac_address = get_mac_address(node_id).replace(":", "")
                    
                    payload1 = bytes.fromhex(mac_address)
                    payload2 = [payload1, build_frame(0x00, 0x15)]
                    transport.write(build_frame(0x00, 0x35, payload2))

            elif cmd == "list_nodes":
                devices = list_all_devices()
                print("| ID |    Mac Address    | Name ")
                for node_id, mac, name in devices:
                    print(f"|  {node_id} | {mac} | {name}")

            elif cmd == "add_node":
                node_id = int(line[1])
                mac = line[2]
                name = line[3]
                add_node(node_id, mac, name)

            elif cmd == "quit":
                break
            
            else:
                print("Commands: ping <id>, temp <id>, set_interval <id> <sec>, mac <id>, list_nodes, add_node <node_id> <mac> <name>, quit")

        except Exception as e:
            print("Error:", e)
