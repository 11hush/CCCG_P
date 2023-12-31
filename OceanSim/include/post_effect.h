#ifndef _POST_EFFECT_H_
#define _POST_EFFECT_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "Myshader.h"
#include "texture.h"
#include "particle_generator.h"

GLubyte frag_cnt = 0;
class Exploder {
private:
    ParticleGenerator smoke_1;
    ParticleGenerator smoke_2;
    ParticleGenerator flame_1;
    ParticleGenerator frag_1;
    float time_=0;

public:
    Exploder()=default;
    Exploder(unsigned int shader, unsigned int smoke_tex1, unsigned int smoke_tex2, unsigned int flame_tex1, unsigned int frag_tex1) :
        smoke_1(15, Particle_Type::SMOKE), 
        smoke_2(15, Particle_Type::SMOKE), 
        flame_1(15, Particle_Type::FLAME),
        frag_1(FRAG_NUM * 100, Particle_Type::FRAG)
        {};
    ~Exploder() { std::cout << "delete!" << std::endl; };
        
    void Init() { 
        time_ = 0.f;
        smoke_1.init(); 
        smoke_2.init();
        flame_1.init();
        frag_1.init();
    };

    void Update(float dt, glm::vec3 center_pos, glm::vec3 camera_pos) 
    {
        int new_particles_smoke1 = 4;
        if(new_particles_smoke1 > smoke_1.max_amount_ - smoke_1.amount_)
            new_particles_smoke1 = smoke_1.max_amount_ - smoke_1.amount_;
        if(time_ < 3)
            smoke_1.Update(dt, center_pos, new_particles_smoke1, glm::vec3(0), camera_pos);
        else
            smoke_1.Update(dt, center_pos, 0, glm::vec3(0), camera_pos);

        int new_particles_smoke2 = 4;
        if(new_particles_smoke2 > smoke_2.max_amount_ - smoke_2.amount_)
            new_particles_smoke2 = smoke_2.max_amount_ - smoke_2.amount_;
        if(time_ < 3)
            smoke_2.Update(dt, center_pos, new_particles_smoke2, glm::vec3(0), camera_pos);
        else
            smoke_2.Update(dt, center_pos, 0, glm::vec3(0), camera_pos);

        int new_particles_flame1 = 5;
        if(new_particles_flame1 > flame_1.max_amount_ - flame_1.amount_)
            new_particles_flame1 = flame_1.max_amount_ - flame_1.amount_;
        flame_1.Update(dt, center_pos, (time_ > 7) ? 0 : new_particles_flame1, glm::vec3(0), camera_pos);

        frag_1.Update(dt, center_pos, (frag_cnt != 0 || time_ > 5) ? 0 : FRAG_NUM, glm::vec3(0), camera_pos);
        frag_cnt = (frag_cnt + 1) % (FRAG_NUM);

        time_ += dt;
    };

    void Draw(glm::mat4 view_mat, glm::mat4 proj_mat) { 
        smoke_1.Draw(view_mat, proj_mat); 
        smoke_2.Draw(view_mat, proj_mat);
        flame_1.Draw(view_mat, proj_mat);
        frag_1.Draw(view_mat, proj_mat);
    };
};

Exploder::Exploder()
{
    unsigned int shader = get_shader_later("shader/particle.vs", "shader/particle.fs");
    unsigned int smoke_tex1 = loadTexture("tex/particle/smoke1.png");
    unsigned int smoke_tex2 = loadTexture("tex/particle/smoke2.png");
    unsigned int flame_tex1 = loadTexture("tex/particle/flame1.png");
    unsigned int frag_tex1 = loadTexture("tex/particle/flame1.png");

    smoke_1.shader_ = shader;
    smoke_1.texture_ = smoke_tex1;

    smoke_2.shader_ = shader;
    smoke_2.texture_ = smoke_tex2;

    flame_1.shader_ = shader;
    flame_1.texture_ = flame_tex1;

    frag_1.shader_ = shader;
    frag_1.texture_ = frag_tex1;
}

class Effect {
public:
    Exploder exploder_;
    bool explode_begin_ = false;
    bool is_exploding_ = false;
    float time_=0;
    
public:
    void init();
    void begin_explode() { explode_begin_ = true; };
    void Update(float dt, glm::vec3 center_pos, glm::vec3 camera_pos);
    void Draw(glm::mat4 view_mat, glm::mat4 proj_mat);
};

void Effect::init()
{
    if(explode_begin_ && !is_exploding_)  
    {
        exploder_.Init();   
        explode_begin_ = false;
        is_exploding_ = true;
    }
}

void Effect::Update(float dt, glm::vec3 center_pos, glm::vec3 camera_pos)
{
    if(is_exploding_)   
    {
        exploder_.Update(dt, center_pos, camera_pos);
        time_ += dt;
        if(time_ > 5.0f)
        {
            is_exploding_ = false;
            time_ = 0;
        }
    }
}

void Effect::Draw(glm::mat4 view_mat, glm::mat4 proj_mat)
{
    if(is_exploding_)   exploder_.Draw(view_mat, proj_mat);
}

#endif