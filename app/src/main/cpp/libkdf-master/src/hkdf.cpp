/*
 *  hkdf.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements an object called HKDF that performs the
 *      Hash-based Message Authentication Code (HMAC) Key Derivation Function
 *      (KDF) procedures defined in RFC 5869.
 *
 *  Portability Issues:
 *      None.
 */

#include <cstring>
#include <terra/secutil/secure_vector.h>
#include <terra/crypto/kdf/hkdf.h>

namespace Terra::Crypto::KDF
{

/*
 *  HKDF::HKDF()
 *
 *  Description:
 *      Constructor for the HKDF object.
 *
 *  Parameters:
 *      algorithm [in]
 *          The underlying hashing algorithm to employ.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
HKDF::HKDF(Hashing::HashAlgorithm algorithm) :
    hmac(algorithm),
    hash_length{hmac.GetHMACLength()},
    keyed{false}
{
    // Nothing more to do.
}

/*
 *  HKDF::HKDF()
 *
 *  Description:
 *      Constructor for the HKDF object.
 *
 *  Parameters:
 *      algorithm [in]
 *          The underlying hashing algorithm to employ.
 *
 *      key [in]
 *          The Input Keying Material.
 *
 *      salt [in]
 *          Optional salt value to utilize.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
HKDF::HKDF(Hashing::HashAlgorithm algorithm,
           const std::span<const std::uint8_t> key,
           const std::span<const std::uint8_t> salt) :
    HKDF(algorithm)
{
    Extract(key, salt);
}

/*
 *  HKDF::~HKDF()
 *
 *  Description:
 *      Destructor for the HKDF object.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
HKDF::~HKDF()
{
    SecUtil::SecureErase(hash_length);
    SecUtil::SecureErase(keyed);
}

/*
 *  HKDF::Extract()
 *
 *  Description:
 *      The HKDF-Extract() function as defined in RFC 5869.  This function will
 *      produce as output a pseudorandom key that is subsequently used with
 *      HMAC object when the Expand() function is invoked.
 *
 *  Parameters:
 *      ikm [in]
 *          The Input Keying Material (IKM) used to key the HMAC-KDF.
 *
 *      salt [in]
 *          Salt value to utilize (optional; may be zero-length).
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void HKDF::Extract(const std::span<const std::uint8_t> ikm,
                   const std::span<const std::uint8_t> salt)
{
    SecUtil::SecureVector<std::uint8_t> prk;

    // Indicate that the HMAC is not keyed
    keyed = false;

    // Ensure there is a valid key provided
    if (ikm.empty()) throw KDFException("HKDF: empty IKM provided");

    // Per RFC 5869 section 2.1, the salt is used as the key for HMAC.  Per
    // section 2.2, if the salt is not provided then a vector of zeros of the
    // hash length is used in its place.  The HMAC algorithm does exactly that
    // if the key length is zero, so nothing special needs to be provided.
    hmac.SetKey(salt);

    // The IKM is then provided as input
    hmac.Input(ikm);
    hmac.Finalize();

    // Set the PRK buffer to the expected length
    prk.resize(hash_length);

    // Retrieve the PRK
    hmac.Result(prk);

    // Re-key (and reset) the HMAC object with the PRK
    hmac.SetKey(prk);

    // Note that the HMAC is now keyed
    keyed = true;
}

/*
 *  HKDF::Expand()
 *
 *  Description:
 *      The HKDF-Expand() as defined in RFC 5869.  This function will produce
 *      "output keying material" using the PRK created via the Extract()
 *      function and the span of octets "info".  The output is placed
 *      into the span "key" and a span having the same pointer and length
 *      is returned from this function.  If Extract() was not called
 *      previously, this function will throw an exception.
 *
 *  Parameters:
 *      info [in]
 *          Application-specific information used to generate distinct keying
 *          material.
 *
 *      key [out]
 *          The Output Keying Material to be used as a key by the application.
 *          The length of the output keying material to generate is taken
 *          from the length of this span.  The span may be in the range of
 *          zero to 255 * HashLen.  HashLen is the length of the underlying
 *          hashing function's output.  In the case of SHA-1, for example,
 *          that is 20.  In the case of SHA-256, that is 32.  One may call
 *          GetHMACLength() to determine the output length of the
 *          underlying hash function.
 *
 *  Returns:
 *      A span of octets to the Output Keying Material.
 *
 *  Comments:
 *      None.
 */
std::span<std::uint8_t> HKDF::Expand(std::span<const std::uint8_t> info,
                                     std::span<std::uint8_t> key)
{
    std::uint8_t i{};
    SecUtil::SecureVector<std::uint8_t> T_i(hash_length);

    // Ensure that the HMAC was properly keyed by calling Extract() first
    if (!keyed) throw KDFException("HKDF was not keyed by calling Extract()");

    // If the output key span is zero, there is nothing to do
    if (key.empty()) return key;

    // Determine number of hash-length blocks
    std::size_t blocks = key.size() / hash_length;
    if ((key.size() % hash_length) > 0) blocks++;

    // Determine the size of the final block
    std::size_t remainder = key.size() - ((blocks - 1) * hash_length);

    // Ensure the requested key length is not excessive (RFC 5869 Section 2.3)
    if (blocks > 255) throw KDFException("Requested key length is excessive");

    // Get a pointer to the start of the span
    std::uint8_t *p = key.data();

    // Produce they key HMAC(PRK, T_[i-1] || info || index)
    for (std::size_t block = 1; block <= blocks; block++)
    {
        // Produce T_i (T_0 is an empty string and thus not fed to HMAC)
        if (block > 1) hmac.Input(T_i);
        hmac.Input(info);
        i = static_cast<std::uint8_t>(block);
        hmac.Input({&i, 1});
        hmac.Finalize();
        hmac.Result(T_i);
        hmac.Reset();

        // Now concatenate T_i into the key span
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
    SecUtil::SecureErase(i);
    SecUtil::SecureErase(blocks);
    SecUtil::SecureErase(remainder);
    SecUtil::SecureErase(p);

    return key;
}

/*
 *  HKDF::Expand()
 *
 *  Description:
 *      The HKDF-Expand() as defined in RFC 5869.  This function will produce
 *      "output keying material" using the PRK created via the Extract()
 *      function and the span of octets "info".  The output is placed
 *      into the span "key" and a span having the same pointer and length
 *      is returned from this function.  If Extract() was not called
 *      previously, this function will throw an exception.
 *
 *  Parameters:
 *      info [in]
 *          Application-specific information used to generate distinct keying
 *          material.
 *
 *      key [out]
 *          The Output Keying Material to be used as a key by the application.
 *          The length of the output keying material to generate is taken
 *          from the length of this span.  The span may be in the range of
 *          zero to 255 * HashLen.  HashLen is the length of the underlying
 *          hashing function's output.  In the case of SHA-1, for example,
 *          that is 20.  In the case of SHA-256, that is 32.  One may call
 *          GetHMACLength() to determine the output length of the
 *          underlying hash function.
 *
 *  Returns:
 *      A span of octets to the Output Keying Material.
 *
 *  Comments:
 *      None.
 */
std::span<std::uint8_t> HKDF::Expand(std::span<const char> info,
                                     std::span<std::uint8_t> key)
{
    return Expand(
        {reinterpret_cast<const std::uint8_t *>(info.data()), info.size()},
        key);
}

} // namespace Terra::Crypto::KDF
