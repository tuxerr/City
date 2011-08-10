#ifndef DEF_TERRAIN
#define DEF_TERRAIN

#include <iostream>
#include <math.h>

class Terrain {
public:
    Terrain(float (*height_func)(float,float),float precision);

private:
    float (*height_func)(float,float);
    float precision;
};

#endif
