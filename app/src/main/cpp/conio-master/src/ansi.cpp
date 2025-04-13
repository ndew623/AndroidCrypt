/*
 *  ansi.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module implements the function to produce the various ANSI
 *      escape sequences to reset the terminal and produce color output.
 *
 *  Portability Issues:
 *      None.
 */

#include <terra/conio/ansi.h>

namespace Terra::ConIO::ANSI
{

namespace
{

const char *CSI = "\033[";
const char *SGRT = "m";

}

/*
 *  Reset()
 *
 *  Description:
 *      Produces terminal reset sequence.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string Reset()
{
    return std::string(CSI) + "0" + SGRT;
}

/*
 *  Bold()
 *
 *  Description:
 *      Cause text to be bold.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string Bold()
{
    return std::string(CSI) + "1" + SGRT;
}

/*
 *  Faint()
 *
 *  Description:
 *      Cause text to be faint.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string Faint()
{
    return std::string(CSI) + "2" + SGRT;
}

/*
 *  Italic()
 *
 *  Description:
 *      Cause text to be italic.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string Italic()
{
    return std::string(CSI) + "3" + SGRT;
}

/*
 *  Underline()
 *
 *  Description:
 *      Cause text to be underlined.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string Underline()
{
    return std::string(CSI) + "4" + SGRT;
}

/*
 *  SlowBlink()
 *
 *  Description:
 *      Cause text to blink slowly.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string SlowBlink()
{
    return std::string(CSI) + "5" + SGRT;
}

/*
 *  RapidBlink()
 *
 *  Description:
 *      Cause text to blink rapidly.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string RapidBlink()
{
    return std::string(CSI) + "6" + SGRT;
}

/*
 *  ReverseVideo()
 *
 *  Description:
 *      Cause text to show in reverse video.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string ReverseVideo()
{
    return std::string(CSI) + "7" + SGRT;
}

/*
 *  Conceal()
 *
 *  Description:
 *      Cause text to be concealed.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string Conceal()
{
    return std::string(CSI) + "8" + SGRT;
}

/*
 *  Strike()
 *
 *  Description:
 *      Cause text to be strikethrough.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string Strike()
{
    return std::string(CSI) + "9" + SGRT;
}

/*
 *  DoubleUnderline()
 *
 *  Description:
 *      Cause text to be double underlined.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string DoubleUnderline()
{
    return std::string(CSI) + "21" + SGRT;
}

/*
 *  NormalIntensity()
 *
 *  Description:
 *      Cause text to be normal intensity.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string NormalIntensity()
{
    return std::string(CSI) + "22" + SGRT;
}

/*
 *  NormalFont()
 *
 *  Description:
 *      Cause text to be a normal font (not italic).
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string NormalFont()
{
    return std::string(CSI) + "23" + SGRT;
}

/*
 *  NoUnderline()
 *
 *  Description:
 *      Cause text to not be underlined.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string NoUnderline()
{
    return std::string(CSI) + "24" + SGRT;
}

/*
 *  NoBlink()
 *
 *  Description:
 *      Cause text to not blink.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string NoBlink()
{
    return std::string(CSI) + "25" + SGRT;
}

/*
 *  NoReverseVideo()
 *
 *  Description:
 *      Cause text to not show in reverse video.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string NoReverseVideo()
{
    return std::string(CSI) + "27" + SGRT;
}

/*
 *  NoConceal()
 *
 *  Description:
 *      Cause text to not be concealed.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string NoConceal()
{
    return std::string(CSI) + "28" + SGRT;
}

/*
 *  NoStrike()
 *
 *  Description:
 *      Cause text to not be strikethrough.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string with the specified command sequence.
 *
 *  Comments:
 *      None.
 */
std::string NoStrike()
{
    return std::string(CSI) + "29" + SGRT;
}

/*
 *  Black()
 *
 *  Description:
 *      Function to return ANSI sequence to produce black text.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the foreground color.
 *
 *  Comments:
 *      None.
 */
std::string Black()
{
    return std::string(CSI) + "30" + SGRT;
}

/*
 *  Gray()
 *
 *  Description:
 *      Function to return ANSI sequence to produce gray text.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the foreground color.
 *
 *  Comments:
 *      Gray is actually "black bold", so note that using this turns on bold
 *      intensity.
 */
std::string Gray()
{
    return Bold() + Black();
}

/*
 *  Red()
 *
 *  Description:
 *      Function to return ANSI sequence to produce red text.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the foreground color.
 *
 *  Comments:
 *      None.
 */
std::string Red()
{
    return std::string(CSI) + "31" + SGRT;
}

/*
 *  Green()
 *
 *  Description:
 *      Function to return ANSI sequence to produce green text.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the foreground color.
 *
 *  Comments:
 *      None.
 */
std::string Green()
{
    return std::string(CSI) + "32" + SGRT;
}

/*
 *  Yellow()
 *
 *  Description:
 *      Function to return ANSI sequence to produce yellow text.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the foreground color.
 *
 *  Comments:
 *      None.
 */
std::string Yellow()
{
    return std::string(CSI) + "33" + SGRT;
}

/*
 *  Blue()
 *
 *  Description:
 *      Function to return ANSI sequence to produce blue text.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the foreground color.
 *
 *  Comments:
 *      None.
 */
std::string Blue()
{
    return std::string(CSI) + "34" + SGRT;
}

/*
 *  Magenta()
 *
 *  Description:
 *      Function to return ANSI sequence to produce magenta text.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the foreground color.
 *
 *  Comments:
 *      None.
 */
std::string Magenta()
{
    return std::string(CSI) + "35" + SGRT;
}

/*
 *  Cyan()
 *
 *  Description:
 *      Function to return ANSI sequence to produce cyan text.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the foreground color.
 *
 *  Comments:
 *      None.
 */
std::string Cyan()
{
    return std::string(CSI) + "36" + SGRT;
}

/*
 *  White()
 *
 *  Description:
 *      Function to return ANSI sequence to produce white text.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the foreground color.
 *
 *  Comments:
 *      None.
 */
std::string White()
{
    return std::string(CSI) + "37" + SGRT;
}

/*
 *  Default()
 *
 *  Description:
 *      Function to return ANSI sequence to produce the default text color.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the foreground color.
 *
 *  Comments:
 *      None.
 */
std::string Default()
{
    return std::string(CSI) + "39" + SGRT;
}

/*
 *  BlackBackground()
 *
 *  Description:
 *      Function to return ANSI sequence to produce a black background.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the background color.
 *
 *  Comments:
 *      None.
 */
std::string BlackBackground()
{
    return std::string(CSI) + "40" + SGRT;
}

/*
 *  RedBackground()
 *
 *  Description:
 *      Function to return ANSI sequence to produce a red background.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the background color.
 *
 *  Comments:
 *      None.
 */
std::string RedBackground()
{
    return std::string(CSI) + "41" + SGRT;
}

/*
 *  GreenBackground()
 *
 *  Description:
 *      Function to return ANSI sequence to produce a green background.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the background color.
 *
 *  Comments:
 *      None.
 */
std::string GreenBackground()
{
    return std::string(CSI) + "42" + SGRT;
}

/*
 *  YellowBackground()
 *
 *  Description:
 *      Function to return ANSI sequence to produce a yellow background text.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the background color.
 *
 *  Comments:
 *      None.
 */
std::string YellowBackground()
{
    return std::string(CSI) + "43" + SGRT;
}

/*
 *  BlueBackground()
 *
 *  Description:
 *      Function to return ANSI sequence to produce a blue background.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the background color.
 *
 *  Comments:
 *      None.
 */
std::string BlueBackground()
{
    return std::string(CSI) + "44" + SGRT;
}

/*
 *  MagentaBackground()
 *
 *  Description:
 *      Function to return ANSI sequence to produce a magenta background.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the background color.
 *
 *  Comments:
 *      None.
 */
std::string MagentaBackground()
{
    return std::string(CSI) + "45" + SGRT;
}

/*
 *  CyanBackground()
 *
 *  Description:
 *      Function to return ANSI sequence to produce a cyan background.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the background color.
 *
 *  Comments:
 *      None.
 */
std::string CyanBackground()
{
    return std::string(CSI) + "46" + SGRT;
}

/*
 *  WhiteBackground()
 *
 *  Description:
 *      Function to return ANSI sequence to produce a white background.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the background color.
 *
 *  Comments:
 *      None.
 */
std::string WhiteBackground()
{
    return std::string(CSI) + "47" + SGRT;
}

/*
 *  HideCursor()
 *
 *  Description:
 *      Function to return ANSI sequence to hide the cursor.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to hide the cursor.
 *
 *  Comments:
 *      None.
 */
std::string HideCursor()
{
    return std::string(CSI) + "?25l";
}

/*
 *  ShowCursor()
 *
 *  Description:
 *      Function to return ANSI sequence to show the cursor.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to show the cursor.
 *
 *  Comments:
 *      None.
 */
std::string ShowCursor()
{
    return std::string(CSI) + "?25h";
}

/*
 *  ClearLineToEnd()
 *
 *  Description:
 *      Function to clear the contents of the line from the cursor position.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to clear the line to the end.
 *
 *  Comments:
 *      None.
 */
std::string ClearLineToEnd()
{
    return std::string(CSI) + "0K";
}

/*
 *  ClearLineToEnd()
 *
 *  Description:
 *      Function to clear the entire line.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to clear the entire line.
 *
 *  Comments:
 *      None.
 */
std::string ClearLineAll()
{
    return std::string(CSI) + "2K";
}

/*
 *  Reset()
 *
 *  Description:
 *      Produces terminal reset sequence.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Reset(std::ostream &os)
{
    os << Reset();

    return os;
}

/*
 *  Bold()
 *
 *  Description:
 *      Cause text to be bold.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Bold(std::ostream &os)
{
    os << Bold();

    return os;
}

/*
 *  Faint()
 *
 *  Description:
 *      Cause text to be faint.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Faint(std::ostream &os)
{
    os << Faint();

    return os;
}

/*
 *  Italic()
 *
 *  Description:
 *      Cause text to be italic.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Italic(std::ostream &os)
{
    os << Italic();

    return os;
}

/*
 *  Underline()
 *
 *  Description:
 *      Cause text to be underlined.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Underline(std::ostream &os)
{
    os << Underline();

    return os;
}

/*
 *  NormalFont()
 *
 *  Description:
 *      Cause text to be a normal font (not italic).
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &NormalFont(std::ostream &os)
{
    os << NormalFont();

    return os;
}

/*
 *  NoUnderline()
 *
 *  Description:
 *      Cause text to not be underlined.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &NoUnderline(std::ostream &os)
{
    os << NoUnderline();

    return os;
}

/*
 *  NoBlink()
 *
 *  Description:
 *      Cause text to not blink.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &NoBlink(std::ostream &os)
{
    os << NoBlink();

    return os;
}

/*
 *  NoReverseVideo()
 *
 *  Description:
 *      Cause text to not show in reverse video.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &NoReverseVideo(std::ostream &os)
{
    os << NoReverseVideo();

    return os;
}

/*
 *  NoConceal()
 *
 *  Description:
 *      Cause text to not be concealed.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &NoConceal(std::ostream &os)
{
    os << NoConceal();

    return os;
}

/*
 *  NoStrike()
 *
 *  Description:
 *      Cause text to not be strikethrough.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &NoStrike(std::ostream &os)
{
    os << NoStrike();

    return os;
}

/*
 *  NormalIntensity()
 *
 *  Description:
 *      Cause text to be normal intensity.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &NormalIntensity(std::ostream &os)
{
    os << NormalIntensity();

    return os;
}

/*
 *  SlowBlink()
 *
 *  Description:
 *      Cause text to blink slowly.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &SlowBlink(std::ostream &os)
{
    os << SlowBlink();

    return os;
}

/*
 *  RapidBlink()
 *
 *  Description:
 *      Cause text to blink rapidly.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &RapidBlink(std::ostream &os)
{
    os << RapidBlink();

    return os;
}

/*
 *  ReverseVideo()
 *
 *  Description:
 *      Cause text to show in reverse video.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &ReverseVideo(std::ostream &os)
{
    os << ReverseVideo();

    return os;
}

/*
 *  Conceal()
 *
 *  Description:
 *      Cause text to be concealed.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Conceal(std::ostream &os)
{
    os << Conceal();

    return os;
}

/*
 *  Strike()
 *
 *  Description:
 *      Cause text to be strikethrough.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Strike(std::ostream &os)
{
    os << Strike();

    return os;
}

/*
 *  DoubleUnderline()
 *
 *  Description:
 *      Cause text to be double underlined.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &DoubleUnderline(std::ostream &os)
{
    os << DoubleUnderline();

    return os;
}

/*
 *  Black()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce black
 *      text.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Black(std::ostream &os)
{
    os << Black();

    return os;
}

/*
 *  Gray()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce gray
 *      text.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      Gray is actually "black bold", so note that using this turns on bold
 *      intensity.
 */
std::ostream &Gray(std::ostream &os)
{
    os << Gray();

    return os;
}

/*
 *  Red()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce red
 *      text.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Red(std::ostream &os)
{
    os << Red();

    return os;
}

/*
 *  Green()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce green
 *      text.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Green(std::ostream &os)
{
    os << Green();

    return os;
}

/*
 *  Yellow()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce yellow
 *      text.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Yellow(std::ostream &os)
{
    os << Yellow();

    return os;
}

/*
 *  Blue()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce blue
 *      text.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Blue(std::ostream &os)
{
    os << Blue();

    return os;
}

/*
 *  Magenta()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce magenta
 *      text.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Magenta(std::ostream &os)
{
    os << Magenta();

    return os;
}

/*
 *  Cyan()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce cyan
 *      text.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Cyan(std::ostream &os)
{
    os << Cyan();

    return os;
}

/*
 *  White()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce white
 *      text.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &White(std::ostream &os)
{
    os << White();

    return os;
}

/*
 *  Default()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce the
 *      default text color.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &Default(std::ostream &os)
{
    os << Default();

    return os;
}

/*
 *  BlackBackground()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce a black
 *      background.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &BlackBackground(std::ostream &os)
{
    os << BlackBackground();

    return os;
}

/*
 *  RedBackground()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce a red
 *      background.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &RedBackground(std::ostream &os)
{
    os << RedBackground();

    return os;
}

/*
 *  GreenBackground()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce a green
 *      background.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &GreenBackground(std::ostream &os)
{
    os << GreenBackground();

    return os;
}

/*
 *  YellowBackground()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce a
 *      yellow background.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &YellowBackground(std::ostream &os)
{
    os << YellowBackground();

    return os;
}

/*
 *  BlueBackground()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce a blue
 *      background.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &BlueBackground(std::ostream &os)
{
    os << BlueBackground();

    return os;
}

/*
 *  MagentaBackground()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce a
 *      magenta background.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &MagentaBackground(std::ostream &os)
{
    os << MagentaBackground();

    return os;
}

/*
 *  CyanBackground()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce a cyan
 *      background.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &CyanBackground(std::ostream &os)
{
    os << CyanBackground();

    return os;
}

/*
 *  WhiteBackground()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to produce a white
 *      background.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &WhiteBackground(std::ostream &os)
{
    os << WhiteBackground();

    return os;
}

/*
 *  HideCursor()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to hide the
 *      cursor.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &HideCursor(std::ostream &os)
{
    os << HideCursor();

    return os;
}

/*
 *  ShowCursor()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to show the
 *      cursor.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &ShowCursor(std::ostream &os)
{
    os << ShowCursor();

    return os;
}

/*
 *  ClearLineToEnd()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to clear the
 *      contents of the line from the current cursor position.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &ClearLineToEnd(std::ostream &os)
{
    os << ClearLineToEnd();

    return os;
}

/*
 *  ClearLineToEnd()
 *
 *  Description:
 *      Function to insert ANSI sequence in the output stream to clear the
 *      entire line.
 *
 *  Parameters:
 *      os [in/out]
 *          The output stream to which the sequence should be written.
 *
 *  Returns:
 *      Output stream object.
 *
 *  Comments:
 *      None.
 */
std::ostream &ClearLineAll(std::ostream &os)
{
    os << ClearLineAll();

    return os;
}

/*
 *  Foreground()
 *
 *  Description:
 *      Function to return a string to facilitate selection of the specified
 *      foreground color.
 *
 *  Parameters:
 *      color [in]
 *          The selected foreground color.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the desired foreground
 *      color.
 *
 *  Comments:
 *      None.
 */
std::string Foreground(Color color)
{
    std::string result;

    switch (color)
    {
        case Color::Black:
            result = Black();
            break;
        case Color::Red:
            result = Red();
            break;
        case Color::Green:
            result = Green();
            break;
        case Color::Yellow:
            result = Yellow();
            break;
        case Color::Blue:
            result = Blue();
            break;
        case Color::Magenta:
            result = Magenta();
            break;
        case Color::Cyan:
            result = Cyan();
            break;
        case Color::White:
            result = White();
            break;
        default:
            result = Reset();
            break;
    };

    return result;
}

/*
 *  Background()
 *
 *  Description:
 *      Function to return a string to facilitate selection of the specified
 *      background color.
 *
 *  Parameters:
 *      color [in]
 *          The selected foreground color.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the desired foreground
 *      color.
 *
 *  Comments:
 *      None.
 */
std::string Background(Color color)
{
    std::string result;

    switch (color)
    {
        case Color::Black:
            result = BlackBackground();
            break;
        case Color::Red:
            result = RedBackground();
            break;
        case Color::Green:
            result = GreenBackground();
            break;
        case Color::Yellow:
            result = YellowBackground();
            break;
        case Color::Blue:
            result = BlueBackground();
            break;
        case Color::Magenta:
            result = MagentaBackground();
            break;
        case Color::Cyan:
            result = CyanBackground();
            break;
        case Color::White:
            result = WhiteBackground();
            break;
        default:
            result = Reset();
            break;
    };

    return result;
}

/*
 *  SelectGraphicRendition()
 *
 *  Description:
 *      Function to produce the Select Graphic Rendition sequence.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      ANSI sequence for the Select Graphic Rendition (SGR).
 *
 *  Comments:
 *      None.
 */
std::string SelectGraphicRendition(SGR sgr)
{
    return std::string(CSI) + std::to_string(static_cast<std::uint8_t>(sgr)) + SGRT;
}

/*
 *  SGRAndColor()
 *
 *  Description:
 *      Function to produce the character sequence to select the specified
 *      Select Graphic Rendition (SGR), foreground color, and background color.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      String containing the ANSI sequence to produce the foreground color.
 *
 *  Comments:
 *      None.
 */
std::string SGRAndColor(SGR sgr, Color background, Color foreground)
{
    return SelectGraphicRendition(sgr) + Background(background) +
           Foreground(foreground);
}

/*
 *  ColorBG256()
 *
 *  Description:
 *      Select the 256-bit background color using the specified color.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      A color table is viewable here:
 *      https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit
 */
std::string ColorBG256(std::uint8_t color)
{
    return std::string(CSI) + "48;5;" + std::to_string(color) + SGRT;
}

/*
 *  ColorFG256()
 *
 *  Description:
 *      Select the 256-bit foreground color using the specified color.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      A color table is viewable here:
 *      https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit
 */
std::string ColorFG256(std::uint8_t color)
{
    return std::string(CSI) + "38;5;" + std::to_string(color) + SGRT;

}

/*
 *  ColorBG256()
 *
 *  Description:
 *      Select the 256-bit background color using the specified RGB color.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      These are not widely supported by terminals, though it does appear to
 *      work with Windows Terminal.
 */
std::string ColorBG24Bit(std::uint8_t red,
                         std::uint8_t green,
                         std::uint8_t blue)
{
    return std::string(CSI) + "48;2;" + std::to_string(red) + ";" +
           std::to_string(green) + ";" + std::to_string(blue) +
           SGRT;
}

/*
 *  ColorFG256()
 *
 *  Description:
 *      Select the 256-bit foreground color using the specified RGB color.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      These are not widely supported by terminals, though it does appear to
 *      work with Windows Terminal.
 */
std::string ColorFG24Bit(std::uint8_t red,
                         std::uint8_t green,
                         std::uint8_t blue)
{
    return std::string(CSI) + "38;2;" + std::to_string(red) + ";" +
           std::to_string(green) + ";" + std::to_string(blue) +
           SGRT;
}

} // namespace Terra::ConIO::ANSI
