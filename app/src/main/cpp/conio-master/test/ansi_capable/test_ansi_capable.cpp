/*
 *  test_ansi_capable.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test the ansi_capable function.
 *
 *  Portability Issues:
 *      None.
 */

#ifdef _WIN32
#include <io.h>     // For _isatty()
#include <stdio.h>  // For _fileno()
#elif defined(__unix__) || defined(__APPLE__)
#include <unistd.h> // For isatty()
#include <stdlib.h> // For getenv()
#include <string.h> // For strcmp()
#endif
#include <terra/conio/ansi_capable.h>
#include <terra/stf/stf.h>

STF_TEST(ANSICapable, NonExistentANSICapable)
{
    // Device descriptor 3 should result in false
    STF_ASSERT_FALSE(Terra::ConIO::IsANSICapable(3));
}

STF_TEST(ANSICapable, StdOutANSICapable)
{
    // If running from an actual TTY, then the results should be true, else
    // they should be false
    bool expected = false;

#if defined(_WIN32)
    // If this is a TTY, then return true
    if (_isatty(_fileno(stdout))) expected = true;
#elif defined(__unix__) || defined(__APPLE__)
    // If this is not a TTY, return false
    if (isatty(STDOUT_FILENO)) expected = true;
#endif

    // Check the expected results
    STF_ASSERT_EQ(expected, Terra::ConIO::IsStdOutANSICapable());
}
