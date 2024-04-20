import json
from Crypto.Cipher import AES
from Crypto.Util.number import long_to_bytes

filename = "output.txt"
with open(filename, "r") as f:
    text = f.read()
unpack = json.loads(text)
iv, flag_enc, shares = bytes.fromhex(unpack["iv"]), bytes.fromhex(unpack["flag_enc"]), unpack["shares"]

I = [shares[i][0] for i in range (16)]
J = [0] + I 

p = 931620319462745440509259849070939082848977
R.<X> = PolynomialRing(FiniteField(p))

def Pi (i):
    global J
    P = prod([X-j for j in J if j != i])
    Q = prod([i-j for j in J if j != i])
    return P/Q

S = sum([shares[i][1] * Pi(I[i]) for i in range(16)])
T = Pi(0)
M = matrix(ZZ, 18)
for i in range (16):
    M[0, i] = T[i+1]
    M[1, i] = S[i+1]
    M[2+i, i] = -p
M[0, 16] = 1
M[1, 17] = 2**128

for v in M.LLL():
    if abs(v[-1]) == 2**128:
        try:
            key = long_to_bytes(abs(v[-2]))
            E = AES.new(key, mode = AES.MODE_CBC, iv = iv)
            flag = E.decrypt(flag_enc).strip()
            if b"FCSC" in flag:
                print(flag.decode())
                break
        except:
            continue