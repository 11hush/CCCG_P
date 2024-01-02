#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Myshader.h"

    float boxVertices[180] = {
        // positions          
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,

        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

         1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 0.0f,

        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,

        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, 0.0f
    };

float blocking[] = {
    2.0f,   0.01f, 0.0f,
    -2.0f,  0.01f, 0.0f
};

glm::vec3 blk_pos[] = {
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 0.0f, -12.0f),
    glm::vec3(0.0f, 0.0f, -14.0f),
    glm::vec3(0.0f, 0.0f, -16.0f)
};

typedef struct aabb_box {
    glm::vec3 ld_point;
    glm::vec3 edges;
} aabb_box;

class Ship {
public:
    unsigned int VAO[3], VBO[3];
    unsigned int shaderProgram;
    aabb_box b_ship = {glm::vec3(-2.207f, 0.f, -25.f), glm::vec3(4.414f, 3.f, 25.f)};
    aabb_box b_tow1 = {glm::vec3(1.183f, 3.f,  -10.801f), glm::vec3(1.024f, 2.16f, 4.424f)};
    aabb_box b_tow2 = {glm::vec3(0.577f, 3.f, -13.686f), glm::vec3(1.63f, 5.926f, 2.886f)};
    Ship() {

    };

    // 95, 538
    // 4.414

    void init() {
        shaderProgram = get_shader_later("../shader/shader.vs", "../shader/shader.fs");
        float box_ship[180];
        float box_tow1[180];
        float box_tow2[180];
        // scale / 2
        get_box(2.207f, 1.5f, 12.5f, 0.0f, 1.5f, -12.5f, box_ship);
        // 10.8 - 6.377 = 4.423     2.207 - 0.512   1.08+3    6.377 + 2.212
        get_box(0.512f, 1.08f,  2.212f, 1.695f, 4.08f,  -8.589f, box_tow1);
        // 13.686-10.8=2.886        2.207 - 0.815             10.8 + 1.443
        get_box(0.815f, 2.963f, 1.443f, 1.392f, 5.963f, -12.243f, box_tow2);


        glGenVertexArrays(3, VAO);
        glGenBuffers(3, VBO);

        glBindVertexArray(VAO[0]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(box_ship), box_ship, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        //
        glBindVertexArray(VAO[1]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(box_tow1), box_tow1, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        //
        glBindVertexArray(VAO[2]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(box_tow2), box_tow2, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void Draw(glm::mat4 view, glm::mat4 projection) {
        glm::mat4 model = glm::mat4(1.0f);
        for(int i=0; i<3; i++) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glUseProgram(shaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform4f(glGetUniformLocation(shaderProgram, "inColor"), 1.0f, 0.0f, 0.0f, 1.0f);        // color white
            glBindVertexArray(VAO[i]);                                  
            // glDrawArrays(GL_TRIANGLES, 0, 12);
            // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


            glUseProgram(shaderProgram);
             glUniform4f(glGetUniformLocation(shaderProgram, "inColor"), 0.4f, 0.4f, 0.4f, 1.0f);        // color white
            glBindVertexArray(VAO[i]);                                  
            // glDrawArrays(GL_TRIANGLES, 0, 12);
            // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

private:
    void get_box(float x, float y, float z, float x_off, float y_off, float z_off, float* output) {
        // std::cout << "________________________" << std::endl;
        // std::cout << x << "\t" << y << "\t" << z << "\t" << x_off << "\t" << y_off << "\t" << z_off << std::endl;
        for(int i=0; i<36; i++) {
            output[i*5] = x * boxVertices[i*5] + x_off;
            output[i*5 + 1] = y * boxVertices[i*5 + 1] + y_off;
            output[i*5 + 2] = z * boxVertices[i*5 + 2] + z_off;
            std::cout << output[i*5] << "\t" << output[i*5 + 1] <<  "\t" << output[i*5 + 2] << std::endl;

        }
    }

};
