#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fstream>

#include "scene.h"
#include "object.h"
#include "loadobject.h"

using namespace std;

bool ortho_per = true;
GLint width = 800,height = 800; 

bool helice = false;
bool subindo = false;
bool descendo = false;
bool vira_braco = false;
bool andando = false;

float frente = 0.0f;
float altura = 0.0f;
float lados = 0.0f;

bool direita = false;
bool esquerda = false;
bool cima = false;
bool baixo = false;
bool reto_altura = true;
bool reto_lado = true;

float angulo_altura = 0.0f;
float angulo_lado = 0.0f;

float angulo_helice = 0.0f;
float angulo_braco = 0.0f;

float x_aviao = 0.0f;
float y_aviao = 0.0f;
float z_aviao = 0.0f;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS){
        switch (key)
        {
        case GLFW_KEY_UP:
            helice = true;
            subindo = true;
            descendo = false;
            break;
        case GLFW_KEY_DOWN:
            subindo = false;
            descendo = true;
            frente = false;
            break;
        case GLFW_KEY_W:
            if(baixo){
                reto_altura = true;
            }else{
                cima = true;
                reto_altura = false;
            }
            baixo = false;
            esquerda = false;
            direita = false;
            break;
        case GLFW_KEY_S:
            if(cima){
                reto_altura = true;
            }else{
                baixo = true;
                reto_altura = false;
            }
            cima = false;
            esquerda = false;
            direita = false;
            break;
        case GLFW_KEY_A:
            if(direita){
                reto_lado = true;
            }else{
                esquerda = true;
                reto_lado = false;
            }
            direita = false;
            cima = false;
            baixo = false;
            break;
        case GLFW_KEY_D:
            if(esquerda){
                reto_lado = true;
            }else{
                direita = true;
                reto_lado = false;
            }
            esquerda = false;
            cima = false;
            baixo = false;
            break;
        default:
            break;
        }
    }
}

int main(void)
{
    GLFWwindow* window; 
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
 
    window = glfwCreateWindow(width, height, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
  
    glEnable(GL_DEPTH_TEST);
    scene my_scene;

    //Cria o chão
    GLfloat groundVertices[] = {
        -50.0f, 0.0f, -50.0f,  
        50.0f, 0.0f, -50.0f, 
        50.0f, 0.0f,  50.0f,  
        -50.0f, 0.0f,  50.0f   
    };

    GLushort groundIndices[] = {
        0, 1, 2,  
        2, 3, 0  
    };

    //Cria o objeto chão
    object* ground = new object(
        4,  //vértices
        6,  //índices
        groundVertices,
        groundIndices
    );

    //Configura a cor do chão
    ground->set_color(0.0f, 1.0f, 0.0f);

    my_scene.push_back_object(ground);

    //Carregar as partes do avião
    object* corpo_aviao = read_obj_file("../models/corpo_aviao.obj");
    corpo_aviao->set_color(0.5f, 0.5f, 0.5f);
    my_scene.push_back_object(corpo_aviao);

    object* braco_direito = read_obj_file("../models/braco_direito.obj");
    braco_direito->set_color(0.5f, 0.5f, 0.5f);
    my_scene.push_back_object(braco_direito);

    object* braco_esquerdo = read_obj_file("../models/braco_esquerdo.obj");
    braco_esquerdo->set_color(0.5f, 0.5f, 0.5f);
    my_scene.push_back_object(braco_esquerdo);

    object* turbina_direita = read_obj_file("../models/turbina_direita.obj");
    turbina_direita->set_color(0.5f, 0.5f, 0.5f);
    my_scene.push_back_object(turbina_direita);

    object* turbina_esquerda = read_obj_file("../models/turbina_esquerda.obj");
    turbina_esquerda->set_color(0.5f, 0.5f, 0.5f);
    my_scene.push_back_object(turbina_esquerda);

    object* leme = read_obj_file("../models/leme.obj");
    leme->set_color(0.5f, 0.5f, 0.5f);
    my_scene.push_back_object(leme);

    object* profundor = read_obj_file("../models/profundor.obj");
    profundor->set_color(0.5f, 0.5f, 0.5f);
    my_scene.push_back_object(profundor);

    glClearColor(0.35f, 0.35f, 0.8f, 0.0f);
    glViewport(0, 0, width, height);

    my_scene.LookAt(0.0, 2.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // Atualiza a câmera com base no avião
        my_scene.update_camera(corpo_aviao);

        // Define a projeção ortográfica
        my_scene.perspective(0.75, 1.0, 0.1, 200.0);

        //ANGULO ROTACAO HELICE
        angulo_helice += 7.0f;  // Incremento em graus por frame (ajuste conforme necessário)
        if (angulo_helice >= 360.0f) {
            angulo_helice -= 360.0f;  // Mantém o ângulo entre 0 e 360
        }

        //ANGULO ROTACAO BRACO
        if(vira_braco){
            if(descendo){
                if (angulo_braco > 0.0f) {
                    angulo_braco -= 1.5f;  
                } else {
                    vira_braco = false;
                }
            }else{
                if (angulo_braco < 90.0f) {
                    angulo_braco += 1.5f;  
                } else {
                    vira_braco = false;
                }
            }
        }
        
        //CORPO AVIAO
        if(subindo){
            //SUBIDA
            if(y_aviao <= 30){
                y_aviao += 0.05;
            } else {
                subindo = false;
                andando = true;
                vira_braco = true;
                frente = true;
            }
        }

        if(descendo){
            if(y_aviao > 0){
                y_aviao -= 0.05;
            if(y_aviao < 0){
                y_aviao = 0.0f;
            }
                vira_braco = true;
                andando = false;
                cima = false;
                baixo = false;
                direita = false;
                esquerda = false;
                reto_altura = false;
                reto_lado = false;
            } else {
                andando = false;
                helice = false;
                descendo = false;
            }
        }

        glm::mat4 matrix_corpo_aviao_angulo = glm::mat4(1.0f);

        if(andando){
            if(angulo_lado < 90 && angulo_lado > -90)
                frente = frente + 0.01f;
            else if(angulo_lado > 90 && angulo_lado < 240 || angulo_lado < -90 && angulo_lado > -240)
                frente = frente - 0.01f;
        }

        if(reto_altura){
            if(angulo_altura > 0){
                angulo_altura -= 0.25f;
            }
            if(angulo_altura < 0){
                angulo_altura += 0.25f;
            }

            matrix_corpo_aviao_angulo = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_altura),glm::vec3(1.0,0.0,0.0));
        }

        if(reto_lado){
            matrix_corpo_aviao_angulo = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_lado),glm::vec3(0.0,1.0,0.0));
        }

        if(baixo){
            y_aviao = y_aviao  - 0.1f;
            if(angulo_altura < 25.0f){
                angulo_altura += 0.25f;
            }

            matrix_corpo_aviao_angulo = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_altura),glm::vec3(1.0,0.0,0.0));
        }

        if(cima){
            y_aviao = y_aviao + 0.1f;
            if(angulo_altura > -25.0f){
                angulo_altura -= 0.25f;
            }

            matrix_corpo_aviao_angulo = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_altura),glm::vec3(1.0,0.0,0.0));
        }

        if(direita){
            lados = lados - 0.1f;
            angulo_lado -= 0.35f;
            
            matrix_corpo_aviao_angulo = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_lado),glm::vec3(0.0,1.0,0.0));
        }

        if(esquerda){
            lados = lados + 0.1f;
            angulo_lado += 0.35f;
            
            matrix_corpo_aviao_angulo = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_lado),glm::vec3(0.0,1.0,0.0));
        }

        glm::mat4 matrix_corpo_aviao = glm::translate(glm::mat4(1.0), glm::vec3(x_aviao + lados, y_aviao, z_aviao + frente));
        corpo_aviao->Model(matrix_corpo_aviao_angulo*matrix_corpo_aviao);

        //BRACO DIREITO
        glm::mat4 braco_translation_centro_direito = glm::translate(glm::mat4(1.0), glm::vec3(4.5, -3.0, 0.0));
        glm::mat4 braco_translation_volta_direito = glm::translate(glm::mat4(1.0), -glm::vec3(4.5, -3.0, 0.0));

        glm::mat4 braco_rotation = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_braco),glm::vec3(1.0,0.0,0.0));

        braco_direito->Model(matrix_corpo_aviao_angulo*matrix_corpo_aviao*braco_translation_volta_direito*braco_rotation*braco_translation_centro_direito);

        //BRACO ESQUERDO
        glm::mat4 braco_translation_centro_esquerdo = glm::translate(glm::mat4(1.0), glm::vec3(-4.5, -3.0, 0.0));
        glm::mat4 braco_translation_volta_esquerdo = glm::translate(glm::mat4(1.0), -glm::vec3(-4.5, -3.0, 0.0));

        braco_esquerdo->Model(matrix_corpo_aviao_angulo*matrix_corpo_aviao*braco_translation_volta_esquerdo*braco_rotation*braco_translation_centro_esquerdo);

        //LEME
        leme->Model(matrix_corpo_aviao_angulo*matrix_corpo_aviao);

        //PROFUNDOR
        profundor->Model(matrix_corpo_aviao_angulo*matrix_corpo_aviao);

        if(helice){
            //ROTACAO TURBINA DIREITA
            glm::mat4 helice_translation_direita_centro = glm::translate(glm::mat4(1.0), glm::vec3(4.5, 0.0, 0.0));
            glm::mat4 helice_translation_direita_volta = glm::translate(glm::mat4(1.0), -glm::vec3(4.5, 0.0, 0.0));

            glm::mat4 helice_rotation_eixo;
            glm::mat4 helice_rotation_mundo = glm::mat4(1.0f);
            if(andando)
                helice_rotation_eixo = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_helice),glm::vec3(0.0,0.0,1.0));
            else
                helice_rotation_eixo = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_helice),glm::vec3(0.0,1.0,0.0));

            if(andando){
                helice_rotation_mundo = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_braco),glm::vec3(1.0,0.0,0.0));
            }

            // Aplica a matriz de rotação à turbina direita
            turbina_direita->Model(matrix_corpo_aviao_angulo*matrix_corpo_aviao*helice_translation_direita_volta*helice_rotation_eixo*helice_translation_direita_centro*helice_rotation_mundo);

            //ROTACAO TURBINA ESQUERDA
            glm::mat4 helice_translation_esquerda_centro = glm::translate(glm::mat4(1.0), -glm::vec3(4.72, 0.0, 0.0));
            glm::mat4 helice_translation_esquerda_volta = glm::translate(glm::mat4(1.0), glm::vec3(4.72, 0.0, 0.0));

            // Aplica a matriz de rotação à turbina direita
            turbina_esquerda->Model(matrix_corpo_aviao_angulo*matrix_corpo_aviao*helice_translation_esquerda_volta*helice_rotation_eixo*helice_translation_esquerda_centro*helice_rotation_mundo);
        }

        // Renderiza a cena
        my_scene.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


 
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
