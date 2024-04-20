# High-level overview of the TAP

The TAP is set between a client and a server, and it "freezes" the two connection channels
(from server to client and from client to server) whenever one of the two parties sends a packet.
When in TAP mode, it is possible for the attacker to either:

- Read the two communication buffers using 'R0' (from server to client) or 'R1' (from client to server).
The output is the hexadecimal value of the buffer.
- Edit these two communication buffers using 'E0' and 'E1' with an hexadecimal value.
- Activate the "always transparent mode from now on" to avoid TAP invocation further away, using 'T'.
- Quit this TAP hook using 'Q', the TAP hook will be spawn again at the next server or client packet
sending action (except if the "always transparent mode" has been set up earlier).

Beware that tampering with the communication channels can freeze the client and the server, or
make them panic: use with care :-)
