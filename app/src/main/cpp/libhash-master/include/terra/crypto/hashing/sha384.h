/*
 *  sha384.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the object SHA384, which implements the Secure
 *      Hashing Algorithm SHA-384 as defined in FIPS 180-4.
 *
 *      Note that while FIPS 180-4 specifies that a message may be any number
 *      of bits in length from 0..(2^128)-1, this object will only operate on
 *      messages containing an integral number of octets.
 *
 *      Example simplified usage:
 *          SHA384 sha384("abc");
 *          std::cout << "Hash: " << sha384 << std::endl;
 *
 *      Typical usage example:
 *          SHA384 sha384;
 *          sha384.Input(some_string_data);
 *          sha384.Input(more_string_data);
 *          sha384.Finalize();
 *          SHA384ResultOctets result;
 *          sha384.Result(result);
 *
 *      Input may be provided via the constructor or via one of the Input()
 *      functions.  Once all of the data has been provided to the object, one
 *      must call Finalize() in order to produce the final message digest
 *      (hash) value.  The reason is that, otherwise, the object remains in a
 *      state where it can accept additional input.  To get the computed
 *      message digest, call any of the Result() functions.  One can retrieve a
 *      string, an octet array, or a "word" array (where a "word" is defined as
 *      a 64-bit integer per FIPS 180-4).
 *
 *  Portability Issues:
 *      This code assumes the compiler and platform can support 64-bit integers.
 */

#pragma once

#include "hash.h"

namespace Terra::Crypto::Hashing
{

// Result of the SHA-384 computation holding Digest_Word_Count words
using SHA384ResultWordSpan = std::span<std::uint64_t>;

// Define a structure to hold the 128-bit message length
struct SHA384MessageLength
{
    std::uint64_t high;
    std::uint64_t low;

    // Comparison operators for SHA384MessageLength
    constexpr bool operator==(const SHA384MessageLength &other) const
    {
        return (high == other.high) && (low == other.low);
    }
    constexpr bool operator!=(const SHA384MessageLength &other) const
    {
        return !(*this == other);
    }
    constexpr bool operator>(const SHA384MessageLength &other) const
    {
        if (high > other.high) return true;
        if (high < other.high) return false;

        // High values must be equal; now test low value
        if (low > other.low) return true;
        if (low < other.low) return false;

        // Low values must be equal
        return false;
    }
    constexpr bool operator<(const SHA384MessageLength &other) const
    {
        if (high < other.high) return true;
        if (high > other.high) return false;

        // High values must be equal; now test low value
        if (low < other.low) return true;
        if (low > other.low) return false;

        // Low values must be equal
        return false;
    }
    constexpr SHA384MessageLength operator+(const std::uint64_t increment)
    {
        SHA384MessageLength length = *this;
        length += increment;
        return length;
    }
    constexpr SHA384MessageLength &operator+=(const std::uint64_t increment)
    {
        std::uint64_t original_low = low;
        low += increment;
        if (low < original_low) high++;
        return *this;
    }
};

// Function to about the SHA384MessageLength to the output stream
std::ostream &operator<<(std::ostream &os, const SHA384MessageLength &length);

// Define the SHA384 class
class SHA384 final : public Hash
{
    public:
        // Maximum message size per FIPS 180-4 (in octets)
        static constexpr SHA384MessageLength Max_Message_Size
        {
            (static_cast<std::uint64_t>(1) << 61) - 1,
            0xffff'ffff'ffff'ffff
        };

        // Size of each input block (in octets)
        static constexpr std::size_t Block_Size = 128;

        // Data word size (in bits)
        static constexpr std::size_t Word_Size = 64;

        // Message digest length (in words)
        static constexpr std::size_t Digest_Word_Count = 6;

        // Message digest length (in octets)
        static constexpr std::size_t Digest_Octet_Count = 48;

        // Type holding result of the SHA-384 computation
        using SHA384ResultWords = std::uint64_t[Digest_Word_Count];

        // Type holding result of the SHA-384 computation
        using SHA384ResultOctets = std::uint8_t[Digest_Octet_Count];

        SHA384() noexcept;
        SHA384(const std::span<const std::uint8_t> data,
               bool auto_finalize = true,
               bool spaces = true);
        SHA384(const std::string_view data,
               bool auto_finalize = true,
               bool spaces = true);
        SHA384(const SHA384 &other) = default;
        SHA384(SHA384 &&other) = default;
        virtual ~SHA384() noexcept;

        SHA384 &operator=(const SHA384 &other) = default;
        SHA384 &operator=(SHA384 &&other) = default;
        bool operator==(const SHA384 &other) const noexcept;
        bool operator!=(const SHA384 &other) const noexcept;

        void Reset() noexcept override;

        void Input(const std::span<const std::uint8_t> data) override;
        void Input(const std::string_view data) override;

        void Finalize() override;

        std::string Result() const override;
        std::span<std::uint8_t> Result(
                                std::span<std::uint8_t> result) const override;
        SHA384ResultWordSpan Result(SHA384ResultWordSpan result) const;

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
            return HashAlgorithm::SHA384;
        }

        SHA384MessageLength GetMessageLength() const noexcept;

    protected:
        void ProcessMessageBlock(const std::uint8_t message_block[Block_Size]);
        void PadMessage();

        SHA384MessageLength message_length;     // Total message length
        std::size_t input_block_length;         // Current input block length
        std::uint8_t input_block[Block_Size];   // Current input block

        // Internal message digest length (in words)
        static constexpr std::size_t Internal_Digest_Word_Count = 8;

        // Message digest
        std::uint64_t message_digest[Internal_Digest_Word_Count];

        // Internal computation variables
        std::uint64_t W[80];                    // Message schedule
        std::uint64_t a, b, c, d, e, f, g, h;   // Working variables
        std::uint64_t T;                        // Temporary variable
};

} // namespace Terra::Crypto::Hashing
