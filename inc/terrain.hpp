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
#include "logger.hpp"

#define TERRAIN_TEX_RESOLUTION 1024

typedef struct Terrain_Data {
    Texture *geometry_tex;
    Texture *color_tex;
    Vec2<float> init_coord;
    float length;
} Terrain_Data;

class Terrain {
public:
    Terrain(PerlinNoise *noise);
    void generate_terrain();
    void generate_patches(float length, Object *o);
    void scale(float x,float y,float z);
    float height(float x,float y);

private:
    Vec3<float> scalar;
    PerlinNoise *noise;
};


#endif
