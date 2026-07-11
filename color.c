#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>
#include <stdlib.h>

//Color declaration
#define c_black (color){15, 15, 15}
#define c_red   (color){255, 0, 0}
#define c_green (color){0, 255, 0}
#define c_blue  (color){0, 0, 255}

typedef struct{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color;

static inline uint32_t cst32(color c){
    return ((uint32_t)c.r << 16) | ((uint32_t)c.g << 8) | c.b;
}

uint32_t ct32(uint8_t r, uint8_t g, uint8_t b){
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

static inline color crand(){
    uint8_t r = rand() % 255;
    uint8_t g = rand() % 255;
    uint8_t b = rand() % 255;

    return (color){r, g, b};
}

#endif