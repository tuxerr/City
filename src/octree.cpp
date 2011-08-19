#include "octree.h"

Octree::Octree(Vec3<float> pos,Vec3<float> size,Octree *parent) : center_position(pos), size(size), parent(parent) {
    
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
        } else {

            bool all_contain=true;
            for(int i=0;i<8;i++) {
                if(!generate_node((Octree_Nodes)i).contains_object(o)) {
                    all_contain=false;
                    break;
                }
            }
        
            if(all_contain) {
                objects.push_back(o);
            } else {
                for(int i=0;i<8;i++) {
                    if(generate_node((Octree_Nodes)i).contains_object(o)) {
                        Octree *node = new Octree(Vec3<float>(0,0,0),Vec3<float>(0,0,0));
                        (*node)=generate_node((Octree_Nodes)i);
                        nodes[i]=node;
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
    float bsize = o->bounding_size();
    return (fabs(objectpos.x-center_position.x)<(size.x+bsize) && 
            fabs(objectpos.y-center_position.y)<(size.y+bsize) && 
            fabs(objectpos.z-center_position.z)<(size.z+bsize));
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
