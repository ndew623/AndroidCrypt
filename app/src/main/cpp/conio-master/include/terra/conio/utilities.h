/*
 *  utilities.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines a few utility functions useful for console I/O.
 *
 *  Portability Issues:
 *      See comments on functions.
 */

#pragma once

#include <cstddef>
#include <utility>

namespace Terra::ConIO
{

/*
 *  IsTerminal()
 *
 *  Description:
 *      This function will determine if the specifed file descriptor is
 *      associated with a terminal / TTY.
 *
 *  Parameters:
 *      fd [in]
 *          The file descriptor to check.
 *
 *  Returns:
 *      True if the file descriptor is associated with a terminal, false if not.
 *
 *  Comments:
 *      This will also return false if the platform is not supported.  Supported
 *      platforms include Windows, Linux/Unix, and Mac.
 */
bool IsTerminal(int fd);

/*
 *  IsStdOutTerminal()
 *
 *  Description:
 *      This function will determine if stdout is associated with a terminal.
 *
 *  Parameters:
 *     None.
 *
 *  Returns:
 *      True if stdout is associated with a terminal, false if not.
 *
 *  Comments:
 *      This will also return false if the platform is not supported.  Supported
 *      platforms include Windows, Linux/Unix, and Mac.
 */
bool IsStdOutTerminal();

/*
 *  IsStdErrTerminal()
 *
 *  Description:
 *      This function will determine if stderr is associated with a terminal.
 *
 *  Parameters:
 *     None.
 *
 *  Returns:
 *      True if stderr is associated with a terminal, false if not.
 *
 *  Comments:
 *      This will also return false if the platform is not supported.  Supported
 *      platforms include Windows, Linux/Unix, and Mac.
 */
bool IsStdErrTerminal();

/*
 *  GetTerminalDimensions()
 *
 *  Description:
 *      This function will return the width and height of the terminal window.
 *      It uses the "standard out" file descriptor / handle to determine the
 *      screen size.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A std::pair containing the screen width and height. If the values
 *      could not be determined or if stdout is not associated with a terminal,
 *      then the values {0, 0} will be returned.
 *
 *  Comments:
 *      None.
 */
std::pair<std::size_t, std::size_t> GetTerminalDimensions();

}
