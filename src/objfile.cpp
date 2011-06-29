#include "objfile.h"

ObjFile::ObjFile(std::string path) : path(path) {
    file.open(path.c_str(),ios::in);
    if(!file) {
        cout<<".obj file "<<path<<" couldn't be opened"<<endl;
    }
}

ObjFile::~ObjFile() {
    file.close();
}

void ObjFile::reset() {
    file.seekg(0,ios::beg);
    vertices.clear();
    normals.clear();
    texcoord.clear();
    vbo_vertices.clear();
    vbo_normals.clear();
    vbo_texcoord.clear();
    vert_indexes.clear();
    tri_index.clear();
    line_index.clear();
}

void ObjFile::load_in_object(Object *o) {
    if(o==NULL) {
        cout<<"Object is NULL : cannot parse "<<path<<endl;
        return;
    }

    o->set_draw_mode(GL_TRIANGLES);
    reset();

    int act_part=0;
    int total_vertices=0;
    bool first_mtl=true;

    while(!file.eof()) {
        string start;
        file>>start;
        if(start=="v") {
            float coord;
            
            file>>coord;
            vertices.push_back(coord);
            file>>coord;
            vertices.push_back(coord);
            file>>coord;
            vertices.push_back(coord);
            
        } else if(start=="vn") {
            float coord;
            
            file>>coord;
            normals.push_back(coord);
            file>>coord;
            normals.push_back(coord);
            file>>coord;
            normals.push_back(coord);
            
        } else if(start=="vt") {
            float coord;
            
            file>>coord;
            texcoord.push_back(coord);
            file>>coord;
            texcoord.push_back(coord);
            file>>coord;
            texcoord.push_back(coord);

        } else if(start=="f") {
            string str[4];
            ObjData data[4];
            file>>str[0]>>str[1]>>str[2];
            if(file.peek()!='\n') {
                file>>str[3];
            }

            for(int i=0;i<4;i++) { // load data structure
                data[i]=parse_face_string(str[i]);
                if(vert_indexes.find(data[i])==vert_indexes.end()) {
                    total_vertices++;

                    vbo_vertices.push_back(vertices[(data[i].pos-1)*3]);
                    vbo_vertices.push_back(vertices[((data[i].pos-1)*3)+1]);
                    vbo_vertices.push_back(vertices[((data[i].pos-1)*3)+2]);

                    if(data[i].texcoord!=-1) {
                        vbo_texcoord.push_back(texcoord[(data[i].texcoord-1)*3]);
                        vbo_texcoord.push_back(texcoord[((data[i].texcoord-1)*3)+1]);
                        vbo_texcoord.push_back(texcoord[((data[i].texcoord-1)*3)+2]);
                    } else {
                        vbo_texcoord.push_back(0);
                        vbo_texcoord.push_back(0);
                        vbo_texcoord.push_back(0);
                    }

                    if(data[i].normal!=-1) {
                        vbo_normals.push_back(normals[(data[i].normal-1)*3]);
                        vbo_normals.push_back(normals[((data[i].normal-1)*3)+1]);
                        vbo_normals.push_back(normals[((data[i].normal-1)*3)+2]);
                    } else {
                        vbo_normals.push_back(0);
                        vbo_normals.push_back(0);
                        vbo_normals.push_back(0);
                    }
                    
                    vert_indexes.insert(pair<ObjData,int>(data[i],vbo_normals.size()/3-1));
                } 
            }
            
            tri_index.push_back(vert_indexes[data[0]]);
            tri_index.push_back(vert_indexes[data[1]]);
            tri_index.push_back(vert_indexes[data[2]]);

            line_index.push_back(vert_indexes[data[0]]);
            line_index.push_back(vert_indexes[data[1]]);

            line_index.push_back(vert_indexes[data[1]]);
            line_index.push_back(vert_indexes[data[2]]);

            if(data[3].pos!=-1) {
                tri_index.push_back(vert_indexes[data[0]]);
                tri_index.push_back(vert_indexes[data[2]]);
                tri_index.push_back(vert_indexes[data[3]]);

                line_index.push_back(vert_indexes[data[2]]);
                line_index.push_back(vert_indexes[data[3]]);

                line_index.push_back(vert_indexes[data[3]]);
                line_index.push_back(vert_indexes[data[0]]);
            } else {
                line_index.push_back(vert_indexes[data[2]]);
                line_index.push_back(vert_indexes[data[0]]);
            }

        } else if(start=="usemtl") {
            if(first_mtl) {
                first_mtl=false;
            } else {
                save_current_data_in_object(o,act_part);
                act_part=o->new_part();
            }

        }
    }
    save_current_data_in_object(o,act_part);
    cout<<"Total OpenGL vertices in the .obj : "<<total_vertices<<endl;
}

ObjData ObjFile::parse_face_string(string substr) {
    ObjData data;
    char tmp;

    data.pos=-1; data.texcoord=-1; data.normal=-1;
    if(substr!="") {
        size_t fs,ss; // first slash, second slash
        stringstream sstr(stringstream::in | stringstream::out);
        sstr<<substr;
                    
        fs=substr.find('/');
        ss=substr.find_last_of('/');
        if(fs==string::npos) { // no slash : only vertices data
            sstr>>data.pos;
        } else if(ss==string::npos) { // only one slash : pos/texture
            sstr>>data.pos;
            sstr>>tmp;
            sstr>>data.texcoord;
        } else if(ss==fs+1) { // two slashes next to each other : no texture data
            sstr>>data.pos;
            sstr>>tmp;
            sstr>>tmp;
            sstr>>data.normal;
        } else { // none of the above : 3 indexes
            sstr>>data.pos;
            sstr>>tmp;
            sstr>>data.texcoord;
            sstr>>tmp;
            sstr>>data.normal;
        }
    }    
    return data;
}

void ObjFile::save_current_data_in_object(Object *o,int part) {
    vector<float> colors=vbo_vertices;
    colors.assign(colors.size(),0.8);

    o->update_vertices_buffer(&vbo_vertices[0],vbo_vertices.size()*sizeof(float),part);
    o->update_normals_buffer(&vbo_normals[0],vbo_normals.size()*sizeof(float),part);
    o->update_texture_buffer(&vbo_texcoord[0],vbo_texcoord.size()*sizeof(float),part);
    o->update_lines_index_buffer(&line_index[0],line_index.size()*sizeof(int),part);
    o->update_triangles_index_buffer(&tri_index[0],tri_index.size()*sizeof(int),part);
    o->update_color_buffer(&colors[0],colors.size()*sizeof(float),part);
    
    vbo_vertices.clear();    
    vbo_normals.clear();     
    vbo_texcoord.clear();
    vert_indexes.clear();
    tri_index.clear();
    line_index.clear();
}
