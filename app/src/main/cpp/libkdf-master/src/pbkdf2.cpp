/*
 *  pbkdf2.cpp
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
 *      as specified in RFC 8018 Section 5.2.
 *
 *  Portability Issues:
 *      None.
 */

#include <cstring>
#include <cstdint>
#include <vector>
#include <limits>
#include <terra/secutil/secure_vector.h>
#include <terra/crypto/hashing/hmac.h>
#include <terra/crypto/hashing/hash.h>
#include <terra/crypto/kdf/pbkdf.h>

namespace Terra::Crypto::KDF
{

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
                               const std::span<std::uint8_t> key)
{
    // Maximum number of blocks allowed by PBKDF2
    constexpr std::size_t max_blocks =
        std::numeric_limits<std::uint32_t>::max();

    // If the key span size is zero, there is no work to do
    if (key.empty()) return key.first(0);

    // Ensure the iterations is not zero
    if (iterations == 0) throw KDFException("Iteration count cannot be zero");

    // Create the PRF (HMAC_{algorithm})
    Hashing::HMAC hmac(algorithm, password);

    // Get the length of the hashing function output
    std::size_t hash_length = hmac.GetHMACLength();

    // Create a vector for the hash result (used repeatedly)
    SecUtil::SecureVector<std::uint8_t> hash_result(hash_length);

    // Vector to hold T_i computation
    SecUtil::SecureVector<std::uint8_t> T_i(hash_length);

    // Vector to hold the value of i in network byte order
    SecUtil::SecureVector<std::uint8_t> int32_big_endian(4);

    // Determine number of hash-length blocks
    std::size_t blocks = key.size() / hash_length;
    if ((key.size() % hash_length) > 0) blocks++;

    // Determine the size of the final block
    std::size_t remainder = key.size() - ((blocks - 1) * hash_length);

    // Ensure the derived key length is not too long
    if (blocks > max_blocks)
    {
        throw KDFException("Requested key length is too large");
    }

    // Get a pointer to the start of the span
    std::uint8_t *p = key.data();

    // Iterate over possible blocks
    for (std::size_t block = 1; block <= blocks; block++)
    {
        // Put i into network byte order
        int32_big_endian[0] = (block >> 24) & 0xff;
        int32_big_endian[1] = (block >> 16) & 0xff;
        int32_big_endian[2] = (block >>  8) & 0xff;
        int32_big_endian[3] = (block      ) & 0xff;

        // U_1 is distinct in its input
        hmac.Input(salt);
        hmac.Input(int32_big_endian);
        hmac.Finalize();
        hmac.Result(hash_result);
        hmac.Reset();

        // Store the result in T_i
        std::memcpy(T_i.data(), hash_result.data(), hash_length);

        // Perform subsequent iterations (2..iterations) to compute U_k
        for (std::size_t iter = 2; iter <= iterations; iter++)
        {
            hmac.Input(hash_result);
            hmac.Finalize();
            hmac.Result(hash_result);
            hmac.Reset();

            // XOR with the previous result
            for (std::size_t j = 0; j < hash_length; j++)
            {
                T_i[j] ^= hash_result[j];
            }
        }

        // At this point T_i == U_xor
        if (block < blocks)
        {
            // Copy all of T_i into key
            std::memcpy(p, T_i.data(), hash_length);
            p += hash_length;
        }
        else
        {
            // Copy "remainder" bytes for the last block
            std::memcpy(p, T_i.data(), remainder);
        }
    }

    // Erase local variables
    SecUtil::SecureErase(hash_length);
    SecUtil::SecureErase(blocks);
    SecUtil::SecureErase(remainder);
    SecUtil::SecureErase(p);

    return key;
}

} // namespace Terra::Crypto::KDF
