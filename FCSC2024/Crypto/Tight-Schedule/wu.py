from json import loads
from Crypto.Cipher import AES
from Crypto.Util.Padding import unpad

file = "output.txt"
output = loads(open(file, "r").read())
flag_enc = bytes.fromhex(output["flag_enc"])
iv = bytes.fromhex(output["iv"])

key = [108, 8, 214, 214, 44, 206, 38, 83, 11, 63, 34, 179, 76, 64, 153, 90]
key = bytes(key)

E = AES.new(key, AES.MODE_CBC, iv = iv)
print(unpad(E.decrypt(flag_enc), 16).decode())