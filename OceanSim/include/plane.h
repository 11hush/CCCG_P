#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Myshader.h"
#include "texture.h"

#include "fonts.h"

// speed here: 0.xf / s --> v*7 = m/s
// speed to show: km/h  --> v*7*3.6 = 25.2 * v




enum Camera_Movement {          // movements
    IDLE,           // move with the current speed

    UP,
    DOWN,
    REC_UD,         // recover
    RET_ROLL,

    FORWARD,        // speed up 
    BACKWARD,       // slow down
    LEFT,
    RIGHT,

    PAUSE,

    ROT_L,
    ROT_R,
    ROT_RET
};
float ud_velocity = 0.015f;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Plane
float plane[] = {
    0.0f,  0.0f,  -1.0f,  0.0f, 0.0f,
    -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
    0.5f,  0.0f,  1.0f, 0.0f, 0.0f,

    0.0f,  0.25f,  1.0f,  0.0f, 0.0f
};
unsigned int plane_ind[] = {
    0, 1, 2,
    3, 1, 2, 
    3, 0, 1,
    3, 0, 2
};

float cords[] = {
    0.0f, 0.0f, 0.0f, 
    0.5f, 0.0f, 0.0f
};

float plane_map[] = {
    0.5f,  0.5f,  0.0f,   1.0f, 0.0f, // top right
    0.5f, -0.5f,  0.0f,   1.0f, 1.0f, // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, // bottom left
    -0.5f,  0.5f, 0.0f,  0.0f, 0.0f  // top left 
};
unsigned int plane_map_ind[] = {
    0, 1, 2,
    0, 2, 3
};


class Plane {
private:

    unsigned int VAO, VBO, EBO;
    unsigned int line_VAO, line_VBO, line_EBO;
    unsigned int map_VAO, map_VBO, map_EBO;

    unsigned int scan_VAO[3], scan_VBO[3];

    bool pause = false;

    bool on_wheels = true;                  // on deck
    bool to_land = false;
    bool crash_deck = true;                 // to detect the first touch
    bool take_off_suc = false;              // successful take off

    // 41.67 m/s = 5.95f/s
    // float velocity_on_wheel = 0.595f;

    // move center to plane
    float MovementSpeed = 0.0f;
    float Accel = 1.5f;
    float Accel_on_ground = 1.4f;
    float Accel_brake = 3.0f;
    float velocity_on_wheel = 4.0f;         // limit


    float Roll_rate_time = 1.2517f;

    float Roll_limit = 1.064f;
    float Roll_delta = 0.0f;

    unsigned int shaderProgram;
    unsigned int shadermap;
    unsigned int map_texture;

    Fonts fonts;

public:
    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);                     // the position of the...
    glm::vec3 Right = glm::vec3(1.0f, 0.0f, 0.0f);                        // with WorldUp, no consider Roll
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    float Pitch = 0.0f;
    float Yaw = -90.0f;
    float Roll = 0.0f;
    
    Plane() { }

    void is_crash() { to_land = true; };

    void init(glm::vec3 Pos) {
        Position = Pos;
        // generate the VAOs, VBO, VEO... shader and texture.
        // fragment shader with one uniform color input, vertex shader with projection * view * model * position
        shaderProgram = get_shader_later("shader/shader.vs", "shader/shader.fs");
        // no view and projection
        shadermap = get_shader_later("shader/shader_tex_map.vs", "shader/shader_tex_map.fs");
        // map_texture = loadTexture("vac_map.png");
        // map_texture = loadTexture("grass.png");
        map_texture = loadTexture("tex/fonts_low/map.png");

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_ind), plane_ind, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture cord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // smaller coord
        glGenVertexArrays(1, &line_VAO);
        glGenBuffers(1, &line_VBO);
        glGenBuffers(1, &line_EBO);
        // the 3d-coordinate (line)
        glBindVertexArray(line_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, line_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cords), cords, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // the map
        glGenVertexArrays(1, &map_VAO);
        glGenBuffers(1, &map_VBO);
        glGenBuffers(1, &map_EBO);
        glBindVertexArray(map_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, map_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(plane_map), plane_map, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, map_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_map_ind), plane_map_ind, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture cord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);



        float center_vertex[] = {
            -0.015f, 0.0f,  0.0f, 
            0.0f,  0.02f,  0.0f,
            0.015f,  0.0f,  0.0f, 
            0.0f,  -0.02f, 0.0f
        };
        float line_y[] = {
            0.0f,  0.24f,  0.0f, 
            0.0f,  -0.24f, 0.0f   
        };
        float line_x[] = {
            0.18f,  0.0f,  0.0f, 
            -0.18f, 0.0f,  0.0f   
        };

        // point and line
        glGenVertexArrays(3, scan_VAO);
        glGenBuffers(3, scan_VBO);
        glBindVertexArray(scan_VAO[0]);
        glBindBuffer(GL_ARRAY_BUFFER, scan_VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(center_vertex), center_vertex, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // line y
        glBindVertexArray(scan_VAO[1]);
        glBindBuffer(GL_ARRAY_BUFFER, scan_VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(line_y), line_y, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // line x
        glBindVertexArray(scan_VAO[2]);
        glBindBuffer(GL_ARRAY_BUFFER, scan_VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(line_x), line_x, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);


        fonts.init();
    }

    // fall, with no enough velocity...
    void fall(float deltaTime) {
        std::cout << "\tfall" << std::endl;
        Pitch -= 1.5 * deltaTime;
        MovementSpeed += Accel_on_ground * deltaTime;

        // update the front vec after all those Yaw / Front / Pitch
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // update right
        Right = glm::normalize(glm::cross(Front, glm::vec3(0.0f, 1.0f, 0.0f)));
        Up = glm::normalize(glm::cross(Right, Front));

        Position += MovementSpeed * deltaTime * Front;
    }


    // if on_wheel (before take off)
    void bef_air(Camera_Movement direction, float deltaTime) {
        // Accelerate
        if(direction == FORWARD) {
            MovementSpeed += Accel_on_ground * deltaTime;    
        }
        Position.z -= MovementSpeed * deltaTime;
        // Position.z -= velocity_on_wheel * deltaTime;
        if (Position.z <= -25.0f) 
        {
            if (MovementSpeed < velocity_on_wheel) {        // no enough speed
                std::cout << MovementSpeed << " < " << velocity_on_wheel << std::endl;
                // pause = true;
            } else {
                std::cout << "succ\t" << MovementSpeed << " < " << velocity_on_wheel << std::endl;
                take_off_suc = true;
            }
            on_wheels = false;
            
            std::cout << "speed and pitch:\t" << MovementSpeed << ": " << Pitch << std::endl;
            std::cout << "Position:\t" << Position.x << " " << Position.y << " " << Position.z << std::endl;
        } 
        else if (Position.z < -19.286f && Position.z > -25.0f) 
        {
            // position.y
            float x = Position.z;
            Position.y = x * x * 0.0193f + 0.7476f * x + 7.2254f + 3.0f; 
            // Pitch
            Pitch += 0.2206 * deltaTime;
        }
        // std::cout << Position.x << " " << Position.y << " " << Position.z << std::endl;

    }

    // when crash on deck, to_land = false, and process_key call this (not when on tower)
    void landing(Camera_Movement direction, float deltaTime) {
        // ????

        // detect angle and velocity..?

        // Accel_brake to slow down and end

        // fall if 
        if(MovementSpeed > 0) MovementSpeed -= Accel_brake * deltaTime;   
        // update the front vec after all those Yaw / Front / Pitch
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // update right
        Right = glm::normalize(glm::cross(Front, glm::vec3(0.0f, 1.0f, 0.0f)));
        Up = glm::normalize(glm::cross(Right, Front));

        Position += MovementSpeed * deltaTime * Front; 
    }

    bool ret_AoA(float deltaTime, float* roll_del) {
        std::cout << "AoA\t" << Pitch << "\t" << Yaw << "\t" << Roll << "\t" << Roll_delta << std::endl;

        if (Pitch < 89.9f && Pitch > -89.9f) {          // idle
            return true;
        }

        Roll_delta += Roll_rate_time * 2 * deltaTime;
        if (Roll_delta > 3.1415f) 
        {
            Roll_delta = 0.0f;
            Yaw += 180.0f;
            if (Pitch < 0) 
            {
                Pitch += 180.0f;
            } else 
            {
                Pitch -= 180.0f;
            }
            return true;
        }
        *roll_del = Roll_delta;
        return false;
    }

    void plane_process_key(Camera_Movement direction, float deltaTime) {
        if (direction == PAUSE) {
            pause = !pause;
        }
        
        if (pause) 
        {
            // finish, stay still
        } 
        else if (on_wheels) 
        {
            bef_air(direction, deltaTime);             // on deck
        } 
        else if (!take_off_suc) 
        {                    // if fail to take off
            fall(deltaTime);
        } 
        else if (to_land) 
        {
            landing(direction, deltaTime);
        } 
        else 
        {                            // process key-input after in-air
            // change the velocity
            if (direction == FORWARD)
            {
                MovementSpeed += deltaTime * Accel;
            }
            if (direction == BACKWARD) 
            {
                MovementSpeed -= deltaTime * Accel;
                if (MovementSpeed < 0) 
                {
                    MovementSpeed = 0.0f;
                }
            }
            float velocity = MovementSpeed * deltaTime;

            // IDLE: move 
            if (direction == IDLE) 
            {
                Position += Front * velocity;
            } 
            // move left / right (no angle change)
            // Euler angle needed...
            if (direction == LEFT) 
            {
                // std::cout << Right.x << " " << Right.y << " " << Right.z << std::endl;
                Position -= Right * velocity;
            }
            if (direction == RIGHT) 
            {
                // std::cout << Right.x << " " << Right.y << " " << Right.z << std::endl;
                Position += Right * velocity;
            }

            // rot UP / DOWN (Pitch)....
            if (direction == REC_UD) 
            {
                if (Pitch > ud_velocity) 
                {
                    Pitch -= ud_velocity;
                }  
                else if (Pitch < -ud_velocity) 
                {
                    Pitch += ud_velocity;
                } 
                else 
                {
                    Pitch = 0.0f;
                }
            } 
            else 
            {
                if (direction == UP) 
                {
                    Pitch += ud_velocity;
                }
                if (direction == DOWN) 
                {
                    Pitch -= ud_velocity;
                }
            }

            // Yaw and Roll (turn will be made with the Front and time)
            if (direction == ROT_R) 
            {
                rot_lr(deltaTime, -1);
            }
            if (direction == ROT_L) 
            {
                rot_lr(deltaTime, 1);
            }
            if (direction == ROT_RET) 
            {
                rot_lr(deltaTime, 0);
            }
        }
        
        if (Position.y < 0) {
            pause = true;
        }

        // update the front vec after all those Yaw / Front / Pitch
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // update right
        Right = glm::normalize(glm::cross(Front, glm::vec3(0.0f, 1.0f, 0.0f)));
        Up = glm::normalize(glm::cross(Right, Front));
    }


    void rot_lr(float deltaTime, int flag) 
    {
        // plane(in rad): v=1.2517, full to 1.064, at 0.85s
        int state = flag;
        // full circle at... 10.0s
        float radi = MovementSpeed * 1.5923;
        float arc_rot = 36.0 * deltaTime;

        float Roll_rate = Roll_rate_time * deltaTime;

        if (flag < 0) {                         // rotate the WorldUp vector of camera...
            if (Roll < Roll_limit) {
                Roll += Roll_rate;
            } else {
                Roll = Roll_limit;
            }
        } else if (flag > 0) {
            if (Roll > -Roll_limit) {
                Roll -= Roll_rate;
            } else {
                Roll = -Roll_limit;
            }
        } else {
            if (Roll > 0) {
                if (Roll < Roll_rate) {
                    Roll = 0.0f;
                } else {
                    Roll -= Roll_rate;
                }
            } else {
                if (Roll > (-Roll_rate)) {
                    Roll = 0.0f;
                } else {
                    Roll += Roll_rate;
                }
            }
        }
        // Yaw angle
        Yaw -= flag * arc_rot;
        // if keeps Forward, then the position will change with the Front...
    }



    // void Draw(float deltaTime, float Yaw, glm::mat4 view, glm::mat4 projection, glm::vec3 cam_pos, glm::vec3 cam_fnt, glm::vec3 cam_rgt, ROT_LR lr, ROT_UD ud, float arc_x = 0.0f, float arc_y = 0.0f) {
    void Draw(float deltaTime, glm::mat4 view, glm::mat4 projection, bool free_view = false) {
        // font
        int v_to_show = MovementSpeed * 25.2f;
        int h_to_show = Position.y * 7.0f;
        // std::cout << "\t" << v_to_show << " " << MovementSpeed << std::endl;
        if (fonts.Draw(shadermap, v_to_show, h_to_show)) 
        {
            pause = true;                           // invalid number to show...
        }
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glm::mat4 model = glm::mat4(1.0f);

        // no process for the input and change, only apply and draw

        model = glm::translate(model, Position);                                    // to the position                                    
        model = glm::rotate(model, (Roll + Roll_delta), Front);                                    // Roll:  when turn, only for... 
        model = glm::rotate(model, glm::radians(Yaw + 90), glm::vec3( 0.0f, -1.0f,  0.0f));     // Yaw:   direction
        model = glm::rotate(model, glm::radians(Pitch), glm::vec3( 1.0f,  0.0f,  0.0f));          // Pitch: rotate up / down 

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform4f(glGetUniformLocation(shaderProgram, "inColor"), 1.0f, 1.0f, 1.0f, 1.0f);        // color white
        // glDrawArrays(GL_LINE_LOOP, 0, num);

        glBindVertexArray(VAO);                                  
        // glDrawArrays(GL_TRIANGLES, 0, 12);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, (void*)0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // the map... no need for the projection and the view
        // just rotate it with the pitch angle, and move it to the left-down corner...
        // make a new shader, with only model
        glEnable(GL_SCISSOR_TEST);                  // scissor_test to limit the draw area
        glScissor(0, 0, 150, 150);
        glUseProgram(shadermap);
        glm::mat4 model_map = glm::mat4(1.0f);
       
        float x_in_map = Position.x * 0.014648f * (-1) * 0.333333;
        float z_in_map = Position.z * 0.014648f * 0.333333;

        model_map = glm::translate(model_map, glm::vec3(-0.8f, -0.8f, 0.0f));

        // here, if flipped, rotate 180...
        float flip = 0.0f;
        if (Pitch > 89.9f || Pitch < -89.9f) {          // idle
            flip = 180.0f;
        }  
        model_map = glm::rotate(model_map, glm::radians(Yaw + 90.0f + flip), glm::vec3(0.0f, 0.0f, 1.0f));
        model_map = glm::translate(model_map, glm::vec3(x_in_map, z_in_map, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shadermap, "model"), 1, GL_FALSE, glm::value_ptr(model_map));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, map_texture);
        glBindVertexArray(map_VAO);    
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));
        glDisable(GL_SCISSOR_TEST);                 // disable scissor_test
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // the point and line...?
        // only when visiable
        // disable when alt...?
        if (!free_view) 
        {
            glm::mat4 view_fix = glm::mat4(1.0f);
            glm::mat4 proj_fix = glm::mat4(1.0f);
            glm::mat4 modl_fix = glm::mat4(1.0f);

            // center point
            glUseProgram(shaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj_fix));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view_fix));
            glBindVertexArray(scan_VAO[0]);
            glUniform4f(glGetUniformLocation(shaderProgram, "inColor"), 0.0f, 1.0f, 0.0f, 1.0f);                   // green
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modl_fix));
            glDrawArrays(GL_LINE_LOOP, 0, 4);
        
            // try decide the point...
            // the last line: (0.0f, 0.0f, -10.0f) - position
            // the center of view: (0.0f, 0.5f, 0.0):       line - pos - center_offset
            // the screen of view: (0.0f, 0.5f, -1.0):      limit to the z

            // glm::vec3 target = glm::vec3(0.0f, -0.5f, -10.0f) - Position;
            // glm::vec3 target_inf = glm::vec3(0.0f, 0.0f, -1.0f) - Position;
            // // to the direction of Front..
            // float len = glm::dot(target, Front);
            // float len_inf = glm::dot(target_inf, Front);
            // // float x_pos = target.x / len;
            // // float y_pos = target.y / len;
            // float x_pos = glm::dot(target_inf, Right) / len_inf;
            // float y_pos = glm::dot(target, Up) / len;


            // // line y
            // glm::mat4 trans_x = glm::translate(modl_fix, glm::vec3(x_pos, 0.0f, 0.0f));
            // glUseProgram(shaderProgram);
            // glBindVertexArray(scan_VAO[1]);
            // glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(trans_x));
            // glDrawArrays(GL_LINES, 0, 2);
            // // line x
            // glm::mat4 trans_y = glm::translate(modl_fix, glm::vec3(0.0f, y_pos, 0.0f));
            // glUseProgram(shaderProgram);
            // glBindVertexArray(scan_VAO[2]);
            // glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(trans_y));
            // glDrawArrays(GL_LINES, 0, 2);
            // std::cout << "----------------------" << std::endl;
            // std::cout << target.x << " " << target.y << " " << target.z << std::endl;
            // std::cout << Position.x << " " << Position.y << " " << Position.z << std::endl;
            // std::cout << Right.x << " " << Right.y << " " << Right.z << std::endl;
            // std::cout << Up.x << " " << Up.y << " " << Up.z << std::endl;
            // std::cout << x_pos << "\t" << y_pos << std::endl;
        }


        ///////////////////////////////////////////////////////////////////////////////////////////////////

        // coords
        for(int i=0; i<3; i++) 
        {
            glUseProgram(shaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        
            // render the line
            glBindVertexArray(line_VAO);
            glm::mat4 model_r = glm::mat4(1.0f);
            if (i == 0) 
            {
                model_r = glm::rotate(model_r, 1.57f, glm::vec3(1.0f, 0.0f, 0.0f));             // X: no rotation
                glUniform4f(glGetUniformLocation(shaderProgram, "inColor"), 1.0f, 0.0f, 0.0f, 1.0f);                   // red
            } 
            else if (i == 1) 
            {
                model_r = glm::rotate(model_r, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));             // -Z
                glUniform4f(glGetUniformLocation(shaderProgram, "inColor"), 0.0f, 1.0f, 0.0f, 1.0f);                   // green
            } 
            else 
            {
                model_r = glm::rotate(model_r, 1.57f, glm::vec3(0.0f, 0.0f, 1.0f));             // Y
                glUniform4f(glGetUniformLocation(shaderProgram, "inColor"), 0.0f, 0.0f, 1.0f, 1.0f);                   // blue
            }

            model_r = model * model_r;
            // draw
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model_r));
            glDrawArrays(GL_LINES, 0, 2);
        }
    }
};
