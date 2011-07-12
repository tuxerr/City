#include "matrix4.h"

void switchvals(float &v1,float &v2) {
    float tmp=v1;
    v1=v2;
    v2=tmp;
}

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

void Matrix4::transpose() {
    switchvals(val[1],val[4]);
    switchvals(val[2],val[8]);
    switchvals(val[3],val[12]);
    switchvals(val[7],val[13]);
    switchvals(val[11],val[14]);
    switchvals(val[6],val[9]);
}

void Matrix4::invert() {
    float inv[16], det;
    int i;

    inv[0] =   val[5]*val[10]*val[15] - val[5]*val[11]*val[14] - val[9]*val[6]*val[15]
        + val[9]*val[7]*val[14] + val[13]*val[6]*val[11] - val[13]*val[7]*val[10];
    inv[4] =  -val[4]*val[10]*val[15] + val[4]*val[11]*val[14] + val[8]*val[6]*val[15]
        - val[8]*val[7]*val[14] - val[12]*val[6]*val[11] + val[12]*val[7]*val[10];
    inv[8] =   val[4]*val[9]*val[15] - val[4]*val[11]*val[13] - val[8]*val[5]*val[15]
        + val[8]*val[7]*val[13] + val[12]*val[5]*val[11] - val[12]*val[7]*val[9];
    inv[12] = -val[4]*val[9]*val[14] + val[4]*val[10]*val[13] + val[8]*val[5]*val[14]
        - val[8]*val[6]*val[13] - val[12]*val[5]*val[10] + val[12]*val[6]*val[9];
    inv[1] =  -val[1]*val[10]*val[15] + val[1]*val[11]*val[14] + val[9]*val[2]*val[15]
        - val[9]*val[3]*val[14] - val[13]*val[2]*val[11] + val[13]*val[3]*val[10];
    inv[5] =   val[0]*val[10]*val[15] - val[0]*val[11]*val[14] - val[8]*val[2]*val[15]
        + val[8]*val[3]*val[14] + val[12]*val[2]*val[11] - val[12]*val[3]*val[10];
    inv[9] =  -val[0]*val[9]*val[15] + val[0]*val[11]*val[13] + val[8]*val[1]*val[15]
        - val[8]*val[3]*val[13] - val[12]*val[1]*val[11] + val[12]*val[3]*val[9];
    inv[13] =  val[0]*val[9]*val[14] - val[0]*val[10]*val[13] - val[8]*val[1]*val[14]
        + val[8]*val[2]*val[13] + val[12]*val[1]*val[10] - val[12]*val[2]*val[9];
    inv[2] =   val[1]*val[6]*val[15] - val[1]*val[7]*val[14] - val[5]*val[2]*val[15]
        + val[5]*val[3]*val[14] + val[13]*val[2]*val[7] - val[13]*val[3]*val[6];
    inv[6] =  -val[0]*val[6]*val[15] + val[0]*val[7]*val[14] + val[4]*val[2]*val[15]
        - val[4]*val[3]*val[14] - val[12]*val[2]*val[7] + val[12]*val[3]*val[6];
    inv[10] =  val[0]*val[5]*val[15] - val[0]*val[7]*val[13] - val[4]*val[1]*val[15]
        + val[4]*val[3]*val[13] + val[12]*val[1]*val[7] - val[12]*val[3]*val[5];
    inv[14] = -val[0]*val[5]*val[14] + val[0]*val[6]*val[13] + val[4]*val[1]*val[14]
        - val[4]*val[2]*val[13] - val[12]*val[1]*val[6] + val[12]*val[2]*val[5];
    inv[3] =  -val[1]*val[6]*val[11] + val[1]*val[7]*val[10] + val[5]*val[2]*val[11]
        - val[5]*val[3]*val[10] - val[9]*val[2]*val[7] + val[9]*val[3]*val[6];
    inv[7] =   val[0]*val[6]*val[11] - val[0]*val[7]*val[10] - val[4]*val[2]*val[11]
        + val[4]*val[3]*val[10] + val[8]*val[2]*val[7] - val[8]*val[3]*val[6];
    inv[11] = -val[0]*val[5]*val[11] + val[0]*val[7]*val[9] + val[4]*val[1]*val[11]
        - val[4]*val[3]*val[9] - val[8]*val[1]*val[7] + val[8]*val[3]*val[5];
    inv[15] =  val[0]*val[5]*val[10] - val[0]*val[6]*val[9] - val[4]*val[1]*val[10]
        + val[4]*val[2]*val[9] + val[8]*val[1]*val[6] - val[8]*val[2]*val[5];

    det = val[0]*inv[0] + val[1]*inv[4] + val[2]*inv[8] + val[3]*inv[12];
    if (det == 0)
        std::cout<<"Matrix is not inversible"<<std::endl;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        val[i] = inv[i] * det;
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
