
#include "Page.h"

namespace liao::PrimedDB
{
	void Page::calculateUsed()
	{
		for (int n = 0;n<m_size;n+=m_byteSize)
		{
			if (*(m_data+n))
			{
				m_used++;
			}
		}
	}
	bool outRange(page_index index)
	{
		return index >= RECORD_NUM;
	}
	page_index Page::convert(page_index index)const
	{
		return index*m_byteSize;
	}
	Page::Page(char* memory, page_index size,int& byteSize)
		:m_data(memory), m_size(size),m_used(0),m_byteSize(byteSize)
	{
		calculateUsed();
	}
	void Page::remove(page_index index)
	{
		*(m_data+convert(index)) = 0;
		m_used--;
	}
	void Page::insert(page_index index, std::string_view data)
	{
		
	}
	char* Page::get(page_index index)
	{
		return nullptr;
	}
	page_index Page::size() const
	{
		return m_size;
	}
	page_index Page::used() const
	{
		return m_used;
	}
	bool Page::isFull() const
	{
		return m_used == m_size;
	}
	bool Page::isEmpty() const
	{
		return m_used == 0;
	}
	PageStorageLevel Page::getLevel() const
	{
		if (m_used == 0)
			return PageStorageLevel::empty;
		else if (m_used < m_size / 4)
		{
			return PageStorageLevel::quarter;
		}
		else if (m_used >= m_size / 4&& m_used<=m_size/2+ m_size / 4)
		{
			return PageStorageLevel::half;
		}
		else if (m_used > m_size / 2 + m_size / 4&& m_used <= m_size)
		{
			return PageStorageLevel::lot;
		}
		else
			return PageStorageLevel::full;
	}
	Page::~Page()
	{
		delete[] m_data;
	}
}
