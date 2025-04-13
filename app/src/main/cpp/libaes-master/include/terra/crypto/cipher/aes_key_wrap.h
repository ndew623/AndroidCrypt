/*
 *  aes_key_wrap.h
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the AESKeyWrap object that implements AES Key Wrap
 *      (RFC 3394) and AES Key Wrap with Padding (RFC 5649).  This code
 *      relies on the AES object to actually perform AES encryption and
 *      decryption.  Note that invalid span or key lengths will cause an
 *      exception to be thrown.
 *
 *      These routines are also documented in NIST Special Publication 800-38F.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include "aes.h"

namespace Terra::Crypto::Cipher
{

class AESKeyWrap
{
    public:
        // The default IV per RFC 3394
        static constexpr std::uint8_t AES_Key_Wrap_Default_IV[8] =
        {
            0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6
        };

        // AIV per RFC 5649
        static constexpr std::uint8_t Alternative_IV[4] =
        {
            0xA6, 0x59, 0x59, 0xA6
        };

        // The maximum plaintext length for Key Wrap with Padding
        static constexpr std::size_t AES_Key_Wrap_with_Padding_Max{0xFFFFFFFF};

        AESKeyWrap();
        AESKeyWrap(const std::span<const std::uint8_t> key);
        ~AESKeyWrap();

        void SetKey(const std::span<const std::uint8_t> key);

        void Wrap(const std::span<const std::uint8_t> plaintext,
                  std::span<std::uint8_t> ciphertext,
                  const std::span<const std::uint8_t> alternative_iv = {});
        bool Unwrap(const std::span<const std::uint8_t> ciphertext,
                    std::span<std::uint8_t> plaintext,
                    std::span<std::uint8_t> integrity_data = {},
                    const std::span<const std::uint8_t> alternative_iv = {});

        std::size_t WrapWithPadding(
                    const std::span<const std::uint8_t> plaintext,
                    std::span<std::uint8_t> ciphertext,
                    const std::span<const std::uint8_t> alternative_iv = {});
        std::size_t UnwrapWithPadding(
                    const std::span<const std::uint8_t> ciphertext,
                    std::span<std::uint8_t> plaintext,
                    const std::span<const std::uint8_t> alternative_iv = {});

    protected:
        AES aes;                                // AES block cipher

        std::size_t i, j, k;                    // Loop counter
        std::size_t n;                          // Number of 64-bit blocks
        std::size_t t, tt;                      // Step counters
        std::uint8_t *A;                        // Integrity check register
        std::uint8_t B[16];                     // Buffer to encrypt/decrypt
        std::uint8_t *R;                        // Pointer to register i

        std::uint32_t network_word;             // Word in network byte order

        std::size_t padding_length;             // Number of padding octets

        std::uint8_t integrity_data[8];         // Integrity data
        std::uint32_t message_length_indicator; // Message length indicator
        std::uint8_t plaintext_buffer[16];      // Plaintext for one block
};

} // namespace Terra::Crypto::Cipher
