#include "building.h"

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

Object Building::generate_object() {
    std::vector<float> vertices;
    std::vector<int> index_lines;
    std::vector<int> index_quads;

    // creating the 3 vectors
    for(std::map<std::string,std::vector<Part> >::iterator it=elements.begin();
        it!=elements.end(); it++) {

        // going through each part in each building section
        for(unsigned int i=0;i<(it->second).size();i++) {

            std::vector<float> vert_tmp=((it->second)[i]).get_vertices_array();
            std::vector<int> index_lines_tmp=((it->second)[i]).get_index_array(vertices.size()/3,PART_LINES);
            std::vector<int> index_quads_tmp=((it->second)[i]).get_index_array(vertices.size()/3,PART_QUADS);

            vertices.insert(vertices.end(),vert_tmp.begin(),vert_tmp.end());
            index_lines.insert(index_lines.end(),index_lines_tmp.begin(),index_lines_tmp.end());
            index_quads.insert(index_quads.end(),index_quads_tmp.begin(),index_quads_tmp.end());

        }
    }

    // generating Object
    Object o;
    o.update_vertices_buffer(&vertices[0],vertices.size()*sizeof(vertices[0]));
    o.update_lines_index_buffer(&index_lines[0],index_lines.size()*sizeof(index_lines[0]));
    o.update_quads_index_buffer(&index_quads[0],index_quads.size()*sizeof(index_quads[0]));
    return o;

}
