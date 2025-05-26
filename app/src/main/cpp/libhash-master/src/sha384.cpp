/*
 *  sha384.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the object SHA384, which implements the Secure
 *      Hashing Algorithm SHA-384 as defined in FIPS 180-4.
 *
 *      Note that while FIPS 180-4 specifies that a message may be any number
 *      of bits in length from 0..2^128-1, this object will only operate on
 *      messages containing an integral number of octets.
 *
 *  Portability Issues:
 *      This code assumes the compiler and platform can support 64-bit integers.
 */

#include <cstring>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <climits>
#include <terra/crypto/hashing/sha384.h>
#include <terra/secutil/secure_erase.h>
#include <terra/bitutil/bit_rotation.h>
#include <terra/bitutil/bit_shift.h>

namespace Terra::Crypto::Hashing
{

namespace
{

// SHA-384 constants defined in FIPS 180-4 section 4.2.3
constexpr std::uint64_t K_t[] =
{
    0x428a2f98d728ae22, 0x7137449123ef65cd,
    0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
    0x3956c25bf348b538, 0x59f111f1b605d019,
    0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
    0xd807aa98a3030242, 0x12835b0145706fbe,
    0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1,
    0x9bdc06a725c71235, 0xc19bf174cf692694,
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3,
    0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483,
    0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210,
    0xb00327c898fb213f, 0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2, 0xd5a79147930aa725,
    0x06ca6351e003826f, 0x142929670a0e6e70,
    0x27b70a8546d22ffc, 0x2e1b21385c26c926,
    0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8,
    0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001,
    0xc24b8b70d0f89791, 0xc76c51a30654be30,
    0xd192e819d6ef5218, 0xd69906245565a910,
    0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
    0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb,
    0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc, 0x78a5636f43172f60,
    0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9,
    0xbef9a3f7b2c67915, 0xc67178f2e372532b,
    0xca273eceea26619c, 0xd186b8c721c0c207,
    0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
    0x06f067aa72176fba, 0x0a637dc5a2c898a6,
    0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493,
    0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a,
    0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

// Functions defined in FIPS 180-4 section 4.1.3
constexpr std::uint64_t SHA384_Ch(const std::uint64_t x,
                                         const std::uint64_t y,
                                         const std::uint64_t z)
{
    return (x & y) ^ ((~x) & z);
}
constexpr std::uint64_t SHA384_Maj(const std::uint64_t x,
                                          const std::uint64_t y,
                                          const std::uint64_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}
constexpr std::uint64_t SHA384_SIGMA_0(const std::uint64_t x)
{
    return BitUtil::RotateRight(x, 28) ^ BitUtil::RotateRight(x, 34) ^
           BitUtil::RotateRight(x, 39);
}
constexpr std::uint64_t SHA384_SIGMA_1(const std::uint64_t x)
{
    return BitUtil::RotateRight(x, 14) ^ BitUtil::RotateRight(x, 18) ^
           BitUtil::RotateRight(x, 41);
}
constexpr std::uint64_t SHA384_sigma_0(const std::uint64_t x)
{
    return BitUtil::RotateRight(x, 1) ^ BitUtil::RotateRight(x, 8) ^
           BitUtil::ShiftRight(x, 7);
}
constexpr std::uint64_t SHA384_sigma_1(const std::uint64_t x)
{
    return BitUtil::RotateRight(x, 19) ^ BitUtil::RotateRight(x, 61) ^
           BitUtil::ShiftRight(x, 6);
}

// Function to help populate the message schedule
constexpr std::uint64_t GetMessageWord(
                        const std::uint8_t message_block[SHA384::Block_Size],
                        const std::size_t index)
{
    return (static_cast<std::uint64_t>(message_block[index    ]) << 56) |
           (static_cast<std::uint64_t>(message_block[index + 1]) << 48) |
           (static_cast<std::uint64_t>(message_block[index + 2]) << 40) |
           (static_cast<std::uint64_t>(message_block[index + 3]) << 32) |
           (static_cast<std::uint64_t>(message_block[index + 4]) << 24) |
           (static_cast<std::uint64_t>(message_block[index + 5]) << 16) |
           (static_cast<std::uint64_t>(message_block[index + 6]) <<  8) |
           (static_cast<std::uint64_t>(message_block[index + 7]));
}

/*
 *  Step3()
 *
 *  Description:
 *      This function will perform Step 3 of the logic to process a message
 *      block as per section 6.1.2 of FIPS 180-4.  (See ProcessMessageBlock()
 *      for more complete context.)
 *
 *  Parameters:
 *      t [in]
 *          The loop counter value in the range 0..79.
 *
 *      W [in/ouy]
 *          A pointer to the message schedule array.
 *
 *      T [in/out]
 *          Temporary variable.
 *
 *      a_ [in]
 *          The current representation of working variable "a".
 *
 *      b_ [in]
 *          The current representation of working variable "b".
 *
 *      c_ [in]
 *          The current representation of working variable "c".
 *
 *      d_ [in/out]
 *          The current representation of working variable "d".
 *
 *      e_ [in]
 *          The current representation of working variable "e".
 *
 *      f_ [in]
 *          The current representation of working variable "f".
 *
 *      g_ [in]
 *          The current representation of working variable "g".
 *
 *      h_ [in/out]
 *          The current representation of working variable "h".
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      The rotation of variables (e.g., e=d, d=c, etc.) is accomplished by
 *      rotating what is passed into this function.  The last two assignments
 *      performed in this function are ones that are necessary and not merely
 *      a rotation step.
 */
constexpr void Step3(const std::size_t t,
                     std::uint64_t W[64],
                     std::uint64_t &T,
                     const std::uint64_t a_,
                     const std::uint64_t b_,
                     const std::uint64_t c_,
                     std::uint64_t &d_,
                     const std::uint64_t e_,
                     const std::uint64_t f_,
                     const std::uint64_t g_,
                     std::uint64_t &h_)
{
    if (t > 15)
    {
        W[t] = SHA384_sigma_1(W[t - 2]) +  W[t - 7] +
               SHA384_sigma_0(W[t - 15]) + W[t - 16];
    }

    T = h_ + SHA384_SIGMA_1(e_) + SHA384_Ch(e_, f_, g_) + K_t[t] + W[t];

    d_ += T;

    h_ = T + SHA384_SIGMA_0(a_) + SHA384_Maj(a_, b_, c_);
}

} // namespace

/*
 *  SHA384::SHA384()
 *
 *  Description:
 *      This is a constructor for the SHA384 object.
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
SHA384::SHA384() noexcept :
    Hash(),
    message_length{},
    input_block_length{},
    input_block{},
    message_digest{},
    W{},
    a{},
    b{},
    c{},
    d{},
    e{},
    f{},
    g{},
    h{},
    T{}
{
    // Initialize all data members
    SHA384::Reset();
}

/*
 *  SHA384::SHA384()
 *
 *  Description:
 *      This is a constructor for the SHA384 object.
 *
 *  Parameters:
 *      data [in]
 *          Data over which to compute a message digest.
 *
 *      auto_finalize [in]
 *          True if Finalize() be called automatically after consuming the
 *          input?  The default is true for this constructor.
 *
 *      spaces [in]
 *          True if spaces should be inserted between words in output strings.
 *          Defaults to true.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
SHA384::SHA384(const std::span<const std::uint8_t> data,
               bool auto_finalize,
               bool spaces) :
    Hash(spaces),
    message_length{},
    input_block_length{},
    input_block{},
    message_digest{},
    W{},
    a{},
    b{},
    c{},
    d{},
    e{},
    f{},
    g{},
    h{},
    T{}
{
    // Initialize all data members
    SHA384::Reset();

    // Process the input data
    SHA384::Input(data);

    // If asked to finalize the message digest, do it now
    if (auto_finalize) SHA384::Finalize();
}

/*
 *  SHA384::SHA384()
 *
 *  Description:
 *      This is a constructor for the SHA384 object.
 *
 *  Parameters:
 *      data [in]
 *          Data over which to compute a message digest.
 *
 *      auto_finalize [in]
 *          True if Finalize() be called automatically after consuming the
 *          input?  The default is true for this constructor.
 *
 *      spaces [in]
 *          True if spaces should be inserted between words in output strings.
 *          Defaults to true.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
SHA384::SHA384(const std::string_view data, bool auto_finalize, bool spaces) :
    SHA384(std::span<const std::uint8_t>(
             reinterpret_cast<const std::uint8_t *>(data.data()), data.size()),
           auto_finalize,
           spaces)
{
    // It is assumed that a character is eight bits
    static_assert(CHAR_BIT == 8);
}

/*
 *  SHA384::~SHA384()
 *
 *  Description:
 *      This is the destructor for the SHA384 object.
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
SHA384::~SHA384() noexcept
{
    // For security reasons, zero all internal data
    SecUtil::SecureErase(input_block, sizeof(input_block));
    SecUtil::SecureErase(input_block_length);
    SecUtil::SecureErase(&message_length, sizeof(message_length));
    SecUtil::SecureErase(W, sizeof(W));
    SecUtil::SecureErase(a);
    SecUtil::SecureErase(b);
    SecUtil::SecureErase(c);
    SecUtil::SecureErase(d);
    SecUtil::SecureErase(e);
    SecUtil::SecureErase(f);
    SecUtil::SecureErase(g);
    SecUtil::SecureErase(h);
    SecUtil::SecureErase(T);
    SecUtil::SecureErase(message_digest, sizeof(message_digest));
}

/*
 *  SHA384::operator==()
 *
 *  Description:
 *      Compare two objects for equality.  Equality includes the state of
 *      the object, so if one is finalized and the other is not, then they are
 *      not considered equal even if the same input was provided to both.  The
 *      reason is that the non-finalized object can accept additional data and
 *      ultimately produce a completely different result.  Further, all base
 *      member data is also compared for equality.
 *
 *  Parameters:
 *      other [in]
 *          The other object with which to compare.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
bool SHA384::operator==(const SHA384 &other) const noexcept
{
    // If the base class is not equal, neither is this object
    if (Hash::operator!=(other)) return false;

    if ((message_length != other.message_length) ||
        (input_block_length != other.input_block_length))
    {
        return false;
    }

    // Compare the input block
    if ((input_block_length > 0) &&
        (std::memcmp(input_block, other.input_block, input_block_length) != 0))
    {
        return false;
    }

    // Compare the message digest
    if (std::memcmp(message_digest,
                    other.message_digest,
                    sizeof(message_digest)) != 0)
    {
        return false;
    }

    return true;
}

/*
 *  SHA384::operator!=()
 *
 *  Description:
 *      Compare two SHA384 objects for inequality.
 *
 *  Parameters:
 *      other [in]
 *          The other SHA384 object with which to compare.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
bool SHA384::operator!=(const SHA384 &other) const noexcept
{
    return !(*this == other);
}

/*
 *  SHA384::Reset()
 *
 *  Description:
 *      This function will reset the state of the SHA384 object so that it may
 *      be reused to compute a new hash value.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      This function exists only to satisfy the required interface function
 *      specification defined in hash.h.
 */
void SHA384::Reset() noexcept
{
     // Initialize variables as required
    digest_finalized = false;
    corrupted = false;
    input_block_length = 0;
    message_length.high = 0;
    message_length.low = 0;

    // Initialize the message digest
    message_digest[0] = 0xcbbb9d5dc1059ed8;
    message_digest[1] = 0x629a292a367cd507;
    message_digest[2] = 0x9159015a3070dd17;
    message_digest[3] = 0x152fecd8f70e5939;
    message_digest[4] = 0x67332667ffc00b31;
    message_digest[5] = 0x8eb44a8768581511;
    message_digest[6] = 0xdb0c2e0d64f98fa7;
    message_digest[7] = 0x47b5481dbefa4fa4;
}

/*
 *  SHA384::Input()
 *
 *  Description:
 *      This function is used to feed the SHA-384 hashing algorithm with
 *      input data.
 *
 *  Parameters:
 *      data [in]
 *          A span of octets to be used as input.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      This function will raise an exception if the message digest has
 *      already been computed or if the message size exceeds the allowable
 *      length of (2^128)-1 bits.
 */
void SHA384::Input(const std::span<const std::uint8_t> data)
{
    std::size_t consumed = 0;
    std::size_t to_be_consumed = 0;

    // Ensure that the length doesn't exceed the maximum length
    if (SHA384MessageLength{ data.size(), 0 } > Max_Message_Size)
    {
        throw HashException("Input length too long");
    }

    // Ensure the internal data is not corrupted
    if (corrupted) throw HashException("SHA-384 message digest is corrupted");

    // If the message digest has already been computed, throw an exception
    if (digest_finalized)
    {
        throw HashException("SHA-384 message digest already computed");
    }

    // Return if there is no data to consume
    if (data.empty()) return;

    // Ensure we do not attempt to read from an invalid memory address
    if (data.data() == nullptr)
    {
        throw HashException("An invalid span was provided as input");
    }

    // Process the data until all of it is consumed
    while (consumed < data.size())
    {
        // How many octets to consume in this iteration?
        to_be_consumed = std::min(Block_Size - input_block_length,
                                  data.size() - consumed);

        // When processing a full message block, no need to copy data
        if (to_be_consumed == Block_Size)
        {
            ProcessMessageBlock(data.data() + consumed);
        }
        else
        {
            // Copy the partial message block into the input block buffer
            std::memcpy(input_block + input_block_length,
                        data.data() + consumed,
                        to_be_consumed);

            input_block_length += to_be_consumed;

            // If the input block is full, process it
            if (input_block_length == Block_Size)
            {
                ProcessMessageBlock(input_block);

                // Reset the input block length to zero
                input_block_length = 0;
            }
        }

        // Update the number of octets consumed and data pointer
        consumed += to_be_consumed;
    }

    // Increase the message length value
    message_length += data.size();

    // The maximum message size is (2^128)-1 bits, so check to ensure not too
    // much data was provided as input
    if (message_length > Max_Message_Size)
    {
        corrupted = true;
        throw HashException("SHA-384 message size exceeded");
    }
}

/*
 *  SHA384::Input()
 *
 *  Description:
 *      This function is used to feed the SHA-384 hashing algorithm with
 *      input data.
 *
 *  Parameters:
 *      data [in]
 *          A string of octets to provide as input into the hashing algorithm.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      This function will raise an exception if the message digest has
 *      already been computed or if the message size exceeds the allowable
 *      length of (2^128)-1 bits.
 */
void SHA384::Input(const std::string_view data)
{
    // It is assumed that a character is eight bits
    static_assert(CHAR_BIT == 8);

    // Provide the data to the Input function
    Input(std::span<const std::uint8_t>{
                    reinterpret_cast<const std::uint8_t *>(data.data()),
                    data.size() });
}

/*
 *  SHA384::ProcessMessageBlock()
 *
 *  Description:
 *      This function will consume a 128-octet (1024-bit) message block,
 *      performing the specified hashing operations on the block.
 *
 *  Parameters:
 *      message_block [in]
 *          A message block to consume as part of the hash computation.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      This function returns no value, but it will alter the message in
 *      preparation for computing the message digest.  Note that variable names
 *      specified here are defined in FIPS 180-4 section 6.4.2.
 */
void SHA384::ProcessMessageBlock(const std::uint8_t message_block[Block_Size])
{
    // STEP 1

    // Break the message block into words, filling part of the message schedule
    W[ 0] = GetMessageWord(message_block,  0);
    W[ 1] = GetMessageWord(message_block,  8);
    W[ 2] = GetMessageWord(message_block, 16);
    W[ 3] = GetMessageWord(message_block, 24);
    W[ 4] = GetMessageWord(message_block, 32);
    W[ 5] = GetMessageWord(message_block, 40);
    W[ 6] = GetMessageWord(message_block, 48);
    W[ 7] = GetMessageWord(message_block, 56);
    W[ 8] = GetMessageWord(message_block, 64);
    W[ 9] = GetMessageWord(message_block, 72);
    W[10] = GetMessageWord(message_block, 80);
    W[11] = GetMessageWord(message_block, 88);
    W[12] = GetMessageWord(message_block, 96);
    W[13] = GetMessageWord(message_block, 104);
    W[14] = GetMessageWord(message_block, 112);
    W[15] = GetMessageWord(message_block, 120);

    // The balance of W[] will be computed as needed in Step 3

    // STEP 2

    // Initialize the working variables
    a = message_digest[0];
    b = message_digest[1];
    c = message_digest[2];
    d = message_digest[3];
    e = message_digest[4];
    f = message_digest[5];
    g = message_digest[6];
    h = message_digest[7];

    // STEP 3

    Step3( 0, W, T, a, b, c, d, e, f, g, h);
    Step3( 1, W, T, h, a, b, c, d, e, f, g);
    Step3( 2, W, T, g, h, a, b, c, d, e, f);
    Step3( 3, W, T, f, g, h, a, b, c, d, e);
    Step3( 4, W, T, e, f, g, h, a, b, c, d);
    Step3( 5, W, T, d, e, f, g, h, a, b, c);
    Step3( 6, W, T, c, d, e, f, g, h, a, b);
    Step3( 7, W, T, b, c, d, e, f, g, h, a);
    Step3( 8, W, T, a, b, c, d, e, f, g, h);
    Step3( 9, W, T, h, a, b, c, d, e, f, g);
    Step3(10, W, T, g, h, a, b, c, d, e, f);
    Step3(11, W, T, f, g, h, a, b, c, d, e);
    Step3(12, W, T, e, f, g, h, a, b, c, d);
    Step3(13, W, T, d, e, f, g, h, a, b, c);
    Step3(14, W, T, c, d, e, f, g, h, a, b);
    Step3(15, W, T, b, c, d, e, f, g, h, a);
    Step3(16, W, T, a, b, c, d, e, f, g, h);
    Step3(17, W, T, h, a, b, c, d, e, f, g);
    Step3(18, W, T, g, h, a, b, c, d, e, f);
    Step3(19, W, T, f, g, h, a, b, c, d, e);
    Step3(20, W, T, e, f, g, h, a, b, c, d);
    Step3(21, W, T, d, e, f, g, h, a, b, c);
    Step3(22, W, T, c, d, e, f, g, h, a, b);
    Step3(23, W, T, b, c, d, e, f, g, h, a);
    Step3(24, W, T, a, b, c, d, e, f, g, h);
    Step3(25, W, T, h, a, b, c, d, e, f, g);
    Step3(26, W, T, g, h, a, b, c, d, e, f);
    Step3(27, W, T, f, g, h, a, b, c, d, e);
    Step3(28, W, T, e, f, g, h, a, b, c, d);
    Step3(29, W, T, d, e, f, g, h, a, b, c);
    Step3(30, W, T, c, d, e, f, g, h, a, b);
    Step3(31, W, T, b, c, d, e, f, g, h, a);
    Step3(32, W, T, a, b, c, d, e, f, g, h);
    Step3(33, W, T, h, a, b, c, d, e, f, g);
    Step3(34, W, T, g, h, a, b, c, d, e, f);
    Step3(35, W, T, f, g, h, a, b, c, d, e);
    Step3(36, W, T, e, f, g, h, a, b, c, d);
    Step3(37, W, T, d, e, f, g, h, a, b, c);
    Step3(38, W, T, c, d, e, f, g, h, a, b);
    Step3(39, W, T, b, c, d, e, f, g, h, a);
    Step3(40, W, T, a, b, c, d, e, f, g, h);
    Step3(41, W, T, h, a, b, c, d, e, f, g);
    Step3(42, W, T, g, h, a, b, c, d, e, f);
    Step3(43, W, T, f, g, h, a, b, c, d, e);
    Step3(44, W, T, e, f, g, h, a, b, c, d);
    Step3(45, W, T, d, e, f, g, h, a, b, c);
    Step3(46, W, T, c, d, e, f, g, h, a, b);
    Step3(47, W, T, b, c, d, e, f, g, h, a);
    Step3(48, W, T, a, b, c, d, e, f, g, h);
    Step3(49, W, T, h, a, b, c, d, e, f, g);
    Step3(50, W, T, g, h, a, b, c, d, e, f);
    Step3(51, W, T, f, g, h, a, b, c, d, e);
    Step3(52, W, T, e, f, g, h, a, b, c, d);
    Step3(53, W, T, d, e, f, g, h, a, b, c);
    Step3(54, W, T, c, d, e, f, g, h, a, b);
    Step3(55, W, T, b, c, d, e, f, g, h, a);
    Step3(56, W, T, a, b, c, d, e, f, g, h);
    Step3(57, W, T, h, a, b, c, d, e, f, g);
    Step3(58, W, T, g, h, a, b, c, d, e, f);
    Step3(59, W, T, f, g, h, a, b, c, d, e);
    Step3(60, W, T, e, f, g, h, a, b, c, d);
    Step3(61, W, T, d, e, f, g, h, a, b, c);
    Step3(62, W, T, c, d, e, f, g, h, a, b);
    Step3(63, W, T, b, c, d, e, f, g, h, a);
    Step3(64, W, T, a, b, c, d, e, f, g, h);
    Step3(65, W, T, h, a, b, c, d, e, f, g);
    Step3(66, W, T, g, h, a, b, c, d, e, f);
    Step3(67, W, T, f, g, h, a, b, c, d, e);
    Step3(68, W, T, e, f, g, h, a, b, c, d);
    Step3(69, W, T, d, e, f, g, h, a, b, c);
    Step3(70, W, T, c, d, e, f, g, h, a, b);
    Step3(71, W, T, b, c, d, e, f, g, h, a);
    Step3(72, W, T, a, b, c, d, e, f, g, h);
    Step3(73, W, T, h, a, b, c, d, e, f, g);
    Step3(74, W, T, g, h, a, b, c, d, e, f);
    Step3(75, W, T, f, g, h, a, b, c, d, e);
    Step3(76, W, T, e, f, g, h, a, b, c, d);
    Step3(77, W, T, d, e, f, g, h, a, b, c);
    Step3(78, W, T, c, d, e, f, g, h, a, b);
    Step3(79, W, T, b, c, d, e, f, g, h, a);

    // STEP 4

    // Compute the i-th intermediate hash value
    message_digest[0] += a;
    message_digest[1] += b;
    message_digest[2] += c;
    message_digest[3] += d;
    message_digest[4] += e;
    message_digest[5] += f;
    message_digest[6] += g;
    message_digest[7] += h;
}

/*
 *  SHA384::Finalize()
 *
 *  Description:
 *      This function will finalize the SHA-384 message digest computation.
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
void SHA384::Finalize()
{
    // Return if the message digest has already been finalized or is corrupted
    if (digest_finalized || corrupted) return;

    // Pad the message per FIPS 180-4 section 5.1.2
    PadMessage();

    // At this point, the message digest will have been computed
    digest_finalized = true;
}

/*
 *  SHA384::PadMessage()
 *
 *  Description:
 *      This function will pad the message as per FIPS 180-4 section 5.1.2
 *      in preparation for computing the message digest.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      This function returns no value, but it will alter the message in
 *      preparation for computing the message digest.
 */
void SHA384::PadMessage()
{
    // Append 0x80 to the end of the message
    input_block[input_block_length++] = 0x80;

    // Pad out to a full input block if we have more than 896 bits (112 octets)
    if (input_block_length > 112)
    {
        // Pad the input block with zeros
        std::memset(input_block + input_block_length,
                    0,
                    128 - input_block_length);

        // The input block is now 128 octets, but that will be reset below

        // Process the current input block
        ProcessMessageBlock(input_block);

        // Reset the input block length to zero
        input_block_length = 0;
    }

    // Pad up to 896 bits (112 octets)
    if (input_block_length < 112)
    {
        std::memset(input_block + input_block_length,
                    0,
                    112 - input_block_length);
    }

    // The final 128 bits contain the message length (convert length to bits),
    // but we need to shift bits to convert from octets
    std::uint64_t length_low = message_length.low;
    std::uint64_t length_high = message_length.high;
    length_high <<= 3;
    length_high |= ((length_low & 0xe000000000000000) >> 61);
    length_low <<= 3;
    for (std::size_t i = 119; i > 111; i--)
    {
        input_block[i] = (length_high & 0xff);
        length_high >>= 8;
    }
    for (std::size_t i = 127; i > 119; i--)
    {
        input_block[i] = (length_low & 0xff);
        length_low >>= 8;
    }

    // Process the current input block
    ProcessMessageBlock(input_block);

    // Reset the input block length to zero
    input_block_length = 0;
}

/*
 *  SHA384::Result()
 *
 *  Description:
 *      This function will return a string containing a string representing the
 *      computed message digest.  Finalize() must have been called previously,
 *      else an exception is thrown.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string containing the computed message digest.
 *
 *  Comments:
 *      None.
 */
std::string SHA384::Result() const
{
    std::ostringstream oss;

    // Ensure the internal data is not corrupted
    if (corrupted) throw HashException("SHA-384 message digest is corrupted");

    // Ensure the message digest computation has been finalized
    if (!digest_finalized)
    {
        throw HashException("SHA-384 message digest has not been finalized");
    }

    oss << std::hex << std::setfill('0');

    for (std::size_t i = 0; i < Digest_Word_Count; i++)
    {
        if (space_separate_words && (i > 0)) oss << " ";
        oss << std::setw(16) << message_digest[i];
    }

    return oss.str();
}

/*
 *  SHA384::Result()
 *
 *  Description:
 *      This function will return the message digest result in a span of octets.
 *      Finalize() must have been called previously, else an exception is
 *      thrown.
 *
 *  Parameters:
 *      result [out]
 *          This is a span of octets into which the hash result will be placed.
 *          The size of the span must be at least Digest_Octet_Count octets
 *          in length.
 *
 *  Returns:
 *      This will return a span over the same input span, but with the
 *      size restricted to exactly Digest_Octet_Count octets.
 *
 *  Comments:
 *      None.
 */
std::span<std::uint8_t> SHA384::Result(std::span<std::uint8_t> result) const
{
    // Ensure the internal data is not corrupted
    if (corrupted) throw HashException("SHA-384 message digest is corrupted");

    // Ensure the message digest computation has been finalized
    if (!digest_finalized)
    {
        throw HashException("SHA-384 message digest has not been finalized");
    }

    // Ensure the span has sufficient space
    if (result.size() < Digest_Octet_Count)
    {
        throw HashException("SHA-384 result input span is too short");
    }

    for (std::size_t i = 0, j = 0; i < Digest_Word_Count; i++, j += 8)
    {
        result[j    ] = ((message_digest[i] >> 56) & 0xff);
        result[j + 1] = ((message_digest[i] >> 48) & 0xff);
        result[j + 2] = ((message_digest[i] >> 40) & 0xff);
        result[j + 3] = ((message_digest[i] >> 32) & 0xff);
        result[j + 4] = ((message_digest[i] >> 24) & 0xff);
        result[j + 5] = ((message_digest[i] >> 16) & 0xff);
        result[j + 6] = ((message_digest[i] >>  8) & 0xff);
        result[j + 7] = ((message_digest[i]      ) & 0xff);
    }

    return result.first(Digest_Octet_Count);
}

/*
 *  SHA384::Result()
 *
 *  Description:
 *      This function will return the message digest result in a span of
 *      64-bit integers.  Finalize() must have been called previously, else
 *      an exception is thrown.
 *
 *  Parameters:
 *      result [out]
 *          Contains the result of the message digest computation.
 *
 *  Returns:
 *      This will return a span over the same input span, but with the
 *      size restricted to exactly Digest_Word_Count octets.
 *
 *  Comments:
 *      None.
 */
SHA384ResultWordSpan SHA384::Result(SHA384ResultWordSpan result) const
{
    // Ensure the internal data is not corrupted
    if (corrupted) throw HashException("SHA-384 message digest is corrupted");

    // Ensure the message digest computation has been finalized
    if (!digest_finalized)
    {
        throw HashException("SHA-384 message digest has not been finalized");
    }

    // Ensure the span has sufficient space
    if (result.size() < Digest_Word_Count)
    {
        throw HashException("SHA-384 result input span is too short");
    }

    // Place the message digest into the result vector
    std::copy_n(message_digest, Digest_Word_Count, result.data());

    return result.first(Digest_Word_Count);
}

/*
 *  SHA384::GetMessageLength()
 *
 *  Description:
 *      Return the message length in octets.  This will be greater than zero if
 *      input was provided and no error occurred.  This has no affect on the
 *      state of the message digest computation.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      The length in octets of the message consumed in computing the message
 *      digest.
 *
 *  Comments:
 *      None.
 */
SHA384MessageLength SHA384::GetMessageLength() const noexcept
{
    return message_length;
}

/*
 *  operator<<() for SHA384MessageLength
 *
 *  Description:
 *      This function will output the SHA384MessageLength in hex.
 *
 *  Parameters:
 *      os [in]
 *          Output stream object.
 *
 *      length [in]
 *          The SHA384MessageLength structure to output.
 *
 *  Returns:
 *      The output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &operator<<(std::ostream &os, const SHA384MessageLength &length)
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(16) << length.high
        << std::setw(16) << length.low;
    os << oss.str();

    return os;
}

} // namespace Terra::Crypto::Hashing
