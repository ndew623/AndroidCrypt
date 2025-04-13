/*
 *  aes_unavailable.h
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the AESUnavailable object which serves as a
 *      stub for when specific AES engines when they are not available.
 *      For example, if one builds the code with Intel Intrinsics disabled,
 *      then the AESIntel object will be an alias of this object.
 *
 *      This helps to simplify code to avoid having conditional compilation
 *      statements.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <span>
#include <cstdint>
#include <terra/crypto/cipher/aes.h>

namespace Terra::Crypto::Cipher
{

struct AESUnavailable : public AESEngine
{
    AESUnavailable() = default;
    AESUnavailable(const std::span<const std::uint8_t>) {}
    ~AESUnavailable() = default;

    AESEngineType GetEngineType() const noexcept override
    {
        return AESEngineType::Unavailable;
    }

    void SetKey(const std::span<const std::uint8_t>) override {}

    void ClearKeyState() override {}

    void Encrypt(const std::span<const std::uint8_t, 16>,
                 std::span<std::uint8_t, 16>) noexcept override
    {
    }

    void Decrypt(const std::span<const std::uint8_t, 16>,
                 std::span<std::uint8_t, 16>) noexcept override
    {
    }

    bool operator==(const AESUnavailable &) const { return true; }

    bool operator!=(const AESUnavailable &) const { return false; }
};

} // namespace Terra::Crypto::Cipher
