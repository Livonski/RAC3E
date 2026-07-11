#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <stdlib.h>
#include <stdio.h>

#include "color.c"
#include "vector.h"

typedef struct{
    v3i* items;
    size_t count;
    size_t capacity;
} vertices;

typedef struct{
    triangle* items;
    vectorSize count;
    vectorSize capacity;
} triangles;

typedef struct{
    vertices v;
    triangles t;
} Model3D;

#define da_append(da, e)\
    do{\
    if(da.count >= da.capacity){\
        if(da.capacity == 0) da.capacity = 256;\
        else da.capacity *= 2;\
        da.items = realloc(da.items, da.capacity * sizeof(v3i));\
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

static inline void model3DFree(Model3D* model){
    da_free(model->v);
}

//Only supports obj files that start with o modelName
//Supports only files with single object
static inline int readObj(const char* filename, Model3D *model){
    FILE* file = fopen(filename, "r");
    if(!file){
        fclose(file);
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
                v3i p = {(vectorSize)(x*100), (vectorSize)(y*100), (vectorSize)(z*100)};
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
                triangle t = {
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

    for(int i = 0; i < model->v.count; i++){
        printf("%d %d %d\n", model->v.items[i].x, model->v.items[i].y, model->v.items[i].z);
    }
    for(int i = 0; i < model->t.count; i++){
        printf("%d/%d/%d\n", model->t.items[i].vertices[0], model->t.items[i].vertices[1], model->t.items[i].vertices[2]);
    }

    fclose(file);
    return 0;
}

#endif