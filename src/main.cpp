#include <iostream>
#include <vector>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Janela.hpp"
#include "Planeta.hpp"

unsigned int criarShader() {
    const char* vertexSrc = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 Normal;

        void main() {
            Normal = mat3(transpose(inverse(model))) * aNormal;
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    const char* fragmentSrc = R"(
        #version 330 core
        in vec3 Normal;
        out vec4 FragColor;

        uniform vec3 objectColor;

        void main() {
            vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
            float diff = max(dot(normalize(Normal), lightDir), 0.25);
            FragColor = vec4(objectColor * diff, 1.0);
        }
    )";

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexSrc, NULL);
    glCompileShader(vs);

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentSrc, NULL);
    glCompileShader(fs);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return shaderProgram;
}

int main()
{
    Janela janela(1280, 720, "Sistema Solar 3D");

    unsigned int shaderProgram = criarShader();

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    std::vector<Planeta> sistemaSolar;
    sistemaSolar.reserve(9); // Evita realocações que destroem e recriam buffers

    sistemaSolar.emplace_back("Sol",      2.2f,  1000.0f, glm::vec3(0.0f, 0.0f, 0.0f),  glm::vec3(0.0f), glm::vec3(1.0f, 0.85f, 0.1f));
    sistemaSolar.emplace_back("Mercurio", 0.18f, 0.05f,   glm::vec3(4.5f, 0.0f, 0.0f),  glm::vec3(0.0f), glm::vec3(0.6f, 0.6f, 0.6f));
    sistemaSolar.emplace_back("Venus",    0.32f, 0.8f,    glm::vec3(7.0f, 0.0f, 0.0f),  glm::vec3(0.0f), glm::vec3(0.9f, 0.7f, 0.4f));
    sistemaSolar.emplace_back("Terra",    0.38f, 1.0f,    glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.2f, 0.5f, 1.0f));
    sistemaSolar.emplace_back("Marte",    0.28f, 0.1f,    glm::vec3(13.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.9f, 0.3f, 0.1f));
    sistemaSolar.emplace_back("Jupiter",  0.9f,  317.0f,  glm::vec3(18.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.8f, 0.6f, 0.4f));
    sistemaSolar.emplace_back("Saturno",  0.75f, 95.0f,   glm::vec3(23.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.9f, 0.8f, 0.5f));
    sistemaSolar.emplace_back("Urano",    0.55f, 14.0f,   glm::vec3(27.5f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.4f, 0.8f, 0.9f));
    sistemaSolar.emplace_back("Netuno",   0.55f, 17.0f,   glm::vec3(32.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.1f, 0.3f, 0.9f));

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 30.0f, 50.0f),
        glm::vec3(12.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), 
        1280.0f / 720.0f, 
        0.1f, 
        500.0f
    );

    while (!janela.deveFechar())
    {
        janela.limpar();

        glUseProgram(shaderProgram);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        for (const auto& planeta : sistemaSolar) {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(planeta.getMatrizModel()));
            glUniform3fv(colorLoc, 1, glm::value_ptr(planeta.getCor()));
            planeta.desenhar();
        }

        janela.atualizar();
    }

    glDeleteProgram(shaderProgram);
    return 0;
}