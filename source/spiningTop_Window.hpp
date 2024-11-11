#pragma once

#include "glfwWindowWrap.hpp"
#include <imgui.h>
#include <string_view>

class spiningTop_Window: public glfwWindowWrap
{
private:

    /* virtual */ void RunInit() override;
    /* virtual */ void RunRenderTick() override;
    /* virtual */ void RunClear() override;

private: 

    void GLFW_SetUpCallbacks();
    static spiningTop_Window* GLFW_GetWindow(GLFWwindow* window);
    static void GLFW_Callback_FramebufferSize(GLFWwindow* window, int width, int height);

    void GUI_Start();
    void GUI_Main();
    void GUI_WindowLayout();
    // Respective function for windows;
    void GUI_WindowSettings();
    void GUI_WindowRender();

    void GUI_UpdateDockingLayout();
    

private:

    const std::string_view c_mainDockingSpaceName = "mainDockingSpace"; 
    const std::string_view c_windowNameMain = "Simulation window";
    const std::string_view c_windowNameRender = "Viewport";
    const std::string_view c_windowNameSettings = "Simulation Parameters";

    ImGuiID m_mainDockingSpace;
    bool    b_dockingInitialized = false;
    const float c_dockRatio = 0.3f;

};