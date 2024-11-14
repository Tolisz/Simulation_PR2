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
    static void GLFW_Callback_CursorPos(GLFWwindow* window, double xpos, double ypos);
    static void GLFW_Callback_MouseButton(GLFWwindow* window, int button, int action, int mods);
    static void GLFW_Callback_MouseButton_Left(spinningTop_Window* w, int action, int mods);
    static void GLFW_Callback_MouseButton_Right(spinningTop_Window* w, int action, int mods);


    void GUI_Start();
    void GUI_Main();
    void GUI_WindowLayout();
    
    // Respective function for windows;
    void GUI_WindowSettings();
    void GUI_SEC_DrawOptions();
    void GUI_SEC_SimulationOptions();
    void GUI_SEC_SimulationActions();
    void GUI_SEC_MiscellaneousInfo();
    void GUI_WindowRender();

    // UI Elements
    void GUI_ELEM_DrawCheckbox(std::string name, glm::vec4& color, bool& draw);

    // GUI miscellaneous
    void GUI_UpdateDockingLayout();
    void GUI_UpdateRenderRegion();
    

private:

    const std::string_view c_mainDockingSpaceName = "mainDockingSpace"; 
    const std::string_view c_windowNameMain = "Simulation window";
    const std::string_view c_windowNameRender = "Viewport";
    const std::string_view c_windowNameSettings = "Simulation Parameters";

    ImGuiID m_mainDockingSpace;
    const float c_dockRatio = 0.3f;
    
    bool b_dockingInitialized       = false;
    bool b_TrajectoryNumberChanging = false;
    bool b_overRenderWindow         = false;
    
    enum class RenderWindowInteration
    {
        None,
        Camera_Zoom,
        Camera_Rotation
    } 
    m_renderInterationState = RenderWindowInteration::None;

    ImVec2      m_lastRenderRegion  = {0, 0};
    glm::vec2   m_lastMousePos      = {0, 0};
    float       m_cameraSpeed       = 0.01f;
    
    static const char* c_angleTypes[];
    int m_selectedAngVelAngleType = 0;

    std::unique_ptr<spinningTop_App> m_app;
};