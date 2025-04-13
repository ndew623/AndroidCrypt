/*
 *  progress_meter.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements an object that renders a progress meter via the
 *      console.  This will only work if stdout is a terminal ANSI-capable.
 *      If stdout is not a ANSI-capable terminal, then this object produces no
 *      output.
 *
 *  Portability Issues:
 *      None.
 */

#include <iostream>
#include <algorithm>
#if defined (__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
#include <langinfo.h>
#elif defined (_WIN32)
#define NOMINMAX
#include <Windows.h>
#endif
#include <terra/conio/progress_meter.h>
#include <terra/conio/utilities.h>
#include <terra/conio/ansi.h>
#include <terra/conio/ansi_capable.h>

namespace Terra::ConIO
{

namespace
{

// Define progress bar character components
constexpr std::size_t Minimum_Width = 3;
constexpr char Meter_Space = ' ';
constexpr char Carriage_Return = '\r';
constexpr char Start_Character = '[';
constexpr char End_Character = ']';
constexpr char Backspace = 0x08;

} // namespace

/*
 *  ProgressMeter::ProgressMeter()
 *
 *  Description:
 *      Constructor for the ProgressMeter object.
 *
 *  Parameters:
 *      length [in]
 *          The length of the input that will be processed that will dictate
 *          rendering of the progress meter. This might be a file length or
 *          period of time, for example.  The ProgressMeter will use this
 *          along with the value passed to Update() to determine how to
 *          render the progress meter.
 *
 *      maximum_width [in]
 *          The maximum width of the progress meter.  This could be set to
 *          std::numeric_limits<std::size_t>::max() to ensure the meter is
 *          rendered across the entire terminal line.  Long meters can be
 *          visually less appealing.  The default meter length is much smaller
 *          than the typical terminal window.  The actual meter length
 *          will be the minimum of this value and the width of the screen - 2.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      On Linux/Unix/Mac, this function may not be thread safe since it calls
 *      nl_langinfo().
 */
ProgressMeter::ProgressMeter(std::size_t length,
                             std::size_t maximum_width) :
    utf8_capable{false},
    length{length},
    render{false},
    running{false},
    meter_width{0},
    maximum_width{maximum_width},
    last_position{0},
    last_location{0}
{
    // Do not attempt to render on zero length
    if (length == 0) return;

    // If unable to enable or verify ANSI support, just return
    if (!EnableStdOutANSIOutput()) return;

    // Get the width of the terminal window
    meter_width = std::min(GetTerminalDimensions().first, maximum_width);

    // If the screen cannot support the meter, turn off rendering
    render = (meter_width >= Minimum_Width);

#ifdef _WIN32
    utf8_capable = (GetConsoleOutputCP() == CP_UTF8);
#else
    // Determine if the locale supports UTF-8 output
    const std::string encoding = "UTF-8";//nl_langinfo(CODESET);
    utf8_capable = (encoding == "UTF-8");
#endif
}

/*
 *  ProgressMeter::~ProgressMeter()
 *
 *  Description:
 *      Destructor for the ProgressMeter object.
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
ProgressMeter::~ProgressMeter()
{
    if (running) Stop();
}

/*
 *  ProgressMeter::IsRendering()
 *
 *  Description:
 *      This will indicate whether the ProgressMeter is rendering (or will
 *      once started).  There are some cases (e.g., older Windows console
 *      windows or dumb terminals) that cannot render the progress bar and,
 *      in those cases, this function will return false.  This will also
 *      be false for other reasons, but in all cases it will indicate the
 *      present state of the "render" variable that indicates current rendering
 *      ability.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      True if the ProgressMeter is or can render output, false if not.
 *
 *  Comments:
 *      None.
 */
bool ProgressMeter::IsRendering() const noexcept
{
    return render;
}

/*
 *  ProgressMeter::Start()
 *
 *  Description:
 *      This will start the progress meter rendering, if rendering is possible.
 *      The current cursor line will be used for rendering.
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
void ProgressMeter::Start()
{
    // If not rendering or already running, just return
    if (!render || running) return;

    // Draw a blank meter
    DrawBlankMeter();

    // If rending, note the running state and hide the cursor
    if (render)
    {
        running = true;
        std::cout << ANSI::HideCursor() << std::flush;
    }
}

/*
 *  ProgressMeter::Update()
 *
 *  Description:
 *      This will update the progress meter to depict the current location as
 *      indicated by the position parameter.
 *
 *  Parameters:
 *      position [in]
 *          Current data position, which should be in the range of
 *          1 .. length, where "length" is the length of the data
 *          being processed and as reported in the object constructor.
 *          A value of zero will be treated as meaning there is no progress
 *          to show, while a value > length will be set to length.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void ProgressMeter::Update(std::size_t position)
{
    bool redraw_required = false;

    // If not running, just return
    if (!running) return;

    // If the position is beyond the length, adjust the position
    if (position > length) position = length;

    // Get the width of the terminal window (as this might have changed)
    std::size_t new_meter_width =
        std::min(GetTerminalDimensions().first, maximum_width);

    // If the screen size changed or the position is less than before,
    // re-draw the progress meter
    if ((new_meter_width != meter_width) || (position < last_position))
    {
        // Clear the line
        ClearLine();

        // Since the screen width changed, re-draw a blank meter
        DrawBlankMeter();

        // Note that a redraw is required
        redraw_required = true;

        // Return if no longer able to render
        if (!render) return;
    }

    // Determine the meter tip location (note this may be one beyond the
    // meter length, in which case there is no tip to show)
    std::size_t location = static_cast<std::size_t>(
        ((static_cast<double>(position) / static_cast<double>(length)) *
         (static_cast<double>(meter_width) - 2)) +
        1);

    // If the meter tip position did not change and redraw not required, return
    if (!redraw_required && (location == last_location)) return;

    // If an initial draw or redraw is required, fill in the meter
    if (redraw_required || (last_location == 0))
    {
        // Move the cursor to the start of the line, emit the start character
        std::cout << Carriage_Return << Start_Character;

        // Fill area before tip location
        for (std::size_t i = 1; i < location; i++) std::cout << MeterFill();

        // Render meter tip if we will not go beyond the drawable area
        if (location <= meter_width - 2)
        {
            std::cout << MeterTip() << Backspace;
        }
    }
    else
    {
        // Insert fill characters starting at the current cursor location
        while (last_location++ < location) std::cout << MeterFill();

        // Render meter tip if we will not go beyond the drawable area
        if (location <= meter_width - 2)
        {
            std::cout << MeterTip() << Backspace;
        }
    }

    // Flush the output (so there is actual rendering)
    std::cout << std::flush;

    // Take note of the last position
    last_position = position;

    // Note the updated last_location value
    last_location = location;
}

/*
 *  ProgressMeter::Stop()
 *
 *  Description:
 *      This will stop the progress meter rendering, which causes the line to
 *      clear and cursor placed at the start of the line.
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
void ProgressMeter::Stop()
{
    // If not running, just return
    if (!running) return;

    // Note that the progress bar is not running
    running = false;

    // If not rendering anymore (not usual, but can happen on terminal resize)
    if (!render) return;

    // Clear the progress meter line (if possible)
    ClearLine();

    // Show the cursor
    std::cout << ANSI::ShowCursor() << std::flush;
}

/*
 *  ProgressMeter::DrawBlankMeter()
 *
 *  Description:
 *      This function will draw a blank progress meter (having only the start,
 *      end, and "space" characters).  This will not flush the output sent to
 *      stdout.  The caller should do that if no other drawing is required.
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
void ProgressMeter::DrawBlankMeter()
{
    // Get the width of the terminal window (as this might have changed!)
    meter_width = std::min(GetTerminalDimensions().first, maximum_width);

    // If the screen cannot support the meter, turn off rendering
    render = (meter_width >= Minimum_Width);

    // If not rendering, return
    if (!render) return;

    // This will draw an empty meter, with the cursor left at the start of
    // the meter (right after the start character)
    std::cout << Carriage_Return
              << Start_Character
              << std::string(meter_width - 2, Meter_Space)
              << End_Character
              << Carriage_Return
              << Start_Character;
}

/*
 *  ProgressMeter::ClearLine()
 *
 *  Description:
 *      This function will clear the progress bar line and move the cursor
 *      to the left-hand side of the line.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      This will not flush the output stream, as it is assumed additional
 *      output will follow.  If there is no additional output, the caller should
 *      flush the output stream.
 */
void ProgressMeter::ClearLine() const
{
    // If not rendering, return
    if (!render) return;

    // Clear the terminal line
    std::cout << ANSI::ClearLineAll();

    // Move the cursor to the start of the line
    std::cout << Carriage_Return;
}

/*
 *  ProgressMeter::MeterTip()
 *
 *  Description:
 *      This function returns the octets to use to render the progress meter's
 *      tip.  If the terminal supports UTF-8 output, the character that will be
 *      used is a right pointing triangle (\u25B8).  Otherwise, an '=' sign
 *      is used to fill the meter.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      The character sequence to use for the progress meter.
 *
 *  Comments:
 *      None.
 */
std::string ProgressMeter::MeterTip() const
{
    // Render using right-pointing triangle character when using UTF-8
    if (utf8_capable) return "\xe2\x96\xb8";

    return ">";
}

/*
 *  ProgressMeter::MeterFill()
 *
 *  Description:
 *      This function returns the octets to use to render the progress meter's
 *      actual meter.  If the terminal supports UTF-8 output, the character
 *      that will be used is a black square (\u25A0).  Otherwise, an '=' sign
 *      is used to fill the meter.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      The character sequence to use for the progress meter.
 *
 *  Comments:
 *      None.
 */
std::string ProgressMeter::MeterFill() const
{
    // Render using a block character when using UTF-8
    if (utf8_capable) return ANSI::Blue() + "\xe2\x96\xa0" + ANSI::Reset();

    return ANSI::Blue() + "=" + ANSI::Reset();
}

} // namespace Terra::ConIO
