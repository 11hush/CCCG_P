#ifndef _OCEAN_H__
#define _OCEAN_H__
#include <random> // for the usage of distribution
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <cstdlib>
// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include "oceanshader.h"
#include <utility>
#define OCEAN_MAX_HEIGHT 1000.0
class Ocean {
public:
    // basic settings
    int N{ 256 };
    const int num{64 };
    const int L{ 1000 };
    int steps;
    const int local_work_group_size = 32;
    int global_work_group_size ;

    const float g = 9.8; // the g in physics
    const float A = 4e-7f; // phillips parameter
    const float pi = 3.14159265358979323;
    const int speed = 30.0; // the speed of the wind
    const glm::vec2 windDirection = glm::vec2(1.0, 1.0); // the direction of the wind
    
    float t{ 0.0 }; // the current time
    // for h0
    std::default_random_engine gen;
    std::normal_distribution<float> normal_dist;

    // bind to gl
    unsigned int VAO{ 0 };
    unsigned int VBO{ 0 };
    unsigned int EBO{ 0 };
    unsigned int t_H0{ 0 };
    unsigned int t_H0Conj{ 0 };
    unsigned int t_Ht{ 0 };
    unsigned int t_Dx{ 0 };
    unsigned int t_Dz{ 0 };
    unsigned int t_Nx{ 0 };
    unsigned int t_Nz{ 0 };
    unsigned int t_ResX{ 0 };
    unsigned int t_ResY{ 0 };
    unsigned int t_ResZ{ 0 };
    unsigned int t_Tmp{ 0 }; // needed since two FFT
    unsigned int t_Pos{ 0 }; // final result position
    unsigned int t_Norm{ 0 }; // final result norm
    unsigned int t_Idx{ 0 }; // the butterfly idx



    // shaders
    OceanShader heightShader = OceanShader("../shader/GenHeight.comp");
    OceanShader xzShader = OceanShader("../shader/GenXZ.comp");
    OceanShader normalShader = OceanShader("../shader/GenNormal.comp");
    OceanShader fftxShader = OceanShader("../shader/FFTX.comp");
    OceanShader fftyShader = OceanShader("../shader/FFTY.comp");
    OceanShader finalShader = OceanShader("../shader/GenFinal.comp");
    
    // for FFT's index
    int bit_rev(int bit, int in) {
        int ret = 0;
        for (int i = 0; i < bit; i++) {
            ret <<= 1;
            if (in & 1) ret += 1;
            in >>= 1;
        }
        return ret;
    }
    glm::vec2 h0(glm::vec2 vec_k) {
        float eps_r = normal_dist(gen);
        float eps_i = normal_dist(gen);
        // first calculate P
        float p = 0.0;
        if (vec_k.x != 0.0 || vec_k.y != 0.0) {
            float k_length = glm::length(vec_k);
            glm::vec2 k_nor = glm::normalize(vec_k);
            glm::vec2 wind_nor = glm::normalize(windDirection);
            float kw = glm::dot(k_nor, wind_nor);
            float ssg = (speed * speed / g);
            p = A * std::exp(-1 / (ssg * ssg * k_length * k_length)) / (k_length * k_length * k_length * k_length) * kw * kw;
        }
        return std::sqrt(0.5f) * glm::vec2(eps_r, eps_i) * std::sqrt(p);
    }

    void bind_texture(unsigned int& texture, float* data, int dim) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        unsigned int format = (dim == 2) ? GL_RG32F : GL_RGBA32F;
        unsigned int type = (dim == 2) ? GL_RG : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, N, N, 0, type, GL_FLOAT, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    unsigned int* indexes{ NULL };
    float* vertexes{ NULL };
    float* h0_ctxt{ NULL };
    float* h0_conj_ctxt{ NULL };
    float* fft_idx{ NULL };
    Ocean(int N=256) {

        this->N = N;
        steps = (int)std::log2(N);
        global_work_group_size = N / local_work_group_size;
        // initialize the start data
        vertexes = new float[N * N * 5];
        float start_x = -L / 2.0;
        float start_z = -L / 2.0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {

                vertexes[i * N * 5 + j * 5] = start_x;
                vertexes[i * N * 5 + j * 5 + 1] = 0.0;
                vertexes[i * N * 5 + j * 5 + 2] = start_z;
                vertexes[i * N * 5 + j * 5 + 3] = (float)j / (N - 1);
                vertexes[i * N * 5 + j * 5 + 4] = (float)i / (N - 1);
                start_x += (float)L / (float)(N-1);
            }
            start_x -= L / 2.0;
            start_z += (float)L / (float)(N - 1);

        }
        // next initialize the indexes
        // similar to sphere 
        indexes = new unsigned int[(N - 1) * (N - 1) * 6];
        for (int i = 0; i < N - 1; i++) {
            for (int j = 0; j < N - 1; j++) {
                //two triangles
                indexes[i * (N - 1) * 6 + j * 6] = i * N + j;
                indexes[i * (N - 1) * 6 + j * 6 + 1] = i * N + j + 1;
                indexes[i * (N - 1) * 6 + j * 6 + 2] = (i + 1) * N + j;
                indexes[i * (N - 1) * 6 + j * 6 + 3] = 1 + i * N + j;
                indexes[i * (N - 1) * 6 + j * 6 + 4] = (i + 1) * N + j;
                indexes[i * (N - 1) * 6 + j * 6 + 5] = (i + 1) * N + j + 1;
            }
        }

        // get h0 and  h0_conj to store it
        // gen.seed(time(NULL));
        gen.seed(1);
        
        h0_ctxt = new float[N * N * 2];
        h0_conj_ctxt = new float[N * N * 2];
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                glm::vec2 k = glm::vec2(2 * pi * (i - N / 2) / L, 2 * pi * (j - N / 2) / L);

                glm::vec2 h0_vec = h0(k);
                glm::vec2 h0_vec_conj = h0(-k);
                h0_ctxt[i * 2 * N + j * 2] = h0_vec.x;
                h0_ctxt[i * 2 * N + j * 2 + 1] = h0_vec.y;

                h0_conj_ctxt[i * 2 * N + j * 2] = h0_vec_conj.x;
                h0_conj_ctxt[i * 2 * N + j * 2 + 1] = -h0_vec_conj.y; // note that this is conj!

            }
        }
        fft_idx = new float[N];
        for (int i = 0; i < N; i++) fft_idx[i] = bit_rev(steps, i);

        // bind the index and vertex
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, N * N * 5 * sizeof(float), vertexes, GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (N - 1) * (N - 1) * 6 * sizeof(int), indexes, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // bind the texture
        //
        bind_texture(t_H0, h0_ctxt, 2);
        bind_texture(t_H0Conj, h0_conj_ctxt, 2);
        bind_texture(t_Ht, 0, 2);
        bind_texture(t_Dx, 0, 2);
        bind_texture(t_Dz, 0, 2);
        bind_texture(t_Nx, 0, 2);
        bind_texture(t_Nz, 0, 2);
        bind_texture(t_ResX, 0, 2);
        bind_texture(t_ResY, 0, 2);
        bind_texture(t_ResZ, 0, 2);
        bind_texture(t_Tmp, 0, 2);
        bind_texture(t_Pos, 0, 3);
        bind_texture(t_Norm, 0, 3);
        // finally for FFT index
        // special care since you know, fft
        glGenTextures(1, &t_Idx);
        glBindTexture(GL_TEXTURE_1D, t_Idx);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, N, 0, GL_RED, GL_FLOAT, fft_idx);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    ~Ocean() {
        delete[] vertexes;
        delete[] indexes;
        delete[] h0_ctxt;
        delete[] h0_conj_ctxt;
        delete[] fft_idx;
    }

    //
    void calculate(float delta_t) {
        t += delta_t;
        // first calculat Ht
        heightShader.use();



        heightShader.bindTexure2d(0, t_H0, 0);

        heightShader.bindTexure2d(1, t_H0Conj, 0);
        heightShader.bindTexure2d(2, t_Ht, 1);

        heightShader.setFloat("time", t);
        heightShader.setInt("N",N);

        heightShader.dispatchCompute(global_work_group_size, global_work_group_size, 1);
        heightShader.memBarrier();

        xzShader.use();
        xzShader.bindTexure2d(0, t_Ht, 0);
        xzShader.bindTexure2d(1, t_Dx, 1);
        xzShader.bindTexure2d(2, t_Dz, 1);
        xzShader.setInt("N", N);

        xzShader.dispatchCompute(global_work_group_size, global_work_group_size, 1);
        xzShader.memBarrier();

        normalShader.use();
        normalShader.bindTexure2d(0, t_Ht, 0);
        normalShader.bindTexure2d(1, t_Nx, 1);
        normalShader.bindTexure2d(2, t_Nz, 1);
        normalShader.setInt("N", N);


        normalShader.dispatchCompute(global_work_group_size, global_work_group_size, 1);
        normalShader.memBarrier();

        // first y
        fftxShader.use();
        fftxShader.bindTexure2d(0, t_Ht, 0);
        fftxShader.bindTexure2d(1, t_Tmp, 1);
        fftxShader.bindTexure1d(2, t_Idx, 0);
        fftxShader.setInt("N", N);


        fftxShader.dispatchCompute(1, N, 1);
        fftxShader.memBarrier();

        fftyShader.use();
        fftyShader.bindTexure2d(0, t_Tmp, 0);
        fftyShader.bindTexure2d(1, t_ResY, 1);
        fftyShader.bindTexure1d(2, t_Idx, 0);
        fftyShader.setInt("N", N);


        fftyShader.dispatchCompute(N, 1, 1);
        fftyShader.memBarrier();

        // then x
        fftxShader.use();
        fftxShader.bindTexure2d(0, t_Dx, 0);
        fftxShader.bindTexure2d(1, t_Tmp, 1);
        fftxShader.bindTexure1d(2, t_Idx, 0);
        fftxShader.setInt("N", N);


        fftxShader.dispatchCompute(1, N, 1);
        fftxShader.memBarrier();

        fftyShader.use();
        fftyShader.bindTexure2d(0, t_Tmp, 0);
        fftyShader.bindTexure2d(1, t_ResX, 1);
        fftyShader.bindTexure1d(2, t_Idx, 0);
        fftyShader.setInt("N", N);


        fftyShader.dispatchCompute(N, 1, 1);
        fftyShader.memBarrier();

        // then z
        fftxShader.use();
        fftxShader.bindTexure2d(0, t_Dz, 0);
        fftxShader.bindTexure2d(1, t_Tmp, 1);
        fftxShader.bindTexure1d(2, t_Idx, 0);
        fftxShader.setInt("N", N);


        fftxShader.dispatchCompute(1, N, 1);
        fftxShader.memBarrier();

        fftyShader.use();
        fftyShader.bindTexure2d(0, t_Tmp, 0);
        fftyShader.bindTexure2d(1, t_ResZ, 1);
        fftyShader.bindTexure1d(2, t_Idx, 0);
        fftyShader.setInt("N", N);


        fftyShader.dispatchCompute(N, 1, 1);
        fftyShader.memBarrier();

        // Nx
        fftxShader.use();
        fftxShader.bindTexure2d(0, t_Nx, 0);
        fftxShader.bindTexure2d(1, t_Tmp, 1);
        fftxShader.bindTexure1d(2, t_Idx, 0);
        fftxShader.setInt("N", N);


        fftxShader.dispatchCompute(1, N, 1);
        fftxShader.memBarrier();

        fftyShader.use();
        fftyShader.bindTexure2d(0, t_Tmp, 0);
        fftyShader.bindTexure2d(1, t_Nx, 1);
        fftyShader.bindTexure1d(2, t_Idx, 0);
        fftyShader.setInt("N", N);

        fftyShader.dispatchCompute(N, 1, 1);
        fftyShader.memBarrier();
        //Nz
        fftxShader.use();
        fftxShader.bindTexure2d(0, t_Nz, 0);
        fftxShader.bindTexure2d(1, t_Tmp, 1);
        fftxShader.bindTexure1d(2, t_Idx, 0);
        fftxShader.setInt("N", N);

        fftxShader.dispatchCompute(1, N, 1);
        fftxShader.memBarrier();

        fftyShader.use();
        fftyShader.bindTexure2d(0, t_Tmp, 0);
        fftyShader.bindTexure2d(1, t_Nz, 1);
        fftyShader.bindTexure1d(2, t_Idx, 0);
        fftyShader.setInt("N", N);

        fftyShader.dispatchCompute(N, 1, 1);
        fftyShader.memBarrier();

        // calculate the final result
        finalShader.use();
        finalShader.bindTexure2d(0, t_ResX, 0);
        finalShader.bindTexure2d(1, t_ResY, 0);
        finalShader.bindTexure2d(2, t_ResZ, 0);
        finalShader.bindTexure2d(3, t_Nx, 0);
        finalShader.bindTexure2d(4, t_Nz, 0);
        finalShader.bindTexure3d(5, t_Pos, 1);
        finalShader.bindTexure3d(6, t_Norm, 1);
        finalShader.setInt("N", N);

        finalShader.dispatchCompute(global_work_group_size, global_work_group_size, 1);
        finalShader.memBarrier();



    }

    void draw() {
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t_Pos);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, t_Norm);
        glBindVertexArray(VAO);
       // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, (N - 1) * (N - 1) * 6, GL_UNSIGNED_INT, 0);

    }
};

#endif // !_OCEAN_H_