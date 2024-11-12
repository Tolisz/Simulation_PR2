#pragma once

#include "simulationParameters.hpp"
#include "drawParameters.hpp"
#include "spinningTop_Renderer.hpp"
#include <memory>


class spinningTop_App
{
public:
	spinningTop_App();
	~spinningTop_App();

	spinningTop_App(spinningTop_App&&) = delete;
	spinningTop_App(const spinningTop_App&) = delete;
	spinningTop_App& operator=(spinningTop_App&&) = delete;
	spinningTop_App& operator=(const spinningTop_App&) = delete;

	std::shared_ptr<simulationParameters> 	GetSimulationParameters();
	std::shared_ptr<drawParameters> 		GetDrawParameters();

	void StartSimulation();
	void StopSimulation();
	void ResetSimulation();

	bool IsRunning();
	bool IsStopped();

	void RenderScene();
	GLuint GetRenderTexture();
	void SetRenderArea(int width, int height);
	void UpdateCameraPosition(float delta);
	void UpdateCameraRotation(float rotX, float rotY);
	
private:

	std::shared_ptr<simulationParameters> 	m_paramsSimulation;
	std::shared_ptr<drawParameters> 		m_paramsDraw;

	enum class State
	{
		Initial,
		Running,
		Stopped
	} 
	m_state = State::Initial;

	std::unique_ptr<spinningTop_Renderer> 	m_renderer;
};