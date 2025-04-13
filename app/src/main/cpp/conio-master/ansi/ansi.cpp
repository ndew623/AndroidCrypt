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
 *      This program will produce colorful output to demonstrate the color
 *      functionality facilitated by the console I/O library.
 *
 *  Portability Issues:
 *      None.
 */

#include <iostream>
#include <iomanip>
#include <terra/conio/ansi.h>
#include <terra/conio/ansi_capable.h>

using namespace Terra::ConIO;

// Produce a human-readable text string for the SGR values
std::string SGRLookup(std::uint8_t sgr)
{
    std::string result;

    switch (sgr)
    {
        case 0:
            result = "Reset";
            break;
        case 1:
            result = "Bold";
            break;
        case 2:
            result = "Faint";
            break;
        case 3:
            result = "Italic";
            break;
        case 4:
            result = "Underline";
            break;
        case 5:
            result = "SlowBlink";
            break;
        case 6:
            result = "RapidBlink";
            break;
        case 7:
            result = "ReverseVideo";
            break;
        case 8:
            result = "Conceal";
            break;
        case 9:
            result = "Strike";
            break;
        default:
            result = "Unknown";
            break;
    }

    return result;
}

int main()
{
    if (!EnableStdOutANSIOutput())
    {
        std::cout << "The output destination is not a terminal, thus no color "
                     "is rendered"
                  << std::endl;
        return 0;
    }

    std::cout << ANSI::Bold << ANSI::Cyan << "Basic ANSI Support" << std::endl;
    std::cout << ANSI::Cyan << "==================" << ANSI::Reset;
    std::cout << std::endl << std::endl;

    std::cout << "If any modes do " << ANSI::Italic << "not" << ANSI::NormalFont
              << " work, it is possibly a terminal limitation." << std::endl
              << std::endl;
    std::cout << "In the following table, the numbers in brackets represent "
                 "background and" << std::endl;
    std::cout << "foreground colors, in that order." << std::endl;

    std::cout << std::endl;

    // Loop over Select Graphics Rendition (SGR)
    for (std::uint8_t sgr = 0;
         sgr <= static_cast<std::uint8_t>(ANSI::SGR::Strike);
         sgr++)
    {
        std::cout << "SGR Value: " << SGRLookup(sgr) << " ("
                  << static_cast<unsigned>(sgr) << ")" << std::endl;

        std::cout << "BG / FG";
        for (int i = 0; i < 8; i++) std::cout << "    " << i << "    ";
        std::cout << std::endl;

        // Loop over background colors
        for (std::uint8_t bg = 0;
             bg <= static_cast<std::uint8_t>(ANSI::Color::White);
             bg++)
        {
            std::cout << " " << unsigned(bg) << "      ";
            // Loop over foreground colors
            for (std::uint8_t fg = 0;
                fg <= static_cast<std::uint8_t>(ANSI::Color::White);
                fg++)
            {
                if (fg > 0) std::cout << " ";
                std::cout << ANSI::SGRAndColor(static_cast<ANSI::SGR>(sgr),
                                               static_cast<ANSI::Color>(bg),
                                               static_cast<ANSI::Color>(fg));
                std::cout << " Sample " << ANSI::Reset;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;


    std::cout << ANSI::Bold << ANSI::Cyan << "256-Bit Color Table" << std::endl;
    std::cout << ANSI::Cyan << "===================" << ANSI::Reset;
    std::cout << std::endl;

    // Get the current stream flags
    std::ios_base::fmtflags flags(std::cout.flags());

    for (unsigned i = 0; i < 256; i++)
    {
        std::string text = "Color " + std::to_string(i);
        std::cout << ANSI::ColorFG256(i) << std::setw(9) << std::left << text;
        if (((i % 8) == 0) || (i == 255))
        {
            std::cout << std::endl;
        }
        else
        {
            std::cout << " ";
        }
    }
    std::cout << ANSI::Reset << std::endl;

    std::cout << "Note: It is also possible " << ANSI::ColorBG256(33)
              << "change background" << ANSI::Reset << " colors." << std::endl;

    // Restore the ostream flags
    std::cout.flags(flags);

    std::cout << std::endl;
    std::cout << ANSI::Bold << ANSI::Cyan << "24-Bit Color Test" << std::endl;
    std::cout << ANSI::Cyan << "==================" << ANSI::Reset;
    std::cout << std::endl;

    std::cout << ANSI::ColorFG24Bit(0x00, 0x66, 0xcc);
    std::cout << "Should render in a shade of blue" << ANSI::Reset;
    std::cout << std::endl;

    std::cout << ANSI::ColorFG24Bit(0x70, 0x08, 0x08);
    std::cout << "Should render in a shade of red" << ANSI::Reset;
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "Note: 24-bit color is not widely supported, though it does "
                 "appear to work with" << std::endl;
    std::cout << "      Windows terminal.  Incorrect colors suggests lack of "
                 " proper support."
              << std::endl;
}
