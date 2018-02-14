/**
 * memop.c 
 * Written by Sivanag Balla
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "util.h"

/* Definitions  */
/* #define DEV_MEM "/dev/mem" */
#define DEV_MEM "dd.bin"

/* Globals */
uint64_t cols = 0;

/**
 * usage ()
 */
void usage(void) {
    printf(" Usage:         \n");
}

typedef struct {
    uint64_t offset;
    uint64_t len;
} output_opt;

#define BIT_8  8
#define BIT_16 16
#define BIT_32 32
#define BIT_64 64

#define GETBIT(val, ind) ((int)((val >> ind) & 0x1))

// TODO overflow checks
char* getBinaryStr(uint64_t val, int bitWidth, char *buf) {
    char opt_nib='_';
    char opt_byte=' ';
    int i = 0;
    for(i=0, bitWidth-=1 ; bitWidth >=0 ; bitWidth--) {
        buf[i++] = GETBIT(val, bitWidth) ? '1' : '0';

        if (bitWidth%8 == 0)
            buf[i++] = opt_byte;
        else if ( bitWidth%4 == 0 )
            buf[i++] = opt_nib;
    }
    buf[i] = '\0';
    return buf;
}

int getTermWidth (void) {
    uint64_t lcols;
    if (cols) 
        return (int)cols;
    
    char *colsStr = getenv("COLUMNS");
    if (colsStr == NULL) {
        dprintf("TermWidth = 80");
        return 80;
    }
    dprintf("TermWidth = %s\n", colsStr);
    parseNumericArg(colsStr, &cols);
    return (int)cols;
}

void hexdump(char *vptr, uint64_t  offset, uint64_t len, int op_type) {

#define SEPERATOR_WIDTH 2
#define MAX_COLS 1024
    uint64_t ind, i, j;

    char *sptr = vptr;
    char *eptr = vptr + len;

    char hline[MAX_COLS];
    char bline[MAX_COLS];
    char cline[MAX_COLS];
    char sline[MAX_COLS];

    char binstr[256];

    switch (op_type) {
        case 'b': op_type = 1; break;
        case 'w': op_type = 2; break;
        case 'd': op_type = 4; break;
        case 'q': op_type = 8; break;
    }
    int tail_bytes = len % op_type;
    if( tail_bytes != 0 )
        len -= tail_bytes;

    int mi = len/op_type;

    int print_binary = 0;

    int cols = getTermWidth() ;
    int huge_offset = (offset > 0xFFFFFFFFUL);
    int offset_cols = huge_offset ? 16 : 8;

    int cols_left = cols 
                        - offset_cols
                        - SEPERATOR_WIDTH
                        - (print_binary ? SEPERATOR_WIDTH : 0)
                        - (op_type == 1 ? SEPERATOR_WIDTH : 0);

    // Each item requries this many cols including a space
    int hex_cols = op_type * 2 + 1 + (op_type == 8 ? 1 : 0);
    int bin_cols = print_binary ? (op_type * 10) : 0;
    int char_cols = 1;

    int item_cols = hex_cols + bin_cols + char_cols;

    //We can fix a maximum of these many items
    int mipl = cols_left/item_cols;
    if (mipl < 1)
        mipl = 1;
    else if (op_type == 1 && print_binary && mipl >= 4)
        mipl = 4;
    else if (op_type == 2 && print_binary && mipl >= 2)
        mipl = 2;


    //But item count should be powers of 2 going upto max 16 items
    for(i = 0; mipl; i++, mipl >>= 1 );
    mipl = (i > 4) ? 16 : (1 << (i-1) );

    dprintf("max items = %d\n", mipl);

    int hex_width = mipl * hex_cols + ((mipl>=8) ? 1 : 0);  
    int bin_width = mipl * bin_cols + ((mipl>=8) ? 1 : 0);   
    int char_width = mipl + 1 +((mipl>=8) ? 1 : 0);   

    int hex_loc  = offset_cols + SEPERATOR_WIDTH ; 
    int bin_loc  = hex_loc + hex_width + SEPERATOR_WIDTH;
    int char_loc = print_binary ? (bin_loc + bin_width + SEPERATOR_WIDTH ) 
                    : bin_loc ;
    int null_loc = offset_cols + SEPERATOR_WIDTH
                    + hex_width  + SEPERATOR_WIDTH
                    + (print_binary ? bin_width + SEPERATOR_WIDTH : 0) 
                    + (op_type == 1 ? char_width + SEPERATOR_WIDTH :0)
                    - 1;
 
    dprintf("hex_loc = %d\n", hex_loc);
    dprintf("bin_loc = %d\n", bin_loc);
    dprintf("char_loc = %d\n", char_loc);
    dprintf("null_loc = %d\n", null_loc);

    int hoff = 0;
    int boff = 0;
    int coff = 0;

    uint8_t val8;
    uint16_t val16;
    uint32_t val32;
    uint64_t val64;

    for(i = 0; i < mi; ) {
        // init line
        memset(sline, ' ', null_loc);

        hoff = hex_loc;
        boff = bin_loc;
        coff = char_loc;

        if (huge_offset)
            sprintf(sline, "%016llx: ", offset + (i*op_type));
        else
            sprintf(sline, "%08llx: ", offset + (i*op_type));

        // Cache the values
        for (j = 0; j < mipl && i < mi; i++, j++) {
            if (j == 8) {
                hoff++;
                boff++;
                coff++;
            }
            switch (op_type) {
                case 1 : 
                    val8 = (uint8_t )*(uint8_t*  )(vptr + (i*op_type)); 
                    hoff += sprintf(sline + hoff, "%02x ", val8);
                    if (print_binary) {
                        boff += sprintf(sline + boff, "%s", 
                                getBinaryStr(val8, 8, binstr));
                    }
                    if( val8 > 0x20 && val8 < 0x1F )
                        sline[coff] = val8;
                    else
                        sline[coff] = '.';
                    coff++;
                    break;

                case 2 : 
                    val16 = (uint16_t )*(uint16_t*  )(vptr + (i*op_type)); 
                    hoff += sprintf(sline + hoff, "%04x ", val16);
                    if (print_binary) {
                        boff += sprintf(sline + boff, "%s", 
                                getBinaryStr(val16, 16, binstr));
                    }
                    break;

                case 4 : 
                    val32 = (uint32_t )*(uint32_t*  )(vptr + (i*op_type)); 
                    hoff += sprintf(sline + hoff, "%08x ", val32);
                    if (print_binary) {
                        boff += sprintf(sline + boff, "%s", 
                                getBinaryStr(val32, 32, binstr));
                    }
                    break;

                case 8 : 
                    val64 = (uint64_t )*(uint64_t*  )(vptr + (i*op_type)); 
                    hoff += sprintf(sline + hoff, "%08x_%08x ", 
                                            (uint32_t)(val64 >> 32),
                                            (uint32_t)(val64 & 0xFFFFFFFF));
                    if (print_binary) {

                        boff += sprintf(sline + boff, "%s", 
                                getBinaryStr(val64, 64, binstr));
                    }
                    break;
            }
        }

        j = 0;
        for ( j = 0; j < null_loc; j++ )
            if (sline[j] == '\0')
                sline[j] = ' ';
        sline[null_loc-1] = '|';
        sline[null_loc-0] = '\0';
        if (print_binary)
            sline[bin_loc-2] = '|';
        if (op_type == 1)
            sline[char_loc-2] = '|';

        printf("%s\n", sline);
    }
    if (offset % op_type != 0) 
        printf("Warning: offset is not %d byte aligned\n", op_type);

    if (tail_bytes != 0) {
        printf("Warning: length is not multiple of %d bytes. Left over %d "\
                    "byte(s) ...\n", op_type, tail_bytes);
        int i = 0;
        printf("%08llx: ", offset + len);
        for (sptr = vptr+len;  i < tail_bytes; i++) 
            printf("%02x ", (uint8_t)*(uint8_t*)(sptr+i) );
        printf("\n");
        
    }
}

/* main() */
int memop_main(int argc, char **argv) {
    int argerr = 0;
    char opt; 
    int fd = 0;
    char filename[256] = "/dev/mem";
    extern char *optarg;
    extern int optind, optopt;
    uint64_t offset=0;
    uint64_t len=0;
    int type = 'd';

    /* parse the arguments */
    while ((opt = getopt(argc, argv, "c:vf:s:o:l:t:")) != -1) {
        switch(opt) {
            case 'v':
                debug_en = 1;
                break;

            case 'f':
                strncpy(filename, optarg, 256);
                break;

            case 'o':
            case 's':
                if (parseNumericArg(optarg, &offset)) {
                    printf("Unknown format for -s\n");
                    argerr++;
                }
                break;

            case 'l':
                if (parseNumericArg(optarg, &len)) {
                    printf("Unknown format for -l\n");
                    argerr++;
                }
                break;

            case 'c':
                if (parseNumericArg(optarg, &cols)) {
                    printf("Unknown format for -c\n");
                    argerr++;
                }
                break;

            case 't':
                if (optarg[0] != 'b' && optarg[0] != 'w' && optarg[0] != 'd' &&
                        optarg[0] != 'q') {
                    printf(" --%s--", optarg);
                    printf("Type should be one of (b)yte, (w)ord,"\
                            " (d)doubleword, (q)quadword\n");
                    argerr++;
                    break;
                }
                type = optarg[0];
                break;

            case '?':
                printf("Unknown option: -%c\n", optopt);
                argerr++;
                break;

            case ':':
                printf("Option -%c requires an operand\n", optopt);
                argerr++;
                break;

            default:
                printf("Unknown arg");
                break;
        }
    }
    if (argerr) {
        usage();
        exit(EXIT_FAILURE);
    }
    dprintf("file : %s\n", filename);
    dprintf("offset : %llu\n", offset);

    struct stat sb = {0};
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Error: Failed to open '%s': %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }
    dprintf("File opened\n");

    if (strncmp(filename, "/dev", 4)) {
        if (fstat(fd, &sb) == -1)
            printf("Warning: Unable to get file size\n");

        dprintf("Filesize: %llu\n", sb.st_size);
        if (offset >= sb.st_size) {
            printf("Offset is past end of file\n");
            exit(EXIT_FAILURE);
        }
    }

    int64_t page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1) {
        printf("Error: Failed to get pagesize: %s\n", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }
    dprintf("System page size: %lld\n", page_size);

    uint64_t pa_offset = offset & (~(page_size-1));
    uint64_t poi = (offset-pa_offset);
    dprintf("Page offset = %llu\n", pa_offset);
    dprintf("Page offset inside= %llu\n", poi);
    dprintf("Map length= %llu\n", len);

    void *vptr = mmap(NULL, poi+len, PROT_READ, MAP_SHARED, 
                                fd, pa_offset);
    if (vptr == MAP_FAILED){
        printf("Error: mmap failed: %s\n", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }
    dprintf("vaddr: %p\n", vptr);

    hexdump( (char*)vptr + poi, offset, len, type);

    munmap(vptr, poi+len) ;
    close(fd);

    dprintf("Exiting ... \n");
    exit(EXIT_SUCCESS);
}
