/*
 *  hash.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the functions of the base object Hash for the
 *      cryptographic hash functions implemented in this library.  Those are
 *      presently some of the Secure Hashing Algorithms defined in FIPS 180-4.
 *
 *  Portability Issues:
 *      This code assumes the compiler and platform can support 64-bit integers.
 */

#include <climits>
#include <cstring>
#include <terra/crypto/hashing/hash.h>
#include <terra/crypto/hashing/sha1.h>
#include <terra/crypto/hashing/sha224.h>
#include <terra/crypto/hashing/sha256.h>
#include <terra/crypto/hashing/sha384.h>
#include <terra/crypto/hashing/sha512.h>
#include <terra/secutil/secure_erase.h>

namespace Terra::Crypto::Hashing
{

/*
 *  GetHashDigestLength()
 *
 *  Description:
 *      Returns the number of octets produced by the specified hashing
 *      algorithm.
 *
 *  Parameters:
 *      hash_algorithm [in]
 *          The hashing algorithm for which the output length is sought.
 *
 *  Returns:
 *      The number of octets produced as output by the specified hashing
 *      algorithm or zero if there was an error.
 *
 *  Comments:
 *      None.
 */
std::size_t GetHashDigestLength(HashAlgorithm hash_algorithm)
{
    std::size_t length{};

    switch (hash_algorithm)
    {
        case HashAlgorithm::SHA1:
            length = 20;
            break;

        case HashAlgorithm::SHA224:
            length = 28;
            break;

        case HashAlgorithm::SHA256:
            length = 32;
            break;

        case HashAlgorithm::SHA384:
            length = 48;
            break;

        case HashAlgorithm::SHA512:
            length = 64;
            break;

        default:
            static_assert(static_cast<unsigned>(HashAlgorithm::Unknown) == 5,
                          "New hash algorithms need explicit support here");
            length = 0;
            break;
    }

    return length;
}

/*
 *  Hash::Hash()
 *
 *  Description:
 *      This is the constructor for the Hash object.
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
Hash::Hash() noexcept :
    space_separate_words(true),
    digest_finalized(false),
    corrupted(false)
{
}

/*
 *  Hash::Hash()
 *
 *  Description:
 *      This is the constructor for the Hash object.
 *
 *  Parameters:
 *      spaces [in]
 *          True if spaces should be inserted between words in output strings.
 *          Defaults to true.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Hash::Hash(bool spaces) noexcept :
    space_separate_words(spaces),
    digest_finalized(false),
    corrupted(false)
{
}

/*
 *  Hash::~Hash()
 *
 *  Description:
 *      This is the virtual destructor for the Hash object.
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
Hash::~Hash() noexcept
{
    // For security reasons, zero all internal data
    SecUtil::SecureErase(space_separate_words);
    SecUtil::SecureErase(digest_finalized);
    SecUtil::SecureErase(corrupted);
}

/*
 *  Hash::operator==()
 *
 *  Description:
 *      Compare two objects for equality.
 *
 *  Parameters:
 *      other [in]
 *          The other object with which to compare.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
bool Hash::operator==(const Hash &other) const noexcept
{
    // If it's the same object, it's equal
    if (this == &other) return true;

    if ((space_separate_words == other.space_separate_words) &&
        (digest_finalized == other.digest_finalized) &&
        (corrupted == other.corrupted))
    {
        return true;
    }

    return false;
}

/*
 *  Hash::operator!=()
 *
 *  Description:
 *      Compare two objects for inequality.
 *
 *  Parameters:
 *      other [in]
 *          The other object with which to compare.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
bool Hash::operator!=(const Hash &other) const noexcept
{
    return !(*this == other);
}

/*
 *  Hash::operator<<()
 *
 *  Description:
 *      Allow character data to be streamed into the hash function as input.
 *
 *  Parameters:
 *      data [in]
 *          The data to be provided as input.  The maximum length of the
 *          data that may be hashed depends on the hashing algorithm.  This
 *          should be documented in each of the algorithms.
 *
 *  Returns:
 *      A reference to this object.
 *
 *  Comments:
 *      None.
 */
Hash &Hash::operator<<(const std::string_view data)
{
    // It is assumed that a character is eight bits
    static_assert(CHAR_BIT == 8);

    // Provide the data to the Input function
    Input(std::span<const std::uint8_t>{
                    reinterpret_cast<const std::uint8_t *>(data.data()),
                    data.size() });

    // Return a reference to this object
    return *this;
}

/*
 *  Hash::IsFinalized()
 *
 *  Description:
 *      Indicates whether the message digest has been computed.  Once computed,
 *      additional input data cannot be provided.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      True if the message digest computation has been finalized, false if it
 *      has not been finalized.
 *
 *  Comments:
 *      None.
 */
bool Hash::IsFinalized() const noexcept
{
    return digest_finalized;
}

/*
 *  Hash::IsCorrupted()
 *
 *  Description:
 *      Indicates whether the message digest algorithm is corrupted due to some
 *      error during input processing.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      True if the message digest computation is corrupted and not usable.
 *
 *  Comments:
 *      This function doesn't generally need to be called.  If an error occurs
 *      that causes this to be true, an exception would have been thrown.
 */
bool Hash::IsCorrupted() const noexcept
{
    return corrupted;
}

/*
 *  Hash::SpaceSeparateWords()
 *
 *  Description:
 *      Indicates whether spaces should be inserted between words in result
 *      strings.
 *
 *  Parameters:
 *      spaces [in]
 *          True if spaces should be inserted between words in output strings.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void Hash::SpaceSeparateWords(bool spaces) noexcept
{
    space_separate_words = spaces;
}

/*
 *  operator<<() for Hash object
 *
 *  Description:
 *      This function will output a hex string to the output stream for the
 *      computed message digest.
 *
 *  Parameters:
 *      os [in/out]
 *          Output stream specifier
 *
 *      hash [in]
 *          Hash instance from which the message digest should be extracted.
 *
 *  Returns:
 *      A reference to the output stream.
 *
 *  Comments:
 *      If the digest was not computed, this function will emit zeros.
 */
std::ostream &operator<<(std::ostream &os, const Hash &hash)
{
    os << hash.Result();

    return os;
}

/*
 *  CreateHashObject()
 *
 *  Description:
 *      This function will produce a hash object as a unique pointer for
 *      the specified hashing algorithm.
 *
 *  Parameters:
 *      hash_algorithm [in]
 *          The hashing algorithm that should be implemented by the Hash
 *          object.
 *
 *  Returns:
 *      A unique pointer to a Hash object implementing the specified algorithm.
 *      An exception will be thrown if the specified hashing algorithm is
 *      unknown.
 *
 *  Comments:
 *      None.
 */
HashPointer CreateHashObject(HashAlgorithm algorithm)
{
    HashPointer hash;

    switch (algorithm)
    {
        case HashAlgorithm::SHA1:
            hash = std::make_unique<SHA1>();
            break;

        case HashAlgorithm::SHA224:
            hash = std::make_unique<SHA224>();
            break;

        case HashAlgorithm::SHA256:
            hash = std::make_unique<SHA256>();
            break;

        case HashAlgorithm::SHA384:
            hash = std::make_unique<SHA384>();
            break;

        case HashAlgorithm::SHA512:
            hash = std::make_unique<SHA512>();
            break;

        default:
            static_assert(static_cast<unsigned>(HashAlgorithm::Unknown) == 5,
                          "New hash algorithms need explicit support here");
            throw HashException("Unknown hashing function requested");
            break;
    }

    return hash;
}

} // namespace Terra::Crypto::Hashing
