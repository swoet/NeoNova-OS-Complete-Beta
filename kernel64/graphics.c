// graphics primitives

#include <stdio.h>

void draw_line(int x0, int y0, int x1, int y1) {
    printf("[Graphics] Draw line from (%d,%d) to (%d,%d)\n", x0, y0, x1, y1);
}

void draw_rect(int x, int y, int w, int h) {
    printf("[Graphics] Draw rect at (%d,%d) size %dx%d\n", x, y, w, h);
}

void draw_circle(int x, int y, int r) {
    printf("[Graphics] Draw circle at (%d,%d) radius %d\n", x, y, r);
}
