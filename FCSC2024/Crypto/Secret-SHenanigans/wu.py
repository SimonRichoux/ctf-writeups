from pwn import *
from Crypto.Cipher import AES
HOST, SERVER = "challenges.france-cybersecurity-challenge.fr", "2154"

while True:
    s = remote(HOST, SERVER)
    s.readline()
    s.readline()
    s.readline()
    s.sendline("Q".encode())
    s.readline()
    s.readline()
    s.readline()
    s.sendline("Q".encode())
    s.readline()
    s.readline()
    s.readline()
    s.sendline("Q".encode())
    s.readline()
    s.readline()
    s.readline()
    s.sendline("Q".encode())

    s.readline()
    s.readline()
    s.readline()
    s.sendline("E0020401".encode())
    s.readline()
    s.sendline("Q".encode())

    s.readline()
    s.readline()
    s.readline()
    s.sendline("Q".encode())

    s.readline()
    s.readline()
    s.readline()

    s.sendline(b'R0')
    a = s.readline().decode()[:-1]
    if len(a) != 130:
        a = a[-130:]
    s.sendline(b'E0')
    s.readline()
    s.sendline("Q".encode())

    packet = bytes.fromhex(a)
    if packet[0] != 64:
        print("err")
        continue
    packet = packet[1:]

    mac = packet[-32:]
    enc_payload = packet[:-32]
    new_iv = enc_payload[-AES.block_size:]
    if b'\x09'[0] ^ new_iv[0] ^ b'\xaa'[0] == 7:
        try:
            s.readline()
            s.readline()
            s.readline()
            s.readline()
            s.sendline("R0".encode())
            b = s.readline().decode()
            s.sendline(("E0" + b[:-1] + "020901").encode())
            s.readline()
            s.sendline(("Q").encode())
            s.sendline(("R1").encode())
            print(s.readline())
            print(s.readline())
            print(s.readline())
            print(s.readline())
            s.sendline(("R1").encode())
            print(s.readline())
            break
        except:
            continue
    else:
        s.close()
        continue
