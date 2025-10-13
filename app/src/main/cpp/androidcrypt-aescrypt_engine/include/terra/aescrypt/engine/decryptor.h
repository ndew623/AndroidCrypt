/*
 *  decryptor.h
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the Decryptor object that is a part of the AES Crypt
 *      Engine.  This object is responsible for decrypting AES Crypt streams
 *      from a specified source, outputting the plaintext to the specified
 *      destination.
 *
 *      The Decryptor object operates on streams serially.  If one wishes to
 *      decrypt a several streams in parallel, use a separate Decryptor object
 *      for each parallel operation.
 *
 *      Operations may be cancelled via a separate thread of execution by
 *      calling Cancel().
 *
 *      Regardless of failure or cancellation, it is the caller's responsibility
 *      to perform any required cleanup of the output stream.
 *
 *      The constructor includes a callback function to which progress updates
 *      are delivered, and the Decrypt() function allows one to specify
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
#include <cstddef>
#include <mutex>
#include <condition_variable>
#include <cstdint>
#include <terra/logger/logger.h>

namespace Terra::AESCrypt::Engine
{

enum class DecryptResult
{
    Success,
    InvalidAESCryptStream,
    UnsupportedAESCryptVersion,
    IOError,
    InvalidPassword,
    InvalidIterations,
    AlteredMessage,
    AlreadyDecrypting,
    DecryptionCancelled,
    InternalError
};

// Define the Decryptor object
class Decryptor
{
    public:
        using ProgressCallback =
            std::function<void(const std::string &, std::size_t)>;

        Decryptor(Logger::LoggerPointer parent_logger = {},
                  const std::string &instance = {});
        virtual ~Decryptor();

        DecryptResult Decrypt(const std::u8string &password,
                              std::istream &source,
                              std::ostream &destination,
                              const ProgressCallback &progress_callback = {},
                              const std::size_t progress_interval = 0);
        void Cancel();
        bool Activate();

    protected:
        DecryptResult BeginDecrypting();
        void FinishedDecrypting();
        DecryptResult DetermineVersion(std::istream &source);
        DecryptResult ReadOctets(std::istream &source,
                                 std::span<std::uint8_t> octets);
        DecryptResult ConsumeExtensions(std::istream &source);
        DecryptResult DeriveKey(const std::u8string &password,
                                const std::uint32_t kdf_iterations,
                                const std::span<const std::uint8_t, 16> iv,
                                std::span<std::uint8_t, 32> key);
        DecryptResult GetSessionKey(std::istream &source,
                                    std::span<std::uint8_t, 16> iv,
                                    std::span<std::uint8_t, 32> key);
        DecryptResult DecryptStream(std::istream &source,
                                    std::ostream &destination,
                                    const std::span<const std::uint8_t, 16> iv,
                                    const std::span<const std::uint8_t, 32> key,
                                    const ProgressCallback &progress_callback,
                                    const std::size_t progress_interval);

        Logger::LoggerPointer logger;
        std::string instance;
        bool active;
        bool cancelled;
        std::uint8_t stream_version;
        std::uint8_t reserved_modulo;
        std::size_t octets_consumed;
        std::size_t progress_octets;
        std::condition_variable cv;
        std::mutex decryptor_mutex;
};

// Declare streaming operator for DecryptResult (useful for logging)
std::ostream &operator<<(std::ostream &o, const DecryptResult result);

} // namespace Terra::AESCrypt::Engine
