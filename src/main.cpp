#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <fstream>

#include "scene.h"
#include "object.h"
#include "loadobject.h"

using namespace std;

bool ortho_per = true;
GLint width = 800,height = 800; 

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
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
        -30.0f, 0.0f, -30.0f,  
        30.0f, 0.0f, -30.0f, 
        30.0f, 0.0f,  30.0f,  
        -30.0f, 0.0f,  30.0f   
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

    //Carregar o modelo de avião
    
    object* airplane = read_obj_file("../models/mesh_aviao.obj");
    airplane->set_color(0.5f, 0.5f, 0.5f);
    my_scene.push_back_object(airplane);

    glClearColor(0.35f, 0.35f, 0.8f, 0.0f);
    glViewport(0, 0, width, height);

    my_scene.LookAt(0.0, 2.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // Atualiza a câmera com base no avião
        my_scene.update_camera(airplane);

        // Define a projeção ortográfica
        my_scene.perspective(0.75, 1.0, 0.1, 200.0);

        // Renderiza a cena
        my_scene.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
