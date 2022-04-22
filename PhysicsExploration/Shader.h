#ifndef SHADER_H
#define SHADER_H

#include "includes.h"

using namespace std;

class Shader
{
    public:
        // the program ID
        unsigned int ID;

        // constructor reads and builds the shader
        Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
        // use/activate the shader
        void use();
        // utility uniform functions
        void setBool(const string& name, bool value) const;
        void setInt(const string& name, int value) const;
        void setFloat(const string& name, float value) const;
        void setVector(const string& name, glm::vec3 value) const;
        void setVector(const std::string& name, glm::vec4 value) const;
        void setMatrix2(const string& name, glm::mat2 mat) const;
        void setMatrix3(const string& name, glm::mat3 mat) const;
        void setMatrix4(const string& name, glm::mat4 mat) const;
};

#endif