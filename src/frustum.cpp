#include "frustum.hpp"

void Frustum::perspective_frustum(Vec3<float> pos, Vec3<float> eye_vec_norm, Vec3<float> up_vector,float ratio) {

     // new frustum calculation for the octree
    Vec3<float> farpoint = pos+(eye_vec_norm*FAR);
    Vec3<float> rightvec = eye_vec_norm.cross(up_vector);
    rightvec.normalize();
    rightvec=rightvec*((tan(FOV_RAD)*FAR)/2);

    up_vector.normalize();
    up_vector=up_vector*( rightvec.norm()/ratio);

    // topright, topleft, lowright and low-left vectors in the far plane of the frustum
    Vec3<float> tr = (farpoint+rightvec+up_vector)-pos;
    Vec3<float> tl = (farpoint-rightvec+up_vector)-pos;
    Vec3<float> lr = (farpoint+rightvec-up_vector)-pos;
    Vec3<float> ll = (farpoint-rightvec-up_vector)-pos;

    // the 5 normals of the 5 plans of the pyramidal frustum (oriented towards the EXTERIOR of the frustum)
    normal[0]=eye_vec_norm;
    normal[1]=(tr.cross(tl)).normalize();
    normal[2]=(lr.cross(tr)).normalize();
    normal[3]=(ll.cross(lr)).normalize();
    normal[4]=(tl.cross(ll)).normalize();
    this->farpoint=farpoint;
    origin=pos;
    ortho_frustum=false;
}

void Frustum::orthogonal_frustum(Vec3<float> pos, Vec3<float> eye_vec_norm, Vec3<float> up_vector,float width, float ratio) {

    // new frustum calculation for the octree
    Vec3<float> farpoint = pos+(eye_vec_norm*FAR);
    Vec3<float> rightvec = eye_vec_norm.cross(up_vector);
    rightvec.normalize();
    rightvec=rightvec*(width/2);

    up_vector.normalize();
    up_vector=up_vector*(rightvec.norm()/ratio);

    // the 5 normals of the 5 plans of the pyramidal frustum (oriented towards the EXTERIOR of the frustum)
    normal[0]=eye_vec_norm;
    normal[1]=Vec3<float>(0,0,0)-eye_vec_norm;
    normal[2]=rightvec.normalize();
    normal[3]=(Vec3<float>(0,0,0)-rightvec).normalize();
    normal[4]=(up_vector.normalize());
    normal[5]=(Vec3<float>(0,0,0)-up_vector).normalize();

    this->farpoint=farpoint;
    origin=pos;
    ortho_up_vec=up_vector;
    ortho_right_vec=rightvec;
    ortho_frustum=true;
}
