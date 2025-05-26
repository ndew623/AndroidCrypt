/*
 *  test_ackdf.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the ACKDF key derivation function.
 *
 *  Portability Issues:
 *      None.
 */

#include <string>
#include <vector>
#include <ostream>
#include <sstream>
#include <terra/crypto/kdf/pbkdf.h>
#include <terra/stf/adapters/integral_vector.h>
#include <terra/stf/stf.h>

using namespace Terra::Crypto;

std::string ASCII2UTF16LE(std::string input)
{
    std::string output;

    for (auto value : input)
    {
        output += value;
        output += '\0';
    }

    return output;
}

STF_TEST(ACKDF, ACKDF_1)
{
    std::vector<std::uint8_t> expected =
    {
        0xe1, 0x9b, 0xf7, 0x92, 0xf2, 0x69, 0xaa, 0xf2,
        0xab, 0x59, 0xb8, 0x0c, 0xb4, 0x9e, 0x38, 0xab,
        0xf2, 0xca, 0x3a, 0x11, 0x5a, 0xda, 0x28, 0xb8,
        0x06, 0x9f, 0xbe, 0x2a, 0x7a, 0x40, 0x4c, 0xe0
    };
    std::vector<std::uint8_t> salt =
    {
        0x47, 0xc8, 0x3a, 0x8e, 0xb6, 0x49, 0xac, 0x43,
        0x1e, 0x87, 0x20, 0x5a, 0xee, 0x18, 0x99, 0x2a
    };
    std::vector<std::uint8_t> key(32);
    std::string password = "hello";

    // Convert the ASCII string to UTF16-LE
    password = ASCII2UTF16LE(password);

    // Derive the key
    auto result = KDF::ACKDF(
        {reinterpret_cast<std::uint8_t *>(password.data()), password.length()},
        salt,
        key);

    STF_ASSERT_EQ(result.size(), key.size());
    STF_ASSERT_EQ(result.data(), key.data());
    STF_ASSERT_EQ(expected, key);
}

STF_TEST(ACKDF, ACKDF_2)
{
    std::vector<std::uint8_t> expected =
    {
        0x97, 0xd8, 0x79, 0x0c, 0x79, 0xdc, 0xc8, 0xce,
        0xce, 0x14, 0xb1, 0x56, 0xba, 0x9a, 0xbc, 0x90,
        0x38, 0x96, 0x96, 0x46, 0x45, 0x66, 0xca, 0x0e,
        0x2b, 0x17, 0x91, 0xd2, 0x13, 0xb6, 0x60, 0x38
    };
    std::vector<std::uint8_t> salt =
    {
        0x47, 0xc8, 0x3a, 0x8e, 0xb6, 0x49, 0xac, 0x43,
        0x1e, 0x87, 0x20, 0x5a, 0xee, 0x18, 0x99, 0x2a
    };
    std::vector<std::uint8_t> key(32);
    std::string password = "test";

    // Convert the ASCII string to UTF16-LE
    password = ASCII2UTF16LE(password);

    // Derive the key
    auto result = KDF::ACKDF(
        {reinterpret_cast<std::uint8_t *>(password.data()), password.length()},
        salt,
        key);

    STF_ASSERT_EQ(result.size(), key.size());
    STF_ASSERT_EQ(result.data(), key.data());
    STF_ASSERT_EQ(expected, key);
}

STF_TEST(ACKDF, ACKDF_3)
{
    std::vector<std::uint8_t> expected =
    {
        0x12, 0x34, 0x87, 0x48, 0x77, 0x8b, 0xbd, 0xde,
        0xef, 0x79, 0xfb, 0xbf, 0xed, 0xcb, 0x78, 0x49,
        0xd7, 0xbf, 0x3b, 0x1d, 0xfa, 0x75, 0xff, 0x18,
        0x7e, 0xb0, 0x06, 0x56, 0xd5, 0x1b, 0x18, 0xae
    };
    std::vector<std::uint8_t> salt =
    {
        0x47, 0xc8, 0x3a, 0x8e, 0xb6, 0x49, 0xac, 0x43,
        0x1e, 0x87, 0x20, 0x5a, 0xee, 0x18, 0x99, 0x2a
    };
    std::vector<std::uint8_t> key(32);
    std::string password = "alphabet";

    // Convert the ASCII string to UTF16-LE
    password = ASCII2UTF16LE(password);

    // Derive the key
    auto result = KDF::ACKDF(
        {reinterpret_cast<std::uint8_t *>(password.data()), password.length()},
        salt,
        key);

    STF_ASSERT_EQ(result.size(), key.size());
    STF_ASSERT_EQ(result.data(), key.data());
    STF_ASSERT_EQ(expected, key);
}

STF_TEST(ACKDF, ACKDF_4)
{
    std::vector<std::uint8_t> expected =
    {
        0x1d, 0xaa, 0x99, 0x92, 0x3c, 0xc2, 0x7a, 0xe2,
        0xab, 0x8f, 0x84, 0xc1, 0x21, 0xc4, 0x67, 0x2d,
        0x4b, 0xbd, 0xea, 0xcc, 0x6c, 0xfd, 0xc4, 0x59,
        0x22, 0xfc, 0xde, 0x2e, 0x5b, 0xe1, 0x2f, 0x44
    };
    std::vector<std::uint8_t> salt =
    {
        0x47, 0xc8, 0x3a, 0x8e, 0xb6, 0x49, 0xac, 0x43,
        0x1e, 0x87, 0x20, 0x5a, 0xee, 0x18, 0x99, 0x2a
    };
    std::vector<std::uint8_t> key(32);
    std::string password = "password";

    // Convert the ASCII string to UTF16-LE
    password = ASCII2UTF16LE(password);

    // Derive the key
    auto result = KDF::ACKDF(
        {reinterpret_cast<std::uint8_t *>(password.data()), password.length()},
        salt,
        key);

    STF_ASSERT_EQ(result.size(), key.size());
    STF_ASSERT_EQ(result.data(), key.data());
    STF_ASSERT_EQ(expected, key);
}

STF_TEST(ACKDF, ACKDF_5)
{
    std::vector<std::uint8_t> expected =
    {
        0x12, 0xfe, 0xc4, 0xaa, 0x9a, 0x30, 0xdd, 0xfc,
        0xed, 0x10, 0x2f, 0x17, 0x9b, 0x88, 0x11, 0x21,
        0xf7, 0xfb, 0x3d, 0x11, 0x3b, 0x71, 0x15, 0xf5,
        0x6b, 0xd9, 0xec, 0x1b, 0xe7, 0x2a, 0x59, 0xb9
    };
    std::vector<std::uint8_t> salt =
    {
        0xac, 0x0c, 0x31, 0xbb, 0xf9, 0x79, 0x69, 0xa3,
        0xda, 0x46, 0x80, 0x7e, 0xb7, 0xda, 0xb0, 0x39
    };
    std::vector<std::uint8_t> key(32);
    std::string password = "hello";

    // Convert the ASCII string to UTF16-LE
    password = ASCII2UTF16LE(password);

    // Derive the key
    auto result = KDF::ACKDF(
        {reinterpret_cast<std::uint8_t *>(password.data()), password.length()},
        salt,
        key);

    STF_ASSERT_EQ(result.size(), key.size());
    STF_ASSERT_EQ(result.data(), key.data());
    STF_ASSERT_EQ(expected, key);
}

STF_TEST(ACKDF, ACKDF_6)
{
    std::vector<std::uint8_t> expected =
    {
        0x08, 0xa8, 0x43, 0x73, 0xbb, 0x7a, 0xbd, 0x4d,
        0xd0, 0x80, 0xf8, 0xfa, 0x72, 0xaa, 0x66, 0xb9,
        0x8c, 0xe4, 0x75, 0x48, 0x3c, 0x95, 0xd8, 0xd9,
        0x09, 0x8f, 0xc5, 0x18, 0x56, 0xdb, 0x42, 0x74
    };
    std::vector<std::uint8_t> salt =
    {
        0xac, 0x0c, 0x31, 0xbb, 0xf9, 0x79, 0x69, 0xa3,
        0xda, 0x46, 0x80, 0x7e, 0xb7, 0xda, 0xb0, 0x39
    };
    std::vector<std::uint8_t> key(32);
    std::string password = "test";

    // Convert the ASCII string to UTF16-LE
    password = ASCII2UTF16LE(password);

    // Derive the key
    auto result = KDF::ACKDF(
        {reinterpret_cast<std::uint8_t *>(password.data()), password.length()},
        salt,
        key);

    STF_ASSERT_EQ(result.size(), key.size());
    STF_ASSERT_EQ(result.data(), key.data());
    STF_ASSERT_EQ(expected, key);
}

STF_TEST(ACKDF, ACKDF_7)
{
    std::vector<std::uint8_t> expected =
    {
        0x79, 0x7b, 0x47, 0x36, 0x63, 0xc9, 0xc3, 0x3d,
        0x4f, 0x24, 0x1c, 0x40, 0x3f, 0x17, 0x18, 0xa4,
        0xa6, 0xfd, 0x51, 0x15, 0xf7, 0x08, 0x7c, 0x77,
        0x9d, 0x43, 0x5f, 0xd4, 0xef, 0x56, 0x0c, 0xa8
    };
    std::vector<std::uint8_t> salt =
    {
        0xac, 0x0c, 0x31, 0xbb, 0xf9, 0x79, 0x69, 0xa3,
        0xda, 0x46, 0x80, 0x7e, 0xb7, 0xda, 0xb0, 0x39
    };
    std::vector<std::uint8_t> key(32);
    std::string password = "alphabet";

    // Convert the ASCII string to UTF16-LE
    password = ASCII2UTF16LE(password);

    // Derive the key
    auto result = KDF::ACKDF(
        {reinterpret_cast<std::uint8_t *>(password.data()), password.length()},
        salt,
        key);

    STF_ASSERT_EQ(result.size(), key.size());
    STF_ASSERT_EQ(result.data(), key.data());
    STF_ASSERT_EQ(expected, key);
}

STF_TEST(ACKDF, ACKDF_8)
{
    std::vector<std::uint8_t> expected =
    {
        0x05, 0xc3, 0x98, 0x4b, 0xa0, 0xd5, 0x34, 0x6b,
        0xd3, 0x3a, 0xb9, 0x35, 0x97, 0x22, 0x95, 0xae,
        0x6d, 0xc6, 0x91, 0x93, 0x5e, 0xb8, 0xf2, 0xbe,
        0x75, 0x4d, 0x25, 0x7d, 0x5b, 0x63, 0xca, 0x76
    };
    std::vector<std::uint8_t> salt =
    {
        0xac, 0x0c, 0x31, 0xbb, 0xf9, 0x79, 0x69, 0xa3,
        0xda, 0x46, 0x80, 0x7e, 0xb7, 0xda, 0xb0, 0x39
    };
    std::vector<std::uint8_t> key(32);
    std::string password = "password";

    // Convert the ASCII string to UTF16-LE
    password = ASCII2UTF16LE(password);

    // Derive the key
    auto result = KDF::ACKDF(
        {reinterpret_cast<std::uint8_t *>(password.data()), password.length()},
        salt,
        key);

    STF_ASSERT_EQ(result.size(), key.size());
    STF_ASSERT_EQ(result.data(), key.data());
    STF_ASSERT_EQ(expected, key);
}
