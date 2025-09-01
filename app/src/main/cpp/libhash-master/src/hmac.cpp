/*
 *  hmac.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the keyed Hash Message Authentication Code (HMAC)
 *      logic defined in FIPS 198-1.  It is intended for use with the
 *      hashing functions implemented in this library.
 *
 *      In the event that std::move() is used to move the HMAC object to a
 *      different object, the hash pointer will be invalid.  For this reason,
 *      each API call checks for a valid hash pointer value.  If the pointer
 *      is invalid, an access violation will not occur even though results
 *      of API calls will be indeterminate.
 *
 *  Portability Issues:
 *      This code assumes the compiler and platform can support 64-bit integers.
 */

#include <memory>
#include <cstring>
#include <climits>
#include <algorithm>
#include <terra/crypto/hashing/hmac.h>
#include <terra/crypto/hashing/sha1.h>
#include <terra/crypto/hashing/sha224.h>
#include <terra/crypto/hashing/sha256.h>
#include <terra/crypto/hashing/sha384.h>
#include <terra/crypto/hashing/sha512.h>
#include <terra/secutil/secure_erase.h>

namespace Terra::Crypto::Hashing
{

// It is assumed that a character is eight bits
static_assert(CHAR_BIT == 8);

namespace
{

/*
 *  CloneHashFunction()
 *
 *  Description:
 *      This function will copy the object stored in the unique pointer.
 *      The reason for this special function is that since Hash is a pure
 *      virtual object, a deep copy is required that is based on the actual
 *      underlying hashing object (e.g., SHA1, SHA256, etc).
 *
 *  Parameters:
 *      source [out]
 *          The source hash object to clone.
 *
 *  Returns:
 *      A new hash object of the same type as "source" and containing the same
 *      state information.
 *
 *  Comments:
 *      None.
 */
std::unique_ptr<Hash> CloneHashFunction(const std::unique_ptr<Hash> &source)
{
    std::unique_ptr<Hash> new_hash;

    // Ensure the source object is valid
    if (!source) throw HashException("The source hash object is invalid");

    // Create an object to do hashing based on the type requested
    switch (source->GetHashAlgorithm())
    {
        case HashAlgorithm::SHA1:
            new_hash =
                std::make_unique<SHA1>(*(dynamic_cast<SHA1 *>(source.get())));
            break;

        case HashAlgorithm::SHA224:
            new_hash = std::make_unique<SHA224>(
                *(dynamic_cast<SHA224 *>(source.get())));
            break;

        case HashAlgorithm::SHA256:
            new_hash = std::make_unique<SHA256>(
                *(dynamic_cast<SHA256 *>(source.get())));
            break;

        case HashAlgorithm::SHA384:
            new_hash = std::make_unique<SHA384>(
                *(dynamic_cast<SHA384 *>(source.get())));
            break;

        case HashAlgorithm::SHA512:
            new_hash = std::make_unique<SHA512>(
                *(dynamic_cast<SHA512 *>(source.get())));
            break;

        default:
            static_assert(static_cast<unsigned>(HashAlgorithm::Unknown) == 5,
                          "New hash algorithms need explicit support here");
            throw HashException("Unknown hashing function requested");
            break;
    }

    return new_hash;
}

/*
 *  CompareHashFunction()
 *
 *  Description:
 *      This function will compare the given hash objects to see if their
 *      underlying hash objects are identical.
 *
 *  Parameters:
 *      hash1 [out]
 *          The first hash object to consider.
 *
 *      hash2 [in]
 *          The second hash object to consider.
 *
 *  Returns:
 *      True if the hash objects are identical.
 *
 *  Comments:
 *      None.
 */
bool CompareHashFunction(const std::unique_ptr<Hash> &hash1,
                         const std::unique_ptr<Hash> &hash2)
{
    bool result{};

    // Ensure the source object is valid
    if (!hash1 || !hash2) throw HashException("Comparing invalid hash object");

    // Ensure the two objects are of the same type
    if (hash1->GetHashAlgorithm() != hash2->GetHashAlgorithm()) return false;

    // Create an object to do hashing based on the type requested
    switch (hash1->GetHashAlgorithm())
    {
        case HashAlgorithm::SHA1:
            result = (*dynamic_cast<SHA1 *>(hash1.get()) ==
                      *dynamic_cast<SHA1 *>(hash2.get()));
            break;

        case HashAlgorithm::SHA224:
            result = (*dynamic_cast<SHA256*>(hash1.get()) ==
                      *dynamic_cast<SHA256 *>(hash2.get()));
            break;

        case HashAlgorithm::SHA256:
            result = (*dynamic_cast<SHA256*>(hash1.get()) ==
                      *dynamic_cast<SHA256 *>(hash2.get()));
            break;

        case HashAlgorithm::SHA384:
            result = (*dynamic_cast<SHA256 *>(hash1.get()) ==
                      *dynamic_cast<SHA256 *>(hash2.get()));
            break;

        case HashAlgorithm::SHA512:
            result = (*dynamic_cast<SHA256 *>(hash1.get()) ==
                      *dynamic_cast<SHA256 *>(hash2.get()));
            break;

        default:
            static_assert(static_cast<unsigned>(HashAlgorithm::Unknown) == 5,
                          "New hash algorithms need explicit support here");
            throw HashException("Unknown hashing function requested");
            break;
    }

    return result;
}

} // namespace

/*
 *  HMAC::HMAC()
 *
 *  Description:
 *      This is the constructor for the HMAC object.
 *
 *  Parameters:
 *      hash_algorithm [in]
 *          The hashing algorithm to use for computing an HMAC.  The user
 *          must call SetKey() before attempting to provide input.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      If an invalid HashAlgorithm is requested, this object will throw an
 *      exception of type HashException.  Also, an exception will be thrown
 *      if the key length exceeds the octet limit of the underlying hash
 *      function.
 */
HMAC::HMAC(const HashAlgorithm hash_algorithm) :
    hash_algorithm{hash_algorithm},
    space_separate_words{true},
    keyed{false},
    block_size{0},
    message_digest{},
    K0{},
    K0_ipad{},
    K0_opad{}
{
    // Create the hash object
    hash = CreateHashObject(hash_algorithm);

    // Ensure the pointer is valid
    if (!hash) throw HashException("Failed to create hashing object");

    // Set spacing preference
    hash->SpaceSeparateWords(space_separate_words);

    // Store the fixed block size to avoid repetitive function calls
    block_size = hash->GetBlockSize();
}

/*
 *  HMAC::HMAC()
 *
 *  Description:
 *      This is the constructor for the HMAC object.
 *
 *  Parameters:
 *      hash_algorithm [in]
 *          The hashing algorithm to use for computing an HMAC.
 *
 *      key [in]
 *          The key to use with this keyed HMAC.
 *
 *      spaces [in]
 *          True if spaces should be inserted between words in output strings.
 *          Defaults to true.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      If an invalid HashAlgorithm is requested, this object will throw an
 *      exception of type HashException.  Also, an exception will be thrown
 *      if the key length exceeds the octet limit of the underlying hash
 *      function.
 */
HMAC::HMAC(const HashAlgorithm hash_algorithm,
           const std::span<const std::uint8_t> key,
           const bool spaces) :
    HMAC(hash_algorithm)
{
    // Set the spacing as requested
    space_separate_words = spaces;

    // Set spacing preference
    hash->SpaceSeparateWords(spaces);

    // Set the key
    SetKey(key);
}

/*
 *  HMAC::HMAC()
 *
 *  Description:
 *      This is the constructor for the HMAC object.
 *
 *  Parameters:
 *      hash_algorithm [in]
 *          The hashing algorithm to use for computing an HMAC.
 *
 *      key [in]
 *          The key to use with this keyed HMAC.
 *
 *      spaces [in]
 *          True if spaces should be inserted between words in output strings.
 *          Defaults to true.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      If an invalid HashAlgorithm is requested, this object will throw an
 *      exception of type HashException.  Also, an exception will be thrown
 *      if the key length exceeds the octet limit of the underlying hash
 *      function.
 */
HMAC::HMAC(const HashAlgorithm hash_algorithm,
           const std::string_view key,
           const bool spaces) :
    HMAC(hash_algorithm,
         std::span<const std::uint8_t>{
             reinterpret_cast<const std::uint8_t *>(key.data()),
             key.length()},
         spaces)
{
    // Nothing more to do
}

/*
 *  HMAC::HMAC()
 *
 *  Description:
 *      This a copy constructor for the HMAC object.
 *
 *  Parameters:
 *      other [in]
 *          The other HMAC object from which to copy.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
HMAC::HMAC(const HMAC &other) :
    hash_algorithm{other.hash_algorithm},
    space_separate_words{other.space_separate_words},
    keyed{other.keyed},
    block_size{other.block_size},
    message_digest{},
    K0{},
    K0_ipad{},
    K0_opad{}
{
    // Clone the hash object
    hash = CloneHashFunction(other.hash);

    // Copy the other values
    std::memcpy(message_digest, other.message_digest, sizeof(message_digest));
    std::memcpy(K0, other.K0, sizeof(K0));
    std::memcpy(K0_ipad, other.K0_ipad, sizeof(K0_ipad));
    std::memcpy(K0_opad, other.K0_opad, sizeof(K0_opad));
}

/*
 *  HMAC::HMAC()
 *
 *  Description:
 *      This a move constructor for the HMAC object.
 *
 *  Parameters:
 *      other [in]
 *          The other HMAC object from which to move resources.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
HMAC::HMAC(HMAC &&other) noexcept :
    hash_algorithm{other.hash_algorithm},
    hash{std::move(other.hash)},
    space_separate_words{other.space_separate_words},
    keyed{other.keyed},
    block_size{other.block_size},
    message_digest{},
    K0{},
    K0_ipad{},
    K0_opad{}
{
    // Copy the other values
    std::memcpy(message_digest, other.message_digest, sizeof(message_digest));
    std::memcpy(K0, other.K0, sizeof(K0));
    std::memcpy(K0_ipad, other.K0_ipad, sizeof(K0_ipad));
    std::memcpy(K0_opad, other.K0_opad, sizeof(K0_opad));

    // Indicate "other" is no longer keyed
    other.keyed = false;
}

/*
 *  HMAC::~HMAC()
 *
 *  Description:
 *      This is the destructor for the HMAC object.
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
HMAC::~HMAC() noexcept
{
    // For security reasons, zero all internal data
    SecUtil::SecureErase(hash_algorithm);
    SecUtil::SecureErase(keyed);
    SecUtil::SecureErase(block_size);
    SecUtil::SecureErase(message_digest, sizeof(message_digest));
    SecUtil::SecureErase(K0, sizeof(K0));
    SecUtil::SecureErase(K0_ipad, sizeof(K0_ipad));
    SecUtil::SecureErase(K0_opad, sizeof(K0_opad));
}

/*
 *  HMAC::operator=()
 *
 *  Description:
 *      Copy assignment operator to copy the given HMAC object.
 *
 *  Parameters:
 *      other [in]
 *          The other HMAC object from which to copy data.
 *
 *  Returns:
 *      A reference to this HMAC object.
 *
 *  Comments:
 *      None.
 */
HMAC &HMAC::operator=(const HMAC &other)
{
    // Do not copy the same object
    if (this == &other) return *this;

    // Make a copy of the hash function object
    hash = CloneHashFunction(other.hash);

    // Copy the other values
    hash_algorithm = other.hash_algorithm;
    space_separate_words = other.space_separate_words;
    keyed = other.keyed;
    block_size = other.block_size;
    std::memcpy(message_digest, other.message_digest, sizeof(message_digest));
    std::memcpy(K0, other.K0, sizeof(K0));
    std::memcpy(K0_ipad, other.K0_ipad, sizeof(K0_ipad));
    std::memcpy(K0_opad, other.K0_opad, sizeof(K0_opad));

    return *this;
}

/*
 *  HMAC::operator=()
 *
 *  Description:
 *      Move assignment operator to move the given HMAC object.
 *
 *  Parameters:
 *      other [in]
 *          The other object from which to move data.
 *
 *  Returns:
 *      A reference to this HMAC object.
 *
 *  Comments:
 *      None.
 */
HMAC &HMAC::operator=(HMAC &&other) noexcept
{
    // Do not move the same object
    if (this == &other) return *this;

    // Move the hash pointer
    hash = std::move(other.hash);

    // Copy the other values
    hash_algorithm = other.hash_algorithm;
    space_separate_words = other.space_separate_words;
    keyed = other.keyed;
    block_size = other.block_size;
    std::memcpy(message_digest, other.message_digest, sizeof(message_digest));
    std::memcpy(K0, other.K0, sizeof(K0));
    std::memcpy(K0_ipad, other.K0_ipad, sizeof(K0_ipad));
    std::memcpy(K0_opad, other.K0_opad, sizeof(K0_opad));

    // Indicate "other" is no longer keyed
    other.keyed = false;

    return *this;
}

/*
 *  HMAC::operator==()
 *
 *  Description:
 *      Compare two objects for equality.  Equality includes the state of
 *      the object, so if one is finalized and the other is not, then they are
 *      not considered equal even if the same input was provided to both.  The
 *      reason is that the non-finalized object can accept additional data and
 *      ultimately produce a completely different result.  Further, all base
 *      member data is also compared for equality.
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
bool HMAC::operator==(const HMAC &other) const noexcept
{
    // Is this the same object?
    if (this == &other) return true;

    try
    {
        // Compare the underlying hashing objects
        if (!CompareHashFunction(hash, other.hash)) return false;
    }
    catch (const HashException &)
    {
        return false;
    }

    // Ensure the spacing, keys, and block size values are the same
    if ((space_separate_words != other.space_separate_words) ||
        (keyed != other.keyed) || (block_size != other.block_size))
    {
        return false;
    }

    // Compare the message digest values
    if (std::memcmp(message_digest,
                    other.message_digest,
                    hash->GetDigestLength()) != 0)
    {
        return false;
    }

    // Compare the keying buffers
    if (std::memcmp(K0, other.K0, sizeof(K0)) != 0) return false;
    if (std::memcmp(K0_ipad, other.K0_ipad, sizeof(K0_ipad)) != 0) return false;
    if (std::memcmp(K0_opad, other.K0_opad, sizeof(K0_opad)) != 0) return false;

    return true;
}

/*
 *  HMAC::operator!=()
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
bool HMAC::operator!=(const HMAC &other) const noexcept
{
    return !(*this == other);
}

/*
 *  HMAC::Reset()
 *
 *  Description:
 *      This function will reset the HMAC object so that it may be
 *      reused to compute another HMAC.  Any previous data provided as
 *      input is effectively erased.  However, the key is retained.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      If the key length exceeds the octet limit of the underlying hash
 *      function an exception will be thrown.
 */
void HMAC::Reset()
{
    // Reset the hash function, if there is one
    if (hash) hash->Reset();

    // Clear the message digest
    std::memset(message_digest, 0, sizeof(message_digest));

    // Feed the hashing algorithm K0 ^ ipad if key provided
    if (keyed) hash->Input({K0_ipad, block_size});
}

/*
 *  HMAC::SetKey()
 *
 *  Description:
 *      This function will allow the user to change the key used with
 *      the HMAC object.  The hash function will be reset (if necessary).
 *      When calling this function, any previous input to will be lost.
 *
 *  Parameters:
 *      key [in]
 *          The key to use with this keyed HMAC.  The key can be of any length,
 *          though keys less the length of the underlying hash function
 *          output is discouraged (per RFC 2104 section 3).
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      If the key length exceeds the octet limit of the underlying hash
 *      function an exception will be thrown.
 */
void HMAC::SetKey(const std::span<const std::uint8_t> key)
{
    // If there is no hash object, create one (this might be due to std::move
    // being used on the object)
    if (!hash)
    {
        // Create the hash object
        hash = CreateHashObject(hash_algorithm);

        // Ensure the pointer is valid
        if (!hash) throw HashException("Failed to create hashing object");

        // Set spacing preference
        hash->SpaceSeparateWords(space_separate_words);

        // Store the fixed block size to avoid repetitive function calls
        block_size = hash->GetBlockSize();
    }

    // Was a key previously used with this hash object?
    if (keyed)
    {
        // Reset the hash function to a known state if it was keyed previously
        hash->Reset();

        // Zero the key buffer
        std::memset(K0, 0, sizeof(K0));

        // Reset the key boolean in case of failure
        keyed = false;
    }

    // Assign the key to K0 per FIPS 198-1 section 4
    if (key.size() <= block_size)
    {
        // Copy the key and zeros fill the balance of the K0 buffer
        std::memcpy(K0, key.data(), key.size());
    }
    else
    {
        // Hash the key
        hash->Input(key);
        hash->Finalize();
        hash->Result(K0);
        hash->Reset();
    }

    // Compute the results of XORing the key with ipad and opad
    for (std::size_t i = 0; i < block_size; i++)
    {
        K0_ipad[i] = K0[i] ^ ipad;
        K0_opad[i] = K0[i] ^ opad;
    }

    // Feed the hashing algorithm K0 ^ ipad
    hash->Input({K0_ipad, block_size});

    // Indicate that the hash object has been keyed
    keyed = true;
}

/*
 *  HMAC::SetKey()
 *
 *  Description:
 *      This function will allow the user to change the key used with
 *      the HMAC object.  The hash function will be reset (if necessary).
 *      When calling this function, any previous input to will be lost.
 *
 *  Parameters:
 *      key [in]
 *          The key to use with this keyed HMAC.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      If the key length exceeds the octet limit of the underlying hash
 *      function an exception will be thrown.
 */
void HMAC::SetKey(const std::string_view key)
{
    SetKey(std::span<const std::uint8_t>{
        reinterpret_cast<const std::uint8_t *>(key.data()),
        key.length()});
}

/*
 *  HMAC::Input()
 *
 *  Description:
 *      This function is used to feed the underlying hashing algorithm with
 *      input data.
 *
 *  Parameters:
 *      data [in]
 *          A span over an array of octets to be used as input.  The length
 *          must not exceed the maximum message size of the underlying hashing
 *          algorithm.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void HMAC::Input(const std::span<const std::uint8_t> data)
{
    // Ensure the hash is keyed
    if (!keyed) throw HashException("No key was provided to HMAC");

    hash->Input(data);
}

/*
 *  HMAC::Input()
 *
 *  Description:
 *      This function is used to feed the underlying hashing algorithm with
 *      input data.
 *
 *  Parameters:
 *      data [in]
 *          A string of octets to provide as input.  The length must not exceed
 *          the maximum message size of the underlying hashing algorithm.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void HMAC::Input(const std::string_view data)
{
    // Ensure the hash is keyed
    if (!keyed) throw HashException("No key was provided to HMAC");

    hash->Input(data);
}

/*
 *  HMAC::operator<<()
 *
 *  Description:
 *      Allow character data to be streamed into the hash function as input.
 *
 *  Parameters:
 *      data [in]
 *          The data to be provided as input.
 *
 *  Returns:
 *      A reference to this object.
 *
 *  Comments:
 *      None.
 */
HMAC &HMAC::operator<<(const std::string_view data)
{
    // Ensure the hash is keyed
    if (!keyed) throw HashException("No key was provided to HMAC");

    // Provide the data to the Input function
    hash->Input(data);

    // Return a reference to this object
    return *this;
}

/*
 *  HMAC::Finalize()
 *
 *  Description:
 *      Finalize the HMAC result.
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
void HMAC::Finalize()
{
    // Ensure the hash is keyed
    if (!keyed) throw HashException("No key was provided to HMAC");

    if (!hash->IsFinalized())
    {
        // Finalize the hashing object
        hash->Finalize();

        // Get the result of the inner hash
        hash->Result(message_digest);

        // Reset the hashing function
        hash->Reset();

        // Feed the hashing algorithm K0 ^ opad
        hash->Input({K0_opad, block_size});

        // Concatenate the previously computed hash
        hash->Input({message_digest, hash->GetDigestLength()});

        // Finalize the result of the outer hash
        hash->Finalize();
    }
}

/*
 *  HMAC::Result()
 *
 *  Description:
 *      Gets the resulting HMAC value as a string.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string containing the hex digit of the computed HMAC.
 *
 *  Comments:
 *      None.
 */
std::string HMAC::Result() const
{
    if (!hash) throw HashException("No hashing object exists");

    return hash->Result();
}

/*
 *  HMAC::Result()
 *
 *  Description:
 *      Gets the resulting HMAC value as an octet array.
 *
 *  Parameters:
 *      result [out]
 *          A span that will be populated with the result of the HMAC
 *          computation.
 *
 *  Returns:
 *      A span over the same input span, but reflecting the actual number of
 *      octets in the hash result.  The length of the result depends on the
 *      underlying hash function.  To determine the octet length, call
 *      GetHMACLength().  This length is always constant for a given
 *      hashing algorithm.
 *
 *  Comments:
 *      None.
 */
std::span<std::uint8_t> HMAC::Result(std::span<std::uint8_t> result) const
{
    if (!hash) throw HashException("No hashing object exists");

    return hash->Result(result);
}

/*
 *  operator<<() for HMAC object
 *
 *  Description:
 *      This function will output a hex string to the output stream for the
 *      computed HMAC.
 *
 *  Parameters:
 *      o [in/out]
 *          Output stream specifier
 *
 *      hmac [in]
 *          HMAC instance from which the HMAC should be rendered.
 *
 *  Returns:
 *      A reference to the output stream.
 *
 *  Comments:
 *      If the digest was not computed, this function will emit zeros.
 */
std::ostream &operator<<(std::ostream &o, const HMAC &hmac)
{
    o << hmac.Result();

    return o;
}

} // namespace Terra::Crypto::Hashing
