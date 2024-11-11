#include "springTop_App.hpp"

#include <cassert> 

springTop_App::springTop_App()
{
	m_paramsSimulation 	= std::make_shared<simulationParameters>();
	m_paramsDraw 		= std::make_shared<drawParameters>();
}

springTop_App::~springTop_App()
{
	// DEGUB ONLY
	assert(("Something still uses shader object", m_paramsDraw.use_count() == 1));
	assert(("Something still uses shader object", m_paramsSimulation.use_count() == 1));
}

std::shared_ptr<simulationParameters> springTop_App::GetSimulationParameters()
{
	return m_paramsSimulation;
}

std::shared_ptr<drawParameters> springTop_App::GetDrawParameters()
{
	return m_paramsDraw;
}

void springTop_App::StartSimulation()
{
	switch (m_state)
	{
	case State::Initial:
		m_state = State::Running;
		break;
	
	case State::Stopped:
		m_state = State::Running;

	default:
		break;
	}
}

void springTop_App::StopSimulation()
{
	switch (m_state)
	{
	case State::Running:
		m_state = State::Stopped;
		break;
	
	default:
		break;
	}
}

void springTop_App::ResetSimulation()
{
	switch (m_state)
	{
	case State::Stopped:
		m_state = State::Initial;
		break;
	
	default:
		break;
	}
}

bool springTop_App::IsRunning()
{
	return m_state == State::Running;
}

bool springTop_App::IsStopped()
{
	return m_state == State::Stopped;
}
