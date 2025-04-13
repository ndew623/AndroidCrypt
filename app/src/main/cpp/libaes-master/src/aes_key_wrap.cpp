/*
 *  aes_key_wrap.cpp
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements AES Key Wrap (RFC 3394) and AES Key Wrap with
 *      Padding (RFC 5649).  Functions are provided to both perform
 *      key wrap and unwrap.
 *
 *      These routines are also documented in NIST Special Publication 800-38F.
 *
 *  Portability Issues:
 *      None.
 */

#include <cstring>
#include <algorithm>
#include <terra/crypto/cipher/aes_key_wrap.h>
#include <terra/bitutil/byte_order.h>
#include <terra/secutil/secure_erase.h>

namespace Terra::Crypto::Cipher
{

/*
 *  AESKeyWrap::AESKeyWrap()
 *
 *  Description:
 *      This is a constructor for the AESKeyWrap object with no given key.
 *      Since a key is not provided to this version of the constructor, one
 *      must call SetKey() with a valid key before calling wrap or unwrap
 *      functions, as the results will otherwise be invalid.
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
AESKeyWrap::AESKeyWrap() :
    aes(),
    i{},
    j{},
    k{},
    n{},
    t{},
    tt{},
    A{},
    B{},
    R{},
    network_word{},
    padding_length{},
    integrity_data{},
    message_length_indicator{},
    plaintext_buffer{}
{
    // Nothing more to do
}

/*
 *  AESKeyWrap::AESKeyWrap()
 *
 *  Description:
 *      This is a constructor for the AESKeyWrap object that accepts a span
 *      of octets holding a AES key that will be used for subsequent encryption
 *      operations.
 *
 *  Parameters:
 *      key [in]
 *          The encryption key to use with this instance of the object.  The
 *          length of the key must be 16, 24, or 32 octets.
 *
 *  Returns:
 *      Nothing, though an AESException will be thrown if the key provided is
 *      an invalid length.
 *
 *  Comments:
 *      None.
 */
AESKeyWrap::AESKeyWrap(const std::span<const std::uint8_t> key) :
    aes(key),
    i{},
    j{},
    k{},
    n{},
    t{},
    tt{},
    A{},
    B{},
    R{},
    network_word{},
    padding_length{},
    integrity_data{},
    message_length_indicator{},
    plaintext_buffer{}
{
    // Nothing more to do
}

/*
 *  AESKeyWrap::~AESKeyWrap()
 *
 *  Description:
 *      This is the destructor for the AESKeyWrap object and is responsible for
 *      zeroing memory to ensure a clean termination with no residue.
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
AESKeyWrap::~AESKeyWrap()
{
    SecUtil::SecureErase(&i, sizeof(i));
    SecUtil::SecureErase(&t, sizeof(t));
    SecUtil::SecureErase(&tt, sizeof(tt));
    SecUtil::SecureErase(&j, sizeof(j));
    SecUtil::SecureErase(&k, sizeof(k));
    SecUtil::SecureErase(&B, sizeof(B));
    SecUtil::SecureErase(&padding_length, sizeof(padding_length));
    SecUtil::SecureErase(&network_word, sizeof(network_word));
    SecUtil::SecureErase(&integrity_data, sizeof(integrity_data));
    SecUtil::SecureErase(&message_length_indicator,
                         sizeof(message_length_indicator));
    SecUtil::SecureErase(&plaintext_buffer, sizeof(plaintext_buffer));
}

/*
 *  AESKeyWrap::SetKey()
 *
 *  Description:
 *      This function will set the key to be used for subsequent wrap and
 *      unwrap calls.
 *
 *  Parameters:
 *      key [in]
 *          The encryption key to use with this instance of the object.  The
 *          length of the key must be 16, 24, or 32 octets.
 *
 *  Returns:
 *      Nothing, though an exception will be thrown if the key provided is not
 *      one of 16, 24, or 32 octets in length as required by the standard.
 *
 *  Comments:
 *      None.
 */
void AESKeyWrap::SetKey(const std::span<const std::uint8_t> key)
{
    aes.SetKey(key);
}

/*
 *  AESKeyWrap::Wrap()
 *
 *  Description:
 *      This performs the AES Key Wrap as per RFC 3394.
 *
 *  Parameters:
 *      plaintext [in]
 *          The plaintext to be encrypted.  This must be at least two 64-bit
 *          blocks in size (i.e., at least 16 octets).  The plaintext must be
 *          and integral number of eight octets.
 *
 *      ciphertext [out]
 *          A buffer to hold the ciphertext.  The ouput will be exactly eight
 *          octets larger than the original plaintext.  Consequentially, the
 *          span must be exactly eight octets larger than the plaintext span.
 *
 *      alternative_iv [in]
 *          An eight octet initialization vector to use with AES Key Wrap.
 *          If this value is not exactly eight octets, then the default IV will
 *          be used as per RFC 3394.  Generally, one should not pass this
 *          parameter, though this is used by WrapWithPadding().
 *
 *  Returns:
 *      Nothing, though the output will be placed in ciphertext.
 *
 *  Comments:
 *      The plaintext and ciphertext buffers must be distinct.
 */
void AESKeyWrap::Wrap(const std::span<const std::uint8_t> plaintext,
                      std::span<std::uint8_t> ciphertext,
                      const std::span<const std::uint8_t> alternative_iv)
{
    // Ensure buffers appear sane ("& 0x07" performs a mod 8 check)
    if ((plaintext.size() < 16) || ((plaintext.size() & 0x07) != 0) ||
        (ciphertext.size() != (plaintext.size() + 8)) ||
        (!alternative_iv.empty() && (alternative_iv.size() != 8)))
    {
        throw AESException("One or more spans have an invalid length");
    }

    // Determine the number of 64-bit blocks to process
    n = (plaintext.size() >> 3);

    // Assign the IV
    A = B;
    if (!alternative_iv.empty())
    {
        std::memcpy(A, alternative_iv.data(), 8);
    }
    else
    {
        std::memcpy(A, AES_Key_Wrap_Default_IV, 8);
    }

    // Perform the key wrap
    std::memcpy(ciphertext.data() + 8, plaintext.data(), plaintext.size());
    for (j = 0, t = 1; j < 6; j++)
    {
        for (i = 1, R = ciphertext.data() + 8; i <= n; i++, t++, R += 8)
        {
            std::memcpy(B + 8, R, 8);
            aes.Encrypt(B, B);
            for (k = 8, tt = t; (k > 0) && (tt > 0); k--, tt >>= 8)
            {
                A[k - 1] ^= static_cast<std::uint8_t>(tt & 0xff);
            }
            std::memcpy(R, B + 8, 8);
        }
    }
    std::memcpy(ciphertext.data(), A, 8);
}

/*
 *  AESKeyWrap::Unwrap()
 *
 *  Description:
 *      This performs the AES Key Unwrap as per RFC 3394.  It allows one
 *      to optionally pass a buffer holding a 64-bit alternative IV that,
 *      if provided, will be used instead of the default value defined in
 *      RFC 3394.
 *
 *      To support AES Key Wrap with Padding (RFC 5639), the alternative IV
 *      should be empty and the caller should provide a span to a 64-bit
 *      "integrity_data". In that case, this function will NOT perform
 *      integrity checking on the unwrapped key.
 *
 *  Parameters:
 *      ciphertext [in]
 *          The ciphertext that is to be decrypted with the given key.  This
 *          must be at least two 64-bit blocks in size plus 8 octets of
 *          integrity data.  Thus, the minimum length is 24 octets.
 *
 *      plaintext [out]
 *          A buffer to hold the plaintext.  The plaintext buffer must be
 *          exactly eight octets less than the original ciphertext.
 *
 *      integrity_data [out]
 *          This is a span to a 64-bit buffer that will contain the integrity
 *          data determined through the unwrap process.  If this parameter
 *          is empty, this function will perform integrity checking internally.
 *          If this parameter is not empty, this function will not perform
 *          integrity checking and simply return the integrity data to the
 *          caller to be checked.  If both this and the initialization_vector
 *          are present, this parameter takes precedence.  This would normally
 *          not be provided by the caller, though is used by the routing
 *          UnwrapWithPadding().
 *
 *      alternative_iv [in]
 *          The eight octet initialization vector to use with AES Key Wrap.  If
 *          this value is empty, the default IV will be used as per RFC 3394.
 *          Note that if "integrity_data" is present, this parameter is unused.
 *
 *  Returns:
 *      True if successful, false if there was an integrity error.  If the
 *      integrity_data parameter provides a buffer, then the 64-bit integrity
 *      check register (A[] as defined in RFC 3394) will be returned to the
 *      caller without the integrity data being checked.
 *
 *  Comments:
 *      The plaintext and ciphertext buffers must be distinct.
 */
bool AESKeyWrap::Unwrap(const std::span<const std::uint8_t> ciphertext,
                        std::span<std::uint8_t> plaintext,
                        std::span<std::uint8_t> integrity_data,
                        const std::span<const std::uint8_t> alternative_iv)
{
    // Ensure buffers appear sane ("& 0x07" performs a mod 8 check)
    if ((ciphertext.size() < 24) || ((ciphertext.size() & 0x07) != 0) ||
        (plaintext.size() != (ciphertext.size() - 8)) ||
        (!integrity_data.empty() && (integrity_data.size() != 8)) ||
        (!alternative_iv.empty() && (alternative_iv.size() != 8)))
    {
        throw AESException("One or more spans are invalid");
    }

    // Determine the number of 64-bit blocks to process
    n = (ciphertext.size() - 8) >> 3;

    // Assign A to be C[0] (first 64-bit block of the ciphertext)
    A = B;
    std::memcpy(A, ciphertext.data(), 8);

    // Perform the key wrap
    std::memcpy(plaintext.data(), ciphertext.data() + 8, ciphertext.size() - 8);
    for (j = 0, t = 6 * n; j < 6; j++)
    {
        for (i = n, R = plaintext.data() + ciphertext.size() - 16;
             i >= 1;
             i--, t--, R -= 8)
        {
            for (k = 8, tt = t; (k > 0) && (tt > 0); k--, tt >>= 8)
            {
                A[k - 1] ^= static_cast<std::uint8_t>(tt & 0xff);
            }
            std::memcpy(B + 8, R, 8);
            aes.Decrypt(B, B);
            std::memcpy(R, B + 8, 8);
        }
    }

    // If the integrity_data parameter is provided, return A[] to the caller
    // so that the caller can perform integrity checking
    if (!integrity_data.empty())
    {
        std::memcpy(integrity_data.data(), A, 8);
        return true;
    }

    // Perform integrity checking internally
    if (alternative_iv.size() == 8)
    {
        if (std::memcmp(alternative_iv.data(), A, 8) != 0) return false;
    }
    else
    {
        if (std::memcmp(AES_Key_Wrap_Default_IV, A, 8) != 0) return false;
    }

    return true;
}

/*
 *  AESKeyWrap::WrapWithPadding()
 *
 *  Description:
 *      This fuction performs the AES Key Wrap with Padding as specified in
 *      RFC 5649.
 *
 *  Parameters:
 *      plaintext [in]
 *          The plaintext buffer that is to be encrypted with the provided key.
 *          Valid span length is between 1 and AES_Key_Wrap_with_Padding_Max.
 *
 *      ciphertext [out]
 *          A buffer to hold the ciphertext.  The span must be large enough to
 *          hold the output, which is dependent on the plaintext length.  If
 *          the plaintext length is an even eight octets, the ciphertext length
 *          will be an additional eight octets.  Otherwise, the ciphertext
 *          length will be the length of the plaintext plus enough octets
 *          to bring it to an even eight octets plus an additional eight octets.
 *          In other words, the ciphertext length will be:
 *            ciphertext_length = plaintext.size() + 8;
 *            if (plaintext.size() % 8 == 0)
 *                ciphertext_length += (8 - (plaintext.size() % 8))
 *          To simplify code, just use a ciphertext buffer that is 16 octets
 *          larger than the plaintext buffer.  The actual length of the
 *          ciphertext will be returned by this function.
 *
 *      alternative_iv [in]
 *          This is an alternative_iv vector to use.  The default value
 *          is specified in RFC 5649, but a different value may be provided.
 *          One would generally pass no value so that this function will use
 *          the default IV.  If provided, the length must be exactly four
 *          octets.
 *
 *  Returns:
 *      The length of the ciphertext.  If there is an error in one of the
 *      input spans, an exception will be thrown.
 *
 *  Comments:
 *      The plaintext and ciphertext buffers must be distinct.
 */
std::size_t AESKeyWrap::WrapWithPadding(
                            const std::span<const std::uint8_t> plaintext,
                            std::span<std::uint8_t> ciphertext,
                            const std::span<const std::uint8_t> alternative_iv)
{
    // Check to ensure that the plaintext length is properly bounded
    if (plaintext.empty() || (plaintext.size() > AES_Key_Wrap_with_Padding_Max))
    {
        throw AESException("Invalid plaintext length");
    }

    // Ensure the alternative IV has a valid length
    if ((!alternative_iv.empty() && (alternative_iv.size() != 4)))
    {
        throw AESException("Invalid Alternative IV length");
    }

    // Compute padding to be an even 8 octets (note: "& 0x07" == "% 8")
    if ((plaintext.size() & 0x07) != 0)
    {
        padding_length = 8 - (plaintext.size() & 0x07);
    }
    else
    {
        padding_length = 0;
    }

    // Ensure the ciphertext buffer is of sufficient length
    if (ciphertext.size() < plaintext.size() + padding_length + 8)
    {
        throw AESException("Ciphertext buffer is too short");
    }

    // Store the initialization vector as the first 4 octets of the ciphertext
    if (alternative_iv.empty())
    {
        std::memcpy(ciphertext.data(), Alternative_IV, 4);
    }
    else
    {
        std::memcpy(ciphertext.data(), alternative_iv.data(), 4);
    }

    // Store the original message length in network byte order as the
    // second 4 octets of the buffer
    network_word = BitUtil::NetworkByteOrder(
                                static_cast<std::uint32_t>(plaintext.size()));
    std::memcpy(ciphertext.data() + 4, &network_word, 4);

    // Copy the plaintext into the ciphertext buffer for encryption
    std::memcpy(ciphertext.data() + 8, plaintext.data(), plaintext.size());

    // Pad the buffer to be an even 8 octets with zeros
    if (padding_length > 0)
    {
        std::memset(ciphertext.data() + plaintext.size() + 8,
                    0,
                    padding_length);
    }

    // Encrypt the plaintext
    if (plaintext.size() <= 8)
    {
        // Encrypt using AES ECB mode
        aes.Encrypt(std::span<const std::uint8_t, 16>(ciphertext.data(), 16),
                    std::span<std::uint8_t, 16>(ciphertext.data(), 16));
    }
    else
    {
        // Encrypt using AES Key Wrap; note that while this buffer use violates
        // the requirement that buffers be distinct when calling Wrap(), this
        // is safe given how data is positioned in the buffer
        Wrap({ciphertext.data() + 8, plaintext.size() + padding_length},
             {ciphertext.data(), plaintext.size() + padding_length + 8},
             {ciphertext.data(), 8});
    }

    return plaintext.size() + padding_length + 8;
}

/*
 *  AESKeyWrap::UnwrapWithPadding()
 *
 *  Description:
 *      This fuction performs the AES Key Unwrap with Padding as specified in
 *      RFC 5649.
 *
 *  Parameters:
 *      ciphertext [in]
 *          A buffer containing the ciphertext to decrypt.
 *
 *      plaintext [out]
 *          A buffer to hold the decrypted ciphertext.  The size of this buffer
 *          MUST be at least the length of ciphertext minus 8 octets.  The
 *          final plaintext may be shorter, but that cannot be determined in
 *          advance.  The final length will be returned as output.
 *
 *      alternative_iv [in]
 *          This is an alternative_iv vector to use.  The default value
 *          is specified in RFC 5649, but a different value may be provided.
 *          Normally, no value is provided causing this function to use the
 *          default IV.
 *
 *  Returns:
 *      The length of the ciphertext or zero if the integrity check failed.
 *      If there is an error in one of the input spans, an exception will be
 *      thrown.
 *
 *  Comments:
 *      The plaintext and ciphertext buffers must be distinct.
 */
std::size_t AESKeyWrap::UnwrapWithPadding(
                            const std::span<const std::uint8_t> ciphertext,
                            std::span<std::uint8_t> plaintext,
                            const std::span<const std::uint8_t> alternative_iv)
{
    // Ensure buffers appear sane ("& 0x07" performs a mod 8 check)
    if ((ciphertext.size() < 16) || ((ciphertext.size() & 0x07) != 0) ||
        (plaintext.size() < (ciphertext.size() - 8)) ||
        (!alternative_iv.empty() && (alternative_iv.size() != 4)))
    {
        throw AESException("One or more spans have an invalid length");
    }

    // Ensure the alternative IV has a valid length
    if ((!alternative_iv.empty() && (alternative_iv.size() != 4)))
    {
        throw AESException("Invalid Alternative IV length");
    }

    // Decrypt the ciphertext
    if (ciphertext.size() == 16)
    {
        // Decrypt using AES ECB mode
        aes.Decrypt(std::span<const std::uint8_t, 16>(ciphertext.data(), 16),
                    plaintext_buffer);

        // Copy the integrity array
        std::memcpy(integrity_data, plaintext_buffer, 8);

        // Copy the plaintext into the output buffer
        std::memcpy(plaintext.data(), plaintext_buffer + 8, 8);
    }
    else
    {
        // Decrypt using AES Key Wrap
        if (!Unwrap(ciphertext,
                    {plaintext.data(), ciphertext.size() - 8},
                    integrity_data))
        {
            return 0;
        }
    }

    // Verify that the first 4 octets of the integrity data are correct
    if (alternative_iv.empty())
    {
        if (std::memcmp(Alternative_IV, integrity_data, 4) != 0) return 0;
    }
    else
    {
        if (std::memcmp(alternative_iv.data(), integrity_data, 4) != 0)
        {
            return 0;
        }
    }

    // Copy the integrity check octets
    std::memcpy(&network_word, integrity_data + 4, 4);

    // Ensure the message length indicator has a valid range
    message_length_indicator = BitUtil::NetworkByteOrder(network_word);
    if ((message_length_indicator > (ciphertext.size() - 8)) ||
        (message_length_indicator < ((ciphertext.size() - 8) - 7)))
    {
        return 0;
    }

    // Ensure that all padding bits are zero
    std::uint8_t *p = plaintext.data() + message_length_indicator;
    std::uint8_t *q = plaintext.data() + ciphertext.size() - 8;
    if (!std::all_of(p, q, [](std::uint8_t v) -> bool { return v == 0; }))
    {
        return 0;
    }

    return message_length_indicator;
}

} // namespace Terra::Crypto::Cipher
