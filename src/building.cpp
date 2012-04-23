#include "building.hpp"

Building::Building(float height,Vec2<float> pos,Vec2<float> size) : 
    height(height), pos(pos), size(size)
{
    
}

Building::Building(float height,float posx,float posy,float sizex,float sizey) : 
    height(height), pos(posx,posy), size(sizex,sizey)
{    
    
}

void Building::create() {
    elements["central"].push_back( Part(Vec3<float>(pos.x,pos.y,0),size.x,size.y,2.0/3*height));
    elements["central/up"].push_back( Part(Vec3<float>(pos.x,pos.y,2.0/3*height),size.x/2,size.y/2,height/3));
}


