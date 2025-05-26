/*
 *  test_sha384.cpp
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
 *      test the SHA384 object to ensure it produces expected results.
 *      It will also exercise some of the APIs to ensure proper behavior.
 *
 *  Portability Issues:
 *      None.
 */

#include <memory>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <terra/crypto/hashing/sha384.h>
#include <terra/stf/stf.h>

using namespace Terra;
using namespace Terra::Crypto::Hashing;

//
// Test vectors
//

// Zero-length message
STF_TEST(SHA384, TestZeroLength)
{
    SHA384 sha384("");

    // Zero-length message
    STF_ASSERT_EQ(std::string("38b060a751ac9638 4cd9327eb1b1e36a "
                              "21fdb71114be0743 4c0cc7bf63f6e1da "
                              "274edebfe76f65fb d51ad2f14898b95b"),
                  sha384.Result());
}

// Test "abc"
STF_TEST(SHA384, TestABC)
{
    SHA384 sha384("abc");

    STF_ASSERT_EQ(std::string("cb00753f45a35e8b b5a03d699ac65007 "
                              "272c32ab0eded163 1a8b605a43ff5bed "
                              "8086072ba1e7cc23 58baeca134c825a7"),
                  sha384.Result());
}

// Test a 448-bit message
STF_TEST(SHA384, Test448Bit)
{
    SHA384 sha384("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");

    STF_ASSERT_EQ(std::string("3391fdddfc8dc739 3707a65b1b470939 "
                              "7cf8b1d162af05ab fe8f450de5f36bc6 "
                              "b0455a8520bc4e6f 5fe95b1fe3c8452b"),
                  sha384.Result());
}

// Test a 456-bit message
STF_TEST(SHA384, Test456Bit)
{
    SHA384 sha384("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopqr");

    STF_ASSERT_EQ(std::string("cbbeb905f80164c6 6a02d6758a06f37c "
                              "aa62b56679a83819 a32ffbc740ef3109 "
                              "0dae487187e67a16 40fb64a8508d9569"),
                  sha384.Result());
}

// Test a 896-bit message
STF_TEST(SHA384, Test896Bit)
{
    SHA384 sha384("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhij"
                  "klmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu");

    STF_ASSERT_EQ(std::string("09330c33f71147e8 3d192fc782cd1b47 "
                              "53111b173b3b05d2 2fa08086e3b0f712 "
                              "fcc7c71a557e2db9 66c3e9fa91746039"),
                  sha384.Result());
}

// Test a 904-bit message
STF_TEST(SHA384, Test904Bit)
{
    SHA384 sha384("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhij"
                  "klmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstuv");

    STF_ASSERT_EQ(std::string("1bf9d3a2ba2f8d8d b25eb125ae170c42 "
                              "c0fa109277474c24 9ca8eb0334dc09af "
                              "aae0aa4b6b739193 0a8dfee33a3501db"),
                    sha384.Result());
}

// Test a 1,000,000 character string of a's
STF_TEST(SHA384, TestMillion)
{
    std::string million_a(1000000, 'a');
    SHA384 sha384(million_a);

    STF_ASSERT_EQ(std::string("9d0e1809716474cb 086e834e310a4a1c "
                              "ed149e9c00f24852 7972cec5704c2a5b "
                              "07b8b3dc38ecc4eb ae97ddd87f3d8985"),
                  sha384.Result());
}

// Test of 128 zero-value octets
STF_TEST(SHA384, Test128Zeros)
{
    std::string zeros_128(128, 0x00);
    SHA384 sha384(zeros_128);

    STF_ASSERT_EQ(std::string("f809b88323411f24 a6f152e5e9d9d1b5 "
                              "466b77e0f3c7550f 8b242c31b6e7b99b "
                              "cb45bdecb6124bc2 3283db3b9fc4f5b3"),
                  sha384.Result());
}

// Test of 257 zero-value octets
STF_TEST(SHA384, Test257Zeros)
{
    std::string zeros_257(257, 0x00);
    SHA384 sha384(zeros_257);

    STF_ASSERT_EQ(std::string("90c5083d81f30744 c206534dfa7fbc32 "
                              "d948de3090af1085 9b30ff90a2ecf8b8 "
                              "c6eea6f05c622c0b 1944e155362df4b0"),
                  sha384.Result());
}

// Perform a very long test
STF_TEST(SHA384, TestLong)
{
    std::string message("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmgh"
                        "ijklmnhijklmno");
    std::string expected("5441235cc0235341 ed806a64fb354742 "
                         "b5e5c02a3c5cb71b 5f63fb793458d8fd "
                         "ae599c8cd8884943 c04f11b31b89f023");
    SHA384 sha384;

    // Provide a ton of input
    for (std::size_t i = 0; i < 16'777'216UL; i++) sha384.Input(message);

    // Finalize the message digest
    sha384.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha384.Result());
}

// Test from NIST's test vectors
// Source: https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing#shavs
STF_TEST(SHA384, TestNISTBinary1)
{
    std::uint8_t vector[] =
    {
        0x62, 0xc6, 0xa1, 0x69, 0xb9, 0xbe, 0x02, 0xb3,
        0xd7, 0xb4, 0x71, 0xa9, 0x64, 0xfc, 0x0b, 0xcc,
        0x72, 0xb4, 0x80, 0xd2, 0x6a, 0xec, 0xb2, 0xed,
        0x46, 0x0b, 0x7f, 0x50, 0x01, 0x6d, 0xda, 0xf0,
        0x4c, 0x51, 0x21, 0x87, 0x83, 0xf3, 0xaa, 0xdf,
        0xdf, 0xf5, 0xa0, 0x4d, 0xed, 0x03, 0x0d, 0x7b,
        0x3f, 0xb7, 0x37, 0x6b, 0x61, 0xba, 0x30, 0xb9,
        0x0e, 0x2d, 0xa9, 0x21, 0xa4, 0x47, 0x07, 0x40,
        0xd6, 0x3f, 0xb9, 0x9f, 0xa1, 0x6c, 0xc8, 0xed,
        0x81, 0xab, 0xaf, 0x8c, 0xe4, 0x01, 0x6e, 0x50,
        0xdf, 0x81, 0xda, 0x83, 0x20, 0x70, 0x37, 0x2c,
        0x24, 0xa8, 0x08, 0x90, 0xaa, 0x3a, 0x26, 0xfa,
        0x67, 0x57, 0x10, 0xb8, 0xfb, 0x71, 0x82, 0x66,
        0x24, 0x9d, 0x49, 0x6f, 0x31, 0x3c, 0x55, 0xd0,
        0xba, 0xda, 0x10, 0x1f, 0x8f, 0x56, 0xee, 0xcc,
        0xee, 0x43, 0x45, 0xa8, 0xf9, 0x8f, 0x60, 0xa3,
        0x66, 0x62, 0xcf, 0xda, 0x79, 0x49, 0x00, 0xd1,
        0x2f, 0x94, 0x14, 0xfc, 0xbd, 0xfd, 0xeb, 0x85,
        0x38, 0x8a, 0x81, 0x49, 0x96, 0xb4, 0x7e, 0x24,
        0xd5, 0xc8, 0x08, 0x6e, 0x7a, 0x8e, 0xdc, 0xc5,
        0x3d, 0x29, 0x9d, 0x0d, 0x03, 0x3e, 0x6b, 0xb6,
        0x0c, 0x58, 0xb8, 0x3d, 0x6e, 0x8b, 0x57, 0xf6,
        0xc2, 0x58, 0xd6, 0x08, 0x1d, 0xd1, 0x0e, 0xb9,
        0x42, 0xfd, 0xf8, 0xec, 0x15, 0x7e, 0xc3, 0xe7,
        0x53, 0x71, 0x23, 0x5a, 0x81, 0x96, 0xeb, 0x9d,
        0x22, 0xb1, 0xde, 0x3a, 0x2d, 0x30, 0xc2, 0xab,
        0xbe, 0x0d, 0xb7, 0x65, 0x0c, 0xf6, 0xc7, 0x15,
        0x9b, 0xac, 0xbe, 0x29, 0xb3, 0xa9, 0x3c, 0x92,
        0x10, 0x05, 0x08
    };
    std::string expected("0730e184e7795575569f87030260bb8e54498e0e5d096b182"
                         "85e988d245b6f3486d1f2447d5f85bcbe59d5689fc49425");
    SHA384 sha384;

    sha384.SpaceSeparateWords(false); // No spaces in output on this test

    // Provide input to the SHA384 object
    sha384.Input(vector);

    // Finalize the message digest
    sha384.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha384.Result());
}

// Test from NIST's test vectors
// Source: https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing#shavs
STF_TEST(SHA384, TestNISTBinary2)
{
    std::uint8_t vector[] =
    {
        0xff, 0xd6, 0x78, 0x90, 0xff, 0x77, 0xf3, 0x44,
        0xad, 0x4f, 0x06, 0x7d, 0xf2, 0xf4, 0xff, 0x1d,
        0xb8, 0xf5, 0x41, 0xc7, 0xa2, 0xbd, 0x9a, 0xe9,
        0xfa, 0xba, 0xd0, 0xfa, 0xeb, 0xbf, 0x7d, 0x00,
        0xf0, 0xa7, 0x1d, 0x56, 0x8c, 0x3c, 0x66, 0xac,
        0x3c, 0x57, 0xd8, 0x4f, 0xaa, 0x48, 0x94, 0xab,
        0x23, 0x77, 0x71, 0x0e, 0x4b, 0x4c, 0x4d, 0xae,
        0x0f, 0x4d, 0xa1, 0xee, 0xdc, 0x86, 0x58, 0xdd,
        0x0e, 0x2e, 0xe2, 0xff, 0xac, 0x87, 0x84, 0x51,
        0x52, 0x06, 0xf2, 0x87, 0x6e, 0xb4, 0x1f, 0x98,
        0xaf, 0xd4, 0x54, 0x7c, 0xbb, 0xc6, 0x80, 0x34,
        0x21, 0x2b, 0xcf, 0x0c, 0x8e, 0x4a, 0x7d, 0x1d,
        0x43, 0xb3, 0xed, 0x15, 0xc6, 0x21, 0xf5, 0x3b,
        0xd8, 0xa5, 0x7c, 0xad, 0xa8, 0x01, 0x48, 0xec,
        0x46, 0x52, 0x11, 0x9b, 0x5a, 0xf3, 0xda, 0x84,
        0x16, 0x9d, 0x81, 0xdc, 0x69, 0xd3, 0x94, 0xc8,
        0x76, 0x7d, 0x66, 0x20, 0x44, 0xd3, 0x62, 0x72,
        0xb7, 0x7c, 0xa0, 0x4a, 0xbf, 0xf7, 0xb6, 0xb0,
        0xcf, 0x3b, 0xd1, 0xf3, 0x91, 0x9a, 0x04, 0xa5,
        0xd8, 0xeb, 0xdf, 0xe7, 0xd6, 0xe8, 0x44, 0xe7,
        0x8f, 0xd5, 0x76, 0xa6, 0x8d, 0x63, 0x73, 0xff,
        0xd5, 0xd3, 0x84, 0xe5, 0x1b, 0x5e, 0x12, 0xec,
        0x32, 0xd5, 0xbb, 0x0a, 0xc6, 0x85, 0xa5, 0x9f,
        0x4d, 0x5d, 0x12, 0xb4, 0x3b, 0x53, 0x35, 0x80,
        0x75, 0x03, 0x45, 0x31, 0x09, 0x99, 0xcf, 0xe9,
        0x1c, 0xf2, 0x50, 0x06, 0x24, 0xfe, 0x03, 0xa6,
        0x57, 0x69, 0xf8, 0x6a, 0x62, 0x7a, 0x66, 0x7b,
        0x5f, 0x3b, 0x42, 0xcb, 0x01, 0xda, 0x10, 0x9e,
        0x12, 0x4f, 0xfa, 0x48, 0x20, 0x3f, 0x1f, 0x38,
        0x73, 0x20, 0x2d, 0x35, 0x42, 0x9f, 0x32, 0xe8,
        0x26, 0x3e, 0xaf, 0x9b, 0xce, 0x42, 0xef, 0x40,
        0xf5, 0xcc, 0x96, 0xb5, 0x91, 0x46, 0x7d, 0x46,
        0x4d, 0x00, 0xbd, 0x74, 0x3a, 0x1b, 0x0a, 0xf4,
        0xc1, 0xa7, 0x43, 0xfb, 0xdd, 0x08, 0x46, 0xb9,
        0x87, 0x9e, 0x09, 0x23, 0x71, 0xa5, 0xe7, 0xf6,
        0xf6, 0x59, 0x37, 0xf9, 0x51, 0x5e, 0x23, 0x82,
        0x0e, 0x60, 0xb8, 0x3b, 0xbf, 0xf7, 0x39, 0x26,
        0xf0, 0xcd, 0xb9, 0xdf, 0x5d, 0x02, 0xe8, 0x22,
        0x62, 0xcf, 0x2e, 0x8c, 0xb2, 0x6a, 0xf6, 0xa6,
        0x4c, 0x2a, 0x4d, 0x1f, 0xab, 0xec, 0xab, 0x59,
        0x3d, 0xb5, 0x10, 0x28, 0x17, 0x99
    };
    std::string expected("1396fea95ce0c1c1c224b50a07dd7197f1d62b993c7fe9e1c"
                         "c1a56101920d4b0fecef587fbcd56b854c8c9da95132f02");
    SHA384 sha384;

    sha384.SpaceSeparateWords(false); // No spaces in output on this test

    // Provide input to the SHA384 object
    sha384.Input(vector);

    // Finalize the message digest
    sha384.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha384.Result());
}

//
// API Tests
//

// Test expected values
STF_TEST(SHA384, TestExpectedValues)
{
    SHA384 sha384("abc");

    STF_ASSERT_EQ(std::string("cb00753f45a35e8b b5a03d699ac65007 "
                              "272c32ab0eded163 1a8b605a43ff5bed "
                              "8086072ba1e7cc23 58baeca134c825a7"),
                  sha384.Result());

    STF_ASSERT_TRUE(sha384.IsFinalized());

    STF_ASSERT_EQ(std::size_t(128), sha384.GetBlockSize());

    STF_ASSERT_EQ(std::size_t(48), sha384.GetDigestLength());
}

// Test Reset() function
STF_TEST(SHA384, TestReset)
{
    SHA384 sha384("abc");

    STF_ASSERT_EQ(std::string("cb00753f45a35e8b b5a03d699ac65007 "
                              "272c32ab0eded163 1a8b605a43ff5bed "
                              "8086072ba1e7cc23 58baeca134c825a7"),
                  sha384.Result());

    sha384.Reset();

    STF_ASSERT_FALSE(sha384.IsFinalized());

    STF_ASSERT_FALSE(sha384.IsFinalized());

    STF_ASSERT_EQ((SHA384MessageLength{0, 0}), sha384.GetMessageLength());

    // Verify object reuse works
    sha384.Input("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
    sha384.Finalize();

    STF_ASSERT_EQ(std::string("3391fdddfc8dc739 3707a65b1b470939 "
                              "7cf8b1d162af05ab fe8f450de5f36bc6 "
                              "b0455a8520bc4e6f 5fe95b1fe3c8452b"),
                  sha384.Result());
}

// Test streaming operator
STF_TEST(SHA384, TestStreamingOperator)
{
    SHA384 sha384;

    sha384 << "a" << "b" << "c";
    sha384.Finalize();

    STF_ASSERT_EQ((SHA384MessageLength{0, 3}), sha384.GetMessageLength());

    STF_ASSERT_EQ(std::string("cb00753f45a35e8b b5a03d699ac65007 "
                              "272c32ab0eded163 1a8b605a43ff5bed "
                              "8086072ba1e7cc23 58baeca134c825a7"),
                  sha384.Result());
}

// Test copy constructor
STF_TEST(SHA384, TestCopyConstructor)
{
    SHA384 sha384("abc");

    STF_ASSERT_EQ(std::string("cb00753f45a35e8b b5a03d699ac65007 "
                              "272c32ab0eded163 1a8b605a43ff5bed "
                              "8086072ba1e7cc23 58baeca134c825a7"),
                  sha384.Result());

    SHA384 sha384_copy = sha384;

    STF_ASSERT_EQ((SHA384MessageLength{0, 3}), sha384_copy.GetMessageLength());

    STF_ASSERT_EQ(sha384, sha384_copy);

    STF_ASSERT_TRUE(sha384_copy.IsFinalized());

    STF_ASSERT_EQ(std::string("cb00753f45a35e8b b5a03d699ac65007 "
                              "272c32ab0eded163 1a8b605a43ff5bed "
                              "8086072ba1e7cc23 58baeca134c825a7"),
                  sha384_copy.Result());
}

// Test move constructor
STF_TEST(SHA384, TestMoveConstructor)
{
    SHA384 sha384("abc");

    SHA384 sha384_copy = sha384;

    STF_ASSERT_EQ(std::string("cb00753f45a35e8b b5a03d699ac65007 "
                              "272c32ab0eded163 1a8b605a43ff5bed "
                              "8086072ba1e7cc23 58baeca134c825a7"),
                  sha384.Result());

    SHA384 sha384_move = std::move(sha384_copy);

    STF_ASSERT_EQ(sha384, sha384_move);

    STF_ASSERT_TRUE(sha384_move.IsFinalized());

    STF_ASSERT_EQ(sha384_move.Result(), sha384.Result());
}

// Test equal constructor
STF_TEST(SHA384, TestEqualOperator)
{
    SHA384 sha384("abc");

    STF_ASSERT_EQ(std::string("cb00753f45a35e8b b5a03d699ac65007 "
                              "272c32ab0eded163 1a8b605a43ff5bed "
                              "8086072ba1e7cc23 58baeca134c825a7"),
                  sha384.Result());

    SHA384 sha384_copy = sha384;

    STF_ASSERT_TRUE(sha384_copy == sha384);
}

// Test not equal constructor
STF_TEST(SHA384, TestNotEqualOperator)
{
    SHA384 sha384("abc");

    STF_ASSERT_EQ(std::string("cb00753f45a35e8b b5a03d699ac65007 "
                              "272c32ab0eded163 1a8b605a43ff5bed "
                              "8086072ba1e7cc23 58baeca134c825a7"),
                  sha384.Result());

    SHA384 sha384_copy;

    STF_ASSERT_TRUE(sha384_copy != sha384);
}

// Test result array
STF_TEST(SHA384, TestResultArray)
{
    SHA384 sha384("abc");

    SHA384::SHA384ResultOctets result_array;

    sha384.Result(result_array);

    std::uint8_t expected_result_octets[] =
    {
        0xcb, 0x00, 0x75, 0x3f, 0x45, 0xa3, 0x5e, 0x8b,
        0xb5, 0xa0, 0x3d, 0x69, 0x9a, 0xc6, 0x50, 0x07,
        0x27, 0x2c, 0x32, 0xab, 0x0e, 0xde, 0xd1, 0x63,
        0x1a, 0x8b, 0x60, 0x5a, 0x43, 0xff, 0x5b, 0xed,
        0x80, 0x86, 0x07, 0x2b, 0xa1, 0xe7, 0xcc, 0x23,
        0x58, 0xba, 0xec, 0xa1, 0x34, 0xc8, 0x25, 0xa7
    };

    STF_ASSERT_MEM_EQ(expected_result_octets, result_array, 48);
}

// Test result words
STF_TEST(SHA384, TestResultWords)
{
    SHA384 sha384("abc");

    SHA384::SHA384ResultWords result_words;

    sha384.Result(result_words);

    SHA384::SHA384ResultWords expected_result_words =
    {
        0xcb00753f45a35e8b, 0xb5a03d699ac65007,
        0x272c32ab0eded163, 0x1a8b605a43ff5bed,
        0x8086072ba1e7cc23, 0x58baeca134c825a7
    };

    STF_ASSERT_EQ(expected_result_words[0], result_words[0]);
    STF_ASSERT_EQ(expected_result_words[1], result_words[1]);
    STF_ASSERT_EQ(expected_result_words[2], result_words[2]);
    STF_ASSERT_EQ(expected_result_words[3], result_words[3]);
    STF_ASSERT_EQ(expected_result_words[4], result_words[4]);
    STF_ASSERT_EQ(expected_result_words[5], result_words[5]);
}

// Test Input() function
STF_TEST(SHA384, TestInput)
{
    SHA384 sha384("abc");

    SHA384 sha384_anew;

    sha384_anew.Input("a");
    sha384_anew.Input("b");
    sha384_anew.Input("c");
    sha384_anew.Finalize();

    STF_ASSERT_EQ(sha384, sha384_anew);
}

// Test Auto Finalize
STF_TEST(SHA384, TestAutoFinalize1)
{
    SHA384 sha384("abc");

    SHA384 sha384_auto("abc", true);

    STF_ASSERT_EQ(sha384, sha384_auto);
}

// Test Auto Finalize
STF_TEST(SHA384, TestAutoFinalize2)
{
    SHA384 sha384("abc");

    SHA384 sha384_auto("abc", false);
    sha384_auto.Finalize();

    STF_ASSERT_EQ(sha384, sha384_auto);
}

// Test output stream
STF_TEST(SHA384, TestOutputStream)
{
    SHA384 sha384("abc");

    // Test streaming operator
    std::ostringstream oss;
    oss << sha384;
    std::string test_string = oss.str();

    STF_ASSERT_EQ(std::string("cb00753f45a35e8b b5a03d699ac65007 "
                              "272c32ab0eded163 1a8b605a43ff5bed "
                              "8086072ba1e7cc23 58baeca134c825a7"),
                  test_string);
}

// Test base class pointer
STF_TEST(SHA384, TestBaseClassPointer)
{
    std::unique_ptr<Hash> hash = std::make_unique<SHA384>("abc");

    STF_ASSERT_EQ(std::string("cb00753f45a35e8b b5a03d699ac65007 "
                              "272c32ab0eded163 1a8b605a43ff5bed "
                              "8086072ba1e7cc23 58baeca134c825a7"),
                 hash->Result());
}

// Test exception on additional input
STF_TEST(SHA384, TestExceptionOnExtraInput)
{
    SHA384 sha384("abc");

    bool exception_thrown = false;
    try
    {
        sha384.Input("abc");
    }
    catch(const std::exception &)
    {
        exception_thrown = true;
    }

    STF_ASSERT_TRUE(exception_thrown);
}

// Test exception when not finalized
STF_TEST(SHA384, TestExceptionNotFinalized)
{
    SHA384 sha384_exception;

    bool exception_thrown = false;
    try
    {
        // This should throw an exception since the result was not finalized
        std::string result = sha384_exception.Result();
    }
    catch (const HashException &)
    {
        exception_thrown = true;
    }

    STF_ASSERT_TRUE(exception_thrown);
}

// Under debug builds, exclude these slow tests
#ifndef NDEBUG
STF_TEST_EXCLUDE(SHA384, TestLong)
#endif
