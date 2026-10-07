#include "Camera.hpp"
#include <cmath>
#include <algorithm>

Camera::Camera(glm::vec3 alvo, float distancia, float yaw, float pitch)
    : alvo(alvo),
      yaw(yaw),
      sensibilidade(0.2f),
      sensibilidadeZoom(0.12f),
      distanciaMinima(4.0f),
      distanciaMaxima(500.0f),
      pitchMinimo(-85.0f),
      pitchMaximo(85.0f)
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
    yaw = std::fmod(yaw + deltaX * sensibilidade, 360.0f);
    pitch = std::clamp(pitch - deltaY * sensibilidade, pitchMinimo, pitchMaximo);
}

void Camera::processarPanMouse(float deltaX, float deltaY, int alturaViewport) {
    if (alturaViewport <= 0) {
        return;
    }

    const glm::vec3 direcao = glm::normalize(alvo - getPosicao());
    const glm::vec3 direita =
        glm::normalize(glm::cross(direcao, glm::vec3(0.0f, 1.0f, 0.0f)));
    const glm::vec3 cima = glm::normalize(glm::cross(direita, direcao));
    const float unidadesPorPixel =
        2.0f * distancia * std::tan(glm::radians(22.5f)) /
        static_cast<float>(alturaViewport);

    alvo += unidadesPorPixel * (-deltaX * direita + deltaY * cima);
}

void Camera::processarZoom(float offset) {
    const float escala = std::exp(-offset * sensibilidadeZoom);
    setDistancia(distancia * escala);
}

void Camera::setSensibilidade(float sens) {
    sensibilidade = sens;
}

void Camera::setSensibilidadeZoom(float sensZoom) {
    sensibilidadeZoom = sensZoom;
}
