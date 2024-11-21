#include "trajectoryBuffer.hpp"

#include <iostream>

trajectoryBuffer::trajectoryBuffer(std::size_t initialPointsNum)
{
	InitGL(initialPointsNum);
	ReallocateMemory(initialPointsNum);
}

trajectoryBuffer::~trajectoryBuffer()
{
	DeInitGL();
}

void trajectoryBuffer::Draw()
{
	Lock();
	ReallocateGPUMemory();
	Unlock();

	glBindVertexArray(m_vertexArray);
	glDrawArrays(GL_POINTS, 0, Size());
	glBindVertexArray(0);
}

void trajectoryBuffer::ReallocateMemory(std::size_t newCapacity)
{
	ReallocateCPUMemory(newCapacity);
	ReallocateGPUMemory();
}

void trajectoryBuffer::FreeMemory()
{
	// CPU buffer free
	m_cpu_buffer.clear();
	m_cpu_writePos = 0;

	// GPU buffer free
	size_t currentCapacity = sizeof(glm::vec3) * Capacity();
	glBindBuffer(GL_ARRAY_BUFFER, m_gpu_buffer);
	glBufferData(GL_ARRAY_BUFFER, currentCapacity, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void trajectoryBuffer::ReallocateCPUMemory(std::size_t newCapacity)
{	
	std::size_t oldCapacity = m_cpu_buffer.capacity();
	std::size_t oldSize = m_cpu_buffer.size();

	if (oldCapacity == newCapacity)
	{
		return;
	}

	if (m_cpu_buffer.capacity() == 0)
	{
		m_cpu_buffer.reserve(newCapacity);
		return;
	}

	std::vector<glm::vec3> newBuffer;
	newBuffer.reserve(newCapacity);

	if (oldCapacity < newCapacity)
	{
		if (oldSize < oldCapacity) 
		{
			auto it_beg = m_cpu_buffer.begin();
			auto it_end = m_cpu_buffer.begin();
			std::advance(it_end, m_cpu_writePos);

			newBuffer.insert(newBuffer.end(), it_beg, it_end);
		}
		else // oldSize == oldCapacity
		{
			auto it_beg = m_cpu_buffer.begin();
			std::advance(it_beg, m_cpu_writePos);
			auto it_end = m_cpu_buffer.end();
			newBuffer.insert(newBuffer.end(), it_beg, it_end);

			it_beg = m_cpu_buffer.begin();
			it_end = m_cpu_buffer.begin();
			std::advance(it_end, m_cpu_writePos);
			newBuffer.insert(newBuffer.end(), it_beg, it_end);
			
			m_cpu_writePos = newBuffer.size();
		}
	}
	else // oldCapacity > newCapacity  
	{
		if (oldSize < oldCapacity) 
		{
			if (m_cpu_writePos < newCapacity) 
			{
				auto it_beg = m_cpu_buffer.begin();
				auto it_end = m_cpu_buffer.begin();
				std::advance(it_end, m_cpu_writePos);
				
				newBuffer.insert(newBuffer.end(), it_beg, it_end);
			}
			else // m_cpu_writePos >= newCapacity
			{
				int range = m_cpu_writePos - newCapacity;
				auto it_beg = m_cpu_buffer.begin();
				std::advance(it_beg, range);
				auto it_end = m_cpu_buffer.begin();
				std::advance(it_end, m_cpu_writePos);
				
				newBuffer.insert(newBuffer.end(), it_beg, it_end);
				m_cpu_writePos = 0;
			}
		}
		else // oldSize == oldCapacity
		{
			if (m_cpu_writePos < newCapacity) 
			{
				int frontLength = m_cpu_writePos;
				int backLength = newCapacity - frontLength;
				int to_back = oldCapacity - backLength;
				
				auto it_beg = m_cpu_buffer.begin();
				std::advance(it_beg, to_back);
				auto it_end = m_cpu_buffer.end();

				newBuffer.insert(newBuffer.end(), it_beg, it_end);

				it_beg = m_cpu_buffer.begin();
				it_end = m_cpu_buffer.begin();
				std::advance(it_end, frontLength);

				newBuffer.insert(newBuffer.end(), it_beg, it_end);
				m_cpu_writePos = 0;
			}
			else // m_cpu_writePos >= newCapacity
			{
				int range = m_cpu_writePos - newCapacity;
				auto it_beg = m_cpu_buffer.begin();
				std::advance(it_beg, range);
				auto it_end = m_cpu_buffer.begin();
				std::advance(it_end, m_cpu_writePos);
				
				newBuffer.insert(newBuffer.end(), it_beg, it_end);
				m_cpu_writePos = 0;
			}
		}
	}
	
	m_cpu_buffer = std::move(newBuffer);
}

void trajectoryBuffer::ReallocateGPUMemory()
{
	size_t newCapacity = sizeof(glm::vec3) * Capacity();
	size_t writeSize = sizeof(glm::vec3) * Size();

	glBindBuffer(GL_ARRAY_BUFFER, m_gpu_buffer);
	glBufferData(GL_ARRAY_BUFFER, newCapacity, nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, writeSize, GetDataFrom(0));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void trajectoryBuffer::PutPoint(const glm::vec3& point)
{
	if (m_cpu_buffer.size() < m_cpu_buffer.capacity())
	{
		m_cpu_buffer.push_back(point);
		m_cpu_writePos += 1;
	}
	else {
		m_cpu_writePos %= m_cpu_buffer.size();
		m_cpu_buffer[m_cpu_writePos] = point;
		m_cpu_writePos += 1;
	}
}

void trajectoryBuffer::Lock()
{
	m_bufferAccess.lock();
}

void trajectoryBuffer::Unlock()
{
	m_bufferAccess.unlock();
}

std::size_t trajectoryBuffer::Capacity()
{
	return m_cpu_buffer.capacity();
}

std::size_t trajectoryBuffer::Size()
{
	return m_cpu_buffer.size();
}

int trajectoryBuffer::WritePos()
{
	return m_cpu_writePos; 
}

void* trajectoryBuffer::GetDataFrom(int pos)
{
	glm::vec3* start = m_cpu_buffer.data();
	start += pos;
	return static_cast<void*>(start);
}

void trajectoryBuffer::Reset()
{
	m_cpu_buffer.clear();
	m_cpu_writePos = 0;
}

void trajectoryBuffer::InitGL(std::size_t initialPointsNum)
{
	glCreateVertexArrays(1, &m_vertexArray);
	glCreateBuffers(1, &m_gpu_buffer);

    glBindVertexArray(m_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, m_gpu_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void trajectoryBuffer::DeInitGL()
{
	glDeleteVertexArrays(1, &m_vertexArray);
	glDeleteBuffers(1, &m_gpu_buffer);
}
