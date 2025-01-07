#ifndef LOAD_H__
#define LOAD_H__
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include "utils.h"
using namespace std;
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "object.h"
#include "scene.h"

object* read_obj_file(const string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        throw std::ios_base::failure("Error opening file: " + filename);
    }

    vector<GLfloat> position;  
    vector<GLfloat> normal;    
    vector<GLfloat> uv;        
    vector<GLushort> index_position; 
    vector<GLushort> index_normal;   
    vector<GLushort> index_uv;       

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            position.push_back(x);
            position.push_back(y);
            position.push_back(z);
        } else if (token == "vn") {
            float nx, ny, nz;
            iss >> nx >> ny >> nz;
            normal.push_back(nx);
            normal.push_back(ny);
            normal.push_back(nz);
        } else if (token == "vt") {
            float u, v;
            iss >> u >> v;
            uv.push_back(u);
            uv.push_back(v);
        } else if (token == "f") {
            for (int i = 0; i < 3; ++i) {  
                std::string vertex_data;
                iss >> vertex_data;

                std::istringstream vertex_stream(vertex_data);
                std::string index_str;
                int index_v = 0, index_u = 0, index_n = 0;

                // Lê índices na ordem: v/vt/vn
                if (std::getline(vertex_stream, index_str, '/')) {
                    index_v = std::stoi(index_str) - 1;  
                }
                if (std::getline(vertex_stream, index_str, '/')) {
                    if (!index_str.empty()) {
                        index_u = std::stoi(index_str) - 1;  
                    }
                }
                if (std::getline(vertex_stream, index_str)) {
                    if (!index_str.empty()) {
                        index_n = std::stoi(index_str) - 1;
                    }
                }

                index_position.push_back(index_v);
                if (index_u >= 0) index_uv.push_back(index_u);
                if (index_n >= 0) index_normal.push_back(index_n);
            }
        }
    }

    file.close();

    // Cria um objeto usando os vértices e índices
    object* my_obj = new object(
        position.size() / 3, 
        index_position.size(), 
        position.data(), 
        index_position.data()
    );

    return my_obj;
}




#endif