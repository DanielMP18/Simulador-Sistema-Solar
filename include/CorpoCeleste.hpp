#ifndef CORPO_CELESTE_HPP
#define CORPO_CELESTE_HPP

#include <string>

#include <glm/glm.hpp>

struct CorpoCeleste {
    std::string nome;
    double raio;
    double massa;
    glm::dvec3 posicao;
    glm::dvec3 velocidade;
    glm::vec3 cor;
};

#endif // CORPO_CELESTE_HPP
