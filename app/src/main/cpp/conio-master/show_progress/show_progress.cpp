/*
 *  show_progress.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This program shows usage of the ProgressMeter class and is intended
 *      both for demonstration of use and verification of functionality.
 *
 *  Portability Issues:
 *      None.
 */

#ifdef _WIN32
#include <Windows.h>
#endif
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <signal.h>
#include <utility>
#include <cstddef>
#include <terra/conio/progress_meter.h>
#include <terra/conio/utilities.h>

namespace
{
    std::atomic<bool> Terminate = false;
}

extern "C"
{

void SignalHandler(int)
{
    // Regardless of signal received, signal program termination
    Terminate = true;
}

}

/*
 *  InstallSignalHandlers()
 *
 *  Description:
 *      This function will install signal handlers to ensure a graceful exit.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void InstallSignalHandlers()
{
#ifdef _WIN32
    signal(SIGABRT, SignalHandler);
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
#else
    struct sigaction sa;

    sa.sa_handler = &SignalHandler;             // Specify the signal handler
    sa.sa_flags = SA_RESTART;                   // Restart system calls
    sigfillset(&sa.sa_mask);                    // Block all other signals

    // Signals to handle
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
#endif
}

int main()
{
#ifdef _WIN32
    // On Windows, use UTF-8 for input/output to console
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#else
    // Get the user's locale for character classification from the environment
    // NOTE: This call is so that the ProgressMeter class can check to see if
    //       UTF-8 character encoding is used.  The progress meter will be
    //       rendered using nicer Unicode characters if this is called and the
    //       character encoding is UTF-8.  Otherwise, it will render using
    //       using plain ASCII characters.
    std::setlocale(LC_CTYPE, "");
#endif

    // Install the signal handlers
    InstallSignalHandlers();

    std::cout << "This program will demonstrate a progress meter render"
              << std::endl;

    // Say the "size" is 500, whatever this might be.
    Terra::ConIO::ProgressMeter progress_meter(500);

    // Show the terminal window dimensions
    auto dimensions = Terra::ConIO::GetTerminalDimensions();
    std::cout << "Screen size: " << dimensions.first << " x "
              << dimensions.second << std::endl;

    // Start rendering
    progress_meter.Start();

    for (std::size_t i = 1; i <= 500; i++)
    {
        if (Terminate) break;
        progress_meter.Update(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (!Terminate)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // Stop rendering
    progress_meter.Stop();

    std::cout << "Final line replaces the meter" << std::endl;

    // Show the terminal window dimensions (again)
    dimensions = Terra::ConIO::GetTerminalDimensions();
    std::cout << "Screen size: " << dimensions.first << " x "
              << dimensions.second << std::endl;
}
