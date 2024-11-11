#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class glfwWindowWrap
{
protected:

    GLFWwindow* m_window = nullptr;
    float m_deltaTime = 0.0f;

    int m_width = 0;
    int m_height = 0;

private:

    float m_currentTime = 0.0f;
    float m_lastTime = 0.0f;

    bool limitFPS = true;
    double fpsLimit = 1.0 / 60.0; // 60 frame per second;

public: 

    // *=*=*=*=*=*=*=*=*=*=
    //   Object Creation
    // *=*=*=*=*=*=*=*=*=*=

    glfwWindowWrap();
    ~glfwWindowWrap();

    glfwWindowWrap(const glfwWindowWrap&) = delete;
    glfwWindowWrap(glfwWindowWrap&&) = delete;
    glfwWindowWrap& operator=(const glfwWindowWrap&) = delete;
    glfwWindowWrap& operator=(glfwWindowWrap&&) = delete;

    // *=*=*=*=*=*=*=*=*=*=
    //      Methods
    // *=*=*=*=*=*=*=*=*=*=

    bool InitWindow(int width, int height, const char* title);
    GLFWwindow* GetWindowPointer();

    // *=*=*=*=*=*=*=*=*=*=
    //  Wrapper Interface
    // *=*=*=*=*=*=*=*=*=*=

    void RunApp();

private:

    virtual void RunInit() = 0;
    virtual void RunRenderTick() = 0;
    virtual void RunClear() = 0;

private:

    void UpdateDeltatime();
};