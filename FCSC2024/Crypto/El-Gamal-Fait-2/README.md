# El Gamal Fait 2/2

| Category | Author | Difficulty | Solves | Points |
| -------- | ------ | ---------- | ------ | ------ |
| Crypto   | JD     | ⭐️          | 50     | 381    |

> On m’a dit que dans certains cas, mon service de signature qui utilise ElGamal est encore plus facile à attaquer. À vous de me montrer ça !

**Challenge files:**

- `el-gamal-fait-2.py`

## Write-up

**Introduction**

This challenge is the follow-up of the challenge El Gamal Fait 1/2. The core functions of `el-gamal-fait-2.py` are identical except for the function `generate`:

```python
def generate(bits):
    p = 2
    while p % 4 != 1:
        p = getPrime(bits)
    x = randrange(p)
    g = 2
    y = pow(g, x, p)
    return p, g, x, y
```

The prime `p` is chosen to have reminder `1 mod 4` and the "generator" is chosen to be `g = 2`. 

The challenge is also more complex :

```  python
try:
    m = randrange(p)
    print(f"Your task is to sign the following message m = {m}")
```

we are asked to forge a signature for a specific message which seems harder (the previous challenge was giving us the option of the message `m`).

**Bad key generation**

Again, we can have a look at the [wikipedia page](https://en.wikipedia.org/wiki/ElGamal_encryption#Key_generation) and compare the key generation algorithm. The main insight in the key generation procedure is that we have to 

> Generate an efficient description of cyclic group $`G`$ of order $`q`$ with generator $`g`$.

Here, our group is obviously the multiplicative group of integers modulo `p` with `g=2`. It's indeed a cylclic group but are we sure that `2` is a generator of that group? 

Proving that a given number is a generator of a multiplicative group is actually a tricky question. To determine that, we have to find a full factorization of $`p-1 = \prod p_i^{r_i}`$. Then the desired necessary and sufficient condition is that $`g^{(p-1)/p_i} \mod p \not = 1`$ for all $`p_i`$.

However checking that a number is NOT a generator only requires to find a single $p_i$ such that $`g^{(p-1)/p_i} \mod p = 1`$. So we can obviously check that condition for $`p_i = 2`$​. 

Thus $`g`$ will not be a generator if $`g^{(p-1)/2} \mod p = 1`$, and this actually happens half the time. (The possible value of $`g^{(p-1)/2} \mod p`$ are $`1`$ and $`-1`$ and their preimages are in bijection by a sign flip).

**Subgroup attack**

Recall that forging a signature for `m`  requires finding $r, s$ such that

```math
g^m = y^r r^s \mod p,
```

where $`y`$ is an unknown power of $`g`$. The trick is to choose $`r = (p-1)/2`$, assuming that $`2^{(p-1)/2} \mod p = 1`$ so the previous equation can be rewritten as 

```math
2^m = ((p-1)/2)^s \mod p,
```

thus 

```math
2^{m + s} = (-1)^s \mod p.
```

Now if we choose $`s=-m \mod (p-1)`$, the previous equation will be satisfied if $`s`$ (or $`m`$) is even so half the time.

**Conclusion**

So the previous attack works one fourth the time. We had to crucially combine the fact that the generation algorithm was not making sure that `g` was a generator with the fact it was set to `2`.
