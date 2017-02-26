#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "hash_helper.h"
#include "libraries/CryptLib/LibMd5.h"
#include "libraries/CryptLib/LibSha1.h"
#include "libraries/CryptLib/LibSha256.h"
#include "libraries/CryptLib/LibSha512.h"

/* Misc helper procs for converting between hex strings and byte arrays.
 * They don't do any validation so it's assumed you're not screwing around.
 **/


// Decs
static void byte_to_hex(uint8_t, char *);
static uint8_t hex_to_byte(const char * hex);


// Hash to hex string
char * md5_to_hex(const MD5_HASH * hash)
{
    assert(hash != NULL);
    static const size_t strLength = sizeof(*hash) * 2 + 1;

    char * ret = malloc(strLength);
    for (int i = 0; i < sizeof(*hash); i++)
    {
        byte_to_hex(hash->bytes[i], ret + (i * 2));
    }

    return ret;
}
char * sha1_to_hex(const SHA1_HASH * hash)
{
    assert(hash != NULL);
    static const size_t strLength = sizeof(*hash) * 2 + 1;

    char * ret = malloc(strLength);
    for (int i = 0; i < sizeof(*hash); i++)
    {
        byte_to_hex(hash->bytes[i], ret + (i * 2));
    }

    return ret;
}
char * sha256_to_hex(const SHA256_HASH * hash)
{
    assert(hash != NULL);
    static const size_t strLength = sizeof(*hash) * 2 + 1;

    char * ret = malloc(strLength);
    for (int i = 0; i < sizeof(*hash); i++)
    {
        byte_to_hex(hash->bytes[i], ret + (i * 2));
    }

    return ret;
}
char * sha512_to_hex(const SHA512_HASH * hash)
{
    assert(hash != NULL);
    static const size_t strLength = sizeof(*hash) * 2 + 1;

    char * ret = malloc(strLength);
    for (int i = 0; i < sizeof(*hash); i++)
    {
        byte_to_hex(hash->bytes[i], ret + (i * 2));
    }

    return ret;
}
static void byte_to_hex(uint8_t byte, char * dest)
{
    static const char lookupTable[] = "0123456789abcdef";
    *dest = lookupTable[byte>>4];
    *(dest+1) = lookupTable[byte&0x0f];
}

// Hex string to hash
MD5_HASH * hex_to_md5(const char * hex)
{
    assert(hex != NULL);
    static const size_t strLength = sizeof(MD5_HASH) * 2;

    MD5_HASH * ret = malloc(sizeof(MD5_HASH));
    for (int i = 0; i < strLength; i += 2)
    {
        ret->bytes[i] = hex_to_byte(hex + i);
    }

    return ret;
}
SHA1_HASH * hex_to_sha1(const char * hex)
{
    assert(hex != NULL);
    static const size_t strLength = sizeof(SHA1_HASH) * 2;

    SHA1_HASH * ret = malloc(sizeof(SHA1_HASH));
    for (int i = 0; i < strLength; i += 2)
    {
        ret->bytes[i] = hex_to_byte(hex + i);
    }

    return ret;
}
SHA256_HASH * hex_to_sha256(const char * hex)
{
    assert(hex != NULL);
    static const size_t strLength = sizeof(SHA256_HASH) * 2;

    SHA256_HASH * ret = malloc(sizeof(SHA256_HASH));
    for (int i = 0; i < strLength; i += 2)
    {
        ret->bytes[i] = hex_to_byte(hex + i);
    }

    return ret;
}
SHA512_HASH * hex_to_sha512(const char * hex)
{
    assert(hex != NULL);
    static const size_t strLength = sizeof(SHA512_HASH) * 2;

    SHA512_HASH * ret = malloc(sizeof(SHA512_HASH));
    for (int i = 0; i < strLength; i += 2)
    {
        ret->bytes[i] = hex_to_byte(hex + i);
    }

    return ret;
}
static uint8_t hex_to_byte(const char * hex)
{
    static const char lookupTable[] = "0123456789abcdef";
    uint8_t ret = 0;

    ret = lookupTable[(*hex) - '0'] << 4;
    ret |= lookupTable[(*(hex+1)) - '0'];
    return ret;
}
