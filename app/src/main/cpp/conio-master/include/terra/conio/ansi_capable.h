/*
 *  ansi_capable.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines a function to determine if the standard output device
 *      (stdout) is ANSI-capable.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

namespace Terra::ConIO
{

/*
 *  IsANSICapable()
 *
 *  Description:
 *      This function will determine if it is possible to send ANSI escape
 *      sequences to the specified output device.  This function checks to see
 *      if the file descriptor is directed to a terminal and, if it is, whether
 *      or not it's a dumb terminal.  On Linux/Unix, anything other than a dumb
 *      terminal is assumed to be ANSI-capable.  This approach may not work for
 *      legacy devices, but it works for all modern systems when the routine
 *      was written, including Linux/Unix, Windows, and Mac.
 *
 *  Parameters:
 *      fd [in]
 *          The file descriptor to check.
 *
 *  Returns:
 *      True if ANSI-capable, false if not.
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
 */
bool IsANSICapable(int fd);

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
bool IsStdOutANSICapable();

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
bool IsStdErrANSICapable();

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
bool EnableStdOutANSIOutput();

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
bool EnableStdErrANSIOutput();

} // namespace Terra::ConIO
