#include "Aplicacao.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
    try {
        Aplicacao aplicacao;
        aplicacao.executar();
    } catch (const std::exception& erro) {
        std::cerr << "Erro ao executar o simulador: " << erro.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
