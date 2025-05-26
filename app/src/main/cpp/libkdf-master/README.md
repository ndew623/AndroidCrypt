# Key Derivation Function Library

This library contains various Key Derivation Functions.

## Standalone functions

Functions implemented in this library:

* ACKDF - The Password-based KDF implemented for AES Crypt version 0
  and used through version 2.
* PBKDF1 - Password-based KDF function 1 as defined in RFC 8018 Section 5.1.
* PBKDF2 - Password-based KDF function 2 as defined in RFC 8018 Section 5.2.

## HMAC-based Extract-and-Expand Key Derivation Function (HKDF)

Additionally, this library implements the "HMAC-based Extract-and-Expand Key
Derivation Function (HKDF)" defined in RFC 5869.  This is also referenced
in NIST SP800-56Cr2 as a type of "Key-Derivation Methods in Key-Establishment
Schemes".  That logic is implemented in an object called, appropriately, HKDF.
