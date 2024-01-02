#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "Myshader.h"
#include "texture.h"

class Particle {
public:
    glm::vec3 position_;
    glm::vec3 velocity_;
    glm::vec3 acceleration_;
    unsigned char r,g,b,a;
    float size_;
    GLfloat life_=-1;
    GLfloat age_=0;
    GLfloat camera_dist_=-1;
    GLfloat grow_rate_;

    Particle()=default;
    Particle(glm::vec3 pos, glm::vec3 v_ini, glm::vec3 acc, GLfloat life) :
        position_(pos), velocity_(v_ini), acceleration_(acc), life_(life), age_(0)
        {};
    void init() { life_ = -1; camera_dist_ = -1; age_ = 0; };
    bool operator<(const Particle& r) const {
        return this->camera_dist_ > r.camera_dist_;
    }
};

#endif