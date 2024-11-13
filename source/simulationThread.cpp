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
		m_blockSimulation.lock();
	}
}

void simulationThread::ContinueSimulation()
{
	if (b_isThreadRunning)
	{
		m_blockSimulation.unlock();
	}
}

void simulationThread::EndSimulation()
{
	if (b_isThreadRunning)
	{
		b_shouldEndSimulation = true;
		m_blockSimulation.unlock();

		m_simulationThread.join();
		b_isThreadRunning = false;
	}
}

void simulationThread::SimulationThread()
{
	while (true)
	{
		m_blockSimulation.lock();
		if (b_shouldEndSimulation)  {
			m_blockSimulation.unlock();
			break; 
		}
		m_blockSimulation.unlock();

		std::this_thread::sleep_for(std::chrono::duration<double>(0.5));
		std::cout << "Jestem" << std::endl;
	}
}