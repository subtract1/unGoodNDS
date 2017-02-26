#include "region.h"

/* Every Nintendo DS cart has a serial in the form of XXX-YYYY-ZZZ.
 * That number is in the format of TYPE-SERIAL-REGION.
 * -TYPE = "NTR" (for regular carts) or "TWL" (for DSi enhanced carts)
 * -SERIAL = a four byte code that exists in the ROM itself at bytes[12-15].
 * -REGION = the region.  The region should correspond to the last byte
 *  of the serial but I don't think that's 100% guaranteed.
 *
 * Generating the serial based on a ROM should be straightforward.
 * Homebrew titles won't have a serial obviously.
 */

const char * GetRegionFromCode(const char regionCode)
{
    switch (regionCode)
    {
        case 'C': return "CHN";
        case 'D': return "NOE";
        case 'E': return "USA";
        case 'F': return "FRA";
        case 'H': return "HOL";
        case 'I': return "ITA";
        case 'J': return "JPN";
        case 'K': return "KOR";
        case 'O': return "INT";
        case 'P': return "EUR";
        case 'R': return "RUS";
        case 'S': return "ESP";
        case 'U': return "AUS";
        case 'V': return "EUU";
        case 'W': return "EUU";
        case 'X': return "EUU";
        default:
            return "???";
    }
}
const char * GetRegionTextFromCode(const char regionCode)
{
    switch (regionCode)
    {
        case 'C': return "China";
        case 'D': return "Germany";
        case 'E': return "USA";
        case 'F': return "France";
        case 'H': return "Netherlands";
        case 'I': return "Italy";
        case 'J': return "Japan";
        case 'K': return "Korea";
        case 'O': return "International";
        case 'P': return "Europe";
        case 'R': return "Russia";
        case 'S': return "Spain";
        case 'U': return "Australia";
        case 'V': return "Europe";
        case 'W': return "Europe";
        case 'X': return "Europe";
        default:
            return "Unknown";
    }
}
