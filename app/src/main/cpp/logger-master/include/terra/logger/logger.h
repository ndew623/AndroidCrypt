/*
 *  logger.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines the Logger object.  The Logger will emit logs
 *      to a stream (file, std::cout, etc) or syslog.  The defined log levels
 *      are a subset of those defined for the Unix syslog interface since that
 *      ensures compatibility and those provides a reasonable level of
 *      granularity.
 *
 *      The Logger utilizes a parent/child relationship in order to produce
 *      logging output that helps to improve clarity.  Each child Logger
 *      instance prepends a short user-defined string to the logging output.
 *      For instance, assume one has a heirarchy of components AAA, BBB, and
 *      CCC.  If CCC emits a log, it will have this form:
 *
 *          yyyy-mm-ddThh:mm:sss [INFO] [AAA] [BBB] [CCC] This is a log message
 *
 *      The top-most parent Logger is referred to as the root Logger object.
 *      Messages from the root logger will not have component names in brackets,
 *      but each child (or chain of child Logger objects) will work to
 *      produce this chain of component names in the logging output.
 *
 *      If one prefers to not see component strings in the logging output, then
 *      do not specify a component string (i.e., provide an empty string).
 *
 *      The precision of the timestamp defaults to milliseconds.  If
 *      microsecond-level timestamps is preferred, call SetTimePrecision()
 *      specifying TimePrecision::Microseconds.  Note, however, this only
 *      applies to the root Logger object.
 *
 *      If output is directed to the console, the output will be colorized
 *      by default if the output device appears to support color output.  If
 *      color is not wanted or should be forced on, one can enable or disable
 *      color output explicitly by calling the `EnableColor()` function
 *      when the Logger is created.
 *
 *      Only the root Logger object emits log messages and, therefore, is
 *      the only instance of the Logger where the color setting applies.
 *
 *      Each Logger instance may have different log levels specified.  This is
 *      useful when trying to debug one module without seeing a lot of verbose
 *      output from other modules.  Unless otherwise specified, each logger
 *      will have a default log level of "Info".  When creating child Logger
 *      objects, one may want to inherit the log level of the parent Logger.
 *      This can be done by passing "parent_logger->GetLogLevel()" in the
 *      constructor of the child Logger.
 *
 *      The function used to request the Logger to produce a log message is
 *      called Log(), which accepts a LogLevel value and a string.  Another
 *      Log() function is defined that does not accept a LogLevel and will
 *      use LogLevel::Info.  This is useful since Info messages are the most
 *      common and avoids the need to specify LogLevel::Info repeatedly.
 *
 *      The Logger defines several public stream objects to allow logging in
 *      this manner:
 *
 *          logger->info << "This is a log message" << std::flush;
 *
 *      When using the streaming interfaces, end logging output with std::flush
 *      instead of std::endl.  While std::endl will call flush() on the output
 *      stream, this will result in blank lines in the output.
 *
 *      One may also use LOGGER_INFO() and other macros.  Refer to the file
 *      logger_macros.h for more information.
 *
 *      Note that the EmitLogMessage() function is a virtual function.  It
 *      is defined this way to facilitate the creation of a custom Logger
 *      that might direct logging output to some destination that is not
 *      natively supported by Logger.
 *
 *  Portability Issues:
 *      Legacy Windows command prompts do not support color, though versions
 *      of Windows since Windows 10 TH2 (v1511) can be made to work.  If
 *      one wishes to get color logging in legacy terminals, first call
 *      the Terra::ConIO::EnableStdOutANSIOutput() or
 *      Terra::ConIO::EnableStdErrANSIOutput() function to enable it for
 *      the logging stream.
 */

#pragma once

#include <memory>
#include <string>
#include <ostream>
#include <cstddef>
#include "log_level.h"
#include "logger_interface.h"
#include "logger_buffer.h"

namespace Terra::Logger
{

// Define the logging facilities
enum class LogFacility
{
    Stream,                                     // Streaming output
    Syslog                                      // Linux/Unix syslog
};

// Define the time precision options
enum class TimePrecision
{
    Milliseconds,
    Microseconds
};

// Make a forward reference to the Logger class
class Logger;

// Define a shared pointer for the Logger object
using LoggerPointer = std::shared_ptr<Logger>;

// Define the Logger class
class Logger : public LoggerInterface
{
    protected:
        Logger(LoggerPointer parent_logger,
               std::string component,
               LogLevel minimum_log_level,
               LogFacility log_facility,
               std::ostream &stream);

    public:
        Logger(const std::string &identifier,
               LogLevel minimum_log_level = LogLevel::Debug);
        Logger(std::ostream &stream,
               LogLevel minimum_log_level = LogLevel::Debug);
        Logger(const LoggerPointer &parent_logger,
               const std::string &component,
               LogLevel minimum_log_level = LogLevel::Debug);
        virtual ~Logger();
        void Log(LogLevel log_level, const std::string &message) const override;
        void Log(const std::string &message) const
        {
            Log(LogLevel::Info, message);
        }

        LogFacility GetLogFacility() const { return log_facility; }
        LogLevel GetLogLevel() const { return minimum_log_level; }
        void EnableColor(bool color) { enable_color = color; };
        void SetTimePrecision(TimePrecision precision)
        {
            time_precision = precision;
        };

    protected:
        void HandleLogMessage(LogLevel log_level,
                              const std::string &message) const;
        virtual void EmitLogMessage(LogLevel log_level,
                                    const std::string &message) const;
        std::string GetCurrentTimestamp() const;

        bool enable_color;                      // Enable color (default = true)
        LogFacility log_facility;               // Logging facility
        LogLevel minimum_log_level;             // Minumum log level to log
        TimePrecision time_precision;           // Time output precision
        std::ostream &stream;                   // Stream for logging
        std::string component;                  // This Logger's component name
        LoggerPointer parent_logger;            // Pointer to parent Logger

        // Underlying buffers associated with the public streams below
        LoggerBuffer critical_buffer;
        LoggerBuffer error_buffer;
        LoggerBuffer warning_buffer;
        LoggerBuffer notice_buffer;
        LoggerBuffer info_buffer;
        LoggerBuffer debug_buffer;

    public:
        std::ostream critical;                  // Critical streaming interface
        std::ostream error;                     // Error streaming interface
        std::ostream warning;                   // Warning streaming interface
        std::ostream notice;                    // Notice streaming interface
        std::ostream info;                      // Info streaming interface
        std::ostream debug;                     // Debug streaming interface
};

} // namespace Terra::Logger
