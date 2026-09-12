#ifndef PLANETA_H
#define PLANETA_H

#include <string>

class Planeta
{
public:
    Planeta(const std::string& nome, double raio, double distancia, double velocidade); //passando a string por referência para evitar cópia desnecessária, os outros parâmetros são passados por valor, pois são tipos primitivos
    std::string getNome() const;
    double getRaio() const;
    double getDistancia() const;
    double getVelocidade() const;
private:
    std::string nome;
    double raio;
    double distancia;
    double velocidade;
};
#endif // PLANETA_H