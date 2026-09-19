#ifndef JANELA_HPP
#define JANELA_HPP

#include <GLFW/glfw3.h>

class Janela {

public:
    Janela(int largura, int altura, const char* titulo);
    ~Janela();
    bool deveFechar() const;
    void atualizar(); 
    void limpar() const;

private:
    GLFWwindow* window;
    int largura() const;
    int altura() const;
};
#endif // JANELA_HPP