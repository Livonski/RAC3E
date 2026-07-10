#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdbool.h>

typedef int32_t vectorSize;


typedef struct
{
    vectorSize x;
    vectorSize y;
    vectorSize z;
} v3i;

typedef struct{
    vectorSize x;
    vectorSize y;
} v2i;

#define v2i_New(xValue, yValue) \
((v2i){                       \
    .x = (vectorSize)(xValue),\
    .y = (vectorSize)(yValue) \
})

typedef struct{
    v2i a;
    v2i b;
    v2i c;

    color col;
} triangle;

static inline triangle triangle_random(int WIDTH, int HEIGHT){

    double w = (double)rand() / (double)RAND_MAX;
    double h = (double)rand() / (double)RAND_MAX;
    v2i a = {w * WIDTH,  h * HEIGHT};

    w = (double)rand() / (double)RAND_MAX;
    h = (double)rand() / (double)RAND_MAX;
    v2i b = {w * WIDTH,  h * HEIGHT};

    w = (double)rand() / (double)RAND_MAX;
    h = (double)rand() / (double)RAND_MAX;
    v2i c = {w * WIDTH,  h * HEIGHT};

    color col = crand();

    return (triangle){a, b, c, col};
}

static inline int64_t v2i_Dot (v2i a, v2i b) { return (int64_t)a.x * b.x + (int64_t)a.y * b.y; }
static inline v2i     v2i_Perp(v2i v)        { return v2i_New(v.y, -v.x);}
static inline v2i     v2i_Sub (v2i a, v2i b) { return v2i_New(a.x - b.x, a.y - b.y);}

static inline bool v2i_RightSideOfLine(v2i* a, v2i* b, v2i* p){
    v2i ap = v2i_Sub(*p, *a);
    v2i ab = v2i_Sub(*b, *a);
    v2i abPerp = v2i_Perp(ab);

    return v2i_Dot(ap, abPerp) >= 0;
}

bool pointInTriangle(triangle* t, v2i* p){
    bool sideAB = v2i_RightSideOfLine(&t->a, &t->b, p);
    bool sideBC = v2i_RightSideOfLine(&t->b, &t->c, p);
    bool sideCA = v2i_RightSideOfLine(&t->c, &t->a, p);
    return sideAB == sideBC && sideBC == sideCA;
}

#endif