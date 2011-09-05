#include "octree.h"

Octree::Octree(Vec3<float> pos,Vec3<float> size,Octree *parent) : center_position(pos), size(size), parent(parent) {
    for(int i=0;i<8;i++) {
        nodes[i]=NULL;
    }
    objects.clear();

}

Octree::~Octree() {
    for(int i=0;i<8;i++) {
        if(nodes[i]!=NULL) {
            delete nodes[i];
            nodes[i]=NULL;
        }
    }
}

void Octree::add_object(Object *o) {
    if(contains_object(o)) {
        // if the object is in our current octree

        if(size.x*2<=OCTREE_PRECISION) {
            objects.push_back(o);
        } else {
            for(int i=0;i<8;i++) {
                if(generate_node((Octree_Nodes)i).contains_object(o)) {
                    if(nodes[i]==NULL) {
                        Octree *node = new Octree(Vec3<float>(0,0,0),Vec3<float>(0,0,0));
                        (*node)=generate_node((Octree_Nodes)i);
                        nodes[i]=node;
                    }
                    nodes[i]->add_object(o);
                }
            }
        }
        
    }

}

Octree_Collisions Octree::frustum_collision(Frustum &frust) {
    float boundingradius = size.norm();
    float distance[6];
    int distmax;

    if(frust.ortho_frustum) {
        distance[0]=((center_position-frust.farpoint).scalar(frust.normal[0]));
        distance[1]=((center_position-frust.origin).scalar(frust.normal[1]));
        distance[2]=((center_position-(frust.origin+frust.ortho_right_vec)).scalar(frust.normal[2]));
        distance[3]=((center_position-(frust.origin-frust.ortho_right_vec)).scalar(frust.normal[3]));
        distance[4]=((center_position-(frust.origin+frust.ortho_up_vec)).scalar(frust.normal[4]));
        distance[5]=((center_position-(frust.origin-frust.ortho_up_vec)).scalar(frust.normal[5]));
        distmax=6;
    } else {
        distance[0]=((center_position-frust.farpoint).scalar(frust.normal[0]));
        distance[1]=((center_position-frust.origin).scalar(frust.normal[1]));
        distance[2]=((center_position-frust.origin).scalar(frust.normal[2]));
        distance[3]=((center_position-frust.origin).scalar(frust.normal[3]));
        distance[4]=((center_position-frust.origin).scalar(frust.normal[4]));
        distmax=5;
    }

    bool fullin=true;
    bool in=true;

    for(int i=0;i<distmax;i++) {
        if(distance[i]+boundingradius>0) {
            fullin=false;
        }
        if(distance[i]-boundingradius>0) {
            in=false;
        }
    }

    if(fullin) {
        return FULL_IN;
    } else if(in) {
        return IN;
    } else {
        return OUT;
    }

}

int Octree::delete_object(Object *o) {
    if(objects.size()!=0) {
        objects.remove(o);
        if(objects.size()==0) {
            return 1;           // this node has to be erased now (it's empty)
        } else {
            return 0;
        }
    } else {
        bool to_erase = true;
        for(int i=0;i<8;i++) {
            if(nodes[i]!=NULL) {
                if(nodes[i]->delete_object(o)) {
                    delete nodes[i];
                    nodes[i]=NULL;
                }
                if(nodes[i]!=NULL) {
                    to_erase=false;
                }
            }
        }
        return to_erase;
    }
}

bool Octree::contains_object(Object *o) {
    Vec3<float> object_pos = o->position();
    float object_bsize = o->bounding_size();

    Vec3<float> center_vector = center_position-object_pos;
    center_vector.normalize();

    float scalar = 
        maxf(
            maxf(fabs(center_vector.scalar(Vec3<float>(0,0,1))),
                 fabs(center_vector.scalar(Vec3<float>(1,0,0)))),
            fabs(center_vector.scalar(Vec3<float>(0,1,0))));
    
    if(scalar==0) {
        return true;
    }
    float center_side_cube_size = size.x/scalar;

    return (center_position-object_pos).norm()<center_side_cube_size+object_bsize;
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

