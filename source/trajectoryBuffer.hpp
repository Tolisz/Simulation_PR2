#pragma once 

#include <vector>
#include <mutex>
#include <glm/glm.hpp>

class trajectoryBuffer
{
public:

	trajectoryBuffer() = default;
	~trajectoryBuffer() = default;

	void ReallocateMemory(std::size_t newCapacity);
	void PutPoint(const glm::vec3& point);

	void Lock();
	void Unlock();

private:

	std::mutex m_bufferAccess;
	std::vector<glm::vec3> m_buffer;

	int m_WritePos = 0;
};