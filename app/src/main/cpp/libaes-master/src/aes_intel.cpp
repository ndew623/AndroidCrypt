/*
 *  aes_intel.cpp
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the AESIntel object which performs encryption and
 *      decryption as specified in FIPS 197 ("Advanced Encryption Standard")
 *      using Intel Intrinsics functions.
 *
 *      Reference implementation code:
 *      https://www.intel.com/content/dam/doc/white-paper/advanced-encryption-standard-new-instructions-set-paper.pdf
 *
 *  Portability Issues:
 *      None.
 */

#include "intel_intrinsics.h"

// Do not attempt to compile unless told to use Intel Intrinsics
#ifdef TERRA_USE_INTEL_INTRINSICS

#include <cstring>
#include <terra/secutil/secure_erase.h>
#include "aes_intel.h"
#include "aes_tables.h"

namespace Terra::Crypto::Cipher
{

/*
 * AESIntel::AESIntel()
 *
 *  Description:
 *      This is a constructor for the AESIntel object with no given key.
 *      It will initialize internal structures to zero.  Since a key is not
 *      provided to this version of the constructor, one must call
 *      SetKey() with a valid key before calling Encrypt() or Decrypt(),
 *      as the results will otherwise be invalid.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
AESIntel::AESIntel() noexcept :
    AESEngine(),
    Nr{},
    W{},
    DW{},
    T1{},
    T2{},
    T3{},
    T4{}
{
    // Nothing to do
}

/*
 * AESIntel::AESIntel()
 *
 *  Description:
 *      This is a constructor for the AESIntel object that accepts a span
 *      of octets as input that contains the key.
 *
 *  Parameters:
 *      key [in]
 *          The encryption key to use with this instance of the object.
 *
 *  Returns:
 *      Nothing, though an exception will be thrown if the key provided is not
 *      one of 16, 24, or 32 octets in length as required by the standard.
 *      This corresponds to 128, 192, and 256 bits.
 *
 *  Comments:
 *      None.
 */
AESIntel::AESIntel(const std::span<const std::uint8_t> key) :
    AESIntel()
{
    // Only set the key if AES-NI instructions are supported
    if (GetEngineType() == AESEngineType::Intel) SetKey(key);
}

/*
 * AESIntel::AESIntel()
 *
 *  Description:
 *      This is a copy constructor for the AESIntel object that accepts a
 *      reference to another AESIntel object as a parameter.
 *
 *  Parameters:
 *      other [in]
 *          The other AESIntel object from which to copy values.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
AESIntel::AESIntel(const AESIntel &other) noexcept : AESIntel()
{
    Nr = other.Nr;
    std::memcpy(W, other.W, sizeof(W));
    std::memcpy(DW, other.DW, sizeof(DW));
}

/*
 * AESIntel::AESIntel()
 *
 *  Description:
 *      This is a move constructor for the AESIntel object that accepts a
 *      reference to another AESIntel object as a parameter.  Note that
 *      this move operation actually just performs a copy of data, since there
 *      is no dynamic data to really move.
 *
 *  Parameters:
 *      other [in]
 *          The other AESIntel object from which to move values.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      This function just copies the other AESIntel object's values since
 *      there is no internal data that can be moved.
 */
AESIntel::AESIntel(AESIntel &&other) noexcept : AESIntel()
{
    Nr = other.Nr;
    std::memcpy(W, other.W, sizeof(W));
    std::memcpy(DW, other.DW, sizeof(DW));
}

/*
 * AESIntel::~AESIntel()
 *
 *  Description:
 *      This is the destructor for the AESIntel object and is responsible
 *      for zeroing memory to ensure a clean termination with no residue.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
AESIntel::~AESIntel()
{
    SecUtil::SecureErase(&Nr, sizeof(Nr));
    SecUtil::SecureErase(W, sizeof(W));
    SecUtil::SecureErase(DW, sizeof(DW));
    SecUtil::SecureErase(&T1, sizeof(T1));
    SecUtil::SecureErase(&T2, sizeof(T2));
    SecUtil::SecureErase(&T3, sizeof(T3));
    SecUtil::SecureErase(&T4, sizeof(T4));
}

/*
 * AESIntel::operator=()
 *
 *  Description:
 *      Assign one AESIntel object to another.
 *
 *  Parameters:
 *      other [in]
 *          The other AESIntel from which to copy data.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
AESIntel &AESIntel::operator=(const AESIntel &other)
{
    // If this is the same object, just return this
    if (this == &other) return *this;

    Nr = other.Nr;
    std::memcpy(W, other.W, sizeof(W));
    std::memcpy(DW, other.DW, sizeof(DW));

    return *this;
}

/*
 * AESIntel::operator=()
 *
 *  Description:
 *      Move assignment operator to assign another AESIntel object to this
 *      one.
 *
 *  Parameters:
 *      other [in]
 *          The other AESIntel from which to move data.
 *
 *  Returns:
 *      A reference to this AESIntel object.
 *
 *  Comments:
 *      None.
 */
AESIntel &AESIntel::operator=(AESIntel &&other) noexcept
{
    // If this is the same object, just return this
    if (this == &other) return *this;

    Nr = other.Nr;
    std::memcpy(W, other.W, sizeof(W));
    std::memcpy(DW, other.DW, sizeof(DW));

    return *this;
}

/*
 * AESIntel::SetKey()
 *
 *  Description:
 *      This function will set the key to be used for subsequent calls to
 *      Encrypt() or Decrypt().
 *
 *  Parameters:
 *      key [in]
 *          The encryption key to use with this instance of the object.
 *
 *  Returns:
 *      Nothing, though an exception will be thrown if the key provided is not
 *      one of 16, 24, or 32 octets in length as required by the standard.
 *
 *  Comments:
 *      None.
 */
void AESIntel::SetKey(const std::span<const std::uint8_t> key)
{
    // Zero the key schedule
    SecUtil::SecureErase(W, sizeof(W));
    SecUtil::SecureErase(DW, sizeof(DW));

    // Create the encryption round keys given the key length (W)
    switch (key.size())
    {
        case 16:
            Nr = 10;

            // Fill the first four 32-bit words in the round key array W
            W[0] =
                _mm_loadu_si128(reinterpret_cast<const __m128i *>(key.data()));

            {
                auto KeyAssist = [&](std::size_t i, __m128i round_key)
                {
                    W[i] =
                        _mm_shuffle_epi32(round_key, _MM_SHUFFLE(3, 3, 3, 3));
                    T1 = _mm_xor_si128(W[i - 1], _mm_slli_si128(W[i - 1], 4));
                    T1 = _mm_xor_si128(T1, _mm_slli_si128(T1, 4));
                    T1 = _mm_xor_si128(T1, _mm_slli_si128(T1, 4));
                    W[i] = _mm_xor_si128(T1, W[i]);
                };

                // Complete the round key schedule W
                KeyAssist( 1, _mm_aeskeygenassist_si128(W[0], 0x01));
                KeyAssist( 2, _mm_aeskeygenassist_si128(W[1], 0x02));
                KeyAssist( 3, _mm_aeskeygenassist_si128(W[2], 0x04));
                KeyAssist( 4, _mm_aeskeygenassist_si128(W[3], 0x08));
                KeyAssist( 5, _mm_aeskeygenassist_si128(W[4], 0x10));
                KeyAssist( 6, _mm_aeskeygenassist_si128(W[5], 0x20));
                KeyAssist( 7, _mm_aeskeygenassist_si128(W[6], 0x40));
                KeyAssist( 8, _mm_aeskeygenassist_si128(W[7], 0x80));
                KeyAssist( 9, _mm_aeskeygenassist_si128(W[8], 0x1B));
                KeyAssist(10, _mm_aeskeygenassist_si128(W[9], 0x36));
            }

            break;

        case 24:
            Nr = 12;

            // Fill the first six 32-bit words in the round key array W
            W[0] =
                _mm_loadu_si128(reinterpret_cast<const __m128i *>(key.data()));

            // Read 16 more octets, starting at +8 (the high-order bits are
            // part of what was read in W[0]) to avoid overrun
            W[1] = _mm_loadu_si128(
                            reinterpret_cast<const __m128i *>(key.data() + 8));

            // Shift right to remove the octets in W[1] that are part of W[0]
            W[1] = _mm_srli_si128(W[1], 8);

            {
                auto KeyAssist = [&](std::size_t i, bool alt, __m128i round_key)
                {
                    T3 = _mm_slli_si128(T1, 4);
                    T1 = _mm_xor_si128(T1, T3);
                    T3 = _mm_slli_si128(T3, 4);
                    T1 = _mm_xor_si128(T1, T3);
                    T3 = _mm_slli_si128(T3, 4);
                    T1 = _mm_xor_si128(T1, T3);
                    T1 = _mm_xor_si128(
                        T1,
                        _mm_shuffle_epi32(round_key, _MM_SHUFFLE(1, 1, 1, 1)));
                    T4 = _mm_shuffle_epi32(T1, _MM_SHUFFLE(3, 3, 3, 3));
                    T3 = _mm_slli_si128(T2, 4);
                    T2 = _mm_xor_si128(T2, T3);
                    T2 = _mm_xor_si128(T2, T4);
                    if (alt)
                    {
                        W[i] = _mm_or_si128(
                                    _mm_srli_si128(_mm_slli_si128(W[i], 8), 8),
                                    _mm_slli_si128(T1, 8));

                        W[i + 1] = _mm_or_si128(_mm_srli_si128(T1, 8),
                                                _mm_slli_si128(T2, 8));
                    }
                    else
                    {
                        W[i] = T1;
                        if (i < 12) W[i + 1] = T2;
                    }
                };

                // Complete the round key schedule W
                T1 = W[0];
                T2 = W[1];
                KeyAssist( 1, true, _mm_aeskeygenassist_si128(T2, 0x01));
                KeyAssist( 3, false, _mm_aeskeygenassist_si128(T2, 0x02));
                KeyAssist( 4, true, _mm_aeskeygenassist_si128(T2, 0x04));
                KeyAssist( 6, false, _mm_aeskeygenassist_si128(T2, 0x08));
                KeyAssist( 7, true, _mm_aeskeygenassist_si128(T2, 0x10));
                KeyAssist( 9, false, _mm_aeskeygenassist_si128(T2, 0x20));
                KeyAssist(10, true, _mm_aeskeygenassist_si128(T2, 0x40));
                KeyAssist(12, false, _mm_aeskeygenassist_si128(T2, 0x80));
            }

            break;

        case 32:
            Nr = 14;

            // Fill the first eight 32-bit words in the round key array W
            W[0] =
                _mm_loadu_si128(reinterpret_cast<const __m128i *>(key.data()));

            // Read 16 more octets, starting at +8 (the high-order bits are
            // part of what was read in W[0]) to avoid overrun
            W[1] = _mm_loadu_si128(
                            reinterpret_cast<const __m128i *>(key.data() + 16));

            {
                auto KeyAssist = [&](std::size_t i, __m128i round_key)
                {
                    T1 = _mm_slli_si128(W[i - 2], 4);
                    T2 = _mm_xor_si128(W[i - 2], T1);
                    T1 = _mm_slli_si128(T1, 4);
                    T2 = _mm_xor_si128(T2, T1);
                    T1 = _mm_slli_si128(T1, 4);
                    T2 = _mm_xor_si128(T2, T1);
                    if ((i & 0x01) != 0)
                    {
                        W[i] = _mm_xor_si128(
                            T2,
                            _mm_shuffle_epi32(round_key,
                                              _MM_SHUFFLE(2, 2, 2, 2)));
                    }
                    else
                    {
                        W[i] = _mm_xor_si128(
                            T2,
                            _mm_shuffle_epi32(round_key,
                                              _MM_SHUFFLE(3, 3, 3, 3)));
                    }
                };

                // Complete the round key schedule W
                KeyAssist( 2, _mm_aeskeygenassist_si128(W[ 1], 0x01));
                KeyAssist( 3, _mm_aeskeygenassist_si128(W[ 2], 0x00));
                KeyAssist( 4, _mm_aeskeygenassist_si128(W[ 3], 0x02));
                KeyAssist( 5, _mm_aeskeygenassist_si128(W[ 4], 0x00));
                KeyAssist( 6, _mm_aeskeygenassist_si128(W[ 5], 0x04));
                KeyAssist( 7, _mm_aeskeygenassist_si128(W[ 6], 0x00));
                KeyAssist( 8, _mm_aeskeygenassist_si128(W[ 7], 0x08));
                KeyAssist( 9, _mm_aeskeygenassist_si128(W[ 8], 0x00));
                KeyAssist(10, _mm_aeskeygenassist_si128(W[ 9], 0x10));
                KeyAssist(11, _mm_aeskeygenassist_si128(W[10], 0x00));
                KeyAssist(12, _mm_aeskeygenassist_si128(W[11], 0x20));
                KeyAssist(13, _mm_aeskeygenassist_si128(W[12], 0x00));
                KeyAssist(14, _mm_aeskeygenassist_si128(W[13], 0x40));
            }

            break;

        default:
            throw AESException("Invalid key length provided");

            break;
    }

    // Populate decryption round key array (DW)
    DW[Nr] = W[0];
    DW[Nr - 1] = _mm_aesimc_si128(W[1]);
    DW[Nr - 2] = _mm_aesimc_si128(W[2]);
    DW[Nr - 3] = _mm_aesimc_si128(W[3]);
    DW[Nr - 4] = _mm_aesimc_si128(W[4]);
    DW[Nr - 5] = _mm_aesimc_si128(W[5]);
    DW[Nr - 6] = _mm_aesimc_si128(W[6]);
    DW[Nr - 7] = _mm_aesimc_si128(W[7]);
    DW[Nr - 8] = _mm_aesimc_si128(W[8]);
    DW[Nr - 9] = _mm_aesimc_si128(W[9]);
    if (Nr > 10)
    {
        DW[Nr - 10] = _mm_aesimc_si128(W[10]);
        DW[Nr - 11] = _mm_aesimc_si128(W[11]);

        if (Nr > 12)
        {
            DW[Nr - 12] = _mm_aesimc_si128(W[12]);
            DW[Nr - 13] = _mm_aesimc_si128(W[13]);
        }
    }
    DW[0] = W[Nr];
}

/*
 * AESIntel::ClearKeyState()
 *
 *  Description:
 *      Clear the key and all state data.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void AESIntel::ClearKeyState()
{
    SecUtil::SecureErase(&Nr, sizeof(Nr));
    SecUtil::SecureErase(W, sizeof(W));
    SecUtil::SecureErase(DW, sizeof(DW));
    SecUtil::SecureErase(&T1, sizeof(T1));
    SecUtil::SecureErase(&T2, sizeof(T2));
    SecUtil::SecureErase(&T3, sizeof(T3));
    SecUtil::SecureErase(&T4, sizeof(T4));
}

/*
 * AESIntel::Encrypt()
 *
 *  Description:
 *      This function will encrypt a block of plaintext and return the
 *      ciphertext.  It is assumed the key was previously set via SetKey().
 *
 *  Parameters:
 *      plaintext [in]
 *          The 16-octet data block to encrypt.
 *
 *      ciphertext [out]
 *          The 16-octet encrypted data block.  AES operates in place, so this
 *          span may be the same memory location as the plaintext span.
 *
 *  Returns:
 *      Nothing, though the encrypted data is stored in the ciphertext
 *      parameter as output.
 *
 *  Comments:
 *      None.
 */
void AESIntel::Encrypt(
                const std::span<const std::uint8_t, AES_Block_Size> plaintext,
                std::span<std::uint8_t, AES_Block_Size> ciphertext) noexcept
{
    // Step 1 - AddRoundKey() (i.e., XOR with W[column])
    T1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(plaintext.data()));
    T1 = _mm_xor_si128(T1, W[0]);

    // Step 2 - Rounds 1 to Nr - 1
    T1 = _mm_aesenc_si128(T1, W[1]);
    T1 = _mm_aesenc_si128(T1, W[2]);
    T1 = _mm_aesenc_si128(T1, W[3]);
    T1 = _mm_aesenc_si128(T1, W[4]);
    T1 = _mm_aesenc_si128(T1, W[5]);
    T1 = _mm_aesenc_si128(T1, W[6]);
    T1 = _mm_aesenc_si128(T1, W[7]);
    T1 = _mm_aesenc_si128(T1, W[8]);
    T1 = _mm_aesenc_si128(T1, W[9]);

    // If Nr > 10 implies either AES-192 or AES-256
    if (Nr > 10)
    {
        T1 = _mm_aesenc_si128(T1, W[10]);
        T1 = _mm_aesenc_si128(T1, W[11]);

        // Nr > 12 implies AES-256
        if (Nr > 12)
        {
            T1 = _mm_aesenc_si128(T1, W[12]);
            T1 = _mm_aesenc_si128(T1, W[13]);
        }
    }

    // Step 3 - Final round
    T1 = _mm_aesenclast_si128(T1, W[Nr]);

    // Store the result in the ciphertext buffer
    _mm_storeu_si128(reinterpret_cast<__m128i *>(ciphertext.data()), T1);
}

/*
 * AESIntel::Decrypt()
 *
 *  Description:
 *      This function will decrypt a block of ciphertext and return the
 *      plaintext.  It is assumed the key was previously set via SetKey().
 *
 *  Parameters:
 *      ciphertext [in]
 *          The 16-octet data block to decrypt.
 *
 *      plaintext [out]
 *          The 16-octet decrypted data block.  AES operates in place, so this
 *          span may be the same memory location as the ciphertext span.
 *
 *  Returns:
 *      Nothing, though the decrypted data is stored in the plaintext
 *      parameter as output.
 *
 *  Comments:
 *      None.
 */
void AESIntel::Decrypt(
                const std::span<const std::uint8_t, AES_Block_Size> ciphertext,
                std::span<std::uint8_t, AES_Block_Size> plaintext) noexcept
{
    // Step 1 - AddRoundKey() (i.e., XOR with W[0])
    T1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ciphertext.data()));
    T1 = _mm_xor_si128(T1, DW[0]);

    // Step 2 - Rounds 1 to Nr - 1
    T1 = _mm_aesdec_si128(T1, DW[1]);
    T1 = _mm_aesdec_si128(T1, DW[2]);
    T1 = _mm_aesdec_si128(T1, DW[3]);
    T1 = _mm_aesdec_si128(T1, DW[4]);
    T1 = _mm_aesdec_si128(T1, DW[5]);
    T1 = _mm_aesdec_si128(T1, DW[6]);
    T1 = _mm_aesdec_si128(T1, DW[7]);
    T1 = _mm_aesdec_si128(T1, DW[8]);
    T1 = _mm_aesdec_si128(T1, DW[9]);

    // If Nr > 10 implies either AES-192 or AES-256
    if (Nr > 10)
    {
        T1 = _mm_aesdec_si128(T1, DW[10]);
        T1 = _mm_aesdec_si128(T1, DW[11]);

        // Nr > 12 implies AES-256
        if (Nr > 12)
        {
            T1 = _mm_aesdec_si128(T1, DW[12]);
            T1 = _mm_aesdec_si128(T1, DW[13]);
        }
    }

    // Step 3 - Final round
    T1 = _mm_aesdeclast_si128(T1, DW[Nr]);

    // Store the result in the ciphertext buffer
    _mm_storeu_si128(reinterpret_cast<__m128i *>(plaintext.data()), T1);
}

/*
 * AESIntel::operator==()
 *
 *  Description:
 *      Compare two AESIntel objects for equality.  Equality means that the
 *      two objects have the same key data.
 *
 *  Parameters:
 *      other [in]
 *          The other AESIntel object with which to compare.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      SSE 4.1 offers _mm_test_all_zeros() that could make this faster,
 *      but since this function is unlikely to be called frequently, this
 *      function takes a slower approach of unloading the values for comparison.
 */
bool AESIntel::operator==(const AESIntel &other) const
{
    const std::array<std::uint8_t, 16> zeros{};
    std::array<std::uint8_t, 16> result{};

    // If comparing to self, return true
    if (this == &other) return true;

    if (Nr != other.Nr) return false;

    // Compare the key schedules
    for (std::size_t i = 0; i < Max_Rounds + 1; i++)
    {
        // XOR the two arrays, so all zeros means they are equal
        __m128i value = _mm_xor_si128(W[i], other.W[i]);

        // Get the result
        _mm_storeu_si128(reinterpret_cast<__m128i *>(result.data()), value);

        // If the result is not all zeros, they are not equal
        if (result != zeros) return false;

        // XOR the two arrays, so all zeros means they are equal
        value = _mm_xor_si128(DW[i], other.DW[i]);

        // Get the result
        _mm_storeu_si128(reinterpret_cast<__m128i *>(result.data()), value);

        // If the result is not all zeros, they are not equal
        if (result != zeros) return false;
    }

    return true;
}

/*
 * AESIntel::operator!=()
 *
 *  Description:
 *      Compare two AESIntel objects for inequality.
 *
 *  Parameters:
 *      other [in]
 *          The other AESIntel object with which to compare.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
bool AESIntel::operator!=(const AESIntel &other) const
{
    return !(*this == other);
}

} // namespace Terra::Crypto::Cipher

#endif // TERRA_USE_INTEL_INTRINSICS
