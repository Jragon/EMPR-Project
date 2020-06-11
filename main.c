#include <string.h>

#include "lib/serial.h"

int main(int argc, char* argv[]) {
    serial_init();
    serial_printf("Hello World\r\n");
    return 0;
}
