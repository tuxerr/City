#include "scene.hpp"

Scene::Scene(Display *disp,UniformBlock *globalvalues) : 
    light_number(0), disp(disp), globalvalues(globalvalues), camera_changed(false), 
    octree(Vec3<float>(0,0,0),Vec3<float>(4096,4096,4096)) 
{
    for(int i=0;i<MAX_LIGHTS;i++) {
        lights[i]=NULL;
    }
    if(disp->has_program("phong")) {
        uniform_light_number=disp->new_uniform("lightnumber",UNIFORM_INT);
        disp->link_program_to_uniform("phong",uniform_light_number);

        uniform_cascaded_shading_zdelta=disp->new_uniform("cascaded_shading_zdelta",UNIFORM_FLOAT);
        disp->link_program_to_uniform("phong",uniform_cascaded_shading_zdelta);

        for(int i=0;i<MAX_LIGHTS;i++) {
            std::stringstream uniform_name;
            uniform_name<<"Light["<<i<<"]";
            uniform_lights[i]=disp->new_uniformblock(uniform_name.str());
            disp->link_program_to_uniformblock("phong",uniform_lights[i]);
            
            uniform_name.str("");
            uniform_name<<"shadowmap["<<i<<"]";
            uniform_light_sampler[i]=disp->new_uniform(uniform_name.str(),UNIFORM_SAMPLER);
            disp->link_program_to_uniform("phong",uniform_light_sampler[i]);

            uniform_name.str("");
            uniform_name<<"shadowcubemap["<<i<<"]";
            uniform_light_samplercube[i]=disp->new_uniform(uniform_name.str(),UNIFORM_SAMPLER);
            disp->link_program_to_uniform("phong",uniform_light_samplercube[i]);
        }

        uniform_light_number->set_value(light_number);
    }
    if(disp->has_program("depth_creation")) {
        uniform_light_projection=disp->new_uniformblock("Light_properties");
        disp->link_program_to_uniformblock("depth_creation",uniform_light_projection);
    }

}

Scene::~Scene() {
    std::set<Object*>::iterator it;
    for(it=objects.begin();it!=objects.end();it++) {
        delete_object(*it);
    }
    objects.clear();

    for(int i=0;i<MAX_LIGHTS;i++) {
        if(lights[i]!=NULL) {
            delete lights[i];
        }
    }
}

void Scene::set_perspective(float angle,float near,float far) {
    perspective.perspective(angle,near,far,(float)disp->get_width()/disp->get_height());
    scene_near=near;
    scene_far=far;
    globalvalues->set_value(scene_near,"near");
    globalvalues->set_value(scene_far,"far");
}

void Scene::set_perspective_ortho(float width,float near,float far) {
    perspective.perspective_ortho(width,near,far,(float)disp->get_width()/disp->get_height());
    scene_near=near;
    scene_far=far;
    globalvalues->set_value(scene_near,"near");
    globalvalues->set_value(scene_far,"far");
}

void Scene::set_camera(Vec3<float> pos,Vec3<float> direction,Vec3<float> up_vector) {
    camera.camera(pos,direction,up_vector);
    camera_pos = pos;
    eye_vector = direction-pos;
    this->up_vector=up_vector;
    globalvalues->set_value(eye_vector,"eye_vector");
    camera_changed=true;
    Vec3<float> eye_vec_norm = eye_vector;
    eye_vec_norm.normalize();

    frustum.perspective_frustum(pos,eye_vec_norm,up_vector,(float)disp->get_width()/disp->get_height());
}

Object* Scene::new_object() {
    Object *o=new Object(&octree);
    objects.insert(o);
    return o;
}

void Scene::delete_object(Object *o) {
    octree.delete_object(o);

    if(o!=NULL) {
        o->destroy();
        delete o;
    }
    objects.erase(o);
}

PointLight* Scene::new_pointlight(Vec3<float> pos,Vec3<float> color,float intensity) {
    for(int i=0;i<MAX_LIGHTS;i++) {
        if(lights[i]==NULL) {
            PointLight *l=new PointLight(uniform_lights[i],pos,intensity,color);
            lights[i]=(Light*)l;
            light_number++;
            uniform_light_number->set_value(light_number);
            return l;            
        }
    }
    
    Logger::log()<<"Could not create a new light : maximum number of lights are already created ("<<MAX_LIGHTS<<")"<<std::endl;
    return NULL;
}

SpotLight* Scene::new_spotlight(Vec3<float> pos,Vec3<float> color,Vec3<float> direction,float illu_angle,float max_illu_angle,float intensity) {
    for(int i=0;i<MAX_LIGHTS;i++) {
        if(lights[i]==NULL) {
            SpotLight *l=new SpotLight(uniform_lights[i],pos,direction,intensity,color,illu_angle,max_illu_angle);
            lights[i]=(Light*)l;
            light_number++;
            uniform_light_number->set_value(light_number);
            return l;            
        }
    }
    
    Logger::log()<<"Could not create a new light : maximum number of lights are already created ("<<MAX_LIGHTS<<")"<<std::endl;
    return NULL;
}

DirectionalLight* Scene::new_directionallight(Vec3<float> direction,Vec3<float> color,float intensity) {
    for(int i=0;i<MAX_LIGHTS;i++) {
        if(lights[i]==NULL) {
            DirectionalLight *l=new DirectionalLight(uniform_lights[i],direction,intensity,color);
            lights[i]=(Light*)l;
            light_number++;
            uniform_light_number->set_value(light_number);
            return l;            
        }
    }
    
    Logger::log()<<"Could not create a new light : maximum number of lights are already created ("<<MAX_LIGHTS<<")"<<std::endl;
    return NULL;
}

void Scene::delete_light(Light* l) {
    if(l!=NULL) {
        for(int i=0;i<MAX_LIGHTS;i++) {
            if(lights[i]==l) {
                for(int j=i;j<MAX_LIGHTS-1;j++) {
                    lights[j]=lights[j+1];
                    if(lights[j]!=NULL) {
                        lights[j]->set_uniform(uniform_lights[j]);
                    }
                }
                lights[MAX_LIGHTS-1]=NULL;
            }
        }
        delete l;
        light_number--;
        uniform_light_number->set_value(light_number);
    }
}

void Scene::render() {
    FBO fbo;
    Texture tex_color(DEPTH_TEXTURE_SIZE,DEPTH_TEXTURE_SIZE,TEXTURE_RGBA);
    fbo.attach_texture(&tex_color,FBO_COLOR0); // useless in this case but necessary tex for the fbo to be used

    for(int i=0;i<MAX_LIGHTS;i++) {

        if(lights[i]!=NULL) {
            if(lights[i]->enable_shadows()) {
                disp->viewport(DEPTH_TEXTURE_SIZE,DEPTH_TEXTURE_SIZE);
            
                switch(lights[i]->get_type()) {
                case POINT_LIGHT:
                    break;

                case SPOT_LIGHT:
                    break;

                case DIRECTION_LIGHT:
                    render_directional_shadowmap((DirectionalLight*) lights[i],fbo,uniform_light_sampler[i]);

                    break;

                case OFF:
                    break;

                default:
                    break;
                }

                disp->viewport();
            }
        }
    }
    
    frustum.perspective_frustum(camera_pos,eye_vector.normalize(),up_vector,(float)disp->get_width()/disp->get_height());
    draw_scene();
}

void Scene::render_directional_shadowmap(DirectionalLight* dirlight,FBO &fbo,Uniform *shadowmap_uni) {
    UniformBlock *uni = dirlight->get_uniformblock();
    Matrix4 light_mat;
    Matrix4 camera_mat;

    Vec3<float> ldir_norm = dirlight->get_direction(); 
    ldir_norm.normalize();

    Vec3<float> eye_norm = eye_vector;
    eye_norm.normalize();

    //shadowing depth and resolution calculations
    int cascaded_depth=0;
    float shadow_min_range = dirlight->get_shadow_min_range(), shadow_max_range=dirlight->get_shadow_max_range();
    if(shadow_max_range==-1)
        shadow_max_range=scene_far;
    if(shadow_min_range==-1)
        shadow_min_range=scene_near;
    float shadow_range = shadow_max_range-shadow_min_range;

    float min_shadow_resolution = (log2(((shadow_min_range-scene_near)/SHADOWING_NEAR_RANGE)+1)+1)*SHADOWING_NEAR_RANGE; //resolution at the nearest distance of the camera. Then go as planned
    float act_shadow_distance=0;
    for(int i=1;i<=SHADOWING_MAX_LAYERS;i++) {
        act_shadow_distance+=pow(2,i-1)*min_shadow_resolution;
        if(act_shadow_distance>shadow_range) {
            cascaded_depth=i;
            break;
        }
    }

    uniform_cascaded_shading_zdelta->set_value(min_shadow_resolution);
    uni->set_value(cascaded_depth,"casc_shading_depth");

    for(int cascaded_layer=0;cascaded_layer<cascaded_depth;cascaded_layer++) {

        float zmin=(pow(2,cascaded_layer)-1)*min_shadow_resolution+shadow_min_range;
        float zmax=(pow(2,cascaded_layer+1)-1)*min_shadow_resolution+shadow_min_range;
        if(cascaded_layer==SHADOWING_MAX_LAYERS-1) {
            zmax=shadow_max_range;
        }
        float zdelta = zmax-zmin;

        float f = tanf(FOV_RAD/2)*zmax;
        float vf = f * ((float)disp->get_height()/(float)disp->get_width());
        float circle_radius = sqrtf(f*f + vf*vf + (zdelta*zdelta/4));

        Vec3<float> cam_pointing_pos = camera_pos + eye_norm*zmin + eye_norm*(zdelta/2);

        Vec3<float> cam_pos = cam_pointing_pos-ldir_norm*scene_far;

        camera_mat.camera(cam_pos,cam_pointing_pos,Vec3<float>(ldir_norm.y,ldir_norm.z,ldir_norm.x));
        light_mat.perspective_ortho(circle_radius,scene_near,scene_far*2,1);
        light_mat = light_mat*camera_mat;

        std::stringstream uniform_name;
        uniform_name<<"matrix"<<(cascaded_layer+1);
        uni->set_value(light_mat,uniform_name.str());

        fbo.attach_texture(dirlight->get_depth_texture(),FBO_DEPTH,cascaded_layer);


        if(fbo.iscomplete()) {
            fbo.bind();
            uniform_light_projection->set_value(light_mat,"matrix");

            // save the current frustum

            frustum.orthogonal_frustum(cam_pos,ldir_norm,Vec3<float>(ldir_norm.y,ldir_norm.z,ldir_norm.x),circle_radius,1);

            draw_scene("depth_creation");
            
//            set_camera(cam_pos,cam_pointing_pos,Vec3<float>(ldir_norm.y,ldir_norm.z,ldir_norm.x));
//            set_perspective_ortho(circle_radius*2,scene_near,scene_far*2);


            fbo.unbind();
        }  else {
            Logger::log()<<"FBO incomplete for the render of directional light "<<std::endl;
        }  
//        draw_scene("depth_creation");
        
    }

    shadowmap_uni->set_value(dirlight->get_depth_texture());
}

void Scene::draw_scene(std::string program_name) {
    disp->new_draw();

    Program *program=NULL;

    if(camera_changed) {
        globalvalues->set_value(camera_pos,"camera_pos");
    }

    if(program_name!="") {
        program=disp->get_program(program_name);
        program->use();
    }

    std::list<Object*> drawn;
    draw_octree(octree,true,drawn,program_name);
 
    int i=0;
    std::list<Object*>::iterator it;
    for(it=drawn.begin();it!=drawn.end();it++) {
        Object *o=*it;
        o->has_been_drawn=false;
        i++;
    }

    if(program_name!="") {
        program->unuse();
    }

    if(camera_changed) {
        camera_changed=false;
    }

}

void Scene::draw_octree(Octree &oct,bool testcollision,std::list<Object*> &drawn,std::string program_name) {
    if(oct.objects.size()!=0) {

        std::list<Object*>::iterator it;
        for(it=oct.objects.begin();it!=oct.objects.end();it++) {
            Object *o=*it;
            if(!o->has_been_drawn) {

                o->update_matrices(&perspective,&camera);

                if(program_name!="") {
                    draw_object(o,false);
                } else {
                    draw_object(o,true);
                }
                drawn.push_back(o);
                o->has_been_drawn=true;

            }
        }

    } else {

        if(testcollision) {
            Octree_Collisions col = oct.frustum_collision(frustum);
            if(col==FULL_IN) {
                for(int i=0;i<8;i++) {
                    if(oct.nodes[i]!=NULL) {
                        draw_octree(*oct.nodes[i],false,drawn,program_name);
                    }
                }
            } else if(col==IN) {
                for(int i=0;i<8;i++) {
                    if(oct.nodes[i]!=NULL) {
                        draw_octree(*oct.nodes[i],true,drawn,program_name);
                    }
                }
            } 
        } else {
            for(int i=0;i<8;i++) {
                if(oct.nodes[i]!=NULL) {
                    draw_octree(*oct.nodes[i],false,drawn,program_name);
                }
            }
        }

    }
}

void Scene::draw_object(Object *o,bool use_shaders) {
    if(o->enable_draw()) {

        std::string program_name=o->get_program();
        // if the object's shader doesn't exist, use default one.
        Program *program=disp->get_program(program_name);
        Vec3<float> object_position((o->modelview_matrix()).val[3],
                                    (o->modelview_matrix()).val[7],
                                    (o->modelview_matrix()).val[11]);
        Vec3<float> objminuscam = object_position - camera_pos;

        globalvalues->set_value(o->modelview_matrix(),"modelview");

        globalvalues->set_value(o->projection_modelview_matrix(),"projection_modelview");

        globalvalues->set_value(o->normal_matrix(),"normal_matrix");

        if(use_shaders) {
            program->use();
        }

        o->draw(objminuscam.norm());

        if(use_shaders) {
            program->unuse();
        }
    }
}
