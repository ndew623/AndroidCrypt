/*
 *  progress_meter.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines an object that renders a progress meter via the
 *      console.  This will only work if stdout is a terminal capable of
 *      rendering color.  If stdout is not a ANSI-capable terminal, then this
 *      object produces no output.
 *
 *      If the user resizes the terminal, that will be detected the next time
 *      the progress meter is updated.
 *
 *      To render the progress meter, one creates a ProgressMeter object and
 *      calls Start().  Update may be called any number of times and the
 *      progress meter will be updated accordingly.  The user may re-size the
 *      terminal window and, if needed, the progress meter will be re-scaled
 *      to adjust to the new screen size.  Once rendering should stop,
 *      call the Stop() function or destroy the object (which will call Stop()).
 *
 *      It is assumed that once rendering starts, the ProgressMeter owns the
 *      current output line.  If there is a need to output anything else to
 *      standard out, the ProgressBar should be stopped.  It is possible to
 *      start it again.
 *
 *  Portability Issues:
 *      Using a long progress meter presents issues on Windows Terminal when
 *      attempting to resize the terminal. The issue is that if one reduces the
 *      window size, the lines wrap and do not clear properly.  This results in
 *      residue appearing on the lines (and multiple lines)  Disabling line
 *      wrapping, both via SetConsoleMode() and via ANSI sequences, did not
 *      resolve the issue.  Further, the same issue is presented when using
 *      Windows Terminal and using SSH to a remote Linux/Unix machine.  In that
 *      case, SetConsoleMode() would have no effect, anyway.  Therefore, it is
 *      best to just use relatively short progress meters to avoid the wrapping
 *      issue.
 */

#pragma once

#include <cstddef>

namespace Terra::ConIO
{

class ProgressMeter
{
    public:
        static constexpr std::size_t Default_Maximum_Width = 50;

        ProgressMeter(std::size_t length,
                      std::size_t maximum_width = Default_Maximum_Width);
        virtual ~ProgressMeter();
        bool IsRendering() const noexcept;
        void Start();
        void Update(std::size_t position);
        void Stop();

    protected:
        void DrawBlankMeter();
        void ClearLine() const;
        std::string MeterTip() const;
        std::string MeterFill() const;

        bool utf8_capable;
        const std::size_t length;
        bool render;
        bool running;
        std::size_t meter_width;
        std::size_t maximum_width;
        std::size_t last_position;
        std::size_t last_location;
};

} // namespace Terra::ConIO
