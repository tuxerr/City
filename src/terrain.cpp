#include "terrain.hpp"

Terrain::Terrain(PerlinNoise *noise) : scalar(Vec3<float>(1,1,1)), noise(noise) {
    Logger::log()<<"Instanciated terrain : "<<(void*)this<<std::endl;
}

void Terrain::scale(float x,float y,float z) {
    scalar.x = x;
    scalar.y = y;
    scalar.z = z;
}

float Terrain::height(float x,float y) {
    return scalar.z*noise->GetHeight(x/scalar.x,y/scalar.y);
}

//Terrain_Data Terrain::generate_terrain(Vec2<float> coord,float length) {
void Terrain::generate_terrain() {
    float length=400;
    Vec2<float> coord(0,0);

    std::cout<<"lol2"<<std::endl;
    Logger::log()<<"Generating a "<<TERRAIN_TEX_RESOLUTION<<" * "<<TERRAIN_TEX_RESOLUTION<<" terrain of length "<<length<<std::endl;

    Terrain_Data return_data;

    float geodata[TERRAIN_TEX_RESOLUTION*TERRAIN_TEX_RESOLUTION*4]; // careful, high RAM usage
    float colordata[TERRAIN_TEX_RESOLUTION*TERRAIN_TEX_RESOLUTION*4]; // careful, high RAM usage

    float precision = length/(TERRAIN_TEX_RESOLUTION-1);

    for(int i=0;i<TERRAIN_TEX_RESOLUTION;i++) {
        for(int j=0;j<TERRAIN_TEX_RESOLUTION;j++) {

            float cx=i*precision,cy=j*precision;
            float cz=height(cx+coord.x,cy+coord.y);

            Vec3<float> n1(EPS,0,height(cx+coord.x+EPS,cy+coord.y)-cz);
            Vec3<float> n2(0,EPS,height(cx+coord.x,cy+coord.y+EPS)-cz);
            n1=n1*10;
            n2=n2*10;
            Vec3<float> normal = n1.cross(n2);
            if(normal.z<0) {
                normal=normal*-1;
            }

            normal.normalize();

            float pente = normal.scalar(Vec3<float>(0,0,1));
                
            Vec3<float> color = mixvec(Vec3<float>(0.35,0.23,0.06),
                                          mixvec(Vec3<float>(0,1,0),Vec3<float>(1,1,1),cz/2),
                                       pente);

            geodata[i*TERRAIN_TEX_RESOLUTION+j]=normal.x;
            geodata[i*TERRAIN_TEX_RESOLUTION+j+1]=normal.y;
            geodata[i*TERRAIN_TEX_RESOLUTION+j+2]=normal.z;
            geodata[i*TERRAIN_TEX_RESOLUTION+j+3]=cz;

            colordata[i*TERRAIN_TEX_RESOLUTION+j]=color.x;
            colordata[i*TERRAIN_TEX_RESOLUTION+j+1]=color.y;
            colordata[i*TERRAIN_TEX_RESOLUTION+j+2]=color.z;
            colordata[i*TERRAIN_TEX_RESOLUTION+j+3]=cz;

        }
    }
    
    Texture *geotex = new Texture(TERRAIN_TEX_RESOLUTION,TERRAIN_TEX_RESOLUTION,TEXTURE_RGBA);
    Texture *colortex = new Texture(TERRAIN_TEX_RESOLUTION,TERRAIN_TEX_RESOLUTION,TEXTURE_RGBA);
    
    geotex->set_data(geodata);
    colortex->set_data(colordata);

    return_data.geometry_tex=geotex;
    return_data.color_tex=colortex;
    return_data.init_coord=coord;
    return_data.length=length;
    
//    return return_data;
}

void Terrain::generate_patches(float length, Object *o) {
    int patch_number = TERRAIN_TEX_RESOLUTION/8; // a patch can be divided up to 64 times, so 8*8 times
    float patch_length = length/patch_number;
    Logger::log()<<"Generating "<<patch_number<<" * "<<patch_number<<" quad patches of individual length "<<patch_length<<std::endl;

    std::vector<Vec3<float>> vertices;
    std::vector<int> quad_index;
    vertices.reserve(patch_number*patch_number);
    quad_index.reserve(patch_number*patch_number*4);
    

    for(int i=0;i<=patch_number;i++) {
        for(int j=0;j<=patch_number;j++) {
            vertices.push_back(Vec3<float>(i*patch_length,j*patch_length,0));
        }   
    }

    for(int i=0;i<patch_number;i++) {
        for(int j=0;j<patch_number;j++) {
            quad_index.push_back(i*patch_number+j);
            quad_index.push_back((i+1)*patch_number+j);
            quad_index.push_back((i+1)*patch_number+j+1);
            quad_index.push_back(i*patch_number+j+1);
        }
    }

    o->set_draw_mode(OBJECT_DRAW_PATCHES);
}



