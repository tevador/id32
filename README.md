ID32 provides a human-readable representation of a hash value. It is suitable as an identifier for self-generated cryptographic keys, such as:

* server SSH keys
* personal public keys
* self-signed certificates

Here are some examples of ID32 values:

```
xrpz9-zf7c8-xjfsr-ygdj4-duxei
obi3a-sdhti-hk3db-fg5od-abmmi
ji94w-upgod-ne1f7-157n9-rm3p6
885m3-xdhuj-3r6mx-t74io-ow5pa
iefux-k1rnn-mbmt7-kzn3d-rpdct
nfp4w-dcxg3-o6dk6-magjc-mqtth
4jop4-7mwji-udswz-3dsuf-cdz58
kq71i-z71w6-ckx39-dbdxu-1bskw
yw13x-u41x9-f5hk7-o5hqb-re9rh
e67es-xwddw-zskpg-mnwdd-pywj9
```

#### Character set

ID32 uses the [z-base-32](https://philzimmermann.com/docs/human-oriented-base-32-encoding.txt) character set.

#### Security

ID32 provides 120 bits of preimage resistance. It should not be used when collision resistance is required. However, for self-generated cryptographic keys, users typically have nothing to gain from generating two keys that result in the same identifier.

#### Checksum

ID32 contains a checksum character which can detect:

* all single substitution errors (e.g. `a` → `s`)
* all single transposition errors (e.g. `we` → `ew`, including jump transpositions like `rew` → `wer`)
* all twin errors (e.g. `dd` → `ff`, including jump twin errors like `dfd` → `gfg`)
* 97% of other errors

### Build

```
git clone https://github.com/tevador/id32.git
cd id32
mkdir build
cd build
cmake ..
make
```

Refer to the [id32.h](include/id32.h) header file for API documentation. ID32 must be used with a hash function that can output at least 128 bits. Hash functions from the SHA2, SHA3 or Blake2 families are recommended.
