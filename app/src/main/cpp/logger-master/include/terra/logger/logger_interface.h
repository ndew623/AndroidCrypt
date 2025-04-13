/*
 *  logger_interface.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This defines an abstract base class called LoggerInterface that
 *      serves as an interface specification for the Logger object with a
 *      single Log() function that is called by the LoggerBuffer in order to
 *      deliver messages.
 *
 *      This abstract base class exists just to work around interdependencies
 *      between LoggerBuffer and Logger.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <string>
#include "log_level.h"

namespace Terra::Logger
{

// Define the LoggerInterface class
class LoggerInterface
{
    public:
        LoggerInterface() = default;
        virtual ~LoggerInterface() = default;
        virtual void Log(LogLevel log_level,
                         const std::string &message) const = 0;
};

} // namespace Terra::Logger
