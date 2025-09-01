/*
 *  logger.cpp
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the Logger object.
 *
 *  Portability Issues:
 *      None.
 */

#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#if defined(__unix__) || defined(__APPLE__)
#include <syslog.h>
#endif
#include <terra/logger/logger.h>
#include <terra/logger/null_ostream.h>
#include <terra/conio/ansi_capable.h>
#include <terra/conio/ansi.h>

namespace Terra::Logger
{

namespace
{

/*
 *  CreateNullLogger()
 *
 *  Description:
 *      This function is used to create a parent logger that outputs to the
 *      a NullOStream when a child logger is created, but the parent logger
 *      is not provided (i.e., parent_logger equates to nullptr).
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A shared pointer to a Logger object that emits no output.
 *
 *  Comments:
 *      None.
 */
LoggerPointer CreateNullLogger()
{
    static NullOStream Null_Stream;

    return std::make_shared<Logger>(Null_Stream);
}

/*
 *  LogLevelString()
 *
 *  Description:
 *      Function to create a human readable string from the LogLevel value.
 *
 *  Parameters:
 *      log_level [in]
 *          The LogLevel value to convert to string form.
 *
 *  Returns:
 *      A human-readable string corresponding to the given LogLevel.
 *
 *  Comments:
 *      None.
 */
std::string LogLevelString(LogLevel log_level)
{
    std::string log_level_string;

    switch (log_level)
    {
        case LogLevel::Critical:
            log_level_string = "CRITICAL";
            break;

        case LogLevel::Error:
            log_level_string = "ERROR";
            break;

        case LogLevel::Warning:
            log_level_string = "WARNING";
            break;

        case LogLevel::Notice:
            log_level_string = "NOTICE";
            break;

        case LogLevel::Debug:
            log_level_string = "DEBUG";
            break;

        default:
            log_level_string = "INFO";
            break;
    }

    return log_level_string;
}

#if defined(__unix__) || defined(__APPLE__)

/*
 *  LogLevelToSyslog()
 *
 *  Description:
 *      This function will convert the LogLevel value to the corresponding
 *      value for syslog.
 *
 *  Parameters:
 *      log_level [in]
 *          The LogLevel value to convert for use by syslog.
 *
 *  Returns:
 *      Integer "priority" value for use by syslog.
 *
 *  Comments:
 *      None.
 */
int LogLevelToSyslog(LogLevel log_level)
{
    int priority{};

    switch (log_level)
    {
        case LogLevel::Critical:
            priority = LOG_CRIT;
            break;

        case LogLevel::Error:
            priority = LOG_ERR;
            break;

        case LogLevel::Warning:
            priority = LOG_WARNING;
            break;

        case LogLevel::Notice:
            priority = LOG_NOTICE;
            break;

        case LogLevel::Debug:
            priority = LOG_DEBUG;
            break;

        default:
            priority = LOG_INFO;
            break;
    }

    return priority;
}

#endif

} // namespace

/*
 *  Logger::Logger()
 *
 *  Description:
 *      Constructor for the Logger object that is called by other constructor
 *      functions to initialize all member variables consistently.  This
 *      constructor is not intended to be called directly by the user.
 *
 *  Parameters:
 *      parent_logger [in]
 *          A shared pointer to the parent logger for this child Logger object.
 *          This must NOT be a nullptr value.
 *
 *      component [in]
 *          A short (3 or 4 characters recommended) string that identifies the
 *          particular component that is the source of the log message.
 *
 *      minimum_log_level [in]
 *          The log level at which messages will be logged.  Any message with
 *          a greater log level value (i.e., lower priority) will not be logged.
 *
 *      log_facility [in]
 *          The facility to which log messages will be directed.  Those are
 *          either syslog or a stream.
 *
 *      stream [in]
 *          When logging to a stream, this is a reference to the logging stream.
 *          If logging to syslog, std::clog is passed in (though not used)
 *          just to satisfy the parameter requirement.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Logger::Logger(LoggerPointer parent_logger,
               std::string component,
               LogLevel minimum_log_level,
               LogFacility log_facility,
               std::ostream &stream) :
    LoggerInterface(),
    enable_color{false},
    log_facility{log_facility},
    minimum_log_level{minimum_log_level},
    time_precision{TimePrecision::Milliseconds},
    stream{stream},
    component{std::move(component)},
    parent_logger{std::move(parent_logger)},
    critical_buffer{LogLevel::Critical, this},
    error_buffer{LogLevel::Error, this},
    warning_buffer{LogLevel::Warning, this},
    notice_buffer{LogLevel::Notice, this},
    info_buffer{LogLevel::Info, this},
    debug_buffer{LogLevel::Debug, this},
    critical{&critical_buffer},
    error{&error_buffer},
    warning{&warning_buffer},
    notice{&notice_buffer},
    info{&info_buffer},
    debug{&debug_buffer}
{
}

/*
 *  Logger::Logger()
 *
 *  Description:
 *      Constructor for the Logger object that will direct output to syslog.
 *      This constructor will create a root Logger object.
 *
 *  Parameters:
 *      identifier [in]
 *          The identifier used when opening syslog via openlog().  If this
 *          is an empty string, nullptr will be passed to openlog() which
 *          will use the name of the executable in log messages.
 *
 *      minimum_log_level [in]
 *          The log level at which messages will be logged.  Any message with
 *          a greater log level value (i.e., lower priority) will not be logged.
 *          This parameter is optional (defaults to LogLevel::Debug).
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Logger::Logger([[maybe_unused]] const std::string &identifier,
               LogLevel minimum_log_level) :
    Logger({}, {}, minimum_log_level, LogFacility::Syslog, std::clog)
{
#if defined(__unix__) || defined(__APPLE__)
    // Open syslog for logging
    if (identifier.empty())
    {
        openlog(nullptr, 0, LOG_USER);
    }
    else
    {
        openlog(identifier.c_str(), 0, LOG_USER);
    }
#endif
}

/*
 *  Logger::Logger()
 *
 *  Description:
 *      Constructor for the Logger object that will direct output to the
 *      specified stream.  This constructor will create a root Logger object.
 *
 *  Parameters:
 *      stream [in]
 *          The stream to which log messages with be directed.  Note that this
 *          takes a reference to the stream object and it is REQUIRED that the
 *          object not be destroyed for the life of the Logger object, else
 *          Logger would cause an access violation.  The reason for this
 *          approach is that it allows the user to provide a reference to
 *          std::cout (normally stdout), std::clog (normally stderr), a file
 *          stream or even a std::ostringstream.  The latter example is
 *          useful for unit testing.
 *
 *      minimum_log_level [in]
 *          The log level at which messages will be logged.  Any message with
 *          a greater log level value (i.e., lower priority) will not be logged.
 *          This parameter is optional (defaults to LogLevel::Debug).
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Logger::Logger(std::ostream &stream, LogLevel minimum_log_level) :
    Logger({}, {}, minimum_log_level, LogFacility::Stream, stream)
{
    // Is the stream std::cout?
    if (stream.rdbuf() == std::cout.rdbuf())
    {
        // Enable color logging if the stream appears to support it
        enable_color = ConIO::IsStdOutANSICapable();
    }
    else
    {
        // Is the stream std::cerr?
        if (stream.rdbuf() == std::cerr.rdbuf())
        {
            // Enable color logging if the stream appears to support it
            enable_color = ConIO::IsStdErrANSICapable();
        }
    }
}

/*
 *  Logger::Logger()
 *
 *  Description:
 *      This constructor will create a child Logger object for the given
 *      parent Logger object.  If the parent Logger object is a nullptr,
 *      this constructor will create a parent Logger that suppresses output.
 *
 *  Parameters:
 *      parent_logger [in]
 *          A shared pointer to the parent logger for this child Logger object.
 *          If a pointer to the parent is not provided (i.e., it equates to
 *          nullptr), then a parent will be created by this function that uses
 *          the NullOStream object to effectively suppress all output.
 *
 *      component [in]
 *          A short (3 or 4 characters recommended) string that identifies the
 *          particular component that is the source of the log message.
 *
 *      minimum_log_level [in]
 *          The log level at which messages will be logged.  Any message with
 *          a greater log level value (i.e., lower priority) will not be logged.
 *          This parameter is optional (defaults to LogLevel::Debug).
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Logger::Logger(LoggerPointer parent_logger,
               const std::string &component,
               LogLevel minimum_log_level) :
    Logger(parent_logger ? std::move(parent_logger) : CreateNullLogger(),
           component,
           minimum_log_level,
           LogFacility::Inherit,
           std::clog)
{
}

/*
 *  Logger::~Logger()
 *
 *  Description:
 *      Destructor for the Logger object.
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
Logger::~Logger()
{
#if defined(__unix__) || defined(__APPLE__)
    // Only the parent logger needs to take action
    if ((!parent_logger) && (log_facility == LogFacility::Syslog)) closelog();
#endif

    // Disassociate the stream buffers from the streams
    critical.rdbuf(nullptr);
    error.rdbuf(nullptr);
    warning.rdbuf(nullptr);
    notice.rdbuf(nullptr);
    info.rdbuf(nullptr);
    debug.rdbuf(nullptr);
}

/*
 *  Logger::Log()
 *
 *  Description:
 *      This function is called by the application to log a message using the
 *      specified log level.  If the current log level is lower (meaning the
 *      requested level is of no interest), the message will not be logged.
 *
 *  Parameters:
 *      log_level [in]
 *          Log level for this particular message.
 *
 *      message [in]
 *          The message to log.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void Logger::Log(LogLevel log_level, const std::string &message) const
{
    // Log only if log level is not lower than the minimum log level value
    if (log_level <= minimum_log_level)
    {
        HandleLogMessage(log_level, message);
    }
}

/*
 *  Logger::HandleLogMessage()
 *
 *  Description:
 *      This function is called internally by the Logger to prepare the
 *      received message for logging.
 *
 *  Parameters:
 *      log_level [in]
 *          Log level for this particular message.
 *
 *      message [in]
 *          The message to log.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void Logger::HandleLogMessage(LogLevel log_level,
                              const std::string &message) const
{
    std::string logged_message;

    // If there is a component name, prepend that name
    if (!component.empty())
    {
        logged_message = "[" + component + "] " + message;
    }
    else
    {
        logged_message = message;
    }

    // Forward message to the parent logger, if there is one
    if (parent_logger)
    {
        parent_logger->HandleLogMessage(log_level, logged_message);
        return;
    }

    // The parent Logger will emit the log message
    EmitLogMessage(log_level, logged_message);
}

/*
 *  Logger::EmitLogMessage()
 *
 *  Description:
 *      This function will emit the log message to the logging facility.
 *
 *  Parameters:
 *      log_level [in]
 *          Log level for this particular message.
 *
 *      message [in]
 *          The message to log.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      This function is called only by the root Logger object in the heirarchy
 *      of parent/child Logger objects.  As such, it can safely rely on
 *      log_facility being set to something other than "Inherit".
 */
void Logger::EmitLogMessage(LogLevel log_level,
                            const std::string &message) const
{
    std::ostringstream oss;

    // Emitting to syslog?
    if (log_facility == LogFacility::Syslog)
    {
#if defined(__unix__) || defined(__APPLE__)
        syslog(LogLevelToSyslog(log_level), "%s", message.c_str());
#endif
        return;
    }

    // If using color, colorize string
    if (enable_color)
    {
        switch (log_level)
        {
            case LogLevel::Critical:
                oss << ConIO::ANSI::Bold << ConIO::ANSI::Red;
                break;

            case LogLevel::Error:
                oss << ConIO::ANSI::Bold << ConIO::ANSI::Magenta;
                break;

            case LogLevel::Warning:
                oss << ConIO::ANSI::Bold << ConIO::ANSI::Yellow;
                break;

            case LogLevel::Notice:
                oss << ConIO::ANSI::Bold << ConIO::ANSI::Blue;
                break;

            case LogLevel::Debug:
                oss << ConIO::ANSI::Gray;
                break;

            default:
                oss << ConIO::ANSI::Reset;
                break;
        }
    }

    // Add the timestamp
    oss << GetCurrentTimestamp()
        << " "
        << "["
        << LogLevelString(log_level)
        << "] "
        << message;

    // If using color, append the ANSI reset string
    if (enable_color) oss << ConIO::ANSI::Reset;

    // Append line terminator
    oss << std::endl;

    // Output the complete message
    stream << oss.str();
}

/*
 *  GetCurrentTimestamp()
 *
 *  Description:
 *      This function will return a timestamp string used by the Logger
 *      for a message to be logged.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A timestamp string to apply to logged messages or empty string if there
 *      was an error.
 *
 *  Comments:
 *      This function uses std::chrono::system_clock, which is subject to
 *      clock adjustment by functions like NTP or user adjustments to the
 *      system clock.  This is not unlike most system logging operations, but
 *      it's nonetheless worth understanding that output is based on what
 *      the system believes is the current local time.
 */
std::string Logger::GetCurrentTimestamp() const
{
    std::uint64_t sub_seconds{};
    int sub_second_digits{};
    tm local_time{};
    std::ostringstream oss;

    // Get the current time
    auto now = std::chrono::system_clock::now();

    // Compute the microseconds or milliseconds given the desired precision
    if (time_precision == TimePrecision::Microseconds)
    {
        sub_second_digits = 6;
        sub_seconds =
            std::chrono::time_point_cast<std::chrono::microseconds>(now)
                .time_since_epoch()
                .count() % 1'000'000;
    }
    else
    {
        sub_second_digits = 3;
        sub_seconds =
            std::chrono::time_point_cast<std::chrono::milliseconds>(now)
                .time_since_epoch()
                .count() % 1'000;
    }

    // Populate the time_t structure given the current time
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    // Convert the time_t value to the local time
#ifdef _WIN32
    if (localtime_s(&local_time, &time) != 0) return {};
#else
    if (localtime_r(&time, &local_time) == nullptr) return {};
#endif

    // Output the timestamp string
    oss << std::put_time(&local_time, "%FT%T")
        << "."
        << std::setfill('0')
        << std::setw(sub_second_digits)
        << sub_seconds;

    return oss.str();
}

} // namespace Terra::Logger
