def byte_xor(ba1, ba2):
    return bytes([_a ^ _b for _a, _b in zip(ba1, ba2)])
cipher_flag = open ("flag.wav.enc", "rb").read()
xor = open("xor.txt", "r").read()
uncipher = bytes.fromhex(xor)
clear = open ("flag.wav", "wb").write(byte_xor(cipher_flag, uncipher))