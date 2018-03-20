#include "Precompiled.h"

GLFWwindow* window;
double x, y;

void InitializeWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

//    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
//    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
//    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
//    window = glfwCreateWindow(ENGINE_WIDTH, ENGINE_HEIGHT, "Engine", monitor, NULL);
    window = glfwCreateWindow(ENGINE_WIDTH, ENGINE_HEIGHT, "Engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glewExperimental = GL_TRUE;
    glewInit();
}
bool WindowShouldClose() {
    return glfwWindowShouldClose(window);
}
void SwapBuffers() {
    glfwSwapBuffers(window);
}


bool GetKey(int key) {
    return glfwGetKey(window, key);
}
bool GetMouseButton(int button) {
    return glfwGetMouseButton(window, button);
}
void GetCursorPos(double* xpos, double* ypos) {
    glfwGetCursorPos(window, xpos, ypos);
}

