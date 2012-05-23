#ifndef DEF_TERRAIN
#define DEF_TERRAIN

#include <iostream>
#include <math.h>
#include <vector>
#include "vec2.hpp"
#include "vec3.hpp"
#include "constants.hpp"
#include "object.hpp"
#include "perlin.hpp"

typedef enum Fill_Modes {
    SIDE_UP_FILL, SIDE_RIGHT_FILL, EDGE_FILL
} Fill_Modes;

class Terrain {
public:
    Terrain(float precision,PerlinNoise *noise);
    void generate_terrain(Vec2<float> coord,float xlength,float ylength,Object *object,bool generate_lod);
    void scale(float x,float y,float z);
    float height(float x,float y);

private:
    float precision;
    Vec3<float> scalar;
    PerlinNoise *noise;
};

void fill_line_buffer(std::vector<int> &line_buffer,int i,int imax,int j,int jmax,Fill_Modes mode);
void fill_triangle_buffer(std::vector<int> &tri_buffer,int i,int imax,int j,int jmax,Fill_Modes mode);

#endif
