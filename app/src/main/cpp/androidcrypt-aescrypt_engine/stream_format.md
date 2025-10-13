# AES Crypt Stream Format Description

AES Crypt data is written to encrypt data streams referred to as the AES
stream format (or, "file format").

Items in quotes are a literal string.  Words outside of quotes are a textual
description of the contents.  Fixed-valued octets are written in hexadecimal
form (e.g., 0x01).

The standard extension page can be found
[here](https://www.aescrypt.com/standard_extensions.html).

The home for this document is located here
[here](https://www.aescrypt.com/aes_stream_format.html).

## AES Crypt Version 3 Stream Format

This stream format is similar to the version 2 format, with the most significant
difference being the input into the Hash Message Authentication Code (HMAC)
functions, which are modified to improve security.  Additionally, the
Key Derivation Function (KDF) employed to generate a key from a password now
uses PBKDF2 with a variable number of iterations using SHA-512 as the hash
function.  Lastly, PKCS#7 is used for padding the final ciphertext block.

```text
 3 Octets - "AES"
 1 Octet  - Version (0x03)
 1 Octet  - Reserved (set to 0x00)
.... Start of repeating extension block section
 2 Octet  - Length in octets (in network byte order) of an extension identifier
            and contents.  If 0x0000, then no further extensions exist and the
            next octet is the start of the Initialization Vector (IV).
            Following an extension, this length indicator would appear again to
            indicate presence or absence of another extension and the size of
            any such extension.
nn Octets - Extension identifier.  This is either a URI or an standard
            identifier documented on the standard extensions page, either of
            which is terminated by a single 0x00 octet.  All extension
            identifiers are case sensitive.  URIs that use domain names,
            for example, must ensure the protocol and host portions are
            lowercase.

                Examples of URIs:
                    http://aescrypt.com/extensions/creator/
                    urn:oid:1.3.6.1.4.1.17090.55.14
                    urn:uuid:85519EA3-1DA6-45b9-9041-8CD368D8C086

                Examples of standard extension identifiers:
                    CREATED_DATE
                    CREATED_BY

            A URI allows anyone to define an extension type without worrying
            about namespace conflicts.

            A special extension is defined that has no name, but is a
            "container" for extensions to be added after the AES stream is
            initially created.  Such an extension avoids the need to read and
            re-write the entire stream in order to add a small extension.
            Software tools that create AES Crypt streams should insert this
            128-octet "container" extension, placing a 0x00 in the first octet
            of the extension identifier field.  Developers may then insert
            extensions into this "container" area and reduce the size of this
            "container" as necessary.  If larger extensions are added or the
            "container" area is filled entirely, then reading and re-writing the
            entire stream would be necessary to add additional extensions.

            Note that extensions are neither encrypted nor authenticated.
            They may provide useful information, but they must not be relied
            upon by software to contain accurate information.
nn Octets - The contents of the extension
.... End of repeating extension block section
 4 octets - KDF iteration value (network byte order)
16 Octets - Initialization Vector (IV) used to encrypt the Session IV and
            Session Key
48 Octets - Encrypted Session IV and 256-bit AES Session Key used to encrypt the
            stream, PBKDF2-derived key using HMAC-SHA512
            (IV = 16 octets; Key = 32 octets)
32 Octets - HMAC-SHA256(48-octet IV and KEY || 0x03)
nn Octets - Ciphertext (AES-256 in CBC mode; PKCS#7 padded; 2^64 octets max)
32 Octets - HMAC-SHA256(nn octets)
```

The footprint of the stream is at least 155 octets.

## AES Crypt Version 2 Stream Format

```text
 3 Octets - "AES"
 1 Octet  - Version (0x02)
 1 Octet  - Reserved (set to 0x00; modulo moved to end of stream)
.... Start of repeating extension block section
 2 Octet  - Length in octets (in network byte order) of an extension identifier
            and contents.  If 0x0000, then no further extensions exist and the
            next octet is the start of the Initialization Vector (IV).
            Following an extension, this length indicator would appear again to
            indicate presence or absence of another extension and the size of
            any such extension.
nn Octets - Extension identifier.  This is either a URI or an identifier
            documented on the standard extensions page, either of which is
            terminated by a single 0x00 octet.  All extension identifiers are
            case sensitive.  URIs that use domain names, for example, must
            ensure the protocol and host portions are lowercase.

                Examples of URIs:
                    http://aescrypt.com/extensions/creator/
                    urn:oid:1.3.6.1.4.1.17090.55.14
                    urn:uuid:85519EA3-1DA6-45b9-9041-8CD368D8C086

                Examples of standard extension identifiers:
                    CREATED_DATE
                    CREATED_BY

            A URI allows anyone to define an extension type without worrying
            about namespace conflicts.

            A special extension is defined that has no name, but is a
            "container" for extensions to be added after the AES stream is
            initially created.  Such an extension avoids the need to read and
            re-write the entire stream in order to add a small extension.
            Software tools that create AES Crypt streams should insert this
            128-octet "container" extension, placing a 0x00 in the first octet
            of the extension identifier field.  Developers may then insert
            extensions into this "container" area and reduce the size of this
            "container" as necessary.  If larger extensions are added or the
            "container" area is filled entirely, then reading and re-writing the
            entire stream would be necessary to add additional extensions.

            Note that extensions are neither encrypted nor authenticated.
            They may provide useful information, but they must not be relied
            upon by software to contain accurate information.
nn Octets - The contents of the extension
.... End of repeating extension block section
16 Octets - Initialization Vector (IV) used to encrypt the Session IV and
            Session Key
48 Octets - Encrypted Session IV and AES Session Key used to encrypt the stream
            (IV = 16 octets; Key = 32 octets)
32 Octets - HMAC-SHA256(48-octet IV and KEY)
nn Octets - Ciphertext (AES-256 in CBC mode; 2^64 octets max)
 1 Octet  - Ciphertext size modulo 16 in least significant bit positions
32 Octets - HMAC-SHA256(nn octets)
```

The footprint of the stream is at least 136 octets.

## AES Crypt Version 1 Stream Format

```text
 3 Octets - "AES"
 1 Octet  - Version (0x01)
 1 Octet  - Reserved (set to 0x00; modulo moved to end of stream)
16 Octets - Initialization Vector (IV) used for encrypting the IV and symmetric
            key that is actually used to encrypt the bulk of the plaintext.
48 Octets - Encrypted IV and 256-bit AES key used to encrypt the bulk of the
            stream (IV = 16 octets; Key = 32 octets)
32 Octets - HMAC-SHA256
nn Octets - Ciphertext (AES-256 in CBC mode; 2^64 octets max)
 1 Octet  - Ciphertext size modulo 16 in least significant bit positions
32 Octets - HMAC-SHA256
```

The footprint of the stream is at least 134 octets.

## AES Crypt Version 0 Stream Format

```text
 3 Octets - "AES"
 1 Octet  - Version (0x00)
 1 Octet  - Ciphertext size modulo 16 in least significant bit positions
16 Octets - Initialization Vector (IV)
nn Octets - Ciphertext (AES-256 in CBC mode; 2^64 octets max)
32 Octets - HMAC-SHA256
```

The footprint of the stream is at least 53 octets.
