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

#define WIDTH 640
#define HEIGHT 360

int main(void){
    uint32_t pixels[WIDTH * HEIGHT];

    struct fenster window = {
        .title = "RAC3E",
        .width = WIDTH,
        .height = HEIGHT,
        .buf = pixels
    };

    fenster_open(&window);

    int64_t timePrev = fenster_time();
    int64_t timeNow  = timePrev;
    int64_t dT = timeNow - timePrev;
    srand(timeNow);

    Model3D model;
    readObj("cube.obj", &model);
    
    while (fenster_loop(&window) == 0)
    {
        memset(pixels, 0, WIDTH*HEIGHT*sizeof(uint32_t));
        for(int i = 0; i < model.t.count; i++){
            triangle2i currentTriangle = worldToScreenTriangle(&model, model.t.items[i], WIDTH, HEIGHT);
            for(int y = currentTriangle.bb.bbLL.y; y < currentTriangle.bb.bbUR.y; y++){
                for(int x = currentTriangle.bb.bbLL.x; x < currentTriangle.bb.bbUR.x; x++){
                    if(!pointInTriangle(currentTriangle, v2i_New(x, y))) continue;
                    pixels[y * WIDTH + x] = cst32(currentTriangle.col);
                }    
            }
        }
        
        timePrev = timeNow;
        timeNow = fenster_time();
        dT = timeNow - timePrev;
        
        printf("\r%lld ms, %lld FPS", dT, 1000/dT);
        fflush(stdout);
    }
    
    model3DFree(&model);
    fenster_close(&window);
    return 0;
}