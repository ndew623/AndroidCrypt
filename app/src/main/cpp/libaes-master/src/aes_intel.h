/*
 *  aes_intel.h
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the AESIntel object which performs encryption and
 *      decryption as specified in FIPS 197 ("Advanced Encryption Standard")
 *      using Intel Intrinsics functions.
 *
 *      Reference implementation code:
 *      https://www.intel.com/content/dam/doc/white-paper/advanced-encryption-standard-new-instructions-set-paper.pdf
 *
 *      Note that if one attempts to use this AES engine on a processor that
 *      does not support the AES NI instructions it will not work and will
 *      likely cause a core dump.  One should always call the GetEngineType()
 *      to ensure that it does not return "AESEngineType::Unavailable" before
 *      attempting to use any of the functions.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <stdexcept>
#include <cstdint>
#include <cstddef>
#include <span>
#include <terra/crypto/cipher/aes.h>
#include "intel_intrinsics.h"
#include "cpu_check.h"
#include "aes_unavailable.h"

namespace Terra::Crypto::Cipher
{

#ifdef TERRA_USE_INTEL_INTRINSICS

// Define the AESIntel class
class AESIntel : public AESEngine
{
    protected:
        // The block size of the AES cipher is fixed at 16 octets
        static constexpr std::size_t AES_Block_Size{16};

        // Number of columns in the State array
        static constexpr std::size_t Nb{4};

        // Specify the maximum number of rounds per the standard
        static constexpr std::size_t Max_Rounds{14};

    public:
        AESIntel() noexcept;
        AESIntel(const std::span<const std::uint8_t> key);
        AESIntel(const AESIntel &other) noexcept;
        AESIntel(AESIntel &&other) noexcept;
        ~AESIntel();

        AESIntel &operator=(const AESIntel &other);
        AESIntel &operator=(AESIntel &&other) noexcept;

        AESEngineType GetEngineType() const noexcept override
        {
            if (CPUSupportsAES_NI()) return AESEngineType::Intel;

            return AESEngineType::Unavailable;
        }

        void SetKey(const std::span<const std::uint8_t> key) override;

        void ClearKeyState() override;

        void Encrypt(const std::span<const std::uint8_t, 16> plaintext,
                     std::span<std::uint8_t, 16> ciphertext) noexcept override;

        void Decrypt(const std::span<const std::uint8_t, 16> ciphertext,
                     std::span<std::uint8_t, 16> plaintext) noexcept override;

        bool operator==(const AESIntel &other) const;
        bool operator!=(const AESIntel &other) const;

    protected:
        // Number of encryption rounds
        unsigned Nr;

        // Encryption round key schedule array
        __m128i W[Max_Rounds + 1];

        // Decryption round key schedule array
        __m128i DW[Max_Rounds + 1];

        // Temporary variables used in key expansion
        __m128i T1, T2, T3, T4;
};

#else // TERRA_USE_INTEL_INTRINSICS

// If building without Intel Intrinsics, alias this engine type as unavailable

using AESIntel = AESUnavailable;

#endif // TERRA_USE_INTEL_INTRINSICS

} // namespace Terra::Crypto::Cipher

