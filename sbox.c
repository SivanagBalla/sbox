/*
 * S-Box Utiliites (SivanagBalla@gmail.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>     /* basename() */
#include <errno.h>

#include "sbox.h"
#include "util.h"

int debug_en = 1;

typedef struct {
    char utilName[MAX_UTIL_NAME];
    int (*utilMain)(int argc, char* arg[]);
    char desc[256];
} utils_t;

utils_t utils[] = {
    { "symlinks",   symlinks_main , ""},
    { "memop",      memop_main    , ""},
    { "memeater",   memeater_main , ""},
    { "hide_lltest",     lltest_main ,   ""},
    { "hide_util",       util_main ,     ""}
};
int utilCount = sizeof(utils)/sizeof(utils_t);

void showhelp() {
    int i;
    printf("S-Box Utilities\n");
    for (i=0; i < utilCount; i++) {
        if (strncmp("hide_", utils[i].utilName, 5))
            printf("  %s\n", utils[i].utilName);
    }
}

int symlinks_main(int argc, char* argv[]) {
    int i;
    for (i=0; i < utilCount; i++){
        if (-1 == symlink("sbox", utils[i].utilName)) {
            printf("Error: Symlink failed for '%s' (%d - %s)\n",
                    utils[i].utilName, errno, strerror(errno));
        }
    }
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {

    char *progname;
    int i;

    progname = basename(argv[0]);
    printf("%s\n", progname);
    if (!strncmp(progname, "sbox", sizeof("sbox"))) {
        if (argc == 1) {
            showhelp();
            return EXIT_SUCCESS;
        }

        /* Consume the first arg */
        argc -= 1;
        argv = &argv[1];
    }

    /* Run the utility */
    for (i=0; i < utilCount; i++) {
        if (!strncmp(argv[0], utils[i].utilName, MAX_UTIL_NAME))
            return utils[i].utilMain(argc, argv);
    }
    printf("Utility not found: %s\n", argv[0]);
    return EXIT_FAILURE;
}
