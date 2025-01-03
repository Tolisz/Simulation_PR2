#pragma once 

#include <vector>
#include <mutex>
#include <glm/glm.hpp>
#include <glad/glad.h>

class trajectoryBuffer
{
public:

	trajectoryBuffer(std::size_t initialPointsNum);
	~trajectoryBuffer();

	void Draw(bool drawLines);

	void ReallocateMemory(std::size_t newCapacity);
	void FreeMemory();
	void PutPoint(const glm::vec3& point);
	
	void Lock();
	void Unlock();

	std::size_t Capacity();
	std::size_t Size();
	int WritePos();
	
	void* GetDataFrom(int pos);

private: 

	void ReallocateCPUMemory(std::size_t newCapacity);
	void ReallocateGPUMemory();

	void SyncCPUGPUBuffers();

	void InitGL();
	void DeInitGL();

private:

	std::mutex m_bufferAccess;

	std::vector<glm::vec3> m_cpu_buffer;
	int m_cpu_writePos = 0;

	GLuint m_vertexArray;
	GLuint m_gpu_buffer;
	int m_gpu_writePos = 0;

	int m_numOfNotSynced = 0;

	GLuint m_vertexArrayLink;
	GLuint m_elementsBuffer;
};