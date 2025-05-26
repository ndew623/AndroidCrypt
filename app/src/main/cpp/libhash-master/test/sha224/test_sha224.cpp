/*
 *  test_sha224.cpp
 *
 *  Copyright (C) 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module contains a number of test vectors which are used to
 *      test the SHA224 object to ensure it produces expected results.
 *      It will also exercise some of the APIs to ensure proper behavior.
 *
 *  Portability Issues:
 *      None.
 */

#include <memory>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <terra/crypto/hashing/sha224.h>
#include <terra/stf/stf.h>

using namespace Terra;
using namespace Terra::Crypto::Hashing;

//
// Test vectors
//

// Zero-length message
STF_TEST(SHA224, TestZeroLength)
{
    SHA224 sha224("");

    STF_ASSERT_EQ(std::string("d14a028c 2a3a2bc9 476102bb 288234c4 15a2b01f "
                              "828ea62a c5b3e42f"),
                  sha224.Result());
}

// Test "abc"
STF_TEST(SHS224, TestABC)
{
    SHA224 sha224("abc");

    STF_ASSERT_EQ(std::string("23097d22 3405d822 8642a477 bda255b3 2aadbce4 "
                              "bda0b3f7 e36c9da7"),
                  sha224.Result());
}

// Test a 448-bit message
STF_TEST(SHA224, Test448Bit)
{
    SHA224 sha224("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");

    STF_ASSERT_EQ(std::string("75388b16 512776cc 5dba5da1 fd890150 b0c6455c "
                              "b4f58b19 52522525"),
                  sha224.Result());
}

// Test a 456-bit message
STF_TEST(SHA224, Test456Bit)
{
    SHA224 sha224("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopqr");

    STF_ASSERT_EQ(std::string("abc3ce1d 0a4bb80e e93cf873 2ce2d7c3 e10a0d47 "
                              "558ee1a2 7540335a"),
                  sha224.Result());
}

// Test a 896-bit message
STF_TEST(SHA224, Test896Bit)
{
    SHA224 sha224("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijk"
                  "lmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu");

    STF_ASSERT_EQ(std::string("c97ca9a5 59850ce9 7a04a96d ef6d99a9 e0e0e2ab "
                              "14e6b8df 265fc0b3"),
                  sha224.Result());
}

// Test a 904-bit message
STF_TEST(SHA224, Test904Bit)
{
    SHA224 sha224("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijk"
                  "lmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstuv");

    STF_ASSERT_EQ(std::string("a0af7243 4071a949 22fe319a cf988964 1d5256a5 "
                              "b5d5d4bd 2830653d"),
                  sha224.Result());
}

// Test a 1,000,000 character string of 0x00
STF_TEST(SHA224, TestMillion0)
{
    std::string million_a(1000000, 0x00);
    SHA224 sha224(million_a);

    STF_ASSERT_EQ(std::string("3a5d74b6 8f14f3a4 b2be9289 b8d37067 2d0b3d2f "
                              "53bc303c 59032df3"),
                  sha224.Result());
}

// Test a 1,000,000 character string of 0x00
STF_TEST(SHA224, TestMillionA)
{
    std::string million_a(1000000, 'a');
    SHA224 sha224(million_a);

    STF_ASSERT_EQ(std::string("20794655 980c91d8 bbb4c1ea 97618a4b f03f4258 "
                              "1948b2ee 4ee7ad67"),
                  sha224.Result());
}

// Test of 128 zero-value octets
STF_TEST(SHA224, Test128Zeros)
{
    std::string zeros_128(128, 0x00);
    SHA224 sha224(zeros_128);

    STF_ASSERT_EQ(std::string("2fbd823e bcd9909d 265827e4 bce793a4 fc572e3f "
                              "39c7c3dd 67749f3e"),
                  sha224.Result());
}

// Test of 257 zero-value octets
STF_TEST(SHA224, Test257Zeros)
{
    std::string zeros_257(257, 0x00);
    SHA224 sha224(zeros_257);

    STF_ASSERT_EQ(std::string("b2f720f9 d67db745 a98e7eb6 6aced852 1dd5464e "
                              "ff788d28 a0999108"),
                  sha224.Result());
}

// Perform a very long test
STF_TEST(SHA224, TestLong)
{
    std::string message("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmgh"
                        "ijklmnhijklmno");
    std::string expected("b5989713 ca4fe47a 009f8621 980b34e6 d63ed306 "
                         "3b2a0a2c 867d8a85");
    SHA224 sha224;

    // Provide a ton of input
    for (std::uint32_t i = 0; i < 16'777'216UL; i++) sha224.Input(message);

    // Finalize the message digest
    sha224.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha224.Result());
}

// Test from NIST's test vectors
// Source: https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing#shavs
STF_TEST(SHA224, TestNISTBinary1)
{
    std::uint8_t vector[] =
    {
         0xf1, 0x49, 0xe4, 0x1d, 0x84, 0x8f, 0x59, 0x27,
         0x6c, 0xfd, 0xdd, 0x74, 0x3b, 0xaf, 0xa9, 0xa9,
         0x0e, 0x1e, 0xe4, 0xa2, 0x63, 0xa1, 0x18, 0x14,
         0x2b, 0x33, 0xe3, 0x70, 0x21, 0x76, 0xef, 0x0a,
         0x59, 0xf8, 0x23, 0x7a, 0x1c, 0xb5, 0x1b, 0x42,
         0xf3, 0xde, 0xd6, 0xb2, 0x02, 0xd9, 0xaf, 0x09,
         0x97, 0x89, 0x8f, 0xdd, 0x03, 0xcf, 0x60, 0xbd,
         0xa9, 0x51, 0xc5, 0x14, 0x54, 0x7a, 0x08, 0x50,
         0xce, 0xc2, 0x54, 0x44, 0xae, 0x2f, 0x24, 0xcb,
         0x71, 0x1b, 0xfb, 0xaf, 0xcc, 0x39, 0x56, 0xc9,
         0x41, 0xd3, 0xde, 0x69, 0xf1, 0x55, 0xe3, 0xf8,
         0xb1, 0x0f, 0x06, 0xdb, 0x5f, 0x37, 0x35, 0x9b,
         0x77, 0x2d, 0xdd, 0x43, 0xe1, 0x03, 0x5a, 0x0a,
         0x0d, 0x3d, 0xb3, 0x32, 0x42, 0xd5, 0x84, 0x30,
         0x33, 0x83, 0x3b, 0x0d, 0xd4, 0x3b, 0x87, 0x0c,
         0x6b, 0xf6, 0x0e, 0x8d, 0xea, 0xb5, 0x5f, 0x31,
         0x7c, 0xc3, 0x27, 0x3f, 0x5e, 0x3b, 0xa7, 0x47,
         0xf0, 0xcb, 0x65, 0x05, 0x0c, 0xb7, 0x22, 0x87,
         0x96, 0x21, 0x0d, 0x92, 0x54, 0x87, 0x36, 0x43,
         0x00, 0x8d, 0x45, 0xf2, 0x9c, 0xfd, 0x6c, 0x5b,
         0x06, 0x0c, 0x9a
    };
    std::string expected("9db6dc3a 23abd7b6 c3d72c38 f4843c7d e48a71d0 "
                         "ba91a86b 18393e5f");
    SHA224 sha224;

    // Provide input to the SHA224 object
    sha224.Input(vector);

    // Finalize the message digest
    sha224.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha224.Result());
}

// Test from NIST's test vectors
// Source: https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing#shavs
STF_TEST(SHA224, TestNISTBinary2)
{
    std::uint8_t vector[] =
    {
        0xd4, 0x88, 0xbd, 0xda, 0x40, 0x09, 0x32, 0xde,
        0x56, 0xa9, 0xf1, 0x05, 0xf0, 0xe7, 0x4e, 0xe7,
        0x9c, 0x2e, 0xd8, 0x69, 0xfa, 0xaa, 0xdc, 0x31,
        0x66, 0x5c, 0x2b, 0x5d, 0xd7, 0x69, 0x1d, 0x74,
        0x07, 0xb8, 0xd6, 0x9c, 0xd5, 0xbb, 0xc8, 0x0e,
        0xe7, 0x8d, 0x79, 0xc8, 0x93, 0x3e, 0x87, 0x81,
        0x00, 0x6b, 0xe9, 0xe2, 0x88, 0x11, 0x77, 0xf3,
        0x61, 0xa2, 0xad, 0xd4, 0x7d, 0x59, 0xac, 0xad,
        0xae, 0xcb, 0x81, 0xe7, 0x0e, 0xc0, 0xaa, 0x9d,
        0x9e, 0x40, 0xd6, 0x4d, 0x2d, 0xf1, 0xed, 0xd4,
        0x7c, 0xa0, 0x50, 0x99, 0x9f, 0x1d, 0x62, 0x08,
        0xa6, 0x4c, 0x47, 0x42, 0x7d, 0x79, 0xec, 0x12,
        0x11, 0xec, 0x47, 0x24, 0xb4, 0xbd, 0xe0, 0x71,
        0xc3, 0xea, 0x90, 0xfe, 0x86, 0xa7, 0x88, 0xc6,
        0x57, 0xf5, 0x96, 0xf4, 0x07, 0x76, 0xa4, 0x56,
        0x7d, 0x80, 0x82, 0x0f, 0xa0, 0x70, 0xe5, 0x8c,
        0x78, 0x41, 0xaf, 0xb9, 0x8f, 0x5d, 0x70, 0x74,
        0x9e, 0xb2, 0x2d, 0x42, 0x65, 0x09, 0x61, 0x40,
        0xdb, 0x93, 0x42, 0x5d, 0x87, 0xc5, 0x45, 0xc7,
        0x2e, 0x66, 0xe4, 0xa3, 0xae, 0xe7, 0x9f, 0x01,
        0x2a, 0xf2, 0x41, 0x0f, 0x67, 0x45, 0x64, 0x8c,
        0x76, 0xc1, 0x37, 0x80, 0x94, 0xbe, 0x7f, 0x2b,
        0x4e, 0xa2, 0xdc, 0x3b, 0xe9, 0xb2, 0x4e, 0x42,
        0x00, 0x28, 0xf7, 0x66, 0x6f, 0x85, 0x23, 0x5b,
        0x70, 0x28, 0x85, 0x8e, 0x04, 0x20, 0xc3, 0xa3,
        0x95, 0x3f, 0x36, 0x22, 0xd9, 0x6f, 0xeb, 0x6d,
        0xf9, 0x49, 0xed, 0xf6, 0xf3, 0xe4, 0xc6, 0xae,
        0x3d, 0xeb, 0x43, 0xc1, 0xab, 0xf3, 0x78, 0x80,
        0x92, 0x0b, 0x04, 0x9e, 0x2e, 0x94, 0x96, 0xa0,
        0x41, 0x53, 0xb0, 0x8f, 0xfd, 0xc4, 0xab, 0xf0,
        0x24, 0xb1, 0xfc, 0xf6, 0x6e, 0x04, 0xb5, 0xf5,
        0x2b, 0xda, 0x4f, 0x1a, 0x29, 0xd8, 0xa6, 0x5a,
        0xd8, 0xc2, 0x45, 0xa8, 0xac, 0x49, 0xe4, 0x6b,
        0xc3, 0x25, 0x2f, 0x2f, 0xc8, 0x64, 0xc9, 0x9a,
        0x17, 0x02, 0x91, 0xa2, 0x21, 0xb9, 0x24, 0xfe,
        0x12, 0x1a, 0xa3, 0xda, 0xb2, 0xdd, 0xb6, 0xcf,
        0x89, 0x76, 0xd0, 0x12, 0x47, 0xe9, 0xb5, 0xec,
        0x63, 0xde, 0x3a, 0x6a, 0x01, 0xf8, 0x4c, 0x55,
        0x19, 0x63, 0x34, 0x64, 0xd6, 0xac, 0x19, 0xb1,
        0x3c, 0xd6, 0xc7, 0x6a, 0x0e, 0xaa, 0xaf, 0xc9,
        0xef, 0x4e, 0x2e, 0x46, 0xc7, 0x19, 0x12, 0x0b,
        0x68, 0xa6, 0x5a, 0xa8, 0x72, 0x27, 0x3d, 0x08,
        0x73, 0xfc, 0x6e, 0xa3, 0x53, 0x85, 0x9f, 0xf6,
        0xf0, 0x34, 0x44, 0x30, 0x05, 0xe6, 0x99, 0x2f,
        0x50, 0x73, 0xe6, 0xaa, 0xf4, 0x83, 0x01, 0xb0,
        0x1c
    };
    std::string expected("e12c2c07628185fcc930d02b5fff6408e8eb1aad2bb00328"
                         "30e04ff2");
    SHA224 sha224;

    sha224.SpaceSeparateWords(false); // No spaces in output on this test

    // Provide input to the SHA224 object
    sha224.Input(vector);

    // Finalize the message digest
    sha224.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha224.Result());
}

//
// API Tests
//

// Test expected values
STF_TEST(SHA224, TestExpectedValues)
{
    SHA224 sha224("abc");

    STF_ASSERT_EQ(std::string("23097d22 3405d822 8642a477 bda255b3 2aadbce4 "
                              "bda0b3f7 e36c9da7"),
                 sha224.Result());

    STF_ASSERT_TRUE(sha224.IsFinalized());

    STF_ASSERT_EQ(std::size_t(64), sha224.GetBlockSize());

    STF_ASSERT_EQ(std::size_t(28), sha224.GetDigestLength());
}

// Test Reset() function
STF_TEST(SHA224, TestReset)
{
    SHA224 sha224("abc");

    STF_ASSERT_EQ(std::string("23097d22 3405d822 8642a477 bda255b3 2aadbce4 "
                              "bda0b3f7 e36c9da7"),
                 sha224.Result());

    sha224.Reset();

    STF_ASSERT_FALSE(sha224.IsFinalized());

    STF_ASSERT_FALSE(sha224.IsFinalized());

    STF_ASSERT_EQ(std::uint64_t(0), sha224.GetMessageLength());

    // Verify object reuse works
    sha224.Input("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
    sha224.Finalize();

    STF_ASSERT_EQ(std::string("75388b16 512776cc 5dba5da1 fd890150 b0c6455c "
                              "b4f58b19 52522525"),
                  sha224.Result());
}

// Test streaming operator
STF_TEST(SHA224, TestStreamingOperator)
{
    SHA224 sha224;

    sha224 << "a" << "b" << "c";
    sha224.Finalize();

    STF_ASSERT_EQ(std::uint64_t(3), sha224.GetMessageLength());

    STF_ASSERT_EQ(std::string("23097d22 3405d822 8642a477 bda255b3 2aadbce4 "
                              "bda0b3f7 e36c9da7"),
                  sha224.Result());
}

// Test copy constructor
STF_TEST(SHA224, TestCopyConstructor)
{
    SHA224 sha224("abc");

    STF_ASSERT_EQ(std::string("23097d22 3405d822 8642a477 bda255b3 2aadbce4 "
                              "bda0b3f7 e36c9da7"),
                  sha224.Result());

    SHA224 sha224_copy = sha224;

    STF_ASSERT_EQ(std::uint64_t(3), sha224_copy.GetMessageLength());

    STF_ASSERT_EQ(sha224, sha224_copy);

    STF_ASSERT_TRUE(sha224_copy.IsFinalized());

    STF_ASSERT_EQ(std::string("23097d22 3405d822 8642a477 bda255b3 2aadbce4 "
                              "bda0b3f7 e36c9da7"),
                  sha224_copy.Result());
}

// Test move constructor
STF_TEST(SHA224, TestMoveConstructor)
{
    SHA224 sha224("abc");

    SHA224 sha224_copy = sha224;

    STF_ASSERT_EQ(std::string("23097d22 3405d822 8642a477 bda255b3 2aadbce4 "
                              "bda0b3f7 e36c9da7"),
                  sha224.Result());

    SHA224 sha224_move = std::move(sha224_copy);

    STF_ASSERT_EQ(sha224, sha224_move);

    STF_ASSERT_TRUE(sha224_move.IsFinalized());

    STF_ASSERT_EQ(sha224_move.Result(), sha224.Result());
}

STF_TEST(SHA224, TestEqualOperator)
{
    SHA224 sha224("abc");

    STF_ASSERT_EQ(std::string("23097d22 3405d822 8642a477 bda255b3 2aadbce4 "
                              "bda0b3f7 e36c9da7"),
                  sha224.Result());

    SHA224 sha224_copy = sha224;

    STF_ASSERT_TRUE(sha224_copy == sha224);
}

STF_TEST(SHA224, TestNotEqualOperator)
{
    SHA224 sha224("abc");

    STF_ASSERT_EQ(std::string("23097d22 3405d822 8642a477 bda255b3 2aadbce4 "
                              "bda0b3f7 e36c9da7"),
                  sha224.Result());

    SHA224 sha224_copy;

    STF_ASSERT_TRUE(sha224_copy != sha224);
}

// Test result array
STF_TEST(SHA224, TestResultArray)
{
    SHA224 sha224("abc");

    SHA224::SHA224ResultOctets result_array;

    sha224.Result(result_array);

    std::uint8_t expected_result_octets[] =
    {
        0x23, 0x09, 0x7d, 0x22, 0x34, 0x05, 0xd8, 0x22,
        0x86, 0x42, 0xa4, 0x77, 0xbd, 0xa2, 0x55, 0xb3,
        0x2a, 0xad, 0xbc, 0xe4, 0xbd, 0xa0, 0xb3, 0xf7,
        0xe3, 0x6c, 0x9d, 0xa7
    };

    STF_ASSERT_MEM_EQ(expected_result_octets, result_array, 28);
}

// Test result words
STF_TEST(SHA224, TestResultWords)
{
    SHA224 sha224("abc");

    SHA224::SHA224ResultWords result_words;

    sha224.Result(result_words);

    SHA224::SHA224ResultWords expected_result_words =
    {
        0x23097d22, 0x3405d822, 0x8642a477, 0xbda255b3,
        0x2aadbce4, 0xbda0b3f7, 0xe36c9da7
    };

    STF_ASSERT_EQ(expected_result_words[0], result_words[0]);
    STF_ASSERT_EQ(expected_result_words[1], result_words[1]);
    STF_ASSERT_EQ(expected_result_words[2], result_words[2]);
    STF_ASSERT_EQ(expected_result_words[3], result_words[3]);
    STF_ASSERT_EQ(expected_result_words[4], result_words[4]);
    STF_ASSERT_EQ(expected_result_words[5], result_words[5]);
    STF_ASSERT_EQ(expected_result_words[6], result_words[6]);
}

// Test Input() function
STF_TEST(SHA224, TestInput)
{
    SHA224 sha224("abc");

    SHA224 sha224_anew;

    sha224_anew.Input("a");
    sha224_anew.Input("b");
    sha224_anew.Input("c");
    sha224_anew.Finalize();

    STF_ASSERT_EQ(sha224, sha224_anew);
}

// Test Auto Finalize
STF_TEST(SHA224, TestAutoFinalize1)
{
    SHA224 sha224("abc");

    SHA224 sha224_auto("abc", true);

    STF_ASSERT_EQ(sha224, sha224_auto);
}

// Test Auto Finalize
STF_TEST(SHA224, TestAutoFinalize2)
{
    SHA224 sha224("abc");

    SHA224 sha224_auto("abc", false);
    sha224_auto.Finalize();

    STF_ASSERT_EQ(sha224, sha224_auto);
}

// Test output stream
STF_TEST(SHA224, TestOutputStream)
{
    SHA224 sha224("abc");

    // Test streaming operator
    std::ostringstream oss;
    oss << sha224;
    std::string test_string = oss.str();

    STF_ASSERT_EQ(std::string("23097d22 3405d822 8642a477 bda255b3 2aadbce4 "
                              "bda0b3f7 e36c9da7"),
                  test_string);
}

// Test base class pointer
STF_TEST(SHA224, TestBaseClassPointer)
{
    std::unique_ptr<Hash> hash = std::make_unique<SHA224>("abc");

    STF_ASSERT_EQ(std::string("23097d22 3405d822 8642a477 bda255b3 2aadbce4 "
                              "bda0b3f7 e36c9da7"),
                  hash->Result());
}

// Test exception on additional input
STF_TEST(SHA224, TestExceptionOnExtraInput)
{
    SHA224 sha224("abc");

    bool exception_thrown = false;
    try
    {
        sha224.Input("abc");
    }
    catch(const std::exception &)
    {
        exception_thrown = true;
    }

    STF_ASSERT_TRUE(exception_thrown);
}

// Test exception when not finalized
STF_TEST(SHA224, TestExceptionNotFinalized)
{
    SHA224 sha224_exception;

    bool exception_thrown = false;
    try
    {
        // This should throw an exception since the result was not finalized
        std::string result = sha224_exception.Result();
    }
    catch (const HashException &)
    {
        exception_thrown = true;
    }

    STF_ASSERT_TRUE(exception_thrown);
}

// Under debug builds, exclude these slow tests
#ifndef NDEBUG
STF_TEST_EXCLUDE(SHA224, TestLong)
#endif
