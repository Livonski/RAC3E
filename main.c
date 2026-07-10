#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

#include "fenster.h"

#include "color.c"
#include "vector.h"

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

    int trianglesCount = 32;
    triangle triangles[32];
    for(int i = 0; i < trianglesCount; i++){
        triangles[i] = triangle_random(WIDTH, HEIGHT);
    }


    color pC;

    while (fenster_loop(&window) == 0)
    {
        for(int y = 0; y < HEIGHT; y++){
            for(int x = 0; x < WIDTH; x++){
                pC = c_black;
                for(int i = 0; i < trianglesCount; i++){
                    triangle* triangle = &triangles[i];
                    v2i p = v2i_New(x, y);
                    if(!bb_inside(&triangle->bb, p)) continue;
                    bool inside = pointInTriangle(triangle, p);
                    if(inside) {pC = triangle->col; break;}
                }
                
                pixels[y * WIDTH + x] = cst32(pC);
            }
        }
        
        timePrev = timeNow;
        timeNow = fenster_time();
        dT = timeNow - timePrev;

        printf("\r%lld ms, %lld FPS", dT, 1000/dT);
        fflush(stdout);
    }

    fenster_close(&window);
    return 0;
}