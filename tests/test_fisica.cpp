#include "Fisica.hpp"
#include "SistemaSolar.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

void exigir(bool condicao, const char* mensagem) {
    if (!condicao) {
        throw std::runtime_error(mensagem);
    }
}

void testarIntegracaoSimetrica() {
    std::vector<CorpoCeleste> corpos{
        {"A", 1.0, 1.0, {-1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, glm::vec3(1.0f)},
        {"B", 1.0, 1.0, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, glm::vec3(1.0f)}
    };
    const Fisica fisica(1.0, 0.0);
    fisica.atualizar(corpos, 0.01);

    exigir(corpos[0].posicao.x > -1.0, "O primeiro corpo deveria se aproximar.");
    exigir(corpos[1].posicao.x < 1.0, "O segundo corpo deveria se aproximar.");
    exigir(corpos[0].velocidade.x > 0.0, "A aceleracao do primeiro corpo deveria ser positiva.");
    exigir(corpos[1].velocidade.x < 0.0, "A aceleracao do segundo corpo deveria ser negativa.");
    exigir(std::abs(corpos[0].posicao.x + corpos[1].posicao.x) < 1e-12,
           "A simetria da posicao deveria ser preservada.");
    exigir(std::abs(corpos[0].velocidade.x + corpos[1].velocidade.x) < 1e-12,
           "A simetria da velocidade deveria ser preservada.");
}

void testarValidacaoDoPasso() {
    std::vector<CorpoCeleste> corpos;
    const Fisica fisica;
    bool rejeitouPassoInvalido = false;
    try {
        fisica.atualizar(corpos, 0.0);
    } catch (const std::invalid_argument&) {
        rejeitouPassoInvalido = true;
    }
    exigir(rejeitouPassoInvalido, "Passo de tempo zero deveria ser rejeitado.");
}

void testarCondicoesOrbitaisIniciais() {
    const ConfiguracaoSimulacao configuracao;
    const auto corpos = criarSistemaSolar(configuracao);
    exigir(corpos.size() == 9, "O sistema inicial deveria conter nove corpos.");
    exigir(glm::length(corpos[3].velocidade) > 0.0,
           "A Terra deveria iniciar com velocidade orbital.");

    glm::dvec3 momentoTotal(0.0);
    for (const CorpoCeleste& corpo : corpos) {
        momentoTotal += corpo.massa * corpo.velocidade;
    }
    exigir(glm::length(momentoTotal) < 1e-12,
           "O momento linear inicial deveria ser aproximadamente nulo.");
}

} // namespace

int main() {
    try {
        testarIntegracaoSimetrica();
        testarValidacaoDoPasso();
        testarCondicoesOrbitaisIniciais();
    } catch (const std::exception& erro) {
        std::cerr << "Falha no teste de fisica: " << erro.what() << '\n';
        return 1;
    }
    return 0;
}
