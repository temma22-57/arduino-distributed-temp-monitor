

def crc_xor(data: bytes) -> int:
    crc = 0
    for b in data:
        crc ^= b
    
    return crc & 0xFF
