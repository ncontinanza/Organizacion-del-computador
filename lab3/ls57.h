#include <dirent.h>
#include <sys/types.h>

/*
 * getsize() devuelve (usando fstatat) el tamaño de un archivo
 * que se ubica en el directorio asociado al file descriptor
 * "atdirfd". Si el archivo no es un archivo regular, se devuelve
 * -1.
 *
 * Nota: en Linux de 64 bits se observan los siguientes tamaños:
 *
 *      sizeof(struct stat) = 144
 *      st_mode = 4 bytes con offset 24
 *      st_size = 8 bytes con offset 48
 *      S_IFREG = 0x8000
 *
 * (Ver al final de este archivo código de ejemplo para calcular
 * estos valores.)
 *
 * Precondiciones: atdirfd >= 0, filename != NULL.
 */
ssize_t getsize(int atdirfd, const char *filename);


/*
 * lsdir() muestra los contenidos de un directorio, incluyendo
 * los tamaños de archivos regulares (usando getsize).
 *
 * Nota: en Linux de 64 bits, el offset de d_name es 19.
 *
 * Nota 2: como formato de salida, incluir en la sección
 * .data las siguientes directivas:
 *
 *   fmt_reg:
 *      .asciz "%s\t%zu\n"  // Recibe d_name, size
 *   fmt_nreg:
 *      .asciz "%s\n"       // Recibe solo d_name
 *
 * Nota 3: buscar información sobre el uso de %al en las
 *         llamadas a printf(3); no es posible una
 *         invocación correcta sin asignarle algún valor. 
 *
 * Precondiciones: dir != NULL.
 */
void lsdir(DIR *dir);


/*
 * Se incluye a continuación, como referencia, el programa usado
 * para mostrar los tamaños de structs stat y dirent (información
 * ya proporcionada arriba).
 */
#if 0
#include <dirent.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>

int main(void) {
    struct stat st;
    struct dirent e;

    printf("struct stat = %zu bytes\n", sizeof(st));

    printf("st_mode = %zu bytes at offset %zu\n",
           sizeof(st.st_mode),
           (uintptr_t) &st.st_mode - (uintptr_t) &st);

    printf("st_size = %zu bytes at offset %zu\n",
           sizeof(st.st_size),
           (uintptr_t) &st.st_size - (uintptr_t) &st);

    printf("S_IFMT = %#x, S_IFREG = %#x\n\n", S_IFMT, S_IFREG);

    printf("struct dirent = %zu bytes\n", sizeof(e));

    printf("d_name = %zu bytes at offset %zu\n",
           sizeof(e.d_name),
           (uintptr_t) &e.d_name - (uintptr_t) &e);
}
#endif
