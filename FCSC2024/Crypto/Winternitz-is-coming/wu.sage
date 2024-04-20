from numpy import argsort

W = 257
Support = [
              8,  17,  26,  32,  52,  53,  57,  58,
             59,  63,  64,  66,  67,  71,  73,  76,
             79,  81, 111, 115, 132, 135, 141, 144,
            151, 157, 170, 176, 191, 192, 200, 201,
            202, 207, 216, 224, 228, 237, 241, 252,
        ]

Zp = FiniteField (257)
M = Matrix(Zp, 40, 20)
for i in range (40):
    for j in range (20):
        M[i, j] = pow(Support[i],(j  + 1), W)

message = vector([Zp(c) for c in b'WINTERNITZ IS COMING'])
w = M * message
I = list(argsort(w))[20:]
J = list(argsort(w))[:20]

P = M[I].inverse()
w_I = vector([w[i] for i in I])

print("[+] beginning of bruteforce")
while True:
    v = vector([randint(0, 256-a) for a in w_I])
    c = M * (P * (w_I + v))
    if all(c[j] >= w[j] for j in J):
        forged_message = P * (w_I + v)
        break
print("[+] ending of bruteforce")

# sanity check
w_forged = M * forged_message
assert all(i >= j for i, j in zip (w_forged, w))

print("[+] message found!")
print(forged_message)