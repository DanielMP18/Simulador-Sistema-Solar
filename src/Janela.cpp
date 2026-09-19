#include "Janela.hpp"
#include <iostream>

Janela::Janela(int largura, int altura, const char* titulo)
{
    if (!glfwInit())
    {
        std::cerr << "Erro ao inicializar GLFW\n";
        window = nullptr;
        return;
    }

    window = glfwCreateWindow(largura, altura, titulo, nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Erro ao criar janela\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
}

bool Janela::deveFechar() const {
    return glfwWindowShouldClose(window);
}

void Janela::atualizar() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Janela::limpar() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Janela::~Janela() {
    glfwDestroyWindow(window);
    glfwTerminate();
}