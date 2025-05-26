/*
 *  pbkdf.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines function prototypes for various password-based
 *      Key Derivation Functions (KDFs).
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <span>
#include <cstdint>
#include <cstddef>
#include <terra/crypto/hashing/hash.h>
#include <terra/crypto/kdf/kdf_exception.h>

namespace Terra::Crypto::KDF
{

/*
 *  ACKDF()
 *
 *  Description:
 *      This function implements the AES Crypt Password-Based Key Derivation
 *      Function (KDF) used in AES Crypt versions 0, 1, and 2.
 *
 *  Parameters:
 *      algorithm [in]
 *          The hashing algorithm to employ.  The hashing algorithm used by
 *          AES Crypt version 0, 1, and 2 was SHA-256.
 *
 *      password [in]
 *          The password to be used as input into the KDF.  The password used
 *          by AES Crypt to v2 was UTF-16LE, though this routine does not demand
 *          that particular character encoding.
 *
 *      salt [in]
 *          A salt value to be used by this algorithm.  In older versions of
 *          AES Crypt, this was a 16-octet random value.  This may be any
 *          span of octets of length zero up to the length of the hash function
 *          message digest length.  For example, SHA-256 produces a 32-octet
 *          message digest.  Thus, the salt may be up to 32 octets.
 *
 *      iterations [in]
 *          The number of times the hashing function should be invoked.  The
 *          value used by AES Crypt up to version 2 was 8192.  This must be 1
 *          or greater.
 *
 *      key [out]
 *          This is the span of octets into which the derived key is written.
 *          The length of the span must be at least as large as the length of
 *          the output from the employed the hashing algorithm.
 *
 *  Returns:
 *      A span over the same span as the key parameter and having the length
 *      set according to the actual key length, which will be the length of
 *      the output from the hashing algorithm.  An exception will be thrown
 *      if the salt length is too large.
 *
 *  Comments:
 *      This algorithm is similar to that defined in PBKDF1 (see RFC 8018
 *      section 5.1). However, the difference in this algorithm and PBKDF1
 *      is that the input password is hashed repeatedly with the output of
 *      the previous hash iteration.  Also, the order in which the password
 *      and salt are given in the initial iteration is reversed.
 */
std::span<std::uint8_t> ACKDF(Hashing::HashAlgorithm algorithm,
                              const std::span<const std::uint8_t> password,
                              const std::span<const std::uint8_t> salt,
                              std::size_t iterations,
                              const std::span<std::uint8_t> key);

/*
 *  ACKDF()
 *
 *  Description:
 *      This function implements the AES Crypt Password-Based Key Derivation
 *      Function (KDF) used in AES Crypt versions 0, 1, and 2.
 *
 *  Parameters:
 *      password [in]
 *          The password to be used as input into the KDF.  The password used
 *          by AES Crypt to v2 was UTF-16LE, though this routine does not demand
 *          that particular character encoding.
 *
 *      salt [in]
 *          A salt value to be used by this algorithm.  In older versions of
 *          AES Crypt, this was a 16-octet random value.  This may be any
 *          span of octets of length zero up to the length of the hash function
 *          message digest length.  For example, SHA-256 produces a 32-octet
 *          message digest.  Thus, the salt may be up to 32 octets.
 *
 *      key [out]
 *          This is the span of octets into which the derived key is written.
 *          The length of the span must be at least as large as the length of
 *          the output from the employed the hashing algorithm.
 *
 *  Returns:
 *      A span over the same span as the key parameter and having the length
 *      set according to the actual key length, which will be the length of
 *      the output from the hashing algorithm.  An exception will be thrown
 *      if the salt length is too large.
 *
 *  Comments:
 *      This algorithm is similar to that defined in PBKDF1 (see RFC 8018
 *      section 5.1). However, the difference in this algorithm and PBKDF1
 *      is that the input password is hashed repeatedly with the output of
 *      the previous hash iteration.  Also, the order in which the password
 *      and salt are given in the initial iteration is reversed.
 *
 *      This version of the function calls the above function utilizing
 *      the SHA-256 algorithm with 8192 iterations, as those were the defaults
 *      employed by AES Crypt Version 0, 1, and 2.
 */
std::span<std::uint8_t> ACKDF(const std::span<const std::uint8_t> password,
                              const std::span<const std::uint8_t> salt,
                              const std::span<std::uint8_t> key);

/*
 *  PBKDF1()
 *
 *  Description:
 *      This function implements the Password-Based Key Derivation Algorithm
 *      PBKDF1 defined in IETF RFC 8018 Section 5.1.
 *
 *  Parameters:
 *      algorithm [in]
 *          The hashing algorithm to employ.
 *
 *      password [in]
 *          The password to be used as input into the KDF.
 *
 *      salt [in]
 *          A salt value to be used by this algorithm.
 *
 *      iterations [in]
 *          The number of times the hashing function should be invoked.
 *          This must be at least 1.
 *
 *      key [out]
 *          This is the span of octets into which the derived key is written.
 *          The length of the span must be any value between 0 and the length
 *          of the output from the employed the hashing algorithm.
 *
 *  Returns:
 *      A span over the same span as the key parameter and having the length
 *      set according to the actual key length, which may be smaller than the
 *      key parameter.  The length will be the minimum of the key span size
 *      and the hash output size.
 *
 *  Comments:
 *      None.
 */
std::span<std::uint8_t> PBKDF1(Hashing::HashAlgorithm algorithm,
                               const std::span<const std::uint8_t> password,
                               const std::span<const std::uint8_t> salt,
                               std::size_t iterations,
                               const std::span<std::uint8_t> key);

/*
 *  PBKDF2()
 *
 *  Description:
 *      This function implements the Password-Based Key Derivation Algorithm
 *      PBKDF2 defined in IETF RFC 8018 Section 5.2.
 *
 *  Parameters:
 *      algorithm [in]
 *          The hashing algorithm to employ.  This will be transformed into
 *          HMAC_{algorithm}.
 *
 *      password [in]
 *          The password to be used as input into the KDF.
 *
 *      salt [in]
 *          A salt value to be used by this algorithm.
 *
 *      iterations [in]
 *          The number of times the hashing function should be invoked.
 *          This must be at least 1.
 *
 *      key [out]
 *          This is the span of octets into which the derived key is written.
 *          The length of the span must be any value between 0 and the length
 *          of the output from the employed the hashing algorithm * (2^32 - 1).
 *
 *  Returns:
 *      A span over the same octets as key if successful.  If there is an error,
 *      an exception will be thrown.
 *
 *  Comments:
 *      None.
 */
std::span<std::uint8_t> PBKDF2(Hashing::HashAlgorithm algorithm,
                               const std::span<const std::uint8_t> password,
                               const std::span<const std::uint8_t> salt,
                               std::size_t iterations,
                               const std::span<std::uint8_t> key);

} // namespace Terra::Crypto::KDF
