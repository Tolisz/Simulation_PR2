#pragma once

#include "simulationParameters.hpp"

#include <unordered_map>
#include <string>
#include <vector>

class simulationParametersSet
{
public:

	simulationParametersSet();
	~simulationParametersSet() = default;

	simulationParametersSet(const simulationParametersSet&) = delete;
	simulationParametersSet(simulationParametersSet&&) = delete;
	simulationParametersSet& operator=(const simulationParametersSet&) = delete;
	simulationParametersSet& operator=(simulationParametersSet&&) = delete;

	simulationParameters GetParametersByID(const int& id);
	std::string GetDescriptionByID(const int& id);

	std::vector<std::string> GetDescriptions();

	int GetCurrentID();
	void SetCurrentID(const int& newId);

private: 

	void PrepareSimulationParameters();
	void RegisterParameters(
		const simulationParameters& params,
		std::string description);

private:

	std::unordered_map<int, simulationParameters> m_ID_to_Params;
	std::unordered_map<int, std::string> m_ID_to_Description;

	int m_NextID = 0;
	int m_CurrentID = 0;
};