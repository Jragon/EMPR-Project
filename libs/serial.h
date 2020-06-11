#pragma once

/**
    @brief initilises the serial port on UART1 (Port 0, Pin2/3)
*/
void serial_init();

/**
    @brief reads data from serial port

    @param buf buffer to read data into
    @param length how many bytes to read
    @return int bytes read
*/
int serial_read(char* buf, int length);

/**
    @brief writes data to serial port

    @param buf data to write
    @param length length of data
    @return int bytes written
*/
int serial_write(char* buf, int length);

/**
    @brief write a format string to serial out

    @param format format string (anything you can pass to printf)
    @param ... arguments
    @return int bytes written
*/
int serial_printf(const char* format, ...);
