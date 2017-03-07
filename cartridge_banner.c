#include <assert.h>
#include <iconv.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cartridge.h"
#include "cartridge_banner.h"
#include "cartridge_header.h"
#include "hash_helper.h"


// The banner structure as it exists on disk
typedef struct __attribute__((__packed__)) ndsBanner_s
{
    uint16_t BannerVersion;
    uint16_t BannerCrc;
    uint16_t CheckCRC;
    uint8_t _reserved[26];
    uint8_t Banner[512];
    uint8_t Palette[32];
    uint8_t BannerNameJ[256]; // These all look to be utf-16
    uint8_t BannerNameE[256];
    uint8_t BannerNameF[256];
    uint8_t BannerNameG[256];
    uint8_t BannerNameI[256];
    uint8_t BannerNameS[256];
    uint8_t BannerNameC[256]; // Exists only in v2
} ndsBanner_t;


// Constructor / Destructor
void create_banner(const nds_cartridge_t * cart, nds_cartridge_banner_t * out_banner)
{
    assert(cart != NULL);
    assert(out_banner != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));

    // If there IS no banner we return a zeroed out structure.
    memset(out_banner, 0, sizeof(*out_banner));
    if (header->IconBannerOffset == 0)
        return;

    // The structure is very flat.  Extract, hash, and re-encode
    // those pesky UTF16 strings into UTF8.
    ndsBanner_t * banner = ((ndsBanner_t *)(cart->Data + header->IconBannerOffset));
    out_banner->BannerHash = get_sha512(banner->Banner, sizeof(banner->Banner));

    uint8_t * name_table[] = { banner->BannerNameJ, banner->BannerNameE, banner->BannerNameF, banner->BannerNameG, banner->BannerNameI, banner->BannerNameS, banner->BannerNameC };
    int name_cnt = (banner->BannerVersion >= 2) ? 7 : 6;
    for (int i = 0; i < name_cnt; i++)
    {
        char temp_utf8[sizeof(banner->BannerNameJ) * 2]; // The max size possible and a significant overestimation
        size_t in_size = sizeof(banner->BannerNameJ);
        size_t out_size = sizeof(banner->BannerNameJ) * 2;
        char * in_ptr = (char *)(name_table[i]);
        char * out_ptr = temp_utf8;

        // Convert UTF16 to UTF8.
        iconv_t icv_ret = iconv_open("UTF-8", "UTF-16LE");
        iconv(icv_ret, &in_ptr, &in_size, &out_ptr, &out_size);
        iconv_close(icv_ret);

        // We have a string, see if it already exists.
        bool found = false;
        for (int j = 0; j < out_banner->NumBannerNames; j++)
        {
            if (strcmp(out_banner->BannerNames[j], temp_utf8) == 0)
            {
                // Match
                out_banner->BannerNameIndexes[i] = j;
                found = true;
                break;
            }
        }

        if (found == false)
        {
            // Didn't find one.  Allocate and finish up.
            out_banner->BannerNames[out_banner->NumBannerNames] = malloc(strlen(temp_utf8) + 1);
            strcpy(out_banner->BannerNames[out_banner->NumBannerNames], temp_utf8);
            out_banner->BannerNameIndexes[i] = out_banner->NumBannerNames;
            out_banner->NumBannerNames++;
        }
    }
}
nds_cartridge_banner_t * load_banner(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    nds_cartridge_banner_t * ret = malloc(sizeof(nds_cartridge_banner_t));
    create_banner(cart, ret);

    // If we didn't get a valid banner (ie we got our structure zeroed out) then just return null.
    if (ret->NumBannerNames == 0)
        return NULL;
    return ret;
}
void clear_banner(nds_cartridge_banner_t * banner)
{
    if (banner == NULL)
        return;

    for (int i = 0; i < banner->NumBannerNames; i++)
    {
        free(banner->BannerNames[i]);
    }

    free(banner->BannerHash);
}
void free_banner(nds_cartridge_banner_t * banner)
{
    if (banner == NULL)
        return;

    clear_banner(banner);
    free(banner);
}


// Validation
int validate_cartridge_banner(const nds_cartridge_t * cart)
{
    // This proc is called as part of the cartridge validation routines.
    // The basic header info should already be validated.

    // The banner doesn't actually HAVE any offsets so validation is easy.
    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->IconBannerOffset > 0)
    {
        if (header->IconBannerOffset + sizeof(ndsBanner_t) >= cart->Size)
            return -40;
    }

    return 0;
}
