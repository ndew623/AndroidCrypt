/*
 *  ansi.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines functions, types, etc. to facilitate ANSI control
 *      code output, including color.  The specification for these control
 *      codes includes ECMA-48, ISO/IEC 6429, FIPS 86, ANSI X3.64, and
 *      JIS X 0211.
 *
 *      One can use the library to construct strings or directly with the
 *      iostream library like this:
 *          std::cout << Terra::ConIO::ANSI::Red << "Red Text";
 *          std::cout << Terra::ConIO::ANSI::Italics << "Italics";
 *      See the comment "Named Select Graphic Rendition functions" below.
 *
 *      See also: https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <ostream>
#include <cstdint>
#include <string>

namespace Terra::ConIO::ANSI
{

// Select Graphic Rendition (not all terminals support all modes)
enum class SGR : std::uint8_t
{
    Reset = 0,
    Bold = 1,           // Increases brightness; bold black = gray
    Faint = 2,          // Decreases brightness
    Italic = 3,
    Underline = 4,
    SlowBlink = 5,
    RapidBlink = 6,
    ReverseVideo = 7,
    Conceal = 8,
    Strike = 9,
    PrimaryFont = 10,
    AltFont1 = 11,
    AltFont2 = 12,
    AltFont3 = 13,
    AltFont4 = 14,
    AltFont5 = 15,
    AltFont6 = 16,
    AltFont7 = 17,
    AltFont8 = 18,
    AltFont9 = 19,
    Gothic = 20,
    DoubleUnderline = 21,
    NormalIntensity = 22,
    NormalFont = 23,
    NoUnderline = 24,
    NoBlink = 25,
    Proportional = 26,
    NoReverseVideo = 27,
    NoConceal = 28,
    NoStrike = 29,
    BlackFG = 30,
    RedFG = 31,
    GreenFG = 32,
    YellowFG = 33,
    BlueFG = 34,
    MagentaFG = 35,
    CyanFG = 36,
    WhiteFG = 37,
    DefaultFG = 39,
    BlackBG = 40,
    RedBG = 41,
    GreenBG = 42,
    YellowBG = 43,
    BlueBG = 44,
    MagentaBG = 45,
    CyanBG = 46,
    WhiteBG = 47,
    DefaultBG = 49,

    // The following are non-standard, but widely supported.  However, they
    // can be produced by sending bold + color, as well, and are thus redundant.
    BrightBlackFG = 90,
    BrightRedFG = 91,
    BrightGreenFG = 92,
    BrightYellowFG = 93,
    BrightBlueFG = 94,
    BrightMagentaFG = 95,
    BrightCyanFG = 96,
    BrightWhiteFG = 97,
    BrightBlackBG = 100,
    BrightRedBG = 101,
    BrightGreenBG = 102,
    BrightYellowBG = 103,
    BrightBlueBG = 104,
    BrightMagentaBG = 105,
    BrightCyanBG = 106,
    BrightWhiteBG = 107
};

// Basic color choices; while color is a subset of SGR sequences, this enum
// exists for convenience since it is used more frequently
enum class Color : std::uint8_t
{
    Black = 0,
    Red = 1,
    Green = 2,
    Yellow = 3,
    Blue = 4,
    Magenta = 5,
    Cyan = 6,
    White = 7,
    Default = 9
};

// Functions to return strings holding ANSI sequences
std::string Reset();
std::string Bold();
std::string Faint();
std::string Italic();
std::string Underline();
std::string SlowBlink();
std::string RapidBlink();
std::string ReverseVideo();
std::string Conceal();
std::string Strike();
std::string DoubleUnderline();
std::string NormalIntensity();
std::string NormalFont();
std::string NoUnderline();
std::string NoBlink();
std::string NoReverseVideo();
std::string NoConceal();
std::string NoStrike();
std::string Black();
std::string Gray();
std::string Red();
std::string Green();
std::string Yellow();
std::string Blue();
std::string Magenta();
std::string Cyan();
std::string White();
std::string Default();
std::string BlackBackground();
std::string RedBackground();
std::string GreenBackground();
std::string YellowBackground();
std::string BlueBackground();
std::string MagentaBackground();
std::string CyanBackground();
std::string WhiteBackground();
std::string HideCursor();
std::string ShowCursor();
std::string ClearLineToEnd();
std::string ClearLineAll();

// Functions to ease use of ANSI sequences with streams; Example:
//      os << ANSI::Red << "Red Text" << ANSI::Reset;
std::ostream &Reset(std::ostream &os);
std::ostream &Bold(std::ostream &os);
std::ostream &Faint(std::ostream &os);
std::ostream &Italic(std::ostream &os);
std::ostream &Underline(std::ostream &os);
std::ostream &SlowBlink(std::ostream &os);
std::ostream &RapidBlink(std::ostream &os);
std::ostream &ReverseVideo(std::ostream &os);
std::ostream &Conceal(std::ostream &os);
std::ostream &Strike(std::ostream &os);
std::ostream &DoubleUnderline(std::ostream &os);
std::ostream &NormalIntensity(std::ostream &os);
std::ostream &NormalFont(std::ostream &os);
std::ostream &NoUnderline(std::ostream &os);
std::ostream &NoBlink(std::ostream &os);
std::ostream &NoReverseVideo(std::ostream &os);
std::ostream &NoConceal(std::ostream &os);
std::ostream &NoStrike(std::ostream &os);
std::ostream &Black(std::ostream &os);
std::ostream &Gray(std::ostream &os);
std::ostream &Red(std::ostream &os);
std::ostream &Green(std::ostream &os);
std::ostream &Yellow(std::ostream &os);
std::ostream &Blue(std::ostream &os);
std::ostream &Magenta(std::ostream &os);
std::ostream &Cyan(std::ostream &os);
std::ostream &White(std::ostream &os);
std::ostream &Default(std::ostream &os);
std::ostream &BlackBackground(std::ostream &os);
std::ostream &RedBackground(std::ostream &os);
std::ostream &GreenBackground(std::ostream &os);
std::ostream &YellowBackground(std::ostream &os);
std::ostream &BlueBackground(std::ostream &os);
std::ostream &MagentaBackground(std::ostream &os);
std::ostream &CyanBackground(std::ostream &os);
std::ostream &WhiteBackground(std::ostream &os);
std::ostream &HideCursor(std::ostream &os);
std::ostream &ShowCursor(std::ostream &os);
std::ostream &ClearLineToEnd(std::ostream &os);
std::ostream &ClearLineAll(std::ostream &os);

// Produce the given background color
std::string Background(Color color);

// Produce the given foreground color
std::string Foreground(Color color);

// Produce the desired Select Graphic Rendition (SGR) sequence
std::string SelectGraphicRendition(SGR sgr);

// Specify SGR, foreground color, and background color
std::string SGRAndColor(SGR sgr, Color background, Color Foreground);

// Specify 256-bit background color
std::string ColorBG256(std::uint8_t color);

// Specify 256-bit foreground color
std::string ColorFG256(std::uint8_t color);

// Specify background color using 24-bit RGB values
std::string ColorBG24Bit(std::uint8_t red,
                         std::uint8_t green,
                         std::uint8_t blue);

// Specify foreground color using 24-bit RGB values
std::string ColorFG24Bit(std::uint8_t red,
                         std::uint8_t green,
                         std::uint8_t blue);

} // namespace Terra::ConIO::ANSI
