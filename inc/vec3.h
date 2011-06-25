#ifndef DEF_VEC3
#define DEF_VEC3

#include <math.h>

template <class number>
class Vec3 {
public:
    Vec3(number =0,number =0,number =0);
    number x;
    number y;
    number z;
    number norm();
    void normalize();
    Vec3<number> operator+(Vec3<number> n);
    Vec3<number> operator-(Vec3<number> n);
    Vec3<number> operator*(Vec3<number> n);
private:
};

template <class number>
Vec3<number>::Vec3(number x,number y,number z) : x(x), y(y), z(z) {
}

template <class number>
number Vec3<number>::norm() {
    return sqrt(x*x+y*y+z*z);
}

template <class number>
void Vec3<number>::normalize() {
    number val=norm();
    if(val!=0) {
        x/=val;
        y/=val;
        z/=val;
    }
}

template <class number>
Vec3<number> Vec3<number>::operator+(Vec3<number> n) {
    n.x+=x;
    n.y+=y;
    n.z+=z;
    return n;
}

template <class number>
Vec3<number> Vec3<number>::operator-(Vec3<number> n) {
    Vec3<number> res;
    res.x=x-n.x;
    res.y=y-n.y;
    res.z=z-n.z;
    return res;
}

// vectorial product
template <class number>
Vec3<number> Vec3<number>::operator*(Vec3<number> n) {
    Vec3<number> res;
    res.x=(y*n.z-z*n.y);
    res.y=(z*n.x-x*n.z);
    res.z=(x*n.y-y*n.x);
    return res;
}

#endif
