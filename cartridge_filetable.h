/* The File Allocation Table and File Name Table provide a simple file
 * structure for DS rom resources.  The structure is a little odd.
 * The FAT is simple.  It lists all of the file offsets and ending points
 * giving us a list of nameless blobs.  Files don't have an explicit ID; they
 * are referred to by array index which is a de facto 'File ID' for us.
 * The FNT lists file names and directories.  The first half describes the
 * directory structure; each directory (implicitly numbered by index again)
 * lists its parent directory, a starting index, and a pointer to the names.
 * The FNT names portion is a bunch of variable length strings.  Once more
 * the IDs are implicit but this time there's an offset to factor in.
 * Each string can be up to 127 chars.  Directories are a special case.
 * Their file ID is meaningless (and often reused by a real file later on).
 * Instead the dir ID is tacked on to the end of the record.
 *
 * If it sounds complex it's not that bad.  The code ought to be much clearer.
 *
 * In any case we want to be able to process all of this because we want to be
 * able to identify unknown ROMs.  And a good way to identify similar roms is
 * by searching for shared resources.
 */

#ifndef _CARTRIDGE_FILE_H
#define _CARTRIDGE_FILE_H

#include <stdint.h>
#include "cartridge.h"


// Our storage records
typedef struct nds_cartridge_file_s
{
    unsigned int FileID;
    unsigned int DirectoryID;
    char * FileName;
    char * FullFileName;
    unsigned int FileSize;
    SHA512_HASH * FileHash;
} nds_cartridge_file_t;
typedef struct nds_cartridge_dir_s
{
    unsigned int DirectoryID;
    unsigned int ParentID;
    char * DirectoryName;
    char * FullDirectoryName;
} nds_cartridge_dir_t;
typedef struct nds_cartridge_filetable_s
{
    unsigned int NumFiles;
    unsigned int NumDirectories;
    nds_cartridge_file_t * Files;
    nds_cartridge_dir_t * Directories;
    char * Messages; // Messages are nonfatal issues such as a lying FNT entry.
} nds_cartridge_filetable_t;


// Procs
void create_filetable(const nds_cartridge_t * cart, nds_cartridge_filetable_t * out_table);
nds_cartridge_filetable_t * load_filetable(const nds_cartridge_t * cart);
void clear_filetable(nds_cartridge_filetable_t * table);
void free_filetable(nds_cartridge_filetable_t * table);

int validate_cartridge_filetable(const nds_cartridge_t * cart);

#endif
