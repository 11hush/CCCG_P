#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Myshader.h"
#include "texture.h"
#include <string.h>

// FONT 48*22
// NUM  12*22
class Fonts {
private:
    unsigned int num_VAO, num_VBO, num_EBO;
    unsigned int ui_VAO, ui_VBO, ui_EBO;
    unsigned int sqr_VAO, sqr_VBO, sqr_EBO;
    unsigned int texture[10], ui_tex[3];
    glm::vec3 char_offset = glm::vec3(0.03f, 0.0f, 0.0f);
public:
    Fonts() {    }
    void init() {
                // width = 800, height = 600  --> 12*22
        // 12/800, 22/600 -> 0.015, 0.037
        
        float font_map[] = {
            0.02f,   0.048f, 0.0f,   1.0f, 0.0f, // top right
            0.02f,  -0.048f, 0.0f,   1.0f, 1.0f, // bottom right
            -0.02f, -0.048f, 0.0f,  0.0f, 1.0f, // bottom left
            -0.02f,  0.048f, 0.0f,  0.0f, 0.0f  // top left 
        };

        float square_map[] = {
            0.035f,   0.035f, 0.0f,   1.0f, 0.0f, // top right
            0.035f,  -0.035f, 0.0f,   1.0f, 1.0f, // bottom right
            -0.035f, -0.035f, 0.0f,   0.0f, 1.0f, // bottom left
            -0.035f,  0.035f, 0.0f,   0.0f, 0.0f  // top left 
        };

        unsigned int font_map_ind[] = {
            0, 1, 2,
            0, 2, 3
        };

        // num 0 ~ 9
        std::string name = "tex/fonts_low/";
        for(int i=0; i<10; i++) {
            std::string tex_name = name + std::to_string(i) + ".png";
            texture[i] = loadTexture(tex_name.c_str());
        }
        ui_tex[0] = loadTexture("tex/fonts_low/speed.png");
        ui_tex[1] = loadTexture("tex/fonts_low/alt.png");
        ui_tex[2] = loadTexture("tex/fonts_low/pointer.png");


        glGenVertexArrays(1, &num_VAO);
        glGenBuffers(1, &num_VBO);
        glGenBuffers(1, &num_EBO);
        glBindVertexArray(num_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, num_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(font_map), font_map, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, num_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(font_map_ind), font_map_ind, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture cord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1); 

        for(int i=0; i<4; i++) {
            // 0.04f,   0.096f ---> 0.16, 0.096  // center...or - 0.4 + 1.5*0.03
            font_map[i*5] *= 4.0;
        }

        glGenVertexArrays(1, &ui_VAO);
        glGenBuffers(1, &ui_VBO);
        glGenBuffers(1, &ui_EBO);
        glBindVertexArray(ui_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(font_map), font_map, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ui_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(font_map_ind), font_map_ind, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture cord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1); 


        glGenVertexArrays(1, &sqr_VAO);
        glGenBuffers(1, &sqr_VBO);
        glGenBuffers(1, &sqr_EBO);
        glBindVertexArray(sqr_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, sqr_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(square_map), square_map, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sqr_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(font_map_ind), font_map_ind, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture cord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1); 
    }

    int Draw(unsigned int shaderProgram, int velocity, int height) 
    {
        glUseProgram(shaderProgram);
        int num = velocity;
        int pl = 1000;
        int num_p = num / pl;
        for(int j=0; j<2; j++) {
            glm::mat4 model_map = glm::mat4(1.0f);
            if (j == 0) {
                model_map = glm::translate(model_map, glm::vec3(-0.4f, 0.0f, 0.0f));
            } else {
                model_map = glm::translate(model_map, glm::vec3(0.31f, 0.0f, 0.0f));        // 0.4 - 0.03*3
            }
            if (num < 0 || num > 9999) {
                std::cout << "invalid num to show, limit" << std::endl;
                return 1;
            }


            for(int i=0; i<4; i++) {
                num_p = num / pl;
                model_map = glm::translate(model_map, char_offset);
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model_map));
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture[num_p]);
                glBindVertexArray(num_VAO);    
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));
            
                num = num % pl;
                pl /= 10;        
            }

            glm::mat4 model_ui = glm::mat4(1.0f);
            if (j == 0) {
                model_ui = glm::translate(model_ui, glm::vec3(-0.355f, 0.096f, 0.0f));
            } else{
                model_ui = glm::translate(model_ui, glm::vec3(0.355f, 0.096f, 0.0f));
            }
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model_ui));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ui_tex[j]);
            glBindVertexArray(ui_VAO);    
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));


            num = height;
            pl = 1000;
        }

        glm::mat4 model_map = glm::mat4(1.0f);
        model_map = glm::translate(model_map, glm::vec3(-0.8f, -0.8f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model_map));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ui_tex[2]);
        glBindVertexArray(sqr_VAO);    
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));


        return 0;
    }
};