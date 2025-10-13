/*
 *  log_level.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      Defines the enumeration LogLevel with values that largely correspond
 *      to syslog.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

namespace Terra::Logger
{

enum class LogLevel
{
    Critical,                                   // Critical error
    Error,                                      // Non-critical error
    Warning,                                    // Warning
    Notice,                                     // Significant notification
    Info,                                       // Normal message
    Debug                                       // Debug message
};

} // namespace Terra::Logger
