/*
 *  test_hmac_sha.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the HMAC object to ensure that it will
 *      properly produce an HMAC-SHA-{1, 256, 384, 512} value.  It will also
 *      exercise the various APIs to ensure they are performing properly.
 *
 *  Portability Issues:
 *      None.
 */

#include <terra/stf/stf.h>
#include <terra/crypto/hashing/hmac.h>

using namespace Terra::Crypto::Hashing;

STF_TEST(HMAC, SHA1_Test1)
{
    std::string key = "key";
    std::string input = "The quick brown fox jumps over the lazy dog";
    std::string expected = "de7c9b85 b8b78aa6 bc8a7a36 f70a9070 1c9db4d9";

    HMAC hmac(HashAlgorithm::SHA1, key);
    hmac.Input(input);
    hmac.Finalize();

    std::string result = hmac.Result();

    STF_ASSERT_EQ(expected, result);
}

STF_TEST(HMAC, SHA256_Test1)
{
    std::string key = "key";
    std::string input = "The quick brown fox jumps over the lazy dog";
    std::string expected = "f7bc83f4 30538424 b13298e6 aa6fb143 "
                           "ef4d59a1 49461759 97479dbc 2d1a3cd8";

    HMAC hmac(HashAlgorithm::SHA256, key);
    hmac.Input(input);
    hmac.Finalize();

    std::string result = hmac.Result();

    STF_ASSERT_EQ(expected, result);
}

// From RFC 2202 section 3 (Test Case 1)
STF_TEST(HMAC, TestRFC2202_Section_3_Test1)
{
    std::uint8_t result[32];
    std::uint8_t key[] =
    {
        0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
        0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
    };
    std::uint8_t hmac_expected[] =
    {
        0xb6, 0x17, 0x31, 0x86, 0x55, 0x05, 0x72, 0x64,
        0xe2, 0x8b, 0xc0, 0xb6, 0xfb, 0x37, 0x8c, 0x8e,
        0xf1, 0x46, 0xbe, 0x00
    };

    HMAC hmac(HashAlgorithm::SHA1, key);
    hmac << "Hi There";
    hmac.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_expected), hmac.Result(result).size());

    STF_ASSERT_MEM_EQ(hmac_expected, result, sizeof(hmac_expected));
}

// From RFC 2202 section 3 (Test Case 2)
STF_TEST(HMAC, TestRFC2202_Section_3_Test2)
{
    std::uint8_t result[32];
    std::uint8_t key[] =
    {
        'J', 'e', 'f', 'e'
    };
    std::uint8_t hmac_expected[] =
    {
        0xef, 0xfc, 0xdf, 0x6a, 0xe5, 0xeb, 0x2f, 0xa2,
        0xd2, 0x74, 0x16, 0xd5, 0xf1, 0x84, 0xdf, 0x9c,
        0x25, 0x9a, 0x7c, 0x79
    };

    HMAC hmac(HashAlgorithm::SHA1, key);
    hmac << "what do ya want for nothing?";
    hmac.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_expected), hmac.Result(result).size());

    STF_ASSERT_MEM_EQ(hmac_expected, result, sizeof(hmac_expected));
}

// From RFC 2202 section 3 (Test Case 3)
STF_TEST(HMAC, TestRFC2202_Section_3_Test3)
{
    std::uint8_t result[32];
    std::uint8_t key[] =
    {
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa
    };
    std::uint8_t hmac_expected[] =
    {
        0x12, 0x5d, 0x73, 0x42, 0xb9, 0xac, 0x11, 0xcd,
        0x91, 0xa3, 0x9a, 0xf4, 0x8a, 0xa1, 0x7b, 0x4f,
        0x63, 0xf1, 0x75, 0xd3
    };
    std::uint8_t data[] =
    {
        0xdd // 50 times
    };

    HMAC hmac(HashAlgorithm::SHA1, key);
    for (std::size_t i = 0; i < 50; i++) hmac.Input(data);
    hmac.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_expected), hmac.Result(result).size());

    STF_ASSERT_MEM_EQ(hmac_expected, result, sizeof(hmac_expected));
}

// From RFC 2202 section 3 (Test Case 4)
STF_TEST(HMAC, TestRFC2202_Section_3_Test4)
{
    std::uint8_t result[32];
    std::uint8_t key[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19
    };
    std::uint8_t hmac_expected[] =
    {
        0x4c, 0x90, 0x07, 0xf4, 0x02, 0x62, 0x50, 0xc6,
        0xbc, 0x84, 0x14, 0xf9, 0xbf, 0x50, 0xc8, 0x6c,
        0x2d, 0x72, 0x35, 0xda
    };
    std::uint8_t data[] =
    {
        0xcd // 50 times
    };

    HMAC hmac(HashAlgorithm::SHA1, key);
    for (std::size_t i = 0; i < 50; i++) hmac.Input(data);
    hmac.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_expected), hmac.Result(result).size());

    STF_ASSERT_MEM_EQ(hmac_expected, result, sizeof(hmac_expected));
}

// From RFC 2202 section 3 (Test Case 5)
STF_TEST(HMAC, TestRFC2202_Section_3_Test5)
{
    std::uint8_t result[32];
    std::uint8_t key[] =
    {
        0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
        0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
        0x0c, 0x0c, 0x0c, 0x0c
    };
    std::uint8_t hmac_expected[] =
    {
        0x4c, 0x1a, 0x03, 0x42, 0x4b, 0x55, 0xe0, 0x7f,
        0xe7, 0xf2, 0x7b, 0xe1, 0xd5, 0x8b, 0xb9, 0x32,
        0x4a, 0x9a, 0x5a, 0x04
    };

    HMAC hmac(HashAlgorithm::SHA1, key);
    hmac << "Test With Truncation";
    hmac.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_expected), hmac.Result(result).size());

    STF_ASSERT_MEM_EQ(hmac_expected, result, sizeof(hmac_expected));
}

// From RFC 2202 section 3 (Test Case 6)
STF_TEST(HMAC, TestRFC2202_Section_3_Test6)
{
    std::uint8_t result[32];
    std::uint8_t key[80];
    for (std::size_t i = 0; i < 80; i++) key[i] = 0xaa;
    std::uint8_t hmac_expected[] =
    {
        0xaa, 0x4a, 0xe5, 0xe1, 0x52, 0x72, 0xd0, 0x0e,
        0x95, 0x70, 0x56, 0x37, 0xce, 0x8a, 0x3b, 0x55,
        0xed, 0x40, 0x21, 0x12
    };

    HMAC hmac(HashAlgorithm::SHA1, key);
    hmac << "Test Using Larger Than Block-Size Key - Hash Key First";
    hmac.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_expected), hmac.Result(result).size());

    STF_ASSERT_MEM_EQ(hmac_expected, result, sizeof(hmac_expected));
}

// From RFC 2202 section 3 (Test Case 7)
STF_TEST(HMAC, TestRFC2202_Section_3_Test7)
{
    std::uint8_t result[32];
    std::uint8_t key[80];
    for (std::size_t i = 0; i < 80; i++) key[i] = 0xaa;
    std::uint8_t hmac_expected[] =
    {
        0xe8, 0xe9, 0x9d, 0x0f, 0x45, 0x23, 0x7d, 0x78,
        0x6d, 0x6b, 0xba, 0xa7, 0x96, 0x5c, 0x78, 0x08,
        0xbb, 0xff, 0x1a, 0x91
    };

    HMAC hmac(HashAlgorithm::SHA1, key);
    hmac << "Test Using Larger Than Block-Size Key and Larger Than One "
            "Block-Size Data";
    hmac.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_expected), hmac.Result(result).size());

    STF_ASSERT_MEM_EQ(hmac_expected, result, sizeof(hmac_expected));
}

// From RFC 4231 section 4.2 (Test Case 1)
STF_TEST(HMAC, TestRFC2202_Section_4_2)
{
    std::uint8_t result[64];
    std::uint8_t key[] =
    {
        0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
        0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
    };
    std::uint8_t hmac_sha_224_expected[] =
    {
        0x89, 0x6f, 0xb1, 0x12, 0x8a, 0xbb, 0xdf, 0x19, 0x68, 0x32,
        0x10, 0x7c, 0xd4, 0x9d, 0xf3, 0x3f, 0x47, 0xb4, 0xb1, 0x16,
        0x99, 0x12, 0xba, 0x4f, 0x53, 0x68, 0x4b, 0x22
    };
    std::uint8_t hmac_sha_256_expected[] =
    {
        0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53,
        0x5c, 0xa8, 0xaf, 0xce, 0xaf, 0x0b, 0xf1, 0x2b,
        0x88, 0x1d, 0xc2, 0x00, 0xc9, 0x83, 0x3d, 0xa7,
        0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32, 0xcf, 0xf7
    };
    std::uint8_t hmac_sha_384_expected[] =
    {
        0xaf, 0xd0, 0x39, 0x44, 0xd8, 0x48, 0x95, 0x62,
        0x6b, 0x08, 0x25, 0xf4, 0xab, 0x46, 0x90, 0x7f,
        0x15, 0xf9, 0xda, 0xdb, 0xe4, 0x10, 0x1e, 0xc6,
        0x82, 0xaa, 0x03, 0x4c, 0x7c, 0xeb, 0xc5, 0x9c,
        0xfa, 0xea, 0x9e, 0xa9, 0x07, 0x6e, 0xde, 0x7f,
        0x4a, 0xf1, 0x52, 0xe8, 0xb2, 0xfa, 0x9c, 0xb6
    };
    std::uint8_t hmac_sha_512_expected[] =
    {
        0x87, 0xaa, 0x7c, 0xde, 0xa5, 0xef, 0x61, 0x9d,
        0x4f, 0xf0, 0xb4, 0x24, 0x1a, 0x1d, 0x6c, 0xb0,
        0x23, 0x79, 0xf4, 0xe2, 0xce, 0x4e, 0xc2, 0x78,
        0x7a, 0xd0, 0xb3, 0x05, 0x45, 0xe1, 0x7c, 0xde,
        0xda, 0xa8, 0x33, 0xb7, 0xd6, 0xb8, 0xa7, 0x02,
        0x03, 0x8b, 0x27, 0x4e, 0xae, 0xa3, 0xf4, 0xe4,
        0xbe, 0x9d, 0x91, 0x4e, 0xeb, 0x61, 0xf1, 0x70,
        0x2e, 0x69, 0x6c, 0x20, 0x3a, 0x12, 0x68, 0x54
    };

    HMAC hmac_sha224(HashAlgorithm::SHA224, key);
    hmac_sha224 << "Hi There";
    hmac_sha224.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_224_expected),
                  hmac_sha224.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_224_expected,
                      result,
                      sizeof(hmac_sha_224_expected));

    HMAC hmac_sha256(HashAlgorithm::SHA256, key);
    hmac_sha256 << "Hi There";
    hmac_sha256.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_256_expected),
                  hmac_sha256.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_256_expected,
                      result,
                      sizeof(hmac_sha_256_expected));

    HMAC hmac_sha384(HashAlgorithm::SHA384, key);
    hmac_sha384 << "Hi There";
    hmac_sha384.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_384_expected),
                  hmac_sha384.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_384_expected,
                      result,
                      sizeof(hmac_sha_384_expected));

    HMAC hmac_sha512(HashAlgorithm::SHA512, key);
    hmac_sha512 << "Hi There";
    hmac_sha512.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_512_expected),
                  hmac_sha512.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_512_expected,
                      result,
                      sizeof(hmac_sha_512_expected));
}

// From RFC 4231 section 4.3 (Test Case 2)
STF_TEST(HMAC, TestRFC2202_Section_4_3)
{
    std::uint8_t result[64];
    std::uint8_t key[] =
    {
        0x4a, 0x65, 0x66, 0x65
    };
    std::uint8_t data[] =
    {
        0x77, 0x68, 0x61, 0x74, 0x20, 0x64, 0x6f, 0x20,
        0x79, 0x61, 0x20, 0x77, 0x61, 0x6e, 0x74, 0x20,
        0x66, 0x6f, 0x72, 0x20, 0x6e, 0x6f, 0x74, 0x68,
        0x69, 0x6e, 0x67, 0x3f
    };
    std::uint8_t hmac_sha_224_expected[] =
    {
        0xa3, 0x0e, 0x01, 0x09, 0x8b, 0xc6, 0xdb, 0xbf,
        0x45, 0x69, 0x0f, 0x3a, 0x7e, 0x9e, 0x6d, 0x0f,
        0x8b, 0xbe, 0xa2, 0xa3, 0x9e, 0x61, 0x48, 0x00,
        0x8f, 0xd0, 0x5e, 0x44
    };
    std::uint8_t hmac_sha_256_expected[] =
    {
        0x5b, 0xdc, 0xc1, 0x46, 0xbf, 0x60, 0x75, 0x4e,
        0x6a, 0x04, 0x24, 0x26, 0x08, 0x95, 0x75, 0xc7,
        0x5a, 0x00, 0x3f, 0x08, 0x9d, 0x27, 0x39, 0x83,
        0x9d, 0xec, 0x58, 0xb9, 0x64, 0xec, 0x38, 0x43
    };
    std::uint8_t hmac_sha_384_expected[] =
    {
        0xaf, 0x45, 0xd2, 0xe3, 0x76, 0x48, 0x40, 0x31,
        0x61, 0x7f, 0x78, 0xd2, 0xb5, 0x8a, 0x6b, 0x1b,
        0x9c, 0x7e, 0xf4, 0x64, 0xf5, 0xa0, 0x1b, 0x47,
        0xe4, 0x2e, 0xc3, 0x73, 0x63, 0x22, 0x44, 0x5e,
        0x8e, 0x22, 0x40, 0xca, 0x5e, 0x69, 0xe2, 0xc7,
        0x8b, 0x32, 0x39, 0xec, 0xfa, 0xb2, 0x16, 0x49
    };
    std::uint8_t hmac_sha_512_expected[] =
    {
        0x16, 0x4b, 0x7a, 0x7b, 0xfc, 0xf8, 0x19, 0xe2,
        0xe3, 0x95, 0xfb, 0xe7, 0x3b, 0x56, 0xe0, 0xa3,
        0x87, 0xbd, 0x64, 0x22, 0x2e, 0x83, 0x1f, 0xd6,
        0x10, 0x27, 0x0c, 0xd7, 0xea, 0x25, 0x05, 0x54,
        0x97, 0x58, 0xbf, 0x75, 0xc0, 0x5a, 0x99, 0x4a,
        0x6d, 0x03, 0x4f, 0x65, 0xf8, 0xf0, 0xe6, 0xfd,
        0xca, 0xea, 0xb1, 0xa3, 0x4d, 0x4a, 0x6b, 0x4b,
        0x63, 0x6e, 0x07, 0x0a, 0x38, 0xbc, 0xe7, 0x37
    };

    HMAC hmac_sha224(HashAlgorithm::SHA224, key);
    hmac_sha224.Input(data);
    hmac_sha224.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_224_expected),
                  hmac_sha224.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_224_expected,
                      result,
                      sizeof(hmac_sha_224_expected));

    HMAC hmac_sha256(HashAlgorithm::SHA256, key);
    hmac_sha256.Input(data);
    hmac_sha256.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_256_expected),
                  hmac_sha256.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_256_expected,
                      result,
                      sizeof(hmac_sha_256_expected));

    HMAC hmac_sha384(HashAlgorithm::SHA384, key);
    hmac_sha384.Input(data);
    hmac_sha384.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_384_expected),
                  hmac_sha384.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_384_expected,
                      result,
                      sizeof(hmac_sha_384_expected));

    HMAC hmac_sha512(HashAlgorithm::SHA512, key);
    hmac_sha512.Input(data);
    hmac_sha512.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_512_expected),
                  hmac_sha512.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_512_expected,
                      result,
                      sizeof(hmac_sha_512_expected));
}

// From RFC 4231 section 4.4 (Test Case 3)
STF_TEST(HMAC, TestRFC2202_Section_4_4)
{
    std::uint8_t result[64];
    std::uint8_t key[] =
    {
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa
    };
    std::uint8_t data[] =
    {
        0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
        0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
        0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
        0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
        0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
        0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
        0xdd, 0xdd
    };
    std::uint8_t hmac_sha_224_expected[] =
    {
        0x7f, 0xb3, 0xcb, 0x35, 0x88, 0xc6, 0xc1, 0xf6,
        0xff, 0xa9, 0x69, 0x4d, 0x7d, 0x6a, 0xd2, 0x64,
        0x93, 0x65, 0xb0, 0xc1, 0xf6, 0x5d, 0x69, 0xd1,
        0xec, 0x83, 0x33, 0xea
    };
    std::uint8_t hmac_sha_256_expected[] =
    {
        0x77, 0x3e, 0xa9, 0x1e, 0x36, 0x80, 0x0e, 0x46,
        0x85, 0x4d, 0xb8, 0xeb, 0xd0, 0x91, 0x81, 0xa7,
        0x29, 0x59, 0x09, 0x8b, 0x3e, 0xf8, 0xc1, 0x22,
        0xd9, 0x63, 0x55, 0x14, 0xce, 0xd5, 0x65, 0xfe
    };
    std::uint8_t hmac_sha_384_expected[] =
    {
        0x88, 0x06, 0x26, 0x08, 0xd3, 0xe6, 0xad, 0x8a,
        0x0a, 0xa2, 0xac, 0xe0, 0x14, 0xc8, 0xa8, 0x6f,
        0x0a, 0xa6, 0x35, 0xd9, 0x47, 0xac, 0x9f, 0xeb,
        0xe8, 0x3e, 0xf4, 0xe5, 0x59, 0x66, 0x14, 0x4b,
        0x2a, 0x5a, 0xb3, 0x9d, 0xc1, 0x38, 0x14, 0xb9,
        0x4e, 0x3a, 0xb6, 0xe1, 0x01, 0xa3, 0x4f, 0x27
    };
    std::uint8_t hmac_sha_512_expected[] =
    {
        0xfa, 0x73, 0xb0, 0x08, 0x9d, 0x56, 0xa2, 0x84,
        0xef, 0xb0, 0xf0, 0x75, 0x6c, 0x89, 0x0b, 0xe9,
        0xb1, 0xb5, 0xdb, 0xdd, 0x8e, 0xe8, 0x1a, 0x36,
        0x55, 0xf8, 0x3e, 0x33, 0xb2, 0x27, 0x9d, 0x39,
        0xbf, 0x3e, 0x84, 0x82, 0x79, 0xa7, 0x22, 0xc8,
        0x06, 0xb4, 0x85, 0xa4, 0x7e, 0x67, 0xc8, 0x07,
        0xb9, 0x46, 0xa3, 0x37, 0xbe, 0xe8, 0x94, 0x26,
        0x74, 0x27, 0x88, 0x59, 0xe1, 0x32, 0x92, 0xfb
    };

    HMAC hmac_sha224(HashAlgorithm::SHA224, key);
    hmac_sha224.Input(data);
    hmac_sha224.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_224_expected),
                  hmac_sha224.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_224_expected,
                      result,
                      sizeof(hmac_sha_224_expected));

    HMAC hmac_sha256(HashAlgorithm::SHA256, key);
    hmac_sha256.Input(data);
    hmac_sha256.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_256_expected),
                  hmac_sha256.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_256_expected,
                      result,
                      sizeof(hmac_sha_256_expected));

    HMAC hmac_sha384(HashAlgorithm::SHA384, key);
    hmac_sha384.Input(data);
    hmac_sha384.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_384_expected),
                  hmac_sha384.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_384_expected,
                      result,
                      sizeof(hmac_sha_384_expected));

    HMAC hmac_sha512(HashAlgorithm::SHA512, key);
    hmac_sha512.Input(data);
    hmac_sha512.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_512_expected),
                  hmac_sha512.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_512_expected,
                      result,
                      sizeof(hmac_sha_512_expected));
}

// From RFC 4231 section 4.5 (Test Case 4)
STF_TEST(HMAC, TestRFC2202_Section_4_5)
{
    std::uint8_t result[64];
    std::uint8_t key[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19
    };
    std::uint8_t data[] =
    {
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
        0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
        0xcd, 0xcd
    };
    std::uint8_t hmac_sha_224_expected[] =
    {
        0x6c, 0x11, 0x50, 0x68, 0x74, 0x01, 0x3c, 0xac,
        0x6a, 0x2a, 0xbc, 0x1b, 0xb3, 0x82, 0x62, 0x7c,
        0xec, 0x6a, 0x90, 0xd8, 0x6e, 0xfc, 0x01, 0x2d,
        0xe7, 0xaf, 0xec, 0x5a
    };
    std::uint8_t hmac_sha_256_expected[] =
    {
        0x82, 0x55, 0x8a, 0x38, 0x9a, 0x44, 0x3c, 0x0e,
        0xa4, 0xcc, 0x81, 0x98, 0x99, 0xf2, 0x08, 0x3a,
        0x85, 0xf0, 0xfa, 0xa3, 0xe5, 0x78, 0xf8, 0x07,
        0x7a, 0x2e, 0x3f, 0xf4, 0x67, 0x29, 0x66, 0x5b
    };
    std::uint8_t hmac_sha_384_expected[] =
    {
        0x3e, 0x8a, 0x69, 0xb7, 0x78, 0x3c, 0x25, 0x85,
        0x19, 0x33, 0xab, 0x62, 0x90, 0xaf, 0x6c, 0xa7,
        0x7a, 0x99, 0x81, 0x48, 0x08, 0x50, 0x00, 0x9c,
        0xc5, 0x57, 0x7c, 0x6e, 0x1f, 0x57, 0x3b, 0x4e,
        0x68, 0x01, 0xdd, 0x23, 0xc4, 0xa7, 0xd6, 0x79,
        0xcc, 0xf8, 0xa3, 0x86, 0xc6, 0x74, 0xcf, 0xfb
    };
    std::uint8_t hmac_sha_512_expected[] =
    {
        0xb0, 0xba, 0x46, 0x56, 0x37, 0x45, 0x8c, 0x69,
        0x90, 0xe5, 0xa8, 0xc5, 0xf6, 0x1d, 0x4a, 0xf7,
        0xe5, 0x76, 0xd9, 0x7f, 0xf9, 0x4b, 0x87, 0x2d,
        0xe7, 0x6f, 0x80, 0x50, 0x36, 0x1e, 0xe3, 0xdb,
        0xa9, 0x1c, 0xa5, 0xc1, 0x1a, 0xa2, 0x5e, 0xb4,
        0xd6, 0x79, 0x27, 0x5c, 0xc5, 0x78, 0x80, 0x63,
        0xa5, 0xf1, 0x97, 0x41, 0x12, 0x0c, 0x4f, 0x2d,
        0xe2, 0xad, 0xeb, 0xeb, 0x10, 0xa2, 0x98, 0xdd
    };

    HMAC hmac_sha224(HashAlgorithm::SHA224, key);
    hmac_sha224.Input(data);
    hmac_sha224.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_224_expected),
                  hmac_sha224.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_224_expected,
                      result,
                      sizeof(hmac_sha_224_expected));

    HMAC hmac_sha256(HashAlgorithm::SHA256, key);
    hmac_sha256.Input(data);
    hmac_sha256.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_256_expected),
                  hmac_sha256.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_256_expected,
                      result,
                      sizeof(hmac_sha_256_expected));

    HMAC hmac_sha384(HashAlgorithm::SHA384, key);
    hmac_sha384.Input(data);
    hmac_sha384.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_384_expected),
                  hmac_sha384.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_384_expected,
                      result,
                      sizeof(hmac_sha_384_expected));

    HMAC hmac_sha512(HashAlgorithm::SHA512, key);
    hmac_sha512.Input(data);
    hmac_sha512.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_512_expected),
                  hmac_sha512.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_512_expected,
                      result,
                      sizeof(hmac_sha_512_expected));
}

// From RFC 4231 section 4.6 (Test Case 5)
//      NOTE: This is a test with truncated hash values
STF_TEST(HMAC, TestRFC2202_Section_4_6)
{
    std::uint8_t result[64];
    std::uint8_t key[] =
    {
        0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
        0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
        0x0c, 0x0c, 0x0c, 0x0c
    };
    std::uint8_t data[] =
    {
        0x54, 0x65, 0x73, 0x74, 0x20, 0x57, 0x69, 0x74,
        0x68, 0x20, 0x54, 0x72, 0x75, 0x6e, 0x63, 0x61,
        0x74, 0x69, 0x6f, 0x6e
    };
    std::uint8_t hmac_sha_224_expected[] =
    {
        0x0e, 0x2a, 0xea, 0x68, 0xa9, 0x0c, 0x8d, 0x37,
        0xc9, 0x88, 0xbc, 0xdb, 0x9f, 0xca, 0x6f, 0xa8
    };
    std::uint8_t hmac_sha_256_expected[] =
    {
        0xa3, 0xb6, 0x16, 0x74, 0x73, 0x10, 0x0e, 0xe0,
        0x6e, 0x0c, 0x79, 0x6c, 0x29, 0x55, 0x55, 0x2b
    };
    std::uint8_t hmac_sha_384_expected[] =
    {
        0x3a, 0xbf, 0x34, 0xc3, 0x50, 0x3b, 0x2a, 0x23,
        0xa4, 0x6e, 0xfc, 0x61, 0x9b, 0xae, 0xf8, 0x97
    };
    std::uint8_t hmac_sha_512_expected[] =
    {
        0x41, 0x5f, 0xad, 0x62, 0x71, 0x58, 0x0a, 0x53,
        0x1d, 0x41, 0x79, 0xbc, 0x89, 0x1d, 0x87, 0xa6
    };

    HMAC hmac_sha224(HashAlgorithm::SHA224, key);
    hmac_sha224.Input(data);
    hmac_sha224.Finalize();
    STF_ASSERT_EQ(28, hmac_sha224.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_224_expected,
                      result,
                      sizeof(hmac_sha_224_expected));

    HMAC hmac_sha256(HashAlgorithm::SHA256, key);
    hmac_sha256.Input(data);
    hmac_sha256.Finalize();
    STF_ASSERT_EQ(32, hmac_sha256.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_256_expected,
                      result,
                      sizeof(hmac_sha_256_expected));

    HMAC hmac_sha384(HashAlgorithm::SHA384, key);
    hmac_sha384.Input(data);
    hmac_sha384.Finalize();
    STF_ASSERT_EQ(48, hmac_sha384.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_384_expected,
                      result,
                      sizeof(hmac_sha_384_expected));

    HMAC hmac_sha512(HashAlgorithm::SHA512, key);
    hmac_sha512.Input(data);
    hmac_sha512.Finalize();
    STF_ASSERT_EQ(64, hmac_sha512.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_512_expected,
                      result,
                      sizeof(hmac_sha_512_expected));
}

// From RFC 4231 section 4.7 (Test Case 6)
STF_TEST(HMAC, TestRFC2202_Section_4_7)
{
    std::uint8_t result[64];
    std::uint8_t key[] =
    {
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa
    };
    std::uint8_t data[] =
    {
        0x54, 0x65, 0x73, 0x74, 0x20, 0x55, 0x73, 0x69,
        0x6e, 0x67, 0x20, 0x4c, 0x61, 0x72, 0x67, 0x65,
        0x72, 0x20, 0x54, 0x68, 0x61, 0x6e, 0x20, 0x42,
        0x6c, 0x6f, 0x63, 0x6b, 0x2d, 0x53, 0x69, 0x7a,
        0x65, 0x20, 0x4b, 0x65, 0x79, 0x20, 0x2d, 0x20,
        0x48, 0x61, 0x73, 0x68, 0x20, 0x4b, 0x65, 0x79,
        0x20, 0x46, 0x69, 0x72, 0x73, 0x74
    };
    std::uint8_t hmac_sha_224_expected[] =
    {
        0x95, 0xe9, 0xa0, 0xdb, 0x96, 0x20, 0x95, 0xad,
        0xae, 0xbe, 0x9b, 0x2d, 0x6f, 0x0d, 0xbc, 0xe2,
        0xd4, 0x99, 0xf1, 0x12, 0xf2, 0xd2, 0xb7, 0x27,
        0x3f, 0xa6, 0x87, 0x0e
    };
    std::uint8_t hmac_sha_256_expected[] =
    {
        0x60, 0xe4, 0x31, 0x59, 0x1e, 0xe0, 0xb6, 0x7f,
        0x0d, 0x8a, 0x26, 0xaa, 0xcb, 0xf5, 0xb7, 0x7f,
        0x8e, 0x0b, 0xc6, 0x21, 0x37, 0x28, 0xc5, 0x14,
        0x05, 0x46, 0x04, 0x0f, 0x0e, 0xe3, 0x7f, 0x54
    };
    std::uint8_t hmac_sha_384_expected[] =
    {
        0x4e, 0xce, 0x08, 0x44, 0x85, 0x81, 0x3e, 0x90,
        0x88, 0xd2, 0xc6, 0x3a, 0x04, 0x1b, 0xc5, 0xb4,
        0x4f, 0x9e, 0xf1, 0x01, 0x2a, 0x2b, 0x58, 0x8f,
        0x3c, 0xd1, 0x1f, 0x05, 0x03, 0x3a, 0xc4, 0xc6,
        0x0c, 0x2e, 0xf6, 0xab, 0x40, 0x30, 0xfe, 0x82,
        0x96, 0x24, 0x8d, 0xf1, 0x63, 0xf4, 0x49, 0x52
    };
    std::uint8_t hmac_sha_512_expected[] =
    {
        0x80, 0xb2, 0x42, 0x63, 0xc7, 0xc1, 0xa3, 0xeb,
        0xb7, 0x14, 0x93, 0xc1, 0xdd, 0x7b, 0xe8, 0xb4,
        0x9b, 0x46, 0xd1, 0xf4, 0x1b, 0x4a, 0xee, 0xc1,
        0x12, 0x1b, 0x01, 0x37, 0x83, 0xf8, 0xf3, 0x52,
        0x6b, 0x56, 0xd0, 0x37, 0xe0, 0x5f, 0x25, 0x98,
        0xbd, 0x0f, 0xd2, 0x21, 0x5d, 0x6a, 0x1e, 0x52,
        0x95, 0xe6, 0x4f, 0x73, 0xf6, 0x3f, 0x0a, 0xec,
        0x8b, 0x91, 0x5a, 0x98, 0x5d, 0x78, 0x65, 0x98
    };

    HMAC hmac_sha224(HashAlgorithm::SHA224, key);
    hmac_sha224.Input(data);
    hmac_sha224.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_224_expected),
                  hmac_sha224.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_224_expected,
                      result,
                      sizeof(hmac_sha_224_expected));

    HMAC hmac_sha256(HashAlgorithm::SHA256, key);
    hmac_sha256.Input(data);
    hmac_sha256.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_256_expected),
                  hmac_sha256.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_256_expected,
                      result,
                      sizeof(hmac_sha_256_expected));

    HMAC hmac_sha384(HashAlgorithm::SHA384, key);
    hmac_sha384.Input(data);
    hmac_sha384.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_384_expected),
                  hmac_sha384.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_384_expected,
                      result,
                      sizeof(hmac_sha_384_expected));

    HMAC hmac_sha512(HashAlgorithm::SHA512, key);
    hmac_sha512.Input(data);
    hmac_sha512.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_512_expected),
                  hmac_sha512.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_512_expected,
                      result,
                      sizeof(hmac_sha_512_expected));
}

// From RFC 4231 section 4.8 (Test Case 7)
STF_TEST(HMAC, TestRFC2202_Section_4_8)
{
    std::uint8_t result[64];
    std::uint8_t key[] =
    {
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa
    };
    std::string data = "This is a test using a larger than block-size key "
                        "and a larger than block-size data. The key needs "
                        "to be hashed before being used by the HMAC "
                        "algorithm.";
    std::uint8_t hmac_sha_224_expected[] =
    {
        0x3a, 0x85, 0x41, 0x66, 0xac, 0x5d, 0x9f, 0x02,
        0x3f, 0x54, 0xd5, 0x17, 0xd0, 0xb3, 0x9d, 0xbd,
        0x94, 0x67, 0x70, 0xdb, 0x9c, 0x2b, 0x95, 0xc9,
        0xf6, 0xf5, 0x65, 0xd1
    };
    std::uint8_t hmac_sha_256_expected[] =
    {
        0x9b, 0x09, 0xff, 0xa7, 0x1b, 0x94, 0x2f, 0xcb,
        0x27, 0x63, 0x5f, 0xbc, 0xd5, 0xb0, 0xe9, 0x44,
        0xbf, 0xdc, 0x63, 0x64, 0x4f, 0x07, 0x13, 0x93,
        0x8a, 0x7f, 0x51, 0x53, 0x5c, 0x3a, 0x35, 0xe2
    };
    std::uint8_t hmac_sha_384_expected[] =
    {
        0x66, 0x17, 0x17, 0x8e, 0x94, 0x1f, 0x02, 0x0d,
        0x35, 0x1e, 0x2f, 0x25, 0x4e, 0x8f, 0xd3, 0x2c,
        0x60, 0x24, 0x20, 0xfe, 0xb0, 0xb8, 0xfb, 0x9a,
        0xdc, 0xce, 0xbb, 0x82, 0x46, 0x1e, 0x99, 0xc5,
        0xa6, 0x78, 0xcc, 0x31, 0xe7, 0x99, 0x17, 0x6d,
        0x38, 0x60, 0xe6, 0x11, 0x0c, 0x46, 0x52, 0x3e
    };
    std::uint8_t hmac_sha_512_expected[] =
    {
        0xe3, 0x7b, 0x6a, 0x77, 0x5d, 0xc8, 0x7d, 0xba,
        0xa4, 0xdf, 0xa9, 0xf9, 0x6e, 0x5e, 0x3f, 0xfd,
        0xde, 0xbd, 0x71, 0xf8, 0x86, 0x72, 0x89, 0x86,
        0x5d, 0xf5, 0xa3, 0x2d, 0x20, 0xcd, 0xc9, 0x44,
        0xb6, 0x02, 0x2c, 0xac, 0x3c, 0x49, 0x82, 0xb1,
        0x0d, 0x5e, 0xeb, 0x55, 0xc3, 0xe4, 0xde, 0x15,
        0x13, 0x46, 0x76, 0xfb, 0x6d, 0xe0, 0x44, 0x60,
        0x65, 0xc9, 0x74, 0x40, 0xfa, 0x8c, 0x6a, 0x58
    };

    HMAC hmac_sha224(HashAlgorithm::SHA224, key);
    hmac_sha224.Input(data);
    hmac_sha224.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_224_expected),
                  hmac_sha224.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_224_expected,
                      result,
                      sizeof(hmac_sha_224_expected));

    HMAC hmac_sha256(HashAlgorithm::SHA256, key);
    hmac_sha256.Input(data);
    hmac_sha256.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_256_expected),
                  hmac_sha256.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_256_expected,
                      result,
                      sizeof(hmac_sha_256_expected));

    HMAC hmac_sha384(HashAlgorithm::SHA384, key);
    hmac_sha384.Input(data);
    hmac_sha384.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_384_expected),
                  hmac_sha384.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_384_expected,
                      result,
                      sizeof(hmac_sha_384_expected));

    HMAC hmac_sha512(HashAlgorithm::SHA512, key);
    hmac_sha512.Input(data);
    hmac_sha512.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_512_expected),
                  hmac_sha512.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_512_expected,
                      result,
                      sizeof(hmac_sha_512_expected));
}

// API Tests

// Test string output with spaces
STF_TEST(HMAC, TestOutputWithSpaces)
{
    std::uint8_t key[] = {'k', 'e', 'y'};

    HMAC hmac(HashAlgorithm::SHA1, key);

    // Stream characters to the hashing algorithm
    hmac << "The quick brown fox jumps over the lazy dog";
    hmac.Finalize();

    std::ostringstream oss;
    oss << hmac;

    STF_ASSERT_EQ(std::string("de7c9b85 b8b78aa6 bc8a7a36 f70a9070 1c9db4d9"),
                  oss.str());
}

// Test string output without spaces
STF_TEST(HMAC, TestOutputWithoutSpaces)
{
    std::uint8_t key[] = {'k', 'e', 'y'};

    HMAC hmac(HashAlgorithm::SHA1, key, false);

    // Stream characters to the hashing algorithm
    hmac << "The quick brown fox jumps over the lazy dog";
    hmac.Finalize();

    std::ostringstream oss;
    oss << hmac;
    STF_ASSERT_EQ(std::string("de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9"),
                  oss.str());
}

// Test exceptions on premature Result() call
STF_TEST(HMAC, TestExceptionPrematureResult)
{
    std::uint8_t key[] = {'k', 'e', 'y'};

    HMAC hmac_exception(HashAlgorithm::SHA1, key);

    bool exception_thrown = false;
    try
    {
        // This should throw an exception since the result was not finalized
        std::string result = hmac_exception.Result();
    }
    catch (const HashException &)
    {
        exception_thrown = true;
    }

    STF_ASSERT_TRUE(exception_thrown);
}

// Test Reset()
STF_TEST(HMAC, TestReset)
{
    std::uint8_t key[] = {'k', 'e', 'y'};

    HMAC hmac(HashAlgorithm::SHA1, key, false);

    // Stream characters to the hashing algorithm
    hmac << "The quick brown fox jumps over the lazy dog";
    hmac.Finalize();

    std::ostringstream oss;
    oss << hmac;
    STF_ASSERT_EQ(std::string("de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9"),
                  oss.str());

    // Reset the HMAC object
    hmac.Reset();

    // Reset the std::ostringstream object
    oss.str(std::string());
    oss.clear();

    // Stream characters to the hashing algorithm
    hmac << "The quick brown fox jumps over the lazy dog";
    hmac.Finalize();

    oss << hmac;
    STF_ASSERT_EQ(std::string("de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9"),
                  oss.str());
}

// Test the ability to change keys
STF_TEST(HMAC, TestKeyChange)
{
    std::uint8_t result[64];
    std::uint8_t key_1[] =
    {
        0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
        0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
    };
    std::uint8_t hmac_sha_256_expected_1[] =
    {
        0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53,
        0x5c, 0xa8, 0xaf, 0xce, 0xaf, 0x0b, 0xf1, 0x2b,
        0x88, 0x1d, 0xc2, 0x00, 0xc9, 0x83, 0x3d, 0xa7,
        0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32, 0xcf, 0xf7
    };
    std::uint8_t key_2[] =
    {
        0x4a, 0x65, 0x66, 0x65
    };
    std::uint8_t hmac_sha_256_expected_2[] =
    {
        0x6b, 0xfb, 0x11, 0x5c, 0xa3, 0x0d, 0xf3, 0xbe,
        0x0d, 0xfd, 0xff, 0xe7, 0x9a, 0x51, 0xcb, 0xee,
        0x88, 0x18, 0x6d, 0xb5, 0x5a, 0xcc, 0x28, 0x7a,
        0xf1, 0x48, 0xd7, 0xff, 0x62, 0x20, 0xf9, 0x2e
    };

    HMAC hmac(HashAlgorithm::SHA256, key_1);
    hmac << "Hi There";
    hmac.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_256_expected_1), hmac.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_256_expected_1,
                      result,
                      sizeof(hmac_sha_256_expected_1));

    // Set the key to something different (this does an implicit Reset())
    hmac.SetKey(key_2);

    hmac << "Hi There";
    hmac.Finalize();
    STF_ASSERT_EQ(sizeof(hmac_sha_256_expected_2), hmac.Result(result).size());
    STF_ASSERT_MEM_EQ(hmac_sha_256_expected_2,
                      result,
                      sizeof(hmac_sha_256_expected_2));
}

// Test the copy constructor
STF_TEST(HMAC, TestCopyConstructor)
{
    std::uint8_t key[] = {'k', 'e', 'y'};

    HMAC hmac(HashAlgorithm::SHA1, key);

    // Stream characters to the hashing algorithm
    hmac << "The quick brown fox jumps over the lazy dog";

    // Copy the HMAC
    HMAC hmac2 = hmac;
    hmac2.Finalize();

    // Verify that "hmac" is not finalized
    STF_ASSERT_FALSE(hmac.IsFinalized());

    // Verify the copy produced the right result
    std::ostringstream oss;
    oss << hmac2;
    STF_ASSERT_EQ(std::string("de7c9b85 b8b78aa6 bc8a7a36 f70a9070 1c9db4d9"),
                  oss.str());

    // Now check that the original produces the right result
    hmac.Finalize();
    oss.str(std::string());
    oss.clear();
    oss << hmac2;
    STF_ASSERT_EQ(std::string("de7c9b85 b8b78aa6 bc8a7a36 f70a9070 1c9db4d9"),
                  oss.str());
}

// Test the move constructor
STF_TEST(HMAC, TestMoveConstructor)
{
    std::uint8_t key[] = {'k', 'e', 'y'};

    HMAC hmac(HashAlgorithm::SHA1, key);

    // Stream characters to the hashing algorithm
    hmac << "The quick brown fox jumps over the lazy dog";
    hmac.Finalize();

    // Move the HMAC
    HMAC hmac2(std::move(hmac));

    std::ostringstream oss;
    oss << hmac2;
    STF_ASSERT_EQ(std::string("de7c9b85 b8b78aa6 bc8a7a36 f70a9070 1c9db4d9"),
                  oss.str());

    // Getting result on the original object should throw since it was moved
    auto test_func = [&]() { hmac.Result(); };
    STF_ASSERT_EXCEPTION_E(test_func, HashException);

    // And the HMAC length should indicate zero since the hash object is moved
    STF_ASSERT_EQ(0, hmac.GetHMACLength());

    // Attempt to re-use the original object, which is possible with re-keying
    hmac.SetKey(key);

    // The hash should now be 20 for SHA-1
    STF_ASSERT_EQ(20, hmac.GetHMACLength());

    // Re-compute the hash
    hmac << "The quick brown fox jumps over the lazy dog";
    hmac.Finalize();
    std::ostringstream oss2;
    oss2 << hmac2;
    STF_ASSERT_EQ(std::string("de7c9b85 b8b78aa6 bc8a7a36 f70a9070 1c9db4d9"),
                  oss2.str());
}

STF_TEST(HMAC, HashResetHash)
{
    std::string key = "key";
    std::string input = "The quick brown fox jumps over the lazy dog";
    std::string expected = "f7bc83f4 30538424 b13298e6 aa6fb143 "
                           "ef4d59a1 49461759 97479dbc 2d1a3cd8";

    HMAC hmac(HashAlgorithm::SHA256, key);
    hmac.Input(input);
    hmac.Finalize();

    std::string result = hmac.Result();

    STF_ASSERT_EQ(expected, result);

    // Reset the object, then repeat the test
    hmac.Reset();

    hmac.Input(input);
    hmac.Finalize();

    result = hmac.Result();

    STF_ASSERT_EQ(expected, result);
}

STF_TEST(HMAC, HMACWithoutKey)
{
    std::string key = "key";
    std::string input = "The quick brown fox jumps over the lazy dog";
    std::string expected = "f7bc83f4 30538424 b13298e6 aa6fb143 "
                           "ef4d59a1 49461759 97479dbc 2d1a3cd8";

    HMAC hmac(HashAlgorithm::SHA256);

    // This should throw an exception since no key was provided
    STF_ASSERT_EXCEPTION_E([&] { hmac.Input(input); }, HashException);

    // Now set the key
    hmac.SetKey(key);

    // Provide input and check results
    hmac.Input(input);
    hmac.Finalize();

    std::string result = hmac.Result();

    STF_ASSERT_EQ(expected, result);
}
