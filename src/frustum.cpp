
#include "frustum.hpp"

void Frustum::perspective_frustum(Vec3<float> pos, Vec3<float> eye_vec_norm, Vec3<float> up_vector,float ratio,float scene_far,float scene_fov) {

     // new frustum calculation for the octree
    Vec3<float> farpoint = pos+(eye_vec_norm*scene_far);
    Vec3<float> rightvec = eye_vec_norm.cross(up_vector);
    rightvec.normalize();
    rightvec=rightvec*((tan(scene_fov)*scene_far)/2);

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

void Frustum::orthogonal_frustum(Vec3<float> pos, Vec3<float> eye_vec_norm, Vec3<float> up_vector,float width, float ratio,float scene_far) {

    // new frustum calculation for the octree
    Vec3<float> farpoint = pos+(eye_vec_norm*scene_far);
    Vec3<float> rightvec = eye_vec_norm.cross(up_vector);
    rightvec.normalize();
    rightvec=rightvec*(width/2);

    up_vector.normalize();
    up_vector=up_vector*(width/(ratio*2));

    // the 6 normals of the rectangular frustum 
    normal[0]=eye_vec_norm;
    normal[1]=Vec3<float>(0,0,0)-eye_vec_norm;
    normal[2]=rightvec;
    normal[2].normalize();
    normal[3]=Vec3<float>(0,0,0)-normal[2];
    normal[4]=up_vector;
    normal[4].normalize();
    normal[5]=Vec3<float>(0,0,0)-normal[4];

    this->farpoint=farpoint;
    origin=pos;
    ortho_up_vec=up_vector;
    ortho_right_vec=rightvec;
    ortho_frustum=true;
}
