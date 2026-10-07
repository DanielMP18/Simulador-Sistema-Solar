#include "SistemaSolar.hpp"

#include <cmath>
#include <stdexcept>

std::vector<CorpoCeleste> criarSistemaSolar(
    const ConfiguracaoSimulacao& configuracao) {
    if (!std::isfinite(configuracao.constanteGravitacional) ||
        configuracao.constanteGravitacional < 0.0 ||
        !std::isfinite(configuracao.epsilon) || configuracao.epsilon < 0.0 ||
        !std::isfinite(configuracao.escalaTempo) || configuracao.escalaTempo <= 0.0 ||
        !std::isfinite(configuracao.passoMaximoQuadro) ||
        configuracao.passoMaximoQuadro <= 0.0 ||
        !std::isfinite(configuracao.passoMaximoIntegracao) ||
        configuracao.passoMaximoIntegracao <= 0.0 ||
        configuracao.maximoPontosTrilha == 0) {
        throw std::invalid_argument("A configuracao da simulacao contem valores invalidos.");
    }

    std::vector<CorpoCeleste> corpos{
        // Nome       Raio  Massa     Posicao          Velocidade      Cor
        {"Sol",       2.2,  1000.0,   {0.0, 0.0, 0.0},  {0.0, 0.0, 0.0}, {1.0f, 0.85f, 0.1f}},
        {"Mercurio",  0.18, 0.000166, {4.5, 0.0, 0.0},  {0.0, 0.0, 0.0}, {0.6f, 0.6f, 0.6f}},
        {"Venus",     0.32, 0.002447, {7.0, 0.0, 0.0},  {0.0, 0.0, 0.0}, {0.9f, 0.7f, 0.4f}},
        {"Terra",     0.38, 0.003003, {10.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.2f, 0.5f, 1.0f}},
        {"Marte",     0.28, 0.000321, {13.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.9f, 0.3f, 0.1f}},
        {"Jupiter",   0.9,  0.9543,   {18.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.8f, 0.6f, 0.4f}},
        {"Saturno",   0.75, 0.2859,  {23.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.9f, 0.8f, 0.5f}},
        {"Urano",     0.55, 0.04354, {27.5, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.4f, 0.8f, 0.9f}},
        {"Netuno",    0.55, 0.05135, {32.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.1f, 0.3f, 0.9f}}
    };

    constexpr std::size_t indiceSol = 0;
    const double massaSol = corpos[indiceSol].massa;
    if (!std::isfinite(massaSol) || massaSol <= 0.0) {
        throw std::invalid_argument("A massa do Sol deve ser finita e positiva.");
    }

    std::vector<glm::dvec3> velocidadesRelativas(corpos.size(), glm::dvec3(0.0));
    glm::dvec3 momentoRelativoTotal(0.0);
    double massaTotal = massaSol;
    const glm::dvec3 normalOrbital(0.0, 0.0, 1.0);
    const double epsilonQuadrado = configuracao.epsilon * configuracao.epsilon;

    for (std::size_t i = 1; i < corpos.size(); ++i) {
        const glm::dvec3 deslocamento = corpos[i].posicao - corpos[indiceSol].posicao;
        const double distancia = glm::length(deslocamento);
        if (!std::isfinite(distancia) || distancia <= 0.0) {
            throw std::invalid_argument("Um planeta deve iniciar a uma distancia positiva do Sol.");
        }

        const glm::dvec3 direcaoRadial = deslocamento / distancia;
        const glm::dvec3 direcaoTangencial =
            glm::normalize(glm::cross(normalOrbital, direcaoRadial));
        const double massaPlaneta = corpos[i].massa;
        const double distanciaSuavizadaQuadrada =
            distancia * distancia + epsilonQuadrado;
        const double velocidadeCircular = std::sqrt(
            configuracao.constanteGravitacional * (massaSol + massaPlaneta) *
            distancia * distancia / std::pow(distanciaSuavizadaQuadrada, 1.5));

        velocidadesRelativas[i] = direcaoTangencial * velocidadeCircular;
        momentoRelativoTotal += massaPlaneta * velocidadesRelativas[i];
        massaTotal += massaPlaneta;
    }

    const glm::dvec3 velocidadeSol = -momentoRelativoTotal / massaTotal;
    corpos[indiceSol].velocidade = velocidadeSol;
    for (std::size_t i = 1; i < corpos.size(); ++i) {
        corpos[i].velocidade = velocidadeSol + velocidadesRelativas[i];
    }

    return corpos;
}
