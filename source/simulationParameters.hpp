#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct simulationParameters
{
	float m_cubeEdgeLength		= 1.0f;					// Edge length in metters
	float m_cubeDensity			= 2.0f;					// Density in kilograms / metters^3
	float m_cubeTilt			= 0.0f;					// Cube's tilt in radians.
	float m_cubeAngularVelocity	= glm::radians(45.0f);	// Cube's angular velocity in radians
	float m_delta 			= 0.01f;		
	
	bool b_Gravity			= true;			// Trun on/off gravity
};

struct simulationDrawParameters 
{
	bool b_simulationStarted;
	bool b_forceApplied;
	float m_cubeEdgeLength;
	glm::quat m_Q;
};
