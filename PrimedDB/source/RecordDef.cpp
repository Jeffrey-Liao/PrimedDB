

#include "Record.h"

namespace liao::PrimedDB
{
	Record::Record(std::vector<bool>& valid, std::vector<unsigned>& bytes, std::vector<char*>& memory, std::vector<ReadLock>& locks)
		:m_valid(std::move(valid)),m_byteSize(std::move(bytes)),m_memory(std::move(memory)),m_locks(std::move(locks))
	{
		
	}
	unsigned Record::byte()
	{
		unsigned byte = 0;
		for (int n = 0;n<m_byteSize.size();++n)
		{
			if (m_valid[n])
                byte += m_byteSize[n];
		}
		return byte;
	}
	std::vector<char*>& Record::select()
	{
		return m_memory;
	}
	unsigned Record::size()
	{
		return m_memory.size();
	}
	bool Record::empty()
	{
		for (bool n:m_valid)
		{
			if (n)
                return false;
		}
		return true;
	}
	const char* Record::get(unsigned pos)const
	{
		return m_memory[pos];
	}
}
