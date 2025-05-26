/*
 *  ackdf.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the Password-Based Key Derivation Function (KDF)
 *      used by AES Crypt version 0, 1, and 2.
 *
 *  Portability Issues:
 *      None.
 */

#include <cstring>
#include <cstdint>
#include <vector>
#include <terra/secutil/secure_vector.h>
#include <terra/crypto/hashing/hash.h>
#include <terra/crypto/kdf/pbkdf.h>

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
                              const std::span<std::uint8_t> key)
{
    // Create the HMAC object
    Hashing::HashPointer hash = Hashing::CreateHashObject(algorithm);
    std::size_t hash_length = hash->GetDigestLength();

    // Verify that the key span is sufficiently long
    if (key.size() < hash_length)
    {
        throw KDFException("Output key span is too short");
    }

    // Ensure the iterations is not zero
    if (iterations == 0) throw KDFException("Iteration count cannot be zero");

    // Ensure the salt is not too long
    if (salt.size() > hash_length) throw KDFException("Salt value is too long");

    // Create a vector for the hash result (used in each iteration)
    SecUtil::SecureVector<std::uint8_t> hash_result(hash_length, 0);

    // Place the salt at the left of the hash_result vector
    std::memcpy(hash_result.data(), salt.data(), salt.size());

    // Hash the hash result and password "iterations" times
    for (std::size_t i = 0; i < iterations; i++)
    {
        hash->Input(hash_result);
        hash->Input(password);
        hash->Finalize();
        hash->Result(hash_result);
        hash->Reset();
    }

    // Place the derived key into the output key span
    std::memcpy(key.data(), hash_result.data(), hash_length);

    return key.first(hash_length);
}

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
                              const std::span<std::uint8_t> key)
{
    return ACKDF(Hashing::HashAlgorithm::SHA256, password, salt, 8192, key);
}

} // namespace Terra::Crypto::KDF
