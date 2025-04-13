/*
 *  null_ostream.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file specifies the NullOStream object for use with the Logger.  It
 *      will act as an output stream, but emit nothing.  This is useful for
 *      programs that want to specify outputting to a streaming destination, but
 *      actually produce no output.
 *
 *      It would also be valid to create a "null stream" object for use
 *      with the Logger like this:
 *
 *          std::ostream null_stream(nullptr);
 *          LoggerPointer logger = std::make_shared<Logger>(null_stream);
 *
 *      However, streams created that way will have the unfortunate side-effect
 *      of indicating that the stream is "bad" when bad() is called.
 *      Using the NullOStream object will indicate true for good(). Thus,
 *      this is probably the better approach:
 *
 *          NullOStream null_stream;
 *          LoggerPointer logger = std::make_shared<Logger>(null_stream);
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <ostream>
#include <sstream>

namespace Terra::Logger
{

// Define the NullBuffer object that just discards input
class NullBuffer : public std::stringbuf
{
    public:
        NullBuffer() noexcept = default;
        ~NullBuffer() = default;

    protected:
        // Do nothing with the input, but report that all characters consumed
        std::streamsize xsputn([[maybe_unused]] const char *s,
                               std::streamsize count) override
        {
            return count;
        }
};

// Define the NullOStream object
class NullOStream : public std::ostream
{
    public:
        NullOStream() noexcept : std::ostream(&null_buffer) {}
        ~NullOStream() = default;

    protected:
        NullBuffer null_buffer;
};

} // namespace Terra::Logger
