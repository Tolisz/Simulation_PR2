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

