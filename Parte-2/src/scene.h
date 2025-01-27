#ifndef SCENE_H__
#define SCENE_H__

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "utils.h"
using namespace std;
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "object.h"
#include "shadow_map.h"

struct LightProperties {
   bool is_camera_coordinate;
   bool isEnabled;
   bool isLocal;
   bool isSpot;
   glm::vec3 ambient;
   glm::vec3 color;
   glm::vec3 position;
   glm::vec3 coneDirection;
   float spotCosCutoff;
   float spotExponent;
   float constantAttenuation;
   float linearAttenuation;
   float quadraticAttenuation;
};


class scene{
    public:
      scene();
      ~scene();
      void render();
      void set_color(float r,float g,float b);
      void Ortho3D(float WL,float WR,float WB, float WT,float zNear,float zFar);
      void perspective(float fovy, float aspect, float zNear, float zFar);
      void LookAt(float eyex,float eyey,float eyez,
                  float centerx,float centery,float centerz,
                  float upx,float upy,float upz);
      void Model(glm::mat4 model_matrix);
      void set_light(int light_number, LightProperties my_light);
      void push_back_object(object *new_object);
      void push_back_objects(vector<object*> new_objects);
      void set_shadow_light_0(float fovy,float light_target_x,float light_target_y,float light_target_z);
      void set_shadow_on(bool Shadow_on){shadow_on = Shadow_on;};
      void update_camera(object* airplane);
    private:
      int Width,Height;
      void renderShadow();
      GLuint ShaderProgram;
      GLuint ShaderProgramShadow;
      glm::mat4 Projection_matrix;
      glm::mat4 View;

      glm::mat4 Projection_matrixShadow;
      glm::mat4 ViewShadow;
      vector<object*> my_objects;      
      LightProperties lights[8];
      const int MaxLights = 8;
      //only light 0 has shadow
      //shadow has a target location
      //shadow has fovy
      bool shadow_on;
      glm::vec3 shadow_target;
      float shadow_fovy;  
      ShadowMapFBO *shadow_map;

      glm::vec3 camera_position;       // Posição atual da câmera
      glm::vec3 camera_target;         // Ponto que a câmera está olhando
      glm::vec3 camera_up;             // Vetor "up" da câmera
      float camera_distance;           // Distância fixa entre a câmera e o avião
      float camera_smoothness;         // Fator de suavidade para movimentação


};
scene::~scene(){
  for (auto it = my_objects.begin(); it != my_objects.end(); ++it)
  { 
     delete (*it);
  }
}

scene::scene(){
   const char* pVSFileName = "../src/shader.vs";
   const char* pFSFileName = "../src/shader.fs";
   ShaderProgram = CompileShaders(pVSFileName,pFSFileName);

   Width = 800;
   Height = 800;
   // Program to shadow map

   const char* pVSFileNameShadow = "../src/shaderShadow.vs";
   const char* pFSFileNameShadow = "../src/shaderShadow.fs";
   ShaderProgramShadow = CompileShaders(pVSFileNameShadow,pFSFileNameShadow);
   

   shadow_map = new ShadowMapFBO();
   shadow_map->Init(1800,1800);
   
   Projection_matrix = glm::mat4(1.0);
   View = glm::mat4(1.0);

 

   shadow_on = false;
   shadow_fovy = 0.9;
   shadow_target[0] = 0.0;
   shadow_target[1] = 0.0;
   shadow_target[2] = 0.0;

   for(int i=1;i < MaxLights;i++)
      lights[i].isEnabled = false;
      
   lights[0].is_camera_coordinate = true;
   lights[0].isEnabled = true;
   lights[0].isLocal = true;
   lights[0].isSpot = false;

   lights[0].ambient[0] = lights[0].ambient[1] = lights[0].ambient[2] = 0.3;
   lights[0].color[0] = lights[0].color[1] = lights[0].color[2] = 1.0;
   lights[0].position[0] =  0.0;
   lights[0].position[1] = 5.0;
   lights[0].position[2] = 0.0;
   
   lights[0].constantAttenuation = 1.0;
   lights[0].linearAttenuation = 0.0;
   lights[0].quadraticAttenuation = 0.0;

    Projection_matrixShadow = glm::perspective(0.9,1.0,0.1,20.0);
   ViewShadow = glm::lookAt(
       glm::vec3(lights[0].position[0],lights[0].position[1],lights[0].position[1]),
       glm::vec3(shadow_target[0],shadow_target[0],shadow_target[0]),
       glm::vec3(0.0,1.0,0.0));
   
   // Inicializa a câmera
  camera_position = glm::vec3(0.0f, 5.0f, 10.0f);
  camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
  camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
  camera_distance = 20.0f;          // Distância da câmera ao avião
  camera_smoothness = 0.1f;         // Suavidade no movimento da câmera
}

void scene::push_back_object(object *new_object){
   my_objects.push_back(new_object);
}

void scene::renderShadow(){

    glUseProgram(ShaderProgramShadow);
    GLint color_u = glGetUniformLocation(ShaderProgramShadow,"Color");
    GLint mvp_u = glGetUniformLocation(ShaderProgramShadow,"MVPMatrix");
    GLint mv_u = glGetUniformLocation(ShaderProgramShadow,"MVMatrix");
    GLint normal_u = glGetUniformLocation(ShaderProgramShadow,"NormalMatrix");
    
    GLint position = glGetAttribLocation(ShaderProgramShadow,"Position");
   
    glEnableVertexAttribArray(position);
    GLint normal = glGetAttribLocation(ShaderProgramShadow,"Normal");
    glEnableVertexAttribArray(normal);

    GLint texcoord = glGetAttribLocation(ShaderProgramShadow,"TexCoord");
    glEnableVertexAttribArray(texcoord);

    //Light 
    for(int i = 0;i < MaxLights;i++){
        string light_name = "Lights[" + std::to_string(i) + "]";
        GLint isEnabled_u = glGetUniformLocation(ShaderProgramShadow,(light_name + ".isEnabled").c_str() );
        GLint isLocal_u = glGetUniformLocation(ShaderProgramShadow,(light_name + ".isLocal").c_str() );
        GLint isSpot_u = glGetUniformLocation(ShaderProgramShadow,(light_name + ".isSpot").c_str() );
        GLint ambient_u = glGetUniformLocation(ShaderProgramShadow,(light_name + ".ambient").c_str() );
        GLint lightcolor_u = glGetUniformLocation(ShaderProgramShadow,(light_name + ".color").c_str() );
        GLint lightposition_u = glGetUniformLocation(ShaderProgramShadow,(light_name + ".position").c_str() );
       
        GLint constantAttenuation_u = glGetUniformLocation(ShaderProgramShadow,(light_name + ".constantAttenuation").c_str() );
        GLint linearAttenuation_u = glGetUniformLocation(ShaderProgramShadow,(light_name + ".linearAttenuation").c_str() );
        GLint quadraticAttenuation_u = glGetUniformLocation(ShaderProgramShadow,(light_name + ".quadraticAttenuation").c_str() );

        glUniform1i(isEnabled_u,lights[i].isEnabled);
        glUniform1i(isLocal_u,lights[i].isLocal);
        glUniform1i(isSpot_u,lights[i].isSpot);
        glUniform3fv(ambient_u,1,glm::value_ptr(lights[i].ambient));
        glUniform3fv(lightcolor_u,1,glm::value_ptr(lights[i].color));
        
        //TODO: TEST is_camera_coordinate and isLocal
        glUniform3fv(lightposition_u,1,glm::value_ptr(lights[i].position));

        //TODO: INCLUDE OTHER Light Properties
        
        glUniform1f(constantAttenuation_u,lights[i].constantAttenuation);
        glUniform1f(linearAttenuation_u,lights[i].linearAttenuation);
        glUniform1f(quadraticAttenuation_u,lights[i].quadraticAttenuation);

    }

    
    //Material
    GLint shininess_u = glGetUniformLocation(ShaderProgramShadow,"Shininess");
    GLint strength_u = glGetUniformLocation(ShaderProgramShadow,"Strength");
    GLint texture_combiner_u = glGetUniformLocation(ShaderProgramShadow,"Texture_combiner");

    GLint gSampler = glGetUniformLocation(ShaderProgramShadow, "gSampler");
    //TEXTURE0 is difuse color
    glUniform1i(gSampler, 0);
    

    for (auto it = my_objects.begin(); it != my_objects.end(); ++it)
    { 
        glUniform1f(shininess_u,(**it).Shininess);
        glUniform1f(strength_u,(**it).Strength);
        glUniform1i(texture_combiner_u,(**it).Texture_combiner);

        glUniform3fv(color_u,1,(**it).color);
        glUniformMatrix4fv(mvp_u,1,GL_FALSE,glm::value_ptr(Projection_matrixShadow*ViewShadow*((**it).Model_matrix))); 
        glUniformMatrix4fv(mv_u,1,GL_FALSE,glm::value_ptr(ViewShadow*((**it).Model_matrix))); 
        glUniformMatrix3fv(normal_u,1,GL_FALSE,glm::value_ptr(glm::inverseTranspose(glm::mat3(ViewShadow*((**it).Model_matrix)))));
        (**it).render(position,normal,texcoord);

    }
  
    
}


void scene::render(){
      
    shadow_map->BindForWriting();
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);  
    renderShadow();        
    shadow_map->BindForReading(GL_TEXTURE1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //SalvarTexura2Image("saida.bmp",shadow_map->getWidth(),shadow_map->getHeight(),1);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,Width,Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    glUseProgram(ShaderProgram);
    
    

    GLint color_u = glGetUniformLocation(ShaderProgram,"Color");
      
    GLint mvp_u = glGetUniformLocation(ShaderProgram,"MVPMatrix");
    GLint mvps_u = glGetUniformLocation(ShaderProgram,"MVPMatrixShadow");
    GLint mv_u = glGetUniformLocation(ShaderProgram,"MVMatrix");
    GLint normal_u = glGetUniformLocation(ShaderProgram,"NormalMatrix");
    
    

    GLint position = glGetAttribLocation(ShaderProgram,"Position");
    
    glEnableVertexAttribArray(position);

    GLint normal = glGetAttribLocation(ShaderProgram,"Normal");

    glEnableVertexAttribArray(normal);
  
    GLint texcoord = glGetAttribLocation(ShaderProgram,"TexCoord");
    glEnableVertexAttribArray(texcoord);

    //Light 
    for(int i = 0;i < MaxLights;i++){
        string light_name = "Lights[" + std::to_string(i) + "]";
        GLint isEnabled_u = glGetUniformLocation(ShaderProgram,(light_name + ".isEnabled").c_str() );
        GLint isLocal_u = glGetUniformLocation(ShaderProgram,(light_name + ".isLocal").c_str() );
        GLint isSpot_u = glGetUniformLocation(ShaderProgram,(light_name + ".isSpot").c_str() );
        GLint ambient_u = glGetUniformLocation(ShaderProgram,(light_name + ".ambient").c_str() );
        GLint lightcolor_u = glGetUniformLocation(ShaderProgram,(light_name + ".color").c_str() );
        GLint lightposition_u = glGetUniformLocation(ShaderProgram,(light_name + ".position").c_str() );
       
        GLint constantAttenuation_u = glGetUniformLocation(ShaderProgram,(light_name + ".constantAttenuation").c_str() );
        GLint linearAttenuation_u = glGetUniformLocation(ShaderProgram,(light_name + ".linearAttenuation").c_str() );
        GLint quadraticAttenuation_u = glGetUniformLocation(ShaderProgram,(light_name + ".quadraticAttenuation").c_str() );

        glUniform1i(isEnabled_u,lights[i].isEnabled);
        glUniform1i(isLocal_u,lights[i].isLocal);
        glUniform1i(isSpot_u,lights[i].isSpot);
        glUniform3fv(ambient_u,1,glm::value_ptr(lights[i].ambient));
        glUniform3fv(lightcolor_u,1,glm::value_ptr(lights[i].color));
        
        //TODO: TEST is_camera_coordinate and isLocal
        glUniform3fv(lightposition_u,1,glm::value_ptr(lights[i].position));

        //TODO: INCLUDE OTHER Light Properties
        
        glUniform1f(constantAttenuation_u,lights[i].constantAttenuation);
        glUniform1f(linearAttenuation_u,lights[i].linearAttenuation);
        glUniform1f(quadraticAttenuation_u,lights[i].quadraticAttenuation);
        
    }

    
    //Material
    GLint shininess_u = glGetUniformLocation(ShaderProgram,"Shininess");
    GLint strength_u = glGetUniformLocation(ShaderProgram,"Strength");
    GLint texture_combiner_u = glGetUniformLocation(ShaderProgram,"Texture_combiner");

    GLint gSampler = glGetUniformLocation(ShaderProgram, "gSampler");
    //TEXTURE0 is difuse color
    glUniform1i(gSampler, 0);

    GLint gShadowMap = glGetUniformLocation(ShaderProgram, "gShadowMap");
    //TEXTURE1 is shadow map

    
    glUniform1i(gShadowMap, 1);
    
    
    
    int i = 0;
    for (auto it = my_objects.begin(); it != my_objects.end(); ++it)
    { 
        glUniform1f(shininess_u,(**it).Shininess);
        glUniform1f(strength_u,(**it).Strength);
        glUniform1i(texture_combiner_u,(**it).Texture_combiner);

        glUniform3fv(color_u,1,(**it).color);
        glUniformMatrix4fv(mvp_u,1,GL_FALSE,glm::value_ptr(Projection_matrix*View*((**it).Model_matrix))); 
        glUniformMatrix4fv(mvps_u,1,GL_FALSE,glm::value_ptr(Projection_matrixShadow*ViewShadow*((**it).Model_matrix))); 

        
        glUniformMatrix4fv(mv_u,1,GL_FALSE,glm::value_ptr(View*((**it).Model_matrix))); 
        glUniformMatrix3fv(normal_u,1,GL_FALSE,glm::value_ptr(glm::inverseTranspose(glm::mat3(View*((**it).Model_matrix)))));
        (**it).render(position,normal,texcoord);
        //if(i == 1){
            //SalvarTexura2Image("saida.bmp",1000,500,3,false);
        //}
        //i++;
    }
  
    
}

void scene::set_color(float r,float g,float b){
  for (auto it = my_objects.begin(); it != my_objects.end(); ++it)
  { 
      (**it).set_color(r,g,b);
  }
  return;
}

void scene::Ortho3D(float WL,float WR,float WB, float WT,float zNear, float zFar){
    
    Projection_matrix = glm::ortho(
WL, WR,
WB, WT,
zNear, zFar);

    return;
}

void scene::Model(glm::mat4 model_matrix){
    for (auto it = my_objects.begin(); it != my_objects.end(); ++it)
    { 
      (**it).push_left_matrix(model_matrix);
    }
    return;
}

void scene::LookAt(float eyex,float eyey,float eyez,
                  float centerx,float centery,float centerz,
                  float upx,float upy,float upz){

    View = glm::lookAt(
       glm::vec3(eyex,eyey,eyez),
       glm::vec3(centerx,centery,centerz),
       glm::vec3(upx,upy,upz));

}

void scene::perspective(float fovy, float aspect, float zNear, float zFar){
  Projection_matrix = glm::perspective(fovy,aspect, zNear, zFar);
}

void scene::push_back_objects(vector<object*> new_objects){
     for (auto it = new_objects.begin(); it != new_objects.end(); ++it)
     {
        my_objects.push_back(*it);
     }
}

void scene::set_light(int light_number, LightProperties my_light){
          if (light_number >= MaxLights || light_number < 0)
             return;
            
          //Light 0 should be local to cast shadow
          if(light_number==0 && (!my_light.isLocal))
               return;
          lights[light_number] = my_light;
          if(light_number==0)
              set_shadow_light_0(shadow_fovy,shadow_target[0],shadow_target[0],shadow_target[0]);
        
          return;      
   
}

void scene::set_shadow_light_0(float fovy,float light_target_x,float light_target_y,float light_target_z){
     shadow_fovy = fovy;
     shadow_target[0] = light_target_x;
     shadow_target[1] = light_target_y;
     shadow_target[2] = light_target_z;
     Projection_matrixShadow = glm::perspective(shadow_fovy,1.0f,0.1f,20.0f);
     
     ViewShadow = glm::lookAt(
       glm::vec3(lights[0].position[0],lights[0].position[1],lights[0].position[2]),
       glm::vec3(shadow_target[0],shadow_target[0],shadow_target[0]),
       glm::vec3(1.0,0.0,0.0));
      
}

void scene::update_camera(object* airplane) {
  // Obtém a matriz de modelo do avião
  glm::mat4 airplane_model = airplane->get_Model_matrix();

  // Extrai a posição do avião (coluna 3 da matriz de modelo)
  glm::vec3 airplane_position = glm::vec3(airplane_model[3]);

  // Calcula a direção para trás do avião (coluna 2 da matriz de modelo, mas invertida)
  glm::vec3 airplane_backward = -glm::vec3(airplane_model[2]);

  // Calcula a nova posição-alvo da câmera (incluindo elevação)
  glm::vec3 target_position = airplane_position + airplane_backward * camera_distance + glm::vec3(0.0f, 20.0f, -3.0f);

  // Suaviza a movimentação da câmera usando interpolação linear
  camera_position = glm::mix(camera_position, target_position, camera_smoothness);

  // Define o alvo da câmera, ajustando o ponto de foco para baixo
  camera_target = airplane_position - glm::vec3(0.1f, -2.0f, -2.0f);

  // Atualiza a matriz de visualização (LookAt)
  View = glm::lookAt(camera_position, camera_target, camera_up);
}


#endif