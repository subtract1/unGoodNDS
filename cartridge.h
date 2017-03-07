/* A cartridge contains a pointer to the blob of data that it represents.
 * It knows if it's a normal cartridge, a DSi cartridge, some weird sort
 * of homebrew, and it can tell you a lot of info about itself.
 */

#ifndef _CARTRIDGE_H
#define _CARTRIDGE_H

#include <stdint.h>
#include "libraries/CryptLib/LibMd5.h"
#include "libraries/CryptLib/LibSha1.h"
#include "libraries/CryptLib/LibSha256.h"
#include "libraries/CryptLib/LibSha512.h"


// Structs
typedef struct nds_cartridge_s
{
    int Status; // 0 == good, < 0 == validation failed and only the base hash will be available.
    size_t Size; // Must be a power of 2, if it isn't this is either homebrew or a trimmed/overdumped rom
    uint8_t * Data; // Pointer to blob of data, it should match the headers we have defined elsewhere

    uint32_t CartCrc;
    SHA512_HASH * CartHash;

    uint16_t HeaderCrc16;
    size_t TrimSize; // Has to be calculated.
    SHA512_HASH * TrimHash;
    SHA512_HASH * Arm9Hash;
    SHA512_HASH * Arm7Hash;
    SHA512_HASH * Arm9OverlayHash;
    SHA512_HASH * Arm7OverlayHash;
    struct nds_cartridge_banner_s * Banner;
    struct nds_cartridge_filetable_s * FileTable;
} nds_cartridge_t;


// Decs
nds_cartridge_t * create_nds_cartridge(FILE * fp);
void free_nds_cartridge(nds_cartridge_t * cart);
char * cartridge_info(const nds_cartridge_t * cart);

#endif
