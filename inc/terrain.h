#ifndef DEF_TERRAIN
#define DEF_TERRAIN

#include <iostream>
#include <math.h>
#include <vector>
#include "vec2.h"
#include "vec3.h"
#include "constants.h"
#include "object.h"
#include "perlin.h"

typedef enum Fill_Modes {
    SIDE_UP_FILL, SIDE_RIGHT_FILL, EDGE_FILL
} Fill_Modes;

class Terrain {
public:
    Terrain(float (*height_func)(float,float),float precision,PerlinNoise *noise);
    void generate_terrain(Vec2<float> coord,float xlength,float ylength,Object *object);
    void scale(float x,float y,float z);
    float height(float x,float y);

private:
    float (*height_func)(float,float);
    float precision;
    Vec3<float> scalar;
    PerlinNoise *noise;
};

void fill_line_buffer(std::vector<int> &line_buffer,int i,int imax,int j,int jmax,Fill_Modes mode);
void fill_triangle_buffer(std::vector<int> &tri_buffer,int i,int imax,int j,int jmax,Fill_Modes mode);

#endif
