This is a simple peer to peer chat client using RakNet.

The NAT punchthrough client plugin of RakNet was used so two peers behind NAT
can hopefully communicate to each other.

The code uses Jenkins Software's free of charge NAT punchthrough server for
RakNet. However, you can also build and run your own NAT punchthrough server.

RakNet gives each peer a unique GUID. Once the peers which are behind NAT are
connected to The NAT punchthrough server(which has a known address), they  use
these GUID's to make NAT punchthrough attempts to each other. Once the NAT
punchthrough is successfull, the peers send each other packets with the
ID\_NAT\_PUNCHTHROUGH\_SUCCEEDED message identifier from their public addresses.
Finally it is possible to make a connection to the remote peer using the address
of the received packet.

**Note:** Don't forget to clone the RakNet source which is added as a submodule:

```
git submodule init
git submodule update
```

