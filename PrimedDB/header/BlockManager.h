#pragma once
#include "Block.h"
#include "Singleton.h"
#include "Transection.h"

namespace liao::PrimedDB
{
	//file structure
	//first line: byte size;
	//second line: length of bits array, bit array.
	class File
	{
	public:
		std::shared_ptr<std::fstream> m_file;
		int m_byteSize;
		std::vector<bool> m_available;
		std::deque<int> m_owned;
		File(const std::string& name);
		void dropBlock(int index);
		~File();
	};
	class BlockManager:public Singleton<BlockManager>
	{
		static unsigned BlockPerRecord(int byteSize);
		INVITESINGLETON;
		
		std::unordered_map<std::string, File> m_files;

		//the index queue of m_blocks
		std::deque<std::pair<size_t,std::string>> m_active;
		
		//store all blocks, which will only enlarge the size.
		std::vector<Block> m_blocks;


		BlockManager();
	public:
		void write(Table& table, unsigned linePos,std::shared_ptr<char>& memory, int size);
		~BlockManager();
	};
}
