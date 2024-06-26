# Share a saucisse

| Category | Author   | Difficulty | Solves | Points |
| -------- | -------- | ---------- | ------ | ------ |
| Crypto   | graniter | ⭐️⭐️         | 27     | 434    |

> C'est l'anniversaire de votre ami Adi. Vous vouliez lui faire une blague à sa fête d'anniversaire en dissimulant un message caché dans chacune des 17 saucisses préparées pour les invités. Malheureusement, le barbecue a pris feu et Adi n'a pu sauver que 16 des 17 saucisses. Peut-il malgré tout s'en sortir ?

**Challenge files:**

- `share-a-saucisse.py`
- `output.txt`

## Write-up

**Introduction**

This challenge features the celebrated Shamir's Secret Sharing (SSS) algorithm. The flag is encrypted using the shared secret of the polynomial generated by the SSS's scheme, however we are given in `output.txt` only 16 of the 17 necessary shares required to reconstruct the secret. We conclude that a flaw must have been introduced in the generation of the coefficients. 

**Vulnerability**  

Let's look at the generation algorithm of the polynomial in `share-a-saucisse.py`.

``` python
def __init__(self, secret, k):
    self.p = 931620319462745440509259849070939082848977
    self.k = k
    self.polynomial = [ int.from_bytes(secret) ]
    for _ in range(k):
        self.polynomial.append(int.from_bytes(os.urandom(16)))
```

We quickly notice that the secret $`k`$ and the coefficients of the polynomial are generated on `128` bits when $`p`$ has `140` bits. This means that the polynomial has relatively small coefficients compared to a random polynomial in the ambiant ring.

**Exploit**

Let's first introduce some notations, writing
```math
\Pi_i = \frac{\prod_{j\not = i}(X-j)}{\prod_{j\not = i}(i-j)}
```

the secret polynomial can be rewritten as
```math
P = \sum_i P(i)\Pi_i =  k \Pi_0 + S.
```

Because we know all the shares except one, the polynomial $`S`$ can be computed with the known shares. Thus only the secret $`k`$ is unknown in the previous equation. Now, recalling that $`P`$ is a small polynomial mod $`p`$, we expect $`P`$ to be a small vector in the lattice generated by the coefficients of $`\Pi_0`$ and $`S`$. To reduce this problem to finding a short vector in $`\mathbb Z`$, the usual trick is to add the "mod $`p`$" vectors to the lattice. More precisely, writing

```math
\Pi_0 = \sum_{i=0}^{16} a_i X^i \quad \text{and} \quad S = \sum_{i=0}^{16} b_i X^i,
```

we use the lattice spanned by the ligns of the following matrix:

```math
\begin{bmatrix}
a_0 & a_1 & \cdots & a_{16} & 1 & 0\\ 
b_0 & b_1 & \cdots & b_{16} & 0 & 2^{128}\\ 
-p & 0 & \cdots & 0 & 0 & 0 \\
0 & -p &  \cdots & 0 & 0 & 0\\
\vdots &\vdots & \ddots & \vdots & \vdots & \vdots\\
0 & 0 &\cdots & -p & 0 & 0
\end{bmatrix}.
```

Note that we added a penalization to the second vector (coefficients of $`S`$​) so that it is only used once while looking for a short vector of the lattice.

Writing this lattice in sage and using the LLL algorithm allows to recover the secret $`k`$, see [wu.sage](wu.sage) for the details.

