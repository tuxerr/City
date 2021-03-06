#include "scene.hpp"

static GLenum bufs[]={GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};

Scene::Scene(Display *disp) : 
    light_number(0), disp(disp), camera_changed(false), 
    octree(Vec3<float>(0,0,0),Vec3<float>(4096,4096,4096))
{
    // program initialisation
    disp->new_program("shaders/default.vert","shaders/default.frag",NULL,NULL,NULL);
    disp->new_program("shaders/fullscreen_draw.vert","shaders/phong.frag",NULL,NULL,NULL,"phong");
    disp->new_program("shaders/blur_vertical.vert","shaders/blur.frag",NULL,NULL,NULL,"vertical_blur");
    disp->new_program("shaders/blur_horizontal.vert","shaders/blur.frag",NULL,NULL,NULL,"horizontal_blur");
    disp->new_program("shaders/pointlight_draw.vert","shaders/phong_pointlight.frag",NULL,NULL,NULL,"phong_pointlight");
    disp->new_program("shaders/pointlight_draw.vert","shaders/depth_creation.frag",NULL,NULL,NULL,"phong_pointlight_stencil");
    disp->new_program("shaders/depth_creation.vert","shaders/depth_creation.frag",NULL,NULL,NULL,"depth_creation");
    disp->new_program("shaders/displaytexture.vert","shaders/displaytexture.frag",NULL,NULL,NULL,"display_texture");
    //disp->new_program("shaders/deferred_tess.vert","shaders/deferred_tess.frag","shaders/terrain_tc.tess","shaders/terrain_te.tess",NULL,"deferred_tess");
    disp->new_program("shaders/deferred.vert","shaders/deferred.frag",NULL,NULL,NULL,"deferred");

    int screen_width=disp->get_width();
    int screen_height=disp->get_height();

    globalvalues=disp->new_uniformblock("GlobalValues");
    disp->link_program_to_uniformblock("default",globalvalues);
    disp->link_program_to_uniformblock("phong",globalvalues);
    disp->link_program_to_uniformblock("phong_pointlight",globalvalues);
    disp->link_program_to_uniformblock("phong_pointlight_stencil",globalvalues);
    disp->link_program_to_uniformblock("depth_creation",globalvalues);
    disp->link_program_to_uniformblock("deferred",globalvalues);
    disp->link_program_to_uniformblock("display_texture",globalvalues);
    disp->link_program_to_uniformblock("vertical_blur",globalvalues);
    disp->link_program_to_uniformblock("horizontal_blur",globalvalues);

    
    Vec2<float> screen_size(screen_width,screen_height);
    globalvalues->set_value(screen_size, "screen_size");

    for(int i=0;i<MAX_LIGHTS;i++) {
        lights[i]=NULL;
    }
    
    if(disp->has_program("phong")) {
        uniform_cascaded_shading_zdelta=disp->new_uniform("cascaded_shading_zdelta",UNIFORM_FLOAT);
        disp->link_program_to_uniform("phong",uniform_cascaded_shading_zdelta);

        uniform_phong_normalmap=disp->new_uniform("normalmap",UNIFORM_SAMPLER);
        disp->link_program_to_uniform("phong",uniform_phong_normalmap);

        uniform_phong_depthmap=disp->new_uniform("depthmap",UNIFORM_SAMPLER);
        disp->link_program_to_uniform("phong",uniform_phong_depthmap);

        uniform_phong_colormap=disp->new_uniform("colormap",UNIFORM_SAMPLER);
        disp->link_program_to_uniform("phong",uniform_phong_colormap);

        uniform_phong_texcoordmap=disp->new_uniform("texcoordmap",UNIFORM_SAMPLER);
        disp->link_program_to_uniform("phong",uniform_phong_texcoordmap);

        uniform_light=disp->new_uniformblock("Light");
        disp->link_program_to_uniformblock("phong",uniform_light);
        
        uniform_light_sampler=disp->new_uniform("shadowmap",UNIFORM_SAMPLER);
        disp->link_program_to_uniform("phong",uniform_light_sampler);

            //uniform_name.str("");
            //uniform_name<<"shadowcubemap["<<i<<"]";
            //uniform_light_samplercube[i]=disp->new_uniform(uniform_name.str(),UNIFORM_SAMPLER);
            //disp->link_program_to_uniform("phong",uniform_light_samplercube[i]);
        

    }
    if(disp->has_program("phong_pointlight")) {
        disp->link_program_to_uniform("phong_pointlight",uniform_phong_normalmap);
        disp->link_program_to_uniform("phong_pointlight",uniform_phong_depthmap);
        disp->link_program_to_uniform("phong_pointlight",uniform_phong_colormap);
        disp->link_program_to_uniformblock("phong_pointlight",uniform_light);
    }
    if(disp->has_program("vertical_blur")) {
        uniform_postprocess_colormap=disp->new_uniform("deferred_colormap",UNIFORM_SAMPLER);

        //disp->link_program_to_uniform("blur_vertical",uniform_phong_normalmap);
        //disp->link_program_to_uniform("blur_vertical",uniform_phong_depthmap);
        disp->link_program_to_uniform("vertical_blur",uniform_postprocess_colormap);
    }
    if(disp->has_program("horizontal_blur")) {
        //disp->link_program_to_uniform("blur_horizontal",uniform_phong_normalmap);
        //disp->link_program_to_uniform("blur_horizontal",uniform_phong_depthmap);
        disp->link_program_to_uniform("horizontal_blur",uniform_postprocess_colormap);
    }
    if(disp->has_program("depth_creation")) {
        uniform_light_projection=disp->new_uniformblock("Light_properties");
        disp->link_program_to_uniformblock("depth_creation",uniform_light_projection);
    }
    if(disp->has_program("display_texture")) {
        uniform_displaytex_tex = disp->new_uniform("tex",UNIFORM_SAMPLER);
        uniform_displaytex_arraytex = disp->new_uniform("arraytex",UNIFORM_SAMPLER);
        uniform_displaytex_choice = disp->new_uniform("choice",UNIFORM_INT);
        disp->link_program_to_uniform("display_texture",uniform_displaytex_tex);
        disp->link_program_to_uniform("display_texture",uniform_displaytex_arraytex);
        disp->link_program_to_uniform("display_texture",uniform_displaytex_choice);
    }

    generate_fullscreen_quad();
    generate_cube(); //cube centered at 0,0,0 and of size 2 (-1 to 1)
    
    // textures initialization
    null_colortex=new Texture(DEPTH_TEXTURE_SIZE,DEPTH_TEXTURE_SIZE,TEXTURE_RGBA);
    deferred_colormap=new Texture(screen_width,screen_height,TEXTURE_RGBA);
    deferred_normalmap=new Texture(screen_width,screen_height,TEXTURE_RGBA);
    deferred_texcoordmap=new Texture(screen_width,screen_height,TEXTURE_RGBA);
    deferred_depthmap=new Texture(screen_width,screen_height,TEXTURE_DEPTH);
    deferred_result=new Texture(screen_width,screen_height,TEXTURE_RGBA);
    postprocess_flip1=new Texture(screen_width,screen_height,TEXTURE_RGBA);
    postprocess_flip2=new Texture(screen_width,screen_height,TEXTURE_RGBA);

    // uniform setting
    uniform_phong_normalmap->set_value(deferred_normalmap);
    uniform_phong_colormap->set_value(deferred_colormap);
    uniform_phong_texcoordmap->set_value(deferred_texcoordmap);
    uniform_phong_depthmap->set_value(deferred_depthmap);

    uniform_displaytex_tex->set_value(postprocess_flip2);
    uniform_displaytex_choice->set_value(-1);
    
    //rendering FBOs initialization
    fbo_shadows.attach_texture(null_colortex,FBO_COLOR0);
    
    fbo_deferred_phong.attach_texture(deferred_result,FBO_COLOR0);
    fbo_deferred_phong.attach_texture(deferred_depthmap,FBO_DEPTH);
    
    fbo_deferred.attach_texture(deferred_normalmap,FBO_COLOR0);
    fbo_deferred.attach_texture(deferred_colormap,FBO_COLOR1);
    fbo_deferred.attach_texture(deferred_texcoordmap,FBO_COLOR2);
    fbo_deferred.attach_texture(deferred_depthmap,FBO_DEPTH);
}

void Scene::generate_cube() {
    //generate the fullscreen cube object
    cube = new_object_outside_octree();
    cube->set_program("phong_pointlight");
    cube->set_enable_draw(false);
    
    float vert[] = {
        // front
        -1.0, -1.0,  1.0,
        1.0, -1.0,  1.0,
        1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        // back
        -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,
        1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0,
    }; //
    
    unsigned int index[] = {
        0, 1, 2,
        2, 3, 0,
        // top
        3, 2, 6,
        6, 7, 3,
        // back
        7, 6, 5,
        5, 4, 7,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // left
        4, 0, 3,
        3, 7, 4,
        // right
        1, 5, 6,
        6, 2, 1,
        };
    
    float color[] = {1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1};
    
    cube->update_vertices_buffer(&vert[0],sizeof(vert));
    cube->update_triangles_index_buffer(&index[0],sizeof(index));
    cube->update_color_buffer(&color[0],sizeof(color));
    cube->set_draw_mode(OBJECT_DRAW_TRIANGLES);
}

void Scene::generate_fullscreen_quad() {
    //generate the fullscreen quad object
    fullscreen_quad = new_object_outside_octree();
    fullscreen_quad->set_program("display_texture");
    fullscreen_quad->set_enable_draw(false);
    
    float vert[] = {-1, -1, 0,
        1, -1, 0,
        1, 1, 0,
        -1, 1, 0};
    
    unsigned int index[] = {0, 1, 2, 2, 3, 0};
    
    float color[] = {1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1};
    
    fullscreen_quad->update_vertices_buffer(&vert[0],sizeof(vert));
    fullscreen_quad->update_triangles_index_buffer(&index[0],sizeof(index));
    fullscreen_quad->update_color_buffer(&color[0],sizeof(color));
    fullscreen_quad->set_draw_mode(OBJECT_DRAW_TRIANGLES);
}

Scene::~Scene() {
    std::set<Object*>::iterator it;
    //for(it=objects.begin();it!=objects.end();it++) {
    //    delete_object(*it);
    //}
    objects.clear();

    for(int i=0;i<MAX_LIGHTS;i++) {
        if(lights[i]!=NULL) {
            delete lights[i];
        }
    }

    delete null_colortex;
    delete deferred_normalmap;
    delete deferred_colormap;
    delete deferred_texcoordmap;
    delete deferred_depthmap;
}

void Scene::set_perspective(float angle,float near,float far) {
    perspective.perspective(angle,near,far,disp->get_ratio());
    scene_near=near;
    scene_far=far;
    scene_fov_rad=(angle*M_PI/180);                                            
    globalvalues->set_value(scene_near,"near");
    globalvalues->set_value(scene_far,"far");
}

void Scene::set_perspective_ortho(float width,float near,float far) {
    perspective.perspective_ortho(width,near,far,disp->get_ratio());
    scene_near=near;
    scene_far=far;
    globalvalues->set_value(scene_near,"near");
    globalvalues->set_value(scene_far,"far");
}

void Scene::set_camera(Vec3<float> pos,Vec3<float> direction,Vec3<float> up_vector) {
    up_vector.normalize();
    camera.camera(pos,direction,up_vector);
    camera_pos = pos;
    eye_vector = direction-pos;
    this->up_vector=up_vector;
    globalvalues->set_value(eye_vector,"eye_vector");
    camera_changed=true;
    Vec3<float> eye_vec_norm = eye_vector;
    eye_vec_norm.normalize();

    frustum.perspective_frustum(pos,eye_vec_norm,up_vector,disp->get_ratio(),scene_far,scene_fov_rad);
}

Object* Scene::new_object() {
    Object *o=new Object(&octree);
    objects.insert(o);
    return o;
}

Object* Scene::new_object_outside_octree() {
    Object *o=new Object(NULL);
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
            PointLight *l=new PointLight(pos,intensity,color);
            lights[i]=(Light*)l;
            light_number++;
            return l;
        }
    }
    
    Logger::log()<<"Could not create a new light : maximum number of lights are already created ("<<MAX_LIGHTS<<")"<<std::endl;
    return NULL;
}

SpotLight* Scene::new_spotlight(Vec3<float> pos,Vec3<float> color,Vec3<float> direction,float illu_angle,float max_illu_angle,float intensity) {
    for(int i=0;i<MAX_LIGHTS;i++) {
        if(lights[i]==NULL) {
            SpotLight *l=new SpotLight(pos,direction,intensity,color,illu_angle,max_illu_angle);
            lights[i]=(Light*)l;
            light_number++;
            return l;
        }
    }
    
    Logger::log()<<"Could not create a new light : maximum number of lights are already created ("<<MAX_LIGHTS<<")"<<std::endl;
    return NULL;
}

DirectionalLight* Scene::new_directionallight(Vec3<float> direction,Vec3<float> color,float intensity) {
    for(int i=0;i<MAX_LIGHTS;i++) {
        if(lights[i]==NULL) {
            DirectionalLight *l=new DirectionalLight(direction,intensity,color);
            lights[i]=(Light*)l;
            light_number++;
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
                }
                lights[MAX_LIGHTS-1]=NULL;
            }
        }
        delete l;
        light_number--;
    }
}

void Scene::set_deferred_cube_position(PointLight *l) {
    float cube_size = l->get_intensity()*LINEAR_LIGHT_ATTENUATION*DEFERRED_CUBE_SIZE;
    cube->reset_modelview();
    cube->set_pos(l->get_pos());
    cube->scale(cube_size,cube_size,cube_size);
    cube->update_matrices(&perspective,&camera);
}

void Scene::render() {
     // useless in this case but necessary tex for the fbo to be complete
    disp->new_draw();
    
    frustum.perspective_frustum(camera_pos,eye_vector.normalize(),up_vector,disp->get_ratio(),scene_far,scene_fov_rad);
    
    fbo_deferred_phong.bind();
    disp->new_draw();
    fbo_deferred_phong.unbind();
    glEnable(GL_DEPTH_TEST);
    
    // deferred rendering : first pass (Gbuffer construction)
    if(fbo_deferred.iscomplete()) {
        fbo_deferred.bind();
        disp->new_draw();
        glDrawBuffers(3,&(bufs[0]));

        draw_scene();
        fbo_deferred.unbind();

    } else {
        Logger::log(LOG_ERROR)<<"Couldn't bind the deferred rendering FBO"<<std::endl;
    }
    
    Matrix4 screen_to_world;
    screen_to_world = perspective*camera;
    screen_to_world.invert();
    globalvalues->set_value(screen_to_world,"screen_to_world");
    
    glDrawBuffers(1,&(bufs[0]));
    for(int i=0;i<MAX_LIGHTS;i++) {
        
        if(lights[i]!=NULL && lights[i]->is_activated()) {
            //shadowing pass
            if(lights[i]->enable_shadows()) {
                disp->viewport(DEPTH_TEXTURE_SIZE,DEPTH_TEXTURE_SIZE);
                
                switch(lights[i]->get_type()) {
                    case POINT_LIGHT:
                        break;
                        
                    case SPOT_LIGHT:
                        break;
                        
                    case DIRECTION_LIGHT:
                        render_directional_shadowmap((DirectionalLight*) lights[i]);
                        break;
                        
                    case OFF:
                        break;
                        
                    default:
                        break;
                }
                
                disp->viewport();
                for(Object* o : objects) {
                    o->reset_lod_to_draw();
                }
                
                uniform_light_sampler->set_value(lights[i]->get_depth_texture());
            }
            
            //lighting pass with phong shader
            if(fbo_deferred_phong.iscomplete()) {
                fbo_deferred_phong.bind();
                
                if(lights[i]->get_type()==POINT_LIGHT || lights[i]->get_type()==SPOT_LIGHT) {
                    //std::cout<<"start: "<<glGetError()<<std::endl;

                    glEnable(GL_STENCIL_TEST);
                    //pretty fucking clever trick that uses the stencil to limit pixels to render http://ogldev.atspace.co.uk/www/tutorial37/tutorial37.html
                    
                    glDepthMask(GL_FALSE);
                    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
                    glDisable(GL_CULL_FACE);
                    glClear(GL_STENCIL_BUFFER_BIT);
                    
                    glStencilFunc(GL_ALWAYS, 0, 0);
                    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
                
                    lights[i]->set_uniform(uniform_light);
                    set_deferred_cube_position((PointLight*)lights[i]);

                    cube->set_program("phong_pointlight_stencil");
                    draw_object(cube);
                    
                    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
                    glDisable(GL_DEPTH_TEST);
                    glClear(GL_COLOR_BUFFER_BIT);
                    
                    glEnable(GL_BLEND);
                    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
                
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_FRONT);
                    
                    cube->set_program("phong_pointlight");
                    draw_object(cube);
                    
                    glDisable(GL_BLEND);
                    glDepthMask(GL_TRUE);
                    
                    glDisable(GL_STENCIL_TEST);
                    glCullFace(GL_BACK);
                    glEnable(GL_DEPTH_TEST);
                    //std::cout<<glGetError()<<std::endl;
                } else {
                    fullscreen_quad->set_program("phong");
                    lights[i]->set_uniform(uniform_light);

                    glEnable(GL_BLEND);
                    glDisable(GL_DEPTH_TEST);
                    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

                    draw_object(fullscreen_quad);
                    glEnable(GL_DEPTH_TEST);

                    glDisable(GL_BLEND);
                }
                
                glDisable(GL_BLEND);

                fbo_deferred_phong.unbind();
            } else {
                Logger::log(LOG_ERROR)<<"Couldn't bind the deferred rendering FBO for the phong pass"<<std::endl;
            }
            
        }
    }
    
    // 2-way blur pass
    uniform_postprocess_colormap->set_value(deferred_result);
    fbo_postprocess.attach_texture(postprocess_flip1,FBO_COLOR0);
    glDisable(GL_DEPTH_TEST);
    if(fbo_postprocess.iscomplete()) {
        fbo_postprocess.bind();
        disp->new_draw();
        fullscreen_quad->set_program("horizontal_blur");
        draw_object(fullscreen_quad);
        fbo_postprocess.unbind();
    } else {
        Logger::log(LOG_ERROR)<<"Couldn't bind the postprocessing FBO for the horizontal blur pass"<<std::endl;
    }
    
    fbo_postprocess.attach_texture(postprocess_flip2,FBO_COLOR0);
    uniform_postprocess_colormap->set_value(postprocess_flip1);
    if(fbo_postprocess.iscomplete()) {
        fbo_postprocess.bind();
        disp->new_draw();
        fullscreen_quad->set_program("vertical_blur");
        draw_object(fullscreen_quad);
        fbo_postprocess.unbind();
    } else {
        Logger::log(LOG_ERROR)<<"Couldn't bind the postprocessing FBO for the vertical blur pass"<<std::endl;
    }

    // draw result of final pass (using fullscreen quad)
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    disp->new_draw();
    fullscreen_quad->set_program("display_texture");
    draw_object(fullscreen_quad);
    
}

void Scene::render_directional_shadowmap(DirectionalLight* dirlight) {
    Matrix4 light_mat,camera_mat;

    Vec3<float> ldir_norm = dirlight->get_direction(); 
    ldir_norm.normalize();

    // vectors defining the light-space plan to project points onto it (for pixel-size only transitions)
    Vec3<float> lsp_y(0,ldir_norm.z/ldir_norm.y,-1);     lsp_y.normalize();
    Vec3<float> lsp_x=ldir_norm.cross(lsp_y);    lsp_x.normalize();

    Vec3<float> eye_norm = eye_vector;
    eye_norm.normalize();

    //shadowing depth and resolution calculations
    int cascaded_depth=8;
    float shadow_min_range = dirlight->get_shadow_min_range(), shadow_max_range=dirlight->get_shadow_max_range();
    if(shadow_max_range==-1)
        shadow_max_range=scene_far;
    if(shadow_min_range==-1)
        shadow_min_range=scene_near;
    float shadow_range = shadow_max_range-shadow_min_range;

    //resolution at the nearest distance of the camera. Then go as planned
    float min_shadow_resolution = (log2(((shadow_min_range-scene_near)/SHADOWING_NEAR_RANGE)+1)+1)*SHADOWING_NEAR_RANGE; 
    uniform_cascaded_shading_zdelta->set_value(min_shadow_resolution);
    
    float act_shadow_distance=0;
    for(int i=1;i<=SHADOWING_MAX_LAYERS;i++) {
        act_shadow_distance+=pow(2,i-1)*min_shadow_resolution;
        if(act_shadow_distance>shadow_range) {
            cascaded_depth=i;
            break;
        }
    }


    // calculation of the general projection matrix (global light) to calculate subfrustum borders
    Matrix4 global_light_projection;
    Vec3<float> global_cam_pos = camera_pos + eye_norm*(shadow_min_range+shadow_range*0.5);
    float gf = tanf(scene_fov_rad/2)*shadow_max_range;
    float vgf = gf / disp->get_ratio();
    float global_radius = sqrt(gf*gf + vgf*vgf + (shadow_min_range+shadow_range)*(shadow_min_range+shadow_range)*0.25);
    camera_mat.camera((global_cam_pos-ldir_norm*scene_far),global_cam_pos,lsp_y);
    global_light_projection.perspective_ortho(global_radius*2,scene_near,scene_far*2,1);
    global_light_projection = global_light_projection * camera_mat;

    Matrix4 global_light_projection_inv = global_light_projection;
    global_light_projection_inv.invert();

    // vector containing subfrustum positions near the camera (each iteration put their far pos into this for the next iteration)
    Vec3<float> subfrustum_near_position[4];
    float sf = tanf(scene_fov_rad/2)*shadow_min_range;
    float vsf = sf / disp->get_ratio();
    Vec3<float> mid_position = camera_pos + eye_norm*shadow_min_range;
    Vec3<float> third_eye_vector = eye_vector.cross(up_vector);
    third_eye_vector.normalize();
    subfrustum_near_position[0] = mid_position + up_vector*vsf + third_eye_vector*sf;
    subfrustum_near_position[1] = mid_position + up_vector*vsf - third_eye_vector*sf;
    subfrustum_near_position[2] = mid_position - up_vector*vsf + third_eye_vector*sf;
    subfrustum_near_position[3] = mid_position - up_vector*vsf - third_eye_vector*sf;

    // projection of the 3D subfrustum positions into the 2D light-view plane.
    for(int i=0;i<4;i++) {
        subfrustum_near_position[i] = global_light_projection*subfrustum_near_position[i];
    }

    for(int cascaded_layer=0;cascaded_layer<cascaded_depth;cascaded_layer++) {

        float zmax=(pow(2,cascaded_layer+1)-1)*min_shadow_resolution+shadow_min_range;
        if(cascaded_layer==SHADOWING_MAX_LAYERS-1) {
            zmax=shadow_max_range;
        }

        float f = tanf(scene_fov_rad/2)*zmax;
        float vf = f / disp->get_ratio();

        // calculate the 4 zmax points and project them into 2D light_space
        Vec3<float> subfrustum_far_position[4];
        Vec3<float> far_position = camera_pos + eye_norm*zmax;
        subfrustum_far_position[0] = far_position + up_vector*vf + third_eye_vector*f;
        subfrustum_far_position[1] = far_position + up_vector*vf - third_eye_vector*f;
        subfrustum_far_position[2] = far_position - up_vector*vf + third_eye_vector*f;
        subfrustum_far_position[3] = far_position - up_vector*vf - third_eye_vector*f;

        for(int i=0;i<4;i++) {
            subfrustum_far_position[i] = global_light_projection*subfrustum_far_position[i];
        }
        float optimal_radius;
        Vec3<float> optimal_center = calculate_shadowing_optimal_point(subfrustum_near_position,subfrustum_far_position,optimal_radius);

        // multiply by global radius, as the optimal radius is only valid in the projected 2D space between 0 and 1
        optimal_radius*=(global_radius); 
        optimal_radius=multiple_of(optimal_radius,2);

        // switch near to far positions for the next layer
        for(int i=0;i<4;i++) {
            subfrustum_near_position[i] = subfrustum_far_position[i];
        }

        float camera_height=DIRECTIONAL_LIGHT_HEIGHT;

        //clamping camera coordonnates to move depth_test pixel by depth_test pixel to avoid shadow glitters 
        Vec3<float> cam_pos = global_light_projection_inv*optimal_center-ldir_norm*camera_height;

        // projection of the camera position onto the light-view plane
        float X=cam_pos.scalar(lsp_x), Y=cam_pos.scalar(lsp_y), Z=cam_pos.scalar(ldir_norm);
        X=multiple_of(X,optimal_radius/DEPTH_TEXTURE_SIZE);
        Y=multiple_of(Y,optimal_radius/DEPTH_TEXTURE_SIZE);
        cam_pos=lsp_x*X+lsp_y*Y+ldir_norm*Z;

        Vec3<float> cam_pointing_pos=cam_pos+ldir_norm*camera_height;
        // light-view transformation matrix calculation
        camera_mat.camera(cam_pos,cam_pointing_pos,lsp_x);
        light_mat.perspective_ortho(optimal_radius,scene_near,camera_height*2,1);
        light_mat = light_mat*camera_mat;

        // matrix send to pixel shader (phong)
        std::stringstream uniform_name;
        uniform_name<<"matrix"<<(cascaded_layer+1);
        uniform_light->set_value(light_mat,uniform_name.str());

        fbo_shadows.attach_texture(dirlight->get_depth_texture(),FBO_DEPTH,cascaded_layer);

        if(fbo_shadows.iscomplete()) {
            fbo_shadows.bind();
            uniform_light_projection->set_value(light_mat,"matrix");

            frustum.orthogonal_frustum(cam_pos,ldir_norm,lsp_y,optimal_radius,1,camera_height*2);

            draw_scene();
            
            fbo_shadows.unbind();
        }  else {
            Logger::log()<<"FBO incomplete for the render of directional light "<<std::endl;
        }  
        
    }

    uniform_light_sampler->set_value(dirlight->get_depth_texture());
}

Vec3<float> Scene::calculate_shadowing_optimal_point(Vec3<float> near_values[4],Vec3<float> far_values[4], float &radius) {
    float max_top=near_values[0].y,max_bot=near_values[0].y,max_right=near_values[0].x,max_left=near_values[0].x;

    float depth=0;

    for(int i=0;i<4;i++) {
        float cur_max_right = maxf(near_values[i].x,far_values[i].x);
        float cur_max_left = minf(near_values[i].x,far_values[i].x);
        float cur_max_top = maxf(near_values[i].y,far_values[i].y);
        float cur_max_bot = minf(near_values[i].y,far_values[i].y);

        max_top=maxf(max_top,cur_max_top);
        max_bot=minf(max_bot,cur_max_bot);
        max_right=maxf(max_right,cur_max_right);
        max_left=minf(max_left,cur_max_left);
        
        depth+=(near_values[0].z+far_values[0].z);
    }

    float max_height=max_top-max_bot;
    float max_width=max_right-max_left;

    radius=maxf(max_height,max_width);
    return Vec3<float>((max_right+max_left)/2,(max_top+max_bot)/2,depth/8);
}

void Scene::draw_scene() {
    disp->new_draw();

    if(camera_changed) {
        globalvalues->set_value(camera_pos,"camera_pos");
    }

    std::list<Object*> drawn;
    draw_octree(octree,true,drawn);
 
    int i=0;
    std::list<Object*>::iterator it;
    for(it=drawn.begin();it!=drawn.end();it++) {
        Object *o=*it;
        o->has_been_drawn=false;
        i++;
    }


    if(camera_changed) {
        camera_changed=false;
    }

}

void Scene::draw_octree(Octree &oct,bool testcollision,std::list<Object*> &drawn) {
    if(oct.objects.size()!=0) {

        std::list<Object*>::iterator it;
        for(it=oct.objects.begin();it!=oct.objects.end();it++) {
            Object *o=*it;
            if(!o->has_been_drawn && o->enable_draw()) {

                o->update_matrices(&perspective,&camera);

                draw_object(o);

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
                        draw_octree(*oct.nodes[i],false,drawn);
                    }
                }
            } else if(col==IN) {
                for(int i=0;i<8;i++) {
                    if(oct.nodes[i]!=NULL) {
                        draw_octree(*oct.nodes[i],true,drawn);
                    }
                }
            } 
        } else {
            for(int i=0;i<8;i++) {
                if(oct.nodes[i]!=NULL) {
                    draw_octree(*oct.nodes[i],false,drawn);
                }
            }
        }

    }
}

void Scene::draw_object(Object *o) {
    Vec3<float> object_position((o->modelview_matrix()).val[3],
                                (o->modelview_matrix()).val[7],
                                (o->modelview_matrix()).val[11]);
    Vec3<float> objminuscam = object_position - camera_pos;

    globalvalues->set_value(o->modelview_matrix(),"modelview");
    
    globalvalues->set_value(o->projection_modelview_matrix(),"projection_modelview");

    globalvalues->set_value(o->normal_matrix(),"normal_matrix");

    disp->use_program(o->get_program());

    o->draw(objminuscam.norm());
}

void Scene::display_texture(Display_Texture tex) {
    displayed_texture=tex;
    if(tex==DT_NONE) {
        uniform_displaytex_tex->set_value(postprocess_flip2);
        uniform_displaytex_choice->set_value(-1);
    } else if(tex==DT_CASCADED1) {
        uniform_displaytex_arraytex->set_value(lights[0]->get_depth_texture());
        uniform_displaytex_choice->set_value(0);
    } else if(tex==DT_CASCADED2) {
        uniform_displaytex_arraytex->set_value(lights[0]->get_depth_texture());
        uniform_displaytex_choice->set_value(1);
    } else if(tex==DT_CASCADED3) {
        uniform_displaytex_arraytex->set_value(lights[0]->get_depth_texture());
        uniform_displaytex_choice->set_value(2);
    } else if(tex==DT_CASCADED4) {
        uniform_displaytex_arraytex->set_value(lights[0]->get_depth_texture());
        uniform_displaytex_choice->set_value(3);
    } else if(tex==DT_CASCADED5) {
        uniform_displaytex_arraytex->set_value(lights[0]->get_depth_texture());
        uniform_displaytex_choice->set_value(4);
    } else if(tex==DT_CASCADED6) {
        uniform_displaytex_arraytex->set_value(lights[0]->get_depth_texture());
        uniform_displaytex_choice->set_value(5);
    } else if(tex==DT_CASCADED7) {
        uniform_displaytex_arraytex->set_value(lights[0]->get_depth_texture());
        uniform_displaytex_choice->set_value(6);
    } else if(tex==DT_CASCADED8) {
        uniform_displaytex_arraytex->set_value(lights[0]->get_depth_texture());
        uniform_displaytex_choice->set_value(7);
    } else if(tex==DT_DEPTH) {
        uniform_displaytex_tex->set_value(deferred_depthmap);
        uniform_displaytex_choice->set_value(-2);
    } else if(tex==DT_NORMAL) {
        uniform_displaytex_tex->set_value(deferred_normalmap);
        uniform_displaytex_choice->set_value(-1);
    } else if(tex==DT_COLOR) {
        uniform_displaytex_tex->set_value(deferred_colormap);
        uniform_displaytex_choice->set_value(-1);
    } else if(tex==DT_TEXCOORD) {
        uniform_displaytex_tex->set_value(deferred_texcoordmap);
        uniform_displaytex_choice->set_value(-1);
    }
}
