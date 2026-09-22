#include "Planeta.hpp"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <utility>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Planeta::Planeta(const std::string& nome, float raio, float massa, 
                 glm::vec3 posicaoInicial, glm::vec3 velocidadeInicial, glm::vec3 cor)
    : nome(nome), raio(raio), massa(massa), posicao(posicaoInicial), 
      velocidade(velocidadeInicial), cor(cor), VAO(0), VBO(0), EBO(0) {
    
    gerarEsfera(36, 18);
    configurarBuffers();
}
//&& referencia valor temporario, que será destruído após a execução da função.
//rouba recursos de um objeto temporário para outro já existente, em vez de fazer uma cópia pesada e recriar buffers na placa de vídeo.
Planeta::Planeta(Planeta&& outro) noexcept //noexcept indica que a função não lançará exceções, permitindo otimizações pelo compilador.
    : nome(std::move(outro.nome)), raio(outro.raio), massa(outro.massa), 
      posicao(outro.posicao), velocidade(outro.velocidade), cor(outro.cor),
      vertices(std::move(outro.vertices)), indices(std::move(outro.indices)),
      VAO(outro.VAO), VBO(outro.VBO), EBO(outro.EBO) {
    
    outro.VAO = 0;
    outro.VBO = 0;
    outro.EBO = 0;
}

Planeta& Planeta::operator=(Planeta&& outro) noexcept {
    if (this != &outro) {
        if (EBO != 0) glDeleteBuffers(1, &EBO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);

        nome = std::move(outro.nome);
        raio = outro.raio;
        massa = outro.massa;
        posicao = outro.posicao;
        velocidade = outro.velocidade;
        cor = outro.cor;
        vertices = std::move(outro.vertices);
        indices = std::move(outro.indices);

        VAO = outro.VAO;
        VBO = outro.VBO;
        EBO = outro.EBO;

        outro.VAO = 0;
        outro.VBO = 0;
        outro.EBO = 0;
    }
    return *this;
}

Planeta::~Planeta() {
    if (EBO != 0) glDeleteBuffers(1, &EBO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
}

void Planeta::gerarEsfera(unsigned int sectores, unsigned int stacks) {
    vertices.clear();
    indices.clear();

    float sectorStep = 2.0f * static_cast<float>(M_PI) / sectores;
    float stackStep = static_cast<float>(M_PI) / stacks;

    for (unsigned int i = 0; i <= stacks; ++i) {
        float stackAngle = static_cast<float>(M_PI) / 2.0f - i * stackStep;
        float xy = cosf(stackAngle);
        float z = sinf(stackAngle);

        for (unsigned int j = 0; j <= sectores; ++j) {
            float sectorAngle = j * sectorStep;

            Vertice vert;
            vert.posicao.x = xy * cosf(sectorAngle);
            vert.posicao.y = xy * sinf(sectorAngle);
            vert.posicao.z = z;

            vert.normal = vert.posicao;

            vert.texCoords.s = static_cast<float>(j) / sectores;
            vert.texCoords.t = static_cast<float>(i) / stacks;

            vertices.push_back(vert);
        }
    }

    for (unsigned int i = 0; i < stacks; ++i) {
        unsigned int k1 = i * (sectores + 1);
        unsigned int k2 = k1 + sectores + 1;

        for (unsigned int j = 0; j < sectores; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

void Planeta::configurarBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertice), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, posicao));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, texCoords));

    glBindVertexArray(0);
}

glm::mat4 Planeta::getMatrizModel() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, posicao);
    model = glm::scale(model, glm::vec3(raio));
    return model;
}

void Planeta::desenhar() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}