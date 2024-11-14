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

	void StartSimulation(std::shared_ptr<const simulationParameters> params);
	void StopSimulation();
	void ContinueSimulation();
	void EndSimulation();

	glm::quat GetRotation();

private:

	void PrepareSimulationValues(std::shared_ptr<const simulationParameters> params);

	// Thread functions
	void SimulationThread();
	std::pair<glm::vec3, glm::quat> RK4();
	glm::vec3 Derivative_W(const glm::vec3& N, const glm::vec3& W);
	glm::quat Derivative_Q(const glm::quat& Q, const glm::vec3& W);
	glm::vec3 ComputeBodyTorque();
	glm::vec3 FindCenterOfMass();
	// ================

private:

	// Thread logic
	std::thread m_simulationThread;
	bool b_isThreadRunning = false;

	std::mutex 			m_blockSimulation;
	std::atomic_bool 	b_shouldEndSimulation;

	// Simulation members 
	glm::mat3 m_I;
	glm::mat3 m_invI;
	glm::vec3 m_f;
	float m_d; // diagonal Length
	float m_h;

	std::mutex			m_blockWQRead;
	glm::quat m_Q;
	glm::vec3 m_W;

};