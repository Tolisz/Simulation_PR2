#include "spiningTop_Window.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

/* virtual */ void spiningTop_Window::RunInit() /* override */
{
	GLFW_SetUpCallbacks();

	m_app = std::make_unique<springTop_App>();
}

/* virtual */ void spiningTop_Window::RunRenderTick() /* override */
{
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GUI_Start();

}

/* virtual */ void spiningTop_Window::RunClear() /* override */
{
	m_app.reset();
}

void spiningTop_Window::GLFW_SetUpCallbacks()
{
	glfwSetFramebufferSizeCallback(m_window, &spiningTop_Window::GLFW_Callback_FramebufferSize);
}

/* static */ spiningTop_Window* spiningTop_Window::GLFW_GetWindow(GLFWwindow* window)
{
	return reinterpret_cast<spiningTop_Window*>(glfwGetWindowUserPointer(window));
}

/* static */ void spiningTop_Window::GLFW_Callback_FramebufferSize(GLFWwindow* window, int width, int height)
{
	spiningTop_Window* w = GLFW_GetWindow(window);
	w->m_width = width;
	w->m_height = height;
	w->b_dockingInitialized = false; 
	
	glViewport(0, 0, width, height);
}

void spiningTop_Window::GUI_Start()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	GUI_Main();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void spiningTop_Window::GUI_Main()
{
	GUI_WindowLayout();

	// DEBUG ONLY !!!!!!!!!!!!
	static bool show_demo_window = true;
	if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
}

void spiningTop_Window::GUI_WindowLayout()
{
	// Main Window
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin(c_windowNameMain.data(), nullptr, window_flags);
	ImGui::PopStyleVar(3);

	m_mainDockingSpace = ImGui::GetID(c_mainDockingSpaceName.data());
	ImGui::DockSpace(m_mainDockingSpace, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	ImGui::End();

	// Settings Window
	if (ImGui::Begin(c_windowNameSettings.data())) 
	{
		GUI_WindowSettings();
	}
	ImGui::End();

	// Render Window
	if (ImGui::Begin(c_windowNameRender.data())) 
	{
		GUI_WindowRender();
	}
	ImGui::End();

	// Update Docking if necessary.
	if (!b_dockingInitialized)
	{
		GUI_UpdateDockingLayout();
	}
}

void spiningTop_Window::GUI_WindowSettings()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
	
	GUI_SEC_SimulationActions();
	GUI_SEC_SimulationOptions();
	GUI_SEC_DrawOptions();
	ImGui::SeparatorText("Miscellaneous Information");
	
	ImGui::PopStyleVar(1);
}

void spiningTop_Window::GUI_SEC_DrawOptions()
{
	std::shared_ptr<drawParameters> drawParams = m_app->GetDrawParameters();

	ImGui::SeparatorText("Draw Options");
	GUI_ELEM_DrawCheckbox("Cube", drawParams->m_colorCube, drawParams->b_drawCube);
	GUI_ELEM_DrawCheckbox("Cube's diagonal", drawParams->m_colorDiagonal, drawParams->b_drawDiagonal);
	GUI_ELEM_DrawCheckbox("Gravitation", drawParams->m_colorGravitation, drawParams->b_drawGravitation);
	GUI_ELEM_DrawCheckbox("Corner's trajectory", drawParams->m_colorTrajectory, drawParams->b_drawTrajectory);

	float itemWidth = ImGui::GetWindowWidth() * 0.4f;
	ImGui::SetNextItemWidth(itemWidth);
	ImGui::DragInt("Number of trajectorys' points", &drawParams->m_trajectoryPointsNum, 100, 100, 1000000, "%d", ImGuiSliderFlags_AlwaysClamp);
	if (b_TrajectoryNumberActivation == true && ImGui::IsItemActive() == false)
	{
		// std::cout << "Aktualizacja" << std::endl;
	}
	b_TrajectoryNumberActivation = ImGui::IsItemActive();
}

void spiningTop_Window::GUI_SEC_SimulationOptions()
{
	std::shared_ptr<simulationParameters> simulationParams = m_app->GetSimulationParameters();
	
	ImGui::SeparatorText("Simulation Options");	
	float itemWidth = ImGui::GetWindowWidth() * 0.6f;
	
	ImGui::SetNextItemWidth(itemWidth);
	ImGui::DragFloat("Cube's edge length", &simulationParams->m_cubeEdgeLength, 0.01f, 0.01f, FLT_MAX, "%.2f m", ImGuiSliderFlags_AlwaysClamp);
	ImGui::SetNextItemWidth(itemWidth);
	ImGui::DragFloat("Cube's density", &simulationParams->m_cubeDensity, 0.01f, 0.01f, FLT_MAX, "%.2f kg/m^3", ImGuiSliderFlags_AlwaysClamp);
	
	float currentTilt = glm::degrees(simulationParams->m_cubeTilt);
	ImGui::SetNextItemWidth(itemWidth);
	if (ImGui::DragFloat("Cube's tilt", &currentTilt, 0.2f, 0.0f, 90.0f, "%.2f deg", ImGuiSliderFlags_AlwaysClamp))
	{
		simulationParams->m_cubeTilt = glm::radians(currentTilt);
	}

	float currentVelocity = glm::degrees(simulationParams->m_cubeAngularVelocity);
	ImGui::SetNextItemWidth(itemWidth);
		if (ImGui::DragFloat("Cube's angular velocity", &currentVelocity, 0.5f, 0.0f, FLT_MAX, "%.2f deg", ImGuiSliderFlags_AlwaysClamp))
	{
		simulationParams->m_cubeAngularVelocity = glm::radians(currentVelocity);
	}

	ImGui::SetNextItemWidth(itemWidth);
	ImGui::DragFloat("simulation step", &simulationParams->m_delta, 0.001f, 0.01f, 0.1f, "%.4f");
}

void spiningTop_Window::GUI_SEC_SimulationActions()
{
	ImGui::SeparatorText("Actions");


}

void spiningTop_Window::GUI_WindowRender()
{
	ImGui::Text("RENDEROWANIE");
}

void spiningTop_Window::GUI_ELEM_DrawCheckbox(std::string name, glm::vec4& color, bool& draw)
{	
	ImGui::ColorEdit4(("##" + name).data(), reinterpret_cast<float*>(&color), 
		ImGuiColorEditFlags_AlphaBar | 
		ImGuiColorEditFlags_NoInputs | 
		ImGuiColorEditFlags_NoBorder);
	
	ImGui::SameLine();
	ImGui::Checkbox(name.data(), &draw);
}


void spiningTop_Window::GUI_UpdateDockingLayout()
{
	b_dockingInitialized = true;

	ImGui::DockBuilderRemoveNode(m_mainDockingSpace);
	ImGui::DockBuilderAddNode(m_mainDockingSpace, ImGuiDockNodeFlags_DockSpace);
	ImGui::DockBuilderSetNodeSize(m_mainDockingSpace, ImGui::GetMainViewport()->Size);

	ImGuiID dock_L, dock_R;
	ImGui::DockBuilderSplitNode(m_mainDockingSpace, ImGuiDir_Right, c_dockRatio, &dock_R, &dock_L);

	ImGui::DockBuilderDockWindow(c_windowNameSettings.data(), dock_R);
	ImGui::DockBuilderDockWindow(c_windowNameRender.data(), dock_L);

	ImGui::DockBuilderFinish(m_mainDockingSpace);
}