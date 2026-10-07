#ifndef FISICA_HPP
#define FISICA_HPP

#include <vector>

#include "CorpoCeleste.hpp"

class Fisica {
public:
    explicit Fisica(double constanteGravitacional = 1.0, double epsilon = 1e-3);

    void atualizar(std::vector<CorpoCeleste>& corpos, double passoDeTempo) const;

private:
    struct Estado {
        glm::dvec3 posicao;
        glm::dvec3 velocidade;
    };

    struct Derivada {
        glm::dvec3 posicao;
        glm::dvec3 velocidade;
    };

    double constanteGravitacional;
    double epsilon;

    std::vector<Derivada> calcularDerivadas(
        const std::vector<Estado>& estado,
        const std::vector<double>& massas) const;
};

#endif // FISICA_HPP
