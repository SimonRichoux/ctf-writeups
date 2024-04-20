from pwn import *
HOST, PORT = "challenges.france-cybersecurity-challenge.fr", "2152"

io = remote(HOST, PORT)
io.recvline()
p = int(io.recvline()[4:-1])
g = int(io.recvline()[4:-1])
y = int(io.recvline()[4:-1])
m = int(io.recvline().split(b'm = ')[1][:-1])

r = (p-1)//2
s = -m % (p-1)

io.sendline(f"{r}")
io.sendline(f"{s}")

msg = io.recvall().decode().strip()
if "FCSC" in msg:
    print("FCSC" + msg.split("FCSC")[-1])
else:
    print("fail")

io.close()