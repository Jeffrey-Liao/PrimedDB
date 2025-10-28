#include "BlockManager.h"
#include "Log.h"

namespace liao::PrimedDB
{
	BlockManager::BlockManager()
	{
		m_blocks.resize(Util::Setting::Get().getBlockNumber());
		m_active.resize(m_blocks.size());
		m_terminate = std::async(std::launch::async, &BlockManager::manager,this);
		for (int n = 0;n<m_blocks.size();++n)
		{
			m_active[n];
		}
	}
	void BlockManager::manager()
	{
		Lock lock(m_cvMutex);
		while (true)
		{
			m_cv.wait(lock, [this]()
			{
					return m_notify||m_end;
			});
			
			while (!m_pendingOperations.empty())
			{
				Transection transection;
				{
					WriteLock lock(m_mutex);
					transection = std::move(m_pendingOperations.front());
					m_pendingOperations.pop_front();
				}
				handle(transection);
			}
			m_notify = false;
			if (m_end)
				break;
		}
	}

	unsigned BlockManager::allocate()
	{
		unsigned pos;
		{
			WriteLock lock(m_mutex);
			pos = m_active.front();
			while (!m_blocks[pos].getMutex().try_lock());
			m_active.pop_front();
			m_active.push_back(pos);
		}
		return pos;
	}
	void BlockManager::handle(Transection& transection)
	{
		if (transection.isValid())
		{
			transection.setInvalid();
			auto memory = transection.getMemory();
			unsigned position = transection.getBlockId();
			auto table = TableManager::Get().get_noLock(transection.getTable());
			unsigned location = transection.getLocation();
			if (transection.getType() == TransectionType::Insert)
			{
				std::pair<unsigned, std::shared_ptr<char[]>> result = std::make_pair(transection.size(), memory);
				do
				{
					if (position == -1)
					{
						position = allocate();
						transection.setBlockId(position);
						{
							m_blocks[position].getMutex().unlock();
							m_blocks[position].assign(table, table->getOwned().size()*totalRecord(table->totalByte()));
						}
						table->addBlock(position);
					}
					result = m_blocks[position].insert(result.second, result.first);
					position = result.first == 0 ? 0 : -1;
				}
				while (result.first > 0);
				table->incrementSize();
			}
			else if (transection.getType() == TransectionType::Update)
			{
				if (position == -1)
				{
					Util::ErrorManager::Get().set(Util::ErrorLevel::Error, "BlockIdError", "Given block id is incorrect.", Infor::ClassInfor(THISFUNC, THISFILE));
					return;
				}
				m_blocks[position].update(location, memory);
			}
			else if (transection.getType() == TransectionType::Delete)
			{
				if (position == -1)
				{
					Util::ErrorManager::Get().set(Util::ErrorLevel::Error, "BlockIdError", "Given block id is incorrect.", Infor::ClassInfor(THISFUNC, THISFILE));
					return;
				}
				m_blocks[position].remove(location);
				if (m_blocks[position].percentage() < 0.5)
				{
					rearrange(table->getOwned(), table->getAvailable());
				}
			}

		}
		
	}
	unsigned BlockManager::totalRecord(unsigned bytes)
	{
		return Util::Setting::Get().getBlockNumber() / bytes;
	}
	void BlockManager::operate(Transection& transection)
	{
		WriteLock lock(m_mutex);
		m_pendingOperations.emplace_back(std::move(transection));
		m_notify = true;
		m_cv.notify_one();
	}
	void BlockManager::rearrange(std::deque<int>& owned, std::vector<bool>& available)
	{
		std::vector<char*> memory;
		unsigned byteSize = 0;
		std::vector<ReadLock> locks;
		for (unsigned n =0;n<owned.size();++n)
		{
			if (byteSize == 0)
			{
				byteSize = m_blocks[n].byte();
			}
			for (unsigned m = 0;m<m_blocks[n].size();++m)
			{
				locks.emplace_back(m_blocks[n].getMutex());
				memory.push_back(m_blocks[n].get_noLock(m));
			}
		}
		unsigned fast = 0, slow =0;
		for (;fast<available.size();++fast)
		{
			if (!available[fast])
			{
				memcpy_s(memory[slow],byteSize,memory[fast],byteSize);
				available[slow] = true;
				slow = fast;
			}
		}
	}
	void BlockManager::drop(unsigned pos)
	{
		if (m_active.empty())
		{
			m_active.erase(std::find(m_active.begin(), m_active.end(), pos));
			m_active.push_back(pos);
			m_blocks[pos].drop(pos);
		}
	}
	Block& BlockManager::get_noLock(unsigned pos)
	{
		return m_blocks[pos];
	}
	BlockManager::~BlockManager()
	{

		m_end = true;
		m_notify = true;
		if (m_terminate.valid())
			m_terminate.get();
		m_blocks.clear();
		TableManager::Get().clear();
	}
}
