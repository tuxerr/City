#include "terrain.h"

Terrain::Terrain(float (*height_func)(float,float),float precision) : height_func(height_func), precision(precision) {
    
}

void Terrain::generate_terrain(Vec2<float> coord,float height,float width,Object *object) {
    if(object==NULL) {
        std::cout<<"Could not generate terrain: object is NULL"<<std::endl;
        return;
    }
    int obj_part = object->new_part();
    int matrix_height = (height+precision)/precision;
    int matrix_width = (width+precision)/precision;
    
    std::vector< Vec3<float> > pos_matrix;
    std::vector< Vec3<float> > normal_matrix;    
    std::vector< Vec3<float> > color_matrix;
    std::vector< int > line_index_buffer;
    std::vector< int > triangles_index_buffer;
    pos_matrix.reserve(matrix_width*matrix_height);
    color_matrix.reserve(matrix_width*matrix_height);
    normal_matrix.reserve(matrix_width*matrix_height);
    triangles_index_buffer.reserve((matrix_width-1)*(matrix_height)*6);
    line_index_buffer.reserve((matrix_width-1)*(matrix_height)*6);
    for(int k=0;k<matrix_width*matrix_height;k++) {
        int i=k%matrix_width;
        int j=(k-i)/matrix_width;

        int cx=coord.x+i*precision,cy=coord.y+j*precision;

        pos_matrix.push_back(Vec3<float>(cx,cy,height_func(cx,cy)));
        color_matrix.push_back(Vec3<float>(1,1,1));

        normal_matrix.push_back(Vec3<float>(height_func(cx-EPS,cy)-height_func(cx+EPS,cy),
                                            2.0f*EPS,
                                            height_func(cx,cy-EPS)-height_func(cx,cy+EPS)));
        
    }

    fill_line_buffer(line_index_buffer,0,matrix_width,0,matrix_height,EDGE_FILL);
    fill_triangle_buffer(triangles_index_buffer,0,matrix_width,0,matrix_height,EDGE_FILL);

    object->update_vertices_buffer(&pos_matrix[0],pos_matrix.size()*sizeof(Vec3<float>),obj_part);
    object->update_normals_buffer(&normal_matrix[0],normal_matrix.size()*sizeof(Vec3<float>),obj_part);
    object->update_lines_index_buffer(&line_index_buffer[0],line_index_buffer.size()*sizeof(int),obj_part);
    object->update_triangles_index_buffer(&triangles_index_buffer[0],triangles_index_buffer.size()*sizeof(int),obj_part);
    object->update_color_buffer(&color_matrix[0],color_matrix.size()*sizeof(Vec3<float>),obj_part);
    
}

void fill_triangle_buffer(std::vector<int> &tri_buffer,int i,int imax,int j,int jmax,Fill_Modes mode) {
    // recursion §§§
    int pos = i+j*imax;
    switch(mode) {
    case SIDE_UP_FILL:
        if(i<imax-1 && j<jmax-1) {
            tri_buffer.push_back(pos);
            tri_buffer.push_back(pos+imax);
            tri_buffer.push_back(pos+imax+1);
            
            tri_buffer.push_back(pos);
            tri_buffer.push_back(pos+imax+1);
            tri_buffer.push_back(pos+1);
            
            fill_triangle_buffer(tri_buffer,i,imax,j+1,jmax,SIDE_UP_FILL);
        }
        break;

    case SIDE_RIGHT_FILL:
        if(i<imax-1 && j<jmax-1) {
            tri_buffer.push_back(pos);
            tri_buffer.push_back(pos+1);
            tri_buffer.push_back(pos+imax+1);

            if(i!=imax-1) {
                tri_buffer.push_back(pos);
                tri_buffer.push_back(pos+imax+1);
                tri_buffer.push_back(pos+imax);
            }
            fill_triangle_buffer(tri_buffer,i+1,imax,j,jmax,SIDE_RIGHT_FILL);
        }
        break;

    case EDGE_FILL:
        fill_triangle_buffer(tri_buffer,i,imax,j,jmax,SIDE_UP_FILL);
        fill_triangle_buffer(tri_buffer,i,imax,j,jmax,SIDE_RIGHT_FILL);
        if(i<imax-1 && j<jmax-1) {
            fill_triangle_buffer(tri_buffer,i+1,imax,j+1,jmax,EDGE_FILL);
        }
        break;

    }
}

void fill_line_buffer(std::vector<int> &line_buffer,int i,int imax,int j,int jmax,Fill_Modes mode) {
    int pos = i+j*imax;
    switch(mode) {
    case SIDE_UP_FILL:
        if(i<imax-1 && j<jmax-1) {
            line_buffer.push_back(pos);
            line_buffer.push_back(pos+imax);
            
            line_buffer.push_back(pos+imax);
            line_buffer.push_back(pos+imax+1);
            
            line_buffer.push_back(pos);
            line_buffer.push_back(pos+imax+1);
            
            fill_line_buffer(line_buffer,i,imax,j+1,jmax,SIDE_UP_FILL);
        }
        break;

    case SIDE_RIGHT_FILL:
        if(i<imax-1 && j<jmax-1) {
            line_buffer.push_back(pos);
            line_buffer.push_back(pos+1);

            line_buffer.push_back(pos+1);
            line_buffer.push_back(pos+imax+1);

            if(i!=imax-1) {
                line_buffer.push_back(pos);
                line_buffer.push_back(pos+imax+1);
            }
            fill_line_buffer(line_buffer,i+1,imax,j,jmax,SIDE_RIGHT_FILL);
        }
        break;

    case EDGE_FILL:
        fill_line_buffer(line_buffer,i,imax,j,jmax,SIDE_UP_FILL);
        fill_line_buffer(line_buffer,i,imax,j,jmax,SIDE_RIGHT_FILL);
        if(i<imax-1 && j<jmax-1) {
            fill_line_buffer(line_buffer,i+1,imax,j+1,jmax,EDGE_FILL);
        }
        break;

    }
}

