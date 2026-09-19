#include <iostream>
#include <GLFW/glfw3.h>
#include "Janela.hpp"

int main()
{
    Janela janela(800, 600, "Sistema Solar");

    while (!janela.deveFechar())
    {
        janela.limpar();

        // (No futuro, as chamadas de desenho e atualização da lógica virão aqui)

        janela.atualizar();
    }

    // Ao terminar a main, o destrutor ~Janela() é chamado automaticamente
    return 0;
}

