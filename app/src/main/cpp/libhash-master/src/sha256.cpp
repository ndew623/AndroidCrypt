/*
 *  sha256.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the object SHA256, which implements the Secure
 *      Hashing Algorithm SHA-256 as defined in FIPS 180-4.
 *
 *      Note that while FIPS 180-4 specifies that a message may be any number
 *      of bits in length from 0..(2^64)-1, this object will only operate on
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
#include <terra/crypto/hashing/sha256.h>
#include <terra/secutil/secure_erase.h>
#include <terra/bitutil/bit_rotation.h>
#include <terra/bitutil/bit_shift.h>

namespace Terra::Crypto::Hashing
{

namespace
{

// SHA-256 constants defined in FIPS 180-4 section 4.2.2
constexpr std::uint32_t K_t[] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// Functions defined in FIPS 180-4 section 4.1.2
constexpr std::uint32_t SHA256_Ch(const std::uint32_t x,
                                  const std::uint32_t y,
                                  const std::uint32_t z)
{
    return (x & y) ^ ((~x) & z);
}
constexpr std::uint32_t SHA256_Maj(const std::uint32_t x,
                                   const std::uint32_t y,
                                   const std::uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}
constexpr std::uint32_t SHA256_SIGMA_0(const std::uint32_t x)
{
    return BitUtil::RotateRight(x, 2) ^ BitUtil::RotateRight(x, 13) ^
           BitUtil::RotateRight(x, 22);
}
constexpr std::uint32_t SHA256_SIGMA_1(const std::uint32_t x)
{
    return BitUtil::RotateRight(x, 6) ^ BitUtil::RotateRight(x, 11) ^
           BitUtil::RotateRight(x, 25);
}
constexpr std::uint32_t SHA256_sigma_0(const std::uint32_t x)
{
    return BitUtil::RotateRight(x, 7) ^ BitUtil::RotateRight(x, 18) ^
           BitUtil::ShiftRight(x, 3);
}
constexpr std::uint32_t SHA256_sigma_1(const std::uint32_t x)
{
    return BitUtil::RotateRight(x, 17) ^ BitUtil::RotateRight(x, 19) ^
           BitUtil::ShiftRight(x, 10);
}

// Function to help populate the message schedule
constexpr std::uint32_t GetMessageWord(
                        const std::uint8_t message_block[SHA256::Block_Size],
                        const std::size_t index)
{
    return (static_cast<std::uint32_t>(message_block[index    ]) << 24) |
           (static_cast<std::uint32_t>(message_block[index + 1]) << 16) |
           (static_cast<std::uint32_t>(message_block[index + 2]) <<  8) |
           (static_cast<std::uint32_t>(message_block[index + 3]));
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
                     std::uint32_t W[64],
                     std::uint32_t &T,
                     const std::uint32_t a_,
                     const std::uint32_t b_,
                     const std::uint32_t c_,
                     std::uint32_t &d_,
                     const std::uint32_t e_,
                     const std::uint32_t f_,
                     const std::uint32_t g_,
                     std::uint32_t &h_)
{
    if (t > 15)
    {
        W[t] = SHA256_sigma_1(W[t - 2]) + W[t - 7] +
               SHA256_sigma_0(W[t - 15]) +  W[t - 16];
    }

    T = (h_) + SHA256_SIGMA_1(e_) + SHA256_Ch(e_, f_, g_) + K_t[t] + W[t];

    d_ += T;

    h_ = T + SHA256_SIGMA_0(a_) + SHA256_Maj(a_, b_, c_);
}

} // namespace

/*
 *  SHA256::SHA256()
 *
 *  Description:
 *      This is a constructor for the SHA256 object.
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
SHA256::SHA256() noexcept :
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
    SHA256::Reset();
}

/*
 *  SHA256::SHA256()
 *
 *  Description:
 *      This is a constructor for the SHA256 object.
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
SHA256::SHA256(const std::span<const std::uint8_t> data,
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
    SHA256::Reset();

    // Process the input data
    SHA256::Input(data);

    // If asked to finalize the message digest, do it now
    if (auto_finalize) SHA256::Finalize();
}

/*
 *  SHA256::SHA256()
 *
 *  Description:
 *      This is a constructor for the SHA256 object.
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
SHA256::SHA256(const std::string_view data, bool auto_finalize, bool spaces) :
    SHA256(std::span<const std::uint8_t>(
             reinterpret_cast<const std::uint8_t *>(data.data()), data.size()),
           auto_finalize,
           spaces)
{
    // It is assumed that a character is eight bits
    static_assert(CHAR_BIT == 8);
}

/*
 *  SHA256::~SHA256()
 *
 *  Description:
 *      This is the destructor for the SHA256 object.
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
SHA256::~SHA256() noexcept
{
    // For security reasons, zero all internal data
    SecUtil::SecureErase(input_block, sizeof(input_block));
    SecUtil::SecureErase(input_block_length);
    SecUtil::SecureErase(message_length);
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
 *  SHA256::operator==()
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
bool SHA256::operator==(const SHA256 &other) const noexcept
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
 *  SHA256::operator!=()
 *
 *  Description:
 *      Compare two SHA256 objects for inequality.
 *
 *  Parameters:
 *      other [in]
 *          The other SHA256 object with which to compare.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
bool SHA256::operator!=(const SHA256 &other) const noexcept
{
    return !(*this == other);
}

/*
 *  SHA256::Reset()
 *
 *  Description:
 *      This function will reset the state of the SHA256 object so that it may
 *      be reused to compute a new hash value.
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
void SHA256::Reset() noexcept
{
    // Initialize variables as required
    digest_finalized = false;
    corrupted = false;
    input_block_length = 0;
    message_length = 0;

    // Initialize the message digest
    message_digest[0] = 0x6a09e667;
    message_digest[1] = 0xbb67ae85;
    message_digest[2] = 0x3c6ef372;
    message_digest[3] = 0xa54ff53a;
    message_digest[4] = 0x510e527f;
    message_digest[5] = 0x9b05688c;
    message_digest[6] = 0x1f83d9ab;
    message_digest[7] = 0x5be0cd19;
}

/*
 *  SHA256::Input()
 *
 *  Description:
 *      This function is used to feed the SHA-256 hashing algorithm with
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
 *      length of (2^64)-1 bits.
 */
void SHA256::Input(const std::span<const std::uint8_t> data)
{
    std::size_t consumed = 0;
    std::size_t to_be_consumed = 0;

    // Ensure that the length is less tha 2^61 octets
    if (data.size() > Max_Message_Size)
    {
        throw HashException("Input length too long");
    }

    // Ensure the internal data is not corrupted
    if (corrupted) throw HashException("SHA-256 message digest is corrupted");

    // If the message digest has already been computed, throw an exception
    if (digest_finalized)
    {
        throw HashException("SHA-256 message digest already computed");
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

    // The maximum message size is (2^64)-1 bits, so check to ensure not too
    // much data was provided as input
    if (message_length > Max_Message_Size)
    {
        corrupted = true;
        throw HashException("SHA-256 message size exceeded");
    }
}

/*
 *  SHA256::Input()
 *
 *  Description:
 *      This function is used to feed the SHA-256 hashing algorithm with
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
 *      length of (2^64)-1 bits.
 */
void SHA256::Input(const std::string_view data)
{
    // It is assumed that a character is eight bits
    static_assert(CHAR_BIT == 8);

    // Provide the data to the Input function
    Input(std::span<const std::uint8_t>{
                    reinterpret_cast<const std::uint8_t *>(data.data()),
                    data.size() });
}

/*
 *  SHA256::ProcessMessageBlock()
 *
 *  Description:
 *      This function will consume a 64-octet (512-bit) message block,
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
 *      specified here are defined in FIPS 180-4 section 6.2.2.
 */
void SHA256::ProcessMessageBlock(const std::uint8_t message_block[Block_Size])
{
    // STEP 1

    // Break the message block into words, filling part of the message schedule
    W[ 0] = GetMessageWord(message_block,  0);
    W[ 1] = GetMessageWord(message_block,  4);
    W[ 2] = GetMessageWord(message_block,  8);
    W[ 3] = GetMessageWord(message_block, 12);
    W[ 4] = GetMessageWord(message_block, 16);
    W[ 5] = GetMessageWord(message_block, 20);
    W[ 6] = GetMessageWord(message_block, 24);
    W[ 7] = GetMessageWord(message_block, 28);
    W[ 8] = GetMessageWord(message_block, 32);
    W[ 9] = GetMessageWord(message_block, 36);
    W[10] = GetMessageWord(message_block, 40);
    W[11] = GetMessageWord(message_block, 44);
    W[12] = GetMessageWord(message_block, 48);
    W[13] = GetMessageWord(message_block, 52);
    W[14] = GetMessageWord(message_block, 56);
    W[15] = GetMessageWord(message_block, 60);

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
 *  SHA256::Finalize()
 *
 *  Description:
 *      This function will finalize the SHA-256 message digest computation.
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
void SHA256::Finalize()
{
    // Return if the message digest has already been finalized or is corrupt
    if (digest_finalized || corrupted) return;

    // Pad the message per FIPS 180-4 section 5.1.1
    PadMessage();

    // At this point, the message digest will have been computed
    digest_finalized = true;
}

/*
 *  SHA256::PadMessage()
 *
 *  Description:
 *      This function will pad the message as per FIPS 180-4 section 5.1.1
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
void SHA256::PadMessage()
{
    // Append 0x80 to the end of the message
    input_block[input_block_length++] = 0x80;

    // Pad out to a full input block if we have more than 448 bits (56 octets)
    if (input_block_length > 56)
    {
        // Pad the input block with zeros
        std::memset(input_block + input_block_length,
                    0,
                    64 - input_block_length);

        // The input block is now 64 octets, but that will be reset below

        // Process the current input block
        ProcessMessageBlock(input_block);

        // Reset the input block length to zero
        input_block_length = 0;
    }

    // Pad up to 448 bits (56 octets)
    if (input_block_length < 56)
    {
        std::memset(input_block + input_block_length,
                    0,
                    56 - input_block_length);
    }

    // The final 64 bits contain the message length (convert length to bits)
    std::uint64_t length = message_length << 3;
    for (std::size_t i = 63; i > 55; i--)
    {
        input_block[i] = (length & 0xff);
        length >>= 8;
    }

    // Process the current input block
    ProcessMessageBlock(input_block);

    // Reset the input block length to zero
    input_block_length = 0;
}

/*
 *  SHA256::Result()
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
std::string SHA256::Result() const
{
    std::ostringstream oss;

    // Ensure the internal data is not corrupted
    if (corrupted) throw HashException("SHA-256 message digest is corrupted");

    // Ensure the message digest computation has been finalized
    if (!digest_finalized)
    {
        throw HashException("SHA-256 message digest has not been finalized");
    }

    oss << std::hex << std::setfill('0');

    for (std::size_t i = 0; i < Digest_Word_Count; i++)
    {
        if (space_separate_words && (i > 0)) oss << " ";
        oss << std::setw(8) << message_digest[i];
    }

    return oss.str();
}

/*
 *  SHA256::Result()
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
std::span<std::uint8_t> SHA256::Result(std::span<std::uint8_t> result) const
{
    // Ensure the internal data is not corrupted
    if (corrupted) throw HashException("SHA-256 message digest is corrupted");

    // Ensure the message digest computation has been finalized
    if (!digest_finalized)
    {
        throw HashException("SHA-256 message digest has not been finalized");
    }

    // Ensure the span has sufficient space
    if (result.size() < Digest_Octet_Count)
    {
        throw HashException("SHA-256 result input span is too short");
    }

    for (std::size_t i = 0, j = 0; i < Digest_Word_Count; i++, j += 4)
    {
        result[j    ] = ((message_digest[i] >> 24) & 0xff);
        result[j + 1] = ((message_digest[i] >> 16) & 0xff);
        result[j + 2] = ((message_digest[i] >>  8) & 0xff);
        result[j + 3] = ((message_digest[i]      ) & 0xff);
    }

    return result.first(Digest_Octet_Count);
}

/*
 *  SHA256::Result()
 *
 *  Description:
 *      This function will return the message digest result in a span of
 *      32-bit integers.  Finalize() must have been called previously, else
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
SHA256ResultWordSpan SHA256::Result(SHA256ResultWordSpan result) const
{
    // Ensure the internal data is not corrupted
    if (corrupted) throw HashException("SHA-256 message digest is corrupted");

    // Ensure the message digest computation has been finalized
    if (!digest_finalized)
    {
        throw HashException("SHA-256 message digest has not been finalized");
    }

    // Ensure the span has sufficient space
    if (result.size() < Digest_Word_Count)
    {
        throw HashException("SHA-256 result input span is too short");
    }

    // Place the message digest into the result vector
    std::copy_n(message_digest, Digest_Word_Count, result.data());

    return result.first(Digest_Word_Count);
}

/*
 *  SHA256::GetMessageLength()
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
std::uint64_t SHA256::GetMessageLength() const noexcept
{
    return message_length;
}

} // namespace Terra::Crypto::Hashing
