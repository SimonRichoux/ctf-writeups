from pwn import *

HOST, PORT = "challenges.france-cybersecurity-challenge.fr", "2151"
io = remote(HOST, PORT)
io.recvline()

p = int(io.recvline()[4:-1])
g = int(io.recvline()[4:-1])
y = int(io.recvline()[4:-1])
e = 2
r = pow(g,e,p) * y % p
s = -r % (p-1)
m = e*s % (p-1)

io.sendline(f"{m}")
io.sendline(f"{r}")
io.sendline(f"{s}")
io.readline()
io.readline()
io.readline()
io.readline()
print(io.readline().decode())