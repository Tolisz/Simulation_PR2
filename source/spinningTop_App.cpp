#include "spinningTop_App.hpp"

#include <cassert> 

spinningTop_App::spinningTop_App()
{
	m_paramsSimulation 	= std::make_shared<simulationParameters>();
	m_paramsDraw 		= std::make_shared<drawParameters>();

	m_renderer 		= std::make_unique<spinningTop_Renderer>();
	m_simThread		= std::make_unique<simulationThread>();
}

spinningTop_App::~spinningTop_App()
{
	// DEGUB ONLY
	assert(("Something still uses shader object", m_paramsDraw.use_count() == 1));
	assert(("Something still uses shader object", m_paramsSimulation.use_count() == 1));
}

std::shared_ptr<simulationParameters> spinningTop_App::GetSimulationParameters()
{
	return m_paramsSimulation;
}

std::shared_ptr<drawParameters> spinningTop_App::GetDrawParameters()
{
	return m_paramsDraw;
}

void spinningTop_App::StartSimulation()
{
	switch (m_state)
	{
	case State::Initial:
		m_state = State::Running;
		m_simThread->StartSimulation();
		break;
	
	case State::Stopped:
		m_state = State::Running;
		m_simThread->ContinueSimulation();
	default:
		break;
	}
}

void spinningTop_App::StopSimulation()
{
	switch (m_state)
	{
	case State::Running:
		m_state = State::Stopped;
		m_simThread->StopSimulation();
		break;
	
	default:
		break;
	}
}

void spinningTop_App::ResetSimulation()
{
	switch (m_state)
	{
	case State::Stopped:
		m_state = State::Initial;
		m_simThread->EndSimulation();
		break;
	
	default:
		break;
	}
}

bool spinningTop_App::IsRunning()
{
	return m_state == State::Running;
}

bool spinningTop_App::IsStopped()
{
	return m_state == State::Stopped;
}

void spinningTop_App::RenderScene()
{
	simulationDrawParameters simDrawParams;
	if (m_state == State::Initial) {
		simDrawParams = GetStartSimulationDrawParams();
	}
	else {
		simDrawParams = GetCurrentSimulationDrawParams();
	}

	m_renderer->Render(m_paramsDraw, simDrawParams);
}

GLuint spinningTop_App::GetRenderTexture()
{
	return m_renderer->GetRenderTexture();
}

void spinningTop_App::SetRenderArea(int width, int height)
{
	if (width <= 0 || height <= 0)
		return;

	m_renderer->UpdateRenderArea(width, height);
}

void spinningTop_App::UpdateCameraPosition(float delta)
{
	m_renderer->UpdateCameraPosition(delta);
}

void spinningTop_App::UpdateCameraRotation(float rotX, float rotY)
{
	m_renderer->UpdateCameraRotation(rotX, rotY);
}

simulationDrawParameters spinningTop_App::GetStartSimulationDrawParams()
{
	simulationDrawParameters params;
	params.m_cubeEdgeLength = m_paramsSimulation->m_cubeEdgeLength;
	params.m_Q =  glm::angleAxis(m_paramsSimulation->m_cubeTilt, glm::vec3({0.0f, 0.0f, 1.0f}));

	return params;
}

simulationDrawParameters spinningTop_App::GetCurrentSimulationDrawParams()
{
	simulationDrawParameters params;
	
	/*
		Do dopisania później
	*/

	params.m_cubeEdgeLength = m_paramsSimulation->m_cubeEdgeLength;
	params.m_Q =  glm::angleAxis(m_paramsSimulation->m_cubeTilt, glm::vec3({0.0f, 0.0f, 1.0f}));
	
	return params;
}