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
    struct nds_cartridge_filetable_s * FileTable;
} nds_cartridge_t;


// Function decs
nds_cartridge_t * create_nds_cartridge(FILE * fp);
void free_nds_cartridge(nds_cartridge_t * cart);
const char * validate_cartridge_errmsg(int errno);
char * cartridge_info(const nds_cartridge_t * cart);

uint16_t get_cart_crc16(const nds_cartridge_t *);
uint32_t get_cart_crc32(const nds_cartridge_t *);
MD5_HASH * get_cart_md5(const nds_cartridge_t *);
SHA1_HASH * get_cart_sha1(const nds_cartridge_t *);
SHA256_HASH * get_cart_sha256(const nds_cartridge_t *);
SHA512_HASH * get_cart_sha512(const nds_cartridge_t *);
uint16_t get_trimcart_crc16(const nds_cartridge_t *);
uint32_t get_trimcart_crc32(const nds_cartridge_t *);
MD5_HASH * get_trimcart_md5(const nds_cartridge_t *);
SHA1_HASH * get_trimcart_sha1(const nds_cartridge_t *);
SHA256_HASH * get_trimcart_sha256(const nds_cartridge_t *);
SHA512_HASH * get_trimcart_sha512(const nds_cartridge_t *);
uint16_t get_cart_header_crc16(const nds_cartridge_t *);
uint32_t get_cart_header_crc32(const nds_cartridge_t *);
MD5_HASH * get_cart_header_md5(const nds_cartridge_t *);
SHA1_HASH * get_cart_header_sha1(const nds_cartridge_t *);
SHA256_HASH * get_cart_header_sha256(const nds_cartridge_t *);
SHA512_HASH * get_cart_header_sha512(const nds_cartridge_t *);
uint16_t get_cart_arm9_crc16(const nds_cartridge_t *);
uint32_t get_cart_arm9_crc32(const nds_cartridge_t *);
MD5_HASH * get_cart_arm9_md5(const nds_cartridge_t *);
SHA1_HASH * get_cart_arm9_sha1(const nds_cartridge_t *);
SHA256_HASH * get_cart_arm9_sha256(const nds_cartridge_t *);
SHA512_HASH * get_cart_arm9_sha512(const nds_cartridge_t *);
uint16_t get_cart_arm7_crc16(const nds_cartridge_t *);
uint32_t get_cart_arm7_crc32(const nds_cartridge_t *);
MD5_HASH * get_cart_arm7_md5(const nds_cartridge_t *);
SHA1_HASH * get_cart_arm7_sha1(const nds_cartridge_t *);
SHA256_HASH * get_cart_arm7_sha256(const nds_cartridge_t *);
SHA512_HASH * get_cart_arm7_sha512(const nds_cartridge_t *);
uint16_t get_cart_arm9ovr_crc16(const nds_cartridge_t *);
uint32_t get_cart_arm9ovr_crc32(const nds_cartridge_t *);
MD5_HASH * get_cart_arm9ovr_md5(const nds_cartridge_t *);
SHA1_HASH * get_cart_arm9ovr_sha1(const nds_cartridge_t *);
SHA256_HASH * get_cart_arm9ovr_sha256(const nds_cartridge_t *);
SHA512_HASH * get_cart_arm9ovr_sha512(const nds_cartridge_t *);

// Hashing (cart arm7 overlay, may not exist)
uint16_t get_cart_arm7ovr_crc16(const nds_cartridge_t *);
uint32_t get_cart_arm7ovr_crc32(const nds_cartridge_t *);
MD5_HASH * get_cart_arm7ovr_md5(const nds_cartridge_t *);
SHA1_HASH * get_cart_arm7ovr_sha1(const nds_cartridge_t *);
SHA256_HASH * get_cart_arm7ovr_sha256(const nds_cartridge_t *);
SHA512_HASH * get_cart_arm7ovr_sha512(const nds_cartridge_t *);


void create_cart_hashes(nds_cartridge_t *);
int validate_cartridge(const nds_cartridge_t *);

#endif
