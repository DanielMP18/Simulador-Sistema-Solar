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
#include "Camera.hpp"
#include "Fisica.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {

void configurarVelocidadesOrbitais(
    std::vector<Planeta>& corpos,
    double constanteGravitacional,
    double epsilon) {
    if (corpos.size() < 2) {
        return;
    }

    constexpr std::size_t indiceSol = 0;
    const auto posicaoSol = corpos[indiceSol].getPosicaoPrecisaoDupla();
    const double massaSol = corpos[indiceSol].getMassa();
    if (massaSol <= 0.0) {
        throw std::invalid_argument("A massa do Sol deve ser positiva para inicializar as orbitas.");
    }

    std::vector<glm::dvec3> velocidadesRelativas(corpos.size(), glm::dvec3(0.0));
    glm::dvec3 momentoRelativoTotal(0.0);
    double massaTotal = massaSol;
    const glm::dvec3 normalOrbital(0.0, 0.0, 1.0);

    for (std::size_t i = 0; i < corpos.size(); ++i) {
        if (i == indiceSol) {
            continue;
        }

        const glm::dvec3 deslocamento =
            corpos[i].getPosicaoPrecisaoDupla() - posicaoSol;
        const double distancia = glm::length(deslocamento);
        if (distancia <= 0.0) {
            throw std::invalid_argument("Um planeta nao pode iniciar na mesma posicao do Sol.");
        }

        const glm::dvec3 direcaoRadial = deslocamento / distancia;
        const glm::dvec3 direcaoTangencial =
            glm::normalize(glm::cross(normalOrbital, direcaoRadial));
        const double massaPlaneta = corpos[i].getMassa();
        const double distanciaSuavizadaQuadrada =
            distancia * distancia + epsilon * epsilon;
        const double velocidadeCircular = std::sqrt(
            constanteGravitacional * (massaSol + massaPlaneta) *
            distancia * distancia /
            std::pow(distanciaSuavizadaQuadrada, 1.5));

        velocidadesRelativas[i] = direcaoTangencial * velocidadeCircular;
        momentoRelativoTotal += massaPlaneta * velocidadesRelativas[i];
        massaTotal += massaPlaneta;
    }

    const glm::dvec3 velocidadeSol = -momentoRelativoTotal / massaTotal;
    corpos[indiceSol].setVelocidadePrecisaoDupla(velocidadeSol);
    for (std::size_t i = 0; i < corpos.size(); ++i) {
        if (i != indiceSol) {
            corpos[i].setVelocidadePrecisaoDupla(velocidadeSol + velocidadesRelativas[i]);
        }
    }
}

} // namespace

unsigned int criarShader() {
    const char* vertexSrc = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 Normal;
        out vec3 FragPos;

        void main() {
            Normal = mat3(transpose(inverse(model))) * aNormal;
            vec4 worldPosition = model * vec4(aPos, 1.0);
            FragPos = worldPosition.xyz;
            gl_Position = projection * view * worldPosition;
        }
    )";

    const char* fragmentSrc = R"(
        #version 330 core
        in vec3 Normal;
        in vec3 FragPos;
        out vec4 FragColor;

        uniform vec3 objectColor;
        uniform vec3 sunPosition;
        uniform bool isSun;
        uniform bool isOrbitTrail;

        void main() {
            if (isOrbitTrail) {
                FragColor = vec4(objectColor, 1.0);
                return;
            }
            if (isSun) {
                FragColor = vec4(objectColor, 1.0);
                return;
            }

            vec3 lightDir = normalize(sunPosition - FragPos);
            float diffuse = max(dot(normalize(Normal), lightDir), 0.0);
            float ambient = 0.08;
            FragColor = vec4(objectColor * (ambient + diffuse), 1.0);
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

struct ControleMouse {
    Camera* camera = nullptr;
    bool arrastando = false;
    bool movendoAlvo = false;
    double ultimoX = 0.0;
    double ultimoY = 0.0;
};

void callbackBotaoMouse(GLFWwindow* window, int button, int action, int mods) {
    auto* controle = static_cast<ControleMouse*>(glfwGetWindowUserPointer(window));
    if (!controle) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            glfwGetCursorPos(window, &controle->ultimoX, &controle->ultimoY);
            controle->arrastando = button == GLFW_MOUSE_BUTTON_LEFT;
            controle->movendoAlvo = button == GLFW_MOUSE_BUTTON_RIGHT;
        } else if (action == GLFW_RELEASE) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                controle->arrastando = false;
            } else {
                controle->movendoAlvo = false;
            }
        }
    }
}

void callbackPosicaoCursor(GLFWwindow* window, double xpos, double ypos) {
    auto* controle = static_cast<ControleMouse*>(glfwGetWindowUserPointer(window));
    if (!controle || (!controle->arrastando && !controle->movendoAlvo) ||
        !controle->camera) return;

    float deltaX = static_cast<float>(xpos - controle->ultimoX);
    float deltaY = static_cast<float>(ypos - controle->ultimoY);

    controle->ultimoX = xpos;
    controle->ultimoY = ypos;

    if (controle->arrastando) {
        controle->camera->processarMovimentoMouse(deltaX, deltaY);
    } else {
        int largura = 0;
        int altura = 0;
        glfwGetFramebufferSize(window, &largura, &altura);
        controle->camera->processarPanMouse(deltaX, deltaY, altura);
    }
}

void callbackScroll(GLFWwindow* window, double xoffset, double yoffset) {
    auto* controle = static_cast<ControleMouse*>(glfwGetWindowUserPointer(window));
    if (!controle || !controle->camera) return;

    controle->camera->processarZoom(static_cast<float>(yoffset));
}

struct TrilhaOrbital {
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    std::vector<glm::vec3> pontos;
};

int main()
{
    Janela janela(1280, 720, "Sistema Solar 3D");

    Camera camera(glm::vec3(16.0f, 0.0f, 0.0f), 60.0f, 90.0f, 30.0f);

    ControleMouse controleMouse{&camera, false, false, 0.0, 0.0};
    glfwSetWindowUserPointer(janela.getGLFWWindow(), &controleMouse);
    glfwSetMouseButtonCallback(janela.getGLFWWindow(), callbackBotaoMouse);
    glfwSetCursorPosCallback(janela.getGLFWWindow(), callbackPosicaoCursor);
    glfwSetScrollCallback(janela.getGLFWWindow(), callbackScroll);

    unsigned int shaderProgram = criarShader();

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    int sunPositionLoc = glGetUniformLocation(shaderProgram, "sunPosition");
    int isSunLoc = glGetUniformLocation(shaderProgram, "isSun");
    int isOrbitTrailLoc = glGetUniformLocation(shaderProgram, "isOrbitTrail");

    std::vector<Planeta> sistemaSolar;
    sistemaSolar.reserve(9); // Evita realocações que destroem e recriam buffers

    sistemaSolar.emplace_back("Sol",      2.2f,  1000.0, glm::dvec3(0.0, 0.0, 0.0),  glm::dvec3(0.0), glm::vec3(1.0f, 0.85f, 0.1f));
    sistemaSolar.emplace_back("Mercurio", 0.18f, 0.000166, glm::dvec3(4.5, 0.0, 0.0),  glm::dvec3(0.0), glm::vec3(0.6f, 0.6f, 0.6f));
    sistemaSolar.emplace_back("Venus",    0.32f, 0.002447, glm::dvec3(7.0, 0.0, 0.0),  glm::dvec3(0.0), glm::vec3(0.9f, 0.7f, 0.4f));
    sistemaSolar.emplace_back("Terra",    0.38f, 0.003003, glm::dvec3(10.0, 0.0, 0.0), glm::dvec3(0.0), glm::vec3(0.2f, 0.5f, 1.0f));
    sistemaSolar.emplace_back("Marte",   0.28f, 0.000321, glm::dvec3(13.0, 0.0, 0.0), glm::dvec3(0.0), glm::vec3(0.9f, 0.3f, 0.1f));
    sistemaSolar.emplace_back("Jupiter",  0.9f,  0.9543,   glm::dvec3(18.0, 0.0, 0.0), glm::dvec3(0.0), glm::vec3(0.8f, 0.6f, 0.4f));
    sistemaSolar.emplace_back("Saturno",  0.75f, 0.2859,  glm::dvec3(23.0, 0.0, 0.0), glm::dvec3(0.0), glm::vec3(0.9f, 0.8f, 0.5f));
    sistemaSolar.emplace_back("Urano",   0.55f, 0.04354,  glm::dvec3(27.5, 0.0, 0.0), glm::dvec3(0.0), glm::vec3(0.4f, 0.8f, 0.9f));
    sistemaSolar.emplace_back("Netuno",   0.55f, 0.05135, glm::dvec3(32.0, 0.0, 0.0), glm::dvec3(0.0), glm::vec3(0.1f, 0.3f, 0.9f));

    constexpr std::size_t maxPontosTrilha = 2400;
    std::vector<TrilhaOrbital> trilhas(sistemaSolar.size());
    for (std::size_t i = 1; i < sistemaSolar.size(); ++i) {
        auto& trilha = trilhas[i];
        trilha.pontos.push_back(sistemaSolar[i].getPosicao());

        glGenVertexArrays(1, &trilha.VAO);
        glGenBuffers(1, &trilha.VBO);
        glBindVertexArray(trilha.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, trilha.VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            maxPontosTrilha * sizeof(glm::vec3),
            nullptr,
            GL_DYNAMIC_DRAW);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            sizeof(glm::vec3),
            trilha.pontos.data());
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    }
    glBindVertexArray(0);

    constexpr double constanteGravitacional = 1e-4;
    constexpr double epsilon = 1e-3;
    configurarVelocidadesOrbitais(sistemaSolar, constanteGravitacional, epsilon);
    Fisica fisica(constanteGravitacional, epsilon);
    constexpr double escalaTempoSimulacao = 100.0;
    constexpr double passoMaximoIntegracao = 0.05;
    double tempoAnterior = glfwGetTime();

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), 
        1280.0f / 720.0f, 
        0.1f, 
        500.0f
    );

    while (!janela.deveFechar())
    {
        const double tempoAtual = glfwGetTime();
        double tempoRestante =
            std::min(tempoAtual - tempoAnterior, 0.05) * escalaTempoSimulacao;
        tempoAnterior = tempoAtual;
        while (tempoRestante > 0.0) {
            const double passoDeTempo =
                std::min(tempoRestante, passoMaximoIntegracao);
            fisica.atualizar(sistemaSolar, passoDeTempo);
            tempoRestante -= passoDeTempo;
        }

        for (std::size_t i = 1; i < sistemaSolar.size(); ++i) {
            auto& pontos = trilhas[i].pontos;
            pontos.push_back(sistemaSolar[i].getPosicao());
            if (pontos.size() > maxPontosTrilha) {
                pontos.erase(pontos.begin());
            }

            glBindBuffer(GL_ARRAY_BUFFER, trilhas[i].VBO);
            glBufferSubData(
                GL_ARRAY_BUFFER,
                0,
                pontos.size() * sizeof(glm::vec3),
                pontos.data());
        }

        janela.limpar();

        glUseProgram(shaderProgram);

        glm::mat4 view = camera.getMatrizView();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3fv(sunPositionLoc, 1, glm::value_ptr(sistemaSolar[0].getPosicao()));
        glUniform1i(isOrbitTrailLoc, GL_TRUE);
        glUniform1i(isSunLoc, GL_FALSE);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        for (std::size_t i = 1; i < sistemaSolar.size(); ++i) {
            glUniform3fv(colorLoc, 1, glm::value_ptr(sistemaSolar[i].getCor() * 0.75f));
            glBindVertexArray(trilhas[i].VAO);
            glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(trilhas[i].pontos.size()));
        }
        glUniform1i(isOrbitTrailLoc, GL_FALSE);

        for (std::size_t i = 0; i < sistemaSolar.size(); ++i) {
            const auto& planeta = sistemaSolar[i];
            glUniform1i(isSunLoc, i == 0 ? GL_TRUE : GL_FALSE);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(planeta.getMatrizModel()));
            glUniform3fv(colorLoc, 1, glm::value_ptr(planeta.getCor()));
            planeta.desenhar();
        }

        janela.atualizar();
    }

    for (std::size_t i = 1; i < trilhas.size(); ++i) {
        glDeleteBuffers(1, &trilhas[i].VBO);
        glDeleteVertexArrays(1, &trilhas[i].VAO);
    }
    glDeleteProgram(shaderProgram);
    return 0;
}