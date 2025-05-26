/*
 *  test_pbkdf1.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the PBKDF1 key derivation function.
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

STF_TEST(PBKDF1, PBKDF1_0)
{
    std::vector<std::uint8_t> expected =
    {
    };
    std::string password = "password";
    std::string salt = "salt";
    std::vector<std::uint8_t> key; // zero-length key

    // Derive the key
    auto result = KDF::PBKDF1(
        Hashing::HashAlgorithm::SHA256,
        {reinterpret_cast<std::uint8_t *>(password.data()), password.length()},
        {reinterpret_cast<std::uint8_t *>(salt.data()), salt.length()},
        1024,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(0, result.size());
}

// Test from https://www.cryptopp.com/wiki/PKCS5_PBKDF1
STF_TEST(PBKDF1, PBKDF1_1)
{
    std::vector<std::uint8_t> expected =
    {
        0xe2, 0xaf, 0x0a, 0xab, 0xf7, 0xc9, 0xb5, 0x3d,
        0x81, 0x58, 0x76, 0xae, 0xee, 0x57, 0x8f, 0x56,
        0xf4, 0x3d, 0xb6, 0xea, 0xb4, 0x4d, 0xfd, 0x20,
        0x7e, 0x83, 0x56, 0x6c, 0x99, 0xf5, 0x8b, 0xdd
    };
    std::string password = "password";
    std::string salt = "salt";
    std::vector<std::uint8_t> key(32);

    // Derive the key
    auto result = KDF::PBKDF1(
        Hashing::HashAlgorithm::SHA256,
        {reinterpret_cast<std::uint8_t *>(password.data()), password.length()},
        {reinterpret_cast<std::uint8_t *>(salt.data()), salt.length()},
        1024,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(32, result.size());
    STF_ASSERT_EQ(expected, key);
}

// Test from https://www.cryptopp.com/wiki/PKCS5_PBKDF1
STF_TEST(PBKDF1, PBKDF1_2)
{
    std::vector<std::uint8_t> expected =
    {
        0xc6, 0x63, 0xfc, 0x66, 0xcb, 0x19, 0xf0, 0x2f,
        0x5f, 0x17, 0xa3, 0x11, 0x6e, 0x74, 0x27, 0xd6
    };
    std::string password = "password";
    std::string salt = "PKCS5_PBKDF1 key derivation";
    std::vector<std::uint8_t> key(16);

    // Derive the key
    auto result = KDF::PBKDF1(
        Hashing::HashAlgorithm::SHA256,
        {reinterpret_cast<std::uint8_t *>(password.data()), password.length()},
        {reinterpret_cast<std::uint8_t *>(salt.data()), salt.length()},
        1024,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(16, result.size());
    STF_ASSERT_EQ(expected, key);
}

// Test from https://www.cryptopp.com/wiki/PKCS5_PBKDF1
STF_TEST(PBKDF1, PBKDF1_3)
{
    std::vector<std::uint8_t> expected =
    {
        0x7f, 0xb9, 0xd3, 0xb9, 0xad, 0xdd, 0x8f, 0x98,
        0x2d, 0xc9, 0x82, 0x51, 0x6a, 0x86, 0x7d, 0x9a
    };
    std::string password = "password";
    std::string salt = "PKCS5_PBKDF1 iv derivation";
    std::vector<std::uint8_t> key(16);

    // Derive the key
    auto result = KDF::PBKDF1(
        Hashing::HashAlgorithm::SHA256,
        {reinterpret_cast<std::uint8_t *>(password.data()), password.length()},
        {reinterpret_cast<std::uint8_t *>(salt.data()), salt.length()},
        1024,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(16, result.size());
    STF_ASSERT_EQ(expected, key);
}

// Tests from:
// https://github.com/junkurihara/jscu/blob/develop/packages/js-crypto-pbkdf/test/pbkdf.spec.ts
STF_TEST(PBKDF1, PBKDF1_4)
{
    std::vector<
        std::pair<Hashing::HashAlgorithm, std::vector<std::uint8_t>>> expected
    {
        {
            Hashing::HashAlgorithm::SHA1,
            {
                0x55, 0xce, 0x9e, 0x9a, 0xa9, 0xbf, 0x73, 0x3f,
                0x19, 0x3e, 0x66, 0x62, 0x03, 0x65, 0xfe, 0x0e
            }
        },
        {
            Hashing::HashAlgorithm::SHA256,
            {
                0x2e, 0x46, 0x00, 0x82, 0xf6, 0x00, 0x2d, 0x37,
                0x70, 0x42, 0xbb, 0xfd, 0x7c, 0x3f, 0xcf, 0x61
            }
        },
        {
            Hashing::HashAlgorithm::SHA384,
            {
                0xbc, 0x3d, 0x24, 0x1b, 0xd9, 0x97, 0x5b, 0xab,
                0xb7, 0xbb, 0x7f, 0xd0, 0xc8, 0x43, 0xc9, 0xe0
            }
        },
        {
            Hashing::HashAlgorithm::SHA512,
            {
                0x2b, 0x87, 0x5a, 0x1a, 0x16, 0x3a, 0xde, 0x05,
                0xf7, 0x88, 0xba, 0x38, 0x60, 0x33, 0xc2, 0x0a
            }
        }
    };
    std::string password = "password";
    std::vector<std::uint8_t> salt =
    {
        0xdc, 0x04, 0xde, 0xff, 0x5a, 0x33, 0xc2, 0x2d,
        0xf3, 0xaa, 0x82, 0x08, 0x5f, 0x9c, 0x2d, 0x0f,
        0x54, 0x77, 0xaf, 0x73, 0xcd, 0x50, 0x0d, 0xfe,
        0x53, 0x16, 0x2d, 0x70, 0xba, 0x09, 0x6a, 0x03
    };
    std::vector<std::uint8_t> key(16);

    for (const auto &test : expected)
    {
        // Derive the key
        auto result =
            KDF::PBKDF1(test.first,
                        {reinterpret_cast<std::uint8_t *>(password.data()),
                         password.length()},
                        salt,
                        2048,
                        key);

        STF_ASSERT_EQ(key.data(), result.data());
        STF_ASSERT_EQ(key.size(), result.size());
        STF_ASSERT_EQ(test.second, key);
    }
}
