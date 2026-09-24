#include "Camera.hpp"
#include <cmath>
#include <algorithm>

Camera::Camera(glm::vec3 alvo, float distancia, float yaw, float pitch)
    : alvo(alvo),
      yaw(yaw),
      sensibilidade(0.1f),
      sensibilidadeZoom(2.0f),
      distanciaMinima(2.0f),
      distanciaMaxima(300.0f),
      pitchMinimo(-89.0f),
      pitchMaximo(89.0f)
{
    this->distancia = std::clamp(distancia, distanciaMinima, distanciaMaxima);
    this->pitch = std::clamp(pitch, pitchMinimo, pitchMaximo);
}

glm::mat4 Camera::getMatrizView() const {
    return glm::lookAt(getPosicao(), alvo, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Camera::getPosicao() const {
    float radPitch = glm::radians(pitch);
    float radYaw = glm::radians(yaw);

    float x = alvo.x + distancia * std::cos(radPitch) * std::cos(radYaw);
    float y = alvo.y + distancia * std::sin(radPitch);
    float z = alvo.z + distancia * std::cos(radPitch) * std::sin(radYaw);

    return glm::vec3(x, y, z);
}

glm::vec3 Camera::getAlvo() const {
    return alvo;
}

float Camera::getDistancia() const {
    return distancia;
}

float Camera::getYaw() const {
    return yaw;
}

float Camera::getPitch() const {
    return pitch;
}

void Camera::setAlvo(const glm::vec3& novoAlvo) {
    alvo = novoAlvo;
}

void Camera::setDistancia(float novaDistancia) {
    distancia = std::clamp(novaDistancia, distanciaMinima, distanciaMaxima);
}

void Camera::processarMovimentoMouse(float deltaX, float deltaY) {
    yaw += deltaX * sensibilidade;
    pitch = std::clamp(pitch + deltaY * sensibilidade, pitchMinimo, pitchMaximo);
}

void Camera::processarZoom(float offset) {
    setDistancia(distancia - offset * sensibilidadeZoom);
}

void Camera::setSensibilidade(float sens) {
    sensibilidade = sens;
}

void Camera::setSensibilidadeZoom(float sensZoom) {
    sensibilidadeZoom = sensZoom;
}
