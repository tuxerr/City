#include "scene.h"

Scene::Scene(Display *disp,UniformBlock *matrices) : light_number(0), disp(disp), matrices(matrices), camera_changed(false), perspective_changed(false) {
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
    perspective_changed=true;
}

void Scene::set_perspective_ortho(float width,float near,float far) {
    perspective.perspective_ortho(width,near,far,(float)disp->get_width()/disp->get_height());
    perspective_changed=true;
}

void Scene::set_camera(Vec3<float> pos,Vec3<float>direction,Vec3<float>up_vector) {
    camera.camera(pos,direction,up_vector);
    camera_pos = pos;
    eye_vector = direction-pos;
    matrices->set_value(eye_vector,"eye_vector");
    camera_changed=true;
}

Object* Scene::new_object() {
    Object *o=new Object();
    objects.insert(o);
    return o;
}

void Scene::delete_object(Object *o) {
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
    
    std::cout<<"Could not create a new light : maximum number of lights are already created ("<<MAX_LIGHTS<<")"<<std::endl;
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
    
    std::cout<<"Could not create a new light : maximum number of lights are already created ("<<MAX_LIGHTS<<")"<<std::endl;
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
    
    std::cout<<"Could not create a new light : maximum number of lights are already created ("<<MAX_LIGHTS<<")"<<std::endl;
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
    fbo.attach_texture(&tex_color,FBO_COLOR0);

    for(int i=0;i<MAX_LIGHTS;i++) {

        if(lights[i]!=NULL) {

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
    draw_scene();

}

void Scene::render_directional_shadowmap(DirectionalLight* dirlight,FBO &fbo,Uniform *shadowmap_uni) {

    UniformBlock *uni = dirlight->get_uniformblock();
    Matrix4 light_mat;
    Matrix4 camera_mat;

    Vec3<float> ldir_norm;
    ldir_norm = dirlight->get_direction(); 
    ldir_norm.normalize();

    Vec3<float> eye_norm;
    eye_norm = eye_vector;
    eye_norm.normalize();

    float min_z_iter = (FAR-NEAR)/(powf(2,CASCADED_SHADING_DEPTH)-1);
    uniform_cascaded_shading_zdelta->set_value(min_z_iter);

    for(int cascaded_layer=0;cascaded_layer<CASCADED_SHADING_DEPTH;cascaded_layer++) {

        float zmin=(pow(2,cascaded_layer)-1)*min_z_iter+NEAR;
        float zmax=(pow(2,cascaded_layer+1)-1)*min_z_iter+NEAR;

        float zdelta = zmax-zmin;
        float f = tanf(FOV_RAD/2)*zmax;
        float n = tanf(FOV_RAD/2)*zmin;
        float vn = n * ((float)disp->get_height()/(float)disp->get_width());
        float vf = f * ((float)disp->get_height()/(float)disp->get_width());
        float ratio = (f*f-n*n)/(2*zdelta*zdelta);
        if(ratio>1) {
            ratio=1;
        }

        float layer_length=maxf(sqrtf(sqrt(n*n + vn*vn) + pow((ratio*zdelta),2)),
                                sqrtf(sqrt(f*f + vf*vf) + pow(((1-ratio)*zdelta),2)));

        Vec3<float> cam_pos;
        cam_pos = camera_pos + eye_norm*zmin + eye_norm*zdelta*ratio;

        camera_mat.camera(cam_pos-ldir_norm*FAR,cam_pos,Vec3<float>(ldir_norm.y,ldir_norm.z,ldir_norm.x));
        light_mat.perspective_ortho(layer_length*2,NEAR,FAR*2,1);
        light_mat = light_mat*camera_mat;
    
        std::stringstream uniform_name;
        uniform_name<<"matrix"<<(cascaded_layer+1);
        uni->set_value(light_mat,uniform_name.str());

        fbo.attach_texture(dirlight->get_depth_texture(),FBO_DEPTH,cascaded_layer);

        if(fbo.iscomplete()) {
            fbo.bind();
            uniform_light_projection->set_value(light_mat,"matrix");
            draw_scene("depth_creation");
            fbo.unbind();
        }  else {
            std::cout<<"FBO incomplete for the render of directional light "<<std::endl;
        }  
        
    }

    shadowmap_uni->set_value(dirlight->get_depth_texture());
}

void Scene::draw_scene(std::string program_name) {
    disp->new_draw();

    Program *program=NULL;

    if(camera_changed) {
        matrices->set_value(camera_pos,"camera_pos");
    }

    if(program_name!="") {
        program=disp->get_program(program_name);
        program->use();
    }

    std::set<Object*>::iterator it;
    for(it=objects.begin();it!=objects.end();it++) {
        Object *o=*it;
        if(o->need_to_update_matrices() || perspective_changed || camera_changed) {
            o->update_matrices(&perspective,&camera);
        }

        if(program_name!="") {
            draw_object(o,false);
        } else {
            draw_object(o,true);
        }

    }

    if(program_name!="") {
        program->unuse();
    }

    if(perspective_changed) {
        perspective_changed=false;
    } 
    if(camera_changed) {
        camera_changed=false;
    }

}

void Scene::draw_object(Object *o,bool use_shaders) {
    if(o->enable_draw()) {

        std::string program_name=o->get_program();
        // if the object's shader doesn't exist, use default one.
        Program *program=disp->get_program(program_name);

        matrices->set_value(o->modelview_matrix(),"modelview");

        matrices->set_value(o->projection_modelview_matrix(),"projection_modelview");

        matrices->set_value(o->normal_matrix(),"normal_matrix");

        if(use_shaders) {
            program->use();
        }

        o->draw();

        if(use_shaders) {
            program->unuse();
        }
    }
}
