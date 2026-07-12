#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stdint.h>

typedef int32_t  vectorSize;

//Math utils
#define PI 3.14159265358979323846f
#define DEG_TO_RAD (PI / 180.0f)

#define MIN_VALUE(a, b) ((a) < (b) ? (a) : (b))
#define MAX_VALUE(a, b) ((a) > (b) ? (a) : (b))
#define clamp(minValue, maxValue, value) \
    MAX_VALUE((minValue), MIN_VALUE((maxValue), (value)))

//Vector 3 int
typedef struct{
    vectorSize x;
    vectorSize y;
    vectorSize z;
} v3i;
#define v3i_New(xValue, yValue, zValue) \
((v3i){                       \
    .x = (vectorSize)(xValue),\
    .y = (vectorSize)(yValue), \
    .z = (vectorSize)(zValue) \
})


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
    float x;
    float y;
    float z;
} v3f;
#define v3f_New(xValue, yValue, zValue) \
((v3f){                       \
    .x = (float)(xValue),\
    .y = (float)(yValue), \
    .z = (float)(zValue) \
})

typedef struct{
    v2i bbUR;
    v2i bbLL;
} boundingBox;

typedef struct{
    v2i a;
    v2i b;
    v2i c;
    
    color col;
    boundingBox bb;
} triangle2i;

//v2i related stuff
static inline int64_t v2i_Dot (v2i a, v2i b)        { return (int64_t)a.x * b.x + (int64_t)a.y * b.y; }
static inline v2i     v2i_Perp(v2i v)               { return v2i_New(v.y, -v.x);}
static inline v2i     v2i_Sub (v2i a, v2i b)        { return v2i_New(a.x - b.x, a.y - b.y);}
static inline v2i     v2i_Add (v2i a, v2i b)        { return v2i_New(a.x + b.x, a.y + b.y);}
static inline v2i     v2i_DivS(v2i v, vectorSize s) { return v2i_New(v.x / s, v.y / s);}

static inline bool v2i_RightSideOfLine(v2i a, v2i b, v2i p){
    v2i ap = v2i_Sub(p, a);
    v2i ab = v2i_Sub(b, a);
    v2i abPerp = v2i_Perp(ab);
    
    return v2i_Dot(ap, abPerp) >= 0;
}

//v3i related stuff
static inline v3i v3i_MulS(v3i v, vectorSize s) {return v3i_New(v.x * s, v.y * s, v.z * s);}
static inline v3i v3i_Add (v3i a, v3i b)        {return v3i_New(a.x + b.x, a.y + b.y, a.z + b.z);}
static inline v3i v3i_Add3(v3i a, v3i b, v3i c) {return v3i_Add(v3i_Add(a, b), c);}


//v3f related stuff
static inline v3f v3f_MulS(v3f v, float s) {return v3f_New(v.x * s, v.y * s, v.z * s);}
static inline v3f v3f_Add (v3f a, v3f b)   {return v3f_New(a.x + b.x, a.y + b.y, a.z + b.z);}
static inline v3f v3f_Add3(v3f a, v3f b, v3f c) {return v3f_Add(v3f_Add(a, b), c);}

static inline v3f v3f_Transform(v3f* ihat, v3f* jhat, v3f* khat, v3f v){
    return v3f_Add3(v3f_MulS(*ihat, v.x), v3f_MulS(*jhat, v.y), v3f_MulS(*khat, v.z));
}

//triangle related stuff
bool pointInTriangle(triangle2i t, v2i p){
    bool sideAB = v2i_RightSideOfLine(t.a, t.b, p);
    bool sideBC = v2i_RightSideOfLine(t.b, t.c, p);
    bool sideCA = v2i_RightSideOfLine(t.c, t.a, p);
    return sideAB == sideBC && sideBC == sideCA;
}


//bounding box related stuff
static inline bool bb_inside(boundingBox *bb, v2i p){
    return p.x > bb->bbLL.x &&
           p.y > bb->bbLL.y &&
           p.x < bb->bbUR.x &&
           p.y < bb->bbUR.y;
}

static inline boundingBox bb_calculate(v2i a, v2i b, v2i c, int WIDTH, int HEIGHT){
    vectorSize minX = MIN_VALUE(MIN_VALUE(a.x, b.x), c.x);
    vectorSize maxX = MAX_VALUE(MAX_VALUE(a.x, b.x), c.x);

    vectorSize minY = MIN_VALUE(MIN_VALUE(a.y, b.y), c.y);
    vectorSize maxY = MAX_VALUE(MAX_VALUE(a.y, b.y), c.y);

    vectorSize bbLLx = clamp(0, WIDTH, minX);
    vectorSize bbLLy = clamp(0, HEIGHT, minY);

    vectorSize bbURx = clamp(0, WIDTH, maxX + 1);
    vectorSize bbURy = clamp(0, HEIGHT, maxY + 1);

    return (boundingBox){
        .bbUR = v2i_New(bbURx, bbURy),
        .bbLL = v2i_New(bbLLx, bbLLy)
    };
}
#endif