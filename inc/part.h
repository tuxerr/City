#ifndef DEF_PART
#define DEF_PART

#include <iostream>
#include "vec3.h"
#include <vector>

typedef enum Draw_Modes {
    PART_LINES, PART_QUADS
} Draw_Modes; 

class Part {
public:
    Part(Vec3<float> pos,float length,float width,float height);
    float get_width();
    float get_length();
    float get_height();
    Vec3<float> get_center();
    int get_vertices_number();
    int get_index_number();
    std::vector<float> get_vertices_array();
    std::vector<int> get_index_array(int start_index,Draw_Modes mode);

private:
    std::vector< Vec3<float> > vertices;
    Vec3<float> pos;
    float height;
    float length;
    float width;
};

#endif
