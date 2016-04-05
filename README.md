#fastecdsa
[![Build Status](https://travis-ci.org/AntonKueltz/fastecdsa.svg?branch=master)](https://travis-ci.org/AntonKueltz/fastecdsa)

## About
This is a python package for doing fast elliptic curve cryptography, specifically
digital signatures.

## Supported Primitives
#### Curves
* P192
* P224
* P256
* P384
* P521
* secp256k1 (bitcoin curve)

#### Hash Functions
* SHA1
* SHA224
* SHA256 (SHA2)
* SHA384
* SHA512

## Performance
Currently it does basic point multiplication significantly faster than the `ecdsa`
package. You can see the times for 1,000 signature and verification operations below,
 `fast.py` corresponding to this package and `regular.py` corresponding to `ecdsa`
 package.

![Performance](http://i.imgur.com/ZH8Oodm.png)

As you can see, this package in this case is ~25x faster.

## Installing
Run `python setup.py install` (or `pip install -e .`). Note that you need to have C compiler that
supports the C99  standard. You also need to have [GMP](https://gmplib.org/) on
your system as the underlying C code in this package includes the `gmp.h` header
(and links against gmp via the `-lgmp` flag). On debian you can install all
dependencies as follows:

```
sudo apt-get install python-dev libgmp3-dev
```

## Usage
#### signing and verifying signatures via ECDSA
```python
from fastecdsa import curve, ecdsa
from hashlib import sha384

keys = ecdsa.KeyPair(curve=curve.P256)  # use NIST curve P256
m = "a message to sign via ECDSA"  # some message

(r, s) = keys.sign(m)  # standard signature, returns two integers
valid = keys.verify((r, s), m)  # should return True as the signature we just generated is valid.

''' specify a different hash function to use with ECDSA '''
keys = ecdsa.KeyPair(curve=curve.P256, hashfunc=sha384)  # use NIST curve P256 with SHA384
(r, s) = keys.sign(m)
valid = keys.verify((r, s), m)
```

## Security
No known current issues. Timing side challenges are prevented via Montgomery point multiplication. Nonces are generate per RFC6979.
