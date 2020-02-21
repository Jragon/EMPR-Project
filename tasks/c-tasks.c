#include <lpc_types.h>

#include <math.h>

#include "../libs/keypad.h"
#include "../libs/lcd.h"
#include "../libs/scanner/grid.h"
#include "../libs/scanner/motors.h"
#include "../libs/scanner/sensor.h"
#include "../libs/serial.h"
#include "../libs/systick_delay.h"
#include "../libs/timer.h"
#include "../libs/util_macros.h"

void _move_print_rgb(uint16_t x, uint16_t y, uint16_t step, uint16_t int_time) {
    uint16_t red, green, blue;
    uint32_t last_time = timer_get();
    uint16_t movex, movey;
    uint8_t tw = 0;
    while (x != grid.x || y != grid.y) {
        if (timer_get() - last_time < int_time << 2) {
            continue;
        }

        sensor_read_rgb(&red, &green, &blue);

        // if (tw == 0 && red != 2048 && green != 2048 && blue != 2048) {
        //     systick_delay_blocking(500);
        //     tw = 1;
        // }

        // if (tw == 1 && red == 2048 && green == 2048 && blue == 2048) {
        //     systick_delay_blocking(500);
        //     tw = 0;
        // }

        grid_step_to_point(x, y, step);

        last_time = timer_get();

        lcd_printf(0x00, "R %5d       ", red);
        lcd_printf(0x40, "G %5d B %5d", green, blue);
        serial_printf("%d %d: %d %d %d;", grid.x - step, grid.y - step, red, green, blue);
    }
}

void flag_edge_detect() {
    serial_printf("[Task]: Flag Edge Detect\r\n");
    grid_home();

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    // do three horizontal scans and three vertical scans
    // then take max value

    // x axis scan
    // uint16_t x_scans[3] = {100, 450, 700};
    // for (int i = 0; i < 3; i++) {
    //     grid_move_to_point(x_scans[i], 0);
    //     systick_delay_blocking(100);
    //     _move_print_rgb(x_scans[i], grid.max_y, 20, int_time);
    //     systick_delay_blocking(100);
    // }

    // uint16_t y_scans[3] = {100, 450, 700};
    // for (int i = 0; i < 3; i++) {
    //     grid_move_to_point(0, y_scans[i]);
    //     // systick_delay_blocking(100);
    //     _move_print_rgb(grid.max_x, y_scans[i], 5, int_time);
    //     // systick_delay_blocking(100);
    // }

    // diagonal scan
    while (1) {
        grid_move_to_point(0, 0);
        _move_print_rgb(grid.max_x, grid.max_y, 5, int_time);

        grid_move_to_point(0, grid.max_y);
        _move_print_rgb(grid.max_x, 0, 5, int_time);
    }
}

#define STEP_X 150
#define STEP_Y 150
void flag_scan() {
    serial_printf("[Task]: Flag Scan\r\n");
    grid_home();

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    serial_printf("{");
    uint16_t red, green, blue;
    for (uint8_t row = 1; row < 5; row++) {
        for (uint8_t col = 1; col < 4; col++) {
            grid_move_to_point(grid.max_x - STEP_X * col, grid.max_y - STEP_Y * row);
            timer_block(int_time);
            sensor_read_rgb(&red, &green, &blue);
            serial_printf("{%d, %d, %d}%c ", red, green, blue,
                          (row == 4 && col == 3) ? '}' : ',');
        }
    }

    serial_printf("\r\n");
}

struct flag {
    char name[10];
    uint16_t vals[12][3];
    // multiplied by 1000
    uint16_t error;
} typedef flag_t;

#define FLAGS_LEN 10
flag_t flags[] = {{"car",
                   {{524, 837, 994},
                    {971, 1136, 976},
                    {1787, 1533, 800},
                    {1104, 451, 443},
                    {1170, 473, 464},
                    {1387, 564, 544},
                    {674, 955, 1065},
                    {1060, 1188, 1039},
                    {1929, 1646, 850},
                    {576, 833, 941},
                    {946, 1080, 933},
                    {1721, 1477, 751}},
                   0},
                  {"france",
                   {{340, 688, 957},
                    {365, 732, 1013},
                    {392, 790, 1090},
                    {2048, 2048, 2048},
                    {2048, 2048, 2048},
                    {2048, 2048, 2048},
                    {1870, 1362, 1346},
                    {1866, 1239, 1230},
                    {1850, 1106, 1102},
                    {981, 395, 387},
                    {1051, 414, 407},
                    {1194, 471, 462}},
                   0},
                  {"uk",
                   {{1298, 1122, 1036},
                    {1649, 1390, 1349},
                    {1125, 1070, 893},
                    {995, 567, 552},
                    {1032, 559, 546},
                    {1133, 1057, 881},
                    {1157, 944, 900},
                    {1577, 1391, 1347},
                    {1059, 1016, 852},
                    {671, 762, 765},
                    {854, 824, 828},
                    {1098, 1052, 881}},
                   0},
                  {"macedonia",
                   {{1025, 723, 440},
                    {1388, 1048, 557},
                    {943, 604, 420},
                    {1552, 1172, 638},
                    {2044, 1652, 854},
                    {1900, 1494, 799},
                    {774, 469, 361},
                    {1337, 986, 545},
                    {851, 499, 405},
                    {944, 651, 401},
                    {1494, 1214, 605},
                    {1023, 744, 453}},
                   0},
                  {"sudan",
                   {{589, 541, 446},
                    {284, 354, 279},
                    {1118, 1059, 868},
                    {1163, 1054, 981},
                    {716, 691, 632},
                    {1111, 1061, 877},
                    {1111, 993, 925},
                    {717, 690, 632},
                    {1088, 1035, 853},
                    {959, 838, 776},
                    {629, 605, 553},
                    {1034, 975, 799}},
                   0},
                  {"czechia",
                   {{1181, 462, 456},
                    {1347, 818, 784},
                    {2048, 2048, 2048},
                    {1220, 478, 471},
                    {1318, 835, 828},
                    {2048, 2048, 2048},
                    {1343, 534, 532},
                    {355, 712, 1008},
                    {2048, 2048, 2048},
                    {423, 819, 1149},
                    {393, 816, 1161},
                    {414, 884, 1255}},
                   0},
                  {"burundi",
                   {{1482, 1170, 1118},
                    {580, 890, 698},
                    {1926, 1585, 1546},
                    {1290, 845, 799},
                    {2048, 2004, 1947},
                    {1556, 1017, 971},
                    {1354, 977, 918},
                    {926, 1155, 991},
                    {1837, 1481, 1426},
                    {1046, 1157, 1016},
                    {378, 638, 461},
                    {898, 1187, 1015}},
                   0},
                  {"burkina",
                   {{362, 605, 441},
                    {503, 431, 354},
                    {684, 351, 330},
                    {372, 621, 452},
                    {1277, 1074, 583},
                    {742, 381, 357},
                    {357, 592, 431},
                    {494, 453, 355},
                    {725, 371, 348},
                    {367, 619, 450},
                    {503, 464, 374},
                    {732, 376, 352}},
                   0},
                  {"finland",
                   {{604, 838, 974},
                    {1282, 778, 751},
                    {847, 1076, 1194},
                    {534, 823, 1011},
                    {1399, 867, 839},
                    {429, 796, 1058},
                    {287, 570, 804},
                    {1254, 799, 775},
                    {349, 715, 1013},
                    {274, 540, 759},
                    {1130, 752, 731},
                    {303, 625, 884}},
                   0},
                  {"syria",
                   {{186, 165, 145},
                    {1284, 1515, 1377},
                    {1432, 572, 561},
                    {202, 189, 168},
                    {2048, 2048, 2048},
                    {1457, 587, 575},
                    {197, 182, 161},
                    {1246, 1439, 1317},
                    {1307, 526, 514},
                    {205, 179, 157},
                    {1709, 1679, 1617},
                    {1231, 493, 481}},
                   0}};

void flag_reset_errors() {
    for (uint8_t flag_index = 0; flag_index < FLAGS_LEN; flag_index++) {
        flags[flag_index].error = 0;
    }
}

int flag_get_min_error() {
    int min_index = 0;
    int min = flags[min_index].error;
    serial_printf("Errors: \r\n");
    for (uint8_t flag_index = 0; flag_index < FLAGS_LEN; flag_index++) {
        serial_printf("%s: %d\r\n", flags[flag_index].name, flags[flag_index].error);
        if (flags[flag_index].error < min) {
            min_index = flag_index;
            min = flags[flag_index].error;
        }
    }

    return min_index;
}
void flag_detect() {
    serial_printf("[Task]: Flag Detect\r\n");
    grid_home();
    lcd_clear_display();

    flag_reset_errors();

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    uint16_t rgb[3] = {0};
    int i = 0;
    for (uint8_t row = 1; row < 5; row++) {
        for (uint8_t col = 1; col < 4; i++, col++) {
            grid_move_to_point(grid.max_x - STEP_X * col, grid.max_y - STEP_Y * row);
            timer_block(int_time);

            // serial_printf("(%d, %d): ", grid.x, grid.y);
            sensor_read_rgb(rgb, rgb + 1, rgb + 2);

            // compare current results an store error
            for (uint8_t flag_index = 0; flag_index < FLAGS_LEN; flag_index++) {
                // serial_printf("%s, %d, %d, %d\r\n", flags[flag_index].name, row, col,
                // i);
                for (uint8_t j = 0; j < 3; j++) {
                    // 1000 offset
                    flags[flag_index].error +=
                      ABS((int)(flags[flag_index].vals[i][j] - rgb[j]));
                    // serial_printf("%d, %d, %d \r\n", flags[flag_index].vals[i][j],
                    // rgb[j], ABS((int)(flags[flag_index].vals[i][j] - rgb[j])));
                }
            }
        }
    }

    int min_index = flag_get_min_error();
    lcd_printf(0, "%s", flags[min_index].name);
    lcd_printf(0x40, "%.2f%% match", (float)(100 - flags[min_index].error / 1000));

    while (1)
        ;
}