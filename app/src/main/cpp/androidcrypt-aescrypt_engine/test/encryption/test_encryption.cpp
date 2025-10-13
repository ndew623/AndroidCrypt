/*
 *  test_encryption.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the AES Crypt Engine's ability to encrypt
 *      various plaintext data.  Encrypted data is then decrypted with the
 *      Decryptor object.  While this module does not use known data (since
 *      every AES Crypt stream is unique), the Decrytor is tested separately
 *      with known data.
 *
 *  Portability Issues:
 *      None.
 */

#include <random>
#include <vector>
#include <utility>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <terra/aescrypt/engine/decryptor.h>
#include <terra/aescrypt/engine/encryptor.h>
#include <terra/logger/null_ostream.h>
#include <terra/logger/logger.h>
#include <terra/stf/stf.h>
#include "../string_stream_buffer.h"

using namespace Terra::AESCrypt::Engine;
using namespace Terra::Logger;

// It is assumed that a char and uint8_t are the same size
static_assert(sizeof(char) == sizeof(std::uint8_t));

STF_TEST(TestEncryption, StandardTestVectors)
{
    const std::u8string password = u8"Hello";
    const std::vector<std::string> tests =
    {
        std::string(""),
        std::string("0"),
        std::string("012"),
        std::string("0123456789ABCDE"),
        std::string("0123456789ABCDEF"),
        std::string("0123456789ABCDEF0"),
        std::string("0123456789ABCDEF0123456789ABCDE"),
        std::string("0123456789ABCDEF0123456789ABCDEF"),
        std::string("0123456789ABCDEF0123456789ABCDEF0"),
        std::string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDE"),
        std::string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"),
        std::string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0"),
        std::string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345"
                    "6789ABCDE"),
        std::string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345"
                    "6789ABCDEF"),
        std::string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345"
                    "6789ABCDEF0"),
        std::string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345"
                    "6789ABCDEF0123456789ABCDE"),
        std::string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345"
                    "6789ABCDEF0123456789ABCDEF"),
        std::string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345"
                    "6789ABCDEF0123456789ABCDEF0"),
        std::string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345"
                    "6789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789AB"
                    "CDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF01"
                    "23456789ABCDEF0123456789ABCDEF0123456789ABCDEF01234567"
                    "89ABCDEF0123456789ABCDEF0123456789ABCDE"),
        std::string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345"
                    "6789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789AB"
                    "CDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF01"
                    "23456789ABCDEF0123456789ABCDEF0123456789ABCDEF01234567"
                    "89ABCDEF0123456789ABCDEF0123456789ABCDEF"),
        std::string("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF012345"
                    "6789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789AB"
                    "CDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF01"
                    "23456789ABCDEF0123456789ABCDEF0123456789ABCDEF01234567"
                    "89ABCDEF0123456789ABCDEF0123456789ABCDEF0")
    };

    // Create a Logger object; chance null_ostream to std::cerr if debugging
    NullOStream null_ostream;
    LoggerPointer logger =
        std::make_shared<Logger>(null_ostream, LogLevel::Debug);

    // Iterate over the test vector
    std::size_t counter = 0;
    for (const auto &plaintext : tests)
    {
        std::string ciphertext;

        // Create an Encryptor object
        Encryptor encryptor(logger, std::to_string(counter));

        // Encrypt the plaintext string
        {
            // Define a place to put the encrypted text
            std::ostringstream oss;

            // Create an input stream over the ciphertext
            StringStreamBuffer plaintext_stream_buffer(plaintext);
            std::istream iss(&plaintext_stream_buffer);

            // Encrypt the plaintext
            auto result =
                encryptor.Encrypt(password, 10, iss, oss, {});
            STF_ASSERT_EQ(EncryptResult::Success, result);

            // Copy the ciphertext for use in the next block
            ciphertext = oss.str();
        }

        // Decrypt what was encrypted and compare the results
        {
            // Create a Decrypt object
            Decryptor decryptor(logger, std::to_string(counter));

            // Define a place to put the decrypted text
            std::ostringstream oss;

            // Create an input stream over the ciphertext
            StringStreamBuffer cipher_stream_buffer(ciphertext);
            std::istream iss(&cipher_stream_buffer);

            // Decrypt the ciphertext
            auto result = decryptor.Decrypt(password, iss, oss, 0);
            STF_ASSERT_EQ(DecryptResult::Success, result);

            // Ensure the output string length is the expected length
            STF_ASSERT_EQ(plaintext.size(), oss.str().size());

            // Compare the strings
            STF_ASSERT_EQ(plaintext, oss.str());
        }

        // Increment the test number counter
        counter++;
    }
}

STF_TEST(TestEncryption, EncryptWithExtensions)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<std::mt19937::result_type> rchar(0, 255);
    std::string plaintext;
    std::string ciphertext;

    // Create Encryptor and Decryptor object
    Encryptor encryptor;
    Decryptor decryptor;

    // Define extensions
    std::vector<std::pair<std::string, std::string>> extensions =
    {
        {"CREATED_BY", "AES Crypt Test"},
        {"CREATED_REASON", "For testing purposes"}
    };

    // Create a string of this length
    const std::size_t length = 4096;

    for (std::size_t i = 0; i < length; i++)
    {
        plaintext += static_cast<char>(rchar(rng));
    }

    {
        // Create an input stream over the ciphertext
        StringStreamBuffer plaintext_stream_buffer(plaintext);
        std::istream iss(&plaintext_stream_buffer);

        // Encrypt the plaintext
        std::ostringstream oss;
        auto result =
            encryptor.Encrypt(u8"secret", 10, iss, oss, extensions);
        STF_ASSERT_EQ(EncryptResult::Success, result);

        // Create an input stream over the ciphertext
        ciphertext = oss.str();
    }

    {
        StringStreamBuffer cipher_stream_buffer(ciphertext);
        std::istream iss(&cipher_stream_buffer);

        // Define a place to put the decrypted text
        std::ostringstream oss;

        // Decrypt the ciphertext
        auto result = decryptor.Decrypt(u8"secret", iss, oss, 0);
        STF_ASSERT_EQ(DecryptResult::Success, result);

        // Ensure the output string length is the expected length
        STF_ASSERT_EQ(plaintext.size(), oss.str().size());

        // Compare the strings
        STF_ASSERT_EQ(plaintext, oss.str());
    }
}

STF_TEST(TestEncryption, RandomStrings)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 8192);
    std::uniform_int_distribution<std::mt19937::result_type> rchar(0, 255);

    // Create Encryptor and Decryptor object
    Encryptor encryptor;
    Decryptor decryptor;

    for (std::size_t i = 0; i < 10; i++)
    {
        std::string plaintext;
        std::string ciphertext;

        // Create a string of this length
        std::size_t length = dist(rng);

        // Put "length" random characters in the string
        std::generate_n(std::back_insert_iterator(plaintext),
                        length,
                        [&]() { return static_cast<char>(rchar(rng)); });

        {
            // Create an input stream over the ciphertext
            StringStreamBuffer plaintext_stream_buffer(plaintext);
            std::istream iss(&plaintext_stream_buffer);

            // Encrypt the plaintext
            std::ostringstream oss;
            auto result = encryptor.Encrypt(u8"secret", 10, iss, oss, {});
            STF_ASSERT_EQ(EncryptResult::Success, result);

            // Create an input stream over the ciphertext
            ciphertext = oss.str();
        }

        {
            StringStreamBuffer cipher_stream_buffer(ciphertext);
            std::istream iss(&cipher_stream_buffer);

            // Define a place to put the decrypted text
            std::ostringstream oss;

            // Decrypt the ciphertext
            auto result = decryptor.Decrypt(u8"secret", iss, oss, 0);
            STF_ASSERT_EQ(DecryptResult::Success, result);

            // Ensure the output string length is the expected length
            STF_ASSERT_EQ(plaintext.size(), oss.str().size());

            // Compare the strings
            STF_ASSERT_EQ(plaintext, oss.str());
        }
    }
}
