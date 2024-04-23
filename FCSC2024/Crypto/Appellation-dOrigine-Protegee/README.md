# Appellation d'Origine Protégée

| Category | Author   | Difficulty | Solves | Points |
| -------- | -------- | ---------- | ------ | ------ |
| Crypto   | graniter | ⭐️⭐️⭐️        | 8      | 482    |

> Une compagnie de labels de produits de consommation a décidé de certifier ses produits grâce aux très innovants NFTs. La compagnie ne voulait pas faire les choses à moitié, alors elle a créé sa propre blockchain.
>
> Votre ami, producteur de mousseux, souhaite en profiter pour monter en gamme. Avec ses compétences en informatique, il a réussi à dénicher une permutation cryptographique utilisée dans la blockchain, et a besoin de résoudre le problème CICO pour enfin pouvoir vendre du champagne.
>
> Sauriez-vous l'aider ?

**Challenge files:**

- `appellation-origine-protegee.py`

## Write-up

**Introduction**

The goal of this challenge is to solve the Constrained Input/Constrained Output (CICO) problem for a handcrafted function defined in `appellation-origine-protegee.py`. For a given function $`P:\mathbb{F}_q^t \to \mathbb{F}_q^t`$ and $` u < t `$ the CICO problem can be formulated as finding:

``` math
X, Y\in \mathbb{F}_q^{t-u} \quad P(0^u, X) = (0^u, Y).
```

This problem is motivated by blockchain applications where primitives are optimized to operate on large fields rather than binary ones. These "Arithmetization-Oriented Primitives" are usually designed around low degree polynomials and their security is based on the difficulty of solving CICO. [See](https://tosc.iacr.org/index.php/ToSC/article/view/8695/8287) for a wider introduction.

**Challenge and design of the primites**

The challenge is quite straightforward:

```python
print("Input your values as a comma-separated list: ")
X = input(">>> ").split(",")
X = [ int(x) for x in X ]
Y = aop(X)
if X[0] == 0 and Y[0] == 0:
    flag = open("flag.txt").read()
    print(flag)
else:
    print("Nope!")
```

We are asked to solve the CICO problem for $`u=1`$ for the primitive `aop`. A simplified version of this function is:

```python
def R(i, S):
    global M, RC, p # M is a 5x5 invertible matrix, RC[i] a vector and p is a prime number
    S = S * M + RC[i]
    e = pow(3, -i, p - 1)
    S[0] = S[0]**e
    return S

def aop(X):
    for i in range (9):
        X = R(i, X)
    return X
```

Thus `aop` simply iterates 9 round functions taking 5 coordinates. Each round is comprised of an affine layer $`\mathcal A`$ and a non-linear layer $`S_i`$ which elevates the first coordinate at the power $3^{-i}$. 

**Naive Algebraic attack**

A simple observation is that inverting the function `aop` leads to smaller degree polynomials:

```python
def R_inv(i, S):
    global M_inv, RC, p
    e = pow(3, i, p - 1)
    S[0] = S[0]**e
    S = (S - RC[i]) * M_inv
    return S
  
def aop_inv(X):
    for i in range (8, -1, -1):
        X = R_inv(i, X)
    return X
```

Now if we attempt to feed the multinomial `[0, w, x, y, z]` to the function `aop_inv`, we observe that sage cannot end the computations because the multinomial has degree $`\approx 2^{45}`$​ which is way to large to be reasonably computed. 

**Skipping the last 3 rounds**

A common trick used to skip some rounds is to "linearize" the first layers (see [for instance](https://tosc.iacr.org/index.php/ToSC/article/view/9850/9350)). Applying this technique here is straightforward, if we impose the round function to output a constant in its first coordinate, the non linear layer $S_i$ will not increase the degree of the system. This adds each time a linear relationship between `w, x, y, z`, thus we can do that 3 times. 

```python
R.<w, x, y, z> = PolynomialRing(Zp)

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
b = w * x + c # affine subspace in w
```

Now computing `aop_inv` with `b` takes only few seconds and yields a polynomial in `w` of degree `59049`, which is reasonable.

**Root-finding algorithm**

A small surprise arised at end of the challenge when I attempted to compute the roots of the polynomial with sage function `.roots()`. This function is weirdly implemented since it first tries to factor the polynomial which is a harder problem. Finding the root of a univariate polynomial $`P`$​ can actually be done in almost linear time in the degree of $`P`$ with the following algorithm:

- Compute $`Q = X^p -X \mod P`$​,
- Compute $`R = \gcd(P, Q)`$,
- Factor $`R`$ .

For a random polynomial, the degree of $`R`$ is $O(1)$, therefore the previous algorithm is very fast.

Writing that in sage is very easy:

```python
Q = gcd(pow(w, p, P) - w, P)
u = b(w=Q.any_root())
```

This allows to solve the CICO problem, see [wu.sage](wu.sage) for the details. 