#include <unistd.h>

extern void __attribute__((noreturn)) my_exit(int status);

const char msg[] = "Hello, world!\n";

int main(void) {
    write(1, msg, sizeof msg - 1);

    my_exit(7);
}
