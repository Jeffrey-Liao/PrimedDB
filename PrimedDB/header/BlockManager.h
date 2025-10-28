#pragma once
#include "Block.h"
#include "DataFile.h"
#include "Singleton.h"
#include "TableManager.h"
#include "Transection.h"

namespace liao::PrimedDB
{
	//file structure
	//first line: byte size;
	//second line: length of bits array, bit array.
	
	class BlockManager:public Singleton<BlockManager>
	{
		INVITESINGLETON;

		std::deque<unsigned> m_active;
		
		//store all blocks, which will only enlarge the size.
		std::vector<Block> m_blocks;
		std::future<void> m_terminate;
		std::atomic<bool> m_notify = false, m_end = false;
		std::condition_variable m_cv;
		std::deque<Transection> m_pendingOperations;
		mutable ShareMutex m_mutex;
		mutable Mutex m_cvMutex;

		BlockManager();


		void manager();
		void rearrange(std::deque<int>&, std::vector<bool>&);
		void handle(Transection&);
	public:
		static unsigned totalRecord(unsigned bytes);
		void operate(Transection&);
		unsigned allocate();
		void drop(unsigned pos);
		Block& get_noLock(unsigned);
		
		~BlockManager();
	};
}
