#ifndef SISTEMA_SOLAR_HPP
#define SISTEMA_SOLAR_HPP

#include <cstddef>
#include <vector>

#include "CorpoCeleste.hpp"

struct ConfiguracaoSimulacao {
    double constanteGravitacional = 1e-4;
    double epsilon = 1e-3;
    double escalaTempo = 100.0;
    double passoMaximoQuadro = 0.05;
    double passoMaximoIntegracao = 0.05;
    std::size_t maximoPontosTrilha = 2400;
};

std::vector<CorpoCeleste> criarSistemaSolar(
    const ConfiguracaoSimulacao& configuracao);

#endif // SISTEMA_SOLAR_HPP
