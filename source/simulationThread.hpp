#pragma once 

#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <semaphore>

#include "simulationParameters.hpp"

class simulationThread
{
public:

	simulationThread();
	~simulationThread();

	simulationThread(const simulationThread&) = delete;
	simulationThread(simulationThread&&) = delete;
	simulationThread& operator=(const simulationThread&) = delete;
	simulationThread& operator=(simulationThread&&) = delete;

	void StartSimulation();
	void StopSimulation();
	void ContinueSimulation();
	void EndSimulation();

private:

	void SimulationThread();

private:

	std::thread m_simulationThread;
	bool b_isThreadRunning;

	std::mutex 			m_stopSimulation;
	std::atomic_bool 	b_shouldEndSimulation;
	bool 				b_SimulationStopped;
};