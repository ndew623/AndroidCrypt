/*
 *  sha1.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the object SHA1, which implements the Secure Hashing
 *      Algorithm SHA-1 as defined in FIPS 180-4.
 *
 *      Note that while FIPS 180-4 specifies that a message may be any number
 *      of bits in length from 0..(2^64)-1, this object will only operate
 *      on messages containing an integral number of octets.
 *
 *      Example simplified usage:
 *          SHA1 sha1("abc");
 *          std::cout << "Hash: " << sha1 << std::endl;
 *
 *      Typical usage example:
 *          SHA1 sha1;
 *          sha1.Input(some_string_data);
 *          sha1.Input(more_string_data);
 *          sha1.Finalize();
 *          SHA1ResultOctets result;
 *          sha1.Result(result);
 *
 *      Input may be provided via the constructor or via one of the Input()
 *      functions.  Once all of the data has been provided to the object, one
 *      must call Finalize() in order to produce the final message digest
 *      (hash) value.  The reason is that, otherwise, the object remains in a
 *      state where it can accept additional input.  To get the computed
 *      message digest, call any of the Result() functions.  One can retrieve a
 *      string, an octet array, or a "word" array (where a "word" is defined as
 *      a 32-bit integer per FIPS 180-4).
 *
 *  Portability Issues:
 *      This code assumes the compiler and platform can support 64-bit integers.
 */

#pragma once

#include "hash.h"

namespace Terra::Crypto::Hashing
{

// Result of the SHA-1 computation holding Digest_Word_Count words
using SHA1ResultWordSpan = std::span<std::uint32_t>;

// Define the SHA1 class
class SHA1 final : public Hash
{
    public:
        // Maximum message size per FIPS 180-4 (in octets)
        static constexpr std::uint64_t Max_Message_Size
        {
            (static_cast<std::uint64_t>(1) << 61) - 1
        };

        // Size of each input block (in octets)
        static constexpr std::size_t Block_Size = 64;

        // Data word size (in bits)
        static constexpr std::size_t Word_Size = 32;

        // Message digest length (in words)
        static constexpr std::size_t Digest_Word_Count = 5;

        // Message digest length (in octets)
        static constexpr std::size_t Digest_Octet_Count = 20;

        // Type for holding result of the SHA-1 computation
        using SHA1ResultWords = std::uint32_t[Digest_Word_Count];

        // Type for holding result of the SHA-1 computation
        using SHA1ResultOctets = std::uint8_t[Digest_Octet_Count];

        SHA1() noexcept;
        SHA1(const std::span<const std::uint8_t> data,
             bool auto_finalize = true,
             bool spaces = true);
        SHA1(const std::string_view data,
             bool auto_finalize = true,
             bool spaces = true);
        SHA1(const SHA1 &other) = default;
        SHA1(SHA1 &&other) = default;
        virtual ~SHA1() noexcept;

        SHA1 &operator=(const SHA1 &other) = default;
        SHA1 &operator=(SHA1 &&other) = default;
        bool operator==(const SHA1 &other) const noexcept;
        bool operator!=(const SHA1 &other) const noexcept;

        void Reset() noexcept override;

        void Input(const std::span<const std::uint8_t> data) override;
        void Input(const std::string_view data) override;

        void Finalize() override;

        std::string Result() const override;
        std::span<std::uint8_t> Result(
                                std::span<std::uint8_t> result) const override;
        SHA1ResultWordSpan Result(SHA1ResultWordSpan result) const;

        constexpr std::size_t GetBlockSize() const noexcept override
        {
            return Block_Size;
        }
        constexpr std::size_t GetDigestLength() const noexcept override
        {
            return Digest_Octet_Count;
        }
        constexpr HashAlgorithm GetHashAlgorithm() const noexcept override
        {
            return HashAlgorithm::SHA1;
        }

        std::uint64_t GetMessageLength() const noexcept;

    protected:
        void ProcessMessageBlock(const std::uint8_t message_block[Block_Size]);
        void PadMessage();

        std::uint64_t message_length;           // Total message length
        std::size_t input_block_length;         // Current input block length
        std::uint8_t input_block[Block_Size];   // Current input block

        // Message digest
        std::uint32_t message_digest[Digest_Word_Count];

        // Internal computation variables
        std::uint32_t W[80];                    // Message schedule
        std::uint32_t a, b, c, d, e;            // Working variables
};

} // namespace Terra::Crypto::Hashing
