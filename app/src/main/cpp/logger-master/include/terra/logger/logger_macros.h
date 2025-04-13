/*
 *  logger_macros.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines C preprocessor macros that can be used with the Logger
 *      to provide an alternative means calling the Logger to log messages.
 *
 *      One may utilize "compile out" debugging log messages that use the
 *      LOGGER_DEBUG macro.  This is controlled with the
 *      logger_DEBUG_MACROS_ALWAYS option defined in the Logger project build.
 *      Those using this library outside of the CMake environment may need to
 *      explicitly define LOGGER_DEBUG_ALWAYS to force building LOGGER_DEBUG()
 *      calls.
 *
 *      The logging macros use the Logger output stream members, so calls like
 *      the following are valid:
 *
 *          LOGGER_INFO(logger, "The value is " << value);
 *
 *      When using macros, one does not need to explicitly use std::flush
 *      since that is defined as a part of the macro.
 *
 *      Logger macros are optional and must be explicitly included by the user.
 *      The reason is to avoid naming conflicts in the global namespace that
 *      might be introduced by other software using the same identifiers.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#define LOGGER_CRITICAL(logger, message) \
    (logger)->critical << message << std::flush;

#define LOGGER_ERROR(logger, message) \
    (logger)->error << message << std::flush;

#define LOGGER_WARNING(logger, message) \
    (logger)->warning << message << std::flush;

#define LOGGER_NOTICE(logger, message) \
    (logger)->notice << message << std::flush;

#define LOGGER_INFO(logger, message) \
    (logger)->info << message << std::flush;

// Compile LOG_DEBUG if NOT a debug build OR if LOGGER_DEBUG_ALWAYS is defined
#if !defined(NDEBUG) || defined(LOGGER_DEBUG_ALWAYS)

#define LOGGER_DEBUG(logger, message) \
    (logger)->debug << message << std::flush;

#else

#define LOGGER_DEBUG(logger, message)

#endif
