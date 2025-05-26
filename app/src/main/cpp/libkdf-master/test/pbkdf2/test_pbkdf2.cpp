/*
 *  test_pbkdf2.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the PBKDF2 key derivation function.
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

// Tests from:
// https://github.com/junkurihara/jscu/blob/develop/packages/js-crypto-pbkdf/test/pbkdf.spec.ts
STF_TEST(PBKDF2, VariousHashFunctions)
{
    const std::vector<
        std::pair<Hashing::HashAlgorithm, std::vector<std::uint8_t>>> expected
    {
        {
            Hashing::HashAlgorithm::SHA1,
            {
                0xca, 0x95, 0xcb, 0xa5, 0x37, 0x3c, 0xa0, 0xb8,
                0x6a, 0xd1, 0xdd, 0x7b, 0x31, 0xfb, 0x51, 0xd7,
                0x7f, 0x4c, 0xdc, 0x42, 0x4c, 0x1d, 0x9b, 0x70,
                0x46, 0x20, 0xce, 0xe5, 0x05, 0xbd, 0xc7, 0x72
            }
        },
        {
            Hashing::HashAlgorithm::SHA256,
            {
                0xbf, 0x3d, 0x09, 0xd4, 0x29, 0xfb, 0xf7, 0x1b,
                0xbb, 0x38, 0x4a, 0x64, 0x21, 0x44, 0x7d, 0xa3,
                0x20, 0x96, 0xff, 0x8a, 0x01, 0x0c, 0x70, 0x42,
                0xd3, 0xe2, 0x91, 0x94, 0x23, 0x77, 0x92, 0xd2
            }
        },
        {
            Hashing::HashAlgorithm::SHA384,
            {
                0xf4, 0xe9, 0xe3, 0x37, 0x7b, 0xfd, 0xb3, 0x76,
                0x95, 0xb4, 0x1d, 0x16, 0x3b, 0x67, 0xf6, 0xa8,
                0xde, 0x01, 0x7d, 0xb9, 0x8e, 0xa6, 0x9b, 0xd5,
                0x16, 0x3f, 0x9d, 0x77, 0x1a, 0x14, 0x18, 0x78
            }
        },
        {
            Hashing::HashAlgorithm::SHA512,
            {
                0xff, 0x05, 0x5f, 0x4a, 0x1f, 0x3b, 0x9b, 0x70,
                0xde, 0x87, 0xec, 0xd9, 0x42, 0xc7, 0xae, 0xd9,
                0xd1, 0xe5, 0xb7, 0x7d, 0x5b, 0x4d, 0x36, 0xf9,
                0x23, 0x89, 0xea, 0xd9, 0xf6, 0xc3, 0x59, 0xbf
            }
        }
    };
    const std::string password = "password";
    const std::vector<std::uint8_t> salt =
    {
        0xdc, 0x04, 0xde, 0xff, 0x5a, 0x33, 0xc2, 0x2d,
        0xf3, 0xaa, 0x82, 0x08, 0x5f, 0x9c, 0x2d, 0x0f,
        0x54, 0x77, 0xaf, 0x73, 0xcd, 0x50, 0x0d, 0xfe,
        0x53, 0x16, 0x2d, 0x70, 0xba, 0x09, 0x6a, 0x03
    };
    std::vector<std::uint8_t> key(32);

    for (const auto &test : expected)
    {
        // Derive the key
        auto result = KDF::PBKDF2(
            test.first,
            {reinterpret_cast<const std::uint8_t *>(password.data()),
             password.length()},
            salt,
            2048,
            key);

        STF_ASSERT_EQ(key.data(), result.data());
        STF_ASSERT_EQ(key.size(), result.size());
        STF_ASSERT_EQ(test.second, key);
    }
}

// Test published on Wikipedia (https://en.wikipedia.org/wiki/PBKDF2)
STF_TEST(PBKDF2, Wikipedia)
{
    const std::vector<std::uint8_t> expected =
    {
        0x17, 0xEB, 0x40, 0x14, 0xC8, 0xC4, 0x61, 0xC3,
        0x00, 0xE9, 0xB6, 0x15, 0x18, 0xB9, 0xA1, 0x8B
    };
    const std::string password = "plnlrtfpijpuhqylxbgqiiyipieyxvfsavzgxbbcfusq"
                                 "kozwpngsyejqlmjsytrmd";
    const std::vector<std::uint8_t> salt =
    {
        0xA0, 0x09, 0xC1, 0xA4, 0x85, 0x91, 0x2C, 0x6A,
        0xE6, 0x30, 0xD3, 0xE7, 0x44, 0x24, 0x0B, 0x04
    };
    std::vector<std::uint8_t> key(16);

    // Derive the key
    auto result =
        KDF::PBKDF2(Hashing::HashAlgorithm::SHA1,
                    {reinterpret_cast<const std::uint8_t *>(password.data()),
                     password.length()},
                    salt,
                    1000,
                    key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// Tests from RFC 6070 (Section 2)
STF_TEST(PBKDF2, RFC6070_1)
{
    const std::vector<std::uint8_t> expected =
    {
        0x0c, 0x60, 0xc8, 0x0f, 0x96, 0x1f, 0x0e, 0x71,
        0xf3, 0xa9, 0xb5, 0x24, 0xaf, 0x60, 0x12, 0x06,
        0x2f, 0xe0, 0x37, 0xa6
    };
    const std::string password = "password";
    const std::string salt = "salt";
    std::vector<std::uint8_t> key(20);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA1,
        {reinterpret_cast<const std::uint8_t *>(password.data()), password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        1,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// Tests from RFC 6070 (Section 2)
STF_TEST(PBKDF2, RFC6070_2)
{
    const std::vector<std::uint8_t> expected =
    {
        0xea, 0x6c, 0x01, 0x4d, 0xc7, 0x2d, 0x6f, 0x8c,
        0xcd, 0x1e, 0xd9, 0x2a, 0xce, 0x1d, 0x41, 0xf0,
        0xd8, 0xde, 0x89, 0x57
    };
    const std::string password = "password";
    const std::string salt = "salt";
    std::vector<std::uint8_t> key(20);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA1,
        {reinterpret_cast<const std::uint8_t *>(password.data()),
         password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        2,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// Tests from RFC 6070 (Section 2)
STF_TEST(PBKDF2, RFC6070_3)
{
    const std::vector<std::uint8_t> expected =
    {
        0x4b, 0x00, 0x79, 0x01, 0xb7, 0x65, 0x48, 0x9a,
        0xbe, 0xad, 0x49, 0xd9, 0x26, 0xf7, 0x21, 0xd0,
        0x65, 0xa4, 0x29, 0xc1
    };
    const std::string password = "password";
    const std::string salt = "salt";
    std::vector<std::uint8_t> key(20);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA1,
        {reinterpret_cast<const std::uint8_t *>(password.data()),
         password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        4096,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// Tests from RFC 6070 (Section 2)
STF_TEST(PBKDF2, RFC6070_4)
{
    const std::vector<std::uint8_t> expected =
    {
        0xee, 0xfe, 0x3d, 0x61, 0xcd, 0x4d, 0xa4, 0xe4,
        0xe9, 0x94, 0x5b, 0x3d, 0x6b, 0xa2, 0x15, 0x8c,
        0x26, 0x34, 0xe9, 0x84
    };
    const std::string password = "password";
    const std::string salt = "salt";
    std::vector<std::uint8_t> key(20);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA1,
        {reinterpret_cast<const std::uint8_t *>(password.data()),
         password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        16777216,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// Tests from RFC 6070 (Section 2)
STF_TEST(PBKDF2, RFC6070_5)
{
    const std::vector<std::uint8_t> expected =
    {
        0x3d, 0x2e, 0xec, 0x4f, 0xe4, 0x1c, 0x84, 0x9b,
        0x80, 0xc8, 0xd8, 0x36, 0x62, 0xc0, 0xe4, 0x4a,
        0x8b, 0x29, 0x1a, 0x96, 0x4c, 0xf2, 0xf0, 0x70,
        0x38
    };
    const std::string password = "passwordPASSWORDpassword";
    const std::string salt = "saltSALTsaltSALTsaltSALTsaltSALTsalt";
    std::vector<std::uint8_t> key(25);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA1,
        {reinterpret_cast<const std::uint8_t *>(password.data()),
         password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        4096,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// Tests from RFC 6070 (Section 2)
STF_TEST(PBKDF2, RFC6070_6)
{
    const std::vector<std::uint8_t> expected =
    {
        0x56, 0xfa, 0x6a, 0xa7, 0x55, 0x48, 0x09, 0x9d,
        0xcc, 0x37, 0xd7, 0xf0, 0x34, 0x25, 0xe0, 0xc3
    };
    const std::string password = std::string("pass") + '\0' + "word";
    const std::string salt = std::string("sa") + '\0' + "lt";
    std::vector<std::uint8_t> key(16);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA1,
        {reinterpret_cast<const std::uint8_t *>(password.data()), password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        4096,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// The following tests use the same RFC 6070 test parameters, except for the
// use of SHA-256 in place of SHA-1 and the desired key length.  Test vectors
// were published on Stack Overflow here:
// https://stackoverflow.com/questions/5130513/pbkdf2-hmac-sha2-test-vectors/5130543#5130543

// Tests from RFC 6070 (Section 2)
STF_TEST(PBKDF2, RFC6070_7)
{
    const std::vector<std::uint8_t> expected =
    {
        0x12, 0x0f, 0xb6, 0xcf, 0xfc, 0xf8, 0xb3, 0x2c,
        0x43, 0xe7, 0x22, 0x52, 0x56, 0xc4, 0xf8, 0x37,
        0xa8, 0x65, 0x48, 0xc9, 0x2c, 0xcc, 0x35, 0x48,
        0x08, 0x05, 0x98, 0x7c, 0xb7, 0x0b, 0xe1, 0x7b
    };
    const std::string password = "password";
    const std::string salt = "salt";
    std::vector<std::uint8_t> key(32);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA256,
        {reinterpret_cast<const std::uint8_t *>(password.data()),
         password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        1,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// Tests from RFC 6070 (Section 2)
STF_TEST(PBKDF2, RFC6070_8)
{
    const std::vector<std::uint8_t> expected =
    {
        0xae, 0x4d, 0x0c, 0x95, 0xaf, 0x6b, 0x46, 0xd3,
        0x2d, 0x0a, 0xdf, 0xf9, 0x28, 0xf0, 0x6d, 0xd0,
        0x2a, 0x30, 0x3f, 0x8e, 0xf3, 0xc2, 0x51, 0xdf,
        0xd6, 0xe2, 0xd8, 0x5a, 0x95, 0x47, 0x4c, 0x43
    };
    const std::string password = "password";
    const std::string salt = "salt";
    std::vector<std::uint8_t> key(32);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA256,
        {reinterpret_cast<const std::uint8_t *>(password.data()),
         password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        2,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// Tests from RFC 6070 (Section 2)
STF_TEST(PBKDF2, RFC6070_9)
{
    const std::vector<std::uint8_t> expected =
    {
        0xc5, 0xe4, 0x78, 0xd5, 0x92, 0x88, 0xc8, 0x41,
        0xaa, 0x53, 0x0d, 0xb6, 0x84, 0x5c, 0x4c, 0x8d,
        0x96, 0x28, 0x93, 0xa0, 0x01, 0xce, 0x4e, 0x11,
        0xa4, 0x96, 0x38, 0x73, 0xaa, 0x98, 0x13, 0x4a
    };
    const std::string password = "password";
    const std::string salt = "salt";
    std::vector<std::uint8_t> key(32);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA256,
        {reinterpret_cast<const std::uint8_t *>(password.data()),
         password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        4096,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// Tests from RFC 6070 (Section 2)
STF_TEST(PBKDF2, RFC6070_10)
{
    const std::vector<std::uint8_t> expected =
    {
        0xcf, 0x81, 0xc6, 0x6f, 0xe8, 0xcf, 0xc0, 0x4d,
        0x1f, 0x31, 0xec, 0xb6, 0x5d, 0xab, 0x40, 0x89,
        0xf7, 0xf1, 0x79, 0xe8, 0x9b, 0x3b, 0x0b, 0xcb,
        0x17, 0xad, 0x10, 0xe3, 0xac, 0x6e, 0xba, 0x46
    };
    const std::string password = "password";
    const std::string salt = "salt";
    std::vector<std::uint8_t> key(32);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA256,
        {reinterpret_cast<const std::uint8_t *>(password.data()),
         password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        16777216,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// Tests from RFC 6070 (Section 2)
STF_TEST(PBKDF2, RFC6070_11)
{
    const std::vector<std::uint8_t> expected =
    {
        0x34, 0x8c, 0x89, 0xdb, 0xcb, 0xd3, 0x2b, 0x2f,
        0x32, 0xd8, 0x14, 0xb8, 0x11, 0x6e, 0x84, 0xcf,
        0x2b, 0x17, 0x34, 0x7e, 0xbc, 0x18, 0x00, 0x18,
        0x1c, 0x4e, 0x2a, 0x1f, 0xb8, 0xdd, 0x53, 0xe1,
        0xc6, 0x35, 0x51, 0x8c, 0x7d, 0xac, 0x47, 0xe9
    };
    const std::string password = "passwordPASSWORDpassword";
    const std::string salt = "saltSALTsaltSALTsaltSALTsaltSALTsalt";
    std::vector<std::uint8_t> key(40);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA256,
        {reinterpret_cast<const std::uint8_t *>(password.data()),
         password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        4096,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// Tests from RFC 6070 (Section 2)
STF_TEST(PBKDF2, RFC6070_12)
{
    const std::vector<std::uint8_t> expected =
    {
        0x89, 0xb6, 0x9d, 0x05, 0x16, 0xf8, 0x29, 0x89,
        0x3c, 0x69, 0x62, 0x26, 0x65, 0x0a, 0x86, 0x87
    };
    const std::string password = std::string("pass") + '\0' + "word";
    const std::string salt = std::string("sa") + '\0' + "lt";
    std::vector<std::uint8_t> key(16);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA256,
        {reinterpret_cast<const std::uint8_t *>(password.data()), password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        4096,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

// Test vectors from RFC 7914 (Section 11)

STF_TEST(PBKDF2, RFC7914_1)
{
    const std::vector<std::uint8_t> expected =
    {
        0x55, 0xac, 0x04, 0x6e, 0x56, 0xe3, 0x08, 0x9f,
        0xec, 0x16, 0x91, 0xc2, 0x25, 0x44, 0xb6, 0x05,
        0xf9, 0x41, 0x85, 0x21, 0x6d, 0xde, 0x04, 0x65,
        0xe6, 0x8b, 0x9d, 0x57, 0xc2, 0x0d, 0xac, 0xbc,
        0x49, 0xca, 0x9c, 0xcc, 0xf1, 0x79, 0xb6, 0x45,
        0x99, 0x16, 0x64, 0xb3, 0x9d, 0x77, 0xef, 0x31,
        0x7c, 0x71, 0xb8, 0x45, 0xb1, 0xe3, 0x0b, 0xd5,
        0x09, 0x11, 0x20, 0x41, 0xd3, 0xa1, 0x97, 0x83
    };
    const std::string password = "passwd";
    const std::string salt = "salt";
    std::vector<std::uint8_t> key(64);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA256,
        {reinterpret_cast<const std::uint8_t *>(password.data()),
         password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        1,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

STF_TEST(PBKDF2, RFC7914_2)
{
    const std::vector<std::uint8_t> expected =
    {
        0x4d, 0xdc, 0xd8, 0xf6, 0x0b, 0x98, 0xbe, 0x21,
        0x83, 0x0c, 0xee, 0x5e, 0xf2, 0x27, 0x01, 0xf9,
        0x64, 0x1a, 0x44, 0x18, 0xd0, 0x4c, 0x04, 0x14,
        0xae, 0xff, 0x08, 0x87, 0x6b, 0x34, 0xab, 0x56,
        0xa1, 0xd4, 0x25, 0xa1, 0x22, 0x58, 0x33, 0x54,
        0x9a, 0xdb, 0x84, 0x1b, 0x51, 0xc9, 0xb3, 0x17,
        0x6a, 0x27, 0x2b, 0xde, 0xbb, 0xa1, 0xd0, 0x78,
        0x47, 0x8f, 0x62, 0xb3, 0x97, 0xf3, 0x3c, 0x8d
    };
    const std::string password = "Password";
    const std::string salt = "NaCl";
    std::vector<std::uint8_t> key(64);

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA256,
        {reinterpret_cast<const std::uint8_t *>(password.data()),
         password.length()},
        {reinterpret_cast<const std::uint8_t *>(salt.data()), salt.length()},
        80000,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

STF_TEST(PBKDF2, VariousKnownResults)
{
    struct TestParameters
    {
        Hashing::HashAlgorithm algorithm;
        std::string password;
        std::vector<std::uint8_t> salt;
        std::size_t iterations;
        std::vector<std::uint8_t> expected;
    };

    const std::vector<TestParameters> various_tests
    {
        {
            Hashing::HashAlgorithm::SHA512,
            "TopSecret",
            {
                's', 'a', 'l', 't'
            },
            1,
            {
                0x2a, 0xf8, 0x5b, 0x06, 0x69, 0x50, 0xdb, 0xa8,
                0x0f, 0xeb, 0x51, 0x84, 0x1c, 0x61, 0xd8, 0x95,
                0xbf, 0x1a, 0x18, 0xa0, 0x2f, 0x64, 0x17, 0x10,
                0x44, 0x27, 0x5e, 0xa5, 0x21, 0x09, 0x18, 0xd3,
                0xa9, 0xf0, 0x81, 0x31, 0x68, 0xdf, 0x1c, 0x53,
                0xe6, 0x18, 0xf6, 0xaa, 0xc7, 0x4d, 0xb4, 0x9c,
                0xd3, 0x69, 0x1f, 0x66, 0x68, 0xf4, 0x57, 0xf0,
                0x5d, 0xfd, 0x2a, 0xc5, 0xc1, 0x0c, 0x56, 0xeb
            }
        },
        {
            Hashing::HashAlgorithm::SHA512,
            "TopSecret",
            {
                'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'r', 'a',
                'n', 'd', 'o', 'm', ' ', 's', 'a', 'l', 't', ' ', 'v', 'a',
                'l', 'u', 'e'
            },
            10000,
            {
                0x0f, 0xbb, 0xcb, 0x23, 0x80, 0x1a, 0x08, 0xb8,
                0x93, 0x14, 0x7b, 0x52, 0xfd, 0x4d, 0x3c, 0x32,
                0x96, 0x00, 0x47, 0x55, 0x24, 0x6d, 0x5a, 0x6a,
                0x5d, 0xe2, 0x98, 0xe7, 0x09, 0xaf, 0xec, 0x63,
                0x96, 0xce, 0xc3, 0xbd, 0x43, 0xdd, 0x35, 0x57,
                0x83, 0xbd, 0xe7, 0xef, 0x79, 0x43, 0xe2, 0xe5,
                0x07, 0x92, 0x6c, 0xf3, 0xb0, 0x21, 0xb8, 0xb2,
                0x25, 0x32, 0x10, 0x8f, 0xc4, 0x57, 0x13, 0x89
            }
        },
        {
            Hashing::HashAlgorithm::SHA512,
            "TopSecret",
            {
                'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'r', 'a',
                'n', 'd', 'o', 'm', ' ', 's', 'a', 'l', 't', ' ', 'v', 'a',
                'l', 'u', 'e'
            },
            20000,
            {
                0x2a, 0xfd, 0x59, 0x80, 0x97, 0x46, 0xbf, 0x8d,
                0x2f, 0xf3, 0xdd, 0x5c, 0x14, 0x0d, 0xb5, 0xea,
                0x49, 0x8b, 0x3e, 0xb8, 0xa4, 0xc9, 0x10, 0x29,
                0xfd, 0xf5, 0x04, 0x72, 0xf9, 0x16, 0xd5, 0x11,
                0x33, 0xd4, 0xf1, 0x91, 0x53, 0xb3, 0xb9, 0xe6,
                0x73, 0xd1, 0x45, 0xc0, 0x23, 0xb1, 0xf7, 0x3a,
                0xaa, 0xdb, 0x47, 0x2c, 0x65, 0x5f, 0x54, 0x2b,
                0xef, 0x90, 0x53, 0x52, 0x5e, 0xe5, 0xc8, 0x15
            }
        },
        {
            Hashing::HashAlgorithm::SHA512,
            "this is the secret password",
            {
                's', 'a', 'l', 't'
            },
            50000,
            {
                0xe3, 0xc1, 0x2a, 0x84, 0x1a, 0xaf, 0xa6, 0xb2,
                0x47, 0x6c, 0x09, 0xe9, 0x69, 0x25, 0x27, 0x84,
                0x30, 0xd1, 0x22, 0x2c, 0x99, 0x80, 0xb5, 0xbf,
                0x45, 0x3a, 0xd2, 0xdf, 0x83, 0xc2, 0x44, 0x70,
                0xde, 0xa0, 0xc3, 0xaf, 0x97, 0x24, 0x0c, 0x1e,
                0x2c, 0xc5, 0x3f, 0x77, 0x58, 0xd2, 0xbb, 0xef,
                0xe4, 0xf0, 0x94, 0xcd, 0x2c, 0xea, 0x0f, 0x0d,
                0xe6, 0x70, 0x6d, 0x59, 0xb7, 0xef, 0xac, 0x8a
            }
        }
    };
    std::vector<std::uint8_t> key;

    for (const auto &test : various_tests)
    {
        // Set the key to the expected size
        key.resize(test.expected.size());

        // Derive the key
        auto result = KDF::PBKDF2(
            test.algorithm,
            {reinterpret_cast<const std::uint8_t *>(test.password.data()),
             test.password.length()},
            test.salt,
            test.iterations,
            key);

        STF_ASSERT_EQ(key.data(), result.data());
        STF_ASSERT_EQ(key.size(), result.size());
        STF_ASSERT_EQ(test.expected, key);
    }
}

STF_TEST(PBKDF2, ShortKeyTest)
{
    const std::vector<std::uint8_t> expected =
    {
        0xe3, 0xc1, 0x2a, 0x84, 0x1a, 0xaf, 0xa6, 0xb2,
        0x47, 0x6c, 0x09, 0xe9, 0x69, 0x25, 0x27, 0x84,
        0x30, 0xd1, 0x22, 0x2c, 0x99, 0x80, 0xb5, 0xbf,
        0x45, 0x3a, 0xd2, 0xdf, 0x83, 0xc2, 0x44, 0x70
    };
    const std::string password = "this is the secret password";
    const std::vector<std::uint8_t> salt = {'s', 'a', 'l', 't'};
    std::vector<std::uint8_t> key(32); // Purposely shorter than SHA-512

    // Derive the key
    auto result = KDF::PBKDF2(
        Hashing::HashAlgorithm::SHA512,
        {reinterpret_cast<const std::uint8_t *>(password.data()),
         password.length()},
        salt,
        50'000,
        key);

    STF_ASSERT_EQ(key.data(), result.data());
    STF_ASSERT_EQ(key.size(), result.size());
    STF_ASSERT_EQ(expected, key);
}

#ifndef NDEBUG
// Due to large iteration values, these tests are too slow to run under a debug
STF_TEST_EXCLUDE(PBKDF2, RFC6070_4)
STF_TEST_EXCLUDE(PBKDF2, RFC6070_10)
#endif
