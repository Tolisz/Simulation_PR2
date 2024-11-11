#pragma once

#include "simulationParameters.hpp"
#include "drawParameters.hpp"
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

};