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

    // Dados da geometria
    std::vector<Vertice> vertices;
    std::vector<unsigned int> indices;

    // Handles de OpenGL (VBO, VAO, EBO)
    unsigned int VAO, VBO, EBO;

    void gerarEsfera(unsigned int sectores = 36, unsigned int stacks = 18); //sectores e stacks sao as camadas verticais e horizontais da esfera
    void configurarBuffers();

public:
    Planeta(const std::string& nome, float raio, float massa, glm::vec3 posicaoInicial, glm::vec3 velocidadeInicial);
    
    ~Planeta();

    // Getters e Setters
    const std::string& getNome() const { return nome; }
    glm::vec3 getPosicao() const { return posicao; }
    void setPosicao(const glm::vec3& novaPos) { posicao = novaPos; }
    
    glm::vec3 getVelocidade() const { return velocidade; }
    void setVelocidade(const glm::vec3& novaVel) { velocidade = novaVel; }

    float getMassa() const { return massa; }
    float getRaio() const { return raio; }

    // Retorna a matriz Model para usar no Shader
    glm::mat4 getMatrizModel() const;

    void desenhar() const;
};

#endif // PLANETA_HPP