#include "spinningTop_Window.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <format>
#include <iostream>

/* static */ const char* spinningTop_Window::c_angleTypes[] = {
	"deg",
	"rad"
};

/* static */ const char* spinningTop_Window::c_trajDrawModes[] = {
	"Points",
	"Lines"
};

/* virtual */ void spinningTop_Window::RunInit() /* override */
{
	glfwSwapInterval(0);
	GLFW_SetUpCallbacks();

	m_app = std::make_unique<spinningTop_App>();
}

/* virtual */ void spinningTop_Window::RunRenderTick() /* override */
{
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GUI_Start();

}
	
/* virtual */ void spinningTop_Window::RunClear() /* override */
{
	m_app.reset();
}

void spinningTop_Window::GLFW_SetUpCallbacks()
{
	glfwSetFramebufferSizeCallback(m_window, &spinningTop_Window::GLFW_Callback_FramebufferSize);
	glfwSetCursorPosCallback(m_window, &spinningTop_Window::GLFW_Callback_CursorPos);
	glfwSetMouseButtonCallback(m_window, &spinningTop_Window::GLFW_Callback_MouseButton);
}

/* static */ spinningTop_Window* spinningTop_Window::GLFW_GetWindow(GLFWwindow* window)
{
	return reinterpret_cast<spinningTop_Window*>(glfwGetWindowUserPointer(window));
}

/* static */ void spinningTop_Window::GLFW_Callback_FramebufferSize(GLFWwindow* window, int width, int height)
{
	spinningTop_Window* w = GLFW_GetWindow(window);
	w->m_width = width;
	w->m_height = height;
	w->b_dockingInitialized = false; 
	
	glViewport(0, 0, width, height);
}

/* static */ void spinningTop_Window::GLFW_Callback_CursorPos(GLFWwindow* window, double xpos, double ypos)
{
	spinningTop_Window* w = GLFW_GetWindow(window);
	
	bool leftClick = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
	bool rightClick = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
	
	if (w->m_renderInterationState == RenderWindowInteration::None)
	{
		ImGuiIO& io = ImGui::GetIO();
    	io.AddMousePosEvent(xpos, ypos);
		return;
	}

	float deltaX = xpos - w->m_lastMousePos.x;
	float deltaY = ypos - w->m_lastMousePos.y;
	w->m_lastMousePos.x = xpos;
	w->m_lastMousePos.y = ypos;

	float speed = w->m_cameraSpeed;
	switch (w->m_renderInterationState)
	{
	case RenderWindowInteration::Camera_Rotation:
		w->m_app->UpdateCameraRotation(-deltaX * speed, -deltaY * speed);
		break;
	
	case RenderWindowInteration::Camera_Zoom:
		w->m_app->UpdateCameraPosition(deltaY * speed);
		break;
	}
}

/* static */ void spinningTop_Window::GLFW_Callback_MouseButton(GLFWwindow* window, int button, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, action == GLFW_PRESS);

	spinningTop_Window* w = GLFW_GetWindow(window);
	if (!w->b_overRenderWindow && w->m_renderInterationState == RenderWindowInteration::None)
		return;

	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		GLFW_Callback_MouseButton_Left(w, action, mods);
		break;

	case GLFW_MOUSE_BUTTON_RIGHT:
		GLFW_Callback_MouseButton_Right(w, action, mods);
		break;
	}

}	

/* static */ void spinningTop_Window::GLFW_Callback_MouseButton_Left(spinningTop_Window* w, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(w->m_window, &xpos, &ypos);

		w->m_lastMousePos = {xpos, ypos}; 
		w->m_renderInterationState = RenderWindowInteration::Camera_Rotation;
	}
	else if (action == GLFW_RELEASE)
	{
		w->m_renderInterationState = RenderWindowInteration::None;
	}
}

/* static */ void spinningTop_Window::GLFW_Callback_MouseButton_Right(spinningTop_Window* w, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(w->m_window, &xpos, &ypos);

		w->m_lastMousePos = {xpos, ypos}; 
		w->m_renderInterationState = RenderWindowInteration::Camera_Zoom;
	}
	else if (action == GLFW_RELEASE)
	{
		w->m_renderInterationState = RenderWindowInteration::None;
	}
}


void spinningTop_Window::GUI_Start()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	GUI_Main();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void spinningTop_Window::GUI_Main()
{
	GUI_WindowLayout();

	// DEBUG ONLY !!!!!!!!!!!!
	static bool show_demo_window = false;
	if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
}

void spinningTop_Window::GUI_WindowLayout()
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
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	if (ImGui::Begin(c_windowNameSettings.data())) 
	{
		GUI_WindowSettings();
	}
	ImGui::End();

	// Render Window
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin(c_windowNameRender.data())) 
	{
		GUI_WindowRender();
	}
	ImGui::End();
	ImGui::PopStyleVar(2);

	// Update Docking if necessary.
	if (!b_dockingInitialized)
	{
		GUI_UpdateDockingLayout();
	}
}

void spinningTop_Window::GUI_WindowSettings()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
	
	GUI_SEC_SimulationActions();
	GUI_SEC_SimulationOptions();
	GUI_SEC_DrawOptions();
	GUI_SEC_MiscellaneousInfo();
	
	ImGui::PopStyleVar(1);
}

void spinningTop_Window::GUI_SEC_DrawOptions()
{
	std::shared_ptr<drawParameters> drawParams = m_app->GetDrawParameters();

	ImGui::SeparatorText("Draw Options");
	GUI_ELEM_DrawCheckbox("Cube", drawParams->m_colorCube, drawParams->b_drawCube);
	GUI_ELEM_DrawCheckbox("Cube's diagonal", drawParams->m_colorDiagonal, drawParams->b_drawDiagonal);
	GUI_ELEM_DrawCheckbox("Gravitation", drawParams->m_colorGravitation, drawParams->b_drawGravitation);
	GUI_ELEM_DrawCheckbox("Corner's trajectory", drawParams->m_colorTrajectory, drawParams->b_drawTrajectory);

	float itemWidth = ImGui::GetWindowWidth() * 0.2f;
	
	m_selectedTrajDrawMode = static_cast<int>(drawParams->b_drawTrajectoryAsLine);
	ImGui::SetNextItemWidth(itemWidth);
	if (ImGui::BeginCombo("##Draw mode", c_trajDrawModes[m_selectedTrajDrawMode])) 
	{
		for (int n = 0; n < 2; n++)
		{
			bool isSelected = (m_selectedTrajDrawMode == n);
			if (ImGui::Selectable(c_trajDrawModes[n], isSelected))
			{
				m_selectedTrajDrawMode = n;
				drawParams->b_drawTrajectoryAsLine = (m_selectedTrajDrawMode != 0);
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::SameLine();

	ImGui::SetNextItemWidth(itemWidth);
	ImGui::DragInt("Trajectory's draw mode / length", &drawParams->m_trajectoryPointsNum, 20, 200, 100000, "%d", ImGuiSliderFlags_AlwaysClamp);
	if (b_TrajectoryNumberChanging == true && ImGui::IsItemActive() == false)
	{
		m_app->SetNewTrajectoryBufferSize(drawParams->m_trajectoryPointsNum);
	}
	b_TrajectoryNumberChanging = ImGui::IsItemActive();
}

void spinningTop_Window::GUI_SEC_SimulationOptions()
{
	std::shared_ptr<simulationParameters> simulationParams = m_app->GetSimulationParameters();
	bool simulationStarted = m_app->IsRunning() || m_app->IsStopped();

	ImGui::SeparatorText("Simulation Options");	

	ImGui::BeginDisabled(simulationStarted);

	float win_width = ImGui::GetWindowWidth();
	float padding = ImGui::GetStyle().WindowPadding.x;
	float spacing = ImGui::GetStyle().ItemSpacing.x;
	float buttonWidth = (win_width - 2 * padding - 2 * spacing) / 3.0f;
	ImVec2 buttonSize{buttonWidth, 0.0f};

	ImGuiStyle previousStyle = ImGui::GetStyle();
	ImGui::GetStyle().FramePadding = ImVec2(4.0f, 6.0f);

	if (ImGui::Button("RESET OPTIONS", buttonSize))
	{
		m_app->ResetPreset();
	}

	ImGui::SameLine();
	
	std::vector<std::string> presetsNames = m_app->GetPresets();
	int presetID = m_app->GetPresetID();
	std::string preview = " PRESET:  " + presetsNames[presetID];

	ImGui::SetNextItemWidth(2.0f * buttonWidth + spacing);
	if (ImGui::BeginCombo("##Preset", preview.data(), ImGuiComboFlags_HeightLargest))
	{
		for (int n = 0; n < presetsNames.size(); n++)
		{
			bool isSelected = (presetID == n);
			if (ImGui::Selectable(presetsNames[n].data(), isSelected)) 
			{
				m_app->SetPresetID(n);
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::GetStyle() = previousStyle;

	ImGui::Spacing();

	float itemWidth = win_width * 0.6f;
	
	ImGui::SetNextItemWidth(itemWidth);
	ImGui::DragFloat("Cube's edge length", &simulationParams->m_cubeEdgeLength, 0.01f, 0.01f, FLT_MAX, "%.2f m", ImGuiSliderFlags_AlwaysClamp);
	ImGui::SetNextItemWidth(itemWidth);
	ImGui::DragFloat("Cube's density", &simulationParams->m_cubeDensity, 0.01f, 0.01f, FLT_MAX, "%.2f kg/m^3", ImGuiSliderFlags_AlwaysClamp);
	
	float currentTilt = glm::degrees(simulationParams->m_cubeTilt);
	ImGui::SetNextItemWidth(itemWidth);
	if (ImGui::DragFloat("Cube's tilt", &currentTilt, 0.6f, 0.0f, 180.0f, "%.2f deg", ImGuiSliderFlags_AlwaysClamp))
	{
		simulationParams->m_cubeTilt = glm::radians(currentTilt);
	}

	ImGui::SetNextItemWidth(itemWidth * 0.8f - spacing);
	if (m_selectedAngVelAngleType == 0)
	{
		float currentVelocity = glm::degrees(simulationParams->m_cubeAngularVelocity);
		if (ImGui::DragFloat("##Angular velocity", &currentVelocity, 5.0f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
		{
			simulationParams->m_cubeAngularVelocity = glm::radians(currentVelocity);
		}
	}
	else if (m_selectedAngVelAngleType == 1) 
	{
		float radPerPi = simulationParams->m_cubeAngularVelocity / glm::pi<float>();
		std::string format = "%.2f  " + std::format("({0:.2f} * pi)", radPerPi);

		ImGui::DragFloat("##Angular velocity", &simulationParams->m_cubeAngularVelocity, 0.1f, 0.0f, FLT_MAX, format.data(), ImGuiSliderFlags_AlwaysClamp);
	}
	
	ImGui::SameLine();
	ImGui::SetNextItemWidth(itemWidth * 0.2f);
	if (ImGui::BeginCombo("##Angular velocity Combo", c_angleTypes[m_selectedAngVelAngleType]))
	{
		for (int n = 0; n < 2; n++)
		{
			bool isSelected = (m_selectedAngVelAngleType == n);
			if (ImGui::Selectable(c_angleTypes[n], isSelected)) 
				m_selectedAngVelAngleType = n;

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	
	ImGui::SameLine(itemWidth + 1.5f * spacing);
	ImGui::Text("Cube's angular velocity");

	ImGui::SetNextItemWidth(itemWidth);
	ImGui::DragFloat("simulation step", &simulationParams->m_delta, 0.001f, 0.01f, 0.1f, "%.4f");
	
	ImGui::EndDisabled();

	if (ImGui::Checkbox("Apply Gravity Force", &simulationParams->b_Gravity)) 
	{
		m_app->ApplyForce();
	}
}

void spinningTop_Window::GUI_SEC_SimulationActions()
{
	ImGui::SeparatorText("Actions");

	float win_width = ImGui::GetWindowWidth();
	float padding = ImGui::GetStyle().WindowPadding.x;
	float spacing = ImGui::GetStyle().ItemSpacing.x;
	float buttonWidth = (win_width - 2 * padding - 2 * spacing) / 3.0f;
	float sliderWidth = 2 * buttonWidth + spacing; 
	ImVec2 buttonSize{buttonWidth, 40.0f};	

	bool isSimulationRun = m_app->IsRunning();
	bool isSimulationPaused = m_app->IsStopped();

	ImGui::BeginDisabled(isSimulationRun);
	if (ImGui::Button(isSimulationPaused ? "CONTINUE" : "START", buttonSize))
	{
		m_app->StartSimulation();
	}
	ImGui::EndDisabled();

	ImGui::BeginDisabled(!isSimulationRun);
	ImGui::SameLine();
	if (ImGui::Button("STOP", buttonSize))
	{
		m_app->StopSimulation();
	}
	ImGui::EndDisabled();

	ImGui::BeginDisabled(!isSimulationPaused);
	ImGui::SameLine();
	if(ImGui::Button("RESET", buttonSize))
	{
		m_app->ResetSimulation();
	}
	ImGui::EndDisabled();	
}

void spinningTop_Window::GUI_SEC_MiscellaneousInfo()
{
	ImGui::SeparatorText("Miscellaneous Information");

	float win_width = ImGui::GetWindowWidth();
	float padding = ImGui::GetStyle().WindowPadding.x;
	float counterWidth = 80.0f + padding;
	
	ImGui::BeginDisabled(!b_limitFPS);

	int framerate = static_cast<int>(m_framerate);
	ImGui::SetNextItemWidth(win_width * 0.5f);
	if (ImGui::InputInt("##FPS Limit Input", &framerate, 15, 50) )
	{
		if (framerate <= 30) 
			framerate = 30;
		
		m_framerate = static_cast<double>(framerate);
	}

	ImGui::EndDisabled();

	ImGui::SameLine();	
	ImGui::Checkbox("Limit FPS", &b_limitFPS);
	ImGui::SameLine(win_width * 0.8f);
	
	ImGui::TextColored(b_limitFPS ? ImGui::GetStyle().Colors[ImGuiCol_Text] : ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 
		"FPS: %6.2f", 1.0f / m_deltaTime);
}

void spinningTop_Window::GUI_WindowRender()
{
	b_overRenderWindow = ImGui::IsWindowHovered();
	GUI_UpdateRenderRegion();

	m_app->RenderScene();

	GLuint tex = m_app->GetRenderTexture();
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddImage(
			tex, 
			pos, 
			ImVec2(pos.x + m_lastRenderRegion.x, pos.y + m_lastRenderRegion.y), 
			ImVec2(0, 1), 
			ImVec2(1, 0)
		);
}

void spinningTop_Window::GUI_ELEM_DrawCheckbox(std::string name, glm::vec4& color, bool& draw)
{	
	ImGui::ColorEdit4(("##" + name).data(), reinterpret_cast<float*>(&color), 
		ImGuiColorEditFlags_AlphaBar | 
		ImGuiColorEditFlags_NoInputs | 
		ImGuiColorEditFlags_NoBorder);
	
	ImGui::SameLine();
	ImGui::Checkbox(name.data(), &draw);
}


void spinningTop_Window::GUI_UpdateDockingLayout()
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

void spinningTop_Window::GUI_UpdateRenderRegion()
{
	ImVec2 currentRenderRegion = ImGui::GetContentRegionAvail();
	if (currentRenderRegion.x != m_lastRenderRegion.x || 
		currentRenderRegion.y != m_lastRenderRegion.y)
	{
		m_lastRenderRegion = currentRenderRegion;
		int width = static_cast<int>(currentRenderRegion.x);
		int height = static_cast<int>(currentRenderRegion.y);

		m_app->SetRenderArea(width, height);
	}
}