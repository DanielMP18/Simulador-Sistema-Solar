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


<img width="1276" height="749" alt="image" src="https://github.com/user-attachments/assets/dce9d709-243a-4c1a-91e4-9a123aab46fc" />
