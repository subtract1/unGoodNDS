
/* This defines the banner structure.  Most carts have a banner.
 * At first glance it looks useful--multiple language names.
 * But in most cases it's really not.
 */

#ifndef _CARTRIDGE_BANNER_H
#define _CARTRIDGE_BANNER_H

#include <stdint.h>

struct __attribute__((__packed__)) ndsBanner_s
{
    uint16_t BannerVersion;
    uint16_t BannerCrc;
    uint8_t CheckCRC;
    uint8_t _reserved[28];
    uint8_t Banner[512];
    uint8_t Palette[32];
    uint8_t BannerNameJ[256]; // These all look to be utf-16
    uint8_t BannerNameE[256];
    uint8_t BannerNameF[256];
    uint8_t BannerNameG[256];
    uint8_t BannerNameI[256];
    uint8_t BannerNameS[256];
};
typedef struct ndsBanner_s ndsBanner_t;

#endif
