#include "matrix4.h"

Matrix4::Matrix4() {
    clear();
}

Matrix4::Matrix4(float *val) {
    for(int i=0;i<16;i++) {
        this->val[i]=val[i];
    }
}

Matrix4::Matrix4(const Matrix4 &mat) {
    for(int i=0;i<16;i++) {
        this->val[i]=mat.val[i];
    }
}

void Matrix4::clear() {
    for(int i=0;i<16;i++) {
        val[i]=0;
    }
}

void Matrix4::identity() {
    clear();
    val[0]=1;
    val[5]=1;
    val[10]=1;
    val[15]=1;
}

void Matrix4::print() {
    std::cout<<val[0]<<" "<<val[1]<<" "<<val[2]<<" "<<val[3]<<"\n"
             <<val[4]<<" "<<val[5]<<" "<<val[6]<<" "<<val[7]<<"\n"
             <<val[8]<<" "<<val[9]<<" "<<val[10]<<" "<<val[11]<<"\n"
             <<val[12]<<" "<<val[13]<<" "<<val[14]<<" "<<val[15]<<"\n"<<std::endl;

}

float *Matrix4::adress() {
    return val;
}

Matrix4 Matrix4::operator*(const Matrix4 &mat) {
    Matrix4 res;
    for(int k = 0; k < 4; k++) {
        for(int j = 0; j < 4; j++) {
            for(int i = 0; i < 4; i++) {
                res.val[4*j+k] += val[4*j+i] * mat.val[4*i+k];
            }
        }
    }
    return res;
}

void Matrix4::translate(float x,float y,float z) {
    Matrix4 t;
    t.identity();
    t.val[3]=x;
    t.val[7]=y;
    t.val[11]=z;
    *this=Matrix4(val)*t;
}

void Matrix4::scale(float x,float y,float z) {
    Matrix4 s;
    s.identity();
    s.val[0]=x;
    s.val[5]=y;
    s.val[10]=z;
    *this=Matrix4(val)*s;
}


void Matrix4::rotate(float angle,float x, float y, float z) {
    Vec3<float> axe(x,y,z);
    axe.normalize();

    angle = angle*M_PI/180;

    Matrix4 rot; 

    rot.val[0]= axe.x*axe.x * (1 - cos(angle)) + cos(angle);
    rot.val[1]= axe.x*axe.y * (1 - cos(angle)) - axe.z*sin(angle);
    rot.val[2]= axe.x*axe.z * (1 - cos(angle)) + axe.y*sin(angle);

    rot.val[4]= axe.x*axe.y * (1 - cos(angle)) + axe.z*sin(angle);
    rot.val[5]= axe.y*axe.y * (1 - cos(angle)) + cos(angle);
    rot.val[6]= axe.y*axe.z * (1 - cos(angle)) - axe.x*sin(angle);

    rot.val[8]= axe.x*axe.z * (1 - cos(angle)) - axe.y*sin(angle);
    rot.val[9]= axe.y*axe.z * (1 - cos(angle)) + axe.x*sin(angle);
    rot.val[10]= axe.z*axe.z * (1 - cos(angle)) + cos(angle);

    rot.val[15]= 1.0;

    *this=Matrix4(val)*rot;
}
