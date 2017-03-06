#ifndef _HASH_HELPER_H
#define _HASH_HELPER_H

#include <stdint.h>
#include "libraries/CryptLib/LibMd5.h"
#include "libraries/CryptLib/LibSha1.h"
#include "libraries/CryptLib/LibSha256.h"
#include "libraries/CryptLib/LibSha512.h"


// Blob to hash
uint16_t get_crc16(void * buf, size_t buflen);
uint32_t get_crc32(void * buf, size_t buflen);
MD5_HASH * get_md5(void * buf, size_t buflen);
SHA1_HASH * get_sha1(void * buf, size_t buflen);
SHA256_HASH * get_sha256(void * buf, size_t buflen);
SHA512_HASH * get_sha512(void * buf, size_t buflen);


// Hash to hex string
void md5_to_hex(const MD5_HASH * hash, char * out_hex);
void sha1_to_hex(const SHA1_HASH * hash, char * out_hex);
void sha256_to_hex(const SHA256_HASH * hash, char * out_hex);
void sha512_to_hex(const SHA512_HASH * hash, char * out_hex);
char * md5_to_hex_str(const MD5_HASH * hash);
char * sha1_to_hex_str(const SHA1_HASH * hash);
char * sha256_to_hex_str(const SHA256_HASH * hash);
char * sha512_to_hex_str(const SHA512_HASH * hash);

// Hex string to hash
uint16_t hex_to_crc16(const char * hex);
uint32_t hex_to_crc32(const char * hex);
MD5_HASH * hex_to_md5(const char * hex);
SHA1_HASH * hex_to_sha1(const char * hex);
SHA256_HASH * hex_to_sha256(const char * hex);
SHA512_HASH * hex_to_sha512(const char * hex);


#endif
