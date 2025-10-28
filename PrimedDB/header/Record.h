#pragma once
#include "Defs.h"
#include "Global.h"

namespace liao::PrimedDB {
	class Record
	{
		std::vector<bool> m_valid;
		std::vector<unsigned> m_byteSize;
		std::vector<char*> m_memory;
		std::vector<ReadLock> m_locks;
	public:
		Record(std::vector<bool>&, std::vector<unsigned>&,std::vector<char*>&, std::vector<ReadLock>&);
		unsigned byte();
		DYNAMIC
		std::vector<T> selectTo()
		{
			std::vector<T> result;
			if (byte() == sizeof(T))
			{
				for (auto ptr: m_memory)
					result.emplace_back(*((T*)ptr));
			}
			return std::move(result);
		}
		std::vector<char*>& select();
		unsigned size();
		bool empty();
		const char* get(unsigned)const;
		~Record();
	};
}
