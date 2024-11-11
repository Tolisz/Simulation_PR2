#pragma once

#include "glfwWindowWrap.hpp"
#include "spinningTop_App.hpp"

#include <imgui.h>
#include <string>
#include <string_view>
#include <memory>

class spinningTop_Window: public glfwWindowWrap
{
private:

    /* virtual */ void RunInit() override;
    /* virtual */ void RunRenderTick() override;
    /* virtual */ void RunClear() override;

private: 

    void GLFW_SetUpCallbacks();
    static spinningTop_Window* GLFW_GetWindow(GLFWwindow* window);
    static void GLFW_Callback_FramebufferSize(GLFWwindow* window, int width, int height);

    void GUI_Start();
    void GUI_Main();
    void GUI_WindowLayout();
    
    // Respective function for windows;
    void GUI_WindowSettings();
    void GUI_SEC_DrawOptions();
    void GUI_SEC_SimulationOptions();
    void GUI_SEC_SimulationActions();
    void GUI_WindowRender();

    // UI Elements
    void GUI_ELEM_DrawCheckbox(std::string name, glm::vec4& color, bool& draw);

    // Docking
    void GUI_UpdateDockingLayout();
    

private:

    const std::string_view c_mainDockingSpaceName = "mainDockingSpace"; 
    const std::string_view c_windowNameMain = "Simulation window";
    const std::string_view c_windowNameRender = "Viewport";
    const std::string_view c_windowNameSettings = "Simulation Parameters";

    ImGuiID m_mainDockingSpace;
    bool    b_dockingInitialized = false;
    const float c_dockRatio = 0.3f;

    bool b_TrajectoryNumberActivation = false;

    std::unique_ptr<spinningTop_App> m_app;
};