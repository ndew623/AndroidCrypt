/*
 *  test_sha1.cpp
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
 *      test the SHA1 object to ensure it produces expected results.
 *      It will also exercise some of the APIs to ensure proper behavior.
 *
 *  Portability Issues:
 *      None.
 */

#include <memory>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <terra/crypto/hashing/sha1.h>
#include <terra/stf/stf.h>

using namespace Terra;
using namespace Terra::Crypto::Hashing;

//
// Test vectors
//

// Zero-length message
STF_TEST(SHA1, TestZeroLength)
{
    SHA1 sha1("");

    STF_ASSERT_EQ(std::string("da39a3ee 5e6b4b0d 3255bfef 95601890 afd80709"),
                  sha1.Result());
}

// Test "abc"
STF_TEST(SHA1, TestABC)
{
    SHA1 sha1("abc");

    STF_ASSERT_EQ(std::string("a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"),
                  sha1.Result());
}

// Test a 448-bit message
STF_TEST(SHA1, Test448Bit)
{
    SHA1 sha1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");

    STF_ASSERT_EQ(std::string("84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1"),
                  sha1.Result());
}

// Test a 456-bit message
STF_TEST(SHA1, Test456Bit)
{
    SHA1 sha1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopqr");

    STF_ASSERT_EQ(std::string("e4690e96 180cb89f dd79a3ba 0f2a7412 24a50e62"),
                  sha1.Result());
}

// Test a 896-bit message
STF_TEST(SHA1, Test896Bit)
{
    SHA1 sha1("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhij"
              "klmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu");

    STF_ASSERT_EQ(std::string("a49b2446 a02c645b f419f995 b6709125 3a04a259"),
                  sha1.Result());
}

// Test a 904-bit message
STF_TEST(SHA1, Test904Bit)
{
    SHA1 sha1("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhij"
              "klmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstuv");

    STF_ASSERT_EQ(std::string("98a3f7e4 c1f3f666 4b7c7bfd 2b2fcdd3 913c88f9"),
                  sha1.Result());
}

// Test a 1,000,000 character string of a's
STF_TEST(SHA1, TestMillion)
{
    std::string million_a(1000000, 'a');
    SHA1 sha1(million_a);

    STF_ASSERT_EQ(std::string("34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f"),
                  sha1.Result());
}

// Test of 128 zero-value octets
STF_TEST(SHA1, Test128Zeros)
{
    std::string zeros_128(128, 0x00);

    SHA1 sha1(zeros_128);

    STF_ASSERT_EQ(std::string("0ae4f711 ef5d6e9d 26c611fd 2c8c8ac4 5ecbf9e7"),
                  sha1.Result());
}

// Test of 257 zero-value octets
STF_TEST(SHA1, Test257Zeros)
{
    std::string zeros_257(257, 0x00);

    SHA1 sha1(zeros_257);

    STF_ASSERT_EQ(std::string("5ee50d67 5c809fe5 9e4a7762 c54b6583 7547eafb"),
                  sha1.Result());
}

// Perform a very long test
STF_TEST(SHA1, TestLong)
{
    std::string message("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmgh"
                        "ijklmnhijklmno");
    std::string expected("7789f0c9 ef7bfc40 d9331114 3dfbe69e 2017f592");
    SHA1 sha1;

    // Provide a ton of input
    for (std::size_t i = 0; i < 16'777'216UL; i++) sha1.Input(message);

    // Finalize the message digest
    sha1.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha1.Result());
}

// Test from NIST's test vectors
// Source: https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing#shavs
STF_TEST(SHA1, TestNISTBinary1)
{
    std::uint8_t vector[] =
    {
        0x7c, 0x9c, 0x67, 0x32, 0x3a, 0x1d, 0xf1, 0xad,
        0xbf, 0xe5, 0xce, 0xb4, 0x15, 0xea, 0xef, 0x01,
        0x55, 0xec, 0xe2, 0x82, 0x0f, 0x4d, 0x50, 0xc1,
        0xec, 0x22, 0xcb, 0xa4, 0x92, 0x8a, 0xc6, 0x56,
        0xc8, 0x3f, 0xe5, 0x85, 0xdb, 0x6a, 0x78, 0xce,
        0x40, 0xbc, 0x42, 0x75, 0x7a, 0xba, 0x7e, 0x5a,
        0x3f, 0x58, 0x24, 0x28, 0xd6, 0xca, 0x68, 0xd0,
        0xc3, 0x97, 0x83, 0x36, 0xa6, 0xef, 0xb7, 0x29,
        0x61, 0x3e, 0x8d, 0x99, 0x79, 0x01, 0x62, 0x04,
        0xbf, 0xd9, 0x21, 0x32, 0x2f, 0xdd, 0x52, 0x22,
        0x18, 0x35, 0x54, 0x44, 0x7d, 0xe5, 0xe6, 0xe9,
        0xbb, 0xe6, 0xed, 0xf7, 0x6d, 0x7b, 0x71, 0xe1,
        0x8d, 0xc2, 0xe8, 0xd6, 0xdc, 0x89, 0xb7, 0x39,
        0x83, 0x64, 0xf6, 0x52, 0xfa, 0xfc, 0x73, 0x43,
        0x29, 0xaa, 0xfa, 0x3d, 0xcd, 0x45, 0xd4, 0xf3,
        0x1e, 0x38, 0x8e, 0x4f, 0xaf, 0xd7, 0xfc, 0x64,
        0x95, 0xf3, 0x7c, 0xa5, 0xcb, 0xab, 0x7f, 0x54,
        0xd5, 0x86, 0x46, 0x3d, 0xa4, 0xbf, 0xea, 0xa3,
        0xba, 0xe0, 0x9f, 0x7b, 0x8e, 0x92, 0x39, 0xd8,
        0x32, 0xb4, 0xf0, 0xa7, 0x33, 0xaa, 0x60, 0x9c,
        0xc1, 0xf8, 0xd4
    };
    std::string expected("d8fd6a91 ef3b6ced 05b98358 a99107c1 fac8c807");
    SHA1 sha1;

    // Provide input to the SHA1 object
    sha1.Input(vector);

    // Finalize the message digest
    sha1.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha1.Result());
}

// Test from NIST's test vectors
// Source: https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing#shavs
STF_TEST(SHA1, TestNISTBinary2)
{
    std::uint8_t vector[] =
    {
        0xbd, 0x74, 0xe7, 0xf6, 0x07, 0xcd, 0x7d, 0x90,
        0x5e, 0x90, 0x17, 0x5d, 0x67, 0x65, 0x0a, 0x6d,
        0xc2, 0xf8, 0xa4, 0xe2, 0xd4, 0xab, 0x12, 0x49,
        0xca, 0x88, 0x81, 0x2b, 0xda, 0x79, 0x84, 0xde,
        0xcc, 0xbb, 0xb6, 0xa1, 0xba, 0x90, 0xa0, 0xe9,
        0x14, 0x34, 0xdd, 0xf5, 0xe6, 0x13, 0x7b, 0xa8,
        0x5e, 0x39, 0xa5, 0x98, 0x89, 0x0a, 0x7f, 0x63,
        0x5d, 0x33, 0x52, 0x42, 0xfc, 0xe0, 0xe9, 0xe0,
        0x37, 0x30, 0x3b, 0x6c, 0x51, 0xe5, 0x4a, 0xec,
        0x06, 0x61, 0x4a, 0xd5, 0xcc, 0xce, 0x06, 0xd9,
        0x59, 0x9c, 0x80, 0x01, 0x65, 0x30, 0xd7, 0xfb,
        0xb1, 0xda, 0x6e, 0xb5, 0x48, 0x08, 0x4b, 0x2b,
        0x05, 0xba, 0xbd, 0x7d, 0x55, 0x36, 0x42, 0x44,
        0x3e, 0xfd, 0xa7, 0x26, 0xa1, 0xfd, 0x71, 0xa8,
        0xbc, 0x08, 0x7c, 0x44, 0xf2, 0x85, 0xe2, 0xbc,
        0xcf, 0x66, 0x1e, 0xad, 0x47, 0x5a, 0x72, 0x67,
        0x3e, 0x43, 0x86, 0xfc, 0x4e, 0xea, 0x51, 0x97,
        0xc4, 0xf1, 0x3c, 0x0f, 0xeb, 0x0a, 0x85, 0xbc,
        0x8e, 0x67, 0xe2, 0x8a, 0xb8, 0x72, 0x68, 0x4b,
        0xbe, 0xbd, 0xaa, 0x52, 0x7f, 0x3c, 0x25, 0x3d,
        0xeb, 0xb2, 0xdc, 0x12, 0xc2, 0x69, 0x3f, 0x8e,
        0x9e, 0x26, 0x51, 0xb9, 0x34, 0x5c, 0x0a, 0xbe,
        0xd7, 0xa0, 0xfa, 0xfa, 0x3e, 0x5d, 0x30, 0x53,
        0x86, 0xc9, 0x5a, 0xcb, 0x7a, 0x17, 0x2e, 0x54,
        0x13, 0xef, 0x08, 0xe7, 0x3b, 0x1b, 0xd4, 0xd0,
        0xd6, 0x83, 0x2e, 0x4c, 0x03, 0x5b, 0xc8, 0x55,
        0x9f, 0x9b, 0x0c, 0xbd, 0x0c, 0xaf, 0x03, 0x7a,
        0x30, 0x70, 0x76, 0x41, 0xc0, 0x54, 0x53, 0x56,
        0xbe, 0xe1, 0x51, 0xa2, 0x40, 0x68, 0xd7, 0x06,
        0x74, 0xef, 0x1b, 0xef, 0xe1, 0x6f, 0x87, 0x2a,
        0xef, 0x40, 0x60, 0xfa, 0xaa, 0xd1, 0xa9, 0x68,
        0xc3, 0x9c, 0x45, 0xdb, 0xd7, 0x59, 0x5d, 0xe8,
        0xf4, 0x72, 0x01, 0x6b, 0x5a, 0xb8, 0x12, 0xd7,
        0x7e, 0x54, 0x5f, 0xca, 0x55, 0x00, 0x0e, 0xe5,
        0xce, 0x77, 0x3e, 0xda, 0xa1, 0x29, 0xea, 0xc6,
        0x47, 0x34, 0x10, 0xc2, 0x49, 0x90, 0x13, 0xb4,
        0xbe, 0x89, 0x5f, 0x6c, 0x0f, 0x73, 0x4b, 0xec,
        0xfe, 0x99, 0x43, 0x06, 0xe7, 0x76, 0x26, 0x2d,
        0x45, 0x28, 0xed, 0x85, 0x77, 0x21, 0x8e, 0x3c,
        0xc5, 0x20, 0x1f, 0x1d, 0x9e, 0x5f, 0x3f, 0x62,
        0x23, 0x0e, 0xb2, 0xca, 0xea, 0x01, 0x4b, 0xec,
        0xfb, 0xa6, 0x0f, 0xcb, 0x1f, 0x39, 0x97, 0xaa,
        0x5b, 0x3b, 0xb6, 0x22, 0xb7, 0x20, 0x5c, 0x71,
        0x43, 0x48, 0xba, 0x15, 0x5c, 0x30, 0xa7, 0x9a,
        0x2c, 0xea, 0x43, 0xb0, 0x70, 0xca, 0xda, 0x80,
        0x7e, 0x63, 0x0b, 0x40, 0x86, 0xb1, 0x29, 0x05,
        0x18, 0x98, 0xe1, 0xd9, 0xe6, 0x8d, 0x1d, 0x0e,
        0xcc, 0x94, 0x29, 0xd2, 0x0d, 0x6a, 0x14, 0x03,
        0xe0, 0x03, 0x5a, 0x44, 0x2b, 0x37, 0xbf, 0x50,
        0x8e, 0xb8, 0x7e, 0x8e, 0xa3, 0x47, 0xa3, 0xe6,
        0x84, 0x27, 0xb6, 0xd4, 0x8e, 0xd2, 0x99, 0xba,
        0x65, 0xec, 0xb3, 0x7b, 0x38, 0x75, 0x4f, 0x45,
        0x47, 0x42, 0x3e, 0xae, 0xa2, 0xae, 0xc4, 0x03,
        0x33, 0x8d, 0xb2, 0xdc, 0xfe, 0x61, 0xcf, 0xf4,
        0xa8, 0xd1, 0x7c, 0x38, 0x36, 0x56, 0x98, 0x1e,
        0x18, 0x38, 0xa2, 0x38, 0x66, 0xb9, 0x1d, 0x09,
        0x69, 0x8f, 0x39, 0x17, 0x5d, 0x98, 0xaf, 0x41,
        0x75, 0xca, 0xed, 0x53
    };
    std::string expected("b22b87ea30f4050913f8f0241fc2ae2c319f52e7");
    SHA1 sha1;

    sha1.SpaceSeparateWords(false); // No spaces in output on this test

    // Provide input to the SHA1 object
    sha1.Input(vector);

    // Finalize the message digest
    sha1.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha1.Result());
}

//
// API Tests
//

// Test expected values
STF_TEST(SHA1, TestExpectedValues)
{
    SHA1 sha1("abc");

    STF_ASSERT_EQ(std::string("a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"),
                  sha1.Result());

    STF_ASSERT_TRUE(sha1.IsFinalized());

    STF_ASSERT_EQ(std::size_t(64), sha1.GetBlockSize());

    STF_ASSERT_EQ(std::size_t(20), sha1.GetDigestLength());
}

// Test Reset() function
STF_TEST(SHA1, TestReset)
{
    SHA1 sha1("abc");

    STF_ASSERT_EQ(std::string("a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"),
                  sha1.Result());

    sha1.Reset();

    STF_ASSERT_FALSE(sha1.IsFinalized());

    STF_ASSERT_FALSE(sha1.IsCorrupted());

    STF_ASSERT_EQ(std::uint64_t(0), sha1.GetMessageLength());

    // Verify object reuse works
    sha1.Input("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
    sha1.Finalize();

    STF_ASSERT_EQ(std::string("84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1"),
                  sha1.Result());
}

// Test streaming operator
STF_TEST(SHA1, TestStreamingOperator)
{
    SHA1 sha1;

    sha1 << "a" << "b" << "c";
    sha1.Finalize();

    STF_ASSERT_EQ(std::uint64_t(3), sha1.GetMessageLength());

    STF_ASSERT_EQ(std::string("a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"),
                  sha1.Result());
}

// Test copy constructor
STF_TEST(SHA1, TestCopyConstructor)
{
    SHA1 sha1("abc");

    STF_ASSERT_EQ(std::string("a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"),
                  sha1.Result());

    SHA1 sha1_copy = sha1;

    STF_ASSERT_EQ(std::uint64_t(3), sha1_copy.GetMessageLength());

    STF_ASSERT_EQ(sha1, sha1_copy);

    STF_ASSERT_TRUE(sha1_copy.IsFinalized());

    STF_ASSERT_EQ(std::string("a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"),
                  sha1_copy.Result());
}

// Test move constructor
STF_TEST(SHA1, TestMoveConstructor)
{
    SHA1 sha1("abc");

    SHA1 sha1_copy = sha1;

    STF_ASSERT_EQ(std::string("a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"),
                  sha1.Result());

    SHA1 sha1_move = std::move(sha1_copy);

    STF_ASSERT_EQ(sha1, sha1_move);

    STF_ASSERT_TRUE(sha1_move.IsFinalized());

    STF_ASSERT_EQ(sha1_move.Result(), sha1.Result());

    STF_ASSERT_EQ(sha1_move, sha1);
}

// Test equal operator
STF_TEST(SHA1, TestEqualOperator)
{
    SHA1 sha1("abc");

    STF_ASSERT_EQ(std::string("a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"),
                  sha1.Result());

    SHA1 sha1_copy = sha1;

    STF_ASSERT_TRUE(sha1_copy == sha1);
}

// Test not equal operator
STF_TEST(SHA1, TestNotEqualOperator)
{
    SHA1 sha1("abc");

    STF_ASSERT_EQ(std::string("a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"),
                  sha1.Result());

    SHA1 sha1_copy;

    STF_ASSERT_TRUE(sha1_copy != sha1);
}

// Test result array
STF_TEST(SHA1, TestResultArray)
{
    SHA1 sha1("abc");

    SHA1::SHA1ResultOctets result_array;

    sha1.Result(result_array);

    std::uint8_t expected_result_octets[] =
    {
        0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a,
        0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c,
        0x9c, 0xd0, 0xd8, 0x9d
    };

    STF_ASSERT_MEM_EQ(expected_result_octets,
                      result_array,
                      sizeof(result_array));
}

// Test result words
STF_TEST(SHA1, TestResultWords)
{
    SHA1 sha1("abc");

    SHA1::SHA1ResultWords result_words;

    sha1.Result(result_words);

    std::uint32_t expected_result_words[] =
    {
        0xa9993e36, 0x4706816a, 0xba3e2571, 0x7850c26c, 0x9cd0d89d
    };

    STF_ASSERT_EQ(expected_result_words[0], result_words[0]);
    STF_ASSERT_EQ(expected_result_words[1], result_words[1]);
    STF_ASSERT_EQ(expected_result_words[2], result_words[2]);
    STF_ASSERT_EQ(expected_result_words[3], result_words[3]);
    STF_ASSERT_EQ(expected_result_words[4], result_words[4]);
}

// Test Input() function
STF_TEST(SHA1, TestInput)
{
    SHA1 sha1("abc");

    SHA1 sha1_anew;

    sha1_anew.Input("a");
    sha1_anew.Input("b");
    sha1_anew.Input("c");
    sha1_anew.Finalize();

    STF_ASSERT_EQ(sha1, sha1_anew);
}

// Test Auto Finalize
STF_TEST(SHA1, TestAutoFinalize1)
{
    SHA1 sha1("abc");

    SHA1 sha1_auto("abc", true);

    STF_ASSERT_EQ(sha1, sha1_auto);
}

// Test Auto Finalize
STF_TEST(SHA1, TestAutoFinalize2)
{
    SHA1 sha1("abc");

    SHA1 sha1_auto("abc", false);
    sha1_auto.Finalize();

    STF_ASSERT_EQ(sha1, sha1_auto);
}

// Test output stream
STF_TEST(SHA1, TestOutputStream)
{
    SHA1 sha1("abc");

    // Test streaming operator
    std::ostringstream oss;
    oss << sha1;
    std::string test_string = oss.str();

    STF_ASSERT_EQ(std::string("a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"),
                  test_string);
}

// Test base class pointer
STF_TEST(SHA1, TestBaseClassPointer)
{
    std::unique_ptr<Hash> hash = std::make_unique<SHA1>("abc");

    STF_ASSERT_EQ(std::string("a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"),
                  hash->Result());
}

// Test exception on additional input
STF_TEST(SHA1, TestExceptionOnExtraInput)
{
    SHA1 sha1("abc");

    bool exception_thrown = false;
    try
    {
        sha1.Input("abc");
    }
    catch(const std::exception &)
    {
        exception_thrown = true;
    }

    STF_ASSERT_TRUE(exception_thrown);
}

// Test exception when not finalized
STF_TEST(SHA1, TestExceptionNotFinalized)
{
    SHA1 sha1_exception;

    bool exception_thrown = false;
    try
    {
        // This should throw an exception since the result was not finalized
        std::string result = sha1_exception.Result();
    }
    catch (const HashException &)
    {
        exception_thrown = true;
    }

    STF_ASSERT_TRUE(exception_thrown);
}

// Under debug builds, exclude these slow tests
#ifndef NDEBUG
STF_TEST_EXCLUDE(SHA1, TestLong)
#endif
