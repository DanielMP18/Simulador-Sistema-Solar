#include "Renderizador.hpp"

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {

constexpr float pi = 3.14159265358979323846f;
constexpr unsigned int setoresEsfera = 48;
constexpr unsigned int pilhasEsfera = 24;

struct Vertice {
    glm::vec3 posicao;
    glm::vec3 normal;
};

struct InstanciaCorpo {
    glm::mat4 modelo;
    glm::vec4 cor;
};

static_assert(std::is_standard_layout<InstanciaCorpo>::value,
              "Os dados de instancia precisam ter layout padrao.");

GLuint compilarShader(GLenum tipo, const char* codigo) {
    const GLuint shader = glCreateShader(tipo);
    glShaderSource(shader, 1, &codigo, nullptr);
    glCompileShader(shader);

    GLint compilado = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compilado);
    if (compilado == GL_TRUE) {
        return shader;
    }

    GLint tamanhoLog = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &tamanhoLog);
    std::string log(static_cast<std::size_t>(tamanhoLog), '\0');
    glGetShaderInfoLog(shader, tamanhoLog, nullptr, log.data());
    glDeleteShader(shader);
    throw std::runtime_error("Falha ao compilar shader: " + log);
}

GLuint criarProgramaShader() {
    constexpr char vertexShader[] = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in mat4 instanceModel;
        layout (location = 6) in vec4 instanceColor;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        uniform vec3 objectColor;
        uniform bool isInstanced;

        out vec3 Normal;
        out vec3 FragPos;
        out vec3 VertexColor;
        out float Emissive;

        void main() {
            mat4 activeModel = isInstanced ? instanceModel : model;
            Normal = mat3(transpose(inverse(activeModel))) * aNormal;
            vec4 worldPosition = activeModel * vec4(aPos, 1.0);
            FragPos = worldPosition.xyz;
            VertexColor = isInstanced ? instanceColor.rgb : objectColor;
            Emissive = isInstanced ? instanceColor.a : 0.0;
            gl_Position = projection * view * worldPosition;
        }
    )";

    constexpr char fragmentShader[] = R"(
        #version 330 core
        in vec3 Normal;
        in vec3 FragPos;
        in vec3 VertexColor;
        in float Emissive;
        out vec4 FragColor;

        uniform vec3 sunPosition;
        uniform bool isOrbitTrail;

        void main() {
            if (isOrbitTrail || Emissive > 0.5) {
                FragColor = vec4(VertexColor, 1.0);
                return;
            }

            vec3 lightDir = normalize(sunPosition - FragPos);
            float diffuse = max(dot(normalize(Normal), lightDir), 0.0);
            FragColor = vec4(VertexColor * (0.08 + diffuse), 1.0);
        }
    )";

    const GLuint vertex = compilarShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fragment = 0;
    GLuint programa = 0;
    try {
        fragment = compilarShader(GL_FRAGMENT_SHADER, fragmentShader);
        programa = glCreateProgram();
        glAttachShader(programa, vertex);
        glAttachShader(programa, fragment);
        glLinkProgram(programa);

        GLint vinculado = GL_FALSE;
        glGetProgramiv(programa, GL_LINK_STATUS, &vinculado);
        if (vinculado != GL_TRUE) {
            GLint tamanhoLog = 0;
            glGetProgramiv(programa, GL_INFO_LOG_LENGTH, &tamanhoLog);
            std::string log(static_cast<std::size_t>(tamanhoLog), '\0');
            glGetProgramInfoLog(programa, tamanhoLog, nullptr, log.data());
            throw std::runtime_error("Falha ao vincular programa de shaders: " + log);
        }
    } catch (...) {
        if (programa != 0) {
            glDeleteProgram(programa);
        }
        if (fragment != 0) {
            glDeleteShader(fragment);
        }
        glDeleteShader(vertex);
        throw;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return programa;
}

} // namespace

Renderizador::Renderizador(std::size_t maximoPontosTrilha)
    : maximoPontosTrilha(maximoPontosTrilha) {
    if (maximoPontosTrilha == 0) {
        throw std::invalid_argument("A capacidade das trilhas deve ser positiva.");
    }

    try {
        programa = criarProgramaShader();
        criarEsfera();
    } catch (...) {
        liberarRecursos();
        throw;
    }

    localModel = glGetUniformLocation(programa, "model");
    localView = glGetUniformLocation(programa, "view");
    localProjection = glGetUniformLocation(programa, "projection");
    localCor = glGetUniformLocation(programa, "objectColor");
    localPosicaoSol = glGetUniformLocation(programa, "sunPosition");
    localEhTrilha = glGetUniformLocation(programa, "isOrbitTrail");
    localEhInstanciado = glGetUniformLocation(programa, "isInstanced");
}

Renderizador::~Renderizador() {
    liberarRecursos();
}

void Renderizador::liberarRecursos() noexcept {
    for (const Trilha& trilha : trilhas) {
        if (trilha.vbo != 0) {
            glDeleteBuffers(1, &trilha.vbo);
        }
        if (trilha.vao != 0) {
            glDeleteVertexArrays(1, &trilha.vao);
        }
    }
    if (esferaEbo != 0) {
        glDeleteBuffers(1, &esferaEbo);
    }
    if (bufferInstancias != 0) {
        glDeleteBuffers(1, &bufferInstancias);
        bufferInstancias = 0;
    }
    if (esferaVbo != 0) {
        glDeleteBuffers(1, &esferaVbo);
    }
    if (esferaVao != 0) {
        glDeleteVertexArrays(1, &esferaVao);
    }
    if (programa != 0) {
        glDeleteProgram(programa);
        programa = 0;
    }
}

void Renderizador::criarEsfera() {
    std::vector<Vertice> vertices;
    std::vector<GLuint> indices;
    vertices.reserve((setoresEsfera + 1) * (pilhasEsfera + 1));
    indices.reserve(setoresEsfera * pilhasEsfera * 6);

    for (unsigned int i = 0; i <= pilhasEsfera; ++i) {
        const float anguloPilha = pi / 2.0f -
            static_cast<float>(i) * pi / static_cast<float>(pilhasEsfera);
        const float xy = std::cos(anguloPilha);
        const float z = std::sin(anguloPilha);

        for (unsigned int j = 0; j <= setoresEsfera; ++j) {
            const float anguloSetor =
                static_cast<float>(j) * 2.0f * pi / static_cast<float>(setoresEsfera);
            const glm::vec3 posicao(
                xy * std::cos(anguloSetor),
                xy * std::sin(anguloSetor),
                z);
            vertices.push_back({posicao, posicao});
        }
    }

    for (unsigned int i = 0; i < pilhasEsfera; ++i) {
        unsigned int k1 = i * (setoresEsfera + 1);
        unsigned int k2 = k1 + setoresEsfera + 1;
        for (unsigned int j = 0; j < setoresEsfera; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.insert(indices.end(), {k1, k2, k1 + 1});
            }
            if (i != pilhasEsfera - 1) {
                indices.insert(indices.end(), {k1 + 1, k2, k2 + 1});
            }
        }
    }

    quantidadeIndicesEsfera = static_cast<GLsizei>(indices.size());
    glGenVertexArrays(1, &esferaVao);
    glGenBuffers(1, &esferaVbo);
    glGenBuffers(1, &esferaEbo);
    glBindVertexArray(esferaVao);

    glBindBuffer(GL_ARRAY_BUFFER, esferaVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertice)),
        vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, esferaEbo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)),
        indices.data(),
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice),
        reinterpret_cast<void*>(offsetof(Vertice, posicao)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice),
        reinterpret_cast<void*>(offsetof(Vertice, normal)));

    configurarAtributosInstancias();
    glBindVertexArray(0);
}

void Renderizador::configurarAtributosInstancias() {
    glGenBuffers(1, &bufferInstancias);
    glBindBuffer(GL_ARRAY_BUFFER, bufferInstancias);

    constexpr GLsizei stride = sizeof(InstanciaCorpo);
    for (GLuint coluna = 0; coluna < 4; ++coluna) {
        const GLuint local = 2 + coluna;
        const std::size_t deslocamento =
            offsetof(InstanciaCorpo, modelo) + coluna * sizeof(glm::vec4);
        glEnableVertexAttribArray(local);
        glVertexAttribPointer(
            local, 4, GL_FLOAT, GL_FALSE, stride,
            reinterpret_cast<void*>(deslocamento));
        glVertexAttribDivisor(local, 1);
    }

    constexpr GLuint atributoCor = 6;
    glEnableVertexAttribArray(atributoCor);
    glVertexAttribPointer(
        atributoCor, 4, GL_FLOAT, GL_FALSE, stride,
        reinterpret_cast<void*>(offsetof(InstanciaCorpo, cor)));
    glVertexAttribDivisor(atributoCor, 1);
}

void Renderizador::criarTrilhas(std::size_t quantidadeCorpos) {
    trilhas.resize(quantidadeCorpos);
    for (std::size_t i = 1; i < quantidadeCorpos; ++i) {
        Trilha& trilha = trilhas[i];
        glGenVertexArrays(1, &trilha.vao);
        glGenBuffers(1, &trilha.vbo);
        glBindVertexArray(trilha.vao);
        glBindBuffer(GL_ARRAY_BUFFER, trilha.vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(maximoPontosTrilha * sizeof(glm::vec3)),
            nullptr,
            GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    }
    glBindVertexArray(0);
}

void Renderizador::atualizarTrilhas(const std::vector<CorpoCeleste>& corpos) {
    if (!trilhasInicializadas) {
        criarTrilhas(corpos.size());
        for (std::size_t i = 1; i < corpos.size(); ++i) {
            trilhas[i].cor = corpos[i].cor * 0.75f;
            trilhas[i].pontos.push_back(glm::vec3(corpos[i].posicao));
        }
        trilhasInicializadas = true;
    }
    if (trilhas.size() != corpos.size()) {
        throw std::logic_error("A quantidade de corpos mudou durante a renderizacao.");
    }

    for (std::size_t i = 1; i < corpos.size(); ++i) {
        Trilha& trilha = trilhas[i];
        trilha.pontos.push_back(glm::vec3(corpos[i].posicao));
        if (trilha.pontos.size() > maximoPontosTrilha) {
            trilha.pontos.erase(trilha.pontos.begin());
        }

        glBindBuffer(GL_ARRAY_BUFFER, trilha.vbo);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            static_cast<GLsizeiptr>(trilha.pontos.size() * sizeof(glm::vec3)),
            trilha.pontos.data());
    }
}

void Renderizador::desenharTrilhas() {
    glUniform1i(localEhTrilha, GL_TRUE);
    glUniform1i(localEhInstanciado, GL_FALSE);
    glUniformMatrix4fv(localModel, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    for (std::size_t i = 1; i < trilhas.size(); ++i) {
        const Trilha& trilha = trilhas[i];
        glUniform3fv(localCor, 1, glm::value_ptr(trilha.cor));
        glBindVertexArray(trilha.vao);
        glDrawArrays(
            GL_LINE_STRIP, 0, static_cast<GLsizei>(trilha.pontos.size()));
    }
    glUniform1i(localEhTrilha, GL_FALSE);
}

void Renderizador::desenharCorpos(const std::vector<CorpoCeleste>& corpos) {
    if (corpos.empty()) {
        return;
    }

    glUniform3fv(
        localPosicaoSol, 1,
        glm::value_ptr(glm::vec3(corpos.front().posicao)));

    std::vector<InstanciaCorpo> instancias;
    instancias.reserve(corpos.size());
    for (std::size_t i = 0; i < corpos.size(); ++i) {
        const CorpoCeleste& corpo = corpos[i];
        glm::mat4 modelo(1.0f);
        modelo = glm::translate(modelo, glm::vec3(corpo.posicao));
        modelo = glm::scale(modelo, glm::vec3(static_cast<float>(corpo.raio)));
        instancias.push_back({
            modelo,
            glm::vec4(corpo.cor, i == 0 ? 1.0f : 0.0f)
        });
    }

    glBindBuffer(GL_ARRAY_BUFFER, bufferInstancias);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(instancias.size() * sizeof(InstanciaCorpo)),
        instancias.data(),
        GL_STREAM_DRAW);
    glUniform1i(localEhInstanciado, GL_TRUE);
    glBindVertexArray(esferaVao);
    glDrawElementsInstanced(
        GL_TRIANGLES,
        quantidadeIndicesEsfera,
        GL_UNSIGNED_INT,
        nullptr,
        static_cast<GLsizei>(instancias.size()));
    glUniform1i(localEhInstanciado, GL_FALSE);
}

void Renderizador::renderizar(
    const std::vector<CorpoCeleste>& corpos,
    const glm::mat4& view,
    const glm::mat4& projection) {
    atualizarTrilhas(corpos);
    glUseProgram(programa);
    glUniformMatrix4fv(localView, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(
        localProjection, 1, GL_FALSE, glm::value_ptr(projection));

    desenharTrilhas();
    desenharCorpos(corpos);
    glBindVertexArray(0);
}
