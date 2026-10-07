# Simulador do Sistema Solar

Simulador visual 3D de um sistema gravitacional N-corpos, implementado em C++17,
OpenGL 3.3, GLFW e GLM. A integracao numerica usa RK4 em precisao dupla.

## Requisitos

- CMake 3.20 ou superior
- Compilador C++17
- Python com o executavel `glad` (GLAD 2)
- Bibliotecas de desenvolvimento do OpenGL
- Conexao com a internet na primeira configuracao do CMake (GLFW e GLM sao
  obtidos via `FetchContent`)

Instale o gerador GLAD no ambiente virtual do projeto, por exemplo:

```sh
python -m venv .venv
. .venv/bin/activate
python -m pip install glad2
```

No Windows, ative `.venv\Scripts\activate` em vez do comando de ativacao Unix.

## Compilar e executar

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/SolarSystem
```

No Windows, execute `build\SolarSystem.exe`.

## Testes

Os testes unitarios da fisica nao precisam criar uma janela OpenGL:

```sh
ctest --test-dir build --output-on-failure
```

## Controles

- Botao esquerdo + arrastar: orbitar a camera.
- Botao direito + arrastar: deslocar o enquadramento.
- Roda do mouse: aproximar ou afastar a camera.

## Organizacao

- `include/` contem as interfaces publicas dos componentes.
- `src/Aplicacao.cpp` coordena janela, entrada, simulacao e renderizacao.
- `src/Janela.cpp` encapsula o ciclo de vida GLFW/OpenGL.
- `src/Renderizador.cpp` possui shaders, malha compartilhada de esfera e trilhas.
- O renderizador envia os atributos de todos os corpos em um buffer de instancias
  e desenha as esferas em uma chamada `glDrawElementsInstanced`.
- `src/Camera.cpp` implementa os controles e matrizes da camera.
- `include/CorpoCeleste.hpp` define os dados dos corpos sem dependencia de OpenGL.
- `src/SistemaSolar.cpp` cria o sistema inicial e velocidades orbitais.
- `src/Fisica.cpp` integra simultaneamente o estado de todos os corpos por RK4.
- `tests/` valida propriedades basicas do integrador.

## Modelo fisico

A gravidade newtoniana com suavizacao `epsilon` e calculada entre todos os pares
de corpos. Os quatro estagios do RK4 avaliam o estado completo do sistema, e os
novos estados so sao aplicados depois de calculada a combinacao final.
Essa parte permanece na CPU, enquanto geometria, transformacoes por instancia,
iluminacao, trilhas e rasterizacao sao processadas pela GPU via OpenGL.

O exemplo usa unidades normalizadas (`G = 1e-4`, `epsilon = 1e-3`), velocidades
tangenciais iniciais para orbitas circulares aproximadas, e avanca o tempo
simulado 100 vezes mais rapido que o tempo real. A cada quadro, o delta e
subdividido em passos de integracao de no maximo `0.05`. Massas e distancias sao
escaladas para visualizacao, portanto o resultado nao e uma efemeride astronomica.
