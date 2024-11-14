#pragma once 

#include <vector>
#include <mutex>
#include <glm/glm.hpp>

struct trajectoryBuffer
{
	std::mutex m_bufferAccess;
	std::vector<glm::vec3> m_buffer;

	int m_capacity;
	int m_size		= 0;
	int m_writePos 	= 0;
};