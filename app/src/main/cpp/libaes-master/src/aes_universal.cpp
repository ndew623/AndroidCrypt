/*
 *  aes_universal.cpp
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This is the implementation file for the AESUniversal object that
 *      performs encryption and decryption as specified in FIPS 197 ("Advanced
 *      Encryption Standard").
 *
 *      The state array defined in FIPS 197 is implemented as an array of
 *      32-bit column vectors where the most significant 8 bits represent
 *      row 0 and the least significant 8 bits represent row 3 of the
 *      of the state array for the given column.
 *
 *      This code is time-optimized, though optimization is dependent on
 *      enabling compiler optimizations that will inline constexpr functions.
 *
 *      This implementation of AES is called "universal" as it can operate on
 *      any processor and is not dependent on processor-specific features.
 *
 *  Portability Issues:
 *      None.
 */

#include <cstring>
#include <terra/secutil/secure_erase.h>
#include "aes_universal.h"
#include "aes_tables.h"
#include "aes_utilities.h"

namespace Terra::Crypto::Cipher
{

/*
 * AESUniversal::AESUniversal()
 *
 *  Description:
 *      This is a constructor for the AESUniversal object with no given key.
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
AESUniversal::AESUniversal() noexcept :
    AESEngine(),
    Nr{},
    Nk{},
    state{},
    alt_state{},
    W{},
    DW{}
{
}

/*
 * AESUniversal::AESUniversal()
 *
 *  Description:
 *      This is a constructor for the AESUniversal object that accepts a span
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
AESUniversal::AESUniversal(const std::span<const std::uint8_t> key) :
    AESUniversal()
{
    SetKey(key);
}

/*
 * AESUniversal::AESUniversal()
 *
 *  Description:
 *      This is a copy constructor for the AESUniversal object that accepts a
 *      reference to another AESUniversal object as a parameter.
 *
 *  Parameters:
 *      other [in]
 *          The other AESUniversal object from which to copy values.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
AESUniversal::AESUniversal(const AESUniversal &other) noexcept : AESUniversal()
{
    Nr = other.Nr;
    Nk = other.Nk;
    std::memcpy(W, other.W, sizeof(W));
    std::memcpy(DW, other.DW, sizeof(DW));
}

/*
 * AESUniversal::AESUniversal()
 *
 *  Description:
 *      This is a move constructor for the AESUniversal object that accepts a
 *      reference to another AESUniversal object as a parameter.  Note that
 *      this move operation actually just performs a copy of data, since there
 *      is no dynamic data to really move.
 *
 *  Parameters:
 *      other [in]
 *          The other AESUniversal object from which to move values.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      This function just copies the other AESUniversal object's values since
 *      there is no internal data that can be moved.
 */
AESUniversal::AESUniversal(AESUniversal &&other) noexcept : AESUniversal()
{
    Nr = other.Nr;
    Nk = other.Nk;
    std::memcpy(W, other.W, sizeof(W));
    std::memcpy(DW, other.DW, sizeof(DW));
}

/*
 * AESUniversal::~AESUniversal()
 *
 *  Description:
 *      This is the destructor for the AESUniversal object and is responsible
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
AESUniversal::~AESUniversal()
{
    SecUtil::SecureErase(&Nr, sizeof(Nr));
    SecUtil::SecureErase(&Nk, sizeof(Nk));
    SecUtil::SecureErase(state, sizeof(state));
    SecUtil::SecureErase(alt_state, sizeof(alt_state));
    SecUtil::SecureErase(W, sizeof(W));
    SecUtil::SecureErase(DW, sizeof(DW));
}

/*
 * AESUniversal::operator=()
 *
 *  Description:
 *      Assign one AESUniversal object to another.
 *
 *  Parameters:
 *      other [in]
 *          The other AESUniversal from which to copy data.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
AESUniversal &AESUniversal::operator=(const AESUniversal &other)
{
    // If assigning to self, jut return this
    if (this == &other) return *this;

    Nr = other.Nr;
    Nk = other.Nk;
    std::memcpy(W, other.W, sizeof(W));
    std::memcpy(DW, other.DW, sizeof(DW));

    return *this;
}

/*
 * AESUniversal::operator=()
 *
 *  Description:
 *      Move assignment operator to assign another AESUniversal object to this
 *      one.
 *
 *  Parameters:
 *      other [in]
 *          The other AESUniversal from which to move data.
 *
 *  Returns:
 *      A reference to this AESUniversal object.
 *
 *  Comments:
 *      None.
 */
AESUniversal &AESUniversal::operator=(AESUniversal &&other) noexcept
{
    // If assigning to self, jut return this
    if (this == &other) return *this;

    Nr = other.Nr;
    Nk = other.Nk;
    std::memcpy(W, other.W, sizeof(W));
    std::memcpy(DW, other.DW, sizeof(DW));

    return *this;
}

/*
 * AESUniversal::SetKey()
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
void AESUniversal::SetKey(const std::span<const std::uint8_t> key)
{
    // Zero the key schedule
    SecUtil::SecureErase(W, sizeof(W));
    SecUtil::SecureErase(DW, sizeof(DW));

    // Create the encryption round keys given the key length (W)
    switch (key.size())
    {
        case 16:
            Nr = 10;
            Nk = 4;

            // Fill the first Nk 32-bit words in the round key array W
            W[0] = GetWordFromBuffer(key.data(), 0);
            W[1] = GetWordFromBuffer(key.data(), 1);
            W[2] = GetWordFromBuffer(key.data(), 2);
            W[3] = GetWordFromBuffer(key.data(), 3);

            // Fill the remaining word in the round key array W
            for (std::size_t i = Nk, j = 0; i <= 40; i += Nk, j++)
            {
                W[i + 0] = W[i - 4] ^ SubBytes(RotWord(W[i - 1])) ^ Rcon[j];
                W[i + 1] = W[i - 3] ^ W[i + 0];
                W[i + 2] = W[i - 2] ^ W[i + 1];
                W[i + 3] = W[i - 1] ^ W[i + 2];
            }

            break;

        case 24:
            Nr = 12;
            Nk = 6;

            // Fill the first Nk words in the round key array W
            W[0] = GetWordFromBuffer(key.data(), 0);
            W[1] = GetWordFromBuffer(key.data(), 1);
            W[2] = GetWordFromBuffer(key.data(), 2);
            W[3] = GetWordFromBuffer(key.data(), 3);
            W[4] = GetWordFromBuffer(key.data(), 4);
            W[5] = GetWordFromBuffer(key.data(), 5);

            // Fill the remaining word in the round key array W
            for (std::size_t i = Nk, j = 0; i <= 42; i += Nk, j++)
            {
                W[i + 0] = W[i - 6] ^ SubBytes(RotWord(W[i - 1])) ^ Rcon[j];
                W[i + 1] = W[i - 5] ^ W[i + 0];
                W[i + 2] = W[i - 4] ^ W[i + 1];
                W[i + 3] = W[i - 3] ^ W[i + 2];
                W[i + 4] = W[i - 2] ^ W[i + 3];
                W[i + 5] = W[i - 1] ^ W[i + 4];
            }
            W[48] = W[42] ^ SubBytes(RotWord(W[47])) ^ Rcon[7];
            W[49] = W[43] ^ W[48];
            W[50] = W[44] ^ W[49];
            W[51] = W[45] ^ W[50];

            break;

        case 32:
            Nr = 14;
            Nk = 8;

            // Fill the first Nk words in the round key array W
            W[0] = GetWordFromBuffer(key.data(), 0);
            W[1] = GetWordFromBuffer(key.data(), 1);
            W[2] = GetWordFromBuffer(key.data(), 2);
            W[3] = GetWordFromBuffer(key.data(), 3);
            W[4] = GetWordFromBuffer(key.data(), 4);
            W[5] = GetWordFromBuffer(key.data(), 5);
            W[6] = GetWordFromBuffer(key.data(), 6);
            W[7] = GetWordFromBuffer(key.data(), 7);

            // Fill the remaining word in the round key array W
            for (std::size_t i = Nk, j = 0; i <= 48; i += Nk, j++)
            {
                W[i + 0] = W[i - 8] ^ SubBytes(RotWord(W[i - 1])) ^ Rcon[j];
                W[i + 1] = W[i - 7] ^ W[i + 0];
                W[i + 2] = W[i - 6] ^ W[i + 1];
                W[i + 3] = W[i - 5] ^ W[i + 2];
                W[i + 4] = W[i - 4] ^ SubBytes(W[i + 3]);
                W[i + 5] = W[i - 3] ^ W[i + 4];
                W[i + 6] = W[i - 2] ^ W[i + 5];
                W[i + 7] = W[i - 1] ^ W[i + 6];
            }
            W[56] = W[48] ^ SubBytes(RotWord(W[55])) ^ Rcon[6];
            W[57] = W[49] ^ W[56];
            W[58] = W[50] ^ W[57];
            W[59] = W[51] ^ W[58];

            break;

        default:
            throw AESException("Invalid key length provided");

            break;
    }

    // Populate decryption round key array (DW)
    for (std::uint_fast32_t *dw = DW,
                            *w = W + (Nr * Nb),
                            *lower_bound = DW + Nb,
                            *upper_bound = DW + ((Nr - 1) * Nb);
         w >= W;
         dw += Nb, w -= Nb)
    {
        if ((dw >= lower_bound) && (dw <= upper_bound))
        {
            dw[0] = FastInvMixColumn(w[0]);
            dw[1] = FastInvMixColumn(w[1]);
            dw[2] = FastInvMixColumn(w[2]);
            dw[3] = FastInvMixColumn(w[3]);
        }
        else
        {
            dw[0] = w[0];
            dw[1] = w[1];
            dw[2] = w[2];
            dw[3] = w[3];
        }
    }
}

/*
 * AESUniversal::ClearKeyState()
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
void AESUniversal::ClearKeyState()
{
    SecUtil::SecureErase(&Nr, sizeof(Nr));
    SecUtil::SecureErase(&Nk, sizeof(Nk));
    SecUtil::SecureErase(state, sizeof(state));
    SecUtil::SecureErase(alt_state, sizeof(alt_state));
    SecUtil::SecureErase(W, sizeof(W));
    SecUtil::SecureErase(DW, sizeof(DW));
}

/*
 * AESUniversal::Encrypt()
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
 *      Encryption steps enumerated in Section 5.1 of FIPS 197:
 *          1. AddRoundKey
 *          2. Round 1..Nr-1
 *              2a. SubBytes
 *              2b. ShiftRows
 *              2c. MixColumns
 *              2d. AddRoundKey
 *          3. Final round
 *              3a. ShiftRow
 *              3b. AddRoundKey
 */
void AESUniversal::Encrypt(
                const std::span<const std::uint8_t, AES_Block_Size> plaintext,
                std::span<std::uint8_t, AES_Block_Size> ciphertext) noexcept
{
    // Step 1 - AddRoundKey() (i.e., XOR with W[0])
    state[0] = AddRoundKey(GetWordFromBuffer(plaintext.data(), 0), W[0]);
    state[1] = AddRoundKey(GetWordFromBuffer(plaintext.data(), 1), W[1]);
    state[2] = AddRoundKey(GetWordFromBuffer(plaintext.data(), 2), W[2]);
    state[3] = AddRoundKey(GetWordFromBuffer(plaintext.data(), 3), W[3]);

    // Step 2 - Rounds 1 to Nr - 1 (MixColumns handled via array subscripts)

    // Round 1
    alt_state[0] = AddRoundKey(MixColShiftRow(0, state), W[4]);
    alt_state[1] = AddRoundKey(MixColShiftRow(1, state), W[5]);
    alt_state[2] = AddRoundKey(MixColShiftRow(2, state), W[6]);
    alt_state[3] = AddRoundKey(MixColShiftRow(3, state), W[7]);

    // Round 2
    state[0] = AddRoundKey(MixColShiftRow(0, alt_state), W[8]);
    state[1] = AddRoundKey(MixColShiftRow(1, alt_state), W[9]);
    state[2] = AddRoundKey(MixColShiftRow(2, alt_state), W[10]);
    state[3] = AddRoundKey(MixColShiftRow(3, alt_state), W[11]);

    // Round 3
    alt_state[0] = AddRoundKey(MixColShiftRow(0, state), W[12]);
    alt_state[1] = AddRoundKey(MixColShiftRow(1, state), W[13]);
    alt_state[2] = AddRoundKey(MixColShiftRow(2, state), W[14]);
    alt_state[3] = AddRoundKey(MixColShiftRow(3, state), W[15]);

    // Round 4
    state[0] = AddRoundKey(MixColShiftRow(0, alt_state), W[16]);
    state[1] = AddRoundKey(MixColShiftRow(1, alt_state), W[17]);
    state[2] = AddRoundKey(MixColShiftRow(2, alt_state), W[18]);
    state[3] = AddRoundKey(MixColShiftRow(3, alt_state), W[19]);

    // Round 5
    alt_state[0] = AddRoundKey(MixColShiftRow(0, state), W[20]);
    alt_state[1] = AddRoundKey(MixColShiftRow(1, state), W[21]);
    alt_state[2] = AddRoundKey(MixColShiftRow(2, state), W[22]);
    alt_state[3] = AddRoundKey(MixColShiftRow(3, state), W[23]);

    // Round 6
    state[0] = AddRoundKey(MixColShiftRow(0, alt_state), W[24]);
    state[1] = AddRoundKey(MixColShiftRow(1, alt_state), W[25]);
    state[2] = AddRoundKey(MixColShiftRow(2, alt_state), W[26]);
    state[3] = AddRoundKey(MixColShiftRow(3, alt_state), W[27]);

    // Round 7
    alt_state[0] = AddRoundKey(MixColShiftRow(0, state), W[28]);
    alt_state[1] = AddRoundKey(MixColShiftRow(1, state), W[29]);
    alt_state[2] = AddRoundKey(MixColShiftRow(2, state), W[30]);
    alt_state[3] = AddRoundKey(MixColShiftRow(3, state), W[31]);

    // Round 8
    state[0] = AddRoundKey(MixColShiftRow(0, alt_state), W[32]);
    state[1] = AddRoundKey(MixColShiftRow(1, alt_state), W[33]);
    state[2] = AddRoundKey(MixColShiftRow(2, alt_state), W[34]);
    state[3] = AddRoundKey(MixColShiftRow(3, alt_state), W[35]);

    // Round 9
    alt_state[0] = AddRoundKey(MixColShiftRow(0, state), W[36]);
    alt_state[1] = AddRoundKey(MixColShiftRow(1, state), W[37]);
    alt_state[2] = AddRoundKey(MixColShiftRow(2, state), W[38]);
    alt_state[3] = AddRoundKey(MixColShiftRow(3, state), W[39]);

    if (Nr > 10)
    {
        // Round 10
        state[0] = AddRoundKey(MixColShiftRow(0, alt_state), W[40]);
        state[1] = AddRoundKey(MixColShiftRow(1, alt_state), W[41]);
        state[2] = AddRoundKey(MixColShiftRow(2, alt_state), W[42]);
        state[3] = AddRoundKey(MixColShiftRow(3, alt_state), W[43]);

        // Round 11
        alt_state[0] = AddRoundKey(MixColShiftRow(0, state), W[44]);
        alt_state[1] = AddRoundKey(MixColShiftRow(1, state), W[45]);
        alt_state[2] = AddRoundKey(MixColShiftRow(2, state), W[46]);
        alt_state[3] = AddRoundKey(MixColShiftRow(3, state), W[47]);

        if (Nr > 12)
        {
            // Round 12
            state[0] = AddRoundKey(MixColShiftRow(0, alt_state), W[48]);
            state[1] = AddRoundKey(MixColShiftRow(1, alt_state), W[49]);
            state[2] = AddRoundKey(MixColShiftRow(2, alt_state), W[50]);
            state[3] = AddRoundKey(MixColShiftRow(3, alt_state), W[51]);

            // Round 13
            alt_state[0] = AddRoundKey(MixColShiftRow(0, state), W[52]);
            alt_state[1] = AddRoundKey(MixColShiftRow(1, state), W[53]);
            alt_state[2] = AddRoundKey(MixColShiftRow(2, state), W[54]);
            alt_state[3] = AddRoundKey(MixColShiftRow(3, state), W[55]);
        }
    }

    // Step 3 - Final round
    state[0] = (Sbox[(alt_state[0] >> 24)       ] << 24) ^
               (Sbox[(alt_state[1] >> 16) & 0xff] << 16) ^
               (Sbox[(alt_state[2] >>  8) & 0xff] <<  8) ^
               (Sbox[(alt_state[3]      ) & 0xff]      ) ^
               W[(Nr << 2) + 0];
    state[1] = (Sbox[(alt_state[1] >> 24)       ] << 24) ^
               (Sbox[(alt_state[2] >> 16) & 0xff] << 16) ^
               (Sbox[(alt_state[3] >>  8) & 0xff] <<  8) ^
               (Sbox[(alt_state[0]      ) & 0xff]      ) ^
               W[(Nr << 2) + 1];
    state[2] = (Sbox[(alt_state[2] >> 24)       ] << 24) ^
               (Sbox[(alt_state[3] >> 16) & 0xff] << 16) ^
               (Sbox[(alt_state[0] >>  8) & 0xff] <<  8) ^
               (Sbox[(alt_state[1]      ) & 0xff]      ) ^
               W[(Nr << 2) + 2];
    state[3] = (Sbox[(alt_state[3] >> 24)       ] << 24) ^
               (Sbox[(alt_state[0] >> 16) & 0xff] << 16) ^
               (Sbox[(alt_state[1] >>  8) & 0xff] <<  8) ^
               (Sbox[(alt_state[2]      ) & 0xff]      ) ^
               W[(Nr << 2) + 3];

    // Move the state column into the ciphertext buffer
    PutStateColumn(state, 0, ciphertext.data());
    PutStateColumn(state, 1, ciphertext.data());
    PutStateColumn(state, 2, ciphertext.data());
    PutStateColumn(state, 3, ciphertext.data());
}

/*
 * AESUniversal::Decrypt()
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
 *      Decryption steps using the more efficient "Equivalent Inverse Cipher"
 *      enumerated in Section 5.3.5 of FIPS 197:
 *          1. AddRoundKey
 *          2. Round 1..Nr-1
 *              2a. InverseSubBytes
 *              2b. InverseShiftRows
 *              2c. InverseMixColumns
 *              2d. AddRoundKey
 *          3. Final round
 *              3a. InverseSubBytes
 *              3b. InverseShiftRows
 *              3c. AddRoundKey
 */
void AESUniversal::Decrypt(
                const std::span<const std::uint8_t, AES_Block_Size> ciphertext,
                std::span<std::uint8_t, AES_Block_Size> plaintext) noexcept
{
    // Step 1 - AddRoundKey() (i.e., XOR with DW[0])
    state[0] = AddRoundKey(GetWordFromBuffer(ciphertext.data(), 0), DW[0]);
    state[1] = AddRoundKey(GetWordFromBuffer(ciphertext.data(), 1), DW[1]);
    state[2] = AddRoundKey(GetWordFromBuffer(ciphertext.data(), 2), DW[2]);
    state[3] = AddRoundKey(GetWordFromBuffer(ciphertext.data(), 3), DW[3]);

    // Round 1
    alt_state[0] = AddRoundKey(InvMixColShiftRow(0, state), DW[4]);
    alt_state[1] = AddRoundKey(InvMixColShiftRow(1, state), DW[5]);
    alt_state[2] = AddRoundKey(InvMixColShiftRow(2, state), DW[6]);
    alt_state[3] = AddRoundKey(InvMixColShiftRow(3, state), DW[7]);

    // Round 2
    state[0] = AddRoundKey(InvMixColShiftRow(0, alt_state), DW[8]);
    state[1] = AddRoundKey(InvMixColShiftRow(1, alt_state), DW[9]);
    state[2] = AddRoundKey(InvMixColShiftRow(2, alt_state), DW[10]);
    state[3] = AddRoundKey(InvMixColShiftRow(3, alt_state), DW[11]);

    // Round 3
    alt_state[0] = AddRoundKey(InvMixColShiftRow(0, state), DW[12]);
    alt_state[1] = AddRoundKey(InvMixColShiftRow(1, state), DW[13]);
    alt_state[2] = AddRoundKey(InvMixColShiftRow(2, state), DW[14]);
    alt_state[3] = AddRoundKey(InvMixColShiftRow(3, state), DW[15]);

    // Round 4
    state[0] = AddRoundKey(InvMixColShiftRow(0, alt_state), DW[16]);
    state[1] = AddRoundKey(InvMixColShiftRow(1, alt_state), DW[17]);
    state[2] = AddRoundKey(InvMixColShiftRow(2, alt_state), DW[18]);
    state[3] = AddRoundKey(InvMixColShiftRow(3, alt_state), DW[19]);

    // Round 5
    alt_state[0] = AddRoundKey(InvMixColShiftRow(0, state), DW[20]);
    alt_state[1] = AddRoundKey(InvMixColShiftRow(1, state), DW[21]);
    alt_state[2] = AddRoundKey(InvMixColShiftRow(2, state), DW[22]);
    alt_state[3] = AddRoundKey(InvMixColShiftRow(3, state), DW[23]);

    // Round 6
    state[0] = AddRoundKey(InvMixColShiftRow(0, alt_state), DW[24]);
    state[1] = AddRoundKey(InvMixColShiftRow(1, alt_state), DW[25]);
    state[2] = AddRoundKey(InvMixColShiftRow(2, alt_state), DW[26]);
    state[3] = AddRoundKey(InvMixColShiftRow(3, alt_state), DW[27]);

    // Round 7
    alt_state[0] = AddRoundKey(InvMixColShiftRow(0, state), DW[28]);
    alt_state[1] = AddRoundKey(InvMixColShiftRow(1, state), DW[29]);
    alt_state[2] = AddRoundKey(InvMixColShiftRow(2, state), DW[30]);
    alt_state[3] = AddRoundKey(InvMixColShiftRow(3, state), DW[31]);

    // Round 8
    state[0] = AddRoundKey(InvMixColShiftRow(0, alt_state), DW[32]);
    state[1] = AddRoundKey(InvMixColShiftRow(1, alt_state), DW[33]);
    state[2] = AddRoundKey(InvMixColShiftRow(2, alt_state), DW[34]);
    state[3] = AddRoundKey(InvMixColShiftRow(3, alt_state), DW[35]);

    // Round 9
    alt_state[0] = AddRoundKey(InvMixColShiftRow(0, state), DW[36]);
    alt_state[1] = AddRoundKey(InvMixColShiftRow(1, state), DW[37]);
    alt_state[2] = AddRoundKey(InvMixColShiftRow(2, state), DW[38]);
    alt_state[3] = AddRoundKey(InvMixColShiftRow(3, state), DW[39]);

    if (Nr > 10)
    {
        // Round 10
        state[0] = AddRoundKey(InvMixColShiftRow(0, alt_state), DW[40]);
        state[1] = AddRoundKey(InvMixColShiftRow(1, alt_state), DW[41]);
        state[2] = AddRoundKey(InvMixColShiftRow(2, alt_state), DW[42]);
        state[3] = AddRoundKey(InvMixColShiftRow(3, alt_state), DW[43]);

        // Round 11
        alt_state[0] = AddRoundKey(InvMixColShiftRow(0, state), DW[44]);
        alt_state[1] = AddRoundKey(InvMixColShiftRow(1, state), DW[45]);
        alt_state[2] = AddRoundKey(InvMixColShiftRow(2, state), DW[46]);
        alt_state[3] = AddRoundKey(InvMixColShiftRow(3, state), DW[47]);

        if (Nr > 12)
        {
            // Round 12
            state[0] = AddRoundKey(InvMixColShiftRow(0, alt_state), DW[48]);
            state[1] = AddRoundKey(InvMixColShiftRow(1, alt_state), DW[49]);
            state[2] = AddRoundKey(InvMixColShiftRow(2, alt_state), DW[50]);
            state[3] = AddRoundKey(InvMixColShiftRow(3, alt_state), DW[51]);

            // Round 13
            alt_state[0] = AddRoundKey(InvMixColShiftRow(0, state), DW[52]);
            alt_state[1] = AddRoundKey(InvMixColShiftRow(1, state), DW[53]);
            alt_state[2] = AddRoundKey(InvMixColShiftRow(2, state), DW[54]);
            alt_state[3] = AddRoundKey(InvMixColShiftRow(3, state), DW[55]);
        }
    }

    // Step 3 - Final round
    state[0] = (InverseSbox[(alt_state[0] >> 24)       ] << 24) ^
               (InverseSbox[(alt_state[3] >> 16) & 0xff] << 16) ^
               (InverseSbox[(alt_state[2] >>  8) & 0xff] <<  8) ^
               (InverseSbox[(alt_state[1]      ) & 0xff]      ) ^
               DW[(Nr << 2) + 0];
    state[1] = (InverseSbox[(alt_state[1] >> 24)       ] << 24) ^
               (InverseSbox[(alt_state[0] >> 16) & 0xff] << 16) ^
               (InverseSbox[(alt_state[3] >>  8) & 0xff] <<  8) ^
               (InverseSbox[(alt_state[2]      ) & 0xff]      ) ^
               DW[(Nr << 2) + 1];
    state[2] = (InverseSbox[(alt_state[2] >> 24)       ] << 24) ^
               (InverseSbox[(alt_state[1] >> 16) & 0xff] << 16) ^
               (InverseSbox[(alt_state[0] >>  8) & 0xff] <<  8) ^
               (InverseSbox[(alt_state[3]      ) & 0xff]      ) ^
               DW[(Nr << 2) + 2];
    state[3] = (InverseSbox[(alt_state[3] >> 24)       ] << 24) ^
               (InverseSbox[(alt_state[2] >> 16) & 0xff] << 16) ^
               (InverseSbox[(alt_state[1] >>  8) & 0xff] <<  8) ^
               (InverseSbox[(alt_state[0]      ) & 0xff]      ) ^
               DW[(Nr << 2) + 3];

    // Move the state column into the plaintext buffer
    PutStateColumn(state, 0, plaintext.data());
    PutStateColumn(state, 1, plaintext.data());
    PutStateColumn(state, 2, plaintext.data());
    PutStateColumn(state, 3, plaintext.data());
}

/*
 * AESUniversal::operator==()
 *
 *  Description:
 *      Compare two AESUniversal objects for equality.  Equality means that the
 *      two objects have the same key data.
 *
 *  Parameters:
 *      other [in]
 *          The other AESUniversal object with which to compare.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
bool AESUniversal::operator==(const AESUniversal &other) const
{
    // If comparing to self, return true
    if (this == &other) return true;

    if (Nk != other.Nk) return false;
    if (Nr != other.Nr) return false;
    if (std::memcmp(W, other.W, sizeof(W)) != 0) return false;
    if (std::memcmp(DW, other.DW, sizeof(DW)) != 0) return false;

    return true;
}

/*
 * AESUniversal::operator!=()
 *
 *  Description:
 *      Compare two AESUniversal objects for inequality.
 *
 *  Parameters:
 *      other [in]
 *          The other AESUniversal object with which to compare.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
bool AESUniversal::operator!=(const AESUniversal &other) const
{
    return !(*this == other);
}

} // namespace Terra::Crypto::Cipher
