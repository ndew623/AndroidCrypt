/*
 *  ansi_capable.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements a function to determine if the standard ouput
 *      device (stdout) is ANSI-capable.
 *
 *  Portability Issues:
 *      None.
 */

#ifdef _WIN32
#include <Windows.h>
#include <stdio.h>      // For _fileno()
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
#include <unistd.h>     // For STDOUT_FILENO / STDERR_FILENO
#include <cstdlib>      // For getenv()
#include <cstring>      // For strcmp()
#endif
#include <terra/conio/ansi_capable.h>
#include <terra/conio/utilities.h>

namespace Terra::ConIO
{

/*
 *  IsANSICapable()
 *
 *  Description:
 *      This function will determine if it is possible to colorize (or
 *      otherwise send ANSI escape sequences) to the specified output device.
 *      This function checks to see if the file descriptor is directed to
 *      a terminal and, if it is, whether or not it's a dumb terminal.  Anything
 *      other than a dumb terminal is assumed to be ANSI-capable.  This
 *      approach may not work for legacy devices, but it works for all modern
 *      systems when the routine was written, including Linux/Unix, Windows, and
 *      Mac.
 *
 *  Parameters:
 *      fd [in]
 *          The file descriptor to check.  By default, this will be stdout.
 *          One may wish to separate stdout from stderr, for example, and thus
 *          the reason for specifying the file descriptor.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      The logic employed is similar to what is used by the GNU grep utility,
 *      where there are "colorize" files that can be found here:
 *          https://git.savannah.gnu.org/cgit/grep.git/tree/lib
 *      And the GNU "ls" command, whose source is here:
 *          https://github.com/coreutils/coreutils
 *      There is a check to see if stdout is a TTY device.  In the case of
 *      Linux/Unix/Apple, there is also a check to see if the TTY device is
 *      something other than a dumb terminal.
 *
 *      On Windows, legacy command-prompt (e.g., cmd.exe) does not support ANSI
 *      output by default.  One must first call EnableStdoutANSIOutput()
 *      or EnableStderrANSIOutput() to enable virtual terminal processing.
 *      However, this function can then be used to verify that it is enabled.
 *
 *      On Linux/Unix/Mac, this function may not be thread safe since it calls
 *      getenv().
 */
bool IsANSICapable(int fd)
{
#if defined(_WIN32)

    HANDLE handle_to_console{};
    DWORD console_mode{};

    // If this is not a terminal, return false
    if (!IsTerminal(fd)) return false;

    // Get the handle associated with the fd
    if (fd == _fileno(stdout))
    {
        // We need to ensure the terminal outputs ANSI codes
        handle_to_console = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    else if (fd == _fileno(stderr))
    {
        // We need to ensure the terminal outputs ANSI codes
        handle_to_console = GetStdHandle(STD_ERROR_HANDLE);
    }
    else
    {
        return false;
    }

    // Check that virtual terminal processing is enabled (has ANSI support)
    if (GetConsoleMode(handle_to_console, &console_mode))
    {
        return (console_mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
    }

    return false;

#elif defined (__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
    // If this is not a terminal, return false
    if (!IsTerminal(fd)) return false;

    // Check the terminal type
    const char *term_type = getenv("TERM");

    // If TERM is defined and it's not "dumb", one can ANSI the output
    return ((term_type != nullptr) && (strcmp(term_type, "dumb") != 0));

#else

    return false;

#endif
}

/*
 *  IsStdOutANSICapable()
 *
 *  Description:
 *      This function calls IsANSICapable() to check to see if the device
 *      associated with standard output is ANSI-capable.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      True if ANSI-capable, false if not.
 *
 *  Comments:
 *      None.
 */
bool IsStdOutANSICapable()
{
#if defined(_WIN32)
    return IsANSICapable(_fileno(stdout));
#elif defined (__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
    return IsANSICapable(STDOUT_FILENO);
#else
    return false;
#endif
}

/*
 *  IsStdErrANSICapable()
 *
 *  Description:
 *      This function calls IsANSICapable() to check to see if the device
 *      associated with standard error is ANSI-capable.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      True if ANSI-capable, false if not.
 *
 *  Comments:
 *      None.
 */
bool IsStdErrANSICapable()
{
#if defined(_WIN32)
    return IsANSICapable(_fileno(stderr));
#elif defined (__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
    return IsANSICapable(STDERR_FILENO);
#else
    return false;
#endif
}

/*
 *  EnableStdOutANSIOutput()
 *
 *  Description:
 *      Legacy Windows console windows (e.g., cmd.exe) do not have the
 *      capability to render ANSI control codes, thus are not ANSI-capable.
 *      However, it can be enabled in Windows 10 TH2 (v1511) and newer systems.
 *      This function will enable ANSI control codes sent to stdout.
 *
 *      On non-Windows platforms, this function will call the function to see
 *      if the terminal is ANSI-capable and return that as the result.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      True if successful, false if not.
 *
 *  Comments:
 *      None.
 */
bool EnableStdOutANSIOutput()
{
#ifdef _WIN32
    DWORD console_mode;

    // Get a handle to the output stream
    HANDLE handle_to_console = GetStdHandle(STD_OUTPUT_HANDLE);

    // Get the current mode
    if (GetConsoleMode(handle_to_console, &console_mode))
    {
        // Enable virtual terminal processing for ANSI
        console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        // Attempt to enable virtual terminal processing
        SetConsoleMode(handle_to_console, console_mode);
    }
#endif

    return IsStdOutANSICapable();
}

/*
 *  EnableStderrANSIOutput()
 *
 *  Description:
 *      Legacy Windows console windows (e.g., cmd.exe) do not have the
 *      capability to render ANSI control codes, thus are not ANSI-capable.
 *      However, it can be enabled in Windows 10 TH2 (v1511) and newer systems.
 *      This function will enable ANSI control codes sent to stderr.
 *
 *      On non-Windows platforms, this function will call the function to see
 *      if the terminal is ANSI-capable and return that as the result.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      True if successful, false if not.
 *
 *  Comments:
 *      None.
 */
bool EnableStdErrANSIOutput()
{
#ifdef _WIN32
    DWORD console_mode;

    // Get a handle to the output stream
    HANDLE handle_to_console = GetStdHandle(STD_ERROR_HANDLE);

    // Get the current mode
    if (GetConsoleMode(handle_to_console, &console_mode))
    {
        // Enable virtual terminal processing for ANSI
        console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        // Attempt to enable virtual terminal processing
        if (SetConsoleMode(handle_to_console, console_mode)) return true;
    }
#endif

    return IsStdErrANSICapable();
}

} // namespace Terra::ConIO
