# El Gamal Fait 1/2

| Category | Author | Difficulty | Solves | Points |
| -------- | ------ | ---------- | ------ | ------ |
| Crypto   | JD     | ⭐️          | 131    | 227    |

> On m’a dit qu’on pouvait utiliser RSA pour signer des messages au lieu de les chiffrer. J’ai essayé de faire pareil avec ElGamal. Est-ce que vous pouvez vérifier si ce que j’ai fait est sécurisé ?

**Challenge files:**

- `el-gamal-fait-1.py`

## Write-up

**Introduction**

This challenge features an implementation of the ElGamal signature scheme. We are asked to forge a signature tp get the flag.

**Existential forgery**

The signature implementation is close to what's presented in [wikipedia](https://en.wikipedia.org/wiki/ElGamal_signature_scheme#Signing)

```python
def sign(p, g, x, m):
    k = randrange(p)
    r = pow(g, k, p)
    inv_k = pow(k, -1, p - 1)
    s = ((m - x * r) * inv_k) % (p - 1)
    return r, s
```

… except that the message `m` is not hashed prior to its signature. This induces a classic attack which was discovered by David Pointcheval and Jacques Stern, see [wikipedia](https://en.wikipedia.org/wiki/ElGamal_signature_scheme#Existential_forgery) and [script](wu.sage) for implementation.