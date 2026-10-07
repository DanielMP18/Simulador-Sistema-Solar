#ifndef RENDERIZADOR_HPP
#define RENDERIZADOR_HPP

#include <cstddef>
#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "CorpoCeleste.hpp"

class Renderizador {
public:
    explicit Renderizador(std::size_t maximoPontosTrilha);
    ~Renderizador();

    Renderizador(const Renderizador&) = delete;
    Renderizador& operator=(const Renderizador&) = delete;
    Renderizador(Renderizador&&) = delete;
    Renderizador& operator=(Renderizador&&) = delete;

    void renderizar(
        const std::vector<CorpoCeleste>& corpos,
        const glm::mat4& view,
        const glm::mat4& projection);

private:
    struct Trilha {
        GLuint vao = 0;
        GLuint vbo = 0;
        glm::vec3 cor{1.0f};
        std::vector<glm::vec3> pontos;
    };

    GLuint programa = 0;
    GLuint esferaVao = 0;
    GLuint esferaVbo = 0;
    GLuint esferaEbo = 0;
    GLuint bufferInstancias = 0;
    GLsizei quantidadeIndicesEsfera = 0;
    GLint localModel = -1;
    GLint localView = -1;
    GLint localProjection = -1;
    GLint localCor = -1;
    GLint localPosicaoSol = -1;
    GLint localEhTrilha = -1;
    GLint localEhInstanciado = -1;
    std::size_t maximoPontosTrilha;
    bool trilhasInicializadas = false;
    std::vector<Trilha> trilhas;

    void criarEsfera();
    void criarTrilhas(std::size_t quantidadeCorpos);
    void liberarRecursos() noexcept;
    void atualizarTrilhas(const std::vector<CorpoCeleste>& corpos);
    void desenharTrilhas();
    void desenharCorpos(const std::vector<CorpoCeleste>& corpos);
    void configurarAtributosInstancias();
};

#endif // RENDERIZADOR_HPP
