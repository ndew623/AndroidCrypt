/*
 *  test_sha512.cpp
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
 *      test the SHA512 object to ensure it produces expected results.
 *      It will also exercise some of the APIs to ensure proper behavior.
 *
 *  Portability Issues:
 *      None.
 */

#include <memory>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <terra/crypto/hashing/sha512.h>
#include <terra/stf/stf.h>

using namespace Terra;
using namespace Terra::Crypto::Hashing;

//
// Test vectors
//

// Zero-length message
STF_TEST(SHA512, TestZeroLength)
{
    SHA512 sha512("");

    // Zero-length message
    STF_ASSERT_EQ(std::string("cf83e1357eefb8bd f1542850d66d8007 "
                              "d620e4050b5715dc 83f4a921d36ce9ce "
                              "47d0d13c5d85f2b0 ff8318d2877eec2f "
                              "63b931bd47417a81 a538327af927da3e"),
                  sha512.Result());
}

// Test "abc"
STF_TEST(SHA512, TestABC)
{
    SHA512 sha512("abc");

    STF_ASSERT_EQ(std::string("ddaf35a193617aba cc417349ae204131 "
                              "12e6fa4e89a97ea2 0a9eeee64b55d39a "
                              "2192992a274fc1a8 36ba3c23a3feebbd "
                              "454d4423643ce80e 2a9ac94fa54ca49f"),
                  sha512.Result());
}

// Test a 448-bit message
STF_TEST(SHA512, Test448Bit)
{
    SHA512 sha512("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");

    STF_ASSERT_EQ(std::string("204a8fc6dda82f0a 0ced7beb8e08a416 "
                              "57c16ef468b228a8 279be331a703c335 "
                              "96fd15c13b1b07f9 aa1d3bea57789ca0 "
                              "31ad85c7a71dd703 54ec631238ca3445"),
                  sha512.Result());
}

// Test a 456-bit message
STF_TEST(SHA512, Test456Bit)
{
    SHA512 sha512("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopqr");

    STF_ASSERT_EQ(std::string("cbc3b3a4e99c74e4 e4f23e14828b7228 "
                              "de8945145f700ebd 4cc412e21510df52 "
                              "e9a0f4f6b5ad508e 25c3189ebbe19d89 "
                              "bc038f036dada88e 2c44c4413ec000ce"),
                  sha512.Result());
}

// Test a 896-bit message
STF_TEST(SHA512, Test896Bit)
{
    SHA512 sha512("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhij"
                  "klmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu");

    STF_ASSERT_EQ(std::string("8e959b75dae313da 8cf4f72814fc143f "
                              "8f7779c6eb9f7fa1 7299aeadb6889018 "
                              "501d289e4900f7e4 331b99dec4b5433a "
                              "c7d329eeb6dd2654 5e96e55b874be909"),
                  sha512.Result());
}

// Test a 904-bit message
STF_TEST(SHA512, Test904Bit)
{
    SHA512 sha512("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhij"
                  "klmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstuv");

    STF_ASSERT_EQ(std::string("3bac30b4b4933ab8 a535cb7b240788a3 "
                              "4177ac2f7b76d8d0 c8fc154ef9f48d5d "
                              "5bf880f32c9ce525 599d496c6daa77c3 "
                              "e32f5072a9e41b68 4e8dddfb3d816d8d"),
                    sha512.Result());
}

// Test a 1,000,000 character string of a's
STF_TEST(SHA512, TestMillion)
{
    std::string million_a(1000000, 'a');
    SHA512 sha512(million_a);

    STF_ASSERT_EQ(std::string("e718483d0ce76964 4e2e42c7bc15b463 "
                              "8e1f98b13b204428 5632a803afa973eb "
                              "de0ff244877ea60a 4cb0432ce577c31b "
                              "eb009c5c2c49aa2e 4eadb217ad8cc09b"),
                  sha512.Result());
}

// Test of 128 zero-value octets
STF_TEST(SHA512, Test128Zeros)
{
    std::string zeros_128(128, 0x00);
    SHA512 sha512(zeros_128);

    STF_ASSERT_EQ(std::string("ab942f526272e456 ed68a979f5020290 "
                              "5ca903a141ed9844 3567b11ef0bf25a5 "
                              "52d639051a01be58 558122c58e3de07d "
                              "749ee59ded36acf0 c55cd91924d6ba11"),
                  sha512.Result());
}

// Test of 257 zero-value octets
STF_TEST(SHA512, Test257Zeros)
{
    std::string zeros_257(257, 0x00);
    SHA512 sha512(zeros_257);

    STF_ASSERT_EQ(std::string("bc5bfa962850742a 502759b477df26cd "
                              "53716fbd3864a53f 3eb726530b535e80 "
                              "74bc55b5d95a8b51 6c835564037b6c4a "
                              "8dc41b561ec5e1a8 b8f01d4a0b765ada"),
                  sha512.Result());
}

// Perform a very long test
STF_TEST(SHA512, TestLong)
{
    std::string message("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmgh"
                        "ijklmnhijklmno");
    std::string expected("b47c933421ea2db1 49ad6e10fce6c7f9 3d0752380180ffd7 "
                         "f4629a712134831d 77be6091b819ed35 2c2967a2e2d4fa50 "
                         "50723c9630691f1a 05a7281dbe6c1086");
    SHA512 sha512;

    // Provide a ton of input
    for (std::size_t i = 0; i < 16'777'216UL; i++) sha512.Input(message);

    // Finalize the message digest
    sha512.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha512.Result());
}

// Test from NIST's test vectors
// Source: https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing#shavs
STF_TEST(SHA512, TestNISTBinary1)
{
    std::uint8_t vector[] =
    {
        0x4f, 0x05, 0x60, 0x09, 0x50, 0x66, 0x4d, 0x51,
        0x90, 0xa2, 0xeb, 0xc2, 0x9c, 0x9e, 0xdb, 0x89,
        0xc2, 0x00, 0x79, 0xa4, 0xd3, 0xe6, 0xbc, 0x3b,
        0x27, 0xd7, 0x5e, 0x34, 0xe2, 0xfa, 0x3d, 0x02,
        0x76, 0x85, 0x02, 0xbd, 0x69, 0x79, 0x00, 0x78,
        0x59, 0x8d, 0x5f, 0xcf, 0x3d, 0x67, 0x79, 0xbf,
        0xed, 0x12, 0x84, 0xbb, 0xe5, 0xad, 0x72, 0xfb,
        0x45, 0x60, 0x15, 0x18, 0x1d, 0x95, 0x87, 0xd6,
        0xe8, 0x64, 0xc9, 0x40, 0x56, 0x4e, 0xaa, 0xfb,
        0x4f, 0x2f, 0xea, 0xd4, 0x34, 0x6e, 0xa0, 0x9b,
        0x68, 0x77, 0xd9, 0x34, 0x0f, 0x6b, 0x82, 0xeb,
        0x15, 0x15, 0x88, 0x08, 0x72, 0x21, 0x3d, 0xa3,
        0xad, 0x88, 0xfe, 0xba, 0x9f, 0x4f, 0x13, 0x81,
        0x7a, 0x71, 0xd6, 0xf9, 0x0a, 0x1a, 0x17, 0xc4,
        0x3a, 0x15, 0xc0, 0x38, 0xd9, 0x88, 0xb5, 0xb2,
        0x9e, 0xdf, 0xfe, 0x2d, 0x6a, 0x06, 0x28, 0x13,
        0xce, 0xdb, 0xe8, 0x52, 0xcd, 0xe3, 0x02, 0xb3,
        0xe3, 0x3b, 0x69, 0x68, 0x46, 0xd2, 0xa8, 0xe3,
        0x6b, 0xd6, 0x80, 0xef, 0xcc, 0x6c, 0xd3, 0xf9,
        0xe9, 0xa4, 0xc1, 0xae, 0x8c, 0xac, 0x10, 0xcc,
        0x52, 0x44, 0xd1, 0x31, 0x67, 0x71, 0x40, 0x39,
        0x91, 0x76, 0xed, 0x46, 0x70, 0x00, 0x19, 0xa0,
        0x04, 0xa1, 0x63, 0x80, 0x6f, 0x7f, 0xa4, 0x67,
        0xfc, 0x4e, 0x17, 0xb4, 0x61, 0x7b, 0xbd, 0x76,
        0x41, 0xaa, 0xff, 0x7f, 0xf5, 0x63, 0x96, 0xba,
        0x8c, 0x08, 0xa8, 0xbe, 0x10, 0x0b, 0x33, 0xa2,
        0x0b, 0x5d, 0xaf, 0x13, 0x4a, 0x2a, 0xef, 0xa5,
        0xe1, 0xc3, 0x49, 0x67, 0x70, 0xdc, 0xf6, 0xba,
        0xa4, 0xf7, 0xbb
    };
    std::string expected("a9db490c708cc72548d78635aa7da79bb253f945d710e5cb"
                         "677a474efc7c65a2aab45bc7ca1113c8ce0f3c32e1399de9"
                         "c459535e8816521ab714b2a6cd200525");
    SHA512 sha512;

    sha512.SpaceSeparateWords(false); // No spaces in output on this test

    // Provide input to the SHA512 object
    sha512.Input(vector);

    // Finalize the message digest
    sha512.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha512.Result());
}

// Test from NIST's test vectors
// Source: https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing#shavs
STF_TEST(SHA512, TestNISTBinary2)
{
    std::uint8_t vector[] =
    {
        0xed, 0xba, 0x7d, 0x63, 0x12, 0x14, 0x4e, 0x90,
        0xec, 0x9e, 0xaa, 0xce, 0x75, 0x76, 0x04, 0x5a,
        0x46, 0xe5, 0x53, 0xdc, 0xb8, 0xee, 0x5a, 0x98,
        0x49, 0x27, 0x2d, 0xe8, 0x41, 0xc8, 0xf1, 0xea,
        0x95, 0x02, 0xe3, 0x76, 0x0b, 0x92, 0x9c, 0x53,
        0x9f, 0x95, 0x93, 0xaa, 0x25, 0x9e, 0xe3, 0xba,
        0x31, 0x06, 0x4e, 0xce, 0x34, 0x83, 0x82, 0x6c,
        0xe7, 0xfc, 0x9c, 0xd0, 0xed, 0x9c, 0xeb, 0x99,
        0xb4, 0x26, 0xf6, 0xc1, 0x9c, 0x88, 0xc0, 0xd2,
        0x5b, 0xb2, 0x4a, 0x57, 0xa0, 0xa8, 0x5a, 0x91,
        0xf6, 0xd6, 0x7b, 0x6c, 0xeb, 0xa5, 0x45, 0xb1,
        0x72, 0x4b, 0xde, 0x7d, 0x87, 0xe7, 0xc1, 0xf2,
        0x11, 0xcd, 0xbe, 0x2c, 0x0c, 0x1e, 0x42, 0x77,
        0xe7, 0x9d, 0xd9, 0xbf, 0x2b, 0xbd, 0xf1, 0x48,
        0x5e, 0xff, 0xe1, 0x9f, 0x27, 0x7a, 0x2a, 0xd0,
        0xb3, 0xbb, 0xfc, 0xd9, 0x56, 0xd0, 0xfc, 0x5f,
        0x76, 0xa0, 0x44, 0xa0, 0xa5, 0x7e, 0xab, 0xa9,
        0x0d, 0xfd, 0x47, 0xc8, 0x11, 0x05, 0x31, 0x45,
        0x31, 0x26, 0x88, 0x4d, 0x2e, 0x2d, 0xb9, 0x2f,
        0x6f, 0xf5, 0xfd, 0xcb, 0xa4, 0x80, 0x23, 0x46,
        0xa9, 0x46, 0x8f, 0x03, 0xad, 0xcd, 0x83, 0xcf,
        0x93, 0x7a, 0x9d, 0x5c, 0x98, 0xa7, 0xa1, 0xf1,
        0x37, 0xf6, 0x79, 0x5f, 0x4a, 0xe4, 0x7a, 0x37,
        0x71, 0xa1, 0xe9, 0x3a, 0xb7, 0x1f, 0xbe, 0x11,
        0x73, 0x68, 0xa7, 0x3f, 0x6d, 0x36, 0x50, 0x53,
        0x42, 0xbd, 0xe2, 0x62, 0x0a, 0xcc, 0xca, 0xb4,
        0x37, 0x14, 0xbb, 0xbe, 0xc0, 0xc7, 0xf6, 0x50,
        0x80, 0x66, 0xc9, 0x10, 0xd9, 0xd4, 0x55, 0x27,
        0x9c, 0xae, 0x95, 0x3a, 0x2d, 0xdf, 0x9c, 0x9c,
        0xe2, 0x6e, 0x1e, 0xc2, 0xad, 0x9b, 0x32, 0xf7,
        0xfb, 0x2b, 0x56, 0x50, 0xdb, 0x00, 0xb4, 0xe3,
        0x63, 0xec, 0xa4, 0x34, 0x4f, 0xc7, 0x71, 0xf0,
        0xec, 0xbf, 0x60, 0xde, 0x35, 0xf0, 0xee, 0x12,
        0x3b, 0x35, 0x5a, 0x64, 0xca, 0x3d, 0x63, 0x20,
        0x49, 0xe1, 0x53, 0xff, 0xe2, 0xb4, 0x3c, 0x56,
        0xe3, 0xdc, 0x82, 0x8e, 0x64, 0x21, 0x99, 0xd4,
        0x2b, 0x44, 0x10, 0x11, 0xcc, 0x92, 0xcf, 0x1d,
        0x14, 0xb8, 0x9d, 0x30, 0xa8, 0xd9, 0xe5, 0x99,
        0xea, 0xde, 0xb9, 0xb2, 0x1c, 0x0e, 0x17, 0x13,
        0xc8, 0xde, 0x6b, 0x06, 0x5f, 0x0a, 0x56, 0x3b,
        0x20, 0xe4, 0xae, 0xbd, 0x3b, 0xec, 0xc6, 0x25,
        0x37, 0xa2, 0xef, 0x0e, 0x91, 0x98, 0x3e, 0xc8,
        0xa6, 0xcb, 0x90, 0x51, 0xd2, 0x33, 0xcc, 0x80,
        0xda, 0x99, 0x68, 0x84, 0x87, 0xa9, 0x2f, 0x8d,
        0x94, 0x48, 0x41, 0xc9, 0xcb, 0x28, 0x5b, 0x92,
        0x6a, 0x99, 0x58, 0xe6, 0xcb, 0xa2, 0xf8, 0x17,
        0x44, 0x50, 0xe1, 0xc3, 0x4d, 0x4e, 0xe3, 0x07,
        0xe0, 0xb6, 0x80, 0xf0, 0x26, 0x27, 0xac, 0x32,
        0x4f, 0x18, 0xd1, 0x1d, 0xc1, 0x6b, 0x16, 0xcc,
        0x76, 0x45, 0xe1, 0xa0, 0x9f, 0x1e, 0x9a, 0xed,
        0x8e, 0x14, 0xb3, 0xf2, 0xd2, 0x85, 0xe5, 0x0f,
        0xa4, 0xdc, 0xbc, 0x32, 0x82, 0xab, 0x55, 0x93,
        0x19, 0xfb, 0x67, 0xa1, 0x68, 0xe0, 0x80, 0x3b,
        0x23, 0x50, 0x91, 0x24, 0x55, 0x34, 0xad, 0xb1,
        0x4a, 0xc0, 0x0e, 0xc5, 0xac, 0x00, 0x59, 0xff,
        0xec, 0xb4, 0x22, 0x32, 0xe3, 0x5e, 0xb3, 0x16,
        0xba, 0x99, 0x09, 0x3d, 0x07, 0xac, 0x20, 0x48,
        0x38, 0x5f, 0x71, 0x5a, 0x35, 0x0e, 0xb8, 0xcd,
        0x41, 0xfb, 0xfa, 0x24, 0x21, 0xe8, 0xc7, 0x41,
        0x81, 0xe6, 0xbd, 0xd9, 0x93, 0x68, 0x4d, 0x24,
        0xf9, 0x9b, 0xc2, 0xd9, 0xe5, 0xfa, 0x7e, 0x59,
        0x7f, 0x1d, 0x87, 0x2a, 0x87, 0xd1, 0x43, 0xa8,
        0x3b, 0x6e, 0x80, 0xf7, 0x7b, 0x65, 0xb7, 0x3c,
        0x2b, 0x19, 0x92, 0x0d, 0xd9, 0xf0, 0xab, 0x26,
        0x9f, 0xa9, 0x97, 0x5d, 0x4b, 0xf0, 0x23, 0xd8,
        0xbe, 0x2f, 0xdd, 0x7d, 0x92, 0xa8, 0xd7, 0xaa,
        0x00, 0xc1, 0xa7, 0x26, 0x00, 0x9e, 0xb2, 0x58,
        0xb1, 0xf5, 0xab, 0xb7, 0xfc, 0x1e, 0x56, 0x82,
        0x74, 0x6a, 0x92, 0xb1, 0x81, 0xb6, 0x96, 0x2b,
        0xbe, 0x40, 0x7b, 0x39, 0x9f, 0xb9, 0xfb, 0x3b,
        0x57, 0x9b, 0x78, 0x9a, 0xf9, 0xbc, 0x3f, 0xfd,
        0xdd, 0x24, 0x0f, 0xc0, 0x45, 0x1b, 0xd5, 0xa2,
        0x40, 0x87, 0xba, 0xbd, 0xcf, 0x75, 0x5f, 0xbc,
        0x47, 0xf6, 0x67, 0x48, 0xf6, 0x2c, 0x37, 0xc3,
        0x09, 0xa4, 0xf9, 0x39, 0xad, 0xe6, 0xe7, 0x93,
        0x75, 0x4b, 0xc6, 0x0e, 0xfc, 0x0f, 0xb0, 0xec,
        0xcb, 0xa7, 0x17, 0x6a, 0x7d, 0x55, 0x07, 0xf1,
        0x21, 0xf9, 0xab, 0x90, 0xdf, 0x2a, 0x13, 0xaa,
        0xdf, 0x85, 0x34, 0xec, 0x66, 0x45, 0x31, 0x82,
        0x42, 0x15, 0xd9, 0x8b, 0x69, 0x22, 0x70, 0xca,
        0xdd, 0x73, 0xb2, 0x9e, 0xd2, 0x7d, 0x15, 0x34,
        0x13, 0xf2, 0x4f, 0xfb, 0xa2, 0xdb, 0x18, 0x58,
        0x9e, 0xe6, 0x31, 0x9c, 0x6a, 0x01, 0xa5, 0x33,
        0x0c, 0x27, 0x4f, 0x1d, 0xd3, 0x7a, 0x9c, 0xa3,
        0x3d, 0x93, 0x92, 0x42, 0x17, 0x16, 0x11, 0xec,
        0xe6, 0x89, 0x59, 0x70, 0x86, 0x66, 0xcd, 0x36,
        0x11, 0x6e, 0xf7, 0x2f, 0x03, 0x41, 0xcb, 0x0c,
        0x9b, 0x2c, 0x30, 0xae, 0x84, 0x42, 0xbc, 0xde,
        0xb4, 0xae, 0xbf, 0x3f, 0x95, 0x47, 0x84, 0x80,
        0x62, 0x65, 0xd3, 0xd6, 0xd4, 0xf3, 0xba, 0xaa,
        0xff, 0xc6, 0x8e, 0x8f, 0x76, 0x2d, 0x2c, 0x76,
        0xb6, 0xca, 0x84, 0x25, 0xc2, 0xa0, 0x59, 0xe5,
        0x0a, 0x6c, 0x42, 0x7b, 0x91, 0x39, 0xf5, 0x5a,
        0x4f, 0x2f, 0xc4, 0x17, 0xd5, 0xac, 0x68, 0x1d,
        0x2f, 0x96, 0xfe, 0xa2, 0x0e, 0xef, 0x4f, 0xda,
        0x27, 0xa9, 0x54, 0x6a, 0xd6, 0xbc, 0xfa, 0x1f,
        0x07, 0x77, 0x22, 0xe0, 0x56, 0x13, 0xec, 0xe1,
        0x75, 0x8e, 0x35, 0x1a, 0xce, 0xb7, 0x5e, 0xf8,
        0x6a, 0x85, 0x4f, 0x52, 0x12, 0xac, 0xbc, 0x9e,
        0xb4, 0x72, 0xa7, 0x65, 0x08, 0xbf, 0x27, 0xf4,
        0x72, 0x74, 0x67, 0x55, 0x2b, 0xfe, 0xf1, 0x7d,
        0x9c, 0x2b, 0x2d, 0xc7, 0x0a, 0x3c, 0x24, 0xa1,
        0x78, 0x3b, 0x8a, 0x85, 0xc9
    };
    std::string expected("ffcb3b313e576ea5b0a87449749b82c1523947ad019c633401a"
                         "185fbcb4fd475dfe42289c5413f440d083f73b65abcf2386e0e"
                         "ff39acfafa398a3d6b778fde23");
    SHA512 sha512;

    sha512.SpaceSeparateWords(false); // No spaces in output on this test

    // Provide input to the SHA512 object
    sha512.Input(vector);

    // Finalize the message digest
    sha512.Finalize();

    // Perform equality test
    STF_ASSERT_EQ(expected, sha512.Result());
}

//
// API Tests
//

// Test expected values
STF_TEST(SHA512, TestExpectedValues)
{
    SHA512 sha512("abc");

    STF_ASSERT_EQ(std::string("ddaf35a193617aba cc417349ae204131 "
                              "12e6fa4e89a97ea2 0a9eeee64b55d39a "
                              "2192992a274fc1a8 36ba3c23a3feebbd "
                              "454d4423643ce80e 2a9ac94fa54ca49f"),
                  sha512.Result());

    STF_ASSERT_TRUE(sha512.IsFinalized());

    STF_ASSERT_EQ(std::size_t(128), sha512.GetBlockSize());

    STF_ASSERT_EQ(std::size_t(64), sha512.GetDigestLength());
}

// Test Reset() function
STF_TEST(SHA512, TestReset)
{
    SHA512 sha512("abc");

    STF_ASSERT_EQ(std::string("ddaf35a193617aba cc417349ae204131 "
                              "12e6fa4e89a97ea2 0a9eeee64b55d39a "
                              "2192992a274fc1a8 36ba3c23a3feebbd "
                              "454d4423643ce80e 2a9ac94fa54ca49f"),
                  sha512.Result());

    sha512.Reset();

    STF_ASSERT_FALSE(sha512.IsFinalized());

    STF_ASSERT_FALSE(sha512.IsFinalized());

    STF_ASSERT_EQ((SHA512MessageLength{0, 0}), sha512.GetMessageLength());

    // Verify object reuse works
    sha512.Input("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
    sha512.Finalize();

    STF_ASSERT_EQ(std::string("204a8fc6dda82f0a 0ced7beb8e08a416 "
                              "57c16ef468b228a8 279be331a703c335 "
                              "96fd15c13b1b07f9 aa1d3bea57789ca0 "
                              "31ad85c7a71dd703 54ec631238ca3445"),
                  sha512.Result());
}

// Test streaming operator
STF_TEST(SHA512, TestStreamingOperator)
{
    SHA512 sha512;

    sha512 << "a" << "b" << "c";
    sha512.Finalize();

    STF_ASSERT_EQ((SHA512MessageLength{0, 3}), sha512.GetMessageLength());

    STF_ASSERT_EQ(std::string("ddaf35a193617aba cc417349ae204131 "
                              "12e6fa4e89a97ea2 0a9eeee64b55d39a "
                              "2192992a274fc1a8 36ba3c23a3feebbd "
                              "454d4423643ce80e 2a9ac94fa54ca49f"),
                  sha512.Result());
}

// Test copy constructor
STF_TEST(SHA512, TestCopyConstructor)
{
    SHA512 sha512("abc");

    STF_ASSERT_EQ(std::string("ddaf35a193617aba cc417349ae204131 "
                              "12e6fa4e89a97ea2 0a9eeee64b55d39a "
                              "2192992a274fc1a8 36ba3c23a3feebbd "
                              "454d4423643ce80e 2a9ac94fa54ca49f"),
                  sha512.Result());

    SHA512 sha512_copy = sha512;

    STF_ASSERT_EQ((SHA512MessageLength{0, 3}), sha512_copy.GetMessageLength());

    STF_ASSERT_EQ(sha512, sha512_copy);

    STF_ASSERT_TRUE(sha512_copy.IsFinalized());

    STF_ASSERT_EQ(std::string("ddaf35a193617aba cc417349ae204131 "
                              "12e6fa4e89a97ea2 0a9eeee64b55d39a "
                              "2192992a274fc1a8 36ba3c23a3feebbd "
                              "454d4423643ce80e 2a9ac94fa54ca49f"),
                  sha512_copy.Result());
}

// Test move constructor
STF_TEST(SHA512, TestMoveConstructor)
{
    SHA512 sha512("abc");

    SHA512 sha512_copy = sha512;

    STF_ASSERT_EQ(std::string("ddaf35a193617aba cc417349ae204131 "
                              "12e6fa4e89a97ea2 0a9eeee64b55d39a "
                              "2192992a274fc1a8 36ba3c23a3feebbd "
                              "454d4423643ce80e 2a9ac94fa54ca49f"),
                  sha512.Result());

    SHA512 sha512_move = std::move(sha512_copy);

    STF_ASSERT_EQ(sha512, sha512_move);

    STF_ASSERT_TRUE(sha512_move.IsFinalized());

    STF_ASSERT_EQ(sha512_move.Result(), sha512.Result());
}

// Test equal constructor
STF_TEST(SHA512, TestEqualOperator)
{
    SHA512 sha512("abc");

    STF_ASSERT_EQ(std::string("ddaf35a193617aba cc417349ae204131 "
                              "12e6fa4e89a97ea2 0a9eeee64b55d39a "
                              "2192992a274fc1a8 36ba3c23a3feebbd "
                              "454d4423643ce80e 2a9ac94fa54ca49f"),
                  sha512.Result());

    SHA512 sha512_copy = sha512;

    STF_ASSERT_TRUE(sha512_copy == sha512);
}

// Test not equal constructor
STF_TEST(SHA512, TestNotEqualOperator)
{
    SHA512 sha512("abc");

    STF_ASSERT_EQ(std::string("ddaf35a193617aba cc417349ae204131 "
                              "12e6fa4e89a97ea2 0a9eeee64b55d39a "
                              "2192992a274fc1a8 36ba3c23a3feebbd "
                              "454d4423643ce80e 2a9ac94fa54ca49f"),
                  sha512.Result());

    SHA512 sha512_copy;

    STF_ASSERT_TRUE(sha512_copy != sha512);
}

// Test result array
STF_TEST(SHA512, TestResultArray)
{
    SHA512 sha512("abc");

    SHA512::SHA512ResultOctets result_array;

    sha512.Result(result_array);

    std::uint8_t expected_result_octets[] =
    {
        0xdd, 0xaf, 0x35, 0xa1, 0x93, 0x61, 0x7a, 0xba,
        0xcc, 0x41, 0x73, 0x49, 0xae, 0x20, 0x41, 0x31,
        0x12, 0xe6, 0xfa, 0x4e, 0x89, 0xa9, 0x7e, 0xa2,
        0x0a, 0x9e, 0xee, 0xe6, 0x4b, 0x55, 0xd3, 0x9a,
        0x21, 0x92, 0x99, 0x2a, 0x27, 0x4f, 0xc1, 0xa8,
        0x36, 0xba, 0x3c, 0x23, 0xa3, 0xfe, 0xeb, 0xbd,
        0x45, 0x4d, 0x44, 0x23, 0x64, 0x3c, 0xe8, 0x0e,
        0x2a, 0x9a, 0xc9, 0x4f, 0xa5, 0x4c, 0xa4, 0x9f
    };

    STF_ASSERT_MEM_EQ(expected_result_octets, result_array, 64);
}

// Test result words
STF_TEST(SHA512, TestResultWords)
{
    SHA512 sha512("abc");

    SHA512::SHA512ResultWords result_words;

    sha512.Result(result_words);

    SHA512::SHA512ResultWords expected_result_words =
    {
        0xddaf35a193617aba, 0xcc417349ae204131,
        0x12e6fa4e89a97ea2, 0x0a9eeee64b55d39a,
        0x2192992a274fc1a8, 0x36ba3c23a3feebbd,
        0x454d4423643ce80e, 0x2a9ac94fa54ca49f
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
STF_TEST(SHA512, TestInput)
{
    SHA512 sha512("abc");

    SHA512 sha512_anew;

    sha512_anew.Input("a");
    sha512_anew.Input("b");
    sha512_anew.Input("c");
    sha512_anew.Finalize();

    STF_ASSERT_EQ(sha512, sha512_anew);
}

// Test Auto Finalize
STF_TEST(SHA512, TestAutoFinalize1)
{
    SHA512 sha512("abc");

    SHA512 sha512_auto("abc", true);

    STF_ASSERT_EQ(sha512, sha512_auto);
}

// Test Auto Finalize
STF_TEST(SHA512, TestAutoFinalize2)
{
    SHA512 sha512("abc");

    SHA512 sha512_auto("abc", false);
    sha512_auto.Finalize();

    STF_ASSERT_EQ(sha512, sha512_auto);
}

// Test output stream
STF_TEST(SHA512, TestOutputStream)
{
    SHA512 sha512("abc");

    // Test streaming operator
    std::ostringstream oss;
    oss << sha512;
    std::string test_string = oss.str();

    STF_ASSERT_EQ(std::string("ddaf35a193617aba cc417349ae204131 "
                              "12e6fa4e89a97ea2 0a9eeee64b55d39a "
                              "2192992a274fc1a8 36ba3c23a3feebbd "
                              "454d4423643ce80e 2a9ac94fa54ca49f"),
                  test_string);
}

// Test base class pointer
STF_TEST(SHA512, TestBaseClassPointer)
{
    std::unique_ptr<Hash> hash = std::make_unique<SHA512>("abc");

    STF_ASSERT_EQ(std::string("ddaf35a193617aba cc417349ae204131 "
                              "12e6fa4e89a97ea2 0a9eeee64b55d39a "
                              "2192992a274fc1a8 36ba3c23a3feebbd "
                              "454d4423643ce80e 2a9ac94fa54ca49f"),
                 hash->Result());
}

// Test exception on additional input
STF_TEST(SHA512, TestExceptionOnExtraInput)
{
    SHA512 sha512("abc");

    bool exception_thrown = false;
    try
    {
        sha512.Input("abc");
    }
    catch(const std::exception &)
    {
        exception_thrown = true;
    }

    STF_ASSERT_TRUE(exception_thrown);
}

// Test exception when not finalized
STF_TEST(SHA512, TestExceptionNotFinalized)
{
    SHA512 sha512_exception;

    bool exception_thrown = false;
    try
    {
        // This should throw an exception since the result was not finalized
        std::string result = sha512_exception.Result();
    }
    catch (const HashException &)
    {
        exception_thrown = true;
    }

    STF_ASSERT_TRUE(exception_thrown);
}

// Under debug builds, exclude these slow tests
#ifndef NDEBUG
STF_TEST_EXCLUDE(SHA512, TestLong)
#endif
