/*
 *  hash.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the an abstract base object Hash for the
 *      cryptographic hash functions implemented in this library.  Those are
 *      presently some of the Secure Hashing Algorithms defined in FIPS 180-4.
 *
 *      Example simplified usage:
 *          HashPointer hash = std::make_unique<SHA256>("abc");
 *          std::cout << "Hash: " << *hash << std::endl;
 *
 *      Typical usage example:
 *          HashPointer hash = std::make_unique<SHA256>();
 *          hash->Input(some_string_data);
 *          hash->Input(more_string_data);
 *          hash->Finalize();
 *          std::string result;
 *          hash->Result(result);
 *
 *      Input may be provided via the constructor or via one of the Input()
 *      functions.  Once all of the data has been provided to the object, one
 *      must call Finalize() in order to produce the final message digest
 *      (hash) value.  The reason is that the object otherwise remains in a
 *      state where it can accept additional input.  To get the computed
 *      message digest, call any of the Result() functions.
 *
 *      Please read the header files for individual derived objects for
 *      specifics related to those objects, such as message length and output
 *      length.  The subclasses also offer additional APIs not found in the
 *      base class that may be useful for specific applications.
 *
 *  Portability Issues:
 *      This code assumes the compiler and platform can support 64-bit integers.
 */

#pragma once

#include <ostream>
#include <cstdint>
#include <cstddef>
#include <span>
#include <string_view>
#include <memory>
#include <stdexcept>

namespace Terra::Crypto::Hashing
{

// Enum to define supported hash function types
enum class HashAlgorithm
{
    SHA1,
    SHA224,
    SHA256,
    SHA384,
    SHA512,
    Unknown
};

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
std::size_t GetHashDigestLength(HashAlgorithm hash_algorithm);

// Define an exception class for Hash-related exceptions
class HashException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// Define the Hash class
class Hash
{
    public:
        Hash() noexcept;
        Hash(bool spaces) noexcept;
        Hash(const Hash &other) = default;
        Hash(Hash &&other) = default;
        virtual ~Hash() noexcept;

        Hash &operator=(const Hash &other) = default;
        Hash &operator=(Hash &&other) = default;
        bool operator==(const Hash &other) const noexcept;
        bool operator!=(const Hash &other) const noexcept;

        virtual void Reset() noexcept = 0;

        virtual void Input(const std::span<const std::uint8_t> data) = 0;
        virtual void Input(const std::string_view data) = 0;

        Hash &operator<<(const std::string_view data);

        virtual void Finalize() = 0;

        virtual std::string Result() const = 0;
        virtual std::span<std::uint8_t> Result(
                                    std::span<std::uint8_t> result) const = 0;

        virtual std::size_t GetBlockSize() const noexcept = 0;
        virtual std::size_t GetDigestLength() const noexcept = 0;
        virtual HashAlgorithm GetHashAlgorithm() const noexcept = 0;

        bool IsFinalized() const noexcept;
        bool IsCorrupted() const noexcept;
        void SpaceSeparateWords(bool spaces) noexcept;

    protected:
        bool space_separate_words;              // Put spaces in output strings?
        bool digest_finalized;                  // Message digest finalized?
        bool corrupted;                         // Message digest corrupted?
};

// Define a unique pointer type for hash functions
using HashPointer = std::unique_ptr<Hash>;

// Streaming operator
std::ostream &operator<<(std::ostream &os, const Hash &hash);

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
 *      If there was an error, nullptr will be returned.
 *
 *  Comments:
 *      None.
 */
HashPointer CreateHashObject(HashAlgorithm hash_algorithm);

} // namespace Terra::Crypto::Hashing
