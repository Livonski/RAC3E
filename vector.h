#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdbool.h>

typedef int16_t vectorSize;

//Math utils
#define clamp(minv, maxv, v) min(max(minv, v),maxv)

//Vector 3 int
typedef struct{
    vectorSize x;
    vectorSize y;
    vectorSize z;
} v3i;

//Vector 2 int
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
    v2i bbUR;
    v2i bbLL;
} boundingBox;

typedef struct{
    vectorSize vertices[3];

    color col;

    boundingBox bb;
} triangle;



//v2i related stuff
static inline int64_t v2i_Dot (v2i a, v2i b) { return (int64_t)a.x * b.x + (int64_t)a.y * b.y; }
static inline v2i     v2i_Perp(v2i v)        { return v2i_New(v.y, -v.x);}
static inline v2i     v2i_Sub (v2i a, v2i b) { return v2i_New(a.x - b.x, a.y - b.y);}

static inline bool v2i_RightSideOfLine(v2i* a, v2i* b, v2i p){
    v2i ap = v2i_Sub(p, *a);
    v2i ab = v2i_Sub(*b, *a);
    v2i abPerp = v2i_Perp(ab);
    
    return v2i_Dot(ap, abPerp) >= 0;
}

/*bool pointInTriangle(triangle* t, v2i p){
    bool sideAB = v2i_RightSideOfLine(&t->a, &t->b, p);
    bool sideBC = v2i_RightSideOfLine(&t->b, &t->c, p);
    bool sideCA = v2i_RightSideOfLine(&t->c, &t->a, p);
    return sideAB == sideBC && sideBC == sideCA;
}*/

//triangle related stuff

//bounding box related stuff
static inline bool bb_inside(boundingBox *bb, v2i p){
    return p.x > bb->bbLL.x &&
           p.y > bb->bbLL.y &&
           p.x < bb->bbUR.x &&
           p.y < bb->bbUR.y;
}

static inline boundingBox bb_calculate(v2i a, v2i b, v2i c, int WIDTH, int HEIGHT){
    vectorSize minX = min(min(a.x, b.x), c.x);
    vectorSize maxX = max(max(a.x, b.x), c.x);

    vectorSize minY = min(min(a.y, b.y), c.y);
    vectorSize maxY = max(max(a.y, b.y), c.y);

    vectorSize bbLLx = clamp(0, WIDTH, minX);
    vectorSize bbLLy = clamp(0, HEIGHT, minY);

    vectorSize bbURx = clamp(0, WIDTH, maxX);
    vectorSize bbURy = clamp(0, HEIGHT, maxY);

    v2i bbUR = v2i_New(bbURx, bbURy);
    v2i bbLL = v2i_New(bbLLx, bbLLy);
    return (boundingBox){bbUR, bbLL};
}
#endif