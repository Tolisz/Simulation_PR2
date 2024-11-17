#include "simulationParametersSet.hpp"

simulationParametersSet::simulationParametersSet()
{
	PrepareSimulationParameters();
}

simulationParametersSet::~simulationParametersSet()
{

}

simulationParameters simulationParametersSet::GetParametersByID(const int& id)
{
	return m_ID_to_Params.at(id);
}

std::string simulationParametersSet::GetDescriptionByID(const int& id)
{
	return m_ID_to_Description.at(id);
}

std::vector<std::string> simulationParametersSet::GetDescriptions()
{
	int n = m_NextID;
	std::vector<std::string> descriptions;
	descriptions.reserve(n);

	for (int i = 0 ; i < n; ++i)
	{
		descriptions.push_back(GetDescriptionByID(i));
	}

	return descriptions;
}

int simulationParametersSet::GetCurrentID()
{	
	return m_CurrentID;
}

void simulationParametersSet::SetCurrentID(const int& newId)
{
	m_CurrentID = newId;
}

void simulationParametersSet::PrepareSimulationParameters()
{
	simulationParameters params;

	// default
	params.m_cubeEdgeLength 		= 1.0f;
	params.m_cubeDensity 			= 2.0f;
	params.m_cubeTilt 				= 0.0f;
	params.m_cubeAngularVelocity 	= 2.0f * glm::pi<float>();
	params.m_delta 					= 0.001f;
	params.b_Gravity 				= true;

	RegisterParameters(params, "Default");

	// pendulum
	params.m_cubeEdgeLength 		= 1.0f;
	params.m_cubeDensity 			= 2.0f;
	params.m_cubeTilt 				= glm::radians(100.0f);
	params.m_cubeAngularVelocity 	= 0.0f;
	params.m_delta 					= 0.001f;
	params.b_Gravity 				= true;

	RegisterParameters(params, "Pendulum");

	// spinning top 1
	params.m_cubeEdgeLength 		= 1.0f;
	params.m_cubeDensity 			= 2.0f;
	params.m_cubeTilt 				= glm::radians(30.0f);
	params.m_cubeAngularVelocity 	= 20.0f * glm::pi<float>();
	params.m_delta 					= 0.001f;
	params.b_Gravity 				= true;

	RegisterParameters(params, "Spinning Top 1");

	// spinning top 2
	params.m_cubeEdgeLength 		= 1.0f;
	params.m_cubeDensity 			= 2.0f;
	params.m_cubeTilt 				= glm::radians(30.0f);
	params.m_cubeAngularVelocity 	= 15.0f * glm::pi<float>();
	params.m_delta 					= 0.001f;
	params.b_Gravity 				= true;

	RegisterParameters(params, "Spinning Top 2");

	// spinning top 3
	params.m_cubeEdgeLength 		= 1.0f;
	params.m_cubeDensity 			= 2.0f;
	params.m_cubeTilt 				= glm::radians(45.0f);
	params.m_cubeAngularVelocity 	= 20.0f * glm::pi<float>();
	params.m_delta 					= 0.001f;
	params.b_Gravity 				= true;

	RegisterParameters(params, "Spinning Top 3");

	// spinning top
	params.m_cubeEdgeLength 		= 1.0f;
	params.m_cubeDensity 			= 2.0f;
	params.m_cubeTilt 				= 0.0f;
	params.m_cubeAngularVelocity 	= 0.0f;
	params.m_delta 					= 0.001f;
	params.b_Gravity 				= true;

	RegisterParameters(params, "Static");

	// bowl
	params.m_cubeEdgeLength 		= 1.0f;
	params.m_cubeDensity 			= 0.1f;
	params.m_cubeTilt 				= glm::radians(90.0f);
	params.m_cubeAngularVelocity 	= 1.0f * glm::pi<float>();
	params.m_delta 					= 0.001f;
	params.b_Gravity 				= true;

	RegisterParameters(params, "Bowl");

	// crown
	params.m_cubeEdgeLength 		= 1.0f;
	params.m_cubeDensity 			= 2.0f;
	params.m_cubeTilt 				= glm::radians(90.0f);
	params.m_cubeAngularVelocity 	= 9.55f * glm::pi<float>();
	params.m_delta 					= 0.001f;
	params.b_Gravity 				= true;

	RegisterParameters(params, "Crown");

	// flower
	params.m_cubeEdgeLength 		= 1.0f;
	params.m_cubeDensity 			= 2.0f;
	params.m_cubeTilt 				= glm::radians(30.0f);
	params.m_cubeAngularVelocity 	= 42.13f;
	params.m_delta 					= 0.001f;
	params.b_Gravity 				= true;

	RegisterParameters(params, "Flower");

}

void simulationParametersSet::RegisterParameters(
	const simulationParameters& params,
	std::string description)
{
	m_ID_to_Params.insert({m_NextID, params});
	m_ID_to_Description.insert({m_NextID, description});

	++m_NextID;
}