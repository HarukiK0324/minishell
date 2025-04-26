#include <dirent.h>
#include "apue.h"

int
main(int argc, char *argv[])
{
    DIR *dp;
    struct dirent *dirp;

    if (argc != 2)
        return(perror("usage: ls directory_name"),1);
    if ((dp = opendir(argv[1])) == NULL)
        printf("can’t open %s", argv[1]);
    while ((dirp = readdir(dp)) != NULL)
        printf("%s        ", dirp->d_name);
    closedir(dp);
    exit(0);
}