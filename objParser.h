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

    v3i ihat;
    v3i jhat;
    v3i khat;

    size_t scale;
} model3D;

typedef struct{
    int width;
    int height;
    
    float PPWU;

    uint32_t* pixels;
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

static inline void getBasisVectors(v3i* ihat, v3i* jhat, v3i* khat, int32_t yaw){
    const double pi = 3.14159265358979323846;

    double yawR = (double)yaw * pi / 180.0;
    int32_t cosYaw = (int32_t)llround(cos(yawR) * BASIS_SCALE);
    int32_t sinYaw = (int32_t)llround(sin(yawR) * BASIS_SCALE);

    *ihat = v3i_New(cosYaw, 0, sinYaw);
    *jhat = v3i_New(0, BASIS_SCALE, 0);
    *khat = v3i_New(-sinYaw, 0, cosYaw);
}

static inline v3i vertexToWorld(model3D* m, v3i v){
    v3i rotated = v3i_Transform(&m->ihat, &m->jhat, &m->khat, v);
    return v3i_Add(rotated, m->wPos);
}

static inline v2i vertexToScreen(model3D* m, v3i p, renderTarget* rT){
    v3i world = vertexToWorld(m, p);

    int32_t screenX = rT->width / 2 + world.x * rT->PPWU;
    int32_t screenY = rT->height / 2 + world.y * rT->PPWU;
    
    return v2i_New(screenX, screenY);
}

static inline triangle2i triangleToScreen(model3D* m, triangle3i t3, renderTarget* rT){
    v2i a = vertexToScreen(m, m->v.items[t3.vertices[0]], rT);
    v2i b = vertexToScreen(m, m->v.items[t3.vertices[1]], rT);
    v2i c = vertexToScreen(m, m->v.items[t3.vertices[2]], rT);
    
    boundingBox bb = bb_calculate(a, b, c, rT->width, rT->height);

    return (triangle2i){.a = a, .b = b, .c = c, .bb = bb, .col = t3.col};
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