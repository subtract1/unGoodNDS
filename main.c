#include <dirent.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cartridge.h"
#include "libraries/sds/sds.h"

// decs
void checkdir(const char *);

int main()
{
    //Find each file.  And analyze it.
    checkdir("./roms");
    checkdir(".");

    return 0;
}

void checkdir(const char * dirname)
{
    // Open the directory
    DIR * dp = opendir(dirname);
    if (dp == NULL)
        return;

    // Start scanning the directory
    while (1)
    {
        // Get an entry...
        struct dirent * entry = readdir(dp);
        if (entry == NULL)
            break;

        // Is it an nds rom?
        if (strncmp(entry->d_name + strlen(entry->d_name) - 4, ".nds", 4) == 0)
        {
            // Open it and print out some useful info.
            char filename[1024];
            FILE *fp;

            sprintf(filename, "%s/%s", dirname, entry->d_name);
            fp = fopen(filename, "rb");
            if (fp != NULL)
            {
                printf("Processing file %s...\n", filename);

                nds_cartridge_t * cart = create_nds_cartridge(fp);
                char * info = cartridge_info(cart);
                printf("%s", info);
                free(info);

                free_nds_cartridge(cart);
            }
        }
    }

    // Cleanup
    closedir(dp);
}
