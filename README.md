- Simulador do Sistema Solar -

Para compilar e executar este projeto, você precisará das seguintes bibliotecas:

    C++11 (ou superior)

    OpenGL 3.3+

    GLFW: Gerenciamento de janelas e entrada de dados.

    GLAD: Carregamento dos ponteiros de função do OpenGL.

    GLM: Biblioteca matemática para cálculos de matrizes e vetores (Mat4, Vec3).

Conceitos de Computação Gráfica Aplicados

    Pipeline Gráfico: Uso de Vertex e Fragment Shaders para projetar coordenadas 3D em uma tela 2D e aplicar cores sólidas com sombreamento difuso básico.

    Transformações (MVP): Cálculo de matrizes locais. A matriz Model translada e escala cada planeta baseando-se em sua distância da origem e raio geométrico.

    Draw Elements: Renderização indexada via glDrawElements e EBO (Element Buffer Object) para economizar memória reutilizando vértices compartilhados entre os triângulos adjacentes da esfera.

Simulação Física

    A gravitação newtoniana é integrada por RK4 sobre o estado completo (posição
    e velocidade de todos os corpos). Cada estágio calcula as acelerações usando
    simultaneamente as posições daquele estágio, incluindo todos os pares de corpos
    e o parâmetro de suavização epsilon.

    O estado físico usa vetores e massas em precisão dupla. O exemplo usa unidades
    normalizadas, com G = 1e-4 e epsilon = 1e-3; ambos podem ser ajustados no
    construtor de Fisica. A animação avança o tempo simulado 100 vezes mais rápido
    que o relógio, usando subpassos RK4 de no máximo 0,05 s para manter a precisão.

    Os planetas começam com velocidades tangenciais calculadas para órbitas
    circulares aproximadas em torno do Sol. O recuo do Sol é incluído para que o
    momento linear inicial total seja zero. As massas planetárias usam a mesma
    escala relativa à massa solar; as distâncias continuam comprimidas para a
    visualização, portanto as órbitas são uma aproximação e não uma efeméride real.

Visualização

    A câmera inicia centralizada no conjunto do sistema solar. Cada planeta deixa
    uma trilha colorida limitada aos seus 2.400 últimos pontos, facilitando
    acompanhar o sentido e o percurso do movimento. Arraste com o botão esquerdo
    para orbitar, com o botão direito para deslocar o enquadramento e use a roda
    do mouse para ajustar o zoom proporcional à distância.

<img width="1276" height="749" alt="image" src="https://github.com/user-attachments/assets/dce9d709-243a-4c1a-91e4-9a123aab46fc" />
