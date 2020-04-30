#include "cp57.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int fdin, fdout;
    size_t bytes;
    uint8_t buf[512];

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <filein> <fileout>\n", argv[0]);
        return 2;
    }

    if ((fdin = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "No se pudo abrir '%s': %m\n", argv[1]);
        return 2;
    }

    if ((fdout = creat(argv[2], 0666)) < 0) {
        fprintf(stderr, "No se pudo abrir '%s': %m\n", argv[1]);
        close(fdin);
        return 2;
    }

    errno = 0;  /* Para detectar el caso en que copyfd() devuelve 0
                 * por no haber podido copiar nada. /o\ */
    int ret = 0;

    if ((bytes = copyfd(fdin, fdout, buf, sizeof buf)) > 0 || errno != 0) {
        ret = 1;
        fprintf(stderr, "Solo se pudo copiar %zu bytes (%m)\n", bytes);
    }

    close(fdin);
    close(fdout);

    return ret;
}
