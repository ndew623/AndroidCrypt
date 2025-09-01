/*
 *  test_sha256.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module contains a number of test vectors which are used to
 *      test the SHA256 object to ensure it produces expected results.
 *      It will also exercise some of the APIs to ensure proper behavior.
 *
 *  Portability Issues:
 *      None.
 */

#include <memory>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <terra/crypto/hashing/sha256.h>
#include <terra/stf/stf.h>

using namespace Terra;
using namespace Terra::Crypto::Hashing;

//
// Test vectors
//

// Zero-length message
STF_TEST(SHA256, TestZeroLength)
{
    SHA256 sha256("");

    STF_ASSERT_EQ(std::string("e3b0c442 98fc1c14 9afbf4c8 996fb924 27ae41e4 "
                              "649b934c a495991b 7852b855"),
                  sha256.Result());
}

// Test "abc"
STF_TEST(SHS256, TestABC)
{
    SHA256 sha256("abc");

    STF_ASSERT_EQ(std::string("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 "
                              "96177a9c b410ff61 f20015ad"),
                  sha256.Result());
}

// Test a 448-bit message
STF_TEST(SHA256, Test448Bit)
{
    SHA256 sha256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");

    STF_ASSERT_EQ(std::string("248d6a61 d20638b8 e5c02693 0c3e6039 a33ce459 "
                              "64ff2167 f6ecedd4 19db06c1"),
                  sha256.Result());
}

// Test a 456-bit message
STF_TEST(SHA256, Test456Bit)
{
    SHA256 sha256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopqr");

    STF_ASSERT_EQ(std::string("4e0ea775 aa7766cf 2736d5a4 0e4c6f76 e1c4c69e "
                              "da0f3c78 2c5a2bd7 7b3f9695"),
                  sha256.Result());
}

// Test a 896-bit message
STF_TEST(SHA256, Test896Bit)
{
    SHA256 sha256("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijk"
                  "lmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu");

    STF_ASSERT_EQ(std::string("cf5b16a7 78af8380 036ce59e 7b049237 0b249b11 "
                              "e8f07a51 afac4503 7afee9d1"),
                  sha256.Result());
}

// Test a 904-bit message
STF_TEST(SHA256, Test904Bit)
{
    SHA256 sha256("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijk"
                  "lmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstuv");

    STF_ASSERT_EQ(std::string("1458ca27 710eb4e9 354ba340 9c99bb4c 98a980bc "
                              "3e5f1675 f835d06f 1a6a8c1c"),
                  sha256.Result());
}

// Test a 1,000,000 character string of a's
STF_TEST(SHA256, TestMillion)
{
    std::string million_a(1000000, 'a');
    SHA256 sha256(million_a);

    STF_ASSERT_EQ(std::string("cdc76e5c 9914fb92 81a1c7e2 84d73e67 f1809a48 "
                              "a497200e 046d39cc c7112cd0"),
                  sha256.Result());
}

// Test of 128 zero-value octets
STF_TEST(SHA256, Test128Zeros)
{
    std::string zeros_128(128, 0x00);
    SHA256 sha256(zeros_128);

    STF_ASSERT_EQ(std::string("38723a2e 5e8a17aa 7950dc00 8209944e 898f69a7 "
                              "bd10a23c 839d341e 935fd5ca"),
                  sha256.Result());
}

// Test of 257 zero-value octets
STF_TEST(SHA256, Test257Zeros)
{
    std::string zeros_257(257, 0x00);
    SHA256 sha256(zeros_257);

    STF_ASSERT_EQ(std::string("6c934d0c df9dba94 b474d6d1 929f1673 9bd9a8ed "
                              "31d0c3bc af82c283 fb7a3568"),
                  sha256.Result());
}

// Perform a very long test
STF_TEST(SHA256, TestLong)
{
    std::string message("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmgh"
                        "ijklmnhijklmno");
    std::string expected("50e72a0e 26442fe2 552dc393 8ac58658 228c0cbf "
                         "b1d2ca87 2ae43526 6fcd055e");
    SHA256 sha256;

    // Provide a ton of input
    for (std::uint32_t i = 0; i < 16'777'216UL; i++) sha256.Input(message);

    // Finalize the message digest
    sha256.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha256.Result());
}

// Test from NIST's test vectors
// Source: https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing#shavs
STF_TEST(SHA256, TestNISTBinary1)
{
    std::uint8_t vector[] =
    {
        0x45, 0x11, 0x01, 0x25, 0x0e, 0xc6, 0xf2, 0x66,
        0x52, 0x24, 0x9d, 0x59, 0xdc, 0x97, 0x4b, 0x73,
        0x61, 0xd5, 0x71, 0xa8, 0x10, 0x1c, 0xdf, 0xd3,
        0x6a, 0xba, 0x3b, 0x58, 0x54, 0xd3, 0xae, 0x08,
        0x6b, 0x5f, 0xdd, 0x45, 0x97, 0x72, 0x1b, 0x66,
        0xe3, 0xc0, 0xdc, 0x5d, 0x8c, 0x60, 0x6d, 0x96,
        0x57, 0xd0, 0xe3, 0x23, 0x28, 0x3a, 0x52, 0x17,
        0xd1, 0xf5, 0x3f, 0x2f, 0x28, 0x4f, 0x57, 0xb8,
        0x5c, 0x8a, 0x61, 0xac, 0x89, 0x24, 0x71, 0x1f,
        0x89, 0x5c, 0x5e, 0xd9, 0x0e, 0xf1, 0x77, 0x45,
        0xed, 0x2d, 0x72, 0x8a, 0xbd, 0x22, 0xa5, 0xf7,
        0xa1, 0x34, 0x79, 0xa4, 0x62, 0xd7, 0x1b, 0x56,
        0xc1, 0x9a, 0x74, 0xa4, 0x0b, 0x65, 0x5c, 0x58,
        0xed, 0xfe, 0x0a, 0x18, 0x8a, 0xd2, 0xcf, 0x46,
        0xcb, 0xf3, 0x05, 0x24, 0xf6, 0x5d, 0x42, 0x3c,
        0x83, 0x7d, 0xd1, 0xff, 0x2b, 0xf4, 0x62, 0xac,
        0x41, 0x98, 0x00, 0x73, 0x45, 0xbb, 0x44, 0xdb,
        0xb7, 0xb1, 0xc8, 0x61, 0x29, 0x8c, 0xdf, 0x61,
        0x98, 0x2a, 0x83, 0x3a, 0xfc, 0x72, 0x8f, 0xae,
        0x1e, 0xda, 0x2f, 0x87, 0xaa, 0x2c, 0x94, 0x80,
        0x85, 0x8b, 0xec
    };
    std::string expected("3c593aa5 39fdcdae 516cdf2f 15000f66 34185c88 "
                         "f505b397 75fb9ab1 37a10aa2");
    SHA256 sha256;

    // Provide input to the SHA256 object
    sha256.Input(vector);

    // Finalize the message digest
    sha256.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha256.Result());
}

// Test from NIST's test vectors
// Source: https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing#shavs
STF_TEST(SHA256, TestNISTBinary2)
{
    std::uint8_t vector[] =
    {
        0x82, 0x82, 0x96, 0x90, 0xaa, 0x37, 0x33, 0xc6,
        0x2b, 0x90, 0xd3, 0x29, 0x78, 0x86, 0x95, 0x2f,
        0xc1, 0xdc, 0x47, 0x3d, 0x67, 0xbb, 0x7d, 0x6b,
        0xb2, 0x99, 0xe0, 0x88, 0xc6, 0x5f, 0xc9, 0x5e,
        0xd3, 0xca, 0x0f, 0x36, 0x8d, 0x11, 0x1d, 0x9f,
        0xdc, 0xc9, 0x47, 0x6c, 0xd4, 0x06, 0x5e, 0xfc,
        0xe7, 0xc4, 0x81, 0xbe, 0x59, 0x85, 0x37, 0xf3,
        0xf5, 0x3b, 0xbb, 0xb6, 0xff, 0x67, 0x97, 0x3a,
        0x69, 0x83, 0x74, 0x54, 0x49, 0x9e, 0x31, 0x39,
        0x8b, 0x46, 0x32, 0x88, 0xe3, 0xaa, 0xfb, 0x8b,
        0x06, 0x00, 0xfd, 0xba, 0x1a, 0x25, 0xaf, 0x80,
        0x6b, 0x83, 0xe1, 0x42, 0x5f, 0x38, 0x4e, 0x9e,
        0xac, 0x75, 0x70, 0xf0, 0xc8, 0x23, 0x98, 0x1b,
        0xa2, 0xcd, 0x3d, 0x86, 0x8f, 0xba, 0x94, 0x64,
        0x87, 0x59, 0x62, 0x39, 0x91, 0xe3, 0x0f, 0x99,
        0x7c, 0x3b, 0xfb, 0x33, 0xd0, 0x19, 0x15, 0x0f,
        0x04, 0x67, 0xa9, 0x14, 0xf1, 0xeb, 0x79, 0xcd,
        0x87, 0x27, 0x10, 0x6d, 0xbf, 0x7d, 0x53, 0x10,
        0xd0, 0x97, 0x59, 0x43, 0xa6, 0x06, 0x7c, 0xc7,
        0x90, 0x29, 0xb0, 0x92, 0x39, 0x51, 0x14, 0x17,
        0xd9, 0x22, 0xc7, 0xc7, 0xac, 0x3d, 0xfd, 0xd8,
        0xa4, 0x1c, 0x52, 0x45, 0x5b, 0x3c, 0x5e, 0x16,
        0x4b, 0x82, 0x89, 0xe1, 0x41, 0xd8, 0x20, 0x91,
        0x0f, 0x17, 0xa9, 0x66, 0x81, 0x29, 0x74, 0x3d,
        0x93, 0x6f, 0x73, 0x12, 0xe1, 0x60, 0x4b, 0xc3,
        0x5f, 0x73, 0xab, 0x16, 0x4a, 0x3f, 0xdd, 0xfe,
        0x5f, 0xe1, 0x9b, 0x1a, 0x4a, 0x9f, 0x23, 0x7f,
        0x61, 0xcb, 0x8e, 0xb7, 0x92, 0xe9, 0x5d, 0x09,
        0x9a, 0x14, 0x55, 0xfb, 0x78, 0x9d, 0x8d, 0x16,
        0x22, 0xf6, 0xc5, 0xe9, 0x76, 0xce, 0xf9, 0x51,
        0x73, 0x7e, 0x36, 0xf7, 0xa9, 0xa4, 0xad, 0x19,
        0xee, 0x0d, 0x06, 0x8e, 0x53, 0xd9, 0xf6, 0x04,
        0x57, 0xd9, 0x14, 0x8d, 0x5a, 0x3c, 0xe8, 0x5a,
        0x54, 0x6b, 0x45, 0xc5, 0xc6, 0x31, 0xd9, 0x95,
        0xf1, 0x1f, 0x03, 0x7e, 0x47, 0x2f, 0xe4, 0xe8,
        0x1f, 0xa7, 0xb9, 0xf2, 0xac, 0x40, 0x68, 0xb5,
        0x30, 0x88, 0x58, 0xcd, 0x6d, 0x85, 0x86, 0x16,
        0x5c, 0x9b, 0xd6, 0xb3, 0x22, 0xaf, 0xa7, 0x55,
        0x40, 0x8d, 0xa9, 0xb9, 0x0a, 0x87, 0xf3, 0x73,
        0x5a, 0x5f, 0x50, 0xeb, 0x85, 0x68, 0xda, 0xa5,
        0x8e, 0xe7, 0xcb, 0xc5, 0x9a, 0xbf, 0x8f, 0xd2,
        0xa4, 0x4e, 0x1e, 0xba, 0x72, 0x92, 0x88, 0x16,
        0xc8, 0x90, 0xd1, 0xb0, 0xdb, 0xf6, 0x00, 0x42,
        0x08, 0xff, 0x73, 0x81, 0xc6, 0x97, 0x75, 0x5a,
        0xda, 0xc0, 0x13, 0x7c, 0xca, 0x34, 0x2b, 0x16,
        0x93
    };
    std::string expected("5f4e16a72d6c9857da0ba009ccacd4f26d7f6bf6c1b78a2ed"
                         "35e68fcb15b8e40");
    SHA256 sha256;

    sha256.SpaceSeparateWords(false); // No spaces in output on this test

    // Provide input to the SHA256 object
    sha256.Input(vector);

    // Finalize the message digest
    sha256.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha256.Result());
}

//
// API Tests
//

// Test expected values
STF_TEST(SHA256, TestExpectedValues)
{
    SHA256 sha256("abc");

    STF_ASSERT_EQ(std::string("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 "
                              "96177a9c b410ff61 f20015ad"),
                 sha256.Result());

    STF_ASSERT_TRUE(sha256.IsFinalized());

    STF_ASSERT_EQ(std::size_t(64), sha256.GetBlockSize());

    STF_ASSERT_EQ(std::size_t(32), sha256.GetDigestLength());
}

// Test Reset() function
STF_TEST(SHA256, TestReset)
{
    SHA256 sha256("abc");

    STF_ASSERT_EQ(std::string("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 "
                              "96177a9c b410ff61 f20015ad"),
                 sha256.Result());

    sha256.Reset();

    STF_ASSERT_FALSE(sha256.IsFinalized());

    STF_ASSERT_FALSE(sha256.IsFinalized());

    STF_ASSERT_EQ(std::uint64_t(0), sha256.GetMessageLength());

    // Verify object reuse works
    sha256.Input("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
    sha256.Finalize();

    STF_ASSERT_EQ(std::string("248d6a61 d20638b8 e5c02693 0c3e6039 a33ce459 "
                              "64ff2167 f6ecedd4 19db06c1"),
                  sha256.Result());
}

// Test streaming operator
STF_TEST(SHA256, TestStreamingOperator)
{
    SHA256 sha256;

    sha256 << "a" << "b" << "c";
    sha256.Finalize();

    STF_ASSERT_EQ(std::uint64_t(3), sha256.GetMessageLength());

    STF_ASSERT_EQ(std::string("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 "
                              "96177a9c b410ff61 f20015ad"),
                  sha256.Result());
}

// Test copy constructor
STF_TEST(SHA256, TestCopyConstructor)
{
    SHA256 sha256("abc");

    STF_ASSERT_EQ(std::string("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 "
                              "96177a9c b410ff61 f20015ad"),
                  sha256.Result());

    SHA256 sha256_copy = sha256;

    STF_ASSERT_EQ(std::uint64_t(3), sha256_copy.GetMessageLength());

    STF_ASSERT_EQ(sha256, sha256_copy);

    STF_ASSERT_TRUE(sha256_copy.IsFinalized());

    STF_ASSERT_EQ(std::string("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 "
                              "96177a9c b410ff61 f20015ad"),
                  sha256_copy.Result());
}

// Test move constructor
STF_TEST(SHA256, TestMoveConstructor)
{
    SHA256 sha256("abc");

    SHA256 sha256_copy = sha256;

    STF_ASSERT_EQ(std::string("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 "
                              "96177a9c b410ff61 f20015ad"),
                  sha256.Result());

    SHA256 sha256_move = std::move(sha256_copy);

    STF_ASSERT_EQ(sha256, sha256_move);

    STF_ASSERT_TRUE(sha256_move.IsFinalized());

    STF_ASSERT_EQ(sha256_move.Result(), sha256.Result());
}

STF_TEST(SHA256, TestEqualOperator)
{
    SHA256 sha256("abc");

    STF_ASSERT_EQ(std::string("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 "
                              "96177a9c b410ff61 f20015ad"),
                  sha256.Result());

    SHA256 sha256_copy = sha256;

    STF_ASSERT_TRUE(sha256_copy == sha256);
}

STF_TEST(SHA256, TestNotEqualOperator)
{
    SHA256 sha256("abc");

    STF_ASSERT_EQ(std::string("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 "
                              "96177a9c b410ff61 f20015ad"),
                  sha256.Result());

    SHA256 sha256_copy;

    STF_ASSERT_TRUE(sha256_copy != sha256);
}

// Test result array
STF_TEST(SHA256, TestResultArray)
{
    SHA256 sha256("abc");

    SHA256::SHA256ResultOctets result_array;

    sha256.Result(result_array);

    std::uint8_t expected_result_octets[] =
    {
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
        0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
        0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
        0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
    };

    STF_ASSERT_MEM_EQ(expected_result_octets, result_array, 32);
}

// Test result words
STF_TEST(SHA256, TestResultWords)
{
    SHA256 sha256("abc");

    SHA256::SHA256ResultWords result_words;

    sha256.Result(result_words);

    SHA256::SHA256ResultWords expected_result_words =
    {
        0xba7816bf, 0x8f01cfea, 0x414140de, 0x5dae2223,
        0xb00361a3, 0x96177a9c, 0xb410ff61, 0xf20015ad
    };

    STF_ASSERT_EQ(expected_result_words[0], result_words[0]);
    STF_ASSERT_EQ(expected_result_words[1], result_words[1]);
    STF_ASSERT_EQ(expected_result_words[2], result_words[2]);
    STF_ASSERT_EQ(expected_result_words[3], result_words[3]);
    STF_ASSERT_EQ(expected_result_words[4], result_words[4]);
    STF_ASSERT_EQ(expected_result_words[5], result_words[5]);
    STF_ASSERT_EQ(expected_result_words[6], result_words[6]);
    STF_ASSERT_EQ(expected_result_words[7], result_words[7]);
}

// Test Input() function
STF_TEST(SHA256, TestInput)
{
    SHA256 sha256("abc");

    SHA256 sha256_anew;

    sha256_anew.Input("a");
    sha256_anew.Input("b");
    sha256_anew.Input("c");
    sha256_anew.Finalize();

    STF_ASSERT_EQ(sha256, sha256_anew);
}

// Test Auto Finalize
STF_TEST(SHA256, TestAutoFinalize1)
{
    SHA256 sha256("abc");

    SHA256 sha256_auto("abc", true);

    STF_ASSERT_EQ(sha256, sha256_auto);
}

// Test Auto Finalize
STF_TEST(SHA256, TestAutoFinalize2)
{
    SHA256 sha256("abc");

    SHA256 sha256_auto("abc", false);
    sha256_auto.Finalize();

    STF_ASSERT_EQ(sha256, sha256_auto);
}

// Test output stream
STF_TEST(SHA256, TestOutputStream)
{
    SHA256 sha256("abc");

    // Test streaming operator
    std::ostringstream oss;
    oss << sha256;
    std::string test_string = oss.str();

    STF_ASSERT_EQ(std::string("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 "
                              "96177a9c b410ff61 f20015ad"),
                  test_string);
}

// Test base class pointer
STF_TEST(SHA256, TestBaseClassPointer)
{
    std::unique_ptr<Hash> hash = std::make_unique<SHA256>("abc");

    STF_ASSERT_EQ(std::string("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 "
                              "96177a9c b410ff61 f20015ad"),
                  hash->Result());
}

// Test exception on additional input
STF_TEST(SHA256, TestExceptionOnExtraInput)
{
    SHA256 sha256("abc");

    bool exception_thrown = false;
    try
    {
        sha256.Input("abc");
    }
    catch(const std::exception &)
    {
        exception_thrown = true;
    }

    STF_ASSERT_TRUE(exception_thrown);
}

// Test exception when not finalized
STF_TEST(SHA256, TestExceptionNotFinalized)
{
    SHA256 sha256_exception;

    bool exception_thrown = false;
    try
    {
        // This should throw an exception since the result was not finalized
        std::string result = sha256_exception.Result();
    }
    catch (const HashException &)
    {
        exception_thrown = true;
    }

    STF_ASSERT_TRUE(exception_thrown);
}

// Under debug builds, exclude these slow tests
#ifndef NDEBUG
STF_TEST_EXCLUDE(SHA256, TestLong)
#endif
