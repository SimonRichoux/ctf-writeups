import bisect
from Crypto.Util.number import long_to_bytes
from tqdm import tqdm

n = 32317006071311007300714876688669951960444102669715484116646415746394635540935921521724498858714153019942728273207389351392489359055364847068112159175081645180751388138858639873796530441238955101568733568176426113140426646976937052525918530731769533330028543775101205176486450544323218351858348091394877603920443607214945787590258064663918917740728042628406529049644873974905470262922605695823668958842546472908325174694333334844222696779786423580284135591089906934014814106737656354549850191740172203279085863986305338341130808123659535483768004216490879857791785965848808238698345641038729244197829259714168491081729
e = 65537
c = 0x00bae65fbca88fea74202821d1773fa90e1a6e3532b3e60f1516ad8e04c84c1c42b733206d5b10bfeada9facd35adc426234b31183398adc4d0e842a3a2d09756f9e0bcdfdfe5b553dab4b21ea602eba4dc7db589f69360e1a598048ea0b719e7ab3ca25dec80acdaec582140877da1ce4c912425c43b1e19757309c2383b3b48ebbfcdac5bddfa167bbf1f7a31ec2a7758a52579956600306ca0dab86d5b37d3a7dfc9429a757f978905c01e46bd6d7c32f314a5916107545ad1cb17d76962b4ac11bbb6020a3ff0175d72081cc47cfd486ff05ed8799e2dd0991ce7b4f4ba2f2eae9dbddecc43e9d7a3899f6b493a839d5be7f9fe856fbe238e10047a7ad2945

KEEP = 1500

# taken from https://sectt.github.io/writeups/GoogleCTF20/crypto_yafm/README
def find_sols_Kplusone(solsK, k):
    global n
    solsk = []
    seen = {}

    n_mod_k = n % 2**k
    for weight, p,q in solsK:
        p2 = p + 2**k
        q2 = q + 2**k
        n1 = p*q % 2**k
        n2 = p2*q % 2**k
        n3 = p*q2 % 2**k
        n4 = p2*q2 % 2**k
    
        if n1 == n_mod_k and (p,q) not in seen:
            bisect.insort(solsk, ((weight, p,q)))
            seen[(p,q)] = True
            seen[(q,p)] = True
        
        if n2 == n_mod_k and (p,q2) not in seen:
            bisect.insort(solsk, ((weight+1, p,q2)))
            seen[(p,q2)] = True
            seen[(q2,p)] = True
        
        if n3 == n_mod_k and (p2,q) not in seen:
            bisect.insort(solsk, ((weight+1, p2,q)))
            seen[(p2,q)] = True
            seen[(q,p2)] = True
        
        if n4 == n_mod_k and (p2,q2) not in seen:
            bisect.insort(solsk, ((weight+2, p2,q2)))
            seen[(p2,q2)] = True
            seen[(q2,p2)] = True
        
    return solsk[:KEEP]

sols = [(2,1,1)]
for k in tqdm(range(1, 1025)):
    sols = find_sols_Kplusone(sols, k)

for _,p,q in sols:
    if n % p == 0:
        p, q = p, n//p 
        break
    if n % q == 0:
        p, q = n//q, q
        break

try:
    assert n == p * q
    d = pow (e, -1, (p-1)*(q-1))
    print(long_to_bytes(pow (c, d, n)).split(b'\x00')[-1].decode())
except:
    print("attack failed")