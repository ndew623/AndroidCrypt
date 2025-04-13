/*
 *  test_aes_intel.cpp
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will exercise the core AES key expansion, encryption, and
 *      decryption routines in the AESIntel module.
 *
 *  Portability Issues:
 *      None.
 */

#include <cstdint>
#include <ostream>
#include <iomanip>
#include <vector>
#include <bit>
#include <intel_intrinsics.h>
#include <aes_intel.h>
#include <cpu_check.h>
#include <terra/stf/adapters/integral_vector.h>
#include <terra/stf/stf.h>

using namespace Terra::Crypto::Cipher;

#ifdef TERRA_USE_INTEL_INTRINSICS

// Test the function that indicated the engine type
STF_TEST(AESIntel, EngineCheck)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    AESIntel aes;

    STF_ASSERT_EQ(aes.GetEngineType(), AESEngineType::Intel);
}

// Test from Appendix A.1
STF_TEST(AESIntel, TestKeyExpansion128)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[16] =
    {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };

    // Expected values of W
    const std::uint32_t Expected_W[60]
    {
        0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c,
        0xa0fafe17, 0x88542cb1, 0x23a33939, 0x2a6c7605,
        0xf2c295f2, 0x7a96b943, 0x5935807a, 0x7359f67f,
        0x3d80477d, 0x4716fe3e, 0x1e237e44, 0x6d7a883b,
        0xef44a541, 0xa8525b7f, 0xb671253b, 0xdb0bad00,
        0xd4d1c6f8, 0x7c839d87, 0xcaf2b8bc, 0x11f915bc,
        0x6d88a37a, 0x110b3efd, 0xdbf98641, 0xca0093fd,
        0x4e54f70e, 0x5f5fc9f3, 0x84a64fb2, 0x4ea6dc4f,
        0xead27321, 0xb58dbad2, 0x312bf560, 0x7f8d292f,
        0xac7766f3, 0x19fadc21, 0x28d12941, 0x575c006e,
        0xd014f9a8, 0xc9ee2589, 0xe13f0cc8, 0xb6630ca6,

        // Balance of array should have been zero-initialized
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000
    };

    // Expected values of DW
    const std::uint32_t Expected_DW[60]
    {
        0xd014f9a8, 0xc9ee2589, 0xe13f0cc8, 0xb6630ca6,
        0x0c7b5a63, 0x1319eafe, 0xb0398890, 0x664cfbb4,
        0xdf7d925a, 0x1f62b09d, 0xa320626e, 0xd6757324,
        0x12c07647, 0xc01f22c7, 0xbc42d2f3, 0x7555114a,
        0x6efcd876, 0xd2df5480, 0x7c5df034, 0xc917c3b9,
        0x6ea30afc, 0xbc238cf6, 0xae82a4b4, 0xb54a338d,
        0x90884413, 0xd280860a, 0x12a12842, 0x1bc89739,
        0x7c1f13f7, 0x4208c219, 0xc021ae48, 0x0969bf7b,
        0xcc7505eb, 0x3e17d1ee, 0x82296c51, 0xc9481133,
        0x2b3708a7, 0xf262d405, 0xbc3ebdbf, 0x4b617d62,
        0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c,

        // Balance of array should have been zero-initialized
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000
    };

    // Copy of the W and DW vectors produced
    std::uint32_t W_vec[sizeof(Expected_W)]{};
    std::uint32_t DW_vec[sizeof(Expected_DW)]{};

    // Define derived object so we can inspect internals
    class AESIntel_ : public AESIntel
    {
        public:
            AESIntel_(const std::span<const std::uint8_t> key) : AESIntel(key)
            {
                // Nothing to do here
            }

            // Give access to W and DW
            using AESIntel::W;
            using AESIntel::DW;
    };

    // Construct an AES object
    AESIntel_ aes(aes_key);

    // Write the aes.W and aes.DW vectors into memory
    for (std::size_t i = 0; i <= 10; i++)
    {
        _mm_storeu_si128((__m128i *) &W_vec[i * 4], aes.W[i]);
        _mm_storeu_si128((__m128i *) &DW_vec[i * 4], aes.DW[i]);
    }

    // The _mm_storeu_si128() call stores integers such that they appear to
    // be in big endian order; flip the endianness for comparison if on a
    // little endian processor
    if constexpr (std::endian::native == std::endian::little)
    {
        for (std::size_t i = 0; i < sizeof(W_vec) / sizeof(W_vec[0]); i++)
        {
            W_vec[i] = ((W_vec[i] >> 24) & 0x000000ff) |
                       ((W_vec[i] >>  8) & 0x0000ff00) |
                       ((W_vec[i] <<  8) & 0x00ff0000) |
                       ((W_vec[i] << 24) & 0xff000000);
            DW_vec[i] = ((DW_vec[i] >> 24) & 0x000000ff) |
                        ((DW_vec[i] >>  8) & 0x0000ff00) |
                        ((DW_vec[i] <<  8) & 0x00ff0000) |
                        ((DW_vec[i] << 24) & 0xff000000);
        }
    }

    // Verify that the key expansion produced the expected values
    STF_ASSERT_MEM_EQ(Expected_W, W_vec, sizeof(Expected_W));
    STF_ASSERT_MEM_EQ(Expected_DW, DW_vec, sizeof(Expected_DW));
}

// Test from Appendix A.2
STF_TEST(AESIntel, TestKeyExpansion192)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[24] =
    {
        0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
        0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
        0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
    };

    // Expected values of W
    const std::uint32_t Expected_W[60]
    {
        0x8e73b0f7, 0xda0e6452, 0xc810f32b, 0x809079e5,
        0x62f8ead2, 0x522c6b7b, 0xfe0c91f7, 0x2402f5a5,
        0xec12068e, 0x6c827f6b, 0x0e7a95b9, 0x5c56fec2,
        0x4db7b4bd, 0x69b54118, 0x85a74796, 0xe92538fd,
        0xe75fad44, 0xbb095386, 0x485af057, 0x21efb14f,
        0xa448f6d9, 0x4d6dce24, 0xaa326360, 0x113b30e6,
        0xa25e7ed5, 0x83b1cf9a, 0x27f93943, 0x6a94f767,
        0xc0a69407, 0xd19da4e1, 0xec1786eb, 0x6fa64971,
        0x485f7032, 0x22cb8755, 0xe26d1352, 0x33f0b7b3,
        0x40beeb28, 0x2f18a259, 0x6747d26b, 0x458c553e,
        0xa7e1466c, 0x9411f1df, 0x821f750a, 0xad07d753,
        0xca400538, 0x8fcc5006, 0x282d166a, 0xbc3ce7b5,
        0xe98ba06f, 0x448c773c, 0x8ecc7204, 0x01002202,

        // Balance of array should have been zero-initialized
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000
    };

    // Expected values of DW
    const std::uint32_t Expected_DW[60]
    {
        0xe98ba06f, 0x448c773c, 0x8ecc7204, 0x01002202,
        0xac491644, 0xe55710b7, 0x46c08a75, 0xc89b2cad,
        0xa3979ac2, 0x8e5ba6d8, 0xe12cc9e6, 0x54b272ba,
        0xf3b42258, 0xb59ebb5c, 0xf8fb64fe, 0x491e06f3,
        0x4d65dfa2, 0xb1e5620d, 0xea899c31, 0x2dcc3c1a,
        0x5b6cfe3c, 0xc745a02b, 0xf8b9a572, 0x462a9904,
        0xc5ddb7f8, 0xbe933c76, 0x0b4f46a6, 0xfc80bdaf,
        0xb5dc7ad0, 0xf7cffb09, 0xa7ec4393, 0x9c295e17,
        0x5023b89a, 0x3bc51d84, 0xd04b1937, 0x7b4e8b8e,
        0x41b34544, 0xab0592b9, 0xce92f15e, 0x421381d9,
        0x659763e7, 0x8c817087, 0x12303943, 0x6be6a51e,
        0x9eb149c4, 0x79d69c5d, 0xfeb4a27c, 0xeab6d7fd,
        0x8e73b0f7, 0xda0e6452, 0xc810f32b, 0x809079e5,

        // Balance of array should have been zero-initialized
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000
    };

    // Copy of the W and DW vectors produced
    std::uint32_t W_vec[sizeof(Expected_W)]{};
    std::uint32_t DW_vec[sizeof(Expected_DW)]{};

    // Define derived object so we can inspect internals
    class AESIntel_ : public AESIntel
    {
        public:
            AESIntel_(const std::span<const std::uint8_t> key) : AESIntel(key)
            {
                // Nothing to do here
            }

            // Give access to W and DW
            using AESIntel::W;
            using AESIntel::DW;
    };

    // Construct an AES object
    AESIntel_ aes(aes_key);

    // Write the aes.W and aes.DW vectors into memory
    for (std::size_t i = 0; i <= 12; i++)
    {
        _mm_storeu_si128((__m128i *) &W_vec[i * 4], aes.W[i]);
        _mm_storeu_si128((__m128i *) &DW_vec[i * 4], aes.DW[i]);
    }

    // The _mm_storeu_si128() call stores integers such that they appear to
    // be in big endian order; flip the endianness for comparison if on a
    // little endian processor
    if constexpr (std::endian::native == std::endian::little)
    {
        for (std::size_t i = 0; i < sizeof(W_vec) / sizeof(W_vec[0]); i++)
        {
            W_vec[i] = ((W_vec[i] >> 24) & 0x000000ff) |
                       ((W_vec[i] >>  8) & 0x0000ff00) |
                       ((W_vec[i] <<  8) & 0x00ff0000) |
                       ((W_vec[i] << 24) & 0xff000000);
            DW_vec[i] = ((DW_vec[i] >> 24) & 0x000000ff) |
                        ((DW_vec[i] >>  8) & 0x0000ff00) |
                        ((DW_vec[i] <<  8) & 0x00ff0000) |
                        ((DW_vec[i] << 24) & 0xff000000);
        }
    }

    // Verify that the key expansion produced the expected values
    STF_ASSERT_MEM_EQ(Expected_W, W_vec, sizeof(Expected_W));
    STF_ASSERT_MEM_EQ(Expected_DW, DW_vec, sizeof(Expected_DW));
}

// Test from Appendix A.3
STF_TEST(AESIntel, TestKeyExpansion256)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[32] =
    {
        0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
        0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
        0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
        0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
    };

    // Expected values of W
    const std::uint32_t Expected_W[60]
    {
        0x603deb10, 0x15ca71be, 0x2b73aef0, 0x857d7781,
        0x1f352c07, 0x3b6108d7, 0x2d9810a3, 0x0914dff4,
        0x9ba35411, 0x8e6925af, 0xa51a8b5f, 0x2067fcde,
        0xa8b09c1a, 0x93d194cd, 0xbe49846e, 0xb75d5b9a,
        0xd59aecb8, 0x5bf3c917, 0xfee94248, 0xde8ebe96,
        0xb5a9328a, 0x2678a647, 0x98312229, 0x2f6c79b3,
        0x812c81ad, 0xdadf48ba, 0x24360af2, 0xfab8b464,
        0x98c5bfc9, 0xbebd198e, 0x268c3ba7, 0x09e04214,
        0x68007bac, 0xb2df3316, 0x96e939e4, 0x6c518d80,
        0xc814e204, 0x76a9fb8a, 0x5025c02d, 0x59c58239,
        0xde136967, 0x6ccc5a71, 0xfa256395, 0x9674ee15,
        0x5886ca5d, 0x2e2f31d7, 0x7e0af1fa, 0x27cf73c3,
        0x749c47ab, 0x18501dda, 0xe2757e4f, 0x7401905a,
        0xcafaaae3, 0xe4d59b34, 0x9adf6ace, 0xbd10190d,
        0xfe4890d1, 0xe6188d0b, 0x046df344, 0x706c631e
    };

    // Expected values of DW
    const std::uint32_t Expected_DW[60]
    {
        0xfe4890d1, 0xe6188d0b, 0x046df344, 0x706c631e,
        0xada23f49, 0x63e23b24, 0x55427c8a, 0x5c709104,
        0x57c96cf6, 0x074f07c0, 0x706abb07, 0x137f9241,
        0xb668b621, 0xce40046d, 0x36a047ae, 0x0932ed8e,
        0x34ad1e44, 0x50866b36, 0x7725bcc7, 0x63152946,
        0x32526c36, 0x7828b24c, 0xf8e043c3, 0x3f92aa20,
        0xc440b289, 0x642b7572, 0x27a3d7f1, 0x14309581,
        0xd669a733, 0x4a7ade7a, 0x80c8f18f, 0xc772e9e3,
        0x25ba3c22, 0xa06bc7fb, 0x4388a283, 0x33934270,
        0x54fb808b, 0x9c137949, 0xcab22ff5, 0x47ba186c,
        0x6c3d6329, 0x85d1fbd9, 0xe3e36578, 0x701be0f3,
        0x4a7459f9, 0xc8e8f9c2, 0x56a156bc, 0x8d083799,
        0x42107758, 0xe9ec98f0, 0x66329ea1, 0x93f8858b,
        0x8ec6bff6, 0x829ca03b, 0x9e49af7e, 0xdba96125,
        0x603deb10, 0x15ca71be, 0x2b73aef0, 0x857d7781
    };

    // Copy of the W and DW vectors produced
    std::uint32_t W_vec[sizeof(Expected_W)]{};
    std::uint32_t DW_vec[sizeof(Expected_DW)]{};

    // Define derived object so we can inspect internals
    class AESIntel_ : public AESIntel
    {
        public:
            AESIntel_(const std::span<const std::uint8_t> key) : AESIntel(key)
            {
                // Nothing to do here
            }

            // Give access to W and DW
            using AESIntel::W;
            using AESIntel::DW;
    };

    // Construct an AES object
    AESIntel_ aes(aes_key);

    // Write the aes.W vector into memory
    for (std::size_t i = 0; i <= 14; i++)
    {
        _mm_storeu_si128((__m128i *) &W_vec[i * 4], aes.W[i]);
        _mm_storeu_si128((__m128i *) &DW_vec[i * 4], aes.DW[i]);
    }

    // The _mm_storeu_si128() call stores integers such that they appear to
    // be in big endian order; flip the endianness for comparison if on a
    // little endian processor
    if constexpr (std::endian::native == std::endian::little)
    {
        for (std::size_t i = 0; i < sizeof(W_vec) / sizeof(W_vec[0]); i++)
        {
            W_vec[i] = ((W_vec[i] >> 24) & 0x000000ff) |
                       ((W_vec[i] >>  8) & 0x0000ff00) |
                       ((W_vec[i] <<  8) & 0x00ff0000) |
                       ((W_vec[i] << 24) & 0xff000000);
            DW_vec[i] = ((DW_vec[i] >> 24) & 0x000000ff) |
                        ((DW_vec[i] >>  8) & 0x0000ff00) |
                        ((DW_vec[i] <<  8) & 0x00ff0000) |
                        ((DW_vec[i] << 24) & 0xff000000);
        }
    }

    // Verify that the key expansion produced the expected values
    STF_ASSERT_MEM_EQ(Expected_W, W_vec, sizeof(Expected_W));
    STF_ASSERT_MEM_EQ(Expected_DW, DW_vec, sizeof(Expected_DW));
}

// Test from Appendix B
STF_TEST(AESIntel, TestVectorBEncrypt128)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[16] =
    {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };

    const std::uint8_t plaintext[16] =
    {
        0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
        0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34
    };

    const std::uint8_t expected_ciphertext[16] =
    {
        0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09, 0xfb,
        0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32
    };

    std::uint8_t ciphertext[16];

    AESIntel aes(aes_key);

    aes.Encrypt(plaintext, ciphertext);

    STF_ASSERT_MEM_EQ(expected_ciphertext,
                      ciphertext,
                      sizeof(expected_ciphertext));
}

// Test from Appendix C.1 (encryption)
STF_TEST(AESIntel, TestVectorC1Encrypt128)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[16] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    const std::uint8_t plaintext[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    const std::uint8_t expected_ciphertext[16] =
    {
        0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
        0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a
    };

    std::uint8_t ciphertext[16];

    AESIntel aes(aes_key);

    aes.Encrypt(plaintext, ciphertext);

    STF_ASSERT_MEM_EQ(expected_ciphertext,
                      ciphertext,
                      sizeof(expected_ciphertext));
}

// Test from Appendix C.2 (encryption)
STF_TEST(AESIntel, TestVectorC2Encrypt192)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[24] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
    };

    const std::uint8_t plaintext[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    const std::uint8_t expected_ciphertext[16] =
    {
        0xdd, 0xa9, 0x7c, 0xa4, 0x86, 0x4c, 0xdf, 0xe0,
        0x6e, 0xaf, 0x70, 0xa0, 0xec, 0x0d, 0x71, 0x91
    };

    std::uint8_t ciphertext[16];

    AESIntel aes(aes_key);

    aes.Encrypt(plaintext, ciphertext);

    STF_ASSERT_MEM_EQ(expected_ciphertext,
                      ciphertext,
                      sizeof(expected_ciphertext));
}

// Test from Appendix C.3 (encryption)
STF_TEST(AESIntel, TestVectorC3Encrypt256)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    const std::uint8_t plaintext[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    const std::uint8_t expected_ciphertext[16] =
    {
        0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
        0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89
    };

    std::uint8_t ciphertext[16];

    AESIntel aes(aes_key);

    aes.Encrypt(plaintext, ciphertext);

    STF_ASSERT_MEM_EQ(expected_ciphertext,
                      ciphertext,
                      sizeof(expected_ciphertext));
}

// Test from Appendix B
STF_TEST(AESIntel, TestVectorBDecrypt128)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[16] =
    {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };

    const std::uint8_t ciphertext[16] =
    {
        0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09, 0xfb,
        0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32
    };

    const std::uint8_t expected_plaintext[16] =
    {
        0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
        0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34
    };

    std::uint8_t plaintext[16];

    AESIntel aes(aes_key);

    aes.Decrypt(ciphertext, plaintext);

    STF_ASSERT_MEM_EQ(expected_plaintext,
                      plaintext,
                      sizeof(expected_plaintext));
}

// Test from Appendix C.1 (decryption)
STF_TEST(AESIntel, TestVectorC1Decrypt128)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[16] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    const std::uint8_t ciphertext[16] =
    {
        0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
        0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a
    };

    const std::uint8_t expected_plaintext[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    std::uint8_t plaintext[16];

    AESIntel aes(aes_key);

    aes.Decrypt(ciphertext, plaintext);

    STF_ASSERT_MEM_EQ(expected_plaintext,
                      plaintext,
                      sizeof(expected_plaintext));
}

// Test from Appendix C.2 (decryption)
STF_TEST(AESIntel, TestVectorC2Decrypt192)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[24] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
    };

    const std::uint8_t ciphertext[16] =
    {
        0xdd, 0xa9, 0x7c, 0xa4, 0x86, 0x4c, 0xdf, 0xe0,
        0x6e, 0xaf, 0x70, 0xa0, 0xec, 0x0d, 0x71, 0x91
    };

    const std::uint8_t expected_plaintext[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    std::uint8_t plaintext[16];

    AESIntel aes(aes_key);

    aes.Decrypt(ciphertext, plaintext);

    STF_ASSERT_MEM_EQ(expected_plaintext,
                      plaintext,
                      sizeof(expected_plaintext));
}

// Test from Appendix C.3 (decryption)
STF_TEST(AESIntel, TestVectorC3Decrypt256)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    const std::uint8_t ciphertext[16] =
    {
        0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
        0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89
    };

    const std::uint8_t expected_plaintext[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    std::uint8_t plaintext[16];

    AESIntel aes(aes_key);

    aes.Decrypt(ciphertext, plaintext);

    STF_ASSERT_MEM_EQ(expected_plaintext,
                      plaintext,
                      sizeof(expected_plaintext));
}

// Test copy constructor
STF_TEST(AESIntel, TestCopyConstructor)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    AESIntel aes1;

    const std::uint8_t aes_key[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    const std::uint8_t plaintext[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    const std::uint8_t expected_ciphertext[16] =
    {
        0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
        0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89
    };

    std::uint8_t ciphertext[16];

    aes1.SetKey(aes_key);

    // aes2 will copy key information from aes1
    AESIntel aes2 = aes1;

    aes2.Encrypt(plaintext, ciphertext);

    STF_ASSERT_MEM_EQ(expected_ciphertext,
                      ciphertext,
                      sizeof(expected_ciphertext));
}

// Test move constructor
STF_TEST(AESIntel, TestMoveConstructor)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    AESIntel aes1;

    const std::uint8_t aes_key[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    const std::uint8_t plaintext[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    const std::uint8_t expected_ciphertext[16] =
    {
        0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
        0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89
    };

    std::uint8_t ciphertext[16];

    aes1.SetKey(aes_key);

    // aes2 will copy key information from aes1
    AESIntel aes2(std::move(aes1));

    aes2.Encrypt(plaintext, ciphertext);

    STF_ASSERT_MEM_EQ(expected_ciphertext,
                      ciphertext,
                      sizeof(expected_ciphertext));
}

// Test assignment
STF_TEST(AESIntel, TestAssignment)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    AESIntel aes1, aes2;

    const std::uint8_t aes_key[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    const std::uint8_t plaintext[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    const std::uint8_t expected_ciphertext[16] =
    {
        0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
        0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89
    };

    std::uint8_t ciphertext[16];

    aes1.SetKey(aes_key);

    aes2 = aes1;

    aes2.Encrypt(plaintext, ciphertext);

    STF_ASSERT_MEM_EQ(expected_ciphertext,
                      ciphertext,
                      sizeof(expected_ciphertext));
}

// Test equality
STF_TEST(AESIntel, TestAESEquality)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    AESIntel aes1, aes2;

    const std::uint8_t aes_key[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    aes1.SetKey(aes_key);

    aes2 = aes1;

    STF_ASSERT_TRUE(aes2 == aes1);
}

// Test inequality
STF_TEST(AESIntel, TestAESInequality)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    AESIntel aes1, aes2;

    const std::uint8_t aes_key1[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    const std::uint8_t aes_key2[32] =
    {
        0x99, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    aes1.SetKey(aes_key1);
    aes2.SetKey(aes_key2);

    STF_ASSERT_TRUE(aes2 != aes1);
}

// Test the set key function
STF_TEST(AESIntel, TestAESSetKey)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    AESIntel aes1, aes2;

    const std::uint8_t aes_key1[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    const std::vector<std::uint8_t> aes_key2 =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    const std::vector<std::uint8_t> null_key;

    bool expected_failure = false;

    aes1.SetKey(aes_key1);
    aes2.SetKey(aes_key2);

    STF_ASSERT_TRUE(aes1 == aes2);

    // Check for invalid keys
    try
    {
        aes1.SetKey(null_key);
    }
    catch (const AESException &)
    {
        expected_failure = true;
    }

    STF_ASSERT_TRUE(expected_failure);
}

// Test encrypt vector API
STF_TEST(AESIntel, TestEncryptVectorAPI)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::vector<std::uint8_t> aes_key =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    const std::vector<std::uint8_t> plaintext =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    const std::vector<std::uint8_t> expected_ciphertext =
    {
        0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
        0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89
    };

    std::vector<std::uint8_t> ciphertext(16);

    AESIntel aes(aes_key);

    aes.Encrypt(std::span<const std::uint8_t, 16>(plaintext.data(), 16),
                std::span<std::uint8_t, 16>(ciphertext.data(), 16));

    // The following requires the streaming operator definition
    // at the top of this file
    STF_ASSERT_EQ(expected_ciphertext, ciphertext);
}

// Test decrypt vector API
STF_TEST(AESIntel, TestDecryptVectorAPI)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::vector<std::uint8_t> aes_key =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    const std::vector<std::uint8_t> ciphertext =
    {
        0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
        0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89
    };

    const std::vector<std::uint8_t> expected_plaintext =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    std::vector<std::uint8_t> plaintext(16);

    AESIntel aes(aes_key);

    aes.Decrypt(std::span<const std::uint8_t, 16>(ciphertext.data(), 16),
                std::span<std::uint8_t, 16>(plaintext.data(), 16));

    // The following requires the streaming operator definition
    // at the top of this file
    STF_ASSERT_EQ(expected_plaintext, plaintext);
}

// This function tests the performance of the encryption code
STF_TEST(AESIntel, EncryptionSpeedTest128)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[16] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    std::uint8_t plaintext[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    const std::uint8_t expected_ciphertext[16] =
    {
        0xa3, 0x19, 0xcf, 0x4b, 0xfe, 0xe5, 0x40, 0x20,
        0x54, 0x83, 0x34, 0x2b, 0x51, 0xca, 0xa7, 0xbd
    };

    AESIntel aes(aes_key);

    // Encrypt in place repeatedly
    for (std::uint64_t i = 0; i < 200'000'000; i++)
    {
        aes.Encrypt(plaintext, plaintext);
    }

    STF_ASSERT_MEM_EQ(expected_ciphertext,
                      plaintext,
                      sizeof(expected_ciphertext));
}

// This function tests the performance of the encryption code
STF_TEST(AESIntel, EncryptionSpeedTest256)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    std::uint8_t plaintext[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    const std::uint8_t expected_ciphertext[16] =
    {
        0x2c, 0x8f, 0x82, 0x94, 0x60, 0x3a, 0x9c, 0xcb,
        0xd3, 0xd0, 0x3b, 0x05, 0xea, 0x5a, 0x89, 0xc1
    };

    AESIntel aes(aes_key);

    // Encrypt in place repeatedly
    for (std::uint64_t i = 0; i < 200'000'000; i++)
    {
        aes.Encrypt(plaintext, plaintext);
    }

    STF_ASSERT_MEM_EQ(expected_ciphertext,
                      plaintext,
                      sizeof(expected_ciphertext));
}

// This function tests the performance of the decryption code
STF_TEST(AESIntel, DecryptionSpeedTest128)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[16] =
    {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };

    std::uint8_t ciphertext[16] =
    {
        0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09, 0xfb,
        0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32
    };

    const std::uint8_t expected_plaintext[16] =
    {
        0xc8, 0x2c, 0x2b, 0x8a, 0x94, 0x4f, 0x8b, 0xc5,
        0x93, 0x29, 0xc2, 0x8c, 0xd7, 0x8e, 0x7b, 0x71
    };

    AESIntel aes(aes_key);

    // Decrypt in place repeatedly
    for (std::uint64_t i = 0; i < 200'000'000; i++)
    {
        aes.Decrypt(ciphertext, ciphertext);
    }

    STF_ASSERT_MEM_EQ(expected_plaintext,
                      ciphertext,
                      sizeof(expected_plaintext));
}

// This function tests the performance of the decryption code
STF_TEST(AESIntel, DecryptionSpeedTest256)
{
    if (!CPUSupportsAES_NI())
    {
        std::cerr << "AES-NI is not supported on this processor" << std::endl;
        return;
    }

    const std::uint8_t aes_key[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    std::uint8_t ciphertext[16] =
    {
        0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
        0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89
    };

    const std::uint8_t expected_plaintext[16] =
    {
        0xe8, 0x33, 0x93, 0x98, 0x27, 0xba, 0xdb, 0x29,
        0xd5, 0xc1, 0xe0, 0x29, 0x2d, 0x0f, 0xd0, 0x0d
    };

    AESIntel aes(aes_key);

    // Decrypt in place repeatedly
    for (std::uint64_t i = 0; i < 200'000'000; i++)
    {
        aes.Decrypt(ciphertext, ciphertext);
    }

    STF_ASSERT_MEM_EQ(expected_plaintext,
                      ciphertext,
                      sizeof(expected_plaintext));
}

#ifndef TERRA_ENABLE_AES_SPEED_TESTS
STF_TEST_EXCLUDE(AESIntel, EncryptionSpeedTest128);
STF_TEST_EXCLUDE(AESIntel, EncryptionSpeedTest256);
STF_TEST_EXCLUDE(AESIntel, DecryptionSpeedTest128);
STF_TEST_EXCLUDE(AESIntel, DecryptionSpeedTest256);
#endif // TERRA_ENABLE_AES_SPEED_TESTS

#else

// If not using Intel Intrinsics, the Intel engine should be unavailable
STF_TEST(AESIntel, EngineCheck)
{
    AESIntel aes;

    STF_ASSERT_EQ(aes.GetEngineType(), AESEngineType::Unavailable);
}

#endif // TERRA_USE_INTEL_INTRINSICS
