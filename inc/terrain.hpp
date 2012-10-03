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

#define TERRAIN_TEX_RESOLUTION 1024

typedef enum Fill_Modes {
    SIDE_UP_FILL, SIDE_RIGHT_FILL, EDGE_FILL
} Fill_Modes;

typedef struct Terrain_Data {
    Texture *geometry_tex;
    Texture *color_tex;
    Vec2<float> init_coord;
    float length;
} Terrain_Data;

class Terrain {
public:
    Terrain(float precision,PerlinNoise *noise);
    Terrain_Data generate_terrain(Vec2<float> coord,float xlength,float ylength);
    void scale(float x,float y,float z);
    float height(float x,float y);

private:
    Vec3<float> scalar;
    PerlinNoise *noise;
};

void fill_line_buffer(std::vector<int> &line_buffer,int i,int imax,int j,int jmax,Fill_Modes mode);
void fill_triangle_buffer(std::vector<int> &tri_buffer,int i,int imax,int j,int jmax,Fill_Modes mode);

#endif
