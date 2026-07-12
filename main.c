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

    uint32_t p[width * height];
    renderTarget rT = {.width = width, .height = height, .pixels = p, .focalLength = 300};

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
    int64_t lastStatsTime = fenster_time();
    int64_t lastRotationTime = fenster_time();

    srand(timeNow);

    model3D model = {.scale = 1, .wPos = (v3i){0, 0, 400}, .yaw = 0, .pitch = 180};
    readObj("cube.obj", &model, 100);
    //readObj("monkey.obj", &model, 100);

    while (fenster_loop(&window) == 0)
    {
        timePrev = timeNow;
        timeNow = fenster_time();
        dT = timeNow - timePrev;
        
        if(timeNow - lastRotationTime >= 50){
            model.wPos.z += 10;
            model.yaw += 1;
            model.pitch += 1;
    
            if (model.yaw >= 360) {
                model.yaw -= 360;
            }
            getBasisVectors(
                &model.ihat,
                &model.jhat,
                &model.khat,
                model.yaw,
                model.pitch
            );
            lastRotationTime = timeNow;
        }

        
        memset(rT.pixels, 0, rT.width*rT.height*sizeof(uint32_t));
        for(int i = 0; i < model.t.count; i++){
            triangle2i currentTriangle;
            if(!triangleToScreen(&model, model.t.items[i], &rT, &currentTriangle)) continue;
            //BUG: if triangle goes completely outside of view space program will crash
            //probably due to going out of bounds or something
            for(int y = currentTriangle.bb.bbLL.y; y < currentTriangle.bb.bbUR.y; y++){
                for(int x = currentTriangle.bb.bbLL.x; x < currentTriangle.bb.bbUR.x; x++){
                    if(!pointInTriangle(currentTriangle, v2i_New(x, y))) continue;
                    rT.pixels[y * rT.width + x] = cst32(currentTriangle.col);
                }    
            }
        }
        
        
        
        if (timeNow - lastStatsTime >= 500)
        {
            int64_t fps = dT > 0 ? 1000 / dT : 0;

            printf(
                "\r%lld ms, %lld FPS, %d triangles",
                dT,
                fps,
                model.t.count
            );

            fflush(stdout);
            lastStatsTime = timeNow;
        }
    }
    
    model3DFree(&model);
    fenster_close(&window);
    return 0;
}