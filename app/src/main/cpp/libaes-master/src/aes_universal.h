/*
 *  aes_universal.h
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the AESUniversal object that performs encryption and
 *      decryption as specified in FIPS 197 ("Advanced Encryption Standard").
 *
 *      Note that this code does not utilize older C-style macros, instead
 *      opting for constexpr functions.  While this makes the code more
 *      readable and safe, performance suffers unless this library is built
 *      with compiler optimizations enabled.
 *
 *      This implementation of AES is called "universal" as it can operate on
 *      any processor and is not dependent on processor-specific instructions.
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

namespace Terra::Crypto::Cipher
{

// Define the AESUniversal class
class AESUniversal : public AESEngine
{
    protected:
        // The block size of the AES cipher is fixed at 16 octets
        static constexpr std::size_t AES_Block_Size{16};

        // Number of columns in the State array
        static constexpr std::size_t Nb{4};

        // Specify the maximum number of rounds per the standard
        static constexpr std::size_t Max_Rounds{14};

    public:
        AESUniversal() noexcept;
        AESUniversal(const std::span<const std::uint8_t> key);
        AESUniversal(const AESUniversal &other) noexcept;
        AESUniversal(AESUniversal &&other) noexcept;
        ~AESUniversal();

        AESUniversal &operator=(const AESUniversal &other);
        AESUniversal &operator=(AESUniversal &&other) noexcept;

        AESEngineType GetEngineType() const noexcept override
        {
            return AESEngineType::Universal;
        }

        void SetKey(const std::span<const std::uint8_t> key) override;

        void ClearKeyState() override;

        void Encrypt(
            const std::span<const std::uint8_t, AES_Block_Size> plaintext,
            std::span<std::uint8_t, AES_Block_Size> ciphertext) noexcept
            override;

        void Decrypt(
            const std::span<const std::uint8_t, AES_Block_Size> ciphertext,
            std::span<std::uint8_t, AES_Block_Size> plaintext) noexcept
            override;

        bool operator==(const AESUniversal &other) const;
        bool operator!=(const AESUniversal &other) const;

    protected:
        unsigned Nr;                            // Number of encryption rounds
        unsigned Nk;                            // 32-bit words in cipher key

        // State array of four columns
        std::uint_fast32_t state[4];

        // Alternating state array (temporary use during encryption/decryption)
        std::uint_fast32_t alt_state[4];

        // Encryption round key schedule array
        std::uint_fast32_t W[Nb * (Max_Rounds + 1)];

        // Decryption round key schedule array
        std::uint_fast32_t DW[Nb * (Max_Rounds + 1)];
};

} // namespace Terra::Crypto::Cipher
