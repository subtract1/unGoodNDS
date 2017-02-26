/* A cartridge contains a pointer to the blob of data that it represents.
 * It knows if it's a normal cartridge, a DSi cartridge, some weird sort
 * of homebrew, and it can tell you a lot of info about itself.
 *
 * It is given an opaque pointer and interacted only through a library.
 */

#ifndef _CARTRIDGE_PRIVATE_H
#define _CARTRIDGE_PRIVATE_H

#include <stdint.h>
#include "cartridge.h"


// Structs
struct nds_cartridge_s
{
    unsigned int size; // Must be a multiple of 2, if it isn't this is either homebrew or a trimmed/overdumped rom
    uint8_t * data; // Pointer to blob of data, it should match the headers we have defined elsewhere
};


// Function decs
nds_cartridge_t * load_nds_cartridge(FILE *);
void free_nds_cartridge(nds_cartridge_t *);

bool validate_cartridge(const nds_cartridge_t *);

#endif
