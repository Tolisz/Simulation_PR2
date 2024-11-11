#pragma once

#include "simulationParameters.hpp"
#include "drawParameters.hpp"
#include <memory>


class springTop_App
{

public:
	springTop_App();
	~springTop_App();

	springTop_App(springTop_App&&) = delete;
	springTop_App(const springTop_App&) = delete;
	springTop_App& operator=(springTop_App&&) = delete;
	springTop_App& operator=(const springTop_App&) = delete;

	std::shared_ptr<simulationParameters> 	GetSimulationParameters();
	std::shared_ptr<drawParameters> 		GetDrawParameters();

private:

	std::shared_ptr<simulationParameters> 	m_paramsSimulation;
	std::shared_ptr<drawParameters> 		m_paramsDraw;

};