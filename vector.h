#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stdint.h>

typedef int32_t  vectorSize;

//Math utils
#define MIN_VALUE(a, b) ((a) < (b) ? (a) : (b))
#define MAX_VALUE(a, b) ((a) > (b) ? (a) : (b))
#define clamp(minValue, maxValue, value) \
    MAX_VALUE((minValue), MIN_VALUE((maxValue), (value)))

#define BASIS_SCALE 10000
#define PI 3.14159265358979323846

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
    v3i a;
    v3i b;
    v3i c;
    
    color col;
    boundingBox bb;
} triangle3iS;

//v2i related stuff
static inline int64_t v2i_Dot (v2i a, v2i b)        { return (int64_t)a.x * b.x + (int64_t)a.y * b.y; }
static inline v2i     v2i_Perp(v2i v)               { return v2i_New(v.y, -v.x);}
static inline v2i     v2i_Sub (v2i a, v2i b)        { return v2i_New(a.x - b.x, a.y - b.y);}
static inline v2i     v2i_Add (v2i a, v2i b)        { return v2i_New(a.x + b.x, a.y + b.y);}
static inline v2i     v2i_DivS(v2i v, vectorSize s) { return v2i_New(v.x / s, v.y / s);}

//v3i related stuff
static inline int64_t v3i_Dot(v3i a, v3i b)        { return (int64_t)a.x * b.x + (int64_t)a.y * b.y + (int64_t)a.z * b.z; }
static inline v3i v3i_MulS   (v3i v, vectorSize s) {return v3i_New(v.x * s, v.y * s, v.z * s);}
static inline v3i v3i_Add    (v3i a, v3i b)        {return v3i_New(a.x + b.x, a.y + b.y, a.z + b.z);}
static inline v3i v3i_Add3   (v3i a, v3i b, v3i c) {return v3i_Add(v3i_Add(a, b), c);}
static inline v2i v3i_To_v2i (v3i s)               {return v2i_New(s.x, s.y);}

#define bs_Center(v) if(v >= 0) {v += BASIS_SCALE / 2;} else {v -= BASIS_SCALE / 2;}

static inline v3i v3i_Transform(const v3i* ihat, const v3i* jhat, const v3i* khat, v3i v){
    int64_t x = (int64_t)ihat->x * v.x + (int64_t)jhat->x * v.y + (int64_t)khat->x * v.z;
    int64_t y = (int64_t)ihat->y * v.x + (int64_t)jhat->y * v.y + (int64_t)khat->y * v.z;
    int64_t z = (int64_t)ihat->z * v.x + (int64_t)jhat->z * v.y + (int64_t)khat->z * v.z;

    bs_Center(x);
    bs_Center(y);
    bs_Center(z);

    return v3i_New(x / BASIS_SCALE, y / BASIS_SCALE, z / BASIS_SCALE);
}

//v3f related stuff
static inline float v3f_Dot(v3f a, v3f b)        { return (float)a.x * b.x + (float)a.y * b.y + (float)a.z * b.z; }
static inline v3f v3f_MulS (v3f v, float s) {return v3f_New(v.x * s, v.y * s, v.z * s);}
static inline v3f v3f_Add  (v3f a, v3f b)   {return v3f_New(a.x + b.x, a.y + b.y, a.z + b.z);}
static inline v3f v3f_Add3 (v3f a, v3f b, v3f c) {return v3f_Add(v3f_Add(a, b), c);}

//triangle related stuff
static inline int32_t SignedTriangleArea(v2i a, v2i b, v2i c){
    v2i ac = v2i_Sub(a, c);
    v2i abPerp = v2i_Perp(v2i_Sub(b, a));
    return v2i_Dot(ac, abPerp) / 2;
}

static inline bool pointInTriangle(triangle3iS t, v2i p, v3f* weights){
    int32_t areaABP = SignedTriangleArea(v3i_To_v2i(t.a), v3i_To_v2i(t.b), p);
    int32_t areaBCP = SignedTriangleArea(v3i_To_v2i(t.b), v3i_To_v2i(t.c), p);
    int32_t areaCAP = SignedTriangleArea(v3i_To_v2i(t.c), v3i_To_v2i(t.a), p);

    bool inTri = areaABP >= 0 && areaBCP >= 0 && areaCAP >= 0;

    float invAreaSum = 1 / (areaABP + areaBCP + areaCAP);
    float weightA = areaBCP * invAreaSum;
    float weightB = areaCAP * invAreaSum;
    float weightC = areaABP * invAreaSum;

    *weights = v3f_New(weightA, weightB, weightC);

    return inTri;
}


//bounding box related stuff
static inline bool bb_inside(boundingBox *bb, v2i p){
    return p.x > bb->bbLL.x &&
           p.y > bb->bbLL.y &&
           p.x < bb->bbUR.x &&
           p.y < bb->bbUR.y;
}

static inline boundingBox bb_calculate(v3i a, v3i b, v3i c, int WIDTH, int HEIGHT){
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