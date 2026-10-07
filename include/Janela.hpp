#ifndef JANELA_HPP
#define JANELA_HPP

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

class Janela final {

public:
    Janela(int largura, int altura, const std::string& titulo);
    ~Janela() noexcept;

    Janela(const Janela&) = delete;
    Janela& operator=(const Janela&) = delete;

    bool deveFechar() const;
    void processarEventos() const;
    void apresentar() const;
    void limpar() const;
    GLFWwindow* getGLFWWindow() const;
    void obterTamanhoFramebuffer(int& largura, int& altura) const;

private:
    GLFWwindow* window = nullptr;

    static void callbackRedimensionamento(GLFWwindow* window, int largura, int altura);
};

#endif // JANELA_HPP