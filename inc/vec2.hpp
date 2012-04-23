#ifndef DEF_VEC2
#define DEF_VEC2

#include <math.h>

// template for vec2 and vec3 classes (to contain 2D and 3D positions both in int and float)
template <class number>
class Vec2 {
public:
    Vec2(number =0,number =0);
    number x;
    number y;
    number norm();
    void normalize();
    Vec2<number> operator+(Vec2<number> n);
    Vec2<number> operator-(Vec2<number> n);
private:
};



// template code cannot be in a different file than its definition
template <class number>
Vec2<number>::Vec2(number x,number y) : x(x), y(y) {
}

template <class number>
number Vec2<number>::norm() {
    return sqrt(x*x+y*y);
}

template <class number>
void Vec2<number>::normalize() {
    number val=norm();
    if(val!=0) {
        x/=val;
        y/=val;
    }
}

template <class number>
Vec2<number> Vec2<number>::operator+(Vec2<number> n) {
    n.x+=x;
    n.y+=y;
    return n;
}

template <class number>
Vec2<number> Vec2<number>::operator-(Vec2<number> n) {
    n.x-=x;
    n.y-=y;
    return n;
}

#endif
