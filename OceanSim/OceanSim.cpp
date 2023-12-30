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
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int w, int h);
void mouseCall(GLFWwindow* window, double xposIn, double yposIn);
void processInput(GLFWwindow* window);


// default screen size 
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstUse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 0.0f; // set with mouse
float lastY = 0.0f; // set with mouse
float fov = 90.0f; // field of view
float sensitivity = 0.2f; // the sensitivity of the camera
// timing
float deltaT = 0.0f;	// time between current frame and last frame
float last = 0.0f; // save the time so as to get delta

int stop = 0;



int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OceanSim", NULL, NULL);
    if (!window)
    {
        std::cout << "Fail to create window" << std::endl;
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCall);

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
    OceanShader shader("./shader/ocean_vert.vs", "./shader/ocean_frag.fs");
    OceanShader skyboxShader("./shader/6.1.skybox.vs", "./shader/6.1.skybox.fs");

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

    glm::vec3 old_cam = cameraPos;
    glm::vec3 new_cam = cameraPos;
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
        new_cam = cameraPos;
        d_cam += new_cam - old_cam;
        while (d_cam.x > 1000.0) d_cam.x -= 1000.0;
        while (d_cam.x < -1000.0) d_cam.x += 1000.0;
        while (d_cam.z > 1000.0) d_cam.z -= 1000.0;
        while (d_cam.z < -1000.0) d_cam.z += 1000.0;
        old_cam = new_cam;

        glfwPollEvents();
        processInput(window);
        if (stop) continue;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ocean512.calculate(deltaT);
        // ocean256.calculate(deltaT);
        // ocean128.calculate(deltaT);
        // ocean64.calculate(deltaT);
        ocean32.calculate(deltaT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, (float)ocean512.L * ocean512.num);
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, (float)ocean32.L * ocean32.num);


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
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); // close if q is pressed
    float speed = 500.0 * deltaT;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += speed * cameraFront;
        if (cameraPos.y > OCEAN_MAX_HEIGHT) {
            cameraPos.y = OCEAN_MAX_HEIGHT;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= speed * cameraFront;
        if (cameraPos.y > OCEAN_MAX_HEIGHT) {
            cameraPos.y = OCEAN_MAX_HEIGHT;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= speed * glm::normalize(glm::cross(cameraFront, cameraUp));
        if (cameraPos.y > OCEAN_MAX_HEIGHT) {
            cameraPos.y = OCEAN_MAX_HEIGHT;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += speed * glm::normalize(glm::cross(cameraFront, cameraUp));
        if (cameraPos.y > OCEAN_MAX_HEIGHT) {
            cameraPos.y = OCEAN_MAX_HEIGHT;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        stop = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        stop = 1;
    }

}

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
    glViewport(0, 0, w, h);
}

void mouseCall(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;
    if (firstUse) // used this for the first time
    {
        lastX = xpos;
        lastY = ypos;
        firstUse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    pitch += yoffset;
    // limit the field of view
    if (pitch > 89.9f) {
        pitch = 89.9f;
    }
    if (pitch < -89.9f) {
        pitch = -89.9f;
    }

    yaw += xoffset;

    glm::vec3 front = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
    cameraFront = glm::normalize(front);
}



