#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>
#include <string.h>
#include "gather.h"

// for error handling
#include <errno.h>

#define FILE_TYPE 1
#define DIRECTORY_TYPE 0
#define NEITHER_TYPE -1

#define MKDIR_MAGIC_NUMBER 0700

typedef struct dirent *dirContentPtr;

// to forge the path to target
void forgePath (relativePath rPath, relativePath auxPath, int start, int end);

// actual recursive traversal function
static void actualTraversal (char *target, char *dest, int *level, int mode);

// copy to destination function
static void copyToDestination (char *target, char *dest, dirContentPtr directoryIterator);

// copy directory (make directory).
// pass folderName as "\0" to create a folder with path = dest
static void createFolder (char *dest, fileName folderName);

static int isFileOrFolder (char *pathFromRootToSuspect);
static int assertNullTerminance (char* suspect);

// error report functions
static void opendirErrorReport (DIR *targetDirectory);
static void closeDirErrorReport (int closeDirRetcode);

void traverse (char *target, char *dest, int mode) {
    printf ("PULL order initialized\n");
    
    // temp buffer
    relativePath auxPath;
    forgePath (dest, auxPath, 0, strlen (dest));
    createFolder (dest, "\0");
    int level = ROOT_LEVEL;
    actualTraversal (target, dest, &level, mode);
    
    return;
}

static void actualTraversal (char *target, char *dest, int *level, int mode) {
    assert (assertNullTerminance (target));
    assert (strlen (target) <= MAX_PATH_LENGHT);
    assert (assertNullTerminance (dest));
    assert (strlen (dest) <= MAX_PATH_LENGHT);
    assert (mode == 0 || mode == 1);

    DIR *targetDirectory;
    dirContentPtr directoryIterator;

    // on error returns NULL setting appropriate errno.
    targetDirectory = opendir (target);
    opendirErrorReport (targetDirectory);
    
    if (targetDirectory == NULL) {
        if (errno == EACCES) {
            int tabCount = 0;
            while (tabCount < *level) {
                printf ("\t");
                tabCount ++;
            }
            printf ("Permission denied.\nMoving on....\n");
        }
    } else {
        // maybe 2 reads will be required to get to the actual list of contents.
        directoryIterator = readdir (targetDirectory);
        directoryIterator = readdir (targetDirectory);
        directoryIterator = readdir (targetDirectory);
        while (directoryIterator != NULL) {
        
            int tabCount = 0;
            while (tabCount < *level) {
                printf ("\t");
                tabCount ++;
            }

            // if this thing pointed by iterator is a file access the file.
            // else recurse for this sub directory.
            int oldTerminalTargetIndex = strlen (target);
            int oldDestTerminalIndex = strlen (dest);
            target = strcat (target, "/");
            target = strcat (target, directoryIterator->d_name);
            int type = isFileOrFolder (target);
            if (type == FILE_TYPE) {
                printf ("%s\n", directoryIterator->d_name);
                // append to dest the name of file..create open write close and disjoint the file name to get the original destination.
                copyToDestination (target, dest, directoryIterator);                
            } else if (type == DIRECTORY_TYPE) {
                printf ("%s\n", directoryIterator->d_name);
                // append to path buffer and traverse
                
                if (mode == 1) {
                    fileName folderName;
                    int folderNameLength = strlen (directoryIterator->d_name);
                    assert (folderNameLength <= MAX_FILE_NAME_LENGTH);
                    strcpy (folderName, directoryIterator->d_name);
                    assert (folderName[folderNameLength] == '\0');
                    puts (folderName);
                    createFolder (dest, folderName);
                }

                (*level) += 1;
                actualTraversal (target, dest, level, mode);
                (*level) -= 1;
            }
            // backtrack
            *(target + oldTerminalTargetIndex) = '\0';
            *(dest + oldDestTerminalIndex) = '\0';
            directoryIterator = readdir (targetDirectory);
        }

        // return 0 on success and -1 on error setting appropriate errno.
        int closeDirRetcode = closedir (targetDirectory);
        closeDirErrorReport (closeDirRetcode);
        assert (closeDirRetcode == 0);
    }
    return;
}

static void copyToDestination (char *target, char *dest, dirContentPtr directoryIterator) {
    FILE *currentSrcFilePtr = fopen (target, "rb");

    int oldTerminalDestIndex = strlen (dest);
    dest = strcat (dest, "/");
    dest = strcat (dest, directoryIterator->d_name);
    FILE *currentDestFilePtr = fopen (dest, "wb");
    assert (currentDestFilePtr != NULL);

    int cypherChar = getc (currentSrcFilePtr);
    while (cypherChar != EOF) {
        putc (cypherChar, currentDestFilePtr);
        cypherChar = getc (currentSrcFilePtr);
    }
    assert (cypherChar == EOF);

    int fcloseRetcode;
    fcloseRetcode = fclose (currentSrcFilePtr);
    assert (fcloseRetcode == 0);
    fcloseRetcode = fclose (currentDestFilePtr);
    assert (fcloseRetcode == 0);

    dest[oldTerminalDestIndex] = '\0';

    return;
}

static void createFolder (char *dest, fileName folderName) {
    // ignores the call if specified folder already exists
    if (folderName[0] == '\0') {

    } else {
        dest = strcat (dest, "/");
        dest = strcat (dest, folderName);
    }
    mkdir (dest, MKDIR_MAGIC_NUMBER);
    return;
}

static void opendirErrorReport (DIR *targetDirectory) {
    if (targetDirectory == NULL) {
        printf ("\n%d\n", errno);
        printf ("\tcheck errno for opendir\n");
    }
    return;
}

static void closeDirErrorReport (int closeDirRetcode) {
    if (closeDirRetcode == -1) {
        printf ("\tcheck errno for closedir\n");
    }
    return;
}

static int isFileOrFolder (char *pathFromRootToSuspect) {
    int type;
    int status;
    struct stat statusBuffer;
    status = stat (pathFromRootToSuspect, &statusBuffer);

    // to prevent gcc from blowing "status declared but unused"
    status ++;

    if (S_ISDIR (statusBuffer.st_mode)) {
        type = DIRECTORY_TYPE;
    } else if (S_ISREG (statusBuffer.st_mode)) {
        type = FILE_TYPE;
    } else {
        type = NEITHER_TYPE;
    }
    return type;
}

static int assertNullTerminance (char* suspect) {
    int nullTerminance;
    if (*(suspect + strlen (suspect)) == '\0') {
        nullTerminance = TRUE;
    } else {
        nullTerminance = FALSE;
    }
    return nullTerminance;
}

void forgePath (relativePath rPath, relativePath auxPath, int start, int end) {
    if (start > end) {
        puts ("BYEEEE!");
    } else {
        int i = start;
        char cChar = rPath[start];
        while (i < end && cChar != '/' && cChar != '\0') {
            auxPath[i] = cChar;
            i ++;
            cChar = rPath[i];
        }
        auxPath[i] = '\0';

        printf ("working working working on: %s\n", auxPath);
        int retCode = mkdir (auxPath, MKDIR_MAGIC_NUMBER);
        printf ("\tLog:%s\n", strerror (errno));
        auxPath[i] = '/';
        start = i + 1;
        forgePath (rPath, auxPath, start, end);
    }
    return;
}

/*static void makeShiftTester () {
    testIsFileOrFolder ();
    testAssertNullTerminance ();
    printf ("***ALL passed!!!\n");
    return;
}

static void testAssertNullTerminance () {
    printf ("testing assertNullTerminance");

    int nulllTerminance = assertNullTerminance ("suspect");
    assert (nulllTerminance == TRUE);

    nulllTerminance = assertNullTerminance (".");
    assert (nulllTerminance == TRUE);

    DIR *sampleDirectory;
    dirContentPtr directoryIterator;
    sampleDirectory = opendir (".");
    directoryIterator = readdir (sampleDirectory);
    directoryIterator = readdir (sampleDirectory);
    directoryIterator = readdir (sampleDirectory);

    printf ("The name of first item is %s\n",directoryIterator->d_name);
    nulllTerminance = assertNullTerminance (directoryIterator->d_name);
    assert (nulllTerminance == TRUE);
    return;
}

static void testIsFileOrFolder () {
    printf ("Testing isFileOrFolder ()\n");
    int type = isFileOrFolder ("testDir");
    assert (type == DIRECTORY_TYPE);
    type = isFileOrFolder ("gather.c");
    assert (type == FILE_TYPE);
    return;
}
*/