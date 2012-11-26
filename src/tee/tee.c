/*
 * MIT license
 * Thu Nov 22 14:27:24 EST 2012
 */

#include "../shared.h"
#include "unistd.h"

#ifndef VERSION
    #define VERSION      "unknown"
#endif

#ifndef COMPILE_DATE
    #define COMPILE_DATE "unknown"
#endif

#define LICENSE      "MIT"

FILE **g_files    = NULL;
char **g_fileName = NULL;
int    g_numFiles = 0;

const char g_writeOverwrite[] = "w";
const char g_writeAppend[]    = "a";
char const * g_writeMode = g_writeOverwrite;

/*helper functions*/

int OpenFiles()
{
    /* go through all the filenames in "g_fileName" and create FILE
     * structures for writing to*/

    /* first allocate that shit */
    g_files = (FILE**)malloc(sizeof(FILE**) * g_numFiles);

    /* now open shit up */
    int i;
    for (i = 0; i < g_numFiles; ++i) {

        /* check for stdout first */
        if (!strcmp(g_fileName[i], "-")) {
            g_files[i] = stdout;
        }

        /* it's just a plain old file */
        else {
            g_files[i] = fopen(g_fileName[i], g_writeMode);
            if (!g_files[i]) {
                fprintf(stderr, "failed to open %s\n", g_fileName[i]);
                return 1;
            }
        }
    }

    return 0;
}

int AddFile(char const * str)
{
    ++g_numFiles;
    g_fileName =  (char**)realloc(g_fileName, g_numFiles * sizeof(char**));

    int index = g_numFiles-1;
    int len   = strlen(str);

    /* allocate string and copy over "str" (NULL byte included */
    g_fileName[index] = (char*)malloc(len+1);
    memcpy(g_fileName[index], str, len+1);

    return 0;
}

void CloseFiles(void)
{
    int i;
    for (i = 0; i < g_numFiles; ++i)
    {
        if (g_files[i] != stdout) fclose(g_files[i]);
        if (g_fileName[i])        free(g_fileName[i]);
    }
        
    free(g_files);
    free(g_fileName);
}

/*param handlers*/

/*ingore interrupt signals*/
void set_ignore(char const * const *str, int argn)
{
    /*todo lel*/   
}

void set_append(char const * const *str, int argn)
{
    g_writeMode = g_writeAppend;
}

void add_file(char const * const *str, int argn)
{
    AddFile(str[argn]);
}

/*hur dur*/
void print_usage(char const * const *str, int argn)
{
    printf("Usage: %s [OPTION]... [FILE]...\n", str[0]);
    printf("Copy standard input to each FILE, and also to standard output.\n");

    printf("  -a, --apend               append to the given FILEs, do not overwrite\n");
    printf("  -i, --ignore-interrupts   ignore unterrupt signals\n");

    printf("      --help     display this help and exit\n");
    printf("      --version  output version information and exit\n\n");
    exit(0);
}

/*hur dur*/
void print_version(char const * const *str, int argn)
{
    printf("%s version: "VERSION" (gentils git)\n", str[0]);
    printf("Build date: "COMPILE_TIME"\n");
    printf("License "LICENSE"\n");
    exit(0);
}

/* 
 * The following struct is responsible
 * for handling command line arguments
 * in a simple way (linear search)
 */

typedef struct {
    char const *name;
    void (*function)(char const * const * str, int argn);
} ArgList;

ArgList ArgInfo[]= {
        { .name="--version", .function=&print_version },
        { .name="--help",    .function=&print_usage   },

        { .name="-a",        .function=&set_append    },
        { .name="--append",  .function=&set_append    },

        { .name="-i",        .function=&set_ignore    },
        { .name="--ignore",  .function=&set_ignore    },

        { .name="",          .function=&add_file      },

        { .name=NULL,        .function=0              }
};

void handle_arguments(char const * const *argv)
{
    char const * const *arg = argv;
    int argn = 0, i;
    while (*(++arg))
    {
        ++argn;

        /* check for parameters */
        for (i = 0; ArgInfo[i].function; ++i)
        {
            char const * const name = ArgInfo[i].name;

            if (!strncmp(*arg, name, strlen(name)))
            {
                /* call the function with all agruments
                 * and integer denoting the current argument*/
                ArgInfo[i].function(argv, argn);
                break;
            }
        }

    }
}

int main(int argc, char const * const *argv)
{
    if (argc > 1)
        handle_arguments(argv);

    /*enable stdout by default*/
    AddFile("-");
    OpenFiles();
    
    char buf[512];
    memset(buf, 0, 512);

    while (!feof(stdin))
    {
        fgets(buf, 512, stdin);

        int i;
        for (i = 0; i < g_numFiles; ++i) {
            fprintf(g_files[i], "%s", buf);
        }
    }

    CloseFiles();
    return 0;
}
