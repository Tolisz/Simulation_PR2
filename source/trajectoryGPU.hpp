#pragma once 

#include <glad/glad.h>
#include <mutex>

struct trajectoryGPU
{
	std::mutex m_modify;
	GLuint m_pointsBuffer;

	unsigned int m_capacity;
	unsigned int m_size;
	unsigned int m_writeIndex;
}; 