#ifndef DEF_BUILDING
#define DEF_BUILDING

#include <iostream>
#include <map>
#include "vec2.h"
#include "part.h"
#include "object.h"

class Building {
public:
    Building(float height,Vec2<float> pos,Vec2<float> size);
    Building(float height,float posx,float posy,float sizex,float sizey);
    void create();
    Object generate_object();

private:
    std::map<std::string, std::vector<Part> > elements;
    float height;
    Vec2<float> pos;
    Vec2<float> size;
};

#endif
