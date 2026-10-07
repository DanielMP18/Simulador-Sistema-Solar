#ifndef APLICACAO_HPP
#define APLICACAO_HPP

#include <vector>

#include "Camera.hpp"
#include "Fisica.hpp"
#include "Janela.hpp"
#include "Renderizador.hpp"
#include "SistemaSolar.hpp"

class Aplicacao {
public:
    Aplicacao();
    void executar();

private:
    ConfiguracaoSimulacao configuracao;
    Janela janela;
    Camera camera;
    std::vector<CorpoCeleste> corpos;
    Fisica fisica;
    Renderizador renderizador;
    double tempoAnterior = 0.0;
    bool arrastandoCamera = false;
    bool deslocandoAlvo = false;
    double ultimoMouseX = 0.0;
    double ultimoMouseY = 0.0;

    void atualizar(double deltaTempo);
    void renderizar();

    static void callbackBotaoMouse(
        GLFWwindow* window, int button, int action, int mods);
    static void callbackPosicaoCursor(GLFWwindow* window, double xpos, double ypos);
    static void callbackScroll(GLFWwindow* window, double xoffset, double yoffset);
};

#endif // APLICACAO_HPP
