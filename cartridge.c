#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cartridge_private.h"
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
 * A Nintendo DS rom is laid out as follows:
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
    //ndsHeader_t * header = ((ndsHeader_t *)(cart->data));

    return false;
}


// Analyzing
bool validate_cartridge(const nds_cartridge_t * cart)
{
    // This proc needs to go through and ensure the file we loaded could
    // conceivably pass as a cartridge, even a broken one.
    // It mostly sanity checks offsets.

    // At bare minimum the cart needs to be LARGE enough to hold the header.
    // The smallest cart known to exist is an experiment (for doing just that)
    // which stores EVERYTHING in the header itself in the reserved spots.
    // It is 352 bytes--just large enough to store the CRC.
    if (cart->size < offsetof(ndsHeader_t, _reserved3))
    //if (cart->size < sizeof(ndsHeader_t))
        return false;

    // It should also be large enough to hold everything the header points to.
    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    if (cart->size < header->Arm9RomOffset + header->Arm9Size)
        return false;
    if (cart->size < header->Arm7RomOffset + header->Arm7Size)
        return false;

    return true;
}

// Hashing (cart)
uint16_t get_cart_crc16(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    return crc16(cart->data, cart->size);
}
uint32_t get_cart_crc32(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    return crc32buf(cart->data, cart->size);
}
MD5_HASH * get_cart_md5(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Md5Context md5Context;
    MD5_HASH * md5Hash = malloc(sizeof(MD5_HASH));

    Md5Initialise(&md5Context);
    Md5Update(&md5Context, cart->data, cart->size);
    Md5Finalise(&md5Context, md5Hash);

    return md5Hash;
}
SHA1_HASH * get_cart_sha1(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Sha1Context sha1Context;
    SHA1_HASH * sha1Hash = malloc(sizeof(SHA1_HASH));

    Sha1Initialise(&sha1Context);
    Sha1Update(&sha1Context, cart->data, cart->size);
    Sha1Finalise(&sha1Context, sha1Hash);

    return sha1Hash;
}
SHA256_HASH * get_cart_sha256(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Sha256Context sha256Context;
    SHA256_HASH * sha256Hash = malloc(sizeof(SHA256_HASH));

    Sha256Initialise(&sha256Context);
    Sha256Update(&sha256Context, cart->data, cart->size);
    Sha256Finalise(&sha256Context, sha256Hash);

    return sha256Hash;
}
SHA512_HASH * get_cart_sha512(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Sha512Context sha512Context;
    SHA512_HASH * sha512Hash = malloc(sizeof(SHA512_HASH));

    Sha512Initialise(&sha512Context);
    Sha512Update(&sha512Context, cart->data, cart->size);
    Sha512Finalise(&sha512Context, sha512Hash);

    return sha512Hash;
}

// Hashing (cart header aka the first 350 bytes)
uint16_t get_cart_header_crc16(const nds_cartridge_t * cart)
{
    // This one in particular should match what the ROM header itself lists
    assert(cart != NULL);

    return crc16(cart->data, offsetof(ndsHeader_t, HeaderCrc));
}
uint32_t get_cart_header_crc32(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    return crc32buf(cart->data, offsetof(ndsHeader_t, HeaderCrc));
}
MD5_HASH * get_cart_header_md5(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Md5Context md5Context;
    MD5_HASH * md5Hash = malloc(sizeof(MD5_HASH));

    Md5Initialise(&md5Context);
    Md5Update(&md5Context, cart->data, offsetof(ndsHeader_t, HeaderCrc));
    Md5Finalise(&md5Context, md5Hash);

    return md5Hash;
}
SHA1_HASH * get_cart_header_sha1(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Sha1Context sha1Context;
    SHA1_HASH * sha1Hash = malloc(sizeof(SHA1_HASH));

    Sha1Initialise(&sha1Context);
    Sha1Update(&sha1Context, cart->data, offsetof(ndsHeader_t, HeaderCrc));
    Sha1Finalise(&sha1Context, sha1Hash);

    return sha1Hash;
}
SHA256_HASH * get_cart_header_sha256(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Sha256Context sha256Context;
    SHA256_HASH * sha256Hash = malloc(sizeof(SHA256_HASH));

    Sha256Initialise(&sha256Context);
    Sha256Update(&sha256Context, cart->data, offsetof(ndsHeader_t, HeaderCrc));
    Sha256Finalise(&sha256Context, sha256Hash);

    return sha256Hash;
}
SHA512_HASH * get_cart_header_sha512(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Sha512Context sha512Context;
    SHA512_HASH * sha512Hash = malloc(sizeof(SHA512_HASH));

    Sha512Initialise(&sha512Context);
    Sha512Update(&sha512Context, cart->data, offsetof(ndsHeader_t, HeaderCrc));
    Sha512Finalise(&sha512Context, sha512Hash);

    return sha512Hash;
}

// Hashing (cart arm9 blob size and position depends on what the header says)
/* These blobs can be encrypted.  Do I care?  Probably not since the only
 * time that would matter to me is if the same blob is used in multiple
 * versions of a cart yet encrypted differently each time.  To research.
  **/
uint16_t get_cart_arm9_crc16(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    return crc16(cart->data + header->Arm9RomOffset, header->Arm9Size);
}
uint32_t get_cart_arm9_crc32(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    return crc32buf(cart->data + header->Arm9RomOffset, header->Arm9Size);
}
MD5_HASH * get_cart_arm9_md5(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Md5Context md5Context;
    MD5_HASH * md5Hash = malloc(sizeof(MD5_HASH));

    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    Md5Initialise(&md5Context);
    Md5Update(&md5Context, cart->data + header->Arm9RomOffset, header->Arm9Size);
    Md5Finalise(&md5Context, md5Hash);

    return md5Hash;
}
SHA1_HASH * get_cart_arm9_sha1(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Sha1Context sha1Context;
    SHA1_HASH * sha1Hash = malloc(sizeof(SHA1_HASH));

    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    Sha1Initialise(&sha1Context);
    Sha1Update(&sha1Context, cart->data + header->Arm9RomOffset, header->Arm9Size);
    Sha1Finalise(&sha1Context, sha1Hash);

    return sha1Hash;
}
SHA256_HASH * get_cart_arm9_sha256(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Sha256Context sha256Context;
    SHA256_HASH * sha256Hash = malloc(sizeof(SHA256_HASH));

    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    Sha256Initialise(&sha256Context);
    Sha256Update(&sha256Context, cart->data + header->Arm9RomOffset, header->Arm9Size);
    Sha256Finalise(&sha256Context, sha256Hash);

    return sha256Hash;
}
SHA512_HASH * get_cart_arm9_sha512(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Sha512Context sha512Context;
    SHA512_HASH * sha512Hash = malloc(sizeof(SHA512_HASH));

    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    Sha512Initialise(&sha512Context);
    Sha512Update(&sha512Context, cart->data + header->Arm9RomOffset, header->Arm9Size);
    Sha512Finalise(&sha512Context, sha512Hash);

    return sha512Hash;
}

// Hashing (cart arm7 blob size and position depends on what the header says)
uint8_t get_cart_arm7_crc16(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    return crc16(cart->data + header->Arm7RomOffset, header->Arm7Size);

}
uint32_t get_cart_arm7_crc32(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    return crc32buf(cart->data + header->Arm7RomOffset, header->Arm7Size);
}
MD5_HASH * get_cart_arm7_md5(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Md5Context md5Context;
    MD5_HASH * md5Hash = malloc(sizeof(MD5_HASH));

    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    Md5Initialise(&md5Context);
    Md5Update(&md5Context, cart->data + header->Arm7RomOffset, header->Arm7Size);
    Md5Finalise(&md5Context, md5Hash);

    return md5Hash;
}
SHA1_HASH * get_cart_arm7_sha1(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Sha1Context sha1Context;
    SHA1_HASH * sha1Hash = malloc(sizeof(SHA1_HASH));

    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    Sha1Initialise(&sha1Context);
    Sha1Update(&sha1Context, cart->data + header->Arm7RomOffset, header->Arm7Size);
    Sha1Finalise(&sha1Context, sha1Hash);

    return sha1Hash;
}
SHA256_HASH * get_cart_arm7_sha256(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Sha256Context sha256Context;
    SHA256_HASH * sha256Hash = malloc(sizeof(SHA256_HASH));

    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    Sha256Initialise(&sha256Context);
    Sha256Update(&sha256Context, cart->data + header->Arm7RomOffset, header->Arm7Size);
    Sha256Finalise(&sha256Context, sha256Hash);

    return sha256Hash;
}
SHA512_HASH * get_cart_arm7_sha512(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    Sha512Context sha512Context;
    SHA512_HASH * sha512Hash = malloc(sizeof(SHA512_HASH));

    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));
    Sha512Initialise(&sha512Context);
    Sha512Update(&sha512Context, cart->data + header->Arm7RomOffset, header->Arm7Size);
    Sha512Finalise(&sha512Context, sha512Hash);

    return sha512Hash;
}



// Reporting
char * cartridge_info(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    // Assemble the base header info first
    bool is_dsi = is_cartridge_dsi(cart);
    bool is_homebrew = is_cartridge_homebrew(cart);
    ndsHeader_t * header = ((ndsHeader_t *)(cart->data));

    sds s = sdsempty();
    s = sdscatprintf(s, " Cartridge Title: %20.12s ", (is_homebrew && memcmp(header->GameTitle, homebrew_header, sizeof(homebrew_header)) == 0) ? " (homebrew) " : header->GameTitle);
    if (is_homebrew && memcmp(header->GameCode, homebrew_gamecode, sizeof(homebrew_gamecode)) == 0)
        s = sdscatprintf(s, " Expected Serial: %20s \n", " (homebrew) ");
    else
        s = sdscatprintf(s, " Expected Serial:  %20s-%.3s-%s\n", (is_dsi) ? "TWL" : "NTR", header->GameCode, GetRegionFromCode(header->GameCode[3]));
    s = sdscatprintf(s, " MC:  %12d  UC:  %12d  Cap: %12d  Ver: %12d \n", header->MakerCode, header->UnitCode, 0x20000 << header->DeviceCapacity, header->RomVersion);
    s = sdscatprintf(s, " Arm9 Off:   %10x Entry: %10x Load:  %10x Size:  %10x \n", header->Arm9RomOffset, header->Arm9EntryAddress, header->Arm9LoadAddress, header->Arm9Size);
    s = sdscatprintf(s, " Arm7 Off:   %10x Entry: %10x Load:  %10x Size:  %10x \n", header->Arm7RomOffset, header->Arm7EntryAddress, header->Arm7LoadAddress, header->Arm7Size);
    s = sdscatprintf(s, " FNT:    %10x Size:  %10x    FAT:    %10x Size:  %10x \n", header->FileNameTableOffset, header->FileNameTableLength, header->FileAllocationTableOffset, header->FileAllocationTableLength);
    s = sdscatprintf(s, " A9ovr:  %10x Size:  %10x    A7ovr:  %10x Size:  %10x \n", header->Arm9OverlayOffset, header->Arm9OverlayLength, header->Arm9OverlayOffset, header->Arm9OverlayLength);
    s = sdscat(s, "\n");

    // Hash checks!  Yay.
    // Todo move this to a separate module.
    uint16_t cart_header_crc = get_cart_header_crc16(cart);
    uint32_t cart_crc = get_cart_crc32(cart);
    SHA512_HASH * cart_sha = get_cart_sha512(cart);
    SHA512_HASH * cart_header_sha = get_cart_header_sha512(cart);
    SHA512_HASH * cart_arm9_sha = get_cart_arm9_sha512(cart);
    SHA512_HASH * cart_arm7_sha = get_cart_arm7_sha512(cart);

    char * s_cart_sha = sha512_to_hex(cart_sha);
    char * s_cart_header_sha = sha512_to_hex(cart_header_sha);
    char * s_cart_arm9_sha = sha512_to_hex(cart_arm9_sha);
    char * s_cart_arm7_sha = sha512_to_hex(cart_arm7_sha);

    s = sdscatprintf(s, " Header CRC16 (reported):  %04x\n", header->HeaderCrc);
    s = sdscatprintf(s, " Header CRC16 (calc):      %04x\n", cart_header_crc);
    s = sdscatprintf(s, " Cart CRC:             %08x\n", cart_crc);
    s = sdscatprintf(s, " Cart SHA512:    %.32s ... %.8s\n", s_cart_sha, s_cart_sha+120);
    s = sdscatprintf(s, " Header SHA512:  %.32s ... %.8s\n", s_cart_header_sha, s_cart_header_sha+120);
    s = sdscatprintf(s, " Arm9 SHA512:    %.32s ... %.8s\n", s_cart_arm9_sha, s_cart_arm9_sha+120);
    s = sdscatprintf(s, " Arm7 SHA512:    %.32s ... %.8s\n", s_cart_arm7_sha, s_cart_arm7_sha+120);


    // Clean up hashes
    free(cart_sha);
    free(cart_header_sha);
    free(cart_arm9_sha);
    free(cart_arm7_sha);
    free(s_cart_sha);
    free(s_cart_header_sha);
    free(s_cart_arm9_sha);
    free(s_cart_arm7_sha);



    // Now report on the DSi header (if it exists)


    s = sdscat(s, "\n");
    // Return a regular, boring, no special library needed string to the end user.
    char * ret = malloc(sdslen(s) + 1);
    memcpy(ret, s, sdslen(s) + 1);
    sdsfree(s);

    return ret;
}
