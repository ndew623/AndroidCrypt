/*
 *  encryptor.cpp
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the Encryptor object that is a part of the
 *      AES Crypt Engine.  This object is responsible for encrypting plaintext
 *      into the AES Crypt stream format from a specified source, outputting the
 *      ciphertext to the specified destination.
 *
 *  Portability Issues:
 *      None.
 */

#include <climits>
#include <algorithm>
#include <array>
#include <terra/aescrypt/engine/encryptor.h>
#include <terra/secutil/secure_erase.h>
#include <terra/secutil/secure_vector.h>
#include <terra/secutil/secure_array.h>
#include <terra/charutil/character_utilities.h>
#include <terra/random/random_generator.h>
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
 *      Streaming operator for the EncryptResult to print text strings for
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
std::ostream &operator<<(std::ostream &o, const EncryptResult result)
{
    switch (result)
    {
        case EncryptResult::Success:
            o << "Success";
            break;

        case EncryptResult::IOError:
            o << "I/O error";
            break;

        case EncryptResult::InvalidExtension:
            o << "Invalid extension";
            break;

        case EncryptResult::InvalidPassword:
            o << "Invalid password provided";
            break;

        case EncryptResult::InvalidIterations:
            o << "Invalid iterations value";
            break;

        case EncryptResult::AlreadyEncrypting:
            o << "Another thread is already encrypting";
            break;

        case EncryptResult::EncryptionCancelled:
            o << "Encryption operation was cancelled";
            break;

        case EncryptResult::InternalError:
            o << "Internal error";
            break;

        default:
            o << "Unknown result";
            break;
    }

    return o;
}

/*
 *  Encryptor::Encryptor()
 *
 *  Description:
 *      Constructor for the Encryptor object.
 *
 *  Parameters:
 *      parent_logger [in]
 *          A shared pointer to a parent Logger object.  If this is a nullptr,
 *          then a default parent will be created that produces no output.
 *
 *      instance [in]
 *          An optional instance string to help differentiate between multiple
 *          instances of the Encryptor object (useful for logging and when
 *          using the same progress callback function).
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Encryptor::Encryptor(Logger::LoggerPointer parent_logger,
                     const std::string &instance) :
    logger{std::make_shared<Logger::Logger>(std::move(parent_logger),
                                            CreateComponent("ENC", instance))},
    instance{instance},
    active{false},
    cancelled{false},
    read_length{},
    octets_consumed{},
    progress_octets{}
{
}

/*
 *  Encryptor::~Encryptor()
 *
 *  Description:
 *      Destructor for the Encryptor object.  This will ensure that any member
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
Encryptor::~Encryptor()
{
    SecUtil::SecureErase(read_length);
    SecUtil::SecureErase(octets_consumed);
    SecUtil::SecureErase(progress_octets);
}

/*
 *  Encryptor::Encrypt()
 *
 *  Description:
 *      This function will encrypt the specified input source using the provided
 *      UTF-8-encoded password, directing output to the specified output stream.
 *
 *  Parameters:
 *      password [in]
 *          The password used to encrypt the plaintext data.  The password
 *          is limited to less than 1/2 the size of std::size_t, since the
 *          Encryptor enforces that length to ensure there is space for
 *          expansion in the event that it needs to convert characters to
 *          UTF-16LE.
 *
 *      kdf_iterations [in]
 *          The number of iterations used by the Key Derivation Function (KDF).
 *          This value must be >= 1.
 *
 *      source [in]
 *          The input stream from which to read the plaintext data.
 *
 *      destination [out]
 *          The destination stream to which the output encrypted data.
 *
 *      extensions [in]
 *          Extensions to insert into the output stream.
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
 *      The result of the encryption process.
 *
 *  Comments:
 *      None.
 */
EncryptResult Encryptor::Encrypt(
    const std::u8string &password,
    const std::uint32_t kdf_iterations,
    std::istream &source,
    std::ostream &destination,
    const std::vector<std::pair<std::string, std::string>> &extensions,
    const ProgressCallback &progress_callback,
    const std::size_t progress_interval)
{
    const std::array<char, 5> aes_crypt_header =
    {
        'A', 'E', 'S',
        static_cast<char>(Latest_AES_Crypt_Stream_Version),
        0x00
    };
    Random::RandomGenerator random_generator;
    SecUtil::SecureArray<std::uint8_t, 16> public_iv;
    SecUtil::SecureArray<std::uint8_t, 16> session_iv;
    SecUtil::SecureArray<std::uint8_t, 32> session_key;
    EncryptResult result{};

    logger->Log("Encrypting the plaintext stream");

    // Ensure the source and destination streams are in a good state
    if (!source.good())
    {
        logger->error << "Source stream is not in a good state" << std::flush;
        return EncryptResult::IOError;
    }
    if (!destination.good())
    {
        logger->error << "Destination stream is not in a good state"
                      << std::flush;
        return EncryptResult::IOError;
    }

    // Ensure the password has at least one octet
    if (password.empty())
    {
        logger->error << "Password is an empty string" << std::flush;
        return EncryptResult::InvalidPassword;
    }

    // Ensure the password is not too long
    if (password.size() > (std::numeric_limits<std::size_t>::max() / 2))
    {
        logger->error << "Password is too long" << std::flush;
        return EncryptResult::InvalidPassword;
    }

    // Ensure the UTF-8 sequence is valid
    if (!CharUtil::IsUTF8Valid(
            {reinterpret_cast<const std::uint8_t *>(password.data()),
             password.size()}))
    {
        logger->error << "Password is not a valid UTF-8 sequence" << std::flush;
        return EncryptResult::InvalidPassword;
    }

    // Ensure the iterations value is bounded
    if ((kdf_iterations < PBKDF2_Min_Iterations) ||
        (kdf_iterations > PBKDF2_Max_Iterations))
    {
        logger->error << "Iteration value is not a valid" << std::flush;
        return EncryptResult::InvalidIterations;
    }

    // Indicate that encryption is starting
    result = BeginEncrypting();
    if (result != EncryptResult::Success) return result;

    // Write AES header
    destination.write(aes_crypt_header.data(), aes_crypt_header.size());
    if (!destination.good())
    {
        logger->error << "Error writing AES Crypt header to output stream"
                      << std::flush;
        return EncryptResult::IOError;
    }

    // Write extensions
    result = WriteExtensions(destination, extensions);
    if (result != EncryptResult::Success)
    {
        logger->error << "Failed to extensions: " << result << std::flush;
        FinishedEncrypting();
        return result;
    }

    // Create the public random IV
    random_generator.GetRandomOctets({public_iv.data(), public_iv.size()});

    // Create a random IV + key (Session values)
    random_generator.GetRandomOctets({session_iv.data(), session_iv.size()});
    random_generator.GetRandomOctets({session_key.data(), session_key.size()});

    // Write the public salt and session data (private IV + key)
    result = WriteSessionData(destination,
                              password,
                              kdf_iterations,
                              public_iv,
                              session_iv,
                              session_key);
    if (result != EncryptResult::Success)
    {
        logger->error << "Failed to write session data to output stream: "
                      << result
                      << std::flush;
        FinishedEncrypting();
        return result;
    }

    // Encrypt the stream
    result = EncryptStream(source,
                           destination,
                           session_iv,
                           session_key,
                           progress_callback,
                           progress_interval);
    if (result != EncryptResult::Success)
    {
        logger->error << "Failed to write ciphertext output stream: "
                      << result
                      << std::flush;
        FinishedEncrypting();
        return result;
    }

    // Indicate that a thread is no longer active
    FinishedEncrypting();

    logger->Log("Finished encrypting the plaintext stream");

    return EncryptResult::Success;
}

/*
 *  Encryptor::Cancel()
 *
 *  Description:
 *      This function is called by a separate thread of execution to request
 *      an ongoing Encrypt() call to terminate.  Calling this function may not
 *      result in immediate termination of the ongoing encryption process, but
 *      it will terminate at the next opportunity to check the termination flag.
 *      The calling thread will be blocked until the encrypting thread
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
void Encryptor::Cancel()
{
    std::unique_lock<std::mutex> lock(encryptor_mutex);

    // Indicate that the encryption thread should stop
    cancelled = true;

    // If no longer encrypting, return
    if (!active) return;

    // Wait for the encrypting thread to complete
    cv.wait(lock, [&]() -> bool { return !active; });
}

/*
 *  Encryptor::Activate()
 *
 *  Description:
 *      This function is called to reactive an object that was previously
 *      placed in a cancelled state.  Without calling this function, subsequent
 *      attempts to use this object would fail with the reason of
 *      EncryptResult::EncryptionCancelled.  If a thread is currently
 *      encrypting, then this function returns false.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      True if the object is reactivated and false if there is an error.  The
 *      only error that might occur is trying to reactivate an object that is
 *      currently encrypting a stream.
 *
 *  Comments:
 *      None.
 */
bool Encryptor::Activate()
{
    std::lock_guard<std::mutex> lock(encryptor_mutex);

    // If there is an active encryption thread, fail
    if (active) return false;

    // Set the cancelled flag to false
    cancelled = false;

    return true;
}

/*
 *  Encryptor::BeginEncrypting()
 *
 *  Description:
 *      This call is made when initiating the encryption process to ensure
 *      that another thread hasn't already cancelled the thread used to do
 *      encryption.
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
EncryptResult Encryptor::BeginEncrypting()
{
    std::lock_guard<std::mutex> lock(encryptor_mutex);

    // If the encryption object is in a cancelled state, just return
    if (cancelled) return EncryptResult::EncryptionCancelled;

    // If already encrypting, return an error response
    if (active) return EncryptResult::AlreadyEncrypting;

    // Indicate that the thread is encrypting
    active = true;

    // Reset the octet count
    octets_consumed = 0;

    return EncryptResult::Success;
}

/*
 *  Encryptor::FinishedEncrypting()
 *
 *  Description:
 *      This function is called when the encryption thread has completed work
 *      or recognizes that the request was cancelled.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      This should only be called by the thread currently encrypting.
 */
void Encryptor::FinishedEncrypting()
{
    std::lock_guard<std::mutex> lock(encryptor_mutex);

    // Indicate that the thread is no longer encrypting
    active = false;

    // Notify any awaiting thread that encryption has stopped
    cv.notify_all();
}

/*
 *  Encryptor::WriteExtensions()
 *
 *  Description:
 *      This function will write the given set of extensions to the output
 *      stream.  If there are no extensions, then a single 0x0000 will be
 *      written.  The extension vector is comprised of an identifier and
 *      value pair.  The identifiers MUST NOT contain a 0x00 octet.
 *      Any extension pair where the length of the identifier and value
 *      exceeds 64K - 1 octets in size will be rejected and the error
 *      EncryptResult::InvalidExtension will be returned.
 *
 *  Parameters:
 *      destination [out]
 *          The destination stream to which the output encrypted data.
 *
 *      extensions [in]
 *          The extensions to write to the output stream.
 *
 *  Returns:
 *      A status code indicating success or failure.
 *
 *  Comments:
 *      None.
 */
EncryptResult Encryptor::WriteExtensions(
            std::ostream &destination,
            const std::vector<std::pair<std::string, std::string>> &extensions)
{
    constexpr std::size_t Max_Extension_Length = 65535;

    // Array to hold an extension length in network byte order
    std::array<char, 2> extension_length{};

    // Iterate over the extensions
    for (const auto &[identifier, value] : extensions)
    {
        // Ensure the identifier does not have a 0x00 in the string
        if (identifier.find('\0') != std::string::npos)
        {
            return EncryptResult::InvalidExtension;
        }

        // Ensure that neither the identifier nor value lengths are excessive
        if ((identifier.size() > Max_Extension_Length) ||
            (value.size() > Max_Extension_Length))
        {
            return EncryptResult::InvalidExtension;
        }

        // Total length is the identifier \0 value
        std::size_t length = identifier.size() + 1 + value.size();

        // Ensure the length is <= 65535
        if (length > 65535) return EncryptResult::InvalidExtension;

        // Assign the length in network byte order
        extension_length[0] = static_cast<char>((length >> 8) & 0xff);
        extension_length[1] = static_cast<char>((length     ) & 0xff);

        // Write the extension length
        destination.write(extension_length.data(), extension_length.size());
        if (!destination.good())
        {
            logger->error << "Error writing extension to output stream"
                          << std::flush;
            return EncryptResult::IOError;
        }

        // Write the actual extension identifier
        destination.write(identifier.data(),
                          static_cast<std::streamsize>(identifier.size()));
        if (!destination.good())
        {
            logger->error << "Error writing extension to output stream"
                          << std::flush;
            return EncryptResult::IOError;
        }

        // Write the \0 octet
        destination.put('\0');
        if (!destination.good())
        {
            logger->error << "Error writing extension to output stream"
                          << std::flush;
            return EncryptResult::IOError;
        }

        // Write the actual extension value
        destination.write(value.data(),
                          static_cast<std::streamsize>(value.size()));
        if (!destination.good())
        {
            logger->error << "Error writing extension to output stream"
                          << std::flush;
            return EncryptResult::IOError;
        }
    }

    // The final extension will be 0x0000
    extension_length = {0x00, 0x00};

    // Write the zero-length extension to indicate end of extensions
    destination.write(extension_length.data(), extension_length.size());
    if (!destination.good())
    {
        logger->error << "Error writing extension to output stream"
                      << std::flush;
        return EncryptResult::IOError;
    }

    return EncryptResult::Success;
}

/*
 *  Encryptor::DeriveKey()
 *
 *  Description:
 *      Derive the encryption key given the user-provided password.
 *
 *  Parameters:
 *      password [in]
 *          The user-provided password in UTF-8 format.
 *
 *      kdf_iterations [in]
 *          The number of iterations used by the Key Derivation Function (KDF).
 *
 *      iv [in]
 *          The initialization vector to use with the key derivation function.
 *
 *      key [out]
 *          The derived key.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
EncryptResult Encryptor::DeriveKey(const std::u8string &password,
                                   const std::uint32_t kdf_iterations,
                                   const std::span<const std::uint8_t, 16> iv,
                                   std::span<std::uint8_t, 32> key)
{
    logger->info << "Performing key derivation" << std::flush;

    try
    {
        // KDF used in AES Crypt formats 3 and onward
        auto kdf_result = Crypto::KDF::PBKDF2(
            PBKDF2_Hash_Algorithm,
            {reinterpret_cast<const std::uint8_t *>(password.data()),
             password.size()},
            iv,
            kdf_iterations,
            key);

        // Check the returned key length
        if (kdf_result.size() != key.size())
        {
            logger->error << "Unexpected key length returned from KDF"
                          << std::flush;

            return EncryptResult::InternalError;
        }
    }
    catch (const Crypto::KDF::KDFException &e)
    {
        logger->critical << "KDF Exception: " << e.what() << std::flush;
        return EncryptResult::InternalError;
    }

    logger->info << "Key derivation completed successfully" << std::flush;

    return EncryptResult::Success;
}

/*
 *  Encryptor::WriteSessionData()
 *
 *  Description:
 *      This function will write the KDF iterations value, public IV, and
 *      private session data to the destination.  The private session data
 *      includes an IV (that could be public, but is encrypted nonetheless)
 *      and private key.  The session data is  encrypted using the given
 *      password and public IV.  The public IV is used in both the key
 *      derivation and CBC computation.
 *
 *  Parameters:
 *      destination [out]
 *          The destination stream to which the output encrypted data.
 *
 *      password [in]
 *          User-provided password to use to encrypt the session data
 *          and in computing the HMAC over the encrypted session data.
 *
 *      kdf_iterations [in]
 *          The number of iterations used by the Key Derivation Function (KDF).
 *          This value must be >= 1.
 *
 *      public_iv [in]
 *          The initialization vector used by the KDF and CBC operation
 *          when encrypting the session data.
 *
 *      session_iv [in]
 *          The session IV used in CBC mode for the bulk of input stream.
 *          This function will encrypt that value and write it to the
 *          output stream.
 *
 *      session_key [in]
 *          The key used to encrypt the bulk of the input stream.  This
 *          function will encrypt that value and write it to the output
 *          stream.
 *
 *  Returns:
 *      A status code indicating success or failure.
 *
 *  Comments:
 *      None.
 */
EncryptResult Encryptor::WriteSessionData(
                        std::ostream &destination,
                        const std::u8string &password,
                        const std::uint32_t kdf_iterations,
                        const std::span<std::uint8_t, 16> public_iv,
                        const std::span<std::uint8_t, 16> session_iv,
                        const std::span<std::uint8_t, 32> session_key)
{
    EncryptResult result{};
    SecUtil::SecureArray<std::uint8_t, 32> key;
    SecUtil::SecureArray<std::uint8_t, 32> computed_hmac;
    SecUtil::SecureArray<std::uint8_t, 16> ciphertext;
    std::array<std::uint8_t, 4> iterations{};

    // Derive the encryption key to encrypt the session data
    result = DeriveKey(password, kdf_iterations, public_iv, key);
    if (result != EncryptResult::Success) return result;

    // If the KDF iterations is 0, report an error
    if (kdf_iterations == 0)
    {
        logger->error << "Invalid iterations value" << std::flush;
        return EncryptResult::InvalidIterations;
    }

    // Write out the iterations value
    iterations[0] = (kdf_iterations >> 24) & 0xff;
    iterations[1] = (kdf_iterations >> 16) & 0xff;
    iterations[2] = (kdf_iterations >>  8) & 0xff;
    iterations[3] = (kdf_iterations      ) & 0xff;
    destination.write(reinterpret_cast<const char *>(iterations.data()),
                      iterations.size());
    if (!destination.good())
    {
        logger->error << "Error writing iterations value" << std::flush;
        return EncryptResult::IOError;
    }

    // Write out the public IV
    destination.write(reinterpret_cast<const char *>(public_iv.data()),
                      public_iv.size());
    if (!destination.good())
    {
        logger->error << "Error writing public IV" << std::flush;
        return EncryptResult::IOError;
    }

    try
    {
        // Prepare to calculate the HMAC
        Crypto::Hashing::HMAC hmac(Crypto::Hashing::HashAlgorithm::SHA256, key);
        if (hmac.GetHMACLength() != computed_hmac.size())
        {
            logger->critical << "HMAC length value is incorrect" << std::flush;
            return EncryptResult::InternalError;
        }

        // Create the AES object to perform encryption
        Crypto::Cipher::AES aes(key);

        // Encrypt the private IV, add to HMAC, and write it out
        XORBlock(session_iv, public_iv, ciphertext);
        aes.Encrypt(ciphertext, ciphertext);
        hmac.Input(ciphertext);
        destination.write(reinterpret_cast<const char *>(ciphertext.data()),
                          ciphertext.size());
        if (!destination.good())
        {
            logger->error << "Error writing session IV" << std::flush;
            return EncryptResult::IOError;
        }

        // Do the same for the first half of the session key
        XORBlock(std::span<const std::uint8_t, 16>(session_key.data(), 16),
                 ciphertext,
                 ciphertext);
        aes.Encrypt(ciphertext, ciphertext);
        hmac.Input(ciphertext);
        destination.write(reinterpret_cast<const char *>(ciphertext.data()),
                          ciphertext.size());
        if (!destination.good())
        {
            logger->error << "Error writing session key" << std::flush;
            return EncryptResult::IOError;
        }

        // Do the same for the second half of the session key
        XORBlock(std::span<const std::uint8_t, 16>(session_key.data() + 16, 16),
                 ciphertext,
                 ciphertext);
        aes.Encrypt(ciphertext, ciphertext);
        hmac.Input(ciphertext);
        destination.write(reinterpret_cast<const char *>(ciphertext.data()),
                          ciphertext.size());
        if (!destination.good())
        {
            logger->error << "Error writing session key" << std::flush;
            return EncryptResult::IOError;
        }

        // Add the AES Crypt stream version to the HMAC
        std::uint8_t current_version = Latest_AES_Crypt_Stream_Version;
        hmac.Input({&current_version, 1});

        // Finalize the HMAC and write it to the stream
        hmac.Finalize();
        hmac.Result(computed_hmac);
        destination.write(reinterpret_cast<const char *>(computed_hmac.data()),
                          computed_hmac.size());
        if (!destination.good())
        {
            logger->error << "Error writing session HMAC" << std::flush;
            return EncryptResult::IOError;
        }
    }
    catch (const Crypto::Cipher::AESException &e)
    {
        logger->critical << "AES Exception: " << e.what() << std::flush;
        return EncryptResult::InternalError;
    }
    catch (const Crypto::Hashing::HashException &e)
    {
        logger->critical << "Hash Exception: " << e.what() << std::flush;
        return EncryptResult::InternalError;
    }

    return EncryptResult::Success;
}

/*
 *  Encryptor::EncryptStream()
 *
 *  Description:
 *      This function will encrypt the source stream, writing the ciphertext
 *      to the output stream.  Once the final block is of input data is read,
 *      padding octets will be appended per PKCS#7 and an HMAC is appended
 *      to the ciphertext to guarantee the integrity of the data.
 *
 *  Parameters:
 *      source [in]
 *          The input stream from which to read the plaintext data.
 *
 *      destination [out]
 *          The destination stream to which the output encrypted data.
 *
 *      iv [in]
 *          The initialization vector to be used as part of the CBC computation.
 *
 *      key [in]
 *          The symmetric key used for encryption.
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
EncryptResult Encryptor::EncryptStream(
                                std::istream &source,
                                std::ostream &destination,
                                const std::span<const std::uint8_t, 16> iv,
                                const std::span<const std::uint8_t, 32> key,
                                const ProgressCallback &progress_callback,
                                const std::size_t progress_interval)
{
    SecUtil::SecureArray<std::uint8_t, 32> computed_hmac;
    SecUtil::SecureArray<std::uint8_t, 16> plaintext;
    SecUtil::SecureArray<std::uint8_t, 16> ciphertext;

    try
    {
        // Prepare to calculate the HMAC
        Crypto::Hashing::HMAC hmac(Crypto::Hashing::HashAlgorithm::SHA256, key);
        if (hmac.GetHMACLength() != computed_hmac.size())
        {
            logger->critical << "HMAC length value is incorrect" << std::flush;
            return EncryptResult::InternalError;
        }

        // Create the AES object to perform encryption
        Crypto::Cipher::AES aes(key);

        // Copy the IV into the ciphertext buffer
        std::copy(iv.begin(), iv.end(), ciphertext.begin());

        // Initialize the consumed / progress counters, read_length
        octets_consumed = 0;
        progress_octets = 0;
        read_length = 0;

        // Issue progress callback (facilitate initial rendering)
        if ((progress_interval > 0) && progress_callback)
        {
            progress_callback(instance, octets_consumed);
        }

        // Read the input stream until there is no more data
        while (!source.eof())
        {
            // Attempt to read the next 16 octets
            source.read(reinterpret_cast<char *>(plaintext.data()), 16);
            if (source.bad() || (!source.good() && !source.eof()))
            {
                logger->error << "Error reading plaintext data" << std::flush;
                return EncryptResult::IOError;
            }

            // Get the number of octets read (which may be zero)
            read_length = static_cast<std::uint8_t>(source.gcount());

            // Update the counters with the number of octets read
            progress_octets += read_length;
            octets_consumed += read_length;

            // If this is the final block, pad per PKCS#7 (RFC 5652)
            if (read_length < 16)
            {
                // This code should execute only at the end of the stream
                if (!source.eof())
                {
                    logger->error << "Read operation returned too few octets"
                                  << std::flush;
                    return EncryptResult::IOError;
                }

                // Pad with value 16 - read_length octets
                std::fill(plaintext.data() + read_length,
                          plaintext.data() + plaintext.size(),
                          static_cast<std::uint8_t>(16 - read_length));
            }

            // Encrypt the block
            XORBlock(plaintext, ciphertext, ciphertext);
            aes.Encrypt(ciphertext, ciphertext);
            hmac.Input(ciphertext);
            destination.write(reinterpret_cast<const char *>(ciphertext.data()),
                              ciphertext.size());
            if (!destination.good())
            {
                logger->error << "Error writing ciphertext" << std::flush;
                return EncryptResult::IOError;
            }

            // If encryption is cancelled, return early
            if (cancelled)
            {
                logger->warning << "Encryption cancelled" << std::flush;
                return EncryptResult::EncryptionCancelled;
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
        }

        // Finalize the HMAC and write it to the destination stream
        hmac.Finalize();
        hmac.Result(computed_hmac);
        destination.write(reinterpret_cast<const char *>(computed_hmac.data()),
                          computed_hmac.size());
        if (!destination.good())
        {
            logger->error << "Error writing final HMAC" << std::flush;
            return EncryptResult::IOError;
        }
    }
    catch (const Crypto::Cipher::AESException &e)
    {
        logger->critical << "AES Exception in Encryptor: " << e.what()
                         << std::flush;
        return EncryptResult::InternalError;
    }
    catch (const Crypto::Hashing::HashException &e)
    {
        logger->critical << "Hash Exception in Encryptor: " << e.what()
                         << std::flush;
        return EncryptResult::InternalError;
    }
    catch (const std::exception &e)
    {
        logger->critical << "Unexpected internal error in Encryptor: "
                         << e.what() << std::flush;
        return EncryptResult::InternalError;
    }
    catch (...)
    {
        logger->critical << "Unexpected internal error in Encryptor"
                         << std::flush;
        return EncryptResult::InternalError;
    }

    // Issue a final progress callback
    if ((progress_interval > 0) && progress_callback)
    {
        // Issue progress callback
        progress_callback(instance, octets_consumed);
    }

    return EncryptResult::Success;
}

} // namespace Terra::AESCrypt::Engine
