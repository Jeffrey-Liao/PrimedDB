#include "BlockManager.h"
#include "Log.h"

namespace liao::PrimedDB
{
	BlockManager::BlockManager()
	{
		m_blocks.resize(Util::Setting::Get().getBlockNumber());
		m_active.resize(m_blocks.size());
		m_terminate = std::async(std::launch::async, &BlockManager::manager,this);
		for (unsigned int n = 0;n<m_blocks.size();++n)
		{
			m_active[n] = n;
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
				Transaction transaction;
				{
					WriteLock lock(m_mutex);
					transaction = std::move(m_pendingOperations.front());
					m_pendingOperations.pop_front();
				}
				StaticFunc::WriteInfo("Transaction", std::format("Handling Transaction [{}]", transaction.getId()));
				handle(transaction);
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
			auto owner = m_blocks[pos].getOwner();
			if (owner)
				owner->dropBlock(pos);
			m_active.pop_front();
			m_active.push_back(pos);
		}
		return pos;
	}
	unsigned BlockManager::allocate(TablePtr table, int pos)
	{
		auto newBlock = allocate();
		if (pos == -1)
		{
			table->addBlock(newBlock);
			m_blocks[newBlock].assign(table, table->getOwned().size()*table->totalByte());

		}
		else
		{
			table->getOwned()[pos/StaticFunc::MaxSizeForBlock(table->totalByte())] = newBlock;
			m_blocks[newBlock].assign(table, pos);
		}
		StaticFunc::WriteInfo("BlockManager",
			std::format("Allocate block id:[{}] to table:[{}] success",
				std::to_string(newBlock), table->getName()));
		return newBlock;
	}
	unsigned BlockManager::allocateWithOutRead(TablePtr table, int pos)
	{
		auto newBlock = allocate();

		if (pos == -1)
		{
			m_blocks[newBlock].assign(table, table->getOwned().size()* Util::Setting::Get().getBlockSize());
			table->getOwned().emplace_back(newBlock);
		}
		else
		{
			m_blocks[newBlock].assign(table, pos);
			table->getOwned()[pos / StaticFunc::MaxSizeForBlock(table->totalByte())] = newBlock;
		}
		StaticFunc::WriteInfo("BlockManager",
			std::format("Allocate block id:[{}] to table:[{}] success",
				std::to_string(newBlock), table->getName()));
		return newBlock;
	}
	void BlockManager::promote(unsigned pos)
	{
		auto iter = std::ranges::find(m_active.begin(), m_active.end(), pos);
		if (iter != m_active.end())
		{
			m_active.erase(iter);
			m_active.push_back(pos);
		}
	}
	void BlockManager::handle(Transaction& transection)
	{
		if (transection.isValid())
		{
			transection.setInvalid();
			auto memory = transection.getMemory();
			int position = transection.getBlockId();
			auto table = TableManager::Get().get_noLock(transection.getTable());
			unsigned location = transection.getLocation();
			if (transection.getType() == SQLType::Insert)
			{
				std::pair<unsigned, std::shared_ptr<char[]>> result = std::make_pair(transection.size(), std::move(memory));
				if (position == -1)
				{
					position = allocate();
					transection.setBlockId(position);
					{
						m_blocks[position].assign(table, table->size());
					}
					table->addBlock(position);
				}
				result = m_blocks[position].insert(result.second, result.first);
				table->incrementSize();
				TableManager::Get().update();
			}
			else if (transection.getType() == SQLType::Update)
			{
				if (position == -1)
				{
					Util::ErrorManager::Get().set(Util::ErrorLevel::Error, "BlockIdError", "Given block id is incorrect.", Infor::ClassInfor(THISFUNC, THISFILE));
					return;
				}
				m_blocks[position].update(location, memory);
			}
			else if (transection.getType() == SQLType::Delete)
			{
				if (position == -1)
				{
					Util::ErrorManager::Get().set(Util::ErrorLevel::Error, "BlockIdError", "Given block id is incorrect.", Infor::ClassInfor(THISFUNC, THISFILE));
					return;
				}
				m_blocks[position].remove(location);
				table->decrementSize(location);
				if (m_blocks[position].percentage() < 0.5)
				{
					rearrange(table->getOwned(), table->getAvailable());
				}
				TableManager::Get().update();
			}

		}
		
	}
	unsigned BlockManager::totalRecord(unsigned bytes)
	{
		return Util::Setting::Get().getBlockSize() / bytes;
	}
	void BlockManager::operate(Transaction& transection)
	{
		WriteLock lock(m_mutex);
		m_pendingOperations.emplace_back(std::move(transection));
		m_notify = true;
		m_cv.notify_all();
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
		if (!m_active.empty())
		{
			WriteLock lock(m_mutex);
			m_active.erase(std::find(m_active.begin(), m_active.end(), pos));
			m_active.push_back(pos);
			m_blocks[pos].drop(pos);
		}
	}
	Block& BlockManager::get_noLock(unsigned pos)
	{
        WriteLock lock(m_mutex);
		m_active.erase(std::find(m_active.begin(), m_active.end(), pos));
		m_active.push_back(pos);
		return m_blocks[pos];
	}
	ShareMutex& BlockManager::getMutex()
	{
		return m_mutex;
	}
	std::future<bool> BlockManager::wait()
	{
		return m_finished.get_future();
	}
	BlockManager::~BlockManager()
	{
		m_end = true;
		m_notify = true;
		m_cv.notify_all();
		if (m_terminate.valid())
			m_terminate.get();
		m_blocks.clear();
		m_finished.set_value(true);
	}
}
