#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cartridge.h"
#include "cartridge_filetable.h"
#include "cartridge_header.h"
#include "hash_helper.h"
#include "libraries/sds/sds.h"


// FAT and FNT structures as they exist on disk
typedef struct __attribute__((__packed__)) ndsFat_s
{
    // FileSize == FileEnd - FileStart
    uint32_t FileStart;
    uint32_t FileEnd;
} ndsFat_t;
typedef struct __attribute__((__packed__)) ndsFnt_s
{
    uint32_t NameOffset;
    uint16_t FileID; // The starting index for a given subtable
    uint16_t DirectoryCountOrParentID; // The first entry is the count, all subsequent ones are parents.
} ndsFnt_t;
typedef struct __attribute__((__packed__)) ndsFntEntry_s
{
    uint8_t NameLenAndDirFlag;
    char Name[];
    // uint16_t SubDirID; // This only exists for dir entries and has a variable position.
} ndsFntEntry_t;


// Constructor / Destructor
void create_filetable(const nds_cartridge_t * cart, nds_cartridge_filetable_t * out_table)
{
    assert(cart != NULL);
    assert(out_table != NULL);

    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));

    // If there IS no FAT/FNT this becomes a simple exercise.
    memset(out_table, 0, sizeof(*out_table));
    if (header->FileAllocationTableLength == 0 || header->FileNameTableLength == 0)
        return;

    sds messages = sdsempty();
    ndsFat_t * fat = ((ndsFat_t *)(cart->Data + header->FileAllocationTableOffset));
    ndsFnt_t * fnt = ((ndsFnt_t *)(cart->Data + header->FileNameTableOffset));

    out_table->NumFiles = header->FileAllocationTableLength / sizeof(ndsFat_t);
    out_table->NumDirectories = fnt[0].DirectoryCountOrParentID;
    out_table->Files = malloc(sizeof(*(out_table->Files)) * out_table->NumFiles);
    out_table->Directories = malloc(sizeof(*(out_table->Directories)) * out_table->NumDirectories);

    // Directories are stored sans name.  We're just building out the
    // hierarchy this pass.
    for (int i = 0; i < out_table->NumDirectories; i++)
    {
        nds_cartridge_dir_t * cur_directory = out_table->Directories + i;
        cur_directory->DirectoryID = i;
        if (i == 0)
        {
            cur_directory->ParentID = 0;
            cur_directory->DirectoryName = malloc(sizeof(char) * 2);
            cur_directory->FullDirectoryName = malloc(sizeof(char) * 2);
            strcpy(cur_directory->DirectoryName, ".");
            strcpy(cur_directory->FullDirectoryName, ".");
        }
        else
        {
            cur_directory->ParentID = fnt[i].DirectoryCountOrParentID & 0x0FFF;
        }

        // Now we start allocating the file and directory names.
        // Files also need to be hashed.  For our purposes that's all we need.
        int cur_offset = header->FileNameTableOffset + fnt[i].NameOffset;
        int file_index_base = fnt[i].FileID;
        for (int j = 0;; j++)
        {
            // This struct is of variable size.  Get the offsets.
            uint8_t * name_len_ptr = cart->Data + cur_offset;
            int name_len = ((*name_len_ptr) & 0x7F);
            bool is_dir = ((*name_len_ptr) & 0x80);
            uint8_t * name_ptr = name_len_ptr + 1;
            uint16_t * dir_id_offset = (uint16_t *)(name_ptr + name_len);

            // There's no name count; they are 'null terminated' as it were.
            if (name_len == 0)
                break;

            // Extract the string
            char * name = malloc(name_len + 1);
            memcpy(name, name_ptr, name_len);
            name[name_len] = '\0';

            // Build our object
            if (is_dir)
            {
                int dir_index = (*dir_id_offset) & 0x0FFF;
                nds_cartridge_dir_t * cur_dir = out_table->Directories + dir_index;

                cur_dir->DirectoryName = name;
            }
            else
            {
                int file_index = file_index_base + j;
                nds_cartridge_file_t * cur_file = out_table->Files + file_index;
                cur_file->FileID = file_index;
                cur_file->DirectoryID = cur_directory->DirectoryID;
                cur_file->FileName = name;
                cur_file->FileSize = fat[file_index].FileEnd - fat[file_index].FileStart;
                cur_file->FileHash = get_sha512(cart->Data + fat[file_index].FileStart, cur_file->FileSize);
            }

            // Move our pointer up
            cur_offset += name_len + 1 + ((is_dir) ? 2 : 0);
        }
    }

    // Now that we have our base names taken care of we need to pop the full names.
    for (int i = 1; i < out_table->NumDirectories; i++)
    {
        // I don't like recursion so I'm going to cheat around it.
        nds_cartridge_dir_t * cur_dir = out_table->Directories + i;
        while (cur_dir->FullDirectoryName == NULL)
        {
            nds_cartridge_dir_t * dir1 = cur_dir;
            nds_cartridge_dir_t * dir2 = out_table->Directories + cur_dir->ParentID;
            while (dir2->FullDirectoryName == NULL)
            {
                dir1 = dir2;
                dir2 = out_table->Directories + dir2->ParentID;
            }

            // At this point dir1 has a name and dir2 has a full name
            size_t name_len = strlen(dir2->FullDirectoryName) + 1 + strlen(dir1->DirectoryName) + 1;
            dir1->FullDirectoryName = malloc(sizeof(char) * name_len);
            sprintf(dir1->FullDirectoryName, "%s/%s", dir2->FullDirectoryName, dir1->DirectoryName);
        }

        i++;
    }
    for (int i = 0; i < out_table->NumFiles; i++)
    {
        nds_cartridge_file_t * cur_file = out_table->Files + i;
        nds_cartridge_dir_t * cur_dir = out_table->Directories + cur_file->DirectoryID;

        size_t name_len = strlen(cur_dir->FullDirectoryName) + 1 + strlen(cur_file->FileName) + 1;
        cur_file->FullFileName = malloc(sizeof(char) * name_len);
        sprintf(cur_file->FullFileName, "%s/%s", cur_dir->FullDirectoryName, cur_file->FileName);
    }

    // Return warning messages (if we have any) as a standard string.
    if (sdslen(messages) > 0)
    {
        out_table->Messages = malloc(sdslen(messages) + 1);
        memcpy(out_table->Messages, messages, sdslen(messages) + 1);
    }

    sdsfree(messages);
}
nds_cartridge_filetable_t * load_filetable(const nds_cartridge_t * cart)
{
    assert(cart != NULL);

    nds_cartridge_filetable_t * ret = malloc(sizeof(nds_cartridge_filetable_t));
    create_filetable(cart, ret);
    return ret;
}
void clear_filetable(nds_cartridge_filetable_t * table)
{
    if (table == NULL)
        return;

    for (int i = 0; i < table->NumFiles; i++)
    {
        free(table->Files[i].FileName);
        free(table->Files[i].FullFileName);
        free(table->Files[i].FileHash);
    }
    for (int i = 0; i < table->NumDirectories; i++)
    {
        free(table->Directories[i].DirectoryName);
        free(table->Directories[i].FullDirectoryName);
    }

    free(table->Files);
    free(table->Directories);
    free(table->Messages);
}
void free_filetable(nds_cartridge_filetable_t * table)
{
    if (table == NULL)
        return;

    clear_filetable(table);
    free(table);
}


// Validation
int validate_cartridge_filetable(const nds_cartridge_t * cart)
{
    // This proc is called as part of the cartridge validation routines.
    // The basic header info should already be validated; we need to ensure
    // all file offsets and dir name offsets fit as well.

    // Check the FAT.  In theory files can be anywhere.  In practice they
    // usually end up chunked but since that's not a guarantee we must
    // check everything anyway.
    ndsHeader_t * header = ((ndsHeader_t *)(cart->Data));
    if (header->FileAllocationTableLength > 0)
    {
        if (header->FileAllocationTableOffset + header->FileAllocationTableLength >= cart->Size)
            return -20;

        ndsFat_t * fat = ((ndsFat_t *)(cart->Data + header->FileAllocationTableOffset));
        for (int i = 0; i < header->FileAllocationTableLength / sizeof(ndsFat_t); i++)
        {
            if (fat[i].FileEnd - fat[i].FileStart < 0)
                return -21;
            if (fat[i].FileEnd >= cart->Size)
                return -22;
        }
    }

    // Looks like the FAT is solid.  The FNT is allowed a bit more leeway
    // but there's plenty to check and it's a bigger pain to do so.
    if (header->FileNameTableLength > 0)
    {
        if (header->FileNameTableOffset + header->FileNameTableLength >= cart->Size)
            return -30;
        if (header->FileNameTableLength < sizeof(ndsFnt_t))
            return -31;

        ndsFnt_t * fnt = ((ndsFnt_t *)(cart->Data + header->FileNameTableOffset));
        int num_dirs = fnt[0].DirectoryCountOrParentID;
        if (num_dirs * sizeof(ndsFnt_t) >= header->FileNameTableLength)
            return -31;

        // Check each directory entry's name blob to ensure it doesn't go over.
        int max_file_index = header->FileAllocationTableLength / sizeof(ndsFat_t);
        for (int i = 0; i < num_dirs; i++)
        {
            // Each string is of variable length.  We know the MAX number of
            // files and we know the end of our FNT blob.  Make sure it fits.
            // This is a null terminated list so to speak so we need at least
            // one byte at all times.
            if (i > 0 && (fnt[i].DirectoryCountOrParentID & 0x0FFF) >= num_dirs)
                return -33;

            int cur_offset = header->FileNameTableOffset + fnt[i].NameOffset;
            int file_index_base = fnt[i].FileID;
            for (int j = 0;; j++)
            {
                if (cur_offset >= header->FileNameTableOffset + header->FileNameTableLength)
                    return -32;

                int name_len = (cart->Data[cur_offset] & 0x7F);
                bool is_dir = (cart->Data[cur_offset] & 0x80);
                if (name_len == 0)
                    break;

                if (is_dir)
                {
                    int dir_index = (*(uint16_t *)(cart->Data + cur_offset + name_len + 1)) & 0x0FFF;
                    if (dir_index == 0 || dir_index >= num_dirs)
                        return -34;
                }
                else if (file_index_base + j >= max_file_index)
                    return -35;

                cur_offset += name_len + 1 + ((is_dir) ? 2 : 0);
            }
        }
    }

    return 0;
}
