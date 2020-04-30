#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

enum encoding {
    UTF8, UTF16BE, UTF16LE, UTF32BE, UTF32LE
};

/*
 * Devuelve el encoding correspondiente al argumento.
 */
static enum encoding str_to_encoding(const char *enc) {
    if (strcmp(enc, "UTF-8") == 0)
        return UTF8;
    else if (strcmp(enc, "UTF-16BE") == 0)
        return UTF16BE;
    else if (strcmp(enc, "UTF-16LE") == 0)
        return UTF16LE;
    else if (strcmp(enc, "UTF-32BE") == 0)
        return UTF32BE;
    else if (strcmp(enc, "UTF-32LE") == 0)
        return UTF32LE;
    else
        return -1;
}

/*
 * Devuelve el encoding correspondiente al byte order mark (BOM).
 */
static enum encoding bom_to_encoding(uint8_t *bom) {
    if (bom[0] == 0xFE && bom[1] == 0xFF)
        return UTF16BE;
    else if (bom[0] == 0x00 && bom[1] == 0x00 && bom[2] == 0xFE && bom[3] == 0xFF)
        return UTF32BE;
    else if (bom[0] == 0xFF && bom[1] == 0xFE && bom[2] == 0x00 && bom[3] == 0x00)
        return UTF32LE;
    else if (bom[0] == 0xFF && bom[1] == 0xFE)
        return UTF16LE;
    // Si no es BOM válida, se asume UTF-8.
    return UTF8;
}

/*
 * Devuelve verdadero si hay un codepoint codificado en buf.
 */
static bool has_codepoint(enum encoding enc, uint8_t *buf, size_t n) {
    switch (enc) {
        case UTF32BE:
        case UTF32LE:
            return n >= 4;
        case UTF16BE:
            return (n >= 4 || ((n >= 2) && ((buf[0] < 0xD8) || (buf[0] > 0xDB))));
        case UTF16LE:
            return (n >= 4 || ((n >= 2) && ((buf[1] < 0xD8) || (buf[1] > 0xDB))));
        case UTF8:
            return (n >= 4 ||
                    (n >= 3 && buf[0] <= 0xEF) ||
                    (n >= 2 && buf[0] <= 0xDF) ||
                    (n >= 1 && buf[0] <= 0x7F));
        default:
            return false;
    }
}

/*
 * Funciones auxiliares para las transformaciones a UCS4
 */

uint32_t utf32le_to_ucs4(uint8_t *buf, size_t *nbytes, int *b_ptr) {
    uint32_t cp = 0;
    int b = *b_ptr;

    cp |= buf[b++];
    cp |= (buf[b++] << 8);
    cp |= (buf[b++] << 16);
    cp |= (buf[b++] << 24);

    *nbytes -= 4;
    *b_ptr = b;

    return cp;
}

uint32_t utf32be_to_ucs4(uint8_t *buf, size_t *nbytes, int *b_ptr) {
    uint32_t cp = 0;
    int b = *b_ptr;

    cp |= (buf[b++] << 24);
    cp |= (buf[b++] << 16);
    cp |= (buf[b++] << 8);
    cp |= buf[b++];

    *nbytes -= 4;
    *b_ptr = b;

    return cp;
}

uint32_t utf8_to_ucs4(uint8_t *buf, size_t *nbytes, int *b_ptr) {
    uint32_t cp = 0;
    int b = *b_ptr;

    if ((buf[b + 0] & 0x80) == 0) {
        cp = buf[b++] & 0x7F;

        *nbytes -= 1;
    } else if (((buf[b + 0] & 0xE0) == 0xC0) && ((buf[b + 1] & 0x80) == 0x80)) {
        cp |= (buf[b++] & 0x3F) << 6;
        cp |= (buf[b++] & 0x7F);

        *nbytes -= 2;
    } else if (((buf[b + 0] & 0xF0) == 0xE0) && ((buf[b + 1] & 0x80) == 0x80) &&
               ((buf[b + 2] & 0x80) == 0x80)) {
        cp |= (buf[b++] & 0x1F) << 12;
        cp |= (buf[b++] & 0x7F) << 6;
        cp |= (buf[b++] & 0x7F);

        *nbytes -= 3;
    } else if (((buf[b + 0] & 0xF8) == 0xF0) && ((buf[b + 1] & 0x80) == 0x80) &&
               ((buf[b + 2] & 0x80) == 0x80) && ((buf[b + 3] & 0x80) == 0x80)) {
        cp |= (buf[b++] & 0x07) << 18;
        cp |= (buf[b++] & 0x7F) << 12;
        cp |= (buf[b++] & 0x7F) << 6;
        cp |= (buf[b++] & 0x7F);

        *nbytes -= 4;
    }

    *b_ptr = b;

    return cp;
}

uint32_t utf16be_to_ucs4(uint8_t *buf, size_t *nbytes, int *b_ptr) {
    uint32_t cp = 0;
    int b = *b_ptr;

    if (((buf[b] >= 0xD8) && (buf[b] <= 0xDB) && (buf[b + 2] >= 0xDC) && (buf[b + 2] <= 0xDF))) {
        cp |= ((((buf[b] << 8) | (buf[b + 1])) - 0xD800) << 10);
        cp |= ((((buf[b + 2] << 8) | (buf[b + 3])) - 0xDC00));
        cp += 0x10000;

        b += 4;
        *nbytes -= 4;
    } else {
        cp |= buf[b++] << 8;
        cp |= buf[b++];

        *nbytes -= 2;
    }

    *b_ptr = b;

    return cp;
}

uint32_t utf16le_to_ucs4(uint8_t *buf, size_t *nbytes, int *b_ptr) {
    uint32_t cp = 0;
    int b = *b_ptr;

    if (((buf[b + 1] >= 0xD8) && (buf[b + 1] <= 0xDB) && (buf[b + 3] >= 0xDC) && (buf[b + 3] <= 0xDF))) {
        cp |= ((((buf[b]) | (buf[b + 1] << 8)) - 0xD800) << 10);
        cp |= ((((buf[b + 2]) | (buf[b + 3] << 8)) - 0xDC00));
        cp |= 0x10000;

        b += 4;
        *nbytes -= 4;
    } else {
        cp |= buf[b++];
        cp |= buf[b++] << 8;

        *nbytes -= 2;
    }

    *b_ptr = b;

    return cp;
}

/*
 * Transforma una codificación a UCS-4.
 *
 * Argumentos:
 *
 *   - enc: el encoding original.
 *   - buf: los bytes originales.
 *   - nbytes: número de bytes disponibles en buf.
 *   - destbuf: resultado de la conversión.
 *
 * Devuelve: el número de CODEPOINTS obtenidos (número
 *           de elementos escritos en destbuf).
 *
 * Actualiza: nbytes contiene el número de bytes que no se
 *            pudieron consumir (secuencia o surrogategate incompleto).
 *
 * Se debe garantiza que "destbuf" puede albergar al menos nbytes
 * elementos (caso enc=UTF-8, buf=ASCII).
 */
int orig_to_ucs4(enum encoding enc, uint8_t *buf, size_t *nbytes, uint32_t *destbuf) {
    int i = 0, b = 0;
    int *b_ptr = &b;

    // La función has_codepoint determina que se podrá decodificar
    // un codepoint en el encoding especificado. Así, adentro del
    // ciclo, en cada "case" se cumple la pre-condición que hay el
    // número adecuado de bytes presentes (no hace falta comprobarlo
    // de nuevo).
    while (has_codepoint(enc, &buf[b], *nbytes)) {
        uint32_t cp = 0;
        switch (enc) {
            case UTF32LE:
                cp = utf32le_to_ucs4(buf, nbytes, b_ptr);
                break;
            case UTF32BE:
                cp = utf32be_to_ucs4(buf, nbytes, b_ptr);
                break;
            case UTF8:
                cp = utf8_to_ucs4(buf, nbytes, b_ptr);
                break;
            case UTF16BE:
                cp = utf16be_to_ucs4(buf, nbytes, b_ptr);
                break;
            case UTF16LE:
                cp = utf16le_to_ucs4(buf, nbytes, b_ptr);
                break;
        }

        destbuf[i++] = cp;
    }
    return i;
}

/*
 * Funciones auxiliares para las transformaciones de UCS4 a la codificación deseada.
 */

void ucs4_to_utf32le(uint8_t *outbuf, int *b_ptr, uint32_t *cp_ptr) {
    int b = *b_ptr;
    int cp = *cp_ptr;

    outbuf[b++] = (cp & 0xFF);
    outbuf[b++] = (cp >> 8) & 0xFF;
    outbuf[b++] = (cp >> 16) & 0xFF;
    outbuf[b++] = (cp >> 24) & 0xFF;

    *b_ptr = b;
    *cp_ptr = cp;
}

void ucs4_to_utf32be(uint8_t *outbuf, int *b_ptr, uint32_t *cp_ptr) {
    int b = *b_ptr;
    int cp = *cp_ptr;

    outbuf[b++] = (cp >> 24) & 0xFF;
    outbuf[b++] = (cp >> 16) & 0xFF;
    outbuf[b++] = (cp >> 8) & 0xFF;
    outbuf[b++] = cp & 0xFF;

    *b_ptr = b;
    *cp_ptr = cp;
}

void ucs4_to_utf16be(uint8_t *outbuf, int *b_ptr, uint32_t *cp_ptr) {
    int b = *b_ptr;
    int cp = *cp_ptr;

    if (cp <= 0xFFFF) {
        outbuf[b++] = cp >> 8;
        outbuf[b++] = cp & 0xFF;
    } else {
        outbuf[b++] = 0xD8 + ((cp >> 10) & 0x300);
        outbuf[b++] = (cp >> 10) & 0x3F;
        outbuf[b++] = 0xDC + ((cp >> 8) & 0x03);
        outbuf[b++] = cp & 0xFF;
    }

    *b_ptr = b;
    *cp_ptr = cp;
}

void ucs4_to_utf16le(uint8_t *outbuf, int *b_ptr, uint32_t *cp_ptr) {
    int b = *b_ptr;
    int cp = *cp_ptr;

    if (cp <= 0xFFFF) {
        outbuf[b++] = cp;
        outbuf[b++] = cp >> 8;
    } else {
        cp -= 0x10000;
        outbuf[b++] = (cp >> 10) & 0x3F;
        outbuf[b++] = 0xD8 + ((cp >> 10) & 0x300);
        outbuf[b++] = cp & 0xFF;
        outbuf[b++] = 0xDC + ((cp >> 8) & 0x03);
    }

    *b_ptr = b;
    *cp_ptr = cp;
}

void ucs4_to_utf8(uint8_t *outbuf, int *b_ptr, uint32_t *cp_ptr) {
    int b = *b_ptr;
    int cp = *cp_ptr;

    if ((cp >= 0x10000) & (cp <= 0x10FFFF)) {
        outbuf[b++] = ((cp >> 18) & 0x03) | 0xF0;
        outbuf[b++] = ((cp >> 12) & 0x3F) | 0x80;
        outbuf[b++] = ((cp >> 6) & 0x3F) | 0x80;
        outbuf[b++] = (cp & 0x3F) | 0x80;
    } else if ((0x800 <= cp) & (cp <= 0xFFFF)) {
        outbuf[b++] = ((cp >> 12) & 0x0F) | 0xE0;
        outbuf[b++] = ((cp >> 6) & 0x3F) | 0x80;
        outbuf[b++] = (cp & 0x3F) | 0x80;
    } else if ((0x80 <= cp) & (cp <= 0x07FF)) {
        outbuf[b++] = (cp >> 6) | 0xC0;
        outbuf[b++] = (cp & 0x3F) | 0x80;
    } else if (cp <= 0x7F) {
        outbuf[b++] = cp | 0x00;
    }

    *b_ptr = b;
    *cp_ptr = cp;
}

/*
 * Transforma UCS-4 a la codificación deseada.
 *
 * Argumentos:
 *
 *   - enc: el encoding destino.
 *   - input: los codepoints a codificar.
 *   - npoints: el número de codepoints en input.
 *   - output: resultado de la conversión.
 *
 * Devuelve: el número de BYTES obtenidos (número
 *           de elementos escritos en destbuf).
 *
 * Se debe garantiza que "destbuf" puede albergar al menos npoints*4
 * elementos, o bytes (caso enc=UTF-32).
 */
int ucs4_to_dest(enum encoding enc, uint32_t *input, int npoints, uint8_t *outbuf) {
    int w = 0, b = 0;
    int *b_ptr = &b;

    if ((enc == UTF16BE) || (enc == UTF16LE)) {
        b = 2;
    } else if ((enc == UTF32BE) || (enc == UTF32LE)) {
        b = 4;
    }

    for (int i = 0; i < npoints; i++) {
        uint32_t cp = input[i];
        uint32_t *cp_ptr = &cp;

        switch (enc) {
            case UTF32LE:
                ucs4_to_utf32le(outbuf, b_ptr, cp_ptr);
                break;
            case UTF32BE:
                ucs4_to_utf32be(outbuf, b_ptr, cp_ptr);
                break;
            case UTF16BE:
                ucs4_to_utf16be(outbuf, b_ptr, cp_ptr);
                break;
            case UTF16LE:
                ucs4_to_utf16le(outbuf, b_ptr, cp_ptr);
                break;
            case UTF8:
                ucs4_to_utf8(outbuf, b_ptr, cp_ptr);
                break;
        }
        w = b;
    }
    return w;
}

int main(int argc, char *argv[]) {
    enum encoding orig_enc, dest_enc;

    if (argc != 2) {
        fprintf(stderr, "Uso: ./recode57 <encoding>\n");
        return 1;
    }

    if ((dest_enc = str_to_encoding(argv[1])) < 0) {
        fprintf(stderr, "Encoding no válido: %s\n", argv[1]);
        return 1;
    }
    // Detectar codificación origen con byte order mark.
    uint8_t bom[4];

    read(STDIN_FILENO, bom, 4);
    orig_enc = bom_to_encoding(bom);

    // En cada iteración, leer hasta 1024 bytes, convertirlos a UCS-4
    // (equivalente a UTF32-LE con enteros nativos) y convertirlo por
    // salida estándar.
    uint8_t inbuf[1024];
    uint8_t outbuf[1024 * 4];
    uint32_t ucs4[1024];
    size_t prevbytes = 0;
    ssize_t inbytes;
    int npoints, outbytes;

    // Si orig_enc no fue UTF-32, quedaron 2 o 4 bytes en "bom" que
    // deben ser prefijados en inbuf.
    if (orig_enc == UTF8) {
        memcpy(inbuf, bom, 4);
        prevbytes = 4;
    } else if (orig_enc == UTF16BE || orig_enc == UTF16LE) {
        memcpy(inbuf, bom + 2, 2);
        prevbytes = 2;
    }

    // Si dest_enc no es UTF-8, hay que escribir un BOM.
    if (dest_enc == UTF16BE) {
        outbuf[0] = 0xFE;
        outbuf[1] = 0xFF;
    } else if (dest_enc == UTF16LE) {
        outbuf[0] = 0xFF;
        outbuf[1] = 0xFE;
    } else if (dest_enc == UTF32BE) {
        outbuf[0] = 0x00;
        outbuf[1] = 0x00;
        outbuf[2] = 0xFE;
        outbuf[3] = 0xFF;
    } else if (dest_enc == UTF32LE) {
        outbuf[0] = 0xFF;
        outbuf[1] = 0xFE;
        outbuf[2] = 0x00;
        outbuf[3] = 0x00;
    }

    while ((inbytes = read(STDIN_FILENO, inbuf + prevbytes, sizeof(inbuf) - prevbytes)) > 0) {
        prevbytes += inbytes;
        //fprintf(stderr, "Processing: %zu bytes, ", prevbytes);

        npoints = orig_to_ucs4(orig_enc, inbuf, &prevbytes, ucs4);
        outbytes = ucs4_to_dest(dest_enc, ucs4, npoints, outbuf);
        //fprintf(stderr, "codepoints: %d, output: %d bytes, remaining: %zu bytes\n",
        // npoints, outbytes, prevbytes);

        write(STDOUT_FILENO, outbuf, outbytes);

        if (prevbytes > 0) {
            memcpy(inbuf, inbuf + outbytes, prevbytes);
        }
    }
}