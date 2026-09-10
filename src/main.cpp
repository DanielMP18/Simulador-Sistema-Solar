#include <iostream>
#include <GLFW/glfw3.h>

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Erro ao inicializar GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(
        800,
        600,
        "Sistema Solar",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cerr << "Erro ao criar janela\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.04f, 0.04f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
