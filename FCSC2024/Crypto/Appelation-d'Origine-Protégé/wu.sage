from hashlib import sha256

# ------------------------------------ SETUP ------------------------------------ #

p = 18446744073709551557
Zp = FiniteField(p)
t = 5
r = 9 
RC = [
        [ int.from_bytes(sha256(b"FCSC2024#" + str(t*j + i).encode()).digest()) % p for i in range(t) ]
            for j in range(r)
    ]
M = [
        [ pow(i, j, p) for i in range(1, t + 1) ]
            for j in range(t)
    ]
M = Matrix(Zp, M)
RC = Matrix(Zp, RC)
M_inv = M.inverse()

def R(r, S):
    global M, RC, p
    S = S * M + RC[r]
    e = pow(3, -r, p - 1)
    S[0] = S[0]**e
    return S

def R_inv(r, S):
    global M_inv, RC, p
    e = pow(3, r, p - 1)
    S[0] = S[0]**e
    S = (S - RC[r]) * M_inv
    return S

def aop(X):
    global r
    for i in range (r):
        X = R(i, X)
    return X

def aop_inv(X):
    global r
    for i in range (r-1, -1, -1):
        X = R_inv(i, X)
    return X

# ------------------------------------------------------------------------------- #

### Skipping last 3 rounds of using an affine space of dim 1 

R.<w, x, y, z> = PolynomialRing(Zp)
M_inv = M_inv.change_ring(R)
RC = RC.change_ring(R)

a = vector(R, [0, w, x, y, z])
# 3 equations removing 3 of the 4 degrees of liberty
eq1 = ((a - RC[r-1]) * M_inv) [0]
eq2 = ((((a - RC[r-1]) * M_inv) - RC[r-2]) * M_inv) [0] # assuming eq1 == 0
eq3 = ((((((a - RC[r-1]) * M_inv) - RC[r-2]) * M_inv) - RC[r-3]) * M_inv) [0] # assuming eq2 == 0 
A = Matrix([
        [eq1.monomial_coefficient(s) for s in R.gens()],
        [eq2.monomial_coefficient(s) for s in R.gens()], 
        [eq3.monomial_coefficient(s) for s in R.gens()]
    ])
B = vector([-eq1.constant_coefficient(), -eq2.constant_coefficient(), -eq3.constant_coefficient()])
C = vector([0, 0, 1]) # we set eq3 == 1, could be replaced by any constant

c = A.solve_right(B + C) # particular solution
x = A.right_kernel().basis()[0] # homogeneous solution 

S.<w> = PolynomialRing(Zp)
x = vector(S, [0] + x.list())
c = vector(S, [0] + c.list())
b = w * x + c

### computation of univariate system to solve CICO

# change the ring definition of matrices and vectors 
# very important, otherwise computations are slower by 2 orders of magnitude!
M_inv = M_inv.change_ring(S)
RC = RC.change_ring(S)

print("[+] beginning of computation of an univariate polynomial")
X = aop_inv(b)
print("[+] end of computation of an univariate polynomial")

### solve the univariate system

print("[+] computation of a root of the univariate polynomial")
P = X[0]
Q = gcd(pow(w, p, P) - w, P)

try:
    u = b(w=Q.any_root())
    print("[+] found a solution!")
    print(f"vector: {aop_inv(u)}")
    print(f"image of permutation: {u}")
except:
    # this can only happen if the polynomial has no roots
    print(f"[+] the polynomial has no root, attack failed")