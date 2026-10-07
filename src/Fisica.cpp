#include "Fisica.hpp"

#include <cmath>
#include <stdexcept>

namespace {

bool vetorFinito(const glm::dvec3& vetor) {
    return std::isfinite(vetor.x) && std::isfinite(vetor.y) && std::isfinite(vetor.z);
}

} // namespace

Fisica::Fisica(double constanteGravitacional, double epsilon)
    : constanteGravitacional(constanteGravitacional), epsilon(epsilon) {
    if (!std::isfinite(constanteGravitacional) || constanteGravitacional < 0.0) {
        throw std::invalid_argument("A constante gravitacional deve ser finita e nao negativa.");
    }
    if (!std::isfinite(epsilon) || epsilon < 0.0) {
        throw std::invalid_argument("O epsilon deve ser finito e nao negativo.");
    }
}

std::vector<Fisica::Derivada> Fisica::calcularDerivadas(
    const std::vector<Estado>& estado,
    const std::vector<double>& massas) const {
    std::vector<Derivada> derivadas(estado.size());
    const double epsilonQuadrado = epsilon * epsilon;

    for (std::size_t i = 0; i < estado.size(); ++i) {
        glm::dvec3 aceleracao(0.0);
        for (std::size_t j = 0; j < estado.size(); ++j) {
            if (i == j) {
                continue;
            }

            const glm::dvec3 deslocamento = estado[j].posicao - estado[i].posicao;
            const double distanciaQuadrada = glm::dot(deslocamento, deslocamento);
            const double denominador = std::pow(distanciaQuadrada + epsilonQuadrado, 1.5);
            if (denominador > 0.0) {
                aceleracao += constanteGravitacional * massas[j] * deslocamento / denominador;
            }
        }
        derivadas[i] = {estado[i].velocidade, aceleracao};
    }
    return derivadas;
}

void Fisica::atualizar(std::vector<Planeta>& corpos, double passoDeTempo) const {
    if (!std::isfinite(passoDeTempo) || passoDeTempo <= 0.0) {
        throw std::invalid_argument("O passo de tempo deve ser finito e positivo.");
    }

    const std::size_t quantidade = corpos.size();
    std::vector<Estado> estadoInicial(quantidade);
    std::vector<double> massas(quantidade);

    for (std::size_t i = 0; i < quantidade; ++i) {
        estadoInicial[i] = {
            corpos[i].getPosicaoPrecisaoDupla(),
            corpos[i].getVelocidadePrecisaoDupla()
        };
        massas[i] = corpos[i].getMassa();
        if (!vetorFinito(estadoInicial[i].posicao) ||
            !vetorFinito(estadoInicial[i].velocidade) ||
            !std::isfinite(massas[i]) || massas[i] < 0.0) {
            throw std::invalid_argument("O estado e a massa dos corpos devem ser finitos e validos.");
        }
    }

    const auto k1 = calcularDerivadas(estadoInicial, massas);
    auto estadoMeio1 = estadoInicial;
    for (std::size_t i = 0; i < quantidade; ++i) {
        estadoMeio1[i].posicao += 0.5 * passoDeTempo * k1[i].posicao;
        estadoMeio1[i].velocidade += 0.5 * passoDeTempo * k1[i].velocidade;
    }
    const auto k2 = calcularDerivadas(estadoMeio1, massas);

    auto estadoMeio2 = estadoInicial;
    for (std::size_t i = 0; i < quantidade; ++i) {
        estadoMeio2[i].posicao += 0.5 * passoDeTempo * k2[i].posicao;
        estadoMeio2[i].velocidade += 0.5 * passoDeTempo * k2[i].velocidade;
    }
    const auto k3 = calcularDerivadas(estadoMeio2, massas);

    auto estadoFinal = estadoInicial;
    for (std::size_t i = 0; i < quantidade; ++i) {
        estadoFinal[i].posicao += passoDeTempo * k3[i].posicao;
        estadoFinal[i].velocidade += passoDeTempo * k3[i].velocidade;
    }
    const auto k4 = calcularDerivadas(estadoFinal, massas);

    for (std::size_t i = 0; i < quantidade; ++i) {
        const glm::dvec3 novaPosicao = estadoInicial[i].posicao +
            (passoDeTempo / 6.0) *
            (k1[i].posicao + 2.0 * k2[i].posicao + 2.0 * k3[i].posicao + k4[i].posicao);
        const glm::dvec3 novaVelocidade = estadoInicial[i].velocidade +
            (passoDeTempo / 6.0) *
            (k1[i].velocidade + 2.0 * k2[i].velocidade +
             2.0 * k3[i].velocidade + k4[i].velocidade);

        if (!vetorFinito(novaPosicao) || !vetorFinito(novaVelocidade)) {
            throw std::overflow_error("A integracao RK4 produziu um estado nao finito.");
        }
        estadoFinal[i] = {novaPosicao, novaVelocidade};
    }

    for (std::size_t i = 0; i < quantidade; ++i) {
        corpos[i].setPosicaoPrecisaoDupla(estadoFinal[i].posicao);
        corpos[i].setVelocidadePrecisaoDupla(estadoFinal[i].velocidade);
    }
}