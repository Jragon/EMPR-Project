#include <lpc_types.h>

struct scanData {
    uint16_t min_x, min_y, max_x, max_y, width, height, widthOffset, heightOffset,
      widthStep, heightStep, startX, startY;
} typedef scan_t;

void detect_edges();
void detect_scan();