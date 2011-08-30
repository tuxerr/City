#include "octree.h"

Octree::Octree(Vec3<float> pos,Vec3<float> size,Octree *parent) : center_position(pos), size(size), parent(parent) {
    for(int i=0;i<8;i++) {
        nodes[i]=NULL;
    }

}

Octree::~Octree() {
    for(int i=0;i<8;i++) {
        if(nodes[i]!=NULL) {
            delete nodes[i];
        }
    }
}

void Octree::add_object(Object *o) {
    if(contains_object(o)) {
        // if the object is in our current octree

        if(size.x*2<OCTREE_PRECISION) {
            objects.push_back(o);
            std::cout<<"object was added in ";
            print();
        } else {

            bool all_contain=true;
            for(int i=0;i<8;i++) {
                Octree node = generate_node((Octree_Nodes)i);
                if(!node.contains_object(o)) {
                    all_contain=false;
                    break;
                }
            }
        
            if(all_contain) {
                objects.push_back(o);
                std::cout<<"object was added in";
                print();
            } else {
                for(int i=0;i<8;i++) {
                    if(generate_node((Octree_Nodes)i).contains_object(o)) {
                        Octree *node = new Octree(Vec3<float>(0,0,0),Vec3<float>(0,0,0));
                        (*node)=generate_node((Octree_Nodes)i);
                        nodes[i]=node;
                        std::cout<<"recursive pattern "<<i<<" : ";
                        print();
                        node->add_object(o);
                        break;
                    }
                }
            }
        }
        
    }

}

bool Octree::contains_object(Object *o) {
    Vec3<float> objectpos = o->position();
    std::cout<<"Object position : "<<objectpos.x<<objectpos.y<<objectpos.z<<std::endl;
    float bsize = o->bounding_size();
    Vec3<float> center_vector = center_position-objectpos;
    center_vector.normalize();
    std::cout<<"Center vector : "<<center_vector.x<<":"<<center_vector.y<<":"<<center_vector.z<<std::endl;
    float scalar = center_vector.scalar(Vec3<float>(0,0,1));
    scalar=fabs(scalar);
    std::cout<<"scalar : "<<scalar<<std::endl;
    if(scalar<0.707) {
        float angle = acos(scalar);
        std::cout<<"angle : "<<angle<<std::endl;
        scalar=fabs(sin(angle));
        std::cout<<"scalar : "<<scalar<<std::endl;
    }

    float center_side_cube_size = size.x/scalar;
    std::cout<<"cube size :"<<(center_position-objectpos).norm()<<std::endl;

    return (center_position-objectpos).norm()<center_side_cube_size+bsize;
}

Octree Octree::generate_node(Octree_Nodes node) {
    Vec3<float> newsize(size.x/2,size.y/2,size.z/2);
    switch(node) {
    case TOP_XY:
        return Octree(Vec3<float>(center_position.x+newsize.x,center_position.y+newsize.y,center_position.z+newsize.z),newsize,this);
        break;

    case TOP_XMY:
        return Octree(Vec3<float>(center_position.x+newsize.x,center_position.y-newsize.y,center_position.z+newsize.z),newsize,this);
        break;

    case TOP_MXMY:
        return Octree(Vec3<float>(center_position.x-newsize.x,center_position.y-newsize.y,center_position.z+newsize.z),newsize,this);
        break;

    case TOP_MXY:
        return Octree(Vec3<float>(center_position.x-newsize.x,center_position.y+newsize.y,center_position.z+newsize.z),newsize,this);
        break;

    case BOT_XY:
        return Octree(Vec3<float>(center_position.x+newsize.x,center_position.y+newsize.y,center_position.z-newsize.z),newsize,this);
        break;

    case BOT_XMY:
        return Octree(Vec3<float>(center_position.x+newsize.x,center_position.y-newsize.y,center_position.z-newsize.z),newsize,this);
        break;

    case BOT_MXMY:
        return Octree(Vec3<float>(center_position.x-newsize.x,center_position.y-newsize.y,center_position.z-newsize.z),newsize,this);
        break;

    case BOT_MXY:
        return Octree(Vec3<float>(center_position.x-newsize.x,center_position.y+newsize.y,center_position.z-newsize.z),newsize,this);
        break;

    default:
        return Octree(Vec3<float>(0,0,0),Vec3<float>(0,0,0),this);
        break;
    }

}

void Octree::print() {
    std::cout<<"Octree center : "<<center_position.x<<"/"<<center_position.y<<"/"<<center_position.z<<"!! Octree size : "<<size.x<<"/"<<size.y<<"/"<<size.z<<std::endl;
}

