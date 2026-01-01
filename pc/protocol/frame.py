

from .crc import crc_xor

SOF = 0xAA

def build_frame(src: int, cmd: int, payload: bytes = b"") -> bytes:
    length = 1 + 1 + len(payload)
    body = bytes([length, src, cmd]) + payload
    crc = crc_xor(body)
    return bytes([SOF]) + body + bytes([crc])
