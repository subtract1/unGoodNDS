/* This defines various structs and headers that make up a NDS rom.
 * We will load our file, search for markers and other identifying
 * characteristics, then cast our byte array to the appropriate struct
 * in order to easily extract the important stuff we care about.
 */

#ifndef _CARTRIDGE_HEADER_H
#define _CARTRIDGE_HEADER_H

#include <stdint.h>


// Since these all map to data that exists on disk
// we must make sure every struct is perfectly aligned.
// I'm pretty sure this is all little endian.
// Were I to target big endian platforms I'd need to do something about that
// but I'm not so I won't.
struct __attribute__((__packed__)) ndsHeader_s
{
    char GameTitle[12]; // This may not be a null terminated string!
    char GameCode[4]; // Three bytes semi-random, one byte region.
    uint16_t MakerCode;
    uint8_t UnitCode; // (00h=NDS, 02h=NDS+DSi, 03h=DSi) (bit1=DSi)
    uint8_t EncryptionSeedSelect;
    uint8_t DeviceCapacity;
    uint8_t _reserved1[7];
    uint8_t _unknown1[2];
    uint8_t RomVersion;
    uint8_t InternalFags; //(Bit2: Autostart)

    uint32_t Arm9RomOffset;
    uint32_t Arm9EntryAddress;
    uint32_t Arm9LoadAddress;
    uint32_t Arm9Size;
    uint32_t Arm7RomOffset;
    uint32_t Arm7EntryAddress;
    uint32_t Arm7LoadAddress;
    uint32_t Arm7Size;

    uint32_t FileNameTableOffset;
    uint32_t FileNameTableLength;
    uint32_t FileAllocationTableOffset;
    uint32_t FileAllocationTableLength;

    uint32_t Arm9OverlayOffset;
    uint32_t Arm9OverlayLength;
    uint32_t Arm7OverlayOffset;
    uint32_t Arm7OverlayLength;

    uint32_t NormalCardControlRegisterSettings;
    uint32_t SecureCardControlRegisterSettings;
    uint32_t IconBannerOffset;
    uint8_t SecureAreaCrc[2];
    uint16_t SecureTransferTimeout;
    uint32_t Arm9Autoload;
    uint32_t Arm7Autoload;
    uint8_t SecureDisable[8];
    uint32_t NtrRegionRomSize;
    uint32_t HeaderSize;
    uint8_t _reserved2[56];

    uint8_t NintendoLogo[156]; // Should always be the same
    uint8_t NintendoLogoCrc[2]; // Should be 0xCF56
    uint16_t HeaderCrc; // CRC of everything before this
    uint8_t _reserved3[32];
};
struct __attribute__((__packed__)) ndsDsiHeader_s
{
    uint8_t GameTitle[12]; // This may not be a null terminated string!
    uint8_t GameCode[4]; // Three bytes semi-random, one byte region.
    uint16_t MakerCode;
    uint8_t UnitCode; // (00h=NDS, 02h=NDS+DSi, 03h=DSi) (bit1=DSi)
    uint8_t EncryptionSeedSelect;
    uint8_t DeviceCapacity;
    uint8_t _reserved1[7];
    uint8_t _unknown1[2];
    uint8_t RomVersion;
    uint8_t InternalFags; //(Bit2: Autostart)

    uint32_t Arm9RomOffset;
    uint32_t Arm9EntryAddress;
    uint32_t Arm9LoadAddress;
    uint32_t Arm9Size;
    uint32_t Arm7RomOffset;
    uint32_t Arm7EntryAddress;
    uint32_t Arm7LoadAddress;
    uint32_t Arm7Size;

    uint32_t FileNameTableOffset;
    uint32_t FileNameTableLength;
    uint32_t FileAllocationTableOffset;
    uint32_t FileAllocationTableLength;

    uint32_t Arm9OverlayOffset;
    uint32_t Arm9OverlayLength;
    uint32_t Arm7OverlayOffset;
    uint32_t Arm7OverlayLength;

    uint32_t NormalCardControlRegisterSettings;
    uint32_t SecureCardControlRegisterSettings;
    uint32_t IconBannerOffset;
    uint8_t SecureAreaCrc[2];
    uint16_t SecureTransferTimeout;
    uint32_t Arm9Autoload;
    uint32_t Arm7Autoload;
    uint8_t SecureDisable[8];
    uint32_t NtrRegionRomSize;
    uint32_t HeaderSize;
    uint8_t _reserved2[56];

    uint8_t NintendoLogo[156]; // Should always be the same
    uint8_t NintendoLogoCrc[2]; // Should be 0xCF56
    uint8_t HeaderCrc[2]; // CRC of everything before this
    uint8_t _reserved3[32];

    uint32_t Mbk1Settings;
    uint32_t Mbk2Settings;
    uint32_t Mbk3Settings;
    uint32_t Mbk4Settings;
    uint32_t Mbk5Settings;
    uint32_t Mbk6Arm9Settings;
    uint32_t Mbk7Arm9Settings;
    uint32_t Mbk8Arm9Settings;
    uint32_t Mbk6Arm7Settings;
    uint32_t Mbk7Arm7Settings;
    uint32_t Mbk8Arm7Settings;
    uint32_t Mbk9Settings;

    uint32_t RegionFlags;
    uint32_t AccessControl;
    uint32_t Arm7ScfgExtMask;
    uint32_t AdditionalBannerFlags;

    uint32_t Arm9iRomOffset;
    uint32_t Arm9iEntryAddress;
    uint32_t Arm9iLoadAddress;
    uint32_t Arm9iSize;
    uint32_t Arm7iRomOffset;
    uint32_t Arm7iEntryAddress;
    uint32_t Arm7iLoadAddress;
    uint32_t Arm7iSize;

    uint32_t DigestNtrRegionOffset;
    uint32_t DigestNtrRegionLength;
    uint32_t DigestTwlRegionOffset;
    uint32_t DigestTwlRegionLength;
    uint32_t DigestSectorHashtableOffset;
    uint32_t DigestSectorHashtableLength;
    uint32_t DigestBlockHashtableOffset;
    uint32_t DigestBlockHashtableLength;
    uint32_t DigestSectorsize;
    uint32_t DigestBlockSectorCount;

    uint32_t IconBannerSize;
    uint8_t _unknown2[4];
    uint32_t NtrPlusTwlRegionRomSize;
    uint8_t _unknown3[12];

    uint32_t ModcryptArea1Offset;
    uint32_t ModcryptArea1Size;
    uint32_t ModcryptArea2Offset;
    uint32_t ModcryptArea2Size;

    uint8_t TitleID[4];
    uint32_t DSiWarePublicSaveSize;
    uint32_t DSiWarePrivateSaveSize;

    uint8_t _reserved4[176];
    uint8_t _unknown4[16];
    uint8_t Arm9SecureSha1Hash[20];
    uint8_t Arm7Sha1Hash[20];
    uint8_t DigestSha1Hash[20];
    uint8_t BannerSha1Hash[20];
    uint8_t Arm9iSha1Hash[20];
    uint8_t Arm7iSha1Hash[20];
    uint8_t _reserved5[40];
    uint8_t Arm9Sha1Hash[20];
    uint8_t _reserved6[2636];
    uint8_t _reserved7[384];
    uint8_t RsaSignature[80];
};


typedef struct ndsHeader_s ndsHeader_t;
typedef struct ndsDsiHeader_s ndsDsiHeader_t;

#endif
