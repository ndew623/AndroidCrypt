/*
 *  logger_demo.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This is a simple test program that demonstrates how to use the Logger.
 *
 *  Portability Issues:
 *      None.
 */

#include <iostream>
#include <memory>
#include <terra/logger/logger.h>

int main()
{
    std::cout << "The following is example console output for the Logger"
              << std::endl << std::flush;

    // Instantiate a Logger object providing it with an output stream;
    // while Logger does not have to be a shared pointer, it helps when
    // creating child logger objects, as the children can hold pointers to
    // the parent Logger objects
    auto logger = std::make_shared<Terra::Logger::Logger>(std::cerr);

    // Basic Logging output using "info"
    logger->Log("Normal log message that defaults to \"info\"");

    // Log using streaming operator
    logger->info << "Normal log message using streaming operator" << std::flush;

    // The are various log levels, and if the terminal supports it, they will
    // be rendered using different colors
    logger->critical << "Example logging a critical message" << std::flush;
    logger->error << "Example logging an error message" << std::flush;
    logger->warning << "Example logging a warning message" << std::flush;
    logger->notice << "Example logging a notice message" << std::flush;
    logger->info << "Example logging an info message" << std::flush;
    logger->debug << "Example logging a debug message" << std::flush;

    // Wrap up the demo
    logger->info << "End of logging demo" << std::flush;
}
