#include "spiningTop_Window.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

/* virtual */ void spiningTop_Window::RunInit() /* override */
{
	GLFW_SetUpCallbacks();
}

/* virtual */ void spiningTop_Window::RunRenderTick() /* override */
{
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GUI_Start();

}

/* virtual */ void spiningTop_Window::RunClear() /* override */
{
	
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
	ImGui::Text("USTAWIENIA");
}

void spiningTop_Window::GUI_WindowRender()
{
	ImGui::Text("RENDEROWANIE");
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