#pragma once
#include "Page.h"

namespace liao::PrimedDB
{
	static const size_t PAGE_SIZE = 5;

	class DataCache
	{
		size_t m_activePage;
		//record number,bool,column1,column2,column3,column4,column5...
		std::vector<char*> m_data;
		int m_byteSize;
		std::fstream m_file;
		size_t filePosition;

		mutable ShareMutex m_mutex;

		void allocate();
		void clear();
		void flush();
		void nextPage();
		void fileLocate(size_t lineNumber);
	public:
		DataCache(std::string_view fileName,size_t size);
		void resize(size_t size);
		std::string_view get(size_t index)const;
		DYNAMIC
		std::shared_ptr<T> get(size_t index)
		{
			if (m_byteSize == sizeof(T)&&index < m_data.size())
			{
				char* object = new char[m_byteSize];
                memcpy(object, m_data[index], m_byteSize);
                return std::make_shared<T>(object);
			}
			return nullptr;
		}
		size_t size()const;
		void update(size_t index,std::string_view data);
		void insert(std::string_view data);
		void remove(size_t index);
		DataCache() = delete;

		~DataCache();

	};
}
