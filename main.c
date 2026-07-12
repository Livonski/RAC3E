#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

#include "fenster.h"

#include "color.c"
#include "vector.h"
#include "objParser.h"

int main(void){
    int width  = 640;
    int height = 360;
    int sHeightWorld = 200;

    uint32_t p[width * height];
    renderTarget rT = {.width = width, .height = height, .pixels = p, .PPWU = height / sHeightWorld};

    struct fenster window = {
        .title = "RAC3E",
        .width = rT.width,
        .height = rT.height,
        .buf = rT.pixels
    };

    fenster_open(&window);

    int64_t timePrev = fenster_time();
    int64_t timeNow  = timePrev;
    int64_t dT = timeNow - timePrev;
    srand(timeNow);

    model3D model = {.scale = 1, .wPos = (v3i){0, 0, 0}};
    readObj("cube.obj", &model, 100);
    
    while (fenster_loop(&window) == 0)
    {
        timePrev = timeNow;
        timeNow = fenster_time();
        dT = timeNow - timePrev;

        float deltaSeconds = (float)dT / 1000.0f;
        model.yaw += 1.0f * deltaSeconds;

        if (model.yaw >= 2.0f * PI)
            model.yaw -= 2.0f * PI;

        memset(rT.pixels, 0, rT.width*rT.height*sizeof(uint32_t));
        for(int i = 0; i < model.t.count; i++){
            triangle2i currentTriangle = triangleToScreen(&model, model.t.items[i], &rT);
            //BUG: if triangle goes completely outside of view space program will crash
            //probably due to going out of bounds or something
            for(int y = currentTriangle.bb.bbLL.y; y < currentTriangle.bb.bbUR.y; y++){
                for(int x = currentTriangle.bb.bbLL.x; x < currentTriangle.bb.bbUR.x; x++){
                    if(!pointInTriangle(currentTriangle, v2i_New(x, y))) continue;
                    rT.pixels[y * rT.width + x] = cst32(currentTriangle.col);
                }    
            }
        }


        
        printf("\r%lld ms, %lld FPS, %d(triangles)", dT, 1000/dT, model.t.count);
        fflush(stdout);
    }
    
    model3DFree(&model);
    fenster_close(&window);
    return 0;
}