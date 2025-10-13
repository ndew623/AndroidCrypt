/*
 *  encryptor.h
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the Encryptor object that is a part of the AES Crypt
 *      Engine.  This object is responsible for encrypting plaintext into
 *      the AES Crypt stream format from a specified source, outputting the
 *      ciphertext to the specified destination.
 *
 *      The Encryptor object operates on streams serially.  If one wishes to
 *      decrypt a several streams in parallel, use a separate Encryptor object
 *      for each parallel operation.
 *
 *      Operations may be cancelled via a separate thread of execution by
 *      calling Cancel().
 *
 *      Regardless of failure or cancellation, it is the caller's responsibility
 *      to perform any required cleanup of the output stream.
 *
 *      The constructor includes a callback function to which progress updates
 *      are delivered, and the Encrypt() function allows one to specify
 *      an interval.  The idea with this is that after each "progress_interval"
 *      group of octets, the "progress_callback" function will be called
 *      to provide the number of octets decrypted to that point.  This allows
 *      the caller to render a progress meter for the user.  These updates
 *      will be delivered as reasonably close to the requested interval as
 *      possible, but do not expect precision since no updates will be provided,
 *      for example, more frequently than one block of ciphertext (16 octets).
 *      Generally, the interval should be set to something like:
 *
 *          interval = file size / meter positions
 *
 *      If the callback function is not callable or if the interval is 0,
 *      updates will not be delivered.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <iostream>
#include <functional>
#include <span>
#include <vector>
#include <utility>
#include <cstddef>
#include <mutex>
#include <condition_variable>
#include <cstdint>
#include <terra/logger/logger.h>

namespace Terra::AESCrypt::Engine
{

enum class EncryptResult
{
    Success,
    IOError,
    InvalidExtension,
    InvalidPassword,
    InvalidIterations,
    AlreadyEncrypting,
    EncryptionCancelled,
    InternalError
};

// Define the Encryptor object
class Encryptor
{
    public:
        using ProgressCallback =
            std::function<void(const std::string &, std::size_t)>;

        Encryptor(Logger::LoggerPointer parent_logger = {},
                  const std::string &instance = {});
        virtual ~Encryptor();

        EncryptResult Encrypt(
            const std::u8string &password,
            const std::uint32_t kdf_iterations,
            std::istream &source,
            std::ostream &destination,
            const std::vector<std::pair<std::string, std::string>> &extensions,
            const ProgressCallback &progress_callback = {},
            const std::size_t progress_interval = 0);
        void Cancel();
        bool Activate();

    protected:
        EncryptResult BeginEncrypting();
        void FinishedEncrypting();
        EncryptResult WriteExtensions(
            std::ostream &destination,
            const std::vector<std::pair<std::string, std::string>> &extensions);
        EncryptResult DeriveKey(const std::u8string &password,
                                const std::uint32_t iterations,
                                const std::span<const std::uint8_t, 16> iv,
                                std::span<std::uint8_t, 32> key);
        EncryptResult WriteSessionData(
                                std::ostream &destination,
                                const std::u8string &password,
                                const std::uint32_t kdf_iterations,
                                const std::span<std::uint8_t, 16> public_iv,
                                const std::span<std::uint8_t, 16> session_iv,
                                const std::span<std::uint8_t, 32> session_key);
        EncryptResult EncryptStream(std::istream &source,
                                    std::ostream &destination,
                                    const std::span<const std::uint8_t, 16> iv,
                                    const std::span<const std::uint8_t, 32> key,
                                    const ProgressCallback &progress_callback,
                                    const std::size_t progress_interval);

        Logger::LoggerPointer logger;
        std::string instance;
        bool active;
        bool cancelled;
        std::uint8_t read_length;
        std::size_t octets_consumed;
        std::size_t progress_octets;
        std::condition_variable cv;
        std::mutex encryptor_mutex;
};

// Declare streaming operator for EncryptResult (useful for logging)
std::ostream &operator<<(std::ostream &o, const EncryptResult result);

} // namespace Terra::AESCrypt::engine
