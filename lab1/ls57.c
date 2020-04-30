#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>


int main(int argc, char **argv) {
    if (argc > 2) {
        fprintf(stderr, "Error en la cantidad de argumentos: %m\n");
        return 1;
    }

    char *dir_name = argv[1] != NULL ? argv[1] : ".";
    DIR *dir = opendir(dir_name);

    if (errno == ENOTDIR) {
        fprintf(stderr, "Solo se pueden listar directorios\n");
        return 1;
    }

    struct dirent *dp;

    while (dp = readdir(dir)) {
        if (errno != 0) {
            fprintf(stderr, "Error al leer el directorio %s: %m\n", dir_name);
            return 1;
        }

        int file_descriptor = dirfd(dir);

        // Ignorar ocultos
        if (dp->d_name[0] != '.') {
            struct stat f_stat;
            int resultado = fstatat(file_descriptor, dp->d_name, &f_stat, 0);
            if (fstatat(file_descriptor, dp->d_name, &f_stat, 0) != 0) {
                fprintf(stderr, "Error al abrir el archivo");
                return 1;
            }
            if (S_ISREG(f_stat.st_mode)) {
                printf(" %s\t", dp->d_name);
                printf(" %ld\n", f_stat.st_size);
            } else {
                printf(" %s\n", dp->d_name);
            }
        }
    }

    closedir(dir);
    return 0;
}