#include "Block.h"
USELIAOUTIL;
USESTD;
namespace liao::PrimedDB
{
	void Block::allocate(const std::shared_ptr<char> source, unsigned size)
	{
		WriteLock lock(m_mutex);
		if (m_memory == nullptr)
		{
			m_memory = new char[Util::Setting::Get().getBlockSize()];
		}
		if (source != nullptr)
			memcpy_s(m_memory, Util::Setting::Get().getBlockSize(), source.get(), size);
		else
			memset(m_memory, 0, Util::Setting::Get().getBlockSize());
	}
	void Block::deallocate()
	{
		WriteLock lock(m_mutex);
		if (m_memory != nullptr)
            delete[] m_memory;
		m_memory = nullptr;
	}
	Block::Block()
		:m_size(0),m_id(StaticFunc::GetUniqueId(Setting::Get().getUserIDHashType()))
	{
		allocate();
	}
	Block::Block(Block&& move) noexcept
		:m_memory(move.m_memory),m_size(move.m_size),m_id(std::move(move.m_id)),m_owner(std::move(move.m_owner))
	{
		move.m_memory = nullptr;
		move.m_id = StaticFunc::GetUniqueId(Setting::Get().getUserIDHashType());
	}
	void Block::assign(TablePtr owner, unsigned pos)
	{
		{
			WriteLock lock(m_mutex);
			m_owner = owner;
			m_start = pos;
			m_size = 0;
			read();
		}
	}
	//check two Blocks are same object or not
	bool Block::same(const Block& object) const
	{
		ReadLock lock(m_mutex);
		return m_id == object.m_id;
	}
	//compare content in memory
	bool Block::equal(const Block& object) const
	{
		ReadLock lock(m_mutex);
		int cmpSize = min(m_size, object.m_size);
		return std::memcmp(m_memory, object.m_memory, cmpSize)==0;
	}
	unsigned Block::max() const
	{
		ReadLock lock(m_mutex);
		return Setting::Get().getBlockSize()/m_owner->totalByte();
	}
	void Block::read()
	{
		if (!empty())
		{
			fstream file;
			{
				WriteLock lock(m_mutex);
				file.open(Setting::Get().getDataDirectory() / (m_owner->getName() + ".dat"), ios::in | ios::binary);
				file.seekg(m_start);
				while (m_size < max())
				{
					file.getline(m_memory, Setting::Get().getBlockSize());
					m_size++;
					if (max() - m_size < m_owner->totalByte())
						break;
				}

			}
            file.close();
		}
	}
	void Block::flush()
	{
		if (!empty())
		{
			fstream file; {
				WriteLock lock(m_mutex);
				auto dir = Setting::Get().getDataDirectory() / (m_owner->getName() + ".dat");
				file.open(dir, ios::out | ios::binary);
				file.seekg(m_start*m_owner->totalByte());
				file.write(m_memory, m_size * m_owner->totalByte());
			}
			file.close();
		}
	}
	void Block::update(unsigned location, std::shared_ptr<char[]> memory)
	{
		WriteLock lock(m_mutex);
        memcpy_s(m_memory + location * m_owner->totalByte(), m_owner->totalByte(), memory.get(), m_owner->totalByte());
	}
	bool Block::operator==(const Block& object) const
	{
		return same(object);
	}
	Block& Block::operator=(Block&& move) noexcept
	{
		m_owner = std::move(move.m_owner);
		m_id = std::move(move.m_id);
        m_memory = move.m_memory;
        m_size = move.m_size;
        move.m_memory = nullptr;
        move.m_id = StaticFunc::GetUniqueId(Setting::Get().getUserIDHashType());
		return *this;
	}
	bool Block::empty() const
	{
		ReadLock lock(m_mutex);
		return m_memory == nullptr||m_owner == nullptr;
	}
	unsigned int Block::size() const
	{
		ReadLock lock(m_mutex);
		return m_size;
	}
	char* Block::reference()
	{
		ReadLock lock(m_mutex);
		return m_memory;
	}

	unsigned Block::byte() const
	{
		ReadLock lock(m_mutex);
		return m_owner->totalByte();
	}
	void Block::remove(unsigned index) const
	{
		WriteLock lock(m_mutex);
		m_owner->setUnavailable(m_start + index);
		--m_size;
	}
	std::pair<unsigned, std::shared_ptr<char[]>> Block::insert(std::shared_ptr<char[]> memory, int number)
	{
		unsigned l_byte = byte();
		int left = Setting::Get().getBlockNumber() / l_byte - m_size;
		unsigned int overflow = 0;
		if (number>left)
		{
			overflow = number - left;
		}

		char* mem = memory.get();
		{
			WriteLock lock(m_mutex);
			memcpy_s(m_memory + m_size * l_byte, l_byte * number, memory.get(), l_byte * number);
			m_size++;
			if (overflow != 0)
				memcpy_s(memory.get(), l_byte * overflow, memory.get() + l_byte * (number - overflow), l_byte * overflow);
		}
		flush();
		return std::make_pair(overflow, std::move(memory));
	}
	
	char* Block::get_noLock(unsigned index)
	{
		return m_memory + index * byte();
	}
	void Block::drop(unsigned index)
	{
		WriteLock lock(m_mutex);
		m_owner->dropBlock(index);
		m_owner = nullptr;
	}
	double Block::percentage() const
	{
		ReadLock lock(m_mutex);
		return m_size / max();
	}
	ShareMutex& Block::getMutex()
	{
		return m_mutex;
	}
	Block::~Block()
	{
		deallocate();
	}
}
