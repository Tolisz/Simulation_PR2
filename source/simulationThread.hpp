#pragma once 

#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <semaphore>

#include "simulationParameters.hpp"
#include "trajectoryBuffer.hpp"

class simulationThread
{
public:

	simulationThread(std::shared_ptr<trajectoryBuffer> buffer);
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
	void ApplyForce(bool apply);

private:

	void PrepareSimulationValues(std::shared_ptr<const simulationParameters> params);

	// Thread functions
	void SimulationThread();
	std::pair<glm::vec3, glm::quat> RK4();
	glm::vec3 Derivative_W(const glm::vec3& W, const glm::mat3& I, const glm::mat3& invI, const glm::vec3& N);
	glm::quat Derivative_Q(const glm::quat& Q, const glm::vec3& W);
	glm::vec3 ComputeBodyTorque();
	glm::vec3 ComputeCenterOfMass();
	// ================

private:

	// Thread logic
	std::thread m_simulationThread;
	bool b_threadRunning = false;
	bool b_simulationStopped = true;

	std::mutex 			m_stopSimulation;
	std::atomic_bool 	b_endSimulation;

	// Simulation members 
	glm::mat3 m_I;
	glm::mat3 m_invI;
	glm::vec3 m_f;
	std::atomic_bool b_applyForce = true;
	float m_diag; // diagonal Length
	float m_dt;

	std::mutex	m_accessWQ;
	glm::quat m_Q;
	glm::vec3 m_W;

	glm::vec3 m_cornerPoint;
	std::shared_ptr<trajectoryBuffer> m_trajectoryBuffer;
};