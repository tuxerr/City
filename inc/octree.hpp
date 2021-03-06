#ifndef DEF_OCTREE
#define DEF_OCTREE

#include <iostream>
#include <list>
#include "vec3.hpp"
#include "vec2.hpp"
#include "constants.hpp"
#include "object.hpp"
#include "utils.hpp"
#include "frustum.hpp"

typedef enum {
    OUT, IN, FULL_IN
} Octree_Collisions;

typedef enum {
    TOP_XY=0, TOP_XMY=1, TOP_MXMY=2, TOP_MXY=3,
    BOT_XY=4, BOT_XMY=5, BOT_MXMY=6, BOT_MXY=7,
} Octree_Nodes;

class Octree {
public:
    Octree(Vec3<float> pos,Vec3<float> size,Octree *parent=NULL);
    ~Octree();
    void add_object(Object *o);
    int delete_object(Object *o);
    Octree_Collisions frustum_collision(Frustum &frust);
    void print();

    Vec3<float> center_position;
    Vec3<float> size;
    Octree *nodes[8];
    Octree *parent;
    std::list<Object*> objects;

private:
    bool contains_object(Object *o);
    Octree generate_node(Octree_Nodes node);
};

#endif
