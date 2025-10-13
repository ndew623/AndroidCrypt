/*
 *  test_logger.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the Logger object.
 *
 *  Portability Issues:
 *      None.
 */

#include <algorithm>
#include <terra/logger/logger.h>
#include <terra/logger/logger_macros.h>
#include <terra/logger/null_ostream.h>
#include <terra/conio/ansi.h>
#include <terra/stf/stf.h>

using namespace Terra::Logger;

// Function to strip timestamp from message (or more leading characters)
std::string StripTimestamp(const std::string &message, std::size_t extra = 0)
{
    // Ensure the message is longer than timestamp plus extra
    if (message.length() < 25 + extra) return {};

    return message.substr(24 + extra);
}

// Function to strip CR/LR from a string
std::string StripCRLF(const std::string &message)
{
    std::string result_string;

    // Remove any CR/LF from the message (copying after the timestamp)
    std::copy_if(message.begin(),
                 message.end(),
                 std::back_inserter(result_string),
                 [](char c) { return (c != '\r') && (c != '\n'); });

    return result_string;
}

// Function to ensure a LF is present at end of string
bool EnsureLFPresent(const std::string &message)
{
    // The last character should be a LF
    if (message.length() == 0) return false;

    return message[message.length() - 1] == '\n';
}

STF_TEST(Logger, BasicTest)
{
    std::ostringstream oss;
    LoggerPointer logger = std::make_shared<Logger>(oss);

    logger->Log("This is a test message");

    // There should be a LF at the end of the string
    STF_ASSERT_TRUE(EnsureLFPresent(oss.str()));

    // Get the logged message without a timestamp
    std::string message = StripTimestamp(oss.str());

    // There should be a CR and/or CRLF on messages, so not equal
    STF_ASSERT_NE(std::string("[INFO] This is a test message"), message);

    // Strip the CR/LF characters
    message = StripCRLF(message);

    // Verify the text of the message matches expected value
    STF_ASSERT_EQ(std::string("[INFO] This is a test message"), message);
}

STF_TEST(Logger, VerifyColor)
{
    std::ostringstream oss;
    LoggerPointer logger = std::make_shared<Logger>(oss);

    // Force color output, even for string writes
    logger->EnableColor(true);

    // Produce a critical error message
    logger->Log(LogLevel::Critical, "This is a test message");

    // There should be a LF at the end of the string
    STF_ASSERT_TRUE(EnsureLFPresent(oss.str()));

    // The critical message should have these ANSI codes at the start
    std::ostringstream red_seq;
    red_seq << Terra::ConIO::ANSI::Bold << Terra::ConIO::ANSI::Red;
    const std::string prefix = red_seq.str();

    // Ensure the string is at least as long as the prefix
    STF_ASSERT_GT(oss.str().length(), prefix.length());

    // See if the ANSI prefix exists and matches
    STF_ASSERT_EQ(0, oss.str().compare(0, prefix.length(), prefix));

    // Get the logged message without a timestamp (and leading color codes)
    std::string message = StripTimestamp(oss.str(), prefix.length());

    // Strip the CR/LF characters
    message = StripCRLF(message);

    // The message should have these ANSI codes as a suffix
    std::ostringstream reset_seq;
    reset_seq << Terra::ConIO::ANSI::Reset;
    const std::string suffix = reset_seq.str();

    // Verify the text of the message matches expected value
    STF_ASSERT_EQ(std::string("[CRITICAL] This is a test message") + suffix,
                  message);
}

STF_TEST(Logger, VerifyChildLoggers)
{
    std::ostringstream oss;
    LoggerPointer logger = std::make_shared<Logger>(oss);

    // Create child logger that inherits from parent "logger"
    LoggerPointer child_logger1 = std::make_shared<Logger>(logger, "FOO");

    // Create grandchild logger that inherits from child_logger1
    LoggerPointer child_logger2 =
        std::make_shared<Logger>(child_logger1, "BAR");

    // Log message to child_logger2
    child_logger2->Log("This is a test message");

    // There should be a LF at the end of the string
    STF_ASSERT_TRUE(EnsureLFPresent(oss.str()));

    // Get the logged message without a timestamp
    std::string message = StripTimestamp(oss.str());

    // There should be a CR and/or CRLF on messages, so not equal
    STF_ASSERT_NE(std::string("[INFO] [FOO] [BAR] This is a test message"),
                  message);

    // Strip the CR/LF characters
    message = StripCRLF(message);

    // Verify the text of the message matches expected value
    STF_ASSERT_EQ(std::string("[INFO] [FOO] [BAR] This is a test message"),
                  message);
}

STF_TEST(Logger, NullOStream)
{
    NullOStream null_stream;
    LoggerPointer logger = std::make_shared<Logger>(null_stream);

    logger->Log("Logger message that should go nowhere");
    STF_ASSERT_TRUE(null_stream.good());
    STF_ASSERT_FALSE(null_stream.bad());
    STF_ASSERT_FALSE(null_stream.eof());
    STF_ASSERT_FALSE(null_stream.fail());
}
