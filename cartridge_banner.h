/* This defines the banner structure.  Most carts have a banner.
 * At first glance it looks useful--multiple language names.
 * But in most cases it's really not.
 */

#ifndef _CARTRIDGE_BANNER_H
#define _CARTRIDGE_BANNER_H

#include <stdint.h>
#include "cartridge.h"


// Our storage records
typedef struct nds_cartridge_banner_s
{
    int NumBannerNames;
    char * BannerNames[7]; // Converted to UTF8 like God intended
    int BannerNameIndexes[7]; // J E F G I S C
    SHA512_HASH * BannerHash;
} nds_cartridge_banner_t;

// Procs
void create_banner(const nds_cartridge_t * cart, nds_cartridge_banner_t * out_banner);
nds_cartridge_banner_t * load_banner(const nds_cartridge_t * cart);
void clear_banner(nds_cartridge_banner_t * banner);
void free_banner(nds_cartridge_banner_t * banner);

int validate_cartridge_banner(const nds_cartridge_t * cart);

#endif
