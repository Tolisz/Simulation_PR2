#include "simulationThread.hpp"

#include <iostream>
// #define GLM_ENABLE_EXPERIMENTAL
// #include <glm/gtx/norm.hpp>

simulationThread::simulationThread(std::shared_ptr<trajectoryBuffer> buffer)
	:m_trajectoryBuffer{buffer}
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

void simulationThread::ApplyForce(bool apply)
{
	b_ApplyForce = apply;
}

void simulationThread::PrepareSimulationValues(std::shared_ptr<const simulationParameters> params)
{
	float a = params->m_cubeEdgeLength;
	float ro = params->m_cubeDensity;

	m_I = glm::mat3(
		(11.0f / 12.0f) * glm::pow(a, 5) * ro, 0, 0,
		0, (glm::pow(a, 5) * ro) / 6.0f, 0,
		0, 0, (11.0f / 12.0f) * glm::pow(a, 5) * ro
	);

	m_invI = glm::mat3(
		1.0f / m_I[0][0], 0, 0,
		0, 1.0f / m_I[1][1], 0,
		0, 0, 1.0f / m_I[2][2]
	);

	float V = a * a * a;
	float m = ro * V;
	m_f = m * glm::vec3(0.0f, -9.8f, 0.0f);
	
	m_Q = glm::angleAxis(params->m_cubeTilt, glm::vec3({0.0f, 0.0f, 1.0f}));
	m_W = glm::vec3(0.0f, params->m_cubeAngularVelocity, 0.0f);

	m_diag = a * glm::sqrt(3);
	m_cornerPoint = glm::vec3(0.0f, m_diag, 0.0f);
	m_dt = params->m_delta;
}

void simulationThread::SimulationThread()
{
	auto START = std::chrono::high_resolution_clock::now();
	int N = 0;
	
	std::chrono::duration<double> DELTA(static_cast<double>(m_dt));
	
	while (true)
	{
		m_blockSimulation.lock();
		if (b_shouldEndSimulation)  {
			m_blockSimulation.unlock();
			break; 
		}
		m_blockSimulation.unlock();

		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> t = now - START;
		int N_theoretical = t.count() / static_cast<double>(m_dt);
		// std::cout << N_theoretical << std::endl;
		
		auto end = N_theoretical > N ? 
			now : std::chrono::high_resolution_clock::now() + DELTA * (N - N_theoretical);
		
		std::pair<glm::vec3, glm::quat> nextValues = RK4();

		m_blockWQRead.lock();
		m_W = nextValues.first;
		m_Q = nextValues.second;
		m_blockWQRead.unlock();

		m_trajectoryBuffer->Lock();
		m_trajectoryBuffer->PutPoint(nextValues.second * m_cornerPoint);
		m_trajectoryBuffer->Unlock();

		while (std::chrono::high_resolution_clock::now() < end) { }

		// std::chrono::duration<double> time = std::chrono::high_resolution_clock::now() - START;
		// std::cout << "N = " << N << "; t = " << time.count() << std::endl;
		N++;
	}
}

std::pair<glm::vec3, glm::quat> simulationThread::RK4()
{
	std::pair<glm::vec3, glm::quat> newValues;

	// First Equation
	glm::vec3 N = b_ApplyForce ? ComputeBodyTorque() : glm::vec3(0.0f);

	glm::vec3 k1_W = Derivative_W(m_W, m_I, m_invI, N);
	glm::vec3 k2_W = Derivative_W(m_W + k1_W * (m_dt / 2.0f), m_I, m_invI, N);
	glm::vec3 k3_W = Derivative_W(m_W + k2_W * (m_dt / 2.0f), m_I, m_invI, N);
	glm::vec3 k4_W = Derivative_W(m_W + k3_W * m_dt, m_I, m_invI, N);
	
	glm::vec3 dW = (k1_W + 2.0f * k2_W + 2.0f * k3_W + k4_W) * (m_dt / 6.0f);
	glm::vec3 W = m_W + dW;
	newValues.first = W;

	// Second Equation
	glm::quat k1_Q = Derivative_Q(m_Q, W);
	glm::quat k2_Q = Derivative_Q(m_Q + k1_Q * (m_dt / 2.0f), W);
	glm::quat k3_Q = Derivative_Q(m_Q + k2_Q * (m_dt / 2.0f), W);
	glm::quat k4_Q = Derivative_Q(m_Q + k3_Q * m_dt, W);

	glm::quat dQ = (k1_Q + 2.0f * k2_Q + 2.0f * k3_Q + k4_Q) * (m_dt / 12.0f);
	newValues.second = glm::normalize(m_Q + dQ); 

	return newValues;
}

glm::vec3 simulationThread::Derivative_W(const glm::vec3& W, const glm::mat3& I, const glm::mat3& invI, const glm::vec3& N)
{
	return invI * (N + glm::cross(I * W, W));
}

glm::quat simulationThread::Derivative_Q(const glm::quat& Q, const glm::vec3& W)
{
	glm::quat W_quat = glm::quat(0.0f, W.x, W.y, W.z);
	return 0.5f * Q * W_quat;
}

glm::vec3 simulationThread::ComputeBodyTorque()
{
	glm::vec3 r = ComputeCenterOfMass();
	glm::vec3 n = glm::cross(r, m_f);

	return glm::conjugate(m_Q) * n;
}

glm::vec3 simulationThread::ComputeCenterOfMass()
{
	glm::vec3 C = glm::vec3(0.0f, 0.5f * m_diag, 0.0f);
	return  m_Q * C;
}