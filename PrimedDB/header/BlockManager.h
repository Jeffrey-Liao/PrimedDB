#pragma once
#include "Block.h"
#include "Singleton.h"
#include "Transection.h"

namespace liao::PrimedDB
{
	//file structure
	//first line: length of bits array , bit array.
	class BlockManager:public Singleton<BlockManager>
	{
		INVITESINGLETON;
		struct File
		{
			std::shared_ptr<std::fstream> m_file;
			std::vector<bool> m_available;
			std::vector<bool> m_blocks;
			File(const std::string& name);
			~File();
		};
		std::unordered_map<std::string, File> m_files;

		//the index queue of m_blocks
		std::deque<size_t> m_active;
        std::deque<size_t> m_inactive;
		//store all blocks, which will only enlarge the size.
		std::vector<Block> m_blocks;

		BlockManager();
	public:
		DYNAMICCON(Concept_TransectionTypeRequired)
		void opearte(const Transection<T>&);
		~BlockManager();
	};
}
