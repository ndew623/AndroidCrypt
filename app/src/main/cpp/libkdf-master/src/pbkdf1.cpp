/*
 *  pbkdf1.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the Password-Based Key Derivation Function (KDF)
 *      as specified in RFC 8018 Section 5.1.
 *
 *  Portability Issues:
 *      None.
 */

#include <cstring>
#include <cstdint>
#include <vector>
#include <terra/secutil/secure_vector.h>
#include <terra/secutil/secure_erase.h>
#include <terra/crypto/hashing/hash.h>
#include <terra/crypto/kdf/pbkdf.h>

namespace Terra::Crypto::KDF
{

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
                               const std::span<std::uint8_t> key)
{
    // If the key span size is zero, there is no work to do
    if (key.empty()) return key.first(0);

    // Ensure the iterations is not zero
    if (iterations == 0) throw KDFException("Iteration count cannot be zero");

    // Create the hashing object
    Hashing::HashPointer hash = Hashing::CreateHashObject(algorithm);
    std::size_t hash_length = hash->GetDigestLength();

    // Create a vector for the hash result (used in each iteration)
    SecUtil::SecureVector<std::uint8_t> hash_result(hash_length);

    // The first iteration is different from the others
    hash->Input(password);
    hash->Input(salt);
    hash->Finalize();
    hash->Result(hash_result);
    hash->Reset();

    // Perform the subsequent iterations
    for (std::size_t i = 1; i < iterations; i++)
    {
        hash->Input(hash_result);
        hash->Finalize();
        hash->Result(hash_result);
        hash->Reset();
    }

    // Place the derived key into the output key span
    std::size_t actual_key_length = std::min(hash_length, key.size());
    std::memcpy(key.data(), hash_result.data(), actual_key_length);

    // Erase local variable
    SecUtil::SecureErase(hash_length);

    return key.first(actual_key_length);
}

} // namespace Terra::Crypto::KDF
