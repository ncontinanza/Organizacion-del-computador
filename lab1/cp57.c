#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Error en la cantidad de argumentos: %m\n");
        return 1;
    }

    int source = open(argv[1], O_RDONLY);

    if (source < 0) {
        fprintf(stderr, "Error al abrir %s: %m\n", argv[1]);
        return 1;
    }

    int dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

    if (dest < 0) {
        fprintf(stderr, "Error al abrir %s: %m\n", argv[2]);
        return 1;
    }

    uint8_t buf[512];

    while (true) {
        ssize_t nread = read(source, buf, sizeof(buf));
        ssize_t nwrite = 0;

        if (errno == EINTR){
            nread = read(source, buf, sizeof(buf));
        }

        if (errno == EISDIR){
            fprintf(stderr,"Error: %s es un directorio\n", argv[1]);
            return 1;
        }

        if (nread < 0){
            fprintf(stderr,"Hubo un error leyendo el archivo %s\n", argv[1]);
            return 1;
        }

        if (nread == 0){
            fprintf(stderr,"Finalizó la copia del archivo exitosamente\n");
            return 0;
        }

        do {
            ssize_t r = write(dest, buf, nread);

            if (errno == EINTR){
                r = write(dest, buf, nread);
            }

            if (errno == EISDIR){
                fprintf(stderr,"Error: %s es un directorio\n", argv[2]);
                return 1;
            }

            if (r < 0){
                fprintf(stderr,"Hubo un error leyendo el archivo %s\n", argv[2]);
                return 1;
            }

            nwrite += r;

        } while (nread != nwrite);
    }

    close(source);
    close(dest);
    return 0;
}