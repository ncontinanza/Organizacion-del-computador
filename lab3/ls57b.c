#include "ls57.h"

#include <stdio.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
    DIR *dir;
    char *dirname = (argc >= 2) ? argv[1] : ".";

    if ((dir = opendir(dirname)) != NULL) {
        lsdir(dir);
    } else {
        fprintf(stderr, "Error al abrir '%s': %m", dirname);
        return 1;
    }
    closedir(dir);
}
