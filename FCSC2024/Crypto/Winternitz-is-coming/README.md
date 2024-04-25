# Winternitz is coming

| Category | Author | Difficulty | Solves | Points |
| -------- | ------ | ---------- | ------ | ------ |
| Crypto   | Maxime | ⭐️⭐️         | 18     | 456    |

> Je vous présente une signature à usage unique inédite et à la pointe de l'innovation ! En plus j'ai bien fait attention à ne signer qu'un seul message. Il ne peut donc rien m'arriver de fâcheux, n'est ce pas ?

**Challenge files:**

- `winternitz-is-coming.py`

## Write-up

**Introduction**

This challenge features a modified version of the Winternitz One-Time Signature (WOTS)  scheme. It's a hash-based signature algorithm which relies on the difficulty of finding a preimage of a hash function. In its most basic form, the idea is to generate a private key $`k`$ which (say) a 256-bit random number and then compute $`\mathcal {PK} :=\mathcal H ^{256} (k) `$ which is going to be the public key. Now, to sign a byte $`m`$, the signer computes $`s := \mathcal H^{m}(k)`$, while the verifier checks if $`\mathcal H^{256-m}(s) \equiv \mathcal{PK}`$. 

As is, this signature scheme is not secure against forgery attacks since an attacker could sign a bigger byte by rehashing the signature, for that reason, a checksum is usually added to make sure that no one can go “further along” a hash chain.

**The challenge**

The endgoal of the challenge is to forge the signature of a message given the signature of `WINTERNITZ IS COMING`. The source code `winternitz-is-coming.py` describes the customized WOTS scheme. Instead of computing iterated hash on the message, the hash is first encoded in a larger space. In the challenge settings, a 20-byte message is encoded onto a 40-byte encoded message with the following function:

``` python
self.Support = [
      8,  17,  26,  32,  52,  53,  57,  58,
     59,  63,  64,  66,  67,  71,  73,  76,
     79,  81, 111, 115, 132, 135, 141, 144,
    151, 157, 170, 176, 191, 192, 200, 201,
    202, 207, 216, 224, 228, 237, 241, 252,
]

def _encoding(self, msg): #len(msg) == 20
    w = [0] * len(self.Support)
    for i in range(len(self.Support)):
        for j in range(len(msg)):
            # Constant coefficient is zero
            w[i] += msg[j] * self.Support[i] ** (j + 1)
        w[i] %= self.W
    return w #len(w) == 40
```

This can be seen as the application of linear matrix $`M`$.

**The attack**

As previously explained, in order to forge a signature, we have to find a message whose encoding bytes are larger than the ones of the signature. If we were to encode random messages, each encoded byte would have roughly one chance over two to be bigger than the one the message, so it would require to encode $`2^{40}`$  random messages which is reasonably expensive. However, in our case the encoded message seems to be biased towards larger bytes and $`2^{84}`$ tries are actually necessary. So a simple bruteforce attack won't work.

However, we can exploit the linear structure of the encoding to make the attack faster. We can divide the lines of $M$ in two groups so that the encoding of a message can be written as:

```math
Mv = \begin{bmatrix} A\\
B\end{bmatrix} v = \begin{bmatrix} Av\\
Bv\end{bmatrix} = w.
```

Now as $A$ (or $B$) are square matrices, they can inverted, so that the upper (or the lower) part of the encoded message can be chosen. This divides the log complexity of forging a signature by a factor of 2. However, we can do better. If we regroup in $A$ the larger bytes of the signature, we will only need to bruteforce the smaller bytes of the signature, this reduces the complexity to $`2^{17}`$, which takes less than a second with the following code.

```python
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
```

Now, using the signature, we can forge a signature by going further in the hash chain. See [wu.sage](wu.sage) for the full code.
