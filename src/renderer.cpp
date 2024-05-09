#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include "model.hpp"
#include <_gl.hpp>
#include <SDL2/SDL.h>

#include <cmath>
#include <cstdio>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>

#include <log.hpp>
#include <r3dgl.hpp>
#include <camera.hpp>

#include <renderer.hpp>
#include <string>

Renderer3d::Renderer3d(int width, int height)
	: scr_w(width), scr_h(height){

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	float cube[] = {

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

	cubeVAO.New();
	cubeVAO.Bind();

	cubeVBO.New();
	cubeVBO.Bind();
	cubeVBO.Data(sizeof(cube), cube, GL_STATIC_DRAW);

	cubeVAO.EnableAttrib(0);
	cubeVAO.AttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
    cubeVAO.Unbind();

    hdriToCubeShader.Load("assets/shaders/hdri_to_cube.vs", "assets/shaders/hdri_to_cube.fs");
    d_irMapShader.Load("assets/shaders/d_irMap.vs", "assets/shaders/d_irMap.fs");
    s_irMapShader.Load("assets/shaders/s_irMap.vs", "assets/shaders/s_irMap.fs");
    brdfShader.Load("assets/shaders/brdf.vs", "assets/shaders/brdf.fs");
    pbrShader.Load("assets/shaders/pbr.vs", "assets/shaders/pbr.fs");
	envShader.Load("assets/shaders/hdri.vs", "assets/shaders/hdri.fs");

	proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
	pbrShader.Use();
	pbrShader.SetMatrix4fv("proj", glm::value_ptr(proj));
    pbrShader.Seti("env", 0);
}

void Renderer3d::LoadHDRI(const char *filepath){

    glDisable(GL_CULL_FACE);

    FrameBuffer fbo;
    fbo.New();

    RenderBuffer depthBuffer;
    depthBuffer.New(GL_DEPTH_COMPONENT24, 512, 512);
    fbo.Attach(depthBuffer, GL_DEPTH_ATTACHMENT);
    fbo.Bind();

    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

    glm::mat4 views[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, -1.0f, 0.0f))
    };

	hdriMap.Delete();
    hdriMap.New(GL_RGB16F, GL_RGB, 512, 512, GL_FLOAT, nullptr);
    hdriMap.Bind(0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    Texture2d hdriTexture;

    stbi_set_flip_vertically_on_load(true);
    int w, h, nrc;
    float *data = stbi_loadf(filepath, &w, &h, &nrc, 0);

    if(data){

        hdriTexture.New(GL_RGB16F, GL_RGB, w, h, GL_FLOAT, data);
        hdriTexture.Bind(0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

    }else{
        LOGERR("error : loading file : %s\n", filepath);
    }

    hdriToCubeShader.Use();
    hdriToCubeShader.Seti("equirectangularMap", 0);
    hdriToCubeShader.SetMatrix4fv("proj", glm::value_ptr(proj));

    glViewport(0, 0, 512, 512);

    cubeVAO.Bind();
    hdriToCubeShader.Use();
    for(int i = 0; i < 6; i++){

        hdriToCubeShader.SetMatrix4fv("view", glm::value_ptr(views[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, hdriMap.id, 0);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    hdriMap.Bind(0);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // for(int i = 0; i < 6; i++){
    //     unsigned char imgdata[3*512*512];
    //     glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB, GL_UNSIGNED_BYTE, &imgdata);
    //     stbi_write_jpg(("misc/sky"+std::to_string(i)+".jpg").c_str(), 512, 512, 3, imgdata, 90);
    // }

    hdriTexture.Delete();

    depthBuffer.Delete();
    depthBuffer.New(GL_DEPTH_COMPONENT24, 32, 32);
    fbo.Attach(depthBuffer, GL_DEPTH_ATTACHMENT);
    fbo.Bind();
    glViewport(0, 0, 32, 32);

    d_irMap.Delete();
    d_irMap.New(GL_RGB16F, GL_RGB, 32, 32, GL_FLOAT, nullptr);
    d_irMap.Bind(0);

    d_irMapShader.Use();
    d_irMapShader.SetMatrix4fv("proj", glm::value_ptr(proj));
    d_irMapShader.Seti("hdriMap", 1);
    hdriMap.Bind(1);

    cubeVAO.Bind();
    d_irMapShader.Use();
    for(int i = 0; i < 6; i++){

        d_irMapShader.SetMatrix4fv("view", glm::value_ptr(views[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, d_irMap.id, 0);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // d_irMap.Bind(0);
    // for(int i = 0; i < 6; i++){
    //     unsigned char imgdata[3*32*32];
    //     glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB, GL_UNSIGNED_BYTE, &imgdata);
    //     stbi_write_jpg(("misc/d_ir"+std::to_string(i)+".jpg").c_str(), 32, 32, 3, imgdata, 90);
    // }

    s_irMap.Delete();
    s_irMap.New(GL_RGB16F, GL_RGB, 128, 128, GL_FLOAT, nullptr);
    s_irMap.Bind(0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    hdriMap.Bind(1);

    s_irMap.Bind(0);
    cubeVAO.Bind();
    s_irMapShader.Use();
    s_irMapShader.SetMatrix4fv("proj", glm::value_ptr(proj));
    s_irMapShader.Seti("hdriMap", 1);

    int mmm = 5;
    for(int mip = 0; mip < mmm; mip++){

        unsigned int mipWidth = 128 * std::pow(0.5, mip);

        depthBuffer.Delete();
        depthBuffer.New(GL_DEPTH_COMPONENT24, mipWidth, mipWidth);
        fbo.Attach(depthBuffer, GL_DEPTH_ATTACHMENT);
        glViewport(0, 0, mipWidth, mipWidth);

        float roughness = (float)mip / (float)(mmm-1);
        s_irMapShader.Setf("roughness", roughness);

        fbo.Attach(depthBuffer, GL_DEPTH_ATTACHMENT);
        fbo.Bind();
        for(int i = 0; i < 6; i++){

            s_irMapShader.SetMatrix4fv("view", glm::value_ptr(views[i]));
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, s_irMap.id, mip);

            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    fbo.Unbind();

    // s_irMap.Bind(0);
    // for(int i = 0; i < 6; i++){
    //     unsigned char imgdata[3*16*16];
    //     glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 3, GL_RGB, GL_UNSIGNED_BYTE, &imgdata);
    //     stbi_write_jpg(("misc/s_ir"+std::to_string(i)+".jpg").c_str(), 16, 16, 3, imgdata, 90);
    // }

    if(!brdfLUT_ready){

        depthBuffer.Delete();
        depthBuffer.New(GL_DEPTH_COMPONENT24, 512, 512);
        fbo.Attach(depthBuffer, GL_DEPTH_ATTACHMENT);

        glViewport(0, 0, 512, 512);

        brdfLUT.Delete();
        brdfLUT.New(GL_RG16F, GL_RG, 512, 512, GL_FLOAT, nullptr);
        brdfLUT.Bind(0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        brdfShader.Use();
        brdfShader.SetMatrix4fv("proj", glm::value_ptr(proj));
        brdfShader.SetMatrix4fv("view", glm::value_ptr(views[1]));
        fbo.Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT.id, 0);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        fbo.Unbind();

        brdfLUT_ready = true;
    }

    fbo.Delete();
    depthBuffer.Delete();
    glEnable(GL_CULL_FACE);
    // LOGERR("setting glViewport %d %d", w, h);
    glViewport(0, 0, scr_w, scr_h);
}

void Renderer3d::BeginDraw(CameraFly &camera){

    glm::mat4 view = camera.LookAtMat();

    glDisable(GL_CULL_FACE);
    cubeVAO.Bind();
    envShader.Use();
    hdriMap.Bind(0);
    // d_irMap.Bind(0);
    envShader.Seti("env", 0);
    envShader.SetMatrix4fv("proj", glm::value_ptr(proj));
    envShader.SetMatrix4fv("view", glm::value_ptr(view));

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glClear(GL_DEPTH_BUFFER_BIT);

    cubeVAO.Unbind();

    pbrShader.Use();
    pbrShader.SetMatrix4fv("proj", glm::value_ptr(proj));
    pbrShader.SetMatrix4fv("view", glm::value_ptr(view));
    pbrShader.Set3f("camPos", camera.pos.x, camera.pos.y, camera.pos.z);

    hdriMap.Bind(0);
    d_irMap.Bind(1);
    s_irMap.Bind(2);
    brdfLUT.Bind(3);
    pbrShader.Seti("hdriMap", 0);
    pbrShader.Seti("d_irMap", 1);
    pbrShader.Seti("s_irMap", 2);
    pbrShader.Seti("brdfLUT", 3);
    pbrShader.Seti("albedo_tex", 4);
    pbrShader.Seti("spec_tex", 5);
    pbrShader.Seti("rough_tex", 6);
    pbrShader.Seti("metal_tex", 7);
    pbrShader.Seti("normal_tex", 8);
    pbrShader.Seti("ao_tex", 9);
}

void Renderer3d::Draw(Model &model){

    int prev_mat = -1;

    glm::mat4 trans(1.0);
    trans = glm::translate(trans, model.position);
    trans = glm::rotate(trans, glm::radians(model.rotation.z), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::rotate(trans, glm::radians(model.rotation.y), glm::vec3(0.0, 1.0, 0.0));
    trans = glm::rotate(trans, glm::radians(model.rotation.x), glm::vec3(1.0, 0.0, 0.0));
    trans = glm::scale(trans, model.scale);
    pbrShader.SetMatrix4fv("model", glm::value_ptr(trans));

    for(int i = 0; i < model.meshes.size(); i++){

        Mesh &mesh = model.meshes[i];
        
        if(mesh.mat_id > -1){

            Material &mat = model.materials[mesh.mat_id];

            if(mat.diffuse.id){
                pbrShader.Seti("albedo_b", 1);
                mat.diffuse.Bind(4);
            }else
                pbrShader.Seti("albedo_b", 0);

            // if(mat.specular.id){
            //     pbrShader.Seti("spec_b", 1);
            //     mat.specular.Bind(5);
            // }else
            //     pbrShader.Seti("spec_b", 0);

            if(mat.roughness.id){
                pbrShader.Seti("rough_b", 1);
                // LOGERR("r_channel %f %f %f\n", mat.r_channel.x, mat.r_channel.y, mat.r_channel.z);
                pbrShader.Set3f("r_channel", mat.r_channel.x, mat.r_channel.y, mat.r_channel.z);
                mat.roughness.Bind(6);
            }else
                pbrShader.Seti("rough_b", 0);

            if(mat.metallic.id){
                pbrShader.Seti("metal_b", 1);
                pbrShader.Set3f("m_channel", mat.m_channel.x, mat.m_channel.y, mat.m_channel.z);
                mat.metallic.Bind(7);
            }else
                pbrShader.Seti("metal_b", 0);

            if(mat.normal.id){
                pbrShader.Seti("normal_b", 1);
                mat.normal.Bind(8);
            }else
                pbrShader.Seti("normal_b", 0);

             if(mat.ao.id){
                pbrShader.Seti("ao_b", 1);
                pbrShader.Set3f("ao_channel", mat.ao_channel.x, mat.ao_channel.y, mat.ao_channel.z);
                // printf("%d\n", mat.ao.id);
                mat.ao.Bind(9);
            }else
                pbrShader.Seti("ao_b", 0);

            prev_mat = mesh.mat_id;
        }

        mesh.vao.Bind();
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    }
}

void Renderer3d::EndDraw(){

}

Renderer3d::~Renderer3d(){

	cubeVAO.Unbind();
	cubeVAO.Delete();
	cubeVBO.Unbind();
    cubeVBO.Delete();

    hdriMap.Delete();
    d_irMap.Delete();
    s_irMap.Delete();
    brdfLUT.Delete();

    brdfShader.Delete();
    s_irMapShader.Delete();
    d_irMapShader.Delete();
    hdriToCubeShader.Delete();
    envShader.Delete();
    pbrShader.Delete();
}