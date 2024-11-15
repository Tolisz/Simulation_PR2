#include "trajectoryBuffer.hpp"

#include <iostream>

void trajectoryBuffer::ReallocateMemory(std::size_t newCapacity)
{	
	std::size_t oldCapacity = m_buffer.capacity();
	std::size_t oldSize = m_buffer.size();

	if (oldCapacity == newCapacity)
	{
		return;
	}

	if (m_buffer.capacity() == 0)
	{
		m_buffer.reserve(newCapacity);
		return;
	}

	std::vector<glm::vec3> newBuffer;
	newBuffer.reserve(newCapacity);

	if (oldCapacity < newCapacity)
	{
		if (oldSize < oldCapacity) 
		{
			auto it_beg = m_buffer.begin();
			auto it_end = m_buffer.begin();
			std::advance(it_end, m_WritePos);

			newBuffer.insert(newBuffer.end(), it_beg, it_end);
		}
		else // oldSize == oldCapacity
		{
			auto it_beg = m_buffer.begin();
			std::advance(it_beg, m_WritePos);
			auto it_end = m_buffer.end();
			newBuffer.insert(newBuffer.end(), it_beg, it_end);

			it_beg = m_buffer.begin();
			it_end = m_buffer.begin();
			std::advance(it_end, m_WritePos);
			newBuffer.insert(newBuffer.end(), it_beg, it_end);
			
			m_WritePos = newBuffer.size();
		}
	}
	else // oldCapacity > newCapacity  
	{
		if (oldSize < oldCapacity) 
		{
			if (m_WritePos < newCapacity) 
			{
				auto it_beg = m_buffer.begin();
				auto it_end = m_buffer.begin();
				std::advance(it_end, m_WritePos);
				
				newBuffer.insert(newBuffer.end(), it_beg, it_end);
			}
			else // m_WritePos >= newCapacity
			{
				int range = m_WritePos - newCapacity;
				auto it_beg = m_buffer.begin();
				std::advance(it_beg, range);
				auto it_end = m_buffer.begin();
				std::advance(it_end, m_WritePos);
				
				newBuffer.insert(newBuffer.end(), it_beg, it_end);
				m_WritePos = 0;
			}
		}
		else // oldSize == oldCapacity
		{
			if (m_WritePos < newCapacity) 
			{
				int frontLength = m_WritePos;
				int backLength = newCapacity - frontLength;
				int to_back = oldCapacity - backLength;

				auto it_beg = m_buffer.begin();
				auto it_end = m_buffer.begin();
				std::advance(it_end, frontLength);
				auto start = newBuffer.begin();
				std::advance(start, backLength);

				newBuffer.insert(newBuffer.end(), it_beg, it_end);

				it_beg = m_buffer.begin();
				std::advance(it_beg, to_back);
				it_end = m_buffer.end();
				start = newBuffer.begin();

				newBuffer.insert(newBuffer.end(), it_beg, it_end);
				m_WritePos = 0;
			}
			else // m_WritePos >= newCapacity
			{
				int range = m_WritePos - newCapacity;
				auto it_beg = m_buffer.begin();
				std::advance(it_beg, range);
				auto it_end = m_buffer.begin();
				std::advance(it_end, m_WritePos);
				
				newBuffer.insert(newBuffer.end(), it_beg, it_end);
				m_WritePos = 0;
			}
		}
	}
	
	m_buffer = std::move(newBuffer);
}

void trajectoryBuffer::PutPoint(const glm::vec3& point)
{
	if (m_buffer.size() < m_buffer.capacity())
	{
		m_buffer.push_back(point);
		m_WritePos += 1;
	}
	else {
		m_WritePos %= m_buffer.size();
		m_buffer[m_WritePos] = point;
		m_WritePos += 1;
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
	return m_buffer.capacity();
}

std::size_t trajectoryBuffer::Size()
{
	return m_buffer.size();
}

int trajectoryBuffer::WritePos()
{
	return m_WritePos; 
}

void* trajectoryBuffer::GetDataFrom(int pos)
{
	glm::vec3* start = m_buffer.data();
	start += pos;
	return static_cast<void*>(start);
}

void trajectoryBuffer::Reset()
{
	m_buffer.clear();
	m_WritePos = 0;
}