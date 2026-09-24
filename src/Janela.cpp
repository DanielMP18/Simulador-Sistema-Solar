#include "Janela.hpp"
#include <glad/gl.h>
#include <iostream>

Janela::Janela(int largura, int altura, const char* titulo)
{
    if (!glfwInit())
    {
        std::cerr << "Erro ao inicializar GLFW\n";
        window = nullptr;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(largura, altura, titulo, nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Erro ao criar janela\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cerr << "Erro ao inicializar GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
        return;
    }

    // Ativa o Depth Test(algoritmo para ordenação de profundidade 3D)
    glEnable(GL_DEPTH_TEST);
}

bool Janela::deveFechar() const {
    return glfwWindowShouldClose(window);
}

void Janela::atualizar() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Janela::limpar() const {
    glClearColor(0.02f, 0.02f, 0.05f, 1.0f); // Cor do espaço
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow* Janela::getGLFWWindow() const {
    return window;
}

Janela::~Janela() {
    glfwDestroyWindow(window);
    glfwTerminate();
}