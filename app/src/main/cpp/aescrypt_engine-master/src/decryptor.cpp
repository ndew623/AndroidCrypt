/*
 *  decryptor.cpp
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the Decryptor object that is a part of the
 *      AES Crypt Engine.  This object is responsible for decrypting
 *      AES Crypt streams from a specified source, outputting the plaintext to
 *      the specified destination stream.
 *
 *  Portability Issues:
 *      None.
 */

#include <climits>
#include <algorithm>
#include <array>
#include <terra/aescrypt/engine/decryptor.h>
#include <terra/secutil/secure_erase.h>
#include <terra/secutil/secure_vector.h>
#include <terra/secutil/secure_array.h>
#include <terra/charutil/character_utilities.h>
#include <terra/crypto/kdf/pbkdf.h>
#include <terra/crypto/hashing/hmac.h>
#include <terra/crypto/cipher/aes.h>
#include "engine_common.h"

// It is assumed a character is 8 bits; assumption used stream I/O
static_assert(CHAR_BIT == (1 << 3), "Characters are assumed to be 8 bits");

namespace Terra::AESCrypt::Engine
{

/*
 *  operator<<()
 *
 *  Description:
 *      Streaming operator for the DecryptResult to print text strings for
 *      error values to make it easier in logging and testing.
 *
 *  Parameters:
 *      o [in/out]
 *          The stream object to which output is directed.
 *
 *      result [in]
 *          The result code to transform into human-readable text.
 *
 *  Returns:
 *      The streaming object "o" passed as the first input parameter.
 *
 *  Comments:
 *      None.
 */
std::ostream &operator<<(std::ostream &o, const DecryptResult result)
{
    switch (result)
    {
        case DecryptResult::Success:
            o << "Success";
            break;

        case DecryptResult::InvalidAESCryptStream:
            o << "Invalid AES Crypt stream";
            break;

        case DecryptResult::UnsupportedAESCryptVersion:
            o << "Unsupported version";
            break;

        case DecryptResult::IOError:
            o << "I/O error";
            break;

        case DecryptResult::InvalidPassword:
            o << "Invalid password provided";
            break;

        case DecryptResult::InvalidIterations:
            o << "Invalid iterations value";
            break;

        case DecryptResult::AlteredMessage:
            o << "Message has been altered or password is incorrect";
            break;

        case DecryptResult::AlreadyDecrypting:
            o << "Another thread is already decrypting";
            break;

        case DecryptResult::DecryptionCancelled:
            o << "Decryption operation was cancelled";
            break;

        case DecryptResult::InternalError:
            o << "Internal error";
            break;

        default:
            o << "Unknown result";
            break;
    }

    return o;
}

/*
 *  Decryptor::Decryptor()
 *
 *  Description:
 *      Constructor for the Decryptor object.
 *
 *  Parameters:
 *      parent_logger [in]
 *          A shared pointer to a parent Logger object.  If this is a nullptr,
 *          then a default parent will be created that produces no output.
 *
 *      instance [in]
 *          An optional instance string to help differentiate between multiple
 *          instances of the Decryptor object (useful for logging and when
 *          using the same progress callback function).
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Decryptor::Decryptor(Logger::LoggerPointer parent_logger,
                     const std::string &instance) :
    logger{std::make_shared<Logger::Logger>(std::move(parent_logger),
                                            CreateComponent("DEC", instance))},
    instance{instance},
    active{false},
    cancelled{false},
    stream_version{},
    reserved_modulo{},
    octets_consumed{},
    progress_octets{}
{
}

/*
 *  Decryptor::~Decryptor()
 *
 *  Description:
 *      Destructor for the Decryptor object.  This will ensure that any member
 *      variables that might hold sensitive information are securely erased.
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
Decryptor::~Decryptor()
{
    SecUtil::SecureErase(stream_version);
    SecUtil::SecureErase(reserved_modulo);
    SecUtil::SecureErase(octets_consumed);
    SecUtil::SecureErase(progress_octets);
}

/*
 *  Decryptor::Decrypt()
 *
 *  Description:
 *      This function will decrypt the specified input source using the provided
 *      UTF-8-encoded password, directing output to the specified output stream.
 *
 *  Parameters:
 *      password [in]
 *          The password used to decrypt the AES Crypt stream.  The password
 *          is limited to less than 1/2 the size of std::size_t, just to ensure
 *          any character conversions can be performed as required.
 *
 *      source [in]
 *          The input stream from which to read the AES Crypt stream.
 *
 *      destination [out]
 *          The destination stream to which the output is to be written.
 *
 *      progress_callback [in]
 *          Optional callback function that will be called to deliver progress
 *          updates.
 *
 *      progress_interval [in]
 *          The number of octets to read before placing a callback to deliver
 *          a progress update.  If this is zero (default), no progress updates
 *          will be delivered.  This number should be a reasonable size, well
 *          above the 16-octet AES block size, as no callbacks will be
 *          delivered more frequently. Ideally, callbacks would report no
 *          more frequently than every 1% of stream consumption.  If the
 *          size of the content is unknown, providing process updates is
 *          probably not useful and just slows processing.
 *
 *  Returns:
 *      The result of the decryption process.
 *
 *  Comments:
 *      None.
 */
DecryptResult Decryptor::Decrypt(const std::u8string &password,
                                 std::istream &source,
                                 std::ostream &destination,
                                 const ProgressCallback &progress_callback,
                                 const std::size_t progress_interval)
{
    SecUtil::SecureArray<std::uint8_t, 16> iv;
    SecUtil::SecureArray<std::uint8_t, 32> key;
    DecryptResult result{};

    logger->Log("Decrypting the AES Crypt stream");

    // Ensure the password has at least one octet
    if (password.empty())
    {
        logger->error << "Password is an empty string" << std::flush;
        return DecryptResult::InvalidPassword;
    }

    // Ensure the password is not too long
    if (password.size() > (std::numeric_limits<std::size_t>::max() / 2))
    {
        logger->error << "Password is too long" << std::flush;
        return DecryptResult::InvalidPassword;
    }

    // Ensure the UTF-8 sequence is valid
    if (!CharUtil::IsUTF8Valid(
            {reinterpret_cast<const std::uint8_t *>(password.data()),
             password.size()}))
    {
        logger->error << "Password is not a valid UTF-8 sequence" << std::flush;
        return DecryptResult::InvalidPassword;
    }

    // Ensure the source and destination streams are in a good state
    if (!source.good())
    {
        logger->error << "Source stream is not in a good state" << std::flush;
        return DecryptResult::IOError;
    }
    if (!destination.good())
    {
        logger->error << "Destination stream is not in a good state"
                      << std::flush;
        return DecryptResult::IOError;
    }

    // Indicate that decryption is starting
    result = BeginDecrypting();
    if (result != DecryptResult::Success) return result;

    // Read the AES Crypt header, return version number
    result = DetermineVersion(source);
    if (result != DecryptResult::Success)
    {
        FinishedDecrypting();
        return result;
    }

    // Read the reserved octet (this is the modulo octet in version 0)
    result = ReadOctets(source, {&reserved_modulo, 1});
    if (result != DecryptResult::Success)
    {
        FinishedDecrypting();
        return result;
    }

    // Consume (and discard) extension data
    result = ConsumeExtensions(source);
    if (result != DecryptResult::Success)
    {
        logger->error << "Error reading extension data" << std::flush;
        FinishedDecrypting();
        return result;
    }

    // If this stream is >= version 3, read the iterations value
    std::uint32_t kdf_iterations = 0;
    if (stream_version >= 3)
    {
        std::array<std::uint8_t, 4> iterations{};
        result = ReadOctets(source, iterations);
        if (result != DecryptResult::Success)
        {
            logger->error << "Unable to read iterations value" << std::flush;
            FinishedDecrypting();
            return result;
        }
        kdf_iterations = (static_cast<std::uint32_t>(iterations[0]) << 24) |
                         (static_cast<std::uint32_t>(iterations[1]) << 16) |
                         (static_cast<std::uint32_t>(iterations[2]) <<  8) |
                         (static_cast<std::uint32_t>(iterations[3])      );
        if ((kdf_iterations < PBKDF2_Min_Iterations) ||
            (kdf_iterations > PBKDF2_Max_Iterations))
        {
            logger->error << "Invalid KDF iterations value" << std::flush;
            return DecryptResult::InvalidIterations;
        }
    }

    // Read the public IV
    result = ReadOctets(source, iv);
    if (result != DecryptResult::Success)
    {
        logger->error << "Unable to read the public IV" << std::flush;
        FinishedDecrypting();
        return result;
    }

    // Derive the encryption key given the user's password
    result = DeriveKey(password, kdf_iterations, iv, key);
    if (result != DecryptResult::Success)
    {
        logger->error << "Unable to derive encryption key" << std::flush;
        FinishedDecrypting();
        return result;
    }

    // Decrypt the session key
    result = GetSessionKey(source, iv, key);
    if (result != DecryptResult::Success)
    {
        logger->error << "Unable to decrypt session key" << std::flush;
        FinishedDecrypting();
        return result;
    }

    // Decrypt the ciphertext, outputting the plaintext to destination
    result = DecryptStream(source,
                           destination,
                           iv,
                           key,
                           progress_callback,
                           progress_interval);
    if (result != DecryptResult::Success)
    {
        if (result != DecryptResult::DecryptionCancelled)
        {
            logger->error << "Failed to decrypt stream: " << result
                          << std::flush;
        }
        FinishedDecrypting();
        return result;
    }

    // Indicate that a thread is no longer active
    FinishedDecrypting();

    logger->Log("Finished decrypting the ciphertext stream");

    return DecryptResult::Success;
}

/*
 *  Decryptor::Cancel()
 *
 *  Description:
 *      This function is called by a separate thread of execution to request
 *      an ongoing Decrypt() call to terminate.  Calling this function may not
 *      result in immediate termination of the ongoing decryption process, but
 *      it will terminate at the next opportunity to check the termination flag.
 *      The calling thread will be blocked until the decrypting thread
 *      indicates it will cancel operation.
 *
 *      Once this function is called, the object remains in a "cancelled" state
 *      until Activate() is subsequently called.
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
void Decryptor::Cancel()
{
    std::unique_lock<std::mutex> lock(decryptor_mutex);

    // Indicate that the decryption thread should stop
    cancelled = true;

    // If no longer decrypting, return
    if (!active) return;

    // Wait for the decrypting thread to complete
    cv.wait(lock, [&]() -> bool { return !active; });
}

/*
 *  Decryptor::Activate()
 *
 *  Description:
 *      This function is called to reactive an object that was previously
 *      placed in a cancelled state.  Without calling this function, subsequent
 *      attempts to use this object would fail with the reason of
 *      DecryptResult::DecryptionCancelled.  If a thread is currently
 *      decrypting, then this function returns false.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      True if the object is reactivated and false if there is an error.  The
 *      only error that might occur is trying to reactivate an object that is
 *      currently decrypting a stream.
 *
 *  Comments:
 *      None.
 */
bool Decryptor::Activate()
{
    std::lock_guard<std::mutex> lock(decryptor_mutex);

    // If there is an active decryption thread, fail
    if (active) return false;

    // Set the cancelled flag to false
    cancelled = false;

    return true;
}

/*
 *  Decryptor::BeginDecrypting()
 *
 *  Description:
 *      This call is made when initiating the decryption process to ensure
 *      that another thread hasn't already cancelled the thread used to do
 *      decryption.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A status code indicating success or failure.
 *
 *  Comments:
 *      None.
 */
DecryptResult Decryptor::BeginDecrypting()
{
    std::lock_guard<std::mutex> lock(decryptor_mutex);

    // If the decryption object is in a cancelled state, just return
    if (cancelled) return DecryptResult::DecryptionCancelled;

    // If already decrypting, return an error response
    if (active) return DecryptResult::AlreadyDecrypting;

    // Indicate that the thread is decrypting
    active = true;

    // Reset the octet count
    octets_consumed = 0;

    return DecryptResult::Success;
}

/*
 *  Decryptor::FinishedDecrypting()
 *
 *  Description:
 *      This function is called when the decryption thread has completed work
 *      or recognizes that the request was cancelled.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      This should only be called by the thread currently decrypting.
 */
void Decryptor::FinishedDecrypting()
{
    std::lock_guard<std::mutex> lock(decryptor_mutex);

    // Indicate that the thread is no longer decrypting
    active = false;

    // Notify any awaiting thread that decryption has stopped
    cv.notify_all();
}

/*
 *  Decrypt::DetermineVersion()
 *
 *  Description:
 *      This function will read from the input source to determine the version
 *      of the AES Crypt stream.  Upon successful return, the input stream
 *      will be positioned to read data immediately following the identifier.
 *
 *  Parameters:
 *      source [in]
 *          The input stream from which to read the AES Crypt stream.
 *
 *  Returns:
 *      The result of trying to determine the AES Crypt stream version.  If
 *      successful, the member variable stream_version will contain the version
 *      of the AES Crypt data found in the stream.
 *
 *  Comments:
 *      None.
 */
DecryptResult Decryptor::DetermineVersion(std::istream &source)
{
    // Buffer for "AES" and version (initialized to zero)
    std::array<std::uint8_t, 4> version_buffer{};

    // Read the first 4 octets of the stream ("AES" and version number)
    auto result = ReadOctets(source, version_buffer);
    if (result != DecryptResult::Success)
    {
        logger->error << "Unable to determine stream version" << std::flush;
        return result;
    }

    // Ensure the stream header looks sane
    if ((version_buffer[0] != 'A') ||
        (version_buffer[1] != 'E') ||
        (version_buffer[2] != 'S'))
    {
        logger->error << "Not a valid AES Crypt stream header" << std::flush;
        return DecryptResult::InvalidAESCryptStream;
    }

    // Verify that the version number is supported
    stream_version = static_cast<std::uint8_t>(version_buffer[3]);
    if (stream_version > Latest_AES_Crypt_Stream_Version)
    {
        logger->error << "Unsupported AES Crypt format version "
                      << stream_version
                      << std::flush;
        return DecryptResult::UnsupportedAESCryptVersion;
    }

    return DecryptResult::Success;
}

/*
 *  Decryptor::ReadOctets()
 *
 *  Description:
 *      This function will read the specified number of octets from the input
 *      source stream.  This function is used when reading the initial octets
 *      in the stream.  It will assume all reads should succeed and report
 *      an error on failure.  This is not intended for use when reading the
 *      ciphertext since at some point there would be an EOF condition that
 *      is not, in fact, an error.  Reads for the bulk of the ciphertext are
 *      handed in the DecryptStream() for this reason (and for a bit more
 *      performance).
 *
 *  Parameters:
 *      source [in]
 *          The input stream from which to read the AES Crypt stream.
 *
 *      octets [out]
 *          A span into which the octets from the stream are to be placed.
 *          The number of octets to read is determined from the size of the
 *          span.
 *
 *  Returns:
 *      A result of Success if the block was read without error or a non-success
 *      response if there was an error.
 *
 *  Comments:
 *      None.
 */
DecryptResult Decryptor::ReadOctets(std::istream &source,
                                    std::span<std::uint8_t> octets)
{
    // Read octets from the source stream
    source.read(reinterpret_cast<char *>(octets.data()),
                static_cast<std::streamsize>(octets.size()));

    // Ensure there was no error reading from the stream
    if (!source.good())
    {
        // Assume the worst
        DecryptResult result = DecryptResult::IOError;

        // If the end of the stream is reached, assume an invalid stream
        if (source.eof()) result = DecryptResult::InvalidAESCryptStream;

        logger->error << "Failed reading octets: " << result << std::flush;

        return result;
    }

    // Updated the octet count
    octets_consumed += octets.size();

    return DecryptResult::Success;
}

/*
 *  Decrypt::ConsumeExtensions()
 *
 *  Description:
 *      This function will read from the input source to skip over extension
 *      data that might be present in the AES Crypt header.  Since AES Crypt
 *      stream format version 0 did not have extensions, this function will
 *      simply return success if the stream version is 0.
 *
 *  Parameters:
 *      source [in]
 *          The input stream from which to read the AES Crypt stream.
 *
 *  Returns:
 *      DecryptResult::Success is able to skip of header extensions or
 *      a different result code indicating the reason why.
 *
 *  Comments:
 *      None.
 */
DecryptResult Decryptor::ConsumeExtensions(std::istream &source)
{
    std::uint16_t extension_length{};
    std::array<std::uint8_t, 2> buffer{};
    DecryptResult result = DecryptResult::Success;

    // Since stream format version 0 & 1 did not use extensions, just return
    if (stream_version < 2) return DecryptResult::Success;

    // Iterate over the extensions in the stream
    while (true)
    {
        // Read the extension length
        result = ReadOctets(source, buffer);
        if (result != DecryptResult::Success)
        {
            logger->error << "Unable to read extension header" << std::flush;
            break;
        }

        // Put the extension length in host order
        extension_length = (static_cast<std::uint16_t>(buffer[0]) << 8) |
                           (static_cast<std::uint16_t>(buffer[1]));

        // If the extension length is 0, break out of the loop
        if (extension_length == 0) break;

        // Skip over the extension data (seekg() is not an option for stdin)
        source.ignore(extension_length);

        // Ensure there was no error skipping over the extensions
        if (!source.good())
        {
            // Assume the worst
            result = DecryptResult::IOError;

            // If the end of the stream is reached, assume an invalid stream
            if (source.eof()) result = DecryptResult::InvalidAESCryptStream;

            logger->error << "Failed skipping over extension: "
                          << result
                          << std::flush;
            break;
        }
    }

    return result;
}

/*
 *  Decryptor::DeriveKey()
 *
 *  Description:
 *      This function will take the user-provided password and return the key
 *      used to decrypt the subsequent ciphertext.  For AES Crypt streams after
 *      version 0, this key is used only to decrypt a session IV and key.
 *
 *  Parameters:
 *      password [in]
 *          The user-provided password.
 *
 *      kdf_iterations [in]
 *          The number of iterations used by the Key Derivation Function (KDF).
 *          This value must be >= 1 for AES Crypt streams version >= 3.
 *          This value is not used in older versions as the iteration count
 *          was a fixed 8192.
 *
 *      iv [in]
 *          A span containing the public IV from the AES Crypt stream.
 *
 *      key [out]
 *          This will contain the encryption key.  This must be precisely
 *          thirty-two octets (256-bit key).
 *
 *  Returns:
 *      The result value from attempting to prepare the IV and Key.
 *
 *  Comments:
 *      None.
 */
DecryptResult Decryptor::DeriveKey(const std::u8string &password,
                                   const std::uint32_t kdf_iterations,
                                   const std::span<const std::uint8_t, 16> iv,
                                   std::span<std::uint8_t, 32> key)
{
    SecUtil::SecureVector<std::uint8_t> pw;

    logger->info << "Performing key derivation" << std::flush;

    // Convert the password to UTF-16LE for stream versions 0, 1, and 2
    if (stream_version <= 2)
    {
        // Set the buffer to a size large enough to handle the conversion
        pw.resize(password.size() * 2);

        // Convert the UTF-8 string to UTF-16LE
        auto [success, length] = CharUtil::ConvertUTF8ToUTF16(
            {reinterpret_cast<const std::uint8_t *>(password.data()),
             password.size()},
            pw,
            true);

        // If there was a conversion failure, report it and return
        if (!success)
        {
            logger->error << "Failed to convert password to UTF-16LE"
                          << std::flush;
            return DecryptResult::InvalidPassword;
        }

        // Resize the buffer given the converted length
        pw.resize(length);
    }
    else
    {
        // Newer versions of AES Crypt use UTF-8 as the character encoding
        std::copy(password.begin(), password.end(), std::back_inserter(pw));
    }

    try
    {
        // Produce a decryption key using the PBKDF algorithm for this stream
        if (stream_version <= 2)
        {
            // KDF used in AES Crypt formats 0, 1, and 2
            auto kdf_result = Crypto::KDF::ACKDF(pw, iv, key);

            // Verify the key length
            if (kdf_result.size() != key.size())
            {
                logger->error << "Unexpected key length returned from KDF"
                              << std::flush;

                return DecryptResult::InternalError;
            }
        }
        else
        {
            // KDF used in AES Crypt formats 3 and onward
            auto kdf_result = Crypto::KDF::PBKDF2(PBKDF2_Hash_Algorithm,
                                                  pw,
                                                  iv,
                                                  kdf_iterations,
                                                  key);

            // Verify the key length
            if (kdf_result.size() != key.size())
            {
                logger->error << "Unexpected key length returned from KDF"
                              << std::flush;

                return DecryptResult::InternalError;
            }
        }
    }
    catch (const Crypto::KDF::KDFException &e)
    {
        logger->critical << "KDF Exception: " << e.what() << std::flush;
        return DecryptResult::InternalError;
    }

    logger->info << "Key derivation completed successfully" << std::flush;

    return DecryptResult::Success;
}

/*
 *  Decryptor::GetSessionKey()
 *
 *  Description:
 *      For AES Crypt stream version 1 and later, the salt and IV used in
 *      the actual decryption of the ciphertext is stored in encrypted form
 *      in the header of the file.  This function will accept the public IV
 *      and key and return in their place the session IV and key.  The stream
 *      should be positioned right at the start of the ciphertext holding
 *      the session IV and key.  For stream version 0, this function has no
 *      affect on the IV and key.
 *
 *  Parameters:
 *      source [in]
 *          The input stream from which to read the AES Crypt stream.
 *
 *      iv [in/out]
 *          A span containing the public IV as input and will receive the
 *          session IV as output.  This must be precisely sixteen octets in
 *          size.
 *
 *      key [in/out]
 *          This will contain derived key as input and will receive the
 *          session key as output.  This must be precisely thirty-two octets
 *          (256-bit key).
 *
 *  Returns:
 *      The result from attempting to decrypt the session IV and key.
 *
 *  Comments:
 *      None.
 */
DecryptResult Decryptor::GetSessionKey(std::istream &source,
                                       std::span<std::uint8_t, 16> iv,
                                       std::span<std::uint8_t, 32> key)
{
    SecUtil::SecureArray<std::uint8_t, 48> iv_and_key;
    SecUtil::SecureArray<std::uint8_t, 16> plaintext_iv;
    SecUtil::SecureArray<std::uint8_t, 32> computed_hmac;
    SecUtil::SecureArray<std::uint8_t, 32> expected_hmac;
    DecryptResult result{};

    // For AES Crypt stream version 0, there is nothing more to do
    if (stream_version == 0) return DecryptResult::Success;

    try
    {
        // Newer streams use a session key encrypted with the derived key
        Crypto::Cipher::AES aes(key);

        // The encrypted data is protected with an HMAC
        Crypto::Hashing::HMAC hmac(Crypto::Hashing::HashAlgorithm::SHA256, key);
        if (hmac.GetHMACLength() != computed_hmac.size())
        {
            logger->critical << "HMAC length value is incorrect" << std::flush;
            return DecryptResult::InternalError;
        }

        // Copy the IV, as it will be over-written
        std::copy(iv.begin(), iv.end(), plaintext_iv.begin());

        // Read the encrypted IV and key
        result = ReadOctets(source, iv_and_key);
        if (result != DecryptResult::Success)
        {
            logger->error << "Unable to read encrypted IV & key" << std::flush;
            return result;
        }

        // Update the HMAC
        hmac.Input(iv_and_key);

        // Decrypt the first block, storying plaintext in IV
        aes.Decrypt(std::span<std::uint8_t, 16>(iv_and_key.data(), 16), iv);

        // XOR the decrypted text with plaintext IV, storing the result in the
        // IV to be returned to the caller
        XORBlock(iv, plaintext_iv, iv);

        // Decrypt the next block into the first half of the key span
        aes.Decrypt(std::span<std::uint8_t, 16>(iv_and_key.data() + 16, 16),
                    std::span<std::uint8_t, 16>(key.data(), 16));

        // XOR the decrypted text with prior_block
        XORBlock(std::span<std::uint8_t, 16>(key.data(), 16),
                 std::span<std::uint8_t, 16>(iv_and_key.data(), 16),
                 std::span<std::uint8_t, 16>(key.data(), 16));

        // Decrypt the block into the second half of the key span
        aes.Decrypt(std::span<std::uint8_t, 16>(iv_and_key.data() + 32, 16),
                    std::span<std::uint8_t, 16>(key.data() + 16, 16));

        // XOR the decrypted text with prior_block
        XORBlock(std::span<std::uint8_t, 16>(key.data() + 16, 16),
                 std::span<std::uint8_t, 16>(iv_and_key.data() + 16, 16),
                 std::span<std::uint8_t, 16>(key.data() + 16, 16));

        // For version 3 and later, add the version octet to the HMAC
        if (stream_version >= 3) hmac.Input({&stream_version, 1});

        // Finalize the HMAC
        hmac.Finalize();
        hmac.Result(computed_hmac);

        // Read the expected HMAC value from the stream
        result = ReadOctets(source, expected_hmac);
        if (result != DecryptResult::Success)
        {
            logger->error << "Unable to read IV/Key HMAC" << std::flush;
            return result;
        }

        // Compare the computed and expected HMAC
        if (computed_hmac != expected_hmac)
        {
            logger->error << "Message appears to be altered" << std::flush;
            return DecryptResult::AlteredMessage;
        }
    }
    catch (const Crypto::Cipher::AESException &e)
    {
        logger->critical << "AES Exception: " << e.what() << std::flush;
        return DecryptResult::InternalError;
    }
    catch (const Crypto::Hashing::HashException &e)
    {
        logger->critical << "Hash Exception: " << e.what() << std::flush;
        return DecryptResult::InternalError;
    }

    return DecryptResult::Success;
}

/*
 *  DecryptStream()
 *
 *  Description:
 *      This function will decrypt the bulk of the AES Crypt stream.  When
 *      called, the stream should be positioned at the start of the ciphertext.
 *
 *  Parameters:
 *      source [in]
 *          The input stream from which to read the AES Crypt stream.
 *
 *      destination [out]
 *          The destination stream to which the output is to be written.
 *
 *      iv [in]
 *          The initialization vector to use to facilitate use of CBC mode.
 *
 *      key [in]
 *          The symmetric key used in decryption.
 *
 *      progress_callback [in]
 *          Optional callback function that will be called to deliver progress
 *          updates.
 *
 *      progress_interval [in]
 *          The number of octets to read before placing a callback to deliver
 *          a progress update.  If this is zero (default), no progress updates
 *          will be delivered.  This number should be a reasonable size, well
 *          above the 16-octet AES block size, as no callbacks will be
 *          delivered more frequently. Ideally, callbacks would report no
 *          more frequently than every 1% of stream consumption.  If the
 *          size of the content is unknown, providing process updates is
 *          probably not useful and just slows processing.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
DecryptResult Decryptor::DecryptStream(
                                std::istream &source,
                                std::ostream &destination,
                                const std::span<const std::uint8_t, 16> iv,
                                const std::span<const std::uint8_t, 32> key,
                                const ProgressCallback &progress_callback,
                                const std::size_t progress_interval)
{
    SecUtil::SecureArray<std::uint8_t, 64> ring_buffer;
    SecUtil::SecureArray<std::uint8_t, 16> plaintext;
    SecUtil::SecureArray<std::uint8_t, 32> computed_hmac;
    SecUtil::SecureArray<std::uint8_t, 32> expected_hmac;
    std::size_t buffer_octets = 0;
    bool plaintext_to_write = false;
    std::uint8_t *tail = nullptr;
    std::uint8_t *current_block = nullptr;
    std::uint8_t *head = nullptr;

    // Place the IV at the start of the ring buffer, as the oldest 16 octets
    // are used to XOR the current block to facilitate CBC mode
    std::copy(iv.begin(), iv.end(), ring_buffer.begin());

    // Assign the tail, head, and current_block variables; head after the IV
    tail = ring_buffer.data();
    current_block = head = tail + 16;

    try
    {
        // Create an AES object to perform decryption
        Crypto::Cipher::AES aes(key);

        // The encrypted data is protected with an HMAC
        Crypto::Hashing::HMAC hmac(Crypto::Hashing::HashAlgorithm::SHA256, key);
        if (hmac.GetHMACLength() != computed_hmac.size())
        {
            logger->critical << "HMAC length value is incorrect" << std::flush;
            return DecryptResult::InternalError;
        }

        // Attempt to read 48 octets into the ring buffer
        source.read(reinterpret_cast<char *>(head), 48);
        if (source.bad() || (!source.good() && !source.eof()))
        {
            logger->error << "Error reading initial ciphertext" << std::flush;
            return DecryptResult::IOError;
        }

        // Update pointers / counters with octets consumed
        octets_consumed += source.gcount();
        head += source.gcount(); // This usually points one beyond the buffer

        // Initialize the progress_octets to drive progress callbacks
        progress_octets = octets_consumed;

        // Issue progress callback (facilitate initial rendering)
        if ((progress_interval > 0) && progress_callback)
        {
            progress_callback(instance, octets_consumed);
        }

        // Iterate over all of the ciphertext blocks (block at tail + 16)
        while(!source.eof())
        {
            // Is there a plaintext block to write?
            if (plaintext_to_write)
            {
                // Write the plaintext to the output stream
                destination.write(reinterpret_cast<char *>(plaintext.data()),
                                  16);
                if (!destination.good())
                {
                    logger->error << "Error writing to output stream"
                                  << std::flush;
                    return DecryptResult::IOError;
                }
            }

            // Add this block to the HMAC computation
            hmac.Input({current_block, 16});

            // Decrypt the block, placing it in plaintext
            aes.Decrypt(std::span<std::uint8_t, 16>(current_block, 16),
                        plaintext);

            // XOR the plaintext with the prior block located at tail (CBC mode)
            XORBlock(plaintext,
                     std::span<std::uint8_t, 16>(tail, 16),
                     plaintext);

            // Note there is plaintext to write out
            plaintext_to_write = true;

            // If decryption is cancelled, return early
            if (cancelled)
            {
                logger->warning << "Decryption cancelled" << std::flush;
                return DecryptResult::DecryptionCancelled;
            }

            // Issue a progress callback when appropriate
            if ((progress_interval > 0) && progress_callback &&
                (progress_octets >= progress_interval))
            {
                // Issue progress callback
                progress_callback(instance, octets_consumed);

                // Clear the progress octet count
                progress_octets = 0;
            }

            // Advance the tail (now pointing at ciphertext just decrypted)
            // and the current_block pointer
            tail += 16;
            current_block += 16;

            // Wrap the ring buffer pointers as necessary
            if (tail == (ring_buffer.data() + ring_buffer.size()))
            {
                tail = ring_buffer.data();
            }
            if (current_block == (ring_buffer.data() + ring_buffer.size()))
            {
                current_block = ring_buffer.data();
            }
            if (head == (ring_buffer.data() + ring_buffer.size()))
            {
                head = ring_buffer.data();
            }

            // Attempt to read the next 16 octets
            source.read(reinterpret_cast<char *>(head), 16);
            if (source.bad() || (!source.good() && !source.eof()))
            {
                logger->error << "Error reading ciphertext" << std::flush;
                return DecryptResult::IOError;
            }
            progress_octets += source.gcount();
            octets_consumed += source.gcount();
            head += source.gcount(); // This may point one beyond the buffer
        }

        // The tail would be pointing at the previous ciphertext block or the
        // IV; advance the tail where it would be pointing to the modulo octet
        // or HMAC (depending on the stream version)
        tail += 16;

        // Adjust the tail to the start if at the end of ring
        if (tail == (ring_buffer.data() + ring_buffer.size()))
        {
            tail = ring_buffer.data();
        }

        // Determine how many octets remain in the ring buffer; if the head
        // and tail are equal, the buffer is empty
        if (head >= tail)
        {
            buffer_octets = head - tail;
        }
        else
        {
            buffer_octets = (ring_buffer.data() + ring_buffer.size()) - tail +
                            (head - ring_buffer.data());
        }

        // Buffer should have 32 or 33 octets, depending on the stream version
        if ((((stream_version == 0) || (stream_version >= 3)) &&
             (buffer_octets != 32)) ||
            (((stream_version == 1) || (stream_version == 2)) &&
             (buffer_octets != 33)))
        {
            // Unexpected number octets remaining
            logger->error << "Not a valid AES Crypt stream" << std::flush;
            return DecryptResult::InvalidAESCryptStream;
        }

        // Consume the final octets
        if ((stream_version == 0) || (stream_version >= 3))
        {
            // Tail should be pointing to HMAC for these stream versions
            std::copy(tail, tail + 16, expected_hmac.begin());
            tail += 16;
            if (tail == (ring_buffer.data() + ring_buffer.size()))
            {
                tail = ring_buffer.data();
            }
            std::copy(tail, tail + 16, expected_hmac.begin() + 16);
        }
        else
        {
            // Tail points at the modulo octet
            reserved_modulo = *tail;

            // Balance is HMAC data (this is the first 15 octets)
            std::copy(tail + 1, tail + 16, expected_hmac.begin());
            tail += 16;
            if (tail == (ring_buffer.data() + ring_buffer.size()))
            {
                tail = ring_buffer.data();
            }

            // Copy the next 16 octets (note only 15 octets copied so far)
            std::copy(tail, tail + 16, expected_hmac.begin() + 15);
            tail += 16;
            if (tail == (ring_buffer.data() + ring_buffer.size()))
            {
                tail = ring_buffer.data();
            }

            // Finally, copy the last octet
            expected_hmac[31] = *tail;
        }

        // Compute the hmac
        hmac.Finalize();
        hmac.Result(computed_hmac);

        // Compare the computed and expected HMAC
        if (computed_hmac != expected_hmac)
        {
            logger->error << "Message appears to be altered" << std::flush;
            return DecryptResult::AlteredMessage;
        }

        // For stream version >= 3, the modulo is stored in the final block
        if (stream_version >= 3)
        {
            // There MUST be a block of unwritten plaintext, else there was a
            // logic error; this should never happen in practice, but it is
            // possible to construct a flawed stream, so test for it
            if (!plaintext_to_write)
            {
                logger->error << "Premature end of ciphertext stream"
                              << std::flush;
                return DecryptResult::InvalidAESCryptStream;
            }

            // Ensure the value is sane
            if ((plaintext[15] == 0) || (plaintext[15] > 16))
            {
                // Unexpected number octets remaining
                logger->error << "Final ciphertext block appears invalid"
                              << std::flush;
                return DecryptResult::InvalidAESCryptStream;
            }

            // Extract the modulo value from the final octet in the array
            reserved_modulo = 16 - plaintext[15];

            // If the modulo is 0, it means the entire block is padding
            if (reserved_modulo == 0) plaintext_to_write = false;
        }

        // Write out any residual plaintext with respect to the modulo
        if (plaintext_to_write)
        {
            // Final block size is the lower 4 bits of the modulo octet
            std::size_t final_block_size =
                ((reserved_modulo & 0x0f) == 0) ? 16 : (reserved_modulo & 0x0f);

            // Write the plaintext to the output stream
            destination.write(reinterpret_cast<char *>(plaintext.data()),
                              static_cast<std::streamsize>(final_block_size));

            // Ensure there are no errors on writing output
            if (!destination.good())
            {
                logger->error << "Error writing to output stream" << std::flush;
                return DecryptResult::IOError;
            }
        }
    }
    catch (const Crypto::Cipher::AESException &e)
    {
        logger->critical << "AES Exception: " << e.what() << std::flush;
        return DecryptResult::InternalError;
    }
    catch (const Crypto::Hashing::HashException &e)
    {
        logger->critical << "Hash Exception: " << e.what() << std::flush;
        return DecryptResult::InternalError;
    }

    // Issue a final progress callback
    if ((progress_interval > 0) && progress_callback)
    {
        // Issue callback
        progress_callback(instance, octets_consumed);
    }

    return DecryptResult::Success;
}

} // namespace Terra::AESCrypt::Engine
