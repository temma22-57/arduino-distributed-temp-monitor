

def handle_frame(src, cmd, payload):
    if cmd == 0x80 and len(payload) == 2:
        
        # merge MSB and LSB from payload
        raw = (payload[0] << 8) | payload[1]
        
        # handle negative temp (int16_t originally)
        if raw & 0x8000:
            raw -= 100000

        # reported temp is in Degrees Celcius x100
        temp = raw / 100.0
        
        print(f"[Node {src}] Temperature: {temp:.2f} Celcius")
    elif cmd == 0x81 and len(payload) == 4:
        firmware_version = payload[0]
        config_version = payload[1]
        node_id = payload[2]
        interval = payload[3]
        print(f"[Node {src}] Firmware: v{firmware_version} Config: v{config_version} Interval: {interval}s")
    elif cmd == 0xFF and len(payload) == 1:
        err_id = payload[0]
        err = "ERR"

        if err_id == 0x01:
            err = "ERR_UNKOWN_CMD"
        if err_id == 0x02:
            err = "ERR_BAD_CRC"
        if err_id == 0x03:
            err = "ERR_BAD_LEN"

        print(f"[Node {src}] {err} error code: 0x{err_id:02X}")
    else:
        print(f"[Node {src}] CMD=0x{cmd:02x} Payload={payload.hex()}")

