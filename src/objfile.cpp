#include "objfile.hpp"

ObjFile::ObjFile(std::string path) : path(path), part_number(0), isclosed(false) {
    file.open(path.c_str(),ios::in);
    if(!file) {
        Logger::log()<<".obj file "<<path<<" couldn't be opened"<<endl;
    } else {
        load();
    }
}

ObjFile::~ObjFile() {
    if(!isclosed) {
        reset();
        file.close();
    }
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

void ObjFile::close() {
    reset();
    file.close();
    isclosed=true;
}

void ObjFile::load() {
    reset();

    int act_part=0;
    int total_vertices=0;
    bool first_mtl=true;
  
    vbo_vertices.resize(1);
    vbo_normals.resize(1);
    vbo_texcoord.resize(1);
    vert_indexes.resize(1);
    tri_index.resize(1);
    line_index.resize(1);

    while(!file.eof()) {
        string start;
        file>>start;
        if(start=="") {
            file.get();
        }

        char line[200]; file.getline(line,200);
        stringstream sstr(stringstream::in | stringstream::out);
        sstr<<line;

        if(start=="v") {
            float coord;
            
            sstr>>coord;
            vertices.push_back(coord);
            sstr>>coord;
            vertices.push_back(coord);
            sstr>>coord;
            vertices.push_back(coord);
            
        } else if(start=="vn") {
            float coord;
            
            sstr>>coord;
            normals.push_back(coord);
            sstr>>coord;
            normals.push_back(coord);
            sstr>>coord;
            normals.push_back(coord);
            
        } else if(start=="vt") {
            float coord;
            
            sstr>>coord;
            texcoord.push_back(coord);

            sstr>>coord;
            texcoord.push_back(coord);

            coord=0;
            sstr>>coord;
            texcoord.push_back(coord);
        } else if(start=="f") {
            string str[4];
            ObjData data[4];
            
            sstr>>str[0]>>str[1]>>str[2]>>str[3];

            for(int i=0;i<4;i++) { // load data structure
                data[i]=parse_face_string(str[i]);

                if(vert_indexes[act_part].find(data[i])==vert_indexes[act_part].end() && data[i].pos!=-1) { 
                     // if the vertex/texture/normal combination was never used before (and is a real one, not the 4th component of a triangle face), add it into the VBOs
                    total_vertices++;

                    vbo_vertices[act_part].push_back(vertices[(data[i].pos-1)*3]);
                    vbo_vertices[act_part].push_back(vertices[((data[i].pos-1)*3)+1]);
                    vbo_vertices[act_part].push_back(vertices[((data[i].pos-1)*3)+2]);

                    if(data[i].texcoord!=-1) {
                        vbo_texcoord[act_part].push_back(texcoord[(data[i].texcoord-1)*3]);
                        vbo_texcoord[act_part].push_back(texcoord[((data[i].texcoord-1)*3)+1]);
                        vbo_texcoord[act_part].push_back(texcoord[((data[i].texcoord-1)*3)+2]);
                    } else {
                        vbo_texcoord[act_part].push_back(0);
                        vbo_texcoord[act_part].push_back(0);
                        vbo_texcoord[act_part].push_back(0);
                    }

                    if(data[i].normal!=-1) {
                        vbo_normals[act_part].push_back(normals[(data[i].normal-1)*3]);
                        vbo_normals[act_part].push_back(normals[((data[i].normal-1)*3)+1]);
                        vbo_normals[act_part].push_back(normals[((data[i].normal-1)*3)+2]);
                    } else {
                        vbo_normals[act_part].push_back(0);
                        vbo_normals[act_part].push_back(0);
                        vbo_normals[act_part].push_back(0);
                    }
                    
                    vert_indexes[act_part].insert(pair<ObjData,int>(data[i],vbo_normals[act_part].size()/3-1));
                } 
            }
            
            tri_index[act_part].push_back(vert_indexes[act_part][data[0]]);
            tri_index[act_part].push_back(vert_indexes[act_part][data[1]]);
            tri_index[act_part].push_back(vert_indexes[act_part][data[2]]);

            line_index[act_part].push_back(vert_indexes[act_part][data[0]]);
            line_index[act_part].push_back(vert_indexes[act_part][data[1]]);

            line_index[act_part].push_back(vert_indexes[act_part][data[1]]);
            line_index[act_part].push_back(vert_indexes[act_part][data[2]]);

            if(data[3].pos!=-1) {
                tri_index[act_part].push_back(vert_indexes[act_part][data[0]]);
                tri_index[act_part].push_back(vert_indexes[act_part][data[2]]);
                tri_index[act_part].push_back(vert_indexes[act_part][data[3]]);

                line_index[act_part].push_back(vert_indexes[act_part][data[2]]);
                line_index[act_part].push_back(vert_indexes[act_part][data[3]]);

                line_index[act_part].push_back(vert_indexes[act_part][data[3]]);
                line_index[act_part].push_back(vert_indexes[act_part][data[0]]);
            } else {
                line_index[act_part].push_back(vert_indexes[act_part][data[2]]);
                line_index[act_part].push_back(vert_indexes[act_part][data[0]]);
            }
        } else if(start=="usemtl") {
            if(first_mtl) {
                first_mtl=false;
            } else {
                act_part++;
                vbo_vertices.resize(act_part+1);
                vbo_normals.resize(act_part+1);
                vbo_texcoord.resize(act_part+1);
                vert_indexes.resize(act_part+1);
                tri_index.resize(act_part+1);
                line_index.resize(act_part+1);
            }
        }
    }

    Logger::log()<<"Total OpenGL vertices in the .obj : "<<total_vertices<<endl;
    part_number=act_part;
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

void ObjFile::load_in_object(Object *o) {

    for(int i=0;i<=part_number;i++) {
        
        vector<float> colors=vbo_vertices[i];
        colors.assign(colors.size(),0.8);
        
        if(i!=0) {
            o->new_part();
        }

        o->update_vertices_buffer(&vbo_vertices[i][0],vbo_vertices[i].size()*sizeof(float),i);
        o->update_normals_buffer(&vbo_normals[i][0],vbo_normals[i].size()*sizeof(float),i);
        o->update_texture_buffer(&vbo_texcoord[i][0],vbo_texcoord[i].size()*sizeof(float),i);
        o->update_lines_index_buffer(&line_index[i][0],line_index[i].size()*sizeof(int),i);
        o->update_triangles_index_buffer(&tri_index[i][0],tri_index[i].size()*sizeof(int),i);
        o->update_color_buffer(&colors[0],colors.size()*sizeof(float),i);

    }

}
