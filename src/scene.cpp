#include "scene.h"

Scene::Scene(Display *disp,UniformBlock *matrices) : light_number(0), disp(disp), matrices(matrices), camera_changed(false), perspective_changed(false) {
    for(int i=0;i<MAX_LIGHTS;i++) {
        lights[i]=NULL;
    }
    if(disp->has_program("phong")) {
        uniform_light_number=disp->new_uniform("lightnumber",UNIFORM_INT);
        uniform_light_sampler=disp->new_uniform("shadowmap",UNIFORM_SAMPLER);

        disp->link_program_to_uniform("phong",uniform_light_number);
        disp->link_program_to_uniform("phong",uniform_light_sampler);

        for(int i=0;i<MAX_LIGHTS;i++) {
            std::stringstream uniform_name;
            uniform_name<<"Light["<<i<<"]";
            uniform_lights[i]=disp->new_uniformblock(uniform_name.str());
            disp->link_program_to_uniformblock("phong",uniform_lights[i]);
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
    Texture tex_depth(DEPTH_TEXTURE_SIZE,DEPTH_TEXTURE_SIZE,TEXTURE_DEPTH);
    fbo.attach_texture(&tex_depth,FBO_DEPTH);
    
    if(fbo.iscomplete()) {
        disp->viewport(DEPTH_TEXTURE_SIZE,DEPTH_TEXTURE_SIZE);
    
        fbo.bind();
        Matrix4 light_mat = lights[0]->get_matrix();
        uniform_light_projection->set_value(light_mat,"matrix");
        draw_scene("depth_creation");
        fbo.unbind();

        uniform_light_sampler->set_value(&tex_depth);

        disp->viewport();

    }  else {
        std::cout<<"FBO incomplete"<<std::endl;
    }  

    draw_scene();
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
