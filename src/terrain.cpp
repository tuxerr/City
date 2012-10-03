#include "terrain.hpp"

Terrain::Terrain(PerlinNoise *noise) : scalar(Vec3<float>(1,1,1)), noise(noise) {
    
}

void Terrain::scale(float x,float y,float z) {
    scalar.x = x;
    scalar.y = y;
    scalar.z = z;
}

float Terrain::height(float x,float y) {
    return scalar.z*noise->GetHeight(x/scalar.x,y/scalar.y);
}

Terrain_Data Terrain::generate_terrain(Vec2<float> coord,float length) {
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
            Vec3<float> res = n1.cross(n2);
            if(res.z<0) {
                res=res*-1;
            }

            res.normalize();

            float pente = res.scalar(Vec3<float>(0,0,1));
                
            color_matrix.push_back(mixvec(Vec3<float>(0.35,0.23,0.06),
                                          mixvec(Vec3<float>(0,1,0),Vec3<float>(1,1,1),cz/2),
                                          pente));
/*                if(pente<0.65) {
                  color_matrix.push_back(Vec3<float>(0.35,0.23,0.06));
                  } else if(cz>0.7) {
                  color_matrix.push_back(Vec3<float>(1,1,1));
                  }*/

            normal_matrix.push_back(res);
            geodata[i

        }
    }
    
    Texture *geotex = new Texture(TERRAIN_TEX_RESOLUTION,TERRAIN_TEX_RESOLUTION,TEXTURE_RGBA);
    Texture *colortex = new Texture(TERRAIN_TEX_RESOLUTION,TERRAIN_TEX_RESOLUTION,TEXTURE_RGBA);
    
    
    return return_data;
}



