#ifndef _HASH_HELPER_H
#define _HASH_HELPER_H

#include <stdint.h>
#include "libraries/CryptLib/LibMd5.h"
#include "libraries/CryptLib/LibSha1.h"
#include "libraries/CryptLib/LibSha256.h"
#include "libraries/CryptLib/LibSha512.h"

// Hash to hex string
char * crc16_to_hex(uint16_t hash);
char * crc32_to_hex(uint32_t hash);
char * md5_to_hex(const MD5_HASH * hash);
char * sha1_to_hex(const SHA1_HASH * hash);
char * sha256_to_hex(const SHA256_HASH * hash);
char * sha512_to_hex(const SHA512_HASH * hash);

// Hex string to hash
uint16_t hex_to_crc16(const char * hex);
uint32_t hex_to_crc32(const char * hex);
MD5_HASH * hex_to_md5(const char * hex);
SHA1_HASH * hex_to_sha1(const char * hex);
SHA256_HASH * hex_to_sha256(const char * hex);
SHA512_HASH * hex_to_sha512(const char * hex);

#endif
