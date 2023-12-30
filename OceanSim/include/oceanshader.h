//reference1::https://learnopengl.com/Getting-started/Shaders

#ifndef __OCEAN_SHADER_H_
#define __OCEAN_SHADER_H_

#include <glad/glad.h>
#include "glm/glm.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class OceanShader
{
public:
    unsigned int ID;
    OceanShader(const char* computeShader) {
        const char* Context;
        std::string tmp;
        std::ifstream ShaderFile;
        std::stringstream Stm;
        try
        {
            ShaderFile.open(computeShader);
            Stm << ShaderFile.rdbuf();

            ShaderFile.close();

            tmp = Stm.str();
            Context = tmp.c_str();

        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "Can NOT open the file "<<computeShader<< "normally " << e.what() << std::endl;
        }

        unsigned int shader;

        shader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(shader, 1, &Context, NULL);
        glCompileShader(shader);

        GLint success;
        GLchar info[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, info);
            std::cout << "compute Shader "<< computeShader<<" compile failed : \n" << info << "\n " << std::endl;
        }
        ID = glCreateProgram();
        glAttachShader(ID, shader);
        glLinkProgram(ID);
        GLint success3;
        GLchar info3[512];
        glGetProgramiv(ID, GL_LINK_STATUS, &success3);
        if (!success3)
        {
            glGetProgramInfoLog(ID, 512, NULL, info3);
            std::cout << "fail to link the program: \n" << computeShader<<"  " << info3 << "\n" << std::endl;
        }
        glDeleteShader(shader);
    }
    OceanShader(const char* vShader, const char* fShader)
    {
        const char* vContext;
        const char* fContext;
        std::string vtmp;
        std::string ftmp;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::stringstream vStm, fStm;

        try
        {
            vShaderFile.open(vShader);
            fShaderFile.open(fShader);
            vStm << vShaderFile.rdbuf();
            fStm << fShaderFile.rdbuf();
            
            vShaderFile.close();
            fShaderFile.close();

            vtmp = vStm.str();
            ftmp = fStm.str();
            vContext = vtmp.c_str();
            fContext = ftmp.c_str();
            
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "Can NOT open the file normally "<< e.what() << std::endl;
        }

        unsigned int vertex, fragment;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vContext, NULL);
        glCompileShader(vertex);

        GLint success1;
        GLchar info1[512];
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success1);
        if (!success1)
        {
            glGetShaderInfoLog(vertex, 512, NULL, info1);
            std::cout << "vShader compile failed:\n" << info1 << "\n " << std::endl;
        }


        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fContext, NULL);
        glCompileShader(fragment);

        GLint success2;
        GLchar info2[512];
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success2);
        if (!success2)
        {
            glGetShaderInfoLog(fragment, 512, NULL, info2);
            std::cout << "fShader compile failed:\n" << info2 << "\n " << std::endl;
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        GLint success3;
        GLchar info3[512];
        glGetProgramiv(ID, GL_LINK_STATUS, &success3);
        if (!success3)
        {
            glGetProgramInfoLog(ID, 512, NULL, info3);
            std::cout << "fail to link the program: \n" << info3 << "\n" << std::endl;
        }
        glDeleteShader(vertex);
        glDeleteShader(fragment);

    }
    
    // the following functions are copied from https://learnopengl.com/Getting-started/Shaders
    void use()
    {
        glUseProgram(ID);
    }
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void bindTexure1d(unsigned int unit, unsigned int tex, int read_write) {
        unsigned int r_w = (read_write == 1) ? GL_WRITE_ONLY : GL_READ_ONLY;
        glBindImageTexture(unit, tex, 0, GL_FALSE, 0, r_w, GL_R32F);

    }
    void bindTexure2d(unsigned int unit, unsigned int tex, int read_write) {
        unsigned int r_w = (read_write == 1) ? GL_WRITE_ONLY : GL_READ_ONLY;
        glBindImageTexture(unit, tex, 0, GL_FALSE, 0, r_w, GL_RG32F);
    }
    void bindTexure3d(int unit, unsigned int tex, int read_write) {
        unsigned int r_w = (read_write == 1) ? GL_WRITE_ONLY : GL_READ_ONLY;
        glBindImageTexture(unit, tex, 0, GL_FALSE, 0, r_w, GL_RGBA32F);

    }
    void dispatchCompute(int group_x,int group_y,int group_z) {
        glDispatchCompute(group_x,group_y,group_z);
    }
    void memBarrier() {
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
};
#endif