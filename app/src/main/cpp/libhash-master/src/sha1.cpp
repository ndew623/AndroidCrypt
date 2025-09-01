/*
 *  sha1.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the object SHA1, which implements the Secure
 *      Hashing Algorithm SHA-1 as defined in FIPS 180-4.
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
#include <terra/crypto/hashing/sha1.h>
#include <terra/secutil/secure_erase.h>
#include <terra/bitutil/bit_rotation.h>
#include <terra/bitutil/bit_shift.h>

namespace Terra::Crypto::Hashing
{

namespace
{

// SHA-1 constants defined in FIPS 180-4 section 4.2.1
constexpr std::uint32_t K_t[] =
{
    0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6
};

// Functions defined in FIPS 180-4 section 4.1.1
constexpr std::uint32_t SHA1_Ch(const std::uint32_t x,
                                const std::uint32_t y,
                                const std::uint32_t z)
{
    return (x & y) ^ ((~x) & z);
}
constexpr std::uint32_t SHA1_Parity(const std::uint32_t x,
                                    const std::uint32_t y,
                                    const std::uint32_t z)
{
    return x ^ y ^ z;
}
constexpr std::uint32_t SHA1_Maj(const std::uint32_t x,
                                 const std::uint32_t y,
                                 const std::uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

// Function to help populate the message schedule
constexpr std::uint32_t GetMessageWord(
                            const std::uint8_t message_block[SHA1::Block_Size],
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
 *      W [in]
 *          A pointer to the message schedule array.
 *
 *      a_ [in]
 *          The current representation of working variable "a".
 *
 *      b_ [in/out]
 *          The current representation of working variable "b".
 *
 *      e_ [in/out]
 *          The current representation of working variable "e".
 *
 *      f [in]
 *          The value of f(c, d, d) as defined in Section 4.1.1 of FIPS 180-4.
 *
 *      k [in]
 *          The value of K[t].
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
                     std::uint32_t W[80],
                     const std::uint32_t a_,
                     std::uint32_t &b_,
                     std::uint32_t &e_,
                     const std::uint32_t f,
                     const std::uint32_t k)
{
    if (t > 15)
    {
        W[t] = BitUtil::RotateLeft(W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16],
                                   1);
    }

    e_ = BitUtil::RotateLeft(a_, 5) + f + e_ + k + W[t];

    b_ = BitUtil::RotateLeft(b_, 30);
}

} // namespace

/*
 *  SHA1::SHA1()
 *
 *  Description:
 *      This is a constructor for the SHA1 object.
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
SHA1::SHA1() noexcept :
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
    e{}
{
    // Initialize all data members
    SHA1::Reset();
}

/*
 *  SHA1::SHA1()
 *
 *  Description:
 *      This is a constructor for the SHA1 object.
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
SHA1::SHA1(const std::span<const std::uint8_t> data,
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
    e{}
{
    // Initialize all data members
    SHA1::Reset();

    // Process the input data
    SHA1::Input(data);

    // If asked to finalize the message digest, do it now
    if (auto_finalize) SHA1::Finalize();
}

/*
 *  SHA1::SHA1()
 *
 *  Description:
 *      This is a constructor for the SHA1 object.
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
SHA1::SHA1(const std::string_view data, bool auto_finalize, bool spaces) :
    SHA1(std::span<const std::uint8_t>(
             reinterpret_cast<const std::uint8_t *>(data.data()), data.size()),
         auto_finalize,
         spaces)
{
    // It is assumed that a character is eight bits
    static_assert(CHAR_BIT == 8);
}

/*
 *  SHA1::~SHA1()
 *
 *  Description:
 *      This is the destructor for the SHA1 object.
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
SHA1::~SHA1() noexcept
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
    SecUtil::SecureErase(message_digest, sizeof(message_digest));
}

/*
 *  SHA1::operator==()
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
bool SHA1::operator==(const SHA1 &other) const noexcept
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

    // Compare the message digest values
    if (std::memcmp(message_digest,
                    other.message_digest,
                    sizeof(message_digest)) != 0)
    {
        return false;
    }

    return true;
}

/*
 *  SHA1::operator!=()
 *
 *  Description:
 *      Compare two objects for inequality.
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
bool SHA1::operator!=(const SHA1 &other) const noexcept
{
    return !(*this == other);
}

/*
 *  SHA1::Reset()
 *
 *  Description:
 *      This function will reset the state of the SHA1 object so that it may
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
void SHA1::Reset() noexcept
{
    // Initialize variables as required
    digest_finalized = false;
    corrupted = false;
    input_block_length = 0;
    message_length = 0;

    // Initialize the message digest
    message_digest[0] = 0x67452301;
    message_digest[1] = 0xefcdab89;
    message_digest[2] = 0x98badcfe;
    message_digest[3] = 0x10325476;
    message_digest[4] = 0xc3d2e1f0;
}

/*
 *  SHA1::Input()
 *
 *  Description:
 *      This function is used to feed the SHA-1 hashing algorithm with
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
void SHA1::Input(const std::span<const std::uint8_t> data)
{
    std::size_t consumed = 0;
    std::size_t to_be_consumed = 0;

    // Ensure that the length is less than 2^61 octets
    if (data.size() > Max_Message_Size)
    {
        throw HashException("Input length too long");
    }

    // Ensure the internal data is not corrupted
    if (corrupted) throw HashException("SHA-1 message digest is corrupted");

    // If the message digest has already been computed, throw an exception
    if (digest_finalized)
    {
        throw HashException("SHA-1 message digest already computed");
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

    // The maximum message size is (2^64)-1 bits or (2^62)-1 octets, so check
    // to ensure not too much data was provided as input
    if (message_length > Max_Message_Size)
    {
        corrupted = true;
        throw HashException("SHA-1 message size exceeded");
    }
}

/*
 *  SHA1::Input()
 *
 *  Description:
 *      This function is used to feed the SHA-1 hashing algorithm with
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
void SHA1::Input(const std::string_view data)
{
    // It is assumed that a character is eight bits
    static_assert(CHAR_BIT == 8);

    // Provide the data to the Input function
    Input(std::span<const std::uint8_t>{
                    reinterpret_cast<const std::uint8_t *>(data.data()),
                    data.size() });
}

/*
 *  SHA1::ProcessMessageBlock()
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
 *      specified here are defined in FIPS 180-4 section 6.1.2.
 */
void SHA1::ProcessMessageBlock(const std::uint8_t message_block[Block_Size])
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

    // The balance of W[] will be populated as needed in Step 3

    // STEP 2

    // Initialize the working variables
    a = message_digest[0];
    b = message_digest[1];
    c = message_digest[2];
    d = message_digest[3];
    e = message_digest[4];

    // STEP 3

    Step3( 0, W, a, b, e, SHA1_Ch(b, c, d),     K_t[0]);
    Step3( 1, W, e, a, d, SHA1_Ch(a, b, c),     K_t[0]);
    Step3( 2, W, d, e, c, SHA1_Ch(e, a, b),     K_t[0]);
    Step3( 3, W, c, d, b, SHA1_Ch(d, e, a),     K_t[0]);
    Step3( 4, W, b, c, a, SHA1_Ch(c, d, e),     K_t[0]);
    Step3( 5, W, a, b, e, SHA1_Ch(b, c, d),     K_t[0]);
    Step3( 6, W, e, a, d, SHA1_Ch(a, b, c),     K_t[0]);
    Step3( 7, W, d, e, c, SHA1_Ch(e, a, b),     K_t[0]);
    Step3( 8, W, c, d, b, SHA1_Ch(d, e, a),     K_t[0]);
    Step3( 9, W, b, c, a, SHA1_Ch(c, d, e),     K_t[0]);
    Step3(10, W, a, b, e, SHA1_Ch(b, c, d),     K_t[0]);
    Step3(11, W, e, a, d, SHA1_Ch(a, b, c),     K_t[0]);
    Step3(12, W, d, e, c, SHA1_Ch(e, a, b),     K_t[0]);
    Step3(13, W, c, d, b, SHA1_Ch(d, e, a),     K_t[0]);
    Step3(14, W, b, c, a, SHA1_Ch(c, d, e),     K_t[0]);
    Step3(15, W, a, b, e, SHA1_Ch(b, c, d),     K_t[0]);
    Step3(16, W, e, a, d, SHA1_Ch(a, b, c),     K_t[0]);
    Step3(17, W, d, e, c, SHA1_Ch(e, a, b),     K_t[0]);
    Step3(18, W, c, d, b, SHA1_Ch(d, e, a),     K_t[0]);
    Step3(19, W, b, c, a, SHA1_Ch(c, d, e),     K_t[0]);
    Step3(20, W, a, b, e, SHA1_Parity(b, c, d), K_t[1]);
    Step3(21, W, e, a, d, SHA1_Parity(a, b, c), K_t[1]);
    Step3(22, W, d, e, c, SHA1_Parity(e, a, b), K_t[1]);
    Step3(23, W, c, d, b, SHA1_Parity(d, e, a), K_t[1]);
    Step3(24, W, b, c, a, SHA1_Parity(c, d, e), K_t[1]);
    Step3(25, W, a, b, e, SHA1_Parity(b, c, d), K_t[1]);
    Step3(26, W, e, a, d, SHA1_Parity(a, b, c), K_t[1]);
    Step3(27, W, d, e, c, SHA1_Parity(e, a, b), K_t[1]);
    Step3(28, W, c, d, b, SHA1_Parity(d, e, a), K_t[1]);
    Step3(29, W, b, c, a, SHA1_Parity(c, d, e), K_t[1]);
    Step3(30, W, a, b, e, SHA1_Parity(b, c, d), K_t[1]);
    Step3(31, W, e, a, d, SHA1_Parity(a, b, c), K_t[1]);
    Step3(32, W, d, e, c, SHA1_Parity(e, a, b), K_t[1]);
    Step3(33, W, c, d, b, SHA1_Parity(d, e, a), K_t[1]);
    Step3(34, W, b, c, a, SHA1_Parity(c, d, e), K_t[1]);
    Step3(35, W, a, b, e, SHA1_Parity(b, c, d), K_t[1]);
    Step3(36, W, e, a, d, SHA1_Parity(a, b, c), K_t[1]);
    Step3(37, W, d, e, c, SHA1_Parity(e, a, b), K_t[1]);
    Step3(38, W, c, d, b, SHA1_Parity(d, e, a), K_t[1]);
    Step3(39, W, b, c, a, SHA1_Parity(c, d, e), K_t[1]);
    Step3(40, W, a, b, e, SHA1_Maj(b, c, d),    K_t[2]);
    Step3(41, W, e, a, d, SHA1_Maj(a, b, c),    K_t[2]);
    Step3(42, W, d, e, c, SHA1_Maj(e, a, b),    K_t[2]);
    Step3(43, W, c, d, b, SHA1_Maj(d, e, a),    K_t[2]);
    Step3(44, W, b, c, a, SHA1_Maj(c, d, e),    K_t[2]);
    Step3(45, W, a, b, e, SHA1_Maj(b, c, d),    K_t[2]);
    Step3(46, W, e, a, d, SHA1_Maj(a, b, c),    K_t[2]);
    Step3(47, W, d, e, c, SHA1_Maj(e, a, b),    K_t[2]);
    Step3(48, W, c, d, b, SHA1_Maj(d, e, a),    K_t[2]);
    Step3(49, W, b, c, a, SHA1_Maj(c, d, e),    K_t[2]);
    Step3(50, W, a, b, e, SHA1_Maj(b, c, d),    K_t[2]);
    Step3(51, W, e, a, d, SHA1_Maj(a, b, c),    K_t[2]);
    Step3(52, W, d, e, c, SHA1_Maj(e, a, b),    K_t[2]);
    Step3(53, W, c, d, b, SHA1_Maj(d, e, a),    K_t[2]);
    Step3(54, W, b, c, a, SHA1_Maj(c, d, e),    K_t[2]);
    Step3(55, W, a, b, e, SHA1_Maj(b, c, d),    K_t[2]);
    Step3(56, W, e, a, d, SHA1_Maj(a, b, c),    K_t[2]);
    Step3(57, W, d, e, c, SHA1_Maj(e, a, b),    K_t[2]);
    Step3(58, W, c, d, b, SHA1_Maj(d, e, a),    K_t[2]);
    Step3(59, W, b, c, a, SHA1_Maj(c, d, e),    K_t[2]);
    Step3(60, W, a, b, e, SHA1_Parity(b, c, d), K_t[3]);
    Step3(61, W, e, a, d, SHA1_Parity(a, b, c), K_t[3]);
    Step3(62, W, d, e, c, SHA1_Parity(e, a, b), K_t[3]);
    Step3(63, W, c, d, b, SHA1_Parity(d, e, a), K_t[3]);
    Step3(64, W, b, c, a, SHA1_Parity(c, d, e), K_t[3]);
    Step3(65, W, a, b, e, SHA1_Parity(b, c, d), K_t[3]);
    Step3(66, W, e, a, d, SHA1_Parity(a, b, c), K_t[3]);
    Step3(67, W, d, e, c, SHA1_Parity(e, a, b), K_t[3]);
    Step3(68, W, c, d, b, SHA1_Parity(d, e, a), K_t[3]);
    Step3(69, W, b, c, a, SHA1_Parity(c, d, e), K_t[3]);
    Step3(70, W, a, b, e, SHA1_Parity(b, c, d), K_t[3]);
    Step3(71, W, e, a, d, SHA1_Parity(a, b, c), K_t[3]);
    Step3(72, W, d, e, c, SHA1_Parity(e, a, b), K_t[3]);
    Step3(73, W, c, d, b, SHA1_Parity(d, e, a), K_t[3]);
    Step3(74, W, b, c, a, SHA1_Parity(c, d, e), K_t[3]);
    Step3(75, W, a, b, e, SHA1_Parity(b, c, d), K_t[3]);
    Step3(76, W, e, a, d, SHA1_Parity(a, b, c), K_t[3]);
    Step3(77, W, d, e, c, SHA1_Parity(e, a, b), K_t[3]);
    Step3(78, W, c, d, b, SHA1_Parity(d, e, a), K_t[3]);
    Step3(79, W, b, c, a, SHA1_Parity(c, d, e), K_t[3]);

    // STEP 4

    // Compute the i-th intermediate hash value
    message_digest[0] += a;
    message_digest[1] += b;
    message_digest[2] += c;
    message_digest[3] += d;
    message_digest[4] += e;
}

/*
 *  SHA1::Finalize()
 *
 *  Description:
 *      This function will finalize the SHA-1 message digest computation.
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
void SHA1::Finalize()
{
    // Return if the message digest has already been finalized or is corrupt
    if (digest_finalized || corrupted) return;

    // Pad the message per FIPS 180-4 section 5.1.1
    PadMessage();

    // At this point, the message digest will have been computed
    digest_finalized = true;
}

/*
 *  SHA1::PadMessage()
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
void SHA1::PadMessage()
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
 *  SHA1::Result()
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
std::string SHA1::Result() const
{
    std::ostringstream oss;

    // Ensure the internal data is not corrupted
    if (corrupted) throw HashException("SHA-1 message digest is corrupted");

    // Ensure the message digest computation has been finalized
    if (!digest_finalized)
    {
        throw HashException("SHA-1 message digest has not been finalized");
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
 *  SHA1::Result()
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
std::span<std::uint8_t> SHA1::Result(std::span<std::uint8_t> result) const
{
    // Ensure the internal data is not corrupted
    if (corrupted) throw HashException("SHA-1 message digest is corrupted");

    // Ensure the message digest computation has been finalized
    if (!digest_finalized)
    {
        throw HashException("SHA-1 message digest has not been finalized");
    }

    // Ensure the span has sufficient space
    if (result.size() < Digest_Octet_Count)
    {
        throw HashException("SHA-1 result input span is too short");
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
 *  SHA1::Result()
 *
 *  Description:
 *      This function will return the message digest result in an array of
 *      32-bit integers passed by reference.  Finalize() must have been called
 *      previously, else an exception is thrown.
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
SHA1ResultWordSpan SHA1::Result(SHA1ResultWordSpan result) const
{
    // Ensure the internal data is not corrupted
    if (corrupted) throw HashException("SHA-1 message digest is corrupted");

    // Ensure the message digest computation has been finalized
    if (!digest_finalized)
    {
        throw HashException("SHA-1 message digest has not been finalized");
    }

    // Ensure the span has sufficient space
    if (result.size() < Digest_Word_Count)
    {
        throw HashException("SHA-1 result input span is too short");
    }

    // Place the message digest into the result vector
    std::copy_n(message_digest, Digest_Word_Count, result.data());

    return result.first(Digest_Word_Count);
}

/*
 *  SHA1::GetMessageLength()
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
std::uint64_t SHA1::GetMessageLength() const noexcept
{
    return message_length;
}

} // namespace Terra::Crypto::Hashing
