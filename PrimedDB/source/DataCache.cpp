#include "DataCache.h"

#include <algorithm>
USESTD;
namespace liao::PrimedDB
{
	void DataCache::allocate()
	{
		m_data.reserve(5);
		char* ptr;
		while (!m_file.eof()&&m_data.size()<5)
		{
			ptr = new char[(m_byteSize + 1) * RECORD_NUM];
			m_file.getline(ptr, (m_byteSize + 1) * RECORD_NUM);
			m_data.push_back(ptr);
		}
		
	}
	void DataCache::clear()
	{
		for (char* data : m_data)
		{
			delete[] data;
		}
	}
	void DataCache::flush()
	{
		
	}
	void DataCache::nextPage()
	{
		
	}
	void DataCache::fileLocate(size_t lineNumber)
	{
		m_file.seekg(lineNumber * (m_byteSize + 1), std::ios::beg);
		m_file.seekp(lineNumber * (m_byteSize + 1), std::ios::beg);
		filePosition = lineNumber;
	}
	DataCache::DataCache(std::string_view fileName, size_t size)
		:m_byteSize(size),m_activePage(0)
	{
		m_file.open(fileName, std::ios::binary | std::ios::in | std::ios::out);
		if (m_file.fail())
		{
			allocate();
		}
	}
	void DataCache::resize(size_t size)
	{
		m_byteSize = size;
	}
	std::string_view DataCache::get(size_t index)const
	{
        ReadLock lock(m_mutex);
		return m_data[m_activePage]+index*m_byteSize;
	}
	void DataCache::update(size_t index, std::string_view data)
	{
        WriteLock lock(m_mutex);
		if (m_file.is_open())
		{

			m_file.flush();
		}
	}
	void DataCache::insert(std::string_view data)
	{

	}
	void DataCache::remove(size_t index)
	{
		*m_data[index] = false;
	}
	size_t DataCache::size()const
	{
		size_t size = 0;
		{
			ReadLock lock(m_mutex);
            
		}
		return size;
	}
	DataCache::~DataCache()
	{
		m_file.close();
		clear();
		m_data.clear();
	}
}
