#ifndef _PARTICLE_GEN_H_
#define _PARTICLE_GEN_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "particle.h"
#include <vector>

#define FRAG_NUM 5

enum Particle_Type {
    SMOKE, FLAME, FRAG
};

class ParticleGenerator {
public:
    std::vector<Particle> particles_;
    unsigned int max_amount_;
    unsigned int amount_=0;
    unsigned int shader_;
    unsigned int texture_;
    unsigned int VAO_;

    GLuint lastUsedParticle_ = 0;

    GLuint billboard_vertex_buffer_;
    GLuint particles_position_buffer_;
    GLuint particles_color_buffer_;

    enum Particle_Type type_;
    float time_=0;

    const GLfloat g_vertex_buffer_data_[12] = { 
        -0.5f,  -0.5f,  0.0f,
        0.5f,   -0.5f,  0.0f,
        -0.5f,  0.5f,   0.0f,
        0.5f,   0.5f,   0.0f
    };
    GLfloat* g_particule_position_size_data_;
    GLubyte* g_particule_color_data_;

public:
    ParticleGenerator()=default;
    ParticleGenerator (unsigned int amount, Particle_Type type)
    : max_amount_(amount), amount_(0), type_(type)
        {};
    void init();
    void Update(float dt, glm::vec3 center_pos, unsigned int new_particles, glm::vec3 offset, glm::vec3 camera_pos);
    void Draw(glm::mat4 view_mat, glm::mat4 proj_mat);
    void SortParticles() { std::sort(&particles_[0], &particles_[amount_]); };
private:
    unsigned int FirstUnusedParticle();
    void RespawnParticle(Particle& particle, glm::vec3 center_pos, glm::vec3 offset = glm::vec3(0, 0, 0));
    void InitSmoke(Particle& particle, glm::vec3 center_pos);
    void InitFlame(Particle& particle, glm::vec3 center_pos);
    void InitFrag(Particle& particle, glm::vec3 center_pos);
    void UpdateSmoke(Particle& particle, float dt);
    void UpdateFlame(Particle& particle, float dt);
    void UpdateFrag(Particle& particle, float dt);
};

void ParticleGenerator::init()
{
    // std::cout << "init" << std::endl;
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);

    glGenBuffers(1, &this->billboard_vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_), g_vertex_buffer_data_, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glGenBuffers(1, &particles_position_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer_);
    glBufferData(GL_ARRAY_BUFFER, max_amount_ * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    glGenBuffers(1, &particles_color_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer_);
	glBufferData(GL_ARRAY_BUFFER, max_amount_ * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

    glBindVertexArray(0);

    amount_ = 0;
    if(!g_particule_position_size_data_) delete[] g_particule_position_size_data_;
    if(!g_particule_color_data_) delete[] g_particule_color_data_;
    g_particule_position_size_data_ = new GLfloat[4*max_amount_];
    g_particule_color_data_ = new GLubyte[4*max_amount_];

    particles_.clear();
    for(int i = 0; i < max_amount_; i++)
    {
        particles_.push_back(Particle());
        particles_.back().init();
    }
}

void ParticleGenerator::Update(float dt, glm::vec3 center_pos, unsigned int new_particles, glm::vec3 offset, glm::vec3 camera_pos)
{
    time_ += dt;
    for(int i = 0; i < new_particles; i++)
    {
        int unused_particle = this->FirstUnusedParticle();
        this->RespawnParticle(particles_[unused_particle], center_pos, offset);
        amount_++;
    }

    int cnt = 0;
    for(unsigned int i = 0; i < max_amount_; i++)
    {
        Particle& p = particles_[i];
        if(p.age_ < p.life_)
        {
            p.age_ += dt;
            if(p.age_ < p.life_)
            {
                // Update
                if(type_ == Particle_Type::FRAG)
                    UpdateFrag(p, dt);
                else if(type_ == Particle_Type::FLAME)
                    UpdateFlame(p, dt);
                else if(type_ == Particle_Type::SMOKE)
                    UpdateSmoke(p, dt);

                p.camera_dist_ = glm::length2(p.position_ - camera_pos);

                // Fill the GPU buffer
                g_particule_position_size_data_[4*cnt+0] = p.position_.x;
                g_particule_position_size_data_[4*cnt+1] = p.position_.y;
                g_particule_position_size_data_[4*cnt+2] = p.position_.z;
                g_particule_position_size_data_[4*cnt+3] = p.size_;
                                                    
                g_particule_color_data_[4*cnt+0] = p.r;
                g_particule_color_data_[4*cnt+1] = p.g;
                g_particule_color_data_[4*cnt+2] = p.b;
                g_particule_color_data_[4*cnt+3] = p.a;

                cnt++;
            }
            else
            {
                p.camera_dist_ = -1.0f;
                p.life_ = -1.f;
                amount_--;
            }
        }
    }
    SortParticles();
}

unsigned int ParticleGenerator::FirstUnusedParticle()
{
    for (GLuint i = lastUsedParticle_; i < max_amount_; ++i) {
        if (particles_[i].life_ == -1){
            lastUsedParticle = i;
            return i;
        }
    }
    // Otherwise, do a linear search
    for (GLuint i = 0; i < lastUsedParticle_; ++i){
        if (particles_[i].life_ == -1){
            lastUsedParticle = i;
            return i;
        }
    }
    // Override first particle if all others are alive
    lastUsedParticle_ = 0;
    return 0;
}

void ParticleGenerator::RespawnParticle(Particle& particle, glm::vec3 center_pos, glm::vec3 offset)
{
    if(type_ == Particle_Type::FRAG)
        InitFrag(particle, object.position_);
    else if(type_ == Particle_Type::SMOKE)
        InitSmoke(particle, object.position_);
    else if(type_ == Particle_Type::FRAME)
        InitFlame(particle, object.position_);
}

void ParticleGenerator::Draw(glm::mat4 view_mat, glm::mat4 proj_mat)
{
    glUseProgram(shader_);
    glUniformMatrix4fv(glGetUniformLocation(shader_, "view"), 1, GL_FALSE, glm::value_ptr(view_mat));
    glUniformMatrix4fv(glGetUniformLocation(shader_, "projection"), 1, GL_FALSE, glm::value_ptr(proj_mat));
    glm::vec3 camera_right_w = glm::vec3(view_mat[0][0], view_mat[0][1], view_mat[0][2]);
    glUniform3fv(glGetUniformLocation(shader_, "CameraRight_worldspace"), 1, glm::value_ptr(camera_right_w));
    glm::vec3 camera_up_w = glm::vec3(view_mat[0][1], view_mat[1][1], view_mat[2][1]);
    glUniform3fv(glGetUniformLocation(shader_, "CameraUp_worldspace"), 1, glm::value_ptr(camera_up_w));

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat) * 3, this->g_vertex_buffer_data_, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, this->particles_position_buffer_);
    glBufferData(GL_ARRAY_BUFFER, max_amount_ * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, amount_ * sizeof(GLfloat) * 4, g_particule_position_size_data_);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, this->particles_color_buffer_);
    glBufferData(GL_ARRAY_BUFFER, max_amount_ * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, amount_ * sizeof(GLubyte) * 4, g_particule_color_data_);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(GLubyte), (void*)0);
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
	glVertexAttribDivisor(2, 1);

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glUniform1i(glGetUniformLocation(shader_, "myTextureSampler"), 0);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, amount_);

    glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
    glBindVertexArray(0);
}

void ParticleGenerator::InitSmoke(Particle& particle, glm::vec3 center_pos)
{

}

void ParticleGenerator::InitFlame(Particle& particle, glm::vec3 center_pos)
{

}

void ParticleGenerator::InitFrag(Particle& particle, glm::vec3 center_pos)
{

}
    
void ParticleGenerator::UpdateSmoke(Particle& particle, float dt)
{

}

void ParticleGenerator::UpdateFlame(Particle& particle, float dt)
{

}

void ParticleGenerator::UpdateFrag(Particle& particle, float dt)
{

}

#endif