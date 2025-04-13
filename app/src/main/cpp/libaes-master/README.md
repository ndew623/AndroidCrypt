# Advanced Encryption Standard Library

This library implements the AES block cipher (FIPS 197) and AES Key Wrap
(IETF RFC 3394) and AES Key Wrap with Padding (IETF RFC 5649).

For Intel processors that support the AES-NI instructions, this library will
make use of those instructions for speed benefits.

## AES Usage

To encrypt or decrypt data, one creates an `AES` object and calls functions
to perform those operations.  The following is an example of how to encrypt
a block of data:

```cpp
// Pass the key of the desired length as an argument to the constructor
AES aes(key);

// Encrypt data
aes.Encrypt(plaintext, ciphertext);
```

AES supports 128, 192, and 256-bit key lengths, all of which are supported
by this library.  There are additional functions to set (or re-set) the
encryption key, so it's not required to provide the key exclusively via the
constructor.

Text is encrypted or decrypted "in place", meaning that the memory pointed
to by the `plaintext` and `ciphertext` arguments in the above
examples _may_ refer to the same memory location.

## AESKeyWrap Usage

The `AESKeyWrap` object implements the specifications that are designed
to allow one to encrypt a key that is subsequently used for encryption.
There are wrapping procedures implemented with and without padding.

The following is an example of the usage of `AESKeyWrap`:

```cpp
// Create AESKeyWrap object using the given key and length
AESKeyWrap aes_kw(key);

// Perform the AES Key Wrap
aes_kw.Wrap(plaintext, ciphertext);
```

The `plaintext` in this example would be the encryption key to wrap and the
`ciphertext` would be the wrapped (encrypted) key.  Note that the output of
the wrapping operation will be longer than the original input.  This is to
hold integrity data that is used to determine that the wrapped key data was
modified when `Unwrap()` is called.  Refer to the `Wrap()` function definition
for more details.
