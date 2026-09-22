#ifndef PLANETA_HPP
#define PLANETA_HPP

#include <string>
#include <vector>
#include <glm/glm.hpp>

struct Vertice {
    glm::vec3 posicao;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Planeta {
private:
    std::string nome;
    float raio;
    glm::vec3 posicao;
    glm::vec3 velocidade;
    float massa;
    glm::vec3 cor;

    std::vector<Vertice> vertices;
    std::vector<unsigned int> indices;

    unsigned int VAO, VBO, EBO;

    void gerarEsfera(unsigned int sectores = 36, unsigned int stacks = 18);
    void configurarBuffers();

public:
    Planeta(const std::string& nome, float raio, float massa, 
            glm::vec3 posicaoInicial, glm::vec3 velocidadeInicial, 
            glm::vec3 cor = glm::vec3(1.0f));
    
    ~Planeta();

    // Desativa cópia para proteger os buffers do OpenGL
    Planeta(const Planeta&) = delete;
    Planeta& operator=(const Planeta&) = delete;

    // Habilita movimentação (Move Semantics)
    Planeta(Planeta&& outro) noexcept;
    Planeta& operator=(Planeta&& outro) noexcept;

    const std::string& getNome() const { return nome; }
    glm::vec3 getPosicao() const { return posicao; }
    void setPosicao(const glm::vec3& novaPos) { posicao = novaPos; }
    
    glm::vec3 getVelocidade() const { return velocidade; }
    void setVelocidade(const glm::vec3& novaVel) { velocidade = novaVel; }

    float getMassa() const { return massa; }
    float getRaio() const { return raio; }
    glm::vec3 getCor() const { return cor; }

    glm::mat4 getMatrizModel() const;
    void desenhar() const;
};

#endif // PLANETA_HPP