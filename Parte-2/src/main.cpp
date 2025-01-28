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
bool roll_direito = false;
bool roll_esquerdo = false;

float angulo_altura = 0.0f;
float angulo_lado = 0.0f;
float angulo_roll = 0.0f;

float angulo_helice = 0.0f;
float angulo_braco = 0.0f;

float x_aviao = 0.0f;
float y_aviao = 0.0f;
float z_aviao = 0.0f;

void set_light_default(LightProperties &light){
   light.is_camera_coordinate = true;
   light.isEnabled = true;
   light.isLocal = true;
   light.isSpot = false;

   light.ambient[0] = light.ambient[1] = light.ambient[2] = 0.3;
   light.color[0] = light.color[1] = light.color[2] = 1.0;
   light.position[0] = -10.0;
   light.position[1] = 20.0;
   light.position[2] = 5.0;
   
   light.constantAttenuation = 1.0;
   light.linearAttenuation = 0.0;
   light.quadraticAttenuation = 0.0;
   return;
}

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
            roll_direito = false;
            roll_esquerdo = false;
            break;
        case GLFW_KEY_W:
            if(baixo){
                reto_altura = true;
            }else{
                cima = true;
                reto_altura = false;
            }
            baixo = false;
            break;
        case GLFW_KEY_S:
            if(cima){
                reto_altura = true;
            }else{
                baixo = true;
                reto_altura = false;
            }
            cima = false;
            break;
        case GLFW_KEY_A:
            if(direita){
                reto_lado = true;
            }else{
                esquerda = true;
                reto_lado = false;
            }
            direita = false;
            break;
        case GLFW_KEY_D:
            if(esquerda){
                reto_lado = true;
            }else{
                direita = true;
                reto_lado = false;
            }
            esquerda = false;
            break;
        case GLFW_KEY_J:
            roll_esquerdo = true;
            roll_direito = false;
            break;
        case GLFW_KEY_L:
            roll_esquerdo = false;
            roll_direito = true;
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

    LightProperties light0,light1;
    set_light_default(light0); 
    
    my_scene.set_light(0,light0);

    //Carregar as partes do avião
    object* corpo_aviao = read_obj_file("../models/corpo_aviao.obj");
    corpo_aviao->set_color(0.5f, 0.5f, 0.5f);
    corpo_aviao->set_texture_combiner(0);
    my_scene.push_back_object(corpo_aviao);

    object* braco_direito = read_obj_file("../models/braco_direito.obj");
    braco_direito->set_color(0.5f, 0.5f, 0.5f);
    braco_direito->set_texture_combiner(0);
    my_scene.push_back_object(braco_direito);

    object* braco_esquerdo = read_obj_file("../models/braco_esquerdo.obj");
    braco_esquerdo->set_color(0.5f, 0.5f, 0.5f);
    braco_esquerdo->set_texture_combiner(0);
    my_scene.push_back_object(braco_esquerdo);

    object* turbina_direita = read_obj_file("../models/turbina_direita.obj");
    turbina_direita->set_color(0.5f, 0.5f, 0.5f);
    turbina_direita->set_texture_combiner(0);
    my_scene.push_back_object(turbina_direita);

    object* turbina_esquerda = read_obj_file("../models/turbina_esquerda.obj");
    turbina_esquerda->set_color(0.5f, 0.5f, 0.5f);
    turbina_esquerda->set_texture_combiner(0);
    my_scene.push_back_object(turbina_esquerda);

    object* leme = read_obj_file("../models/leme.obj");
    leme->set_color(0.5f, 0.5f, 0.5f);
    leme->set_texture_combiner(0);
    my_scene.push_back_object(leme);

    object* profundor = read_obj_file("../models/profundor.obj");
    profundor->set_color(0.5f, 0.5f, 0.5f);
    profundor->set_texture_combiner(0);
    my_scene.push_back_object(profundor);

    object* chao = read_obj_file("../models/cubo.obj");
    chao->set_color(1.0,0.5,0.5);
    chao->load_texture2D_simple_bmp("../models/grama.bmp",122,256,256);
    chao->set_texture_combiner(2);
    my_scene.push_back_object(chao);

    // COLOCANDO OS PREDIOS
    for(int i=0;i<15;i++){
        object* predio = read_obj_file("../models/predio.obj");
        predio->set_color(0.2,0.2,0.2);
        //predio->load_texture2D_simple_bmp("../models/predio.jpg",122,256,256);
        predio->set_texture_combiner(0);
        my_scene.push_back_object(predio);

        //Transformação do predio
        glm::mat4 matriz_escala_predio = glm::scale(glm::mat4(1.0), glm::vec3(2.0, 2.0, 2.0));
        glm::mat4 rotacao_predio = glm::rotate(glm::mat4(1.0),glm::radians(90.0f),glm::vec3(0.0,1.0,0.0));
        glm::mat4 matriz_translacao_predio = glm::translate(glm::mat4(1.0), glm::vec3(-40.0, 0.0, 0.0 + (i*30)));
        predio->Model(matriz_translacao_predio*matriz_escala_predio*rotacao_predio);
    }

    for(int i=0;i<15;i++){
        object* predio = read_obj_file("../models/predio.obj");
        predio->set_color(0.2,0.2,0.2);
        //predio->load_texture2D_simple_bmp("../models/predio.jpg",122,256,256);
        predio->set_texture_combiner(0);
        my_scene.push_back_object(predio);

        //Transformação do predio
        glm::mat4 matriz_escala_predio = glm::scale(glm::mat4(1.0), glm::vec3(2.0, 2.0, 2.0));
        glm::mat4 rotacao_predio = glm::rotate(glm::mat4(1.0),glm::radians(-90.0f),glm::vec3(0.0,1.0,0.0));
        glm::mat4 matriz_translacao_predio = glm::translate(glm::mat4(1.0), glm::vec3(40.0, 0.0, 0 + (i*30)));
        predio->Model(matriz_translacao_predio*matriz_escala_predio*rotacao_predio);
    }

    object* skysphere = read_obj_file("../models/esfera_sm_inverted.obj");
    skysphere->set_color(1.0,1.0,1.0);
    skysphere->load_texture2D_simple_bmp("../models/ceu.bmp",122, 256, 256);
    skysphere->set_texture_combiner(2);
    my_scene.push_back_object(skysphere);

    for(int i=0;i<5;i++){
        object* bull = read_obj_file("../models/bull.obj");
        bull->set_color(1.0,0.5,0.5);
        bull->load_texture2D_simple_bmp("../models/bull_texture.bmp",138, 1024, 1024);
        bull->set_texture_combiner(1);
        my_scene.push_back_object(bull);

        // Configura a transformação da skysphere (centrada no mundo)
        glm::mat4 bull_transform = glm::scale(glm::mat4(1.0), glm::vec3(1.5, 1.5, 1.5)); 
        glm::mat4 bull_translate = glm::translate(glm::mat4(1.0), glm::vec3(-60.0, 2.0, 0.0 + (i*20)));
        bull->Model(bull_translate*bull_transform);
    }

    for(int i=0;i<15;i++){
        object* tree = read_obj_file("../models/tree_textura.obj");
        tree->set_color(1.0,1.0,1.0);
        tree->load_texture2D_simple_bmp("../models/grama.bmp",122, 256, 256);
        tree->set_texture_combiner(2);
        my_scene.push_back_object(tree);

        //Transformação do predio
        glm::mat4 matriz_translacao_tree = glm::translate(glm::mat4(1.0), glm::vec3(15.0, 0.0, 0 + (i*30)));
        tree->Model(matriz_translacao_tree);
    }

    glClearColor(0.35f, 0.35f, 0.8f, 0.0f);
    glViewport(0, 0, width, height);

    my_scene.LookAt(0.0, 2.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    //Transformação do chão
    glm::mat4 matriz_escala_chao = glm::scale(glm::mat4(1.0), glm::vec3(100.0, 1.0, 1000.0));
    chao->Model(matriz_escala_chao);

    // Configura a transformação da skysphere (centrada no mundo)
    glm::mat4 skysphere_transform = glm::scale(glm::mat4(1.0), glm::vec3(500.0, 500.0, 500.0)); // Grande o suficiente para envolver a cena
    skysphere->Model(skysphere_transform);

    //Posicionando as partes
    glm::mat4 helice_direita_position = glm::translate(glm::mat4(1.0), glm::vec3(0.5, 3.5, 2.5));
    turbina_direita->Model(helice_direita_position);

    glm::mat4 helice_esquerda_position = glm::translate(glm::mat4(1.0), glm::vec3(-0.5, 3.5, 2.5));
    turbina_esquerda->Model(helice_esquerda_position);

    glm::mat4 braco_esquerdo_position = glm::translate(glm::mat4(1.0), glm::vec3(-0.3, 0.0, 0.0));
    braco_esquerdo->Model(braco_esquerdo_position);

    glm::mat4 braco_direito_position = glm::translate(glm::mat4(1.0), glm::vec3(0.3, 0.0, 0.0));
    braco_direito->Model(braco_direito_position);

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

        glm::mat4 matrix_corpo_aviao_roll = glm::mat4(1.0f);
        glm::mat4 matrix_corpo_aviao_pitch = glm::mat4(1.0f);
        glm::mat4 matrix_corpo_aviao_yaw = glm::mat4(1.0f);

        if(andando){
            if(angulo_lado < 90 && angulo_lado > -90)
                frente = frente + 0.040;
            else if(angulo_lado > 90 && angulo_lado < 240 || angulo_lado < -90 && angulo_lado > -240)
                frente = frente - 0.040;
        }

        if(reto_altura){
            if(angulo_altura > 0){
                angulo_altura -= 0.040;
            }
            if(angulo_altura < 0){
                angulo_altura += 0.040;
            }

            matrix_corpo_aviao_pitch = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_altura),glm::vec3(1.0,0.0,0.0));
        }

        if(reto_lado){
            matrix_corpo_aviao_yaw = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_lado),glm::vec3(0.0,1.0,0.0));
        }

        if(baixo){
            y_aviao = y_aviao  - 0.040;
            if(angulo_altura < 40.0f){
                angulo_altura += 0.040;
            }

            matrix_corpo_aviao_pitch = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_altura),glm::vec3(1.0,0.0,0.0));
        }

        if(cima){
            y_aviao = y_aviao + 0.040;
            if(angulo_altura > -40.0f){
                angulo_altura -= 0.040;
            }

            matrix_corpo_aviao_pitch = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_altura),glm::vec3(1.0,0.0,0.0));
        }

        if(direita){
            lados = lados - 0.040;
            angulo_lado -= 0.040;
            
            matrix_corpo_aviao_yaw = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_lado),glm::vec3(0.0,1.0,0.0));
        }

        if(esquerda){
            lados = lados + 0.040;
            angulo_lado += 0.040;
            
            matrix_corpo_aviao_yaw = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_lado),glm::vec3(0.0,1.0,0.0));
        }

        if(roll_direito){
            angulo_roll += 0.040;

            matrix_corpo_aviao_roll = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_roll),glm::vec3(0.0,0.0,1.0));
        }

        if(roll_esquerdo){
            angulo_roll -= 0.040;

            matrix_corpo_aviao_roll = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_roll),glm::vec3(0.0,0.0,1.0));
        }

        glm::mat4 matrix_corpo_aviao = glm::translate(glm::mat4(1.0), glm::vec3(x_aviao + lados, y_aviao, z_aviao + frente));
        corpo_aviao->Model(matrix_corpo_aviao_yaw*matrix_corpo_aviao_pitch*matrix_corpo_aviao_roll*matrix_corpo_aviao);

        //BRACO DIREITO
        glm::mat4 braco_translation_mundo_ajuste = glm::mat4(1.0);

        glm::mat4 braco_translation_centro_direito = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -4.5, -3.0));
        glm::mat4 braco_translation_volta_direito = glm::translate(glm::mat4(1.0), -glm::vec3(0.0, -4.5, -3.0));

        glm::mat4 braco_rotation = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_braco),glm::vec3(1.0,0.0,0.0));

        if(andando){
            braco_translation_mundo_ajuste = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.5, 1.5));
        }

        braco_direito->Model(matrix_corpo_aviao_yaw*matrix_corpo_aviao_pitch*matrix_corpo_aviao_roll*matrix_corpo_aviao*braco_translation_mundo_ajuste*braco_translation_volta_direito*braco_rotation*braco_translation_centro_direito*braco_direito_position);

        //BRACO ESQUERDO
        glm::mat4 braco_translation_centro_esquerdo = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -4.5, -3.0));
        glm::mat4 braco_translation_volta_esquerdo = glm::translate(glm::mat4(1.0), -glm::vec3(0.0, -4.5, -3.0));

        braco_esquerdo->Model(matrix_corpo_aviao_yaw*matrix_corpo_aviao_pitch*matrix_corpo_aviao_roll*matrix_corpo_aviao*braco_translation_mundo_ajuste*braco_translation_volta_esquerdo*braco_rotation*braco_translation_centro_esquerdo*braco_esquerdo_position);

        //LEME
        leme->Model(matrix_corpo_aviao_yaw*matrix_corpo_aviao_pitch*matrix_corpo_aviao_roll*matrix_corpo_aviao);

        //PROFUNDOR
        profundor->Model(matrix_corpo_aviao_yaw*matrix_corpo_aviao_pitch*matrix_corpo_aviao_roll*matrix_corpo_aviao);

        if(helice){
            glm::mat4 helice_rotation_eixo;
            glm::mat4 helice_rotation_mundo = glm::mat4(1.0f);

            //ROTACAO TURBINA DIREITA
            glm::mat4 helice_translation_direita_centro = glm::mat4(1.0f);
            glm::mat4 helice_translation_direita_volta = glm::mat4(1.0f);
            glm::mat4 helice_translation_mundo_ajuste_direito = glm::mat4(1.0f);

            //ROTACAO TURBINA ESQUERDA
            glm::mat4 helice_translation_esquerda_centro = glm::mat4(1.0f);
            glm::mat4 helice_translation_esquerda_volta = glm::mat4(1.0f);
            glm::mat4 helice_translation_mundo_ajuste_esquerdo = glm::mat4(1.0f);

            if(andando){
                helice_rotation_mundo = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_braco),glm::vec3(1.0,0.0,0.0));
                helice_rotation_eixo = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_helice),glm::vec3(0.0,0.0,1.0));

                //ROTACAO DA HELICE DIREITA COM O AVIAO ANDANDO
                helice_translation_mundo_ajuste_direito = glm::translate(glm::mat4(1.0), glm::vec3(-0.2, 6.3, -0.7));

                helice_translation_direita_centro = glm::translate(glm::mat4(1.0), glm::vec3(5.4, 2.7, 0.0));
                helice_translation_direita_volta = glm::translate(glm::mat4(1.0), -glm::vec3(5.4, 2.7, 0.0));

                //ROTACAO DA HELICE ESQUERDA COM O AVIAO ANDANDO
                helice_translation_mundo_ajuste_esquerdo = glm::translate(glm::mat4(1.0), glm::vec3(0.1, 1.0, -1.0));

                helice_translation_esquerda_centro = glm::translate(glm::mat4(1.0), glm::vec3(-5.4, 2.7, 0.0));
                helice_translation_esquerda_volta = glm::translate(glm::mat4(1.0), glm::vec3(5.4, 2.7, 0.0));
            }
            else{
                helice_rotation_eixo = glm::rotate(glm::mat4(1.0),glm::radians((float)angulo_helice),glm::vec3(0.0,1.0,0.0));

                //ROTACAO DA HELICE DIREITA COM O AVIAO PARADO OU SUBINDO
                helice_translation_direita_centro = glm::translate(glm::mat4(1.0), glm::vec3(5.4, 0.0, -2.75));
                helice_translation_direita_volta = glm::translate(glm::mat4(1.0), -glm::vec3(5.4, 0.0, -2.75));

                //ROTACAO DA HELICE ESQUERDA COM O AVIAO PARADO OU SUBINDO
                helice_translation_esquerda_centro = glm::translate(glm::mat4(1.0), -glm::vec3(5.4, 0.0, 2.75));
                helice_translation_esquerda_volta = glm::translate(glm::mat4(1.0), glm::vec3(5.4, 0.0, 2.75));
            }

            // Aplica a matriz de rotação à turbina direita
            turbina_direita->Model(matrix_corpo_aviao_yaw*matrix_corpo_aviao_pitch*matrix_corpo_aviao_roll*matrix_corpo_aviao* helice_translation_mundo_ajuste_direito*helice_translation_direita_volta*helice_rotation_eixo*helice_translation_direita_centro*helice_rotation_mundo*helice_direita_position);

            // Aplica a matriz de rotação à turbina esquerda
            turbina_esquerda->Model(matrix_corpo_aviao_yaw*matrix_corpo_aviao_pitch*matrix_corpo_aviao_roll*matrix_corpo_aviao*helice_translation_mundo_ajuste_esquerdo*helice_translation_esquerda_volta*helice_rotation_eixo*helice_translation_esquerda_centro*helice_rotation_mundo*helice_esquerda_position);
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
