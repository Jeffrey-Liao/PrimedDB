#include "BlockManager.h"
#include "Log.h"

namespace liao::PrimedDB
{
	unsigned BlockManager::BlockPerRecord(int byteSize)
	{
		return Util::Setting::Get().getBlockSize() / byteSize;
	}
	File::File(const std::string& name)
		:m_file(StaticFunc::OpenDataFile(name))
	{
		int size;
		char* buffer = nullptr;
		if (m_file != nullptr)
		{
			(*m_file) >> m_byteSize;
			(*m_file) >> size;
			m_available.reserve(size);
			buffer = new char[size = size/8];
            (*m_file).read(buffer, size);
			for (size_t byte_idx = 0; byte_idx < size; ++byte_idx) {
				unsigned char current_byte = static_cast<unsigned char>(buffer[byte_idx]);

				// 一次性处理一个字节的所有比特
				for (int bit_idx = 7; bit_idx >= 0; --bit_idx) {
					m_available.push_back((current_byte >> bit_idx) & 1);
				}
			}
		}
		delete[] buffer;
	}
	File::~File()
	{
		m_file->close();
	}
	void File::dropBlock(int index)
	{
		auto iter = std::find(m_owned.begin(), m_owned.end(), index);
		if (iter != m_owned.end())
			*iter = -1;
	}
	BlockManager::BlockManager()
		:m_blocks(Util::Setting::Get().getBlockNumber())
	{
		auto& value = TableManager::Get().all();
        for (auto& table : value) {
			m_files.emplace(table->getName(), File(table->getName()));
		}
	}
	BlockManager::~BlockManager()
	{
		m_files.clear();
        m_active.clear();
		m_blocks.clear();
	}
	int BlockManager::allocate(const std::string& name)
	{
		std::pair<int, std::string> pair = std::make_pair(0, "");
		if (!m_active.empty())
		{
			if (m_active.size()==m_blocks.size())
			{
				pair = m_active.front();
                m_active.pop_front();
                m_files[pair.second].dropBlock(pair.first);
				pair.second = name;
			}
			else
			{
				pair.first = m_active.back().first+1;
				pair.second = name;
			}
		}
		m_active.emplace_back(pair);
		return pair.first;
	}
	void BlockManager::write(Table& table, unsigned linePos, std::shared_ptr<char>& memory)
	{
		File& file = m_files[table.getName()];
		if (linePos > file.m_available.size())
		{
			Util::ErrorManager::Get().set(Util::ErrorLevel::Error, "IndexOutRange", "Given line number is bigger than the max size of table",Infor::ClassInfor(THISFUNC,THISFILE));
		}
		int blockIndex = linePos/BlockPerRecord(file.m_byteSize),
			lineIndex = linePos % BlockPerRecord(file.m_byteSize);
		m_blocks[file.m_owned[blockIndex]].modify(lineIndex,file.m_byteSize,memory,file.m_byteSize);
	}
	void BlockManager::insert(Table& table, std::shared_ptr<char>& memory)
	{
		File& file = m_files[table.getName()];
		int index= file.m_owned.back();
		while (!m_blocks[index].insert(file.m_byteSize,memory))
		{
			index = allocate(table.getName());
			file.addBlock(index);
            m_blocks[index].insert(file.m_byteSize,memory);
		}

	}
}
