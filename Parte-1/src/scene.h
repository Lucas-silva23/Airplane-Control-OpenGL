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

class scene{
    public:
      scene();
      ~scene();
      void render();
      void set_color(float r,float g,float b);
      void perspective(float fovy, float aspect, float zNear, float zFar);
      void LookAt(float eyex,float eyey,float eyez,
                  float centerx,float centery,float centerz,
                  float upx,float upy,float upz);
      void Model(glm::mat4 model_matrix);
      void push_back_object(object *new_object);
      void push_back_objects(vector<object*> new_objects);
      void set_wireframe(bool on_wireframe);
      void update_camera(object* airplane);
    private:
      GLuint ShaderProgram;
      glm::mat4 Projection_matrix;
      glm::mat4 View;
      vector<object*> my_objects;

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
  Projection_matrix = glm::mat4(1.0);
  View = glm::mat4(1.0);

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

void scene::render(){
    glUseProgram(ShaderProgram);
    GLint color_u = glGetUniformLocation(ShaderProgram,"Color");
    GLint projection_u = glGetUniformLocation(ShaderProgram,"Projection");
    GLint model_u = glGetUniformLocation(ShaderProgram,"Model");
    GLint view_u = glGetUniformLocation(ShaderProgram,"View");
    GLint position = glGetAttribLocation(ShaderProgram,"Position");
    glEnableVertexAttribArray(position);

    for (auto it = my_objects.begin(); it != my_objects.end(); ++it)
    { 
        glUniform3fv(color_u,1,(**it).color);
        glUniformMatrix4fv(projection_u,1,GL_FALSE,glm::value_ptr(Projection_matrix)); 
        glUniformMatrix4fv(view_u,1,GL_FALSE,glm::value_ptr(View)); 
        glUniformMatrix4fv(model_u,1,GL_FALSE,glm::value_ptr(((**it).Model_matrix)));
        (**it).render(position);
    }
  
    
}

void scene::set_color(float r,float g,float b){
  for (auto it = my_objects.begin(); it != my_objects.end(); ++it)
  { 
      (**it).set_color(r,g,b);
  }
  return;
}

void scene::set_wireframe(bool on_wireframe){
  for (auto it = my_objects.begin(); it != my_objects.end(); ++it)
  { 
      (**it).set_wireframe(true);
  }
  return;
}

void scene::perspective(float fovy, float aspect, float zNear, float zFar){
  Projection_matrix = glm::perspective(fovy,aspect, zNear, zFar);
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

void scene::push_back_objects(vector<object*> new_objects){
     for (auto it = new_objects.begin(); it != new_objects.end(); ++it)
     {
        my_objects.push_back(*it);
     }
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