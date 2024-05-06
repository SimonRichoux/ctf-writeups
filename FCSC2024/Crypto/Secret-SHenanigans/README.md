

# Secret SHenanigans

| Category | Author | Difficulty | Solves | Points |
| -------- | ------ | ---------- | ------ | ------ |
| Crypto   | rbe    | ⭐️⭐️⭐️        | 10     | 477    |

> Un malfrat, versé dans la technique, se méfie des protocoles de canaux sécurisés éprouvés comme SSH ou TLS : “ils sont tous certainement backdoorés” pense t’il. Comme il doit régulièrement recevoir des informations confidentielles de collègues à lui, il a développé son propre protocole d’établissement de canal sécurisé permettant à des clients anonymes d’envoyer à son serveur ces informations confidentielles.
>
> Vous avez accès à un TAP réseau vous positionnant en Man-in-the-Middle entre un client et le serveur, et vous avez par ailleurs réussi à dérober le code source de l’application (sans la clé privée du serveur malheureusement …). Saurez-vous récupérer le secret que le client envoie au serveur ?

**Challenge files:**

- `secret-shenanigans.tar.xz`
- `README.md`
- `docker-compose.yml`

## Write-up

**Introduction**

The challenge asks us to attack a SSH-like protocol. The challenge settings allow us to mount a man-in-the-middle attack against the protocol so we are allowed to listen and/or modify the packets sent by the two parties. 

The source code in the archive  `secret-shenanigans.tar.xz` details the protocol. A high-level view of it is the following:

- First a symmetric key is established between the two parties (say Alice & Bob) using an authentified [Diffie-Hellman key exchange](https://en.wikipedia.org/wiki/Diffie–Hellman_key_exchange). The authentification is ensured by a public key and a ECC-based signature algorithm.
- Then, communication continues using an [Encrypt-then-MAC](https://en.wikipedia.org/wiki/Authenticated_encryption#Encrypt-then-MAC_(EtM)) authenticated encryption with established keys.
- Finally, communication ends after Alice sends the encrypted flag to Bob. 

As in, the authenticated symmetric key exchange seems to be well implemented so it does not seem possible to mount the classical [man-in-the-middle attack](https://en.wikipedia.org/wiki/Diffie–Hellman_key_exchange#Security) against DH. Therefore, there's not much hope that we will be able to read the encrypted flag at the end of the protocol. However, Alice allows Bob to switch to an unencrypted channel if Bob asks Alice to do so:

```python
while True:
    if secure_channel:
        cmd, payload, recv_packet_counter, iv_recv = sc_recv_packet(send, recv, kenc_recv, kmac_recv, recv_packet_counter, iv_recv)
    else:
        cmd, payload, recv_packet_counter = clear_recv_packet(send, recv, recv_packet_counter)

    # [...]
    
    elif cmd == CMD_DISABLE_SC:
        # This can only be excuted when the secure channel is set
        logger.warning("[CLIENT] CMD_DISABLE_SC received")
        if not secure_channel:
            logger.error("[CLIENT] Error: CMD_DISABLE_SC while secure is not active. Aborting.")
            thread_exit()
        # Disable the secure channel
        secure_channel = False
        continueived")
            if not secure_channel:
                logger.error("[CLIENT] Error: CMD_DISABLE_SC while secure is not active. Aborting.")
                thread_exit()
            # Disable the secure channel
            secure_channel = False
            continue
```

So if we find a clever way to trigger the command `CMD_DISABLE_SC`, we will be able to read the flag in clear. However, this command requires Bob to send that packet to Alice while the secure channel mode is set … so we will need to understand how one can modify Bob's packet to send that command. 

**Packet structure**

First, let's delve into the implementation of `sc_recv_packet` (and `sc_send_packet`) to understand how packets are sent in the secure channel. The plain payload has the the following structure:

```
cmd || len || payload || padding
```

`cmd` will be read by the receiver as above and some padding is used to ensure the payload has a size proportional to `AES.block_size`. 

The payload is then encrypted using an AES-CBC cipher where the `iv` used is set before the message is sent. The packet's payload has the final form:

```
len || enc_payload || HMAC
```

where the HMAC ensures the integrity of the length of `ENC_PAYLOAD || HMAC` (equal to `len`), the encrypted payload  `enc_payload` and the `packet_counter`.

Finnaly, the new `iv` is set to be the last bytes of `enc_payload`. 

The receiver obviously reverses the previous steps to read `cmd, payload` and update ` recv_packet_counter, iv_recv`.  

**Packet number manipulation**

The basic idea of the attack is to manipulate the `iv` used to decrypt the messages. Because it's directly linked to the previous messages, if we can manipulate the previous packet sent, it will complety change the decryption of `enc_payload`. With some luck, the decrypted byte `cmd` will be equal to `CMD_DISABLE_SC` so we  will win. 

However, we should not forget that the `packet_counter` is authentified so we always want to make sure that Alice's value of  `packet_counter` matches the one of Bob.

```sequence
Alice->Charlie: Hello Bob, how are you?
Note right of Charlie: Bob thinks
Charlie-->Bob: I am good thanks!
Alice->Bob: yes

```
