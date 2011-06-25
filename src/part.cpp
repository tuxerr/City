#include "part.h"

Part::Part(Vec3<float> pos,float length,float width,float height) : 
    pos(pos), height(height), length(length), width(width) 
{
    vertices.resize(8);
    vertices[0]=pos;
    vertices[1]=pos+Vec3<float>(length,0,0);
    vertices[2]=pos+Vec3<float>(length,width,0);
    vertices[3]=pos+Vec3<float>(0,width,0);
    vertices[4]=pos+Vec3<float>(0,0,height);
    vertices[5]=pos+Vec3<float>(length,0,height);
    vertices[6]=pos+Vec3<float>(length,width,height);
    vertices[7]=pos+Vec3<float>(0,width,height);
}

float Part::get_width() {
    return width;
}

float Part::get_length() {
    return length;
}

float Part::get_height() {
    return height;
}

Vec3<float> Part::get_center() {
    return pos+Vec3<float>(length/2,width/2,height/2);
}

int Part::get_vertices_number() {
    return vertices.size();
}

int Part::get_index_number() {
    // each vertex is bounded 3 times, and we don't use STRIP
    return get_vertices_number()*3;
}

std::vector<float> Part::get_vertices_array() {
    std::vector<float> res;

    for(unsigned int i=0;i<vertices.size();i++) {
        res.push_back(vertices[i].x);
        res.push_back(vertices[i].y);
        res.push_back(vertices[i].z);
    }
    
    return res;
}

std::vector<int> Part::get_index_array(int start_index,Draw_Modes mode) {
    std::vector<int> index;
    // for a 8-vertices quad
    if(mode==PART_LINES) {
        
        // under lines
        index.push_back(0); index.push_back(1);
        index.push_back(1); index.push_back(2);
        index.push_back(2); index.push_back(3);
        index.push_back(3); index.push_back(0);
        
        // roof lines
        index.push_back(4); index.push_back(5);
        index.push_back(5); index.push_back(6);
        index.push_back(6); index.push_back(7);
        index.push_back(7); index.push_back(4);

        // side lines
        index.push_back(0); index.push_back(4);
        index.push_back(1); index.push_back(5);
        index.push_back(2); index.push_back(6);
        index.push_back(3); index.push_back(7);

    } else if(mode==PART_QUADS) {
        // under face
        index.push_back(0); index.push_back(1); index.push_back(2); index.push_back(3);
        
        // top face
        index.push_back(4); index.push_back(5); index.push_back(6); index.push_back(7);

        // front
        index.push_back(0); index.push_back(1); index.push_back(5); index.push_back(4);

        // right
        index.push_back(1); index.push_back(2); index.push_back(6); index.push_back(5);

        // bottom
        index.push_back(2); index.push_back(3); index.push_back(7); index.push_back(6);

        // left
        index.push_back(3); index.push_back(0); index.push_back(4); index.push_back(7);
    }

    for(unsigned int i=0;i<index.size();i++) {
        index[i]+=start_index;
    }

    return index;
}
