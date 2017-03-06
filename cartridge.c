#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cartridge.h"
#include "cartridge_filetable.h"
#include "cartridge_header.h"
#include "hash_helper.h"
#include "region.h"
#include "libraries/crc.h"
#include "libraries/CryptLib/LibMd5.h"
#include "libraries/CryptLib/LibSha1.h"
#include "libraries/CryptLib/LibSha256.h"
#include "libraries/CryptLib/LibSha512.h"
#include "libraries/sds/sds.h"


/* The cartridge routines, loaded into memory and ready to report on
 * what it is they find.  The main program doesn't get to poke around
 * the internals any; it just asks the cartridge to describe itself
 * like a good little class.
 *
 * A Nintendo DS rom is (usually) laid out as follows:
 *  -Header: Could be a normal header or a DSI header, both are fairly small.
 *   The header describes where other parts of the ROM are and in official
 *   titles is almost always padded out with zeros to make it an even 0x4000
 *   bytes.  Homebrew headers tend to be more bare bones.
 *  -Arm9 program: A binary chunk of (possibly encrypted) data.  In every
 *   commercial ROM I've seen so far this starts at 0x4000.  In homebrews it's
 *   usually something else.  The location and size are stored in the header.
 *   If the next entry is byte aligned it's not unusual to see end padding.
 *  -Something *can* but doesn't always go here.  Is it an extra resource file?  DSi stuff? Rearranged items?  It's not always padding
 *   The Arm9 blob probably stores non-code resources but I haven't gotten that far yet.
 *  -Arm7 program: Same as the Arm9 except since it's not the first blob in the
 *   cart the offset isn't going to be a fixed 4000.
 *  -Data tree: (haven't dug into yet)
 */
static const uint8_t homebrew_header[] = { 0x2e, 0x00, 0x00, 0xea };
static const uint8_t homebrew_gamecode[] = { '#', '#', '#', '#' };


// Init / Destroy
nds_cartridge_t * create_nds_cartridge(FILE * fp)
{
    assert(fp != NULL);

    nds_cartridge_t * ret = malloc(sizeof(nds_cartridge_t));
    memset(ret, 0, sizeof(*ret));

    // Get the size...
    fseek(fp, 0, SEEK_END);
    ret->Size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Read the file
    ret->Data = (uint8_t *)malloc(ret->Size);
    if (ret->Size != fread(ret->Data, sizeof(uint8_t), ret->Size, fp))
    {
        free_nds_cartridge(ret);
        return NULL;
    }

    // Analyze the file (cutting out early if it's borked)
    ret->CartCrc = get_cart_crc32(ret);
    ret->CartHash = get_cart_sha512(ret);

    ret->Status = validate_cartridge(ret);
    if (ret->Status == 0)
    {
        create_cart_hashes(ret);
        ret->FileTable = load_filetable(ret);
    }

    return ret;
}
void free_nds_cartridge(nds_cartridge_t * cart)
{
    if (cart == NULL)
        return;

    free(cart->Data);
    free(cart->CartHash);
    free(cart->TrimHash);
    free(cart->Arm9Hash);
    free(cart->Arm7Hash);
    free(cart->Arm9OverlayHash);
    free(cart->Arm7OverlayHash);
    free_filetable(cart->FileTable);
    free(cart);
}

// Attributes
bool is_cartridge_homebrew(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    // In theory a perfectly crafted homebrew cart could pass for a legitimate rom.
    // In practice they tend to nicely identify themselves by name and code.
    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));

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
    //ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));

    return false;
}


// Analysis
void create_cart_hashes(nds_cartridge_t * cart)
{
    if (cart->TrimSize != 0 && cart->TrimSize != cart->Size)
        cart->TrimHash = get_trimcart_sha512(cart);
    cart->HeaderCrc16 = get_cart_header_crc16(cart);
    cart->Arm9Hash = get_cart_arm9_sha512(cart);
    cart->Arm7Hash = get_cart_arm7_sha512(cart);
    cart->Arm9OverlayHash = get_cart_arm9ovr_sha512(cart);
    cart->Arm7OverlayHash = get_cart_arm7ovr_sha512(cart);
}
int validate_cartridge(const nds_cartridge_t * cart)
{
    // This proc needs to go through and ensure the file we loaded could
    // conceivably pass as a cartridge, even a broken one.
    // It mostly sanity checks offsets.

    // At bare minimum the cart needs to be LARGE enough to hold the header.
    // The smallest cart known to exist is an experiment (for doing just that)
    // which stores EVERYTHING in the header itself in the reserved spots.
    // It is 352 bytes--just large enough to store the CRC.
    if (cart->Size < offsetof(ndsHeader_t, _reserved3))
    //if (cart->Size < sizeof(ndsHeader_t))
        return -1;

    // It should also be large enough to hold everything the header points to.
    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (cart->Size < header->Arm9RomOffset + header->Arm9Size)
        return -2;
    if (cart->Size < header->Arm7RomOffset + header->Arm7Size)
        return -3;

    int ret = validate_cartridge_filetable(cart);
    if (ret < 0)
        return ret;

    return 0;
}
const char * validate_cartridge_errmsg(int errno)
{
    switch (errno)
    {
        case 0:
            return "No error";
        case -1:
            return "File is not large enough to hold the minimum ROM header";
        case -2:
            return "Arm9 assembly is located outside of ROM boundaries";
        case -3:
            return "Arm7 assembly is located outside of ROM boundaries";
        case -20:
            return "FAT is located outside of ROM boundaries";
        case -21:
            return "FAT describes file with negative size";
        case -22:
            return "FAT describes file with location outside of ROM boundaries";
        case -30:
            return "FNT is located outside of ROM boundaries";
        case -31:
            return "FNT directory location outside of FNT boundaries";
        case -32:
            return "FNT name outside of FNT boundaries";
        case -33:
            return "FNT parent has no corresponding FNT index";
        case -34:
            return "FNT name has no corresponding FAT index";
        case -35:
            return "FNT has no corresponding FAT index";

        default:
            return "Unknown error";
    }
}


// Hashing (cart)
uint16_t get_cart_crc16(const nds_cartridge_t * cart) { return get_crc16(cart->Data, cart->Size); }
uint32_t get_cart_crc32(const nds_cartridge_t * cart) { return get_crc32(cart->Data, cart->Size); }
MD5_HASH * get_cart_md5(const nds_cartridge_t * cart) { return get_md5(cart->Data, cart->Size); }
SHA1_HASH * get_cart_sha1(const nds_cartridge_t * cart) { return get_sha1(cart->Data, cart->Size); }
SHA256_HASH * get_cart_sha256(const nds_cartridge_t * cart) { return get_sha256(cart->Data, cart->Size); }
SHA512_HASH * get_cart_sha512(const nds_cartridge_t * cart) { return get_sha512(cart->Data, cart->Size); }

// Hashing (trimmed cart)
uint16_t get_trimcart_crc16(const nds_cartridge_t * cart) { return get_crc16(cart->Data, cart->TrimSize); }
uint32_t get_trimcart_crc32(const nds_cartridge_t * cart) { return get_crc32(cart->Data, cart->TrimSize); }
MD5_HASH * get_trimcart_md5(const nds_cartridge_t * cart) { return get_md5(cart->Data, cart->TrimSize); }
SHA1_HASH * get_trimcart_sha1(const nds_cartridge_t * cart) { return get_sha1(cart->Data, cart->TrimSize); }
SHA256_HASH * get_trimcart_sha256(const nds_cartridge_t * cart) { return get_sha256(cart->Data, cart->TrimSize); }
SHA512_HASH * get_trimcart_sha512(const nds_cartridge_t * cart) { return get_sha512(cart->Data, cart->TrimSize); }

// Hashing (cart header aka the first 350 bytes)
uint16_t get_cart_header_crc16(const nds_cartridge_t * cart) { return get_crc16(cart->Data, offsetof(ndsHeader_t, HeaderCrc)); }
uint32_t get_cart_header_crc32(const nds_cartridge_t * cart) { return get_crc32(cart->Data, offsetof(ndsHeader_t, HeaderCrc)); }
MD5_HASH * get_cart_header_md5(const nds_cartridge_t * cart) { return get_md5(cart->Data, offsetof(ndsHeader_t, HeaderCrc)); }
SHA1_HASH * get_cart_header_sha1(const nds_cartridge_t * cart) { return get_sha1(cart->Data, offsetof(ndsHeader_t, HeaderCrc)); }
SHA256_HASH * get_cart_header_sha256(const nds_cartridge_t * cart) { return get_sha256(cart->Data, offsetof(ndsHeader_t, HeaderCrc)); }
SHA512_HASH * get_cart_header_sha512(const nds_cartridge_t * cart) { return get_sha512(cart->Data, offsetof(ndsHeader_t, HeaderCrc)); }

// Hashing (cart arm9 blob size and position depends on what the header says)
/* These blobs can be encrypted.  Do I care?  Probably not since the only
 * time that would matter to me is if the same blob is used in multiple
 * versions of a cart yet encrypted differently each time.  To research.
  **/
uint16_t get_cart_arm9_crc16(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    return get_crc16(cart->Data + header->Arm9RomOffset, header->Arm9Size);
}
uint32_t get_cart_arm9_crc32(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    return get_crc32(cart->Data + header->Arm9RomOffset, header->Arm9Size);
}
MD5_HASH * get_cart_arm9_md5(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    return get_md5(cart->Data + header->Arm9RomOffset, header->Arm9Size);
}
SHA1_HASH * get_cart_arm9_sha1(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    return get_sha1(cart->Data + header->Arm9RomOffset, header->Arm9Size);
}
SHA256_HASH * get_cart_arm9_sha256(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    return get_sha256(cart->Data + header->Arm9RomOffset, header->Arm9Size);
}
SHA512_HASH * get_cart_arm9_sha512(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    return get_sha512(cart->Data + header->Arm9RomOffset, header->Arm9Size);
}

// Hashing (cart arm7 blob size and position depends on what the header says)
uint16_t get_cart_arm7_crc16(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    return get_crc16(cart->Data + header->Arm7RomOffset, header->Arm7Size);
}
uint32_t get_cart_arm7_crc32(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    return get_crc32(cart->Data + header->Arm7RomOffset, header->Arm7Size);
}
MD5_HASH * get_cart_arm7_md5(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    return get_md5(cart->Data + header->Arm7RomOffset, header->Arm7Size);
}
SHA1_HASH * get_cart_arm7_sha1(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    return get_sha1(cart->Data + header->Arm7RomOffset, header->Arm7Size);
}
SHA256_HASH * get_cart_arm7_sha256(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    return get_sha256(cart->Data + header->Arm7RomOffset, header->Arm7Size);
}
SHA512_HASH * get_cart_arm7_sha512(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    return get_sha512(cart->Data + header->Arm7RomOffset, header->Arm7Size);
}

// Hashing (cart arm9 overlay, may not exist)
uint16_t get_cart_arm9ovr_crc16(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->Arm9OverlayOffset == 0 || header->Arm9OverlayLength == 0)
        return 0;

    return get_crc16(cart->Data + header->Arm9OverlayOffset, header->Arm9OverlayLength);
}
uint32_t get_cart_arm9ovr_crc32(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->Arm9OverlayOffset == 0 || header->Arm9OverlayLength == 0)
        return 0;

    return get_crc32(cart->Data + header->Arm9OverlayOffset, header->Arm9OverlayLength);
}
MD5_HASH * get_cart_arm9ovr_md5(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->Arm9OverlayOffset == 0 || header->Arm9OverlayLength == 0)
        return NULL;

    return get_md5(cart->Data + header->Arm9OverlayOffset, header->Arm9OverlayLength);
}
SHA1_HASH * get_cart_arm9ovr_sha1(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->Arm9OverlayOffset == 0 || header->Arm9OverlayLength == 0)
        return NULL;

    return get_sha1(cart->Data + header->Arm9OverlayOffset, header->Arm9OverlayLength);
}
SHA256_HASH * get_cart_arm9ovr_sha256(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->Arm9OverlayOffset == 0 || header->Arm9OverlayLength == 0)
        return NULL;

    return get_sha256(cart->Data + header->Arm9OverlayOffset, header->Arm9OverlayLength);
}
SHA512_HASH * get_cart_arm9ovr_sha512(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->Arm9OverlayOffset == 0 || header->Arm9OverlayLength == 0)
        return NULL;

    return get_sha512(cart->Data + header->Arm9OverlayOffset, header->Arm9OverlayLength);
}

// Hashing (cart arm7 overlay, may not exist)
uint16_t get_cart_arm7ovr_crc16(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->Arm7OverlayOffset == 0 || header->Arm7OverlayLength == 0)
        return 0;

    return get_crc16(cart->Data + header->Arm7OverlayOffset, header->Arm7OverlayLength);
}
uint32_t get_cart_arm7ovr_crc32(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->Arm7OverlayOffset == 0 || header->Arm7OverlayLength == 0)
        return 0;

    return get_crc32(cart->Data + header->Arm7OverlayOffset, header->Arm7OverlayLength);
}
MD5_HASH * get_cart_arm7ovr_md5(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->Arm7OverlayOffset == 0 || header->Arm7OverlayLength == 0)
        return NULL;

    return get_md5(cart->Data + header->Arm7OverlayOffset, header->Arm7OverlayLength);
}
SHA1_HASH * get_cart_arm7ovr_sha1(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->Arm7OverlayOffset == 0 || header->Arm7OverlayLength == 0)
        return NULL;

    return get_sha1(cart->Data + header->Arm7OverlayOffset, header->Arm7OverlayLength);
}
SHA256_HASH * get_cart_arm7ovr_sha256(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->Arm7OverlayOffset == 0 || header->Arm7OverlayLength == 0)
        return NULL;

    return get_sha256(cart->Data + header->Arm7OverlayOffset, header->Arm7OverlayLength);
}
SHA512_HASH * get_cart_arm7ovr_sha512(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->Arm7OverlayOffset == 0 || header->Arm7OverlayLength == 0)
        return NULL;

    return get_sha512(cart->Data + header->Arm7OverlayOffset, header->Arm7OverlayLength);
}

// Reporting
char * cartridge_info(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    // If this is a broken cartridge report only basic data.
    // I am not interested in trying to rip files out of a broken cart.
    char hash_buffer[sizeof(SHA512_HASH) * 2 + 1];
    sds s = sdsempty();
    if (cart->Status != 0)
    {
        s = sdscatprintf(s, " ROM is broken.  %s\n", validate_cartridge_errmsg(cart->Status));
    }

    if (cart->Status != -1)
    {
        // Assemble the base header info
        bool is_dsi = is_cartridge_dsi(cart);
        bool is_homebrew = is_cartridge_homebrew(cart);
        ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));

        s = sdscatprintf(s, " Cartridge Title: %20.12s ", (is_homebrew && memcmp(header->GameTitle, homebrew_header, sizeof(homebrew_header)) == 0) ? " (homebrew) " : header->GameTitle);
        if (is_homebrew && memcmp(header->GameCode, homebrew_gamecode, sizeof(homebrew_gamecode)) == 0)
            s = sdscatprintf(s, " Expected Serial: %20s \n", " (homebrew) ");
        else
            s = sdscatprintf(s, " Expected Serial: %12s-%.3s-%s\n", (is_dsi) ? "TWL" : "NTR", header->GameCode, GetRegionFromCode(header->GameCode[3]));
        s = sdscatprintf(s, " MC:  %12.2s  UC:   %12d  Cap:  %12d Ver:  %12d \n", header->MakerCode, header->UnitCode, 0x20000 << header->DeviceCapacity, header->RomVersion);
        s = sdscatprintf(s, " Arm9 Off: %8x Entry:    %8x  Load:     %8x Size:     %8x \n", header->Arm9RomOffset, header->Arm9EntryAddress, header->Arm9LoadAddress, header->Arm9Size);
        s = sdscatprintf(s, " Arm7 Off: %8x Entry:    %8x  Load:     %8x Size:     %8x \n", header->Arm7RomOffset, header->Arm7EntryAddress, header->Arm7LoadAddress, header->Arm7Size);
        s = sdscatprintf(s, " FNT:      %8x Size:     %8x  FAT:      %8x Size:     %8x \n", header->FileNameTableOffset, header->FileNameTableLength, header->FileAllocationTableOffset, header->FileAllocationTableLength);
        s = sdscatprintf(s, " A9ovr:    %8x Size:     %8x  A7ovr:    %8x Size:     %8x \n", header->Arm9OverlayOffset, header->Arm9OverlayLength, header->Arm9OverlayOffset, header->Arm9OverlayLength);
        s = sdscat(s, "\n");
    }

    // Even bad files get the overall hash
    sha512_to_hex(cart->CartHash, hash_buffer);
    s = sdscatprintf(s, " Cart CRC:       %08x\n", cart->CartCrc);
    s = sdscatprintf(s, " Cart SHA512:    %.32s ... %.8s\n", hash_buffer, hash_buffer+120);

    if (cart->Status == 0)
    {
        ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));

        s = sdscatprintf(s, " Header CRC16 (reported):  %04x\n", header->HeaderCrc);
        s = sdscatprintf(s, " Header CRC16 (calc):      %04x\n", cart->HeaderCrc16);

        if (cart->Arm9Hash != NULL)
        {
            sha512_to_hex(cart->Arm9Hash, hash_buffer);
            s = sdscatprintf(s, " Arm9 SHA512:       %.32s ... %.8s\n", hash_buffer, hash_buffer+120);
        }
        if (cart->Arm7Hash != NULL)
        {
            sha512_to_hex(cart->Arm7Hash, hash_buffer);
            s = sdscatprintf(s, " Arm7 SHA512:       %.32s ... %.8s\n", hash_buffer, hash_buffer+120);
        }
        if (cart->Arm9OverlayHash != NULL)
        {
            sha512_to_hex(cart->Arm9OverlayHash, hash_buffer);
            s = sdscatprintf(s, " Arm9Ovr SHA512:    %.32s ... %.8s\n", hash_buffer, hash_buffer+120);
        }
        if (cart->Arm7OverlayHash != NULL)
        {
            sha512_to_hex(cart->Arm7OverlayHash, hash_buffer);
            s = sdscatprintf(s, " Arm7Ovr SHA512:    %.32s ... %.8s\n", hash_buffer, hash_buffer+120);
        }

        // Now report on the files
        if (cart->FileTable != NULL)
        {
            s = sdscatprintf(s, " %d files in %d directories\n", cart->FileTable->NumFiles, cart->FileTable->NumDirectories);
            for (int i = 0; i < cart->FileTable->NumFiles; i++)
            {
                nds_cartridge_file_t * cur_file = cart->FileTable->Files + i;
                sha512_to_hex(cur_file->FileHash, hash_buffer);
                s = sdscatprintf(s, "  %05d %s\n   %.32s ... %.8s\n", cur_file->FileID, cur_file->FullFileName, hash_buffer, hash_buffer + 120);
            }
        }
    }
    s = sdscat(s, "\n");

    // Return a regular, boring, no special library needed string to the end user.
    char * ret = malloc(sdslen(s) + 1);
    memcpy(ret, s, sdslen(s) + 1);
    sdsfree(s);

    return ret;
}
