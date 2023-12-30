#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "plane.h"
#include <vector>
// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
// const float SPEED       =  2.5f;
const float SPEED       =  0.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

// tp the center of plane
glm::vec3 camera_pos = glm::vec3(0.0f, 0.5f, 3.0f);






class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    
    bool MouseEnable = false;

    // euler Angles
    float Yaw;
    float Pitch;
    float Roll;

    float Yaw_mouse = 0.0f;
    float Pitch_mouse = 0.0f;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // float Roll_rate = 0.1745f;
    float Roll_rate_time = 0.1745f;
    float Roll_delta = 0.0f;

    float Roll_limit = 0.1483f;
    float x_offset = 0.0f;
    float x_off_rate_time = 0.9412f;
    float x_off_limit = 0.8f;

    bool roll_ret = false;
    bool need_ret = false;

    Plane plane;

    // constructor with vectors
    Camera() {};
    
    void init(glm::vec3 position, glm::vec3 position_off, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)  {
        
        // : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
        Front = glm::vec3(0.0f, 0.0f, -1.0f);
        MovementSpeed = SPEED;
        MouseSensitivity = SENSITIVITY;
        Zoom = ZOOM;

        Position = position_off;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;

        plane.init(position);

        Roll = 0.0f;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void reset_Roll() {

    }


    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    // A / D: turn and rotate       release: return 
    // Q / E: move L / R
    // space / Lshift (1 / 3): up / down
    // W / S: speed up / slow down
    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        // use plane to update the values.
        plane.plane_process_key(direction, deltaTime);
        

        // release

        
        if (roll_ret) {
            // test if valid / end --> valid: set false
            float roll_del;
            if (plane.ret_AoA(deltaTime, &roll_del)) {     // if finished the roll
                Roll_delta = 0.0f;
                Pitch = plane.Pitch;
                Yaw = plane.Yaw;
                roll_ret = false;
            }
        } else {
            // normal, flip(push) included
            if (plane.Pitch > 89.9f || plane.Pitch < -89.9f) {
                // set roll to keep the view
                need_ret = true;
                Roll_delta = 3.1415926535f;
            } 
            Pitch = plane.Pitch;
            Yaw = plane.Yaw;
        }



        // if (plane.Pitch > 89.9f || plane.Pitch < -89.9f) {
        //     float roll_del;
        //     if (roll_ret) {
        //         // recover ROLL...or use ROLL
        //         if (plane.ret_AoA(deltaTime, &roll_del)) {     // if finished the roll
        //             Roll_delta = 0.0f;
        //             Pitch = plane.Pitch;
        //             Yaw = plane.Yaw;
        //             roll_ret = false;
        //         } else {
        //             Roll_delta -= roll_del;
        //             std::cout << "\t" << roll_del << "\t" << Roll << std::endl;
        //         }
        //     } else {
        //         Roll_delta = 3.1415926535f;

        //         Pitch = plane.Pitch;
        //         Yaw = plane.Yaw;
        //     }
        // } else {
        //     Pitch = plane.Pitch;
        //     Yaw = plane.Yaw;
        // }



        // position with the plane

        // when up/down: same Pitch with plane
        // Yaw same
        // if (Pitch > 89.9f) {
        //     std::cout << Pitch << std::endl;
        //     // Pitch = 
        // } else if (Pitch < -89.9f) {
        //     std::cout << Pitch << std::endl;

        // }

        // when turn: roll a little more and make x_offset
        if (direction == ROT_R) {
            rot_r(deltaTime, -1);
        }
        if (direction == ROT_L) {
            rot_r(deltaTime, 1);
        }
        if (direction == ROT_RET) {
            rot_r(deltaTime, 0);
        }

        // Later: enable the mouse with cable


        // cameraVector
        updateCameraVectors();        
    }

    // statistics: later....
    void rot_r(float deltaTime, int flag) {
        // now only for the camera roll and x_offset
        float Roll_rate = Roll_rate_time * deltaTime;
        float x_off_rate = x_off_rate_time * deltaTime;
        if (flag == 0) {                // recover the Roll and the x_offset
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
            if (x_offset > x_off_rate) {                  // move back to center
                x_offset -= x_off_rate;
            } else if (x_offset < -x_off_rate) {
                x_offset += x_off_rate;
            } else {
                x_offset = 0.0f;
            }

        } else {                        // L / R
            if (Roll*flag < Roll_limit) {
                Roll += flag * Roll_rate;
            } else {
                Roll = flag * Roll_limit;
            }
            if (x_offset*flag < x_off_limit) {
                x_offset += flag * x_off_rate;
            } else {
                x_offset = flag * x_off_limit;
            }
        }
    }

    
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw_mouse   += xoffset;
        Pitch_mouse += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        // if (constrainPitch)
        // {
        //     if (Pitch > 89.0f)
        //         Pitch = 89.0f;
        //     if (Pitch < -89.0f)
        //         Pitch = -89.0f;
        // }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    glm::vec3 retrive_pos() {
        return Position;
    }
    glm::vec3 retrive_tar() {
        return Front;
    }
    void Draw(float deltaTime, glm::mat4 view, glm::mat4 projection) {
        plane.Draw(deltaTime, view, projection, MouseEnable);
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    // angles from plane
    // with mouse seperately: with Yaw_mouse and Pitch_mouse
    // and update the camera Front and Position.
    void updateCameraVectors() {        // update the vectors and position
        if (!MouseEnable) {             // clear Yaw_mouse and Pitch_mouse if ALT is released
            Yaw_mouse = 0.0f;
            Pitch_mouse = 0.0f;
        }

        glm::mat4 rot_mouse = glm::mat4(1.0f);
        rot_mouse = glm::rotate(rot_mouse, glm::radians(Yaw_mouse), plane.Up);
        rot_mouse = glm::rotate(rot_mouse, glm::radians(Pitch_mouse), plane.Right);

        // calculate the WorldUp
        glm::mat4 rot = glm::mat4(1.0f);

        // prob here
        // if (Roll_delta > 0.01f) {
        //     std::cout << "\t\t\t" << Roll << "\t" << Roll_delta << std::endl;
        // }
        rot = glm::rotate(rot, (Roll + Roll_delta), Front);
        glm::vec4 up = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        up = rot * up;
        WorldUp = glm::vec3(up);


        // calculate the new Front vector // 
        glm::vec4 front_4 = glm::vec4(plane.Front, 1.0f);
        Front = glm::vec3(glm::normalize(rot_mouse * front_4));

        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));

        // position: plane.Position + (rotate with Yaw and Pitch)camer_pos + x_offset*Right
        glm::mat4 rot_Yaw = glm::mat4(1.0f);
        rot_Yaw = glm::rotate(rot_mouse, glm::radians(-Yaw - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rot_Yaw = glm::rotate(rot_Yaw, glm::radians(Pitch), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::vec4 move_pos = rot_Yaw * glm::vec4(camera_pos, 1.0f);

        // more to say for the mouse:
        // rotate around the center of the plane, with Yaw_mouse and Pitch_mouse
        Position = plane.Position + glm::vec3(move_pos) - Right * x_offset;
    }


    // what if the mouse..
    // Yaw and Pitch from mouse, others are the same
};

#endif