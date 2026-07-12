#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "color.c"
#include "vector.h"

typedef int32_t vertexIndex;

typedef struct{
    vectorSize vertices[3];
    
    color col;
} triangle3i;

typedef struct{
    v3i* items;
    size_t count;
    size_t capacity;
} vertices;

typedef struct{
    triangle3i* items;
    vectorSize count;
    vectorSize capacity;
} triangles;

typedef struct{
    vertices v;
    triangles t;

    v3i wPos;

    int32_t yaw;
    int32_t pitch;

    v3i ihat;
    v3i jhat;
    v3i khat;

    size_t scale;
} model3D;

typedef struct{
    uint32_t fov;

    float focalLength;
} camera;

typedef struct{
    int width;
    int height;

    camera cam;

    uint32_t* pixels;
    uint32_t* depthBuffer;
} renderTarget;

#define da_append(da, e)\
    do{\
    if(da.count >= da.capacity){\
        if(da.capacity == 0) da.capacity = 256;\
        else da.capacity *= 2;\
        da.items = realloc(da.items, da.capacity * sizeof(*da.items));\
    }\
    da.items[da.count++] = e;\
    } while(0)

#define da_free(da)\
    do{\
    free(da.items);\
    da.items = NULL;\
    da.count = 0;\
    da.capacity = 0;\
} while(0)

static inline void model3DFree(model3D* model){
    da_free(model->v);
    da_free(model->t);
}

static inline void getBasisVectors(v3i* ihat, v3i* jhat, v3i* khat, int32_t yaw, int32_t pitch){
    double yawR = (double)yaw * PI / 180.0;
    int32_t cosYaw = (int32_t)llround(cos(yawR) * BASIS_SCALE);
    int32_t sinYaw = (int32_t)llround(sin(yawR) * BASIS_SCALE);

    double pitchR = (double)pitch * PI / 180.0;
    int32_t cosPitch = (int32_t)llround(cos(pitchR) * BASIS_SCALE);
    int32_t sinPitch = (int32_t)llround(sin(pitchR) * BASIS_SCALE);

    v3i ihat_yaw = v3i_New(cosYaw, 0, sinYaw);
    v3i jhat_yaw = v3i_New(0, BASIS_SCALE, 0);
    v3i khat_yaw = v3i_New(-sinYaw, 0, cosYaw);

    v3i ihat_pitch = v3i_New(BASIS_SCALE, 0, 0);
    v3i jhat_pitch = v3i_New(0, cosPitch, -sinPitch);
    v3i khat_pitch = v3i_New(0, sinPitch, cosPitch);

    *ihat = v3i_Transform(&ihat_yaw, &jhat_yaw, &khat_yaw, ihat_pitch);
    *jhat = v3i_Transform(&ihat_yaw, &jhat_yaw, &khat_yaw, jhat_pitch);
    *khat = v3i_Transform(&ihat_yaw, &jhat_yaw, &khat_yaw, khat_pitch);
}

static inline v3i vertexToWorld(model3D* m, v3i v){
    v3i rotated = v3i_Transform(&m->ihat, &m->jhat, &m->khat, v);
    return v3i_Add(rotated, m->wPos);
}

static inline bool vertexToScreen(model3D* m, v3i p, renderTarget* rT, v3i* screenPosition){
    v3i world = vertexToWorld(m, p);

    const int32_t nearPlane = 1;
    if(world.z <= nearPlane){
        return false;
    }

    int64_t projectedX = (int64_t)world.x * (int64_t) rT->cam.focalLength / (int64_t)world.z;
    int64_t projectedY = (int64_t)world.y * (int64_t) rT->cam.focalLength / (int64_t)world.z;

    int64_t screenX = (int64_t)rT->width / 2 + projectedX;
    int64_t screenY = (int64_t)rT->height / 2 + projectedY;
    
    if (
        screenX < INT32_MIN ||
        screenX > INT32_MAX ||
        screenY < INT32_MIN ||
        screenY > INT32_MAX
    ) {
        return false;
    }

    *screenPosition = v3i_New(screenX, screenY, world.z);

    return true;
}

static inline bool triangleToScreen(model3D* m, triangle3i t3, renderTarget* rT, triangle3iS* screenTriangle){
    v3i a;
    v3i b;
    v3i c;
    
    bool aVisible = vertexToScreen(m, m->v.items[t3.vertices[0]], rT, &a);
    bool bVisible = vertexToScreen(m, m->v.items[t3.vertices[1]], rT, &b);
    bool cVisible = vertexToScreen(m, m->v.items[t3.vertices[2]], rT, &c);
    
    if (!aVisible || !bVisible || !cVisible) {
        return false;
    }

    boundingBox bb = bb_calculate(a, b, c, rT->width, rT->height);

    *screenTriangle =  (triangle3iS){.a = a, .b = b, .c = c, .bb = bb, .col = t3.col};

    return true;
}

//Only supports obj files that start with o modelName
//Supports only files with single object
static inline int readObj(const char* filename, model3D *model, size_t mReadScale){
    FILE* file = fopen(filename, "r");
    if(!file){
        return 1;
    }

    char line[1024];

    fgets(line, sizeof(line), file);
    if(line[0] != 'o' || line[1] != ' ') { fclose(file); return 2; }

    while(fgets(line, sizeof(line), file)){
        if(line[0] == 'v' && line[1] == ' '){
            float x;
            float y;
            float z;

            if(sscanf_s(line + 2, "%f %f %f", &x, &y, &z) == 3){
                v3i p = {(vectorSize)(x*mReadScale), (vectorSize)(y*mReadScale), (vectorSize)(z*mReadScale)};
                da_append(model->v, p);
            }
        }
        else if(line[0] == 'f' && line[1] == ' '){
            char* tmp = _strdup(line + 2);
            if (!tmp) {	fclose(file); return 3; }

            int faceIndices[64];
			int faceCount = 0;

            char* next = NULL;
            char* tok = strtok_s(tmp, " \t\r\n", &next);    
            while (tok)
            {
                char* slash = strchr(tok, '/');
                if(slash) *slash = '\0';
                int idx = atoi(tok) - 1;
                faceIndices[faceCount++] = idx;
                tok = strtok_s(NULL, " \t\r\n", &next);
            }
            for(int k = 1; k < faceCount - 1; k++){
                color c = crand();
                triangle3i t = {
                    .vertices = {
                        faceIndices[0],
                        faceIndices[k],
                        faceIndices[k + 1]
                    },
                    .col = c
                };

                da_append(model->t, t);
            }
            
            free(tmp);
        }
    }
    fclose(file);
    return 0;
}

#endif