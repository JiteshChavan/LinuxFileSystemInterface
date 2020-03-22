// gather.h
//
// created by Jaskier on 26/12/2019.
//
//
#define TRUE 1
#define FALSE 0
#define MAX_FILE_NAME_LENGTH 255
#define MAX_PATH_LENGHT 4096

// path traversal definitions
#define ROOT "."
#define ACCESS_PREFIX "./"
#define PREVIOUS_LEVEL_RELATIVE ".."

//levels
#define ROOT_LEVEL 0
#define PRE_TARGET_LEVEL -1

typedef char fileName [MAX_FILE_NAME_LENGTH];
typedef char relativePath [MAX_PATH_LENGHT];

// traverses the target directory accessing all the sub directories recursively along with their content files.
// note that both 'dest' and 'target' are setup on mainframe size 4097 and are recursively used
void traverse (char *target, char *dest, int mode);