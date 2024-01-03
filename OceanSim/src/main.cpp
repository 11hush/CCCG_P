//
// references:
// the related knowledge and pipeline: https://learnopengl.com/
// the texture of the ship:
// texture loader: https://github.com/nothings/stb/blob/master/stb_image.h

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include "include/glm/gtx/string_cast.hpp"
#include "include/ocean.h"
#include "include/oceanshader.h"
#include "include/skybox.h"
#include "include/collision.h"
#include <iostream>

// added
// change the Myshader.h
#include "include/Myshader.h"
#include "include/texture.h"
#include "include/camera.h"
#include "include/ship.h"

#include "include/post_effect.h"


void framebuffer_size_callback(GLFWwindow* window, int w, int h);
void mouseCall(GLFWwindow* window, double xposIn, double yposIn);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// 1-time call
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mod);


// default screen size 
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// // camera settings
// glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
// glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// bool firstUse = true;
// float yaw = -90.0f;
// float pitch = 0.0f;
// float lastX = 0.0f; // set with mouse
// float lastY = 0.0f; // set with mouse
// float fov = 90.0f; // field of view
// float sensitivity = 0.2f; // the sensitivity of the camera
// // timing
// float deltaT = 0.0f;	// time between current frame and last frame
// float last = 0.0f; // save the time so as to get delta

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaT = 0.0f;
float last = 0.0f;

int stop = 0;

glm::vec3 collid_pos;
int is_colli;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OceanSim", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (!window)
    {
        std::cout << "Fail to create window" << std::endl;
        glfwTerminate();
        return 0;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCall);
    glfwSetScrollCallback(window, scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // get the mouse

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to start GLAD" << std::endl;
        return 0;
    }

    glEnable(GL_DEPTH_TEST); // get the depth
    glEnable(GL_MULTISAMPLE);
    //248 g141 b30
    glm::vec3 lightColors = glm::vec3(248.0, 141.0, 30.0);
    glm::vec3 lightDirection = glm::vec3(3.0, 3.0, -1.7);
    lightColors = 0.1f * lightColors;
    OceanShader shader("../shader/ocean_vert.vs", "../shader/ocean_frag.fs");
    OceanShader skyboxShader("../shader/6.1.skybox.vs", "../shader/6.1.skybox.fs");



    camera.init(glm::vec3(-1.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.5f, 3.0f));
    Ship ship;
    ship.init();

    Effect effect;

    shader.use();
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // Ocean ocean512(512);
    // Ocean ocean256(256);
    // Ocean ocean128(128);
    // Ocean ocean64(64);
    Ocean ocean32(32);
    SkyBox skybox;
    shader.setInt("skybox", 0);

    shader.setInt("ourTexture", 0);

    // glm::vec3 old_cam = cameraPos;
    // glm::vec3 new_cam = cameraPos;
    glm::vec3 old_cam = camera.Position;
    glm::vec3 new_cam = camera.Position;
    glm::vec3 d_cam = new_cam - old_cam; // the amount of change in the camera
    while (d_cam.x > 1000.0) d_cam.x -= 1000.0;
    while (d_cam.x < -1000.0) d_cam.x += 1000.0;
    while (d_cam.z > 1000.0) d_cam.z -= 1000.0;
    while (d_cam.z < -1000.0) d_cam.z += 1000.0;
    while (!glfwWindowShouldClose(window))
    {
        //std::cout << "HI\n";
        float cur = static_cast<float>(glfwGetTime());
        deltaT = cur - last;
        last = cur;

        // process the inputs
        processInput(window);
        glfwSetKeyCallback(window, key_callback);
        glm::vec3 cameraPos = camera.Position;
        glm::vec3 cameraFront = camera.Front;
        glm::vec3 cameraUp = camera.Up;

        new_cam = cameraPos;
        d_cam += new_cam - old_cam;
        while (d_cam.x > 1000.0) d_cam.x -= 1000.0;
        while (d_cam.x < -1000.0) d_cam.x += 1000.0;
        while (d_cam.z > 1000.0) d_cam.z -= 1000.0;
        while (d_cam.z < -1000.0) d_cam.z += 1000.0;
        old_cam = new_cam;

        // glfwPollEvents();
        // processInput(window);
        if (stop) continue;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ocean512.calculate(deltaT);
        // ocean256.calculate(deltaT);
        // ocean128.calculate(deltaT);
        // ocean64.calculate(deltaT);
        // ocean32.calculate(deltaT);

        // glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // // glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, (float)ocean512.L * ocean512.num);
        // glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, (float)ocean32.L * ocean32.num);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // draw
        ship.Draw(view, projection);
        camera.Draw(deltaT, camera.GetViewMatrix(), projection);
        glm::vec3 explode_pos = collid_pos * 0.6f + camera.plane.Position * 0.4f;
        if(explode_pos.y < 3.0f)
            explode_pos.y = 3.05f;
        effect.Update(deltaT, collid_pos * 0.6f + camera.plane.Position * 0.4f, camera.Position);
        effect.Draw(camera.GetViewMatrix(), projection);

        // do collision
        if(!is_colli)
        {
            is_colli = collision_detect(camera.plane, ship, collid_pos);
            if(is_colli && !camera.plane.is_ondeck())
            {
                if(is_colli == COLLID_ON_SHIP)
                    camera.plane.touch_deck();
                else if(is_colli == COLLID_ON_TOW)
                    camera.plane.touch_tow();
                if(camera.plane.is_crash()) 
                {
                    effect.begin_explode();
                    std::cout << "colli!" << std::endl;
                    is_colli = true;
                }
                else
                    is_colli = false;
            }
            else
            {
                if(is_colli && camera.plane.is_ondeck())
                    // std::cout << "on deck" << std::endl;
                is_colli = false;
            }
        }
        // std::cout << "camera: " << static_cast<float>(glfwGetTime()) << std::endl;
        // update effect
        // std::cout << "effect: " << static_cast<float>(glfwGetTime()) << std::endl;

        shader.use();
        shader.setMat4("view", view);
        shader.setVec3("camPos", cameraPos);
        shader.setMat4("projection", projection);

        shader.setVec3("lightDir", lightDirection);
        shader.setVec3("lightColors", lightColors);

        // glm::mat4 pre_model = glm::mat4(1.0);
        // pre_model = glm::translate(pre_model, glm::vec3(cameraPos.x - ocean512.L * ocean512.num - d_cam.x, 0.0, cameraPos.z - ocean512.L * ocean512.num - d_cam.z));

        // glm::mat4 model = glm::mat4(1.0);
        // for (int i = 0; i < ocean512.num * 2; i++) {
        //     for (int j = 0; j < ocean512.num * 2; j++) {
        //         model = glm::mat4(1.0);
        //         model = glm::translate(pre_model, glm::vec3((float)i * (ocean512.L - 50), 0.0, (float)j * (ocean512.L - 50)));
        //         shader.setMat4("model", model);
        //         // if (i >= ocean512.num - 2 && i <= ocean512.num + 2 && j >= ocean512.num - 2 && j <= ocean512.num + 2) {
        //         //     ocean512.draw();
        //         // }
        //         // else if (i >= ocean512.num - 4 && i <= ocean512.num + 4 && j >= ocean512.num - 4 && j <= ocean512.num + 4) {
        //         //     ocean256.draw();
        //         // }
        //         // else if (i >= ocean512.num - 8 && i <= ocean512.num + 8 && j >= ocean512.num - 8 && j <= ocean512.num + 8) {
        //         //     ocean128.draw();
        //         // }
        //         // else if (i >= ocean512.num - 16 && i <= ocean512.num + 16 && j >= ocean512.num - 16 && j <= ocean512.num + 16) {
        //         //     ocean64.draw();
        //         // }
        //         // else if ((i - ocean512.num) * (i - ocean512.num) + (j - ocean512.num) * (j - ocean512.num) <= ocean512.num * ocean512.num) {
        //         //     ocean32.draw();
        //         // }
        //         ocean32.draw();
        //     }
        // }



        glm::mat4 pre_model = glm::mat4(1.0);
        pre_model = glm::translate(pre_model, glm::vec3(cameraPos.x - ocean32.L * ocean32.num - d_cam.x, 0.0, cameraPos.z - ocean32.L * ocean32.num - d_cam.z));

        glm::mat4 model = glm::mat4(1.0);
        for (int i = 0; i < ocean32.num * 2; i++) {
            for (int j = 0; j < ocean32.num * 2; j++) {
                model = glm::mat4(1.0);
                model = glm::translate(pre_model, glm::vec3((float)i * (ocean32.L - 50), 0.0, (float)j * (ocean32.L - 50)));
                shader.setMat4("model", model);
                ocean32.draw();
            }
        }
        // std::cout << "ocean: " << static_cast<float>(glfwGetTime()) << std::endl;

        skyboxShader.use();
        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        skybox.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)          // exit
        glfwSetWindowShouldClose(window, true);

    // A / D: turn and rotate       release: return 
    // Q / E: move L / R
    // space / Lshift / 2: up / down / recover AoA
    // W / S: speed up / slow down
    // p : pause

    // accelerate
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)               // w
        camera.ProcessKeyboard(FORWARD, deltaT);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)               // s
        camera.ProcessKeyboard(BACKWARD, deltaT);

    // move left / right
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)               // q
        camera.ProcessKeyboard(LEFT, deltaT);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)               // e
        camera.ProcessKeyboard(RIGHT, deltaT);
    

    // recover the up / down 
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        camera.ProcessKeyboard(REC_UD, deltaT);
    } else {
        // turn up / down
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {              // space
            camera.ProcessKeyboard(UP, deltaT);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {     // left_shift
            camera.ProcessKeyboard(DOWN, deltaT);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
        }
    }

    // delta = 0.01 ~ 0.05
    // turn left / right / recover
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ) {             // a
        camera.ProcessKeyboard(ROT_L, deltaT);   
    }else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {       // d
        camera.ProcessKeyboard(ROT_R, deltaT);
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
        camera.ProcessKeyboard(ROT_RET, deltaT);
    }

    // limit the height
    if (camera.plane.Position.y > OCEAN_MAX_HEIGHT) {
        camera.plane.Position.y = OCEAN_MAX_HEIGHT;
    }

    // move forward
    camera.ProcessKeyboard(IDLE, deltaT);
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
    glViewport(0, 0, w, h);
}

void mouseCall(GLFWwindow* window, double xposIn, double yposIn)
{
    if (camera.MouseEnable) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(-xoffset, -yoffset);
    }
}



// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// process the inputs
// key_callback for those tests only single press...
// in practice: single press -> 1-set value
//              hold -> multiple times, but much fewer than processInput
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mod) {
    // if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)          // exit
    //     glfwSetWindowShouldClose(window, true);

    // delta = 0.01 ~ 0.05
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ) {             // a
        // printf("A\n");
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {       // d
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        camera.MouseEnable = true;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE) {       // for later: close the test...
        camera.MouseEnable = false;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
        if (camera.need_ret) {
            camera.roll_ret = true;
            camera.need_ret = false;
        }
    }

    // pause
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)               // w
        camera.ProcessKeyboard(PAUSE, deltaT);


}