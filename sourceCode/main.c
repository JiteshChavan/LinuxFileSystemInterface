#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "gather.h"

int main (int argc, char *argv[]) {
    printf (">Initializing order: PULL\n");

    relativePath src;
    relativePath dest;
    
    printf ("Mode[0/1]?\n");
    printf ("0: copy all leaf files to to destination\n1: standard copy paste\n");
    int mode;
    int retCode = scanf ("%d", &mode);
    assert (retCode == 1);

    printf ("\tSource Path:\n\t");
    retCode = scanf ("%s",src);
    assert (retCode == 1);
    printf ("\tDestination Path:\n\t");
    retCode = scanf ("%s", dest);
    assert (retCode == 1);
    printf ("\n");

    traverse (src, dest, mode);

    return EXIT_SUCCESS;
}