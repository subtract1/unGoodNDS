#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "hash_helper.h"
#include "libraries/crc.h"
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


// Blob to hash
uint16_t get_crc16(void * buf, size_t buflen)
{
    assert(buf != NULL);

    return crc16(buf, buflen);
}
uint32_t get_crc32(void * buf, size_t buflen)
{
    assert(buf != NULL);

    return crc32buf(buf, buflen);
}
MD5_HASH * get_md5(void * buf, size_t buflen)
{
    assert(buf != NULL);

    Md5Context md5Context;
    MD5_HASH * md5Hash = malloc(sizeof(MD5_HASH));

    Md5Initialise(&md5Context);
    Md5Update(&md5Context, buf, buflen);
    Md5Finalise(&md5Context, md5Hash);

    return md5Hash;
}
SHA1_HASH * get_sha1(void * buf, size_t buflen)
{
    assert(buf != NULL);

    Sha1Context sha1Context;
    SHA1_HASH * sha1Hash = malloc(sizeof(SHA1_HASH));

    Sha1Initialise(&sha1Context);
    Sha1Update(&sha1Context, buf, buflen);
    Sha1Finalise(&sha1Context, sha1Hash);

    return sha1Hash;
}
SHA256_HASH * get_sha256(void * buf, size_t buflen)
{
    assert(buf != buf);

    Sha256Context sha256Context;
    SHA256_HASH * sha256Hash = malloc(sizeof(SHA256_HASH));

    Sha256Initialise(&sha256Context);
    Sha256Update(&sha256Context, buf, buflen);
    Sha256Finalise(&sha256Context, sha256Hash);

    return sha256Hash;
}
SHA512_HASH * get_sha512(void * buf, size_t buflen)
{
    assert(buf != NULL);

    Sha512Context sha512Context;
    SHA512_HASH * sha512Hash = malloc(sizeof(SHA512_HASH));

    Sha512Initialise(&sha512Context);
    Sha512Update(&sha512Context, buf, buflen);
    Sha512Finalise(&sha512Context, sha512Hash);

    return sha512Hash;
}

// Hash to hex string
void md5_to_hex(const MD5_HASH * hash, char * out_hex)
{
    assert(hash != NULL);
    assert(out_hex != NULL);

    for (int i = 0; i < sizeof(*hash); i++)
    {
        byte_to_hex(hash->bytes[i], out_hex + (i * 2));
    }
    out_hex[sizeof(*hash)] = '\0';
}
void sha1_to_hex(const SHA1_HASH * hash, char * out_hex)
{
    assert(hash != NULL);
    assert(out_hex != NULL);

    for (int i = 0; i < sizeof(*hash); i++)
    {
        byte_to_hex(hash->bytes[i], out_hex + (i * 2));
    }
    out_hex[sizeof(*hash)] = '\0';
}
void sha256_to_hex(const SHA256_HASH * hash, char * out_hex)
{
    assert(hash != NULL);
    assert(out_hex != NULL);

    for (int i = 0; i < sizeof(*hash); i++)
    {
        byte_to_hex(hash->bytes[i], out_hex + (i * 2));
    }
    out_hex[sizeof(*hash)] = '\0';
}
void sha512_to_hex(const SHA512_HASH * hash, char * out_hex)
{
    assert(hash != NULL);
    assert(out_hex != NULL);

    for (int i = 0; i < sizeof(*hash); i++)
    {
        byte_to_hex(hash->bytes[i], out_hex + (i * 2));
    }
    out_hex[sizeof(*hash)] = '\0';
}

char * md5_to_hex_str(const MD5_HASH * hash)
{
    assert(hash != NULL);
    static const size_t strLength = sizeof(*hash) * 2 + 1;

    char * ret = malloc(strLength);
    md5_to_hex(hash, ret);
    return ret;
}
char * sha1_to_hex_str(const SHA1_HASH * hash)
{
    assert(hash != NULL);
    static const size_t strLength = sizeof(*hash) * 2 + 1;

    char * ret = malloc(strLength);
    sha1_to_hex(hash, ret);
    return ret;
}
char * sha256_to_hex_str(const SHA256_HASH * hash)
{
    assert(hash != NULL);
    static const size_t strLength = sizeof(*hash) * 2 + 1;

    char * ret = malloc(strLength);
    sha256_to_hex(hash, ret);
    return ret;
}
char * sha512_to_hex_str(const SHA512_HASH * hash)
{
    assert(hash != NULL);
    static const size_t strLength = sizeof(*hash) * 2 + 1;

    char * ret = malloc(strLength);
    sha512_to_hex(hash, ret);
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

