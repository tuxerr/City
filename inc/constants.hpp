#ifndef DEF_CONSTANTS
#define DEF_CONSTANTS

#define MAX_LIGHTS 100
#define TITLE "City"
#define DEPTH_TEXTURE_SIZE 1024
#define SHADOWING_NEAR_RANGE 4  // if the shadows begin at NEAR, the first layer will be of distance SHADOWING_NEAR_RANGE
#define SHADOWING_MAX_LAYERS 7 // max layers
#define EPS 0.01
#define TERRAIN_LOD_DEPTH 4
#define OCTREE_SIZE 4096
#define OCTREE_PRECISION 16     
#define LINEAR_LIGHT_ATTENUATION 1
#define DEFERRED_CUBE_SIZE 2

#endif
