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

void simulationThread::StartSimulation(std::shared_ptr<const simulationParameters> params)
{
	PrepareSimulationValues(params);

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

glm::quat simulationThread::GetRotation()
{
	glm::quat currentRotation;
	m_blockWQRead.lock();
	currentRotation = m_Q;
	m_blockWQRead.unlock();

	return currentRotation;
}

void simulationThread::PrepareSimulationValues(std::shared_ptr<const simulationParameters> params)
{
	float a = params->m_cubeEdgeLength;
	float ro = params->m_cubeDensity;
	m_I = glm::mat3(
		(11.0f / 5.0f) * glm::pow(a, 5) * ro, 0, 0,
		0, (glm::pow(a, 5) * ro) / 6.0f, 0,
		0, 0, (11.0f / 5.0f) * glm::pow(a, 5) * ro
	);

	m_invI = glm::mat3(
		1.0f / m_I[0][0], 0, 0,
		0, 1.0f / m_I[1][1], 0,
		0, 0, 1.0f / m_I[2][2]
	);

	float V = a * a * a;
	float m = ro * V;
	m_f = glm::vec3(0.0f, - m * 9.8f, 0.0f);
	
	m_Q = glm::angleAxis(params->m_cubeTilt, glm::vec3({0.0f, 0.0f, 1.0f}));
	m_W = glm::vec3(0.0f, params->m_cubeAngularVelocity, 0.0f);



	m_d = a * glm::sqrt(3);
	m_h = params->m_delta;
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

		std::pair<glm::vec3, glm::quat> nextValues = RK4();

		m_blockWQRead.lock();
		m_W = nextValues.first;
		m_Q = nextValues.second;
		m_blockWQRead.unlock();

		std::this_thread::sleep_for(std::chrono::duration<double>(m_h));
		// std::cout << "Jestem" << std::endl;
	}
}

std::pair<glm::vec3, glm::quat> simulationThread::RK4()
{
	std::pair<glm::vec3, glm::quat> newValues;

	// First Equation
	glm::vec3 N = ComputeBodyTorque();
	{
		glm::vec3 k1 = m_h * Derivative_W(N, m_W);
		glm::vec3 k2 = m_h * Derivative_W(N, (m_W + k1 / 2.0f));
		glm::vec3 k3 = m_h * Derivative_W(N, (m_W + k2 / 2.0f));
		glm::vec3 k4 = m_h * Derivative_W(N, (m_W + k3));
		glm::vec3 dW = (k1 + 2.0f * k2 + 2.0f * k3 + k4) / 6.0f;
		newValues.first = m_W + dW;
	}

	// Second Equation
	{
		glm::quat k1 = m_h * Derivative_Q(m_Q, m_W);
		glm::quat k2 = m_h * Derivative_Q(m_Q + k1 / 2.0f, m_W);
		glm::quat k3 = m_h * Derivative_Q(m_Q + k2 / 2.0f, m_W);
		glm::quat k4 = m_h * Derivative_Q(m_Q + k3, m_W);
		glm::quat dQ = (k1 + 2.0f * k2 + 2.0f * k3 + k4) / 6.0f;
		newValues.second = m_Q + dQ; 
	}

	return newValues;
}

glm::vec3 simulationThread::Derivative_W(const glm::vec3& N, const glm::vec3& W)
{
	return m_invI * (N + glm::cross((m_I * W), W));
}

glm::quat simulationThread::Derivative_Q(const glm::quat& Q, const glm::vec3& W)
{
	glm::quat W_quat = glm::quat(0.0f, W.x, W.y, W.z);
	return 0.5f * Q * W_quat;
}

glm::vec3 simulationThread::ComputeBodyTorque()
{
	glm::vec3 r = FindCenterOfMass();
	glm::vec3 n = glm::cross(r, m_f);
	glm::quat p = glm::quat(0.0f, n);

	return glm::axis(m_Q * p * glm::conjugate(m_Q));
}

glm::vec3 simulationThread::FindCenterOfMass()
{
	glm::quat p = glm::quat(0.0f, glm::vec3(0.0f, 1.0f, 0.0f)); 
	glm::vec3 v = glm::axis(m_Q * p * glm::conjugate(m_Q));

	return 0.5f * m_d * v;
}