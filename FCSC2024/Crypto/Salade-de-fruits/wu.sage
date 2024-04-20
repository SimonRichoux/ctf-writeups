from tqdm import tqdm
from hashlib import sha256

N = 94
E = EllipticCurve([0,0,0,0,-432*N**2])
P = E.gens()[0]

def return_z (P):
    global N
    u, v = P[0], P[1]
    x, y = (36*N + v)/(6*u), (36*N - v)/(6*u)
    return lcm (x.denominator(), y.denominator())
    
minimum = +Infinity
R = P
for i in tqdm(range (100)):
    if abs(float(((R)[1]))) < 36 * N:
        z = return_z(R)
        minimum = min(z, minimum)
    R += P

if minimum != +Infinity:
    h = sha256(str(minimum).encode()).hexdigest()
    print(f"FCSC{{{h}}}")
else:
    print("fail")