/*
 *  engine_common.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines values that are common between the Encryptor and
 *      Decryptor objects.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <terra/crypto/hashing/hash.h>

namespace
{

// Recommended number of PBKDF2 iterations to employ for AES Crypt stream
// format version 3 for typical passwords
constexpr std::size_t PBKDF2_Iterations = 300'000;

// Bound the number of iterations to ensure values are valid and does not cause
// the process to hang for an exceedingly long period of time; if the stream
// contains an iteration value beyond PBKDF2_Max_Iterations, it will be treated
// as an error
constexpr std::size_t PBKDF2_Min_Iterations = 1;
constexpr std::size_t PBKDF2_Max_Iterations = 5'000'000;

// The hashing algorithm to employ with PBKDF2
constexpr Terra::Crypto::Hashing::HashAlgorithm PBKDF2_Hash_Algorithm =
    Terra::Crypto::Hashing::HashAlgorithm::SHA512;

// Indicate the latest defined AES Crypt stream version
constexpr std::uint8_t Latest_AES_Crypt_Stream_Version = 3;

/*
 *  CreateComponent()
 *
 *  Description:
 *      This is a utility function that will create the component string
 *      used in the construction of the child Logger object.
 *
 *  Parameters:
 *      component [in]
 *          The component name.
 *
 *      instance [in]
 *          The instance string for this component.
 *
 *  Returns:
 *      The string to use as the component parameter to the Logger.
 *
 *  Comments:
 *      This can be made constexpr in C++20, but some newish compilers do not
 *      yet support constexpr string functions like this.
 */
static inline std::string CreateComponent(const std::string &component,
                                          const std::string &instance)
{
    return instance.empty() ? component : component + ":" + instance;
}

/*
 *  XORBlock()
 *
 *  Description:
 *      This function will the two given spans.  This is used as a part of
 *      the CBC operation.
 *
 *  Parameters:
 *      a [in]
 *          This is one of the input spans.
 *
 *      b [in]
 *          This is one of the input spans.
 *
 *      result [out]
 *          This is the output span (which may point to the same memory
 *          location as either a or b).
 *
 *  Returns:
 *      Nothing, though the result of the XOR operation will be stored in
 *      the result span.
 *
 *  Comments:
 *      None.
 */
constexpr void XORBlock(const std::span<const std::uint8_t, 16> a,
                        const std::span<const std::uint8_t, 16> b,
                        std::span<std::uint8_t, 16> result)
{
    result[0]  = a[0]  ^ b[0];
    result[1]  = a[1]  ^ b[1];
    result[2]  = a[2]  ^ b[2];
    result[3]  = a[3]  ^ b[3];
    result[4]  = a[4]  ^ b[4];
    result[5]  = a[5]  ^ b[5];
    result[6]  = a[6]  ^ b[6];
    result[7]  = a[7]  ^ b[7];
    result[8]  = a[8]  ^ b[8];
    result[9]  = a[9]  ^ b[9];
    result[10] = a[10] ^ b[10];
    result[11] = a[11] ^ b[11];
    result[12] = a[12] ^ b[12];
    result[13] = a[13] ^ b[13];
    result[14] = a[14] ^ b[14];
    result[15] = a[15] ^ b[15];
}

} // namespace
