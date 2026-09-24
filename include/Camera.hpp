#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(glm::vec3 alvo = glm::vec3(0.0f, 0.0f, 0.0f),
           float distancia = 50.0f,
           float yaw = 90.0f,
           float pitch = 30.0f);

    glm::mat4 getMatrizView() const;
    glm::vec3 getPosicao() const;
    glm::vec3 getAlvo() const;
    float getDistancia() const;
    float getYaw() const;
    float getPitch() const;

    void setAlvo(const glm::vec3& novoAlvo);
    void setDistancia(float novaDistancia);

    void processarMovimentoMouse(float deltaX, float deltaY);
    void processarZoom(float offset);

    void setSensibilidade(float sens);
    void setSensibilidadeZoom(float sensZoom);

private:
    glm::vec3 alvo;
    float distancia;
    float yaw;
    float pitch;

    float sensibilidade;
    float sensibilidadeZoom;
    float distanciaMinima;
    float distanciaMaxima;
    float pitchMinimo;
    float pitchMaximo;
};

#endif // CAMERA_HPP
