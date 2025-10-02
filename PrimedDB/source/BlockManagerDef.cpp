#include "BlockManager.h"
namespace liao::PrimedDB
{
	BlockManager::File::File(const std::string& name)
		:m_file(StaticFunc::OpenDataFile(name)),m_blocks(Util::Setting::Get().getBlockNumber())
	{
		int size;
		char* buffer = nullptr;
		if (m_file != nullptr)
		{
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
	BlockManager::File::~File()
	{
		m_file->close();
	}
	BlockManager::BlockManager()
		:m_blocks(Util::Setting::Get().getBlockNumber())
	{}
	BlockManager::~BlockManager()
	{
		m_files.clear();
        m_active.clear();
        m_inactive.clear();
		m_blocks.clear();
	}
}