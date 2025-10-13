# Change Log

v4.1.3

- Addressed compiler warnings on Windows
- CMake changes
- Updated dependencies

v4.1.2

- Minor cleanup in the code; revised comments
- CMake changes to support downstream unit testing
- Updated dependencies

v4.1.1

- Upgraded library dependencies as a part of expanding support for additional
  Unix-like platforms
- Changed how the Logger object is passed to Encryptor and Decryptor
- Replaced std::copy with std::ranges::copy (modernization change)
- Replaced pointers with index values in Decryptor

v4.1.0

- Changed "modulo" in Encryptor to "read_length", as the term is a leftover from
  legacy behavior where a modulo octet was appended to the ciphertext
- Revised comments in functions
- Updated library dependencies
- Use stricter compiler warnings
- Fixed MSVC warnings

v4.0.10

- Updated the following to the latest library version:
  libaes, secutil, libhash, libkdf, and logger.

v4.0.9

- Updated libhash and libkdf dependencies

v4.0.8

- Changes required to build on FreeBSD

v4.0.7

- Updated STF and AES dependencies

v4.0.6

- Updated to latest dependencies

v4.0.5

- Changed the encryption/decryption cancellation logic for consistency
- Updated dependencies (libaes, secutil)

v4.0.4

- Updated to latest dependencies

v4.0.3

- Issue initial progress callback before iterating over the entire stream
  to facilitate rendering by the called application

v4.0.2

- Updated secutil to 1.0.1 for better Linux compatibility
- Updated other library dependencies to align to secutil dependency

v4.0.1

- Simplification of CreateComponent() function

v4.0.0

- Initial release as a part of AES Crypt v4
