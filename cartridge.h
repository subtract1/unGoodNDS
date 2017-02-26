/* A cartridge contains a pointer to the blob of data that it represents.
 * It knows if it's a normal cartridge, a DSi cartridge, some weird sort
 * of homebrew, and it can tell you a lot of info about itself.
 *
 * It is given an opaque pointer and interacted only through a library.
 */

#ifndef _CARTRIDGE_H
#define _CARTRIDGE_H

#include <stdbool.h>
#include <stdio.h>

struct nds_cartridge_s;
typedef struct nds_cartridge_s nds_cartridge_t;


nds_cartridge_t * load_nds_cartridge(FILE * fp);
void free_nds_cartridge(nds_cartridge_t * cart);
char * cartridge_info(const nds_cartridge_t * cart);

#endif
