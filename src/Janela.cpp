#include "Janela.hpp"

#include <glad/gl.h>

#include <stdexcept>

Janela::Janela(int largura, int altura, const std::string& titulo) {
    if (largura <= 0 || altura <= 0) {
        throw std::invalid_argument("As dimensoes da janela devem ser positivas.");
    }

    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("Falha ao inicializar GLFW.");
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(largura, altura, titulo.c_str(), nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Falha ao criar a janela GLFW.");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();
        throw std::runtime_error("Falha ao inicializar GLAD.");
    }

    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(window, callbackRedimensionamento);
    int framebufferLargura = 0;
    int framebufferAltura = 0;
    glfwGetFramebufferSize(window, &framebufferLargura, &framebufferAltura);
    callbackRedimensionamento(window, framebufferLargura, framebufferAltura);
}

bool Janela::deveFechar() const {
    return glfwWindowShouldClose(window) == GLFW_TRUE;
}

void Janela::processarEventos() const {
    glfwPollEvents();
}

void Janela::apresentar() const {
    glfwSwapBuffers(window);
}

void Janela::limpar() const {
    glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow* Janela::getGLFWWindow() const {
    return window;
}

void Janela::obterTamanhoFramebuffer(int& largura, int& altura) const {
    glfwGetFramebufferSize(window, &largura, &altura);
}

void Janela::callbackRedimensionamento(GLFWwindow*, int largura, int altura) {
    glViewport(0, 0, largura, altura);
}

Janela::~Janela() noexcept {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}