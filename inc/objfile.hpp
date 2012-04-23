#ifndef DEF_OBJFILE
#define DEF_OBJFILE

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "object.hpp"

using namespace std;

typedef struct ObjData {        /* contains vertice data used in f lines */
    int pos;
    int texcoord;
    int normal;
    
    bool operator<( const ObjData & n ) const { /* for map usages */
        return (pos<n.pos 
                || (pos==n.pos && texcoord<n.texcoord)
                || (pos==n.pos && texcoord==n.texcoord && normal<n.normal));
    }
} ObjData;

class ObjFile {
public:
    ObjFile(string path);
    ~ObjFile();
    void load();
    void close();
    void load_in_object(Object *o);
    void reset();

private:
    ObjData parse_face_string(string substr);
    void save_current_data_in_object(Object *o,int part);
    string path;
    int part_number;
    bool isclosed;

    fstream file;

    vector<float> vertices; // these vectors contain v,vn,vt data as written in the .obj
    vector<float> normals;
    vector<float> texcoord;

    vector< vector<float> > vbo_vertices; // these vectors are meant to be sent to the vbos (same size/
    vector< vector<float> > vbo_normals;  // ordering)
    vector< vector<float> > vbo_texcoord;

    vector< map<ObjData,int> > vert_indexes; // obj vertices indexes to put in index
    vector< vector<int> > tri_index;  // .obj final rendering index for GL_TRIANGLES visualization
    vector< vector<int> > line_index;  // .obj final rendering index for GL_LINES visualization
};

#endif
