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

#ifdef _WIN32
#include <Windows.h>    // For GetStdHandle()
#include <io.h>         // For _isatty()
#include <stdio.h>      // For _fileno()
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
#include <unistd.h>     // For isatty()
#include <cstdlib>      // For getenv()
#include <cstring>      // For strcmp()
#include <sys/ioctl.h>
#endif
#include <terra/conio/utilities.h>

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
bool IsTerminal(int fd)
{
#if defined(_WIN32)

    // If this is a terminal / TTY, then return true
    return (_isatty(fd) != 0);

#elif defined (__linux__) || defined(__FreeBSD__) || defined(__APPLE__)

    // If this is a terminal / TTY, return true
    return (isatty(fd) != 0);

#endif
}

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
bool IsStdOutTerminal()
{
#if defined(_WIN32)

    return IsTerminal(_fileno(stdout));

#elif defined (__linux__) || defined(__FreeBSD__) || defined(__APPLE__)

    return IsTerminal(STDOUT_FILENO);

#else

    return false;

#endif
}

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
bool IsStdErrTerminal()
{
#if defined(_WIN32)

    return IsTerminal(_fileno(stderr));

#elif defined (__linux__) || defined(__FreeBSD__) || defined(__APPLE__)

    return IsTerminal(STDERR_FILENO);

#else

    return false;
#endif
}

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
std::pair<std::size_t, std::size_t> GetTerminalDimensions()
{
#if defined(_WIN32)

    CONSOLE_SCREEN_BUFFER_INFO screen_buffer;

    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
                                   &screen_buffer))
    {
        return {screen_buffer.srWindow.Right - screen_buffer.srWindow.Left + 1,
                screen_buffer.srWindow.Bottom - screen_buffer.srWindow.Top + 1};
    }

    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE),
                                   &screen_buffer))
    {
        return {screen_buffer.srWindow.Right - screen_buffer.srWindow.Left + 1,
                screen_buffer.srWindow.Bottom - screen_buffer.srWindow.Top + 1};
    }

#elif defined (__linux__) || defined(__FreeBSD__) || defined(__APPLE__)

    struct winsize window_size{};

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == 0)
    {
        return {window_size.ws_col, window_size.ws_row};
    }
    if (ioctl(STDERR_FILENO, TIOCGWINSZ, &window_size) == 0)
    {
        return {window_size.ws_col, window_size.ws_row};
    }

#endif

    return {0, 0};
}

} // namespace Terra::ConIO
