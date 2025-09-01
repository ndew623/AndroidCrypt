/*
 *  hmac.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the keyed Hash Message Authentication Code (HMAC)
 *      logic defined in FIPS 198-1.  It is intended for use with the
 *      hashing functions implemented in this library.
 *
 *  Portability Issues:
 *      This code assumes the compiler and platform can support 64-bit integers.
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <string_view>
#include <span>
#include <memory>
#include "hash.h"

namespace Terra::Crypto::Hashing
{

// Define the HMAC class
class HMAC
{
    protected:
        static constexpr std::uint8_t ipad = 0x36;
        static constexpr std::uint8_t opad = 0x5c;

        // These two values must be set to the largest possible values
        // of any hash functions supported
        static constexpr std::size_t Max_Block_Size = 128;
        static constexpr std::size_t Max_Digest = 64;

    public:
        HMAC(const HashAlgorithm hash_algorithm);
        HMAC(const HashAlgorithm hash_algorithm,
             const std::span<const std::uint8_t> key,
             const bool spaces = true);
        HMAC(const HashAlgorithm hash_algorithm,
             const std::string_view key,
             const bool spaces = true);
        HMAC(const HMAC &other);
        HMAC(HMAC &&other) noexcept;
        ~HMAC() noexcept;

        HMAC &operator=(const HMAC &other);
        HMAC &operator=(HMAC &&other) noexcept;
        bool operator==(const HMAC &other) const noexcept;
        bool operator!=(const HMAC &other) const noexcept;

        void Reset();

        void SetKey(const std::span<const std::uint8_t> key);
        void SetKey(const std::string_view key);

        void Input(const std::span<const std::uint8_t> data);
        void Input(const std::string_view data);
        HMAC &operator<<(const std::string_view data);

        void Finalize();

        std::string Result() const;
        std::span<std::uint8_t> Result(std::span<std::uint8_t> result) const;

        std::size_t GetHMACLength() const noexcept
        {
            return hash ? hash->GetDigestLength() : 0;
        }

        bool IsFinalized() const noexcept
        {
            return hash ? hash->IsFinalized() : false;
        }

        bool IsCorrupted() const noexcept
        {
            return hash ? hash->IsCorrupted() : true;
        }

        void SpaceSeparateWords(bool spaces) noexcept
        {
            space_separate_words = spaces;
            if (hash) hash->SpaceSeparateWords(spaces);
        }

    protected:
        HashAlgorithm hash_algorithm;
        std::unique_ptr<Hash> hash;
        bool space_separate_words;
        bool keyed;
        std::size_t block_size;
        std::uint8_t message_digest[Max_Digest];
        std::uint8_t K0[Max_Block_Size];
        std::uint8_t K0_ipad[Max_Block_Size];
        std::uint8_t K0_opad[Max_Block_Size];
};

// Streaming operator
std::ostream &operator<<(std::ostream &o, const HMAC &hmac);

} // namespace Terra::Crypto::Hashing
