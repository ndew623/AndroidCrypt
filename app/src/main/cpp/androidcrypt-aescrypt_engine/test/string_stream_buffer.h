/*
 *  string_stream_buffer.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements a simple custom StringStreamBuffer object
 *      used to facilitate testing.  While it is referred to as
 *      a "string" buffer, it can receive a span of characters or uint8_t.
 *      The buffer provided must live at least as long as this object.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <sstream>
#include <span>
#include <string>
#include <cstdint>

namespace {

// Define the StringStreamBuffer class
class StringStreamBuffer : public std::streambuf
{
    public:
        StringStreamBuffer(const std::span<const char>(buffer))
        {
            auto p =  const_cast<char *>(buffer.data());
            setg(p, p, p + buffer.size());
            setp(p, p + buffer.size());
        }

        StringStreamBuffer(const std::string &buffer) :
            StringStreamBuffer(
                std::span<const char>(buffer.data(), buffer.size()))
        {
        }
        StringStreamBuffer(const std::span<const std::uint8_t>(buffer)) :
            StringStreamBuffer(std::span<const char>(
                reinterpret_cast<const char *>(buffer.data()),
                buffer.size()))
        {
        }
    protected:
        pos_type seekoff(off_type off,
                         std::ios_base::seekdir dir,
                         [[maybe_unused]] std::ios_base::openmode which =
                             std::ios_base::in | std::ios_base::out) override
        {
            if (dir == std::ios_base::cur)
            {
                if (which & std::ios_base::in) gbump(static_cast<int>(off));
                if (which & std::ios_base::out) pbump(static_cast<int>(off));
            }
            else if (dir == std::ios_base::end)
            {
                if (which & std::ios_base::in)
                {
                    setg(eback(), egptr() + off, egptr());
                }
                if (which & std::ios_base::out)
                {
                    pbump(static_cast<int>(epptr() - pptr() + off));
                }
            }
            else if (dir == std::ios_base::beg)
            {
                if (which & std::ios_base::in)
                {
                    setg(eback(), eback() + off, egptr());
                }
                if (which & std::ios_base::out)
                {
                    pbump(static_cast<int>(pbase() - pptr() + off));
                }
            }

            return gptr() - eback();
        }

        pos_type seekpos(pos_type pos,
                         std::ios_base::openmode which =
                             std::ios_base::in | std::ios_base::out) override
        {
            return seekoff(pos, std::ios_base::beg, which);
        }
};

} // namespace
