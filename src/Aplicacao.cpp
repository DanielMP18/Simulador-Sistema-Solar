#include "Aplicacao.hpp"

#include <algorithm>
#include <exception>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Aplicacao::Aplicacao()
    : janela(1280, 720, "Sistema Solar 3D"),
      camera(glm::vec3(16.0f, 0.0f, 0.0f), 60.0f, 90.0f, 30.0f),
      corpos(criarSistemaSolar(configuracao)),
      fisica(configuracao.constanteGravitacional, configuracao.epsilon),
      renderizador(configuracao.maximoPontosTrilha) {
}

void Aplicacao::executar() {
    GLFWwindow* const window = janela.getGLFWWindow();
    glfwSetWindowUserPointer(window, this);
    glfwSetMouseButtonCallback(window, callbackBotaoMouse);
    glfwSetCursorPosCallback(window, callbackPosicaoCursor);
    glfwSetScrollCallback(window, callbackScroll);
    tempoAnterior = glfwGetTime();

    while (!janela.deveFechar()) {
        janela.processarEventos();
        const double tempoAtual = glfwGetTime();
        const double deltaTempo =
            std::min(tempoAtual - tempoAnterior, configuracao.passoMaximoQuadro);
        tempoAnterior = tempoAtual;

        atualizar(deltaTempo);
        janela.limpar();
        renderizar();
        janela.apresentar();
    }
}

void Aplicacao::atualizar(double deltaTempo) {
    double tempoRestante = deltaTempo * configuracao.escalaTempo;
    while (tempoRestante > 0.0) {
        const double passo = std::min(
            tempoRestante, configuracao.passoMaximoIntegracao);
        fisica.atualizar(corpos, passo);
        tempoRestante -= passo;
    }
}

void Aplicacao::renderizar() {
    int largura = 0;
    int altura = 0;
    janela.obterTamanhoFramebuffer(largura, altura);
    if (largura <= 0 || altura <= 0) {
        return;
    }

    const glm::mat4 view = camera.getMatrizView();
    const glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(largura) / static_cast<float>(altura),
        0.1f,
        500.0f);
    renderizador.renderizar(corpos, view, projection);
}

void Aplicacao::callbackBotaoMouse(
    GLFWwindow* window, int button, int action, int) {
    auto* const aplicacao =
        static_cast<Aplicacao*>(glfwGetWindowUserPointer(window));
    if (!aplicacao) {
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            glfwGetCursorPos(
                window, &aplicacao->ultimoMouseX, &aplicacao->ultimoMouseY);
            aplicacao->arrastandoCamera = button == GLFW_MOUSE_BUTTON_LEFT;
            aplicacao->deslocandoAlvo = button == GLFW_MOUSE_BUTTON_RIGHT;
        } else if (action == GLFW_RELEASE) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                aplicacao->arrastandoCamera = false;
            } else {
                aplicacao->deslocandoAlvo = false;
            }
        }
    }
}

void Aplicacao::callbackPosicaoCursor(
    GLFWwindow* window, double xpos, double ypos) {
    auto* const aplicacao =
        static_cast<Aplicacao*>(glfwGetWindowUserPointer(window));
    if (!aplicacao ||
        (!aplicacao->arrastandoCamera && !aplicacao->deslocandoAlvo)) {
        return;
    }

    const float deltaX = static_cast<float>(xpos - aplicacao->ultimoMouseX);
    const float deltaY = static_cast<float>(ypos - aplicacao->ultimoMouseY);
    aplicacao->ultimoMouseX = xpos;
    aplicacao->ultimoMouseY = ypos;

    if (aplicacao->arrastandoCamera) {
        aplicacao->camera.processarMovimentoMouse(deltaX, deltaY);
    } else {
        int largura = 0;
        int altura = 0;
        glfwGetFramebufferSize(window, &largura, &altura);
        aplicacao->camera.processarPanMouse(deltaX, deltaY, altura);
    }
}

void Aplicacao::callbackScroll(GLFWwindow* window, double, double yoffset) {
    auto* const aplicacao =
        static_cast<Aplicacao*>(glfwGetWindowUserPointer(window));
    if (aplicacao) {
        aplicacao->camera.processarZoom(static_cast<float>(yoffset));
    }
}
