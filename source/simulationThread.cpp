#include "simulationThread.hpp"

#include <iostream>

simulationThread::simulationThread()
{

}

simulationThread::~simulationThread()
{
	if (b_isThreadRunning)
	{
		b_shouldEndSimulation = true;
		m_simulationThread.join();
	}
}

void simulationThread::StartSimulation()
{
	b_shouldEndSimulation = false;
	m_simulationThread = std::thread(&simulationThread::SimulationThread, this);
	b_isThreadRunning = true;
}

void simulationThread::StopSimulation()
{
	if (b_isThreadRunning)
	{
		m_stopSimulation.lock();
	}
}

void simulationThread::ContinueSimulation()
{
	if (b_isThreadRunning)
	{
		m_stopSimulation.unlock();
	}
}

void simulationThread::EndSimulation()
{
	if (b_isThreadRunning)
	{
		b_shouldEndSimulation = true;
		m_stopSimulation.unlock();

		m_simulationThread.join();
		b_isThreadRunning = false;
	}
}

void simulationThread::SimulationThread()
{
	while (true)
	{
		m_stopSimulation.lock();
		if (b_shouldEndSimulation)  {
			m_stopSimulation.unlock();
			break; 
		}
		m_stopSimulation.unlock();

		std::this_thread::sleep_for(std::chrono::duration<double>(0.5));
		std::cout << "Jestem" << std::endl;
	}
}