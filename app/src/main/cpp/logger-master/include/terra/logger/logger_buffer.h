/*
 *  logger_buffer.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      The LoggerBuffer object is derived from std::stringbuf and is used
 *      in the construction of the Logger's std::ostream streaming objects
 *      to enable logging with syntax like the following:
 *
 *          logger->info << "This is an informational message" << std::flush;
 *
 *      Note the use of std::flush at the end.  This is required in order to
 *      inform the streaming object that the log message is complete.  Until
 *      the buffer is flushed, any other threads attempting to write to the
 *      buffer will be blocked waiting for writing to complete.  By using this
 *      approach, one may break logging output over multiple lines of code.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <sstream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <terra/logger/log_level.h>
#include <terra/logger/logger_interface.h>

namespace Terra::Logger
{

// Define the LoggerBuffer object
class LoggerBuffer : public std::stringbuf
{
    public:
        LoggerBuffer(LogLevel log_level, LoggerInterface *logger) :
            std::stringbuf(),
            log_level{log_level},
            logger{logger},
            available{true},
            owning_thread{}
        {
            // Nothing else to do
        }
        virtual ~LoggerBuffer() = default;

    protected:
        void synchronize()
        {
            // Lock the mutex
            std::unique_lock<std::mutex> lock(logger_buffer_mutex);

            // Take note of this thread ID
            std::thread::id thread_id = std::this_thread::get_id();

            // Attempt to grab the lock, but wait no more than a second
            if (!cv.wait_for(lock,
                             std::chrono::seconds(1),
                             [&]() -> bool {
                                 return available ||
                                        (!available &&
                                         (owning_thread == thread_id));
                             }))
            {
                logger->Log(LogLevel::Warning,
                            "Logger noticed the stream was not flushed");
            }

            // Indicate that the stream is unavailable to other threads
            available = false;

            // Note that this thread owns the right to write to the stream
            owning_thread = thread_id;
        }

        std::streamsize xsputn(const char *s, std::streamsize count) override
        {
            // Ensure only one thread is writing
            synchronize();

            return std::stringbuf::xsputn(s, count);
        }

        int sync() override
        {
            // Write out string to be logged
            logger->Log(log_level, str());

            // Clear the internal string buffer
            str({});

            // Lock the mutex
            std::lock_guard<std::mutex> lock(logger_buffer_mutex);

            // Note that the stream is now available to other threads
            available = true;

            // Awaken a waiting thread (if any)
            cv.notify_one();

            return 0;
        }

        LogLevel log_level;
        LoggerInterface *logger;
        bool available;
        std::thread::id owning_thread;
        std::condition_variable cv;
        std::mutex logger_buffer_mutex;
};

} // namespace Terra::Logger
