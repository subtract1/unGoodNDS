#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cartridge_private.h"
#include "cartridge_header.h"
#include "region.h"
#include "libraries/sds.h"


/* The cartridge routines, loaded into memory and ready to report on
 * what it is they find.  The main program doesn't get to poke around
 * the internals any; it just asks the cartridge to describe itself
 * like a good little class.
 */
static const uint8_t homebrew_header[] = { 0x2e, 0x00, 0x00, 0xea };
static const uint8_t homebrew_gamecode[] = { '#', '#', '#', '#' };


// Init / Destroy
nds_cartridge_t * load_nds_cartridge(FILE * fp)
{
    assert(fp != NULL);

    nds_cartridge_t * ret = malloc(sizeof(nds_cartridge_t));

    // Get the size...
    fseek(fp, 0, SEEK_END);
    ret->size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Read the file
    ret->data = (uint8_t *)malloc(ret->size);
    if (ret->size != fread(ret->data, sizeof(uint8_t), ret->size, fp))
    {
        free_nds_cartridge(ret);
        return NULL;
    }

    // Validate file
    if (validate_cartridge(ret) == false)
    {
        free_nds_cartridge(ret);
        return NULL;
    }

    return ret;
}
void free_nds_cartridge(nds_cartridge_t * cart)
{
    if (cart != NULL)
    {
        free(cart->data);
        free(cart);
    }
}

// Attributes
bool is_cartridge_homebrew(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    // In theory a perfectly crafted homebrew cart could pass for a legitimate rom.
    // In practice they tend to nicely identify themselves by name and code.
    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));

    // Most homebrews use the following bytes for their title
    if (memcmp(header->GameTitle, homebrew_header, sizeof(homebrew_header)) == 0)
        return true;

    // Most homebrews also use "####" for their game code.
    if (memcmp(header->GameCode, homebrew_gamecode, sizeof(homebrew_gamecode)) == 0)
        return true;

    // One thing that seems pretty definitive is the Arm9 offset--it's 4000 in EVERY legit rom and practically no homebrews.
    if (header->Arm9RomOffset != 4000)
        return true;

    return false;
}
bool is_cartridge_dsi(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    // I don't know what we're doing here yet.  We could try using the unitCode
    // we could try for offsets or we could try reading the 0x180+ data and see if it looks like a header.
    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));

    return false;
}





// Analyzing
bool validate_cartridge(const nds_cartridge_t * cart)
{
    // This proc needs to go through and ensure the file we loaded could
    // conceivably pass as a cartridge, even a broken one.
    // It mostly sanity checks offsets.

    // At bare minimum the cart needs to be LARGE enough to hold the header.
    if (cart->size < sizeof(ndsHeader_t))
        return false;

    return true;
}



// Hashing




// Reporting

char * cartridge_info(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    // Assemble the base header info first
    bool is_dsi = is_cartridge_dsi(cart);
    bool is_homebrew = is_cartridge_homebrew(cart);
    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));

    sds s = sdsempty();
    s = sdscatprintf(s, " Cartridge Title: %20s ", (is_homebrew && memcmp(header->GameTitle, homebrew_header, sizeof(homebrew_header)) == 0) ? " (homebrew) " : header->GameTitle);
    if (is_homebrew && memcmp(header->GameCode, homebrew_gamecode, sizeof(homebrew_gamecode)) == 0)
        s = sdscatprintf(s, " Expected Serial: %20s \n", " (homebrew) ");
    else
        s = sdscatprintf(s, " Expected Serial:  %20s-%c%c%c%c-%s\n", (is_dsi) ? "TWL" : "NTR", header->GameCode[0], header->GameCode[1], header->GameCode[2], header->GameCode[3], GetRegionFromCode(header->GameCode[3]));
    s = sdscatprintf(s, " MC:  %12d  UC:  %12d  Cap: %12d  Ver: %12d \n", header->MakerCode, header->UnitCode, header->DeviceCapacity, header->RomVersion);
    s = sdscatprintf(s, " Arm9 Off:   %10x Entry: %10x Load:  %10x Size:  %10x \n", header->Arm9RomOffset, header->Arm9EntryAddress, header->Arm9LoadAddress, header->Arm9Size);
    s = sdscatprintf(s, " Arm7 Off:   %10x Entry: %10x Load:  %10x Size:  %10x \n", header->Arm7RomOffset, header->Arm7EntryAddress, header->Arm7LoadAddress, header->Arm7Size);
    s = sdscat(s, "\n");

    // We'll want to calc and put hashes in here at some point both file and arm




    // Now report on the DSi header (if it exists)


    s = sdscat(s, "\n");
    // Return a regular, boring, no special library needed string to the end user.
    char * ret = malloc(sdslen(s) + 1);
    memcpy(ret, s, sdslen(s) + 1);
    sdsfree(s);

    return ret;
}
