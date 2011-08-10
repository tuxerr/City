#include "terrain.h"

Terrain::Terrain(float (*height_func)(float,float),float precision) : height_func(height_func), precision(precision) {
    
}
