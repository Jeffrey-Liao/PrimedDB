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
		:m_count(-1),m_id(StaticFunc::GetUniqueId(Setting::Get().getUserIDHashType()))
	{
		allocate();
	}
	Block::Block(Block&& move) noexcept
		:m_memory(move.m_memory), m_count(-1),m_id(std::move(move.m_id)),m_owner(std::move(move.m_owner)),m_records(std::move(move.m_records))
	{
		move.m_memory = nullptr;
		move.m_id = StaticFunc::GetUniqueId(Setting::Get().getUserIDHashType());
	}
	void Block::assign(TablePtr owner, unsigned pos)
	{
		int value = Setting::Get().getBlockSize() / owner->totalByte();
		{
			WriteLock lock(m_mutex);
			m_owner = owner;
			m_start = pos;
			m_count = -1;
			m_records.clear();
			m_records.reserve(value);
			memset(m_memory, 0, Setting::Get().getBlockSize());
			build();
		}
		int size = this->size();
		WriteLock lock(m_mutex);
		m_pointer = size;

	}
	//check two Blocks are same object or not
	bool Block::same(const Block& object) const
	{
		ReadLock lock(m_mutex);
		return m_id == object.m_id;
	}
	//compare content in memory
	bool Block::equal(const Block& object)
	{
		ReadLock lock(m_mutex);
		int cmpSize = min(size(), static_cast<unsigned>(object.m_count));
		return std::memcmp(m_memory, object.m_memory, cmpSize)==0;
	}
	unsigned Block::max() const
	{
		return Setting::Get().getBlockSize()/m_owner->totalByte();
	}
	void Block::writeLine(UCharPtr buffer, unsigned pos)
	{
		if (pos<max())
		{
			
		}
	}
	void Block::flush()
	{
		if (!empty())
		{
			 {
				WriteLock lock(m_mutex);
				fstream file;
				auto dir = Setting::Get().getDataDirectory() / (m_owner->getName() + ".dat");
				file.open(dir, ios::out | ios::binary);
				file.seekg(m_start*m_owner->totalByte());
				int size = m_records.size() * m_owner->totalByte();
				file.write(m_memory, size);
				file.close();
			}
			
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
        move.m_memory = nullptr;
        move.m_id = StaticFunc::GetUniqueId(Setting::Get().getUserIDHashType());
		return *this;
	}
	bool Block::empty()
	{
		ReadLock lock(m_mutex);
		return size() < 0;
	}
	unsigned int Block::size()
	{
		ReadLock lock(m_mutex);
		if (m_count == -1)
		{
			auto end = max() + m_start;
			auto& available = m_owner->getAvailable();
			m_count = 0;
			for (int n = m_start;n< available.size()&&n < end;++n)
			{
				if (available[n])
					m_count++;
			}
		}
		return m_count;
	}
	char* Block::reference()
	{
		return m_memory;
	}

	unsigned Block::byte() const
	{
		ReadLock lock(m_mutex);
		return m_owner->totalByte();
	}
	void Block::build()
	{
		if (m_records.empty()&&m_owner != nullptr)
		{
			auto max = Setting::Get().getBlockSize();
			for (int n = 0;n < max;n+=m_owner->totalByte())
			{
				m_records.emplace_back(m_memory + n);
			}
			m_pointer = m_owner->getAvailable().size();
		}
	}
	TablePtr Block::getOwner()
	{
		ReadLock lock(m_mutex);
        return m_owner;
	}
	void Block::remove(unsigned index)
	{
		WriteLock lock(m_mutex);
		m_owner->setUnavailable(m_start + index);
		m_count = -1;
	}
	std::pair<unsigned, std::shared_ptr<char[]>> Block::insert(std::shared_ptr<char[]> memory, unsigned number)
	{
		if (!memory) {
			return { 0, nullptr };
		}

		unsigned l_byte = byte();
		unsigned capacity = Setting::Get().getBlockSize() / l_byte;
		unsigned left = capacity - size() < 0 ? 0: capacity - size();

		// 边界检查防止越界写入
		if (number < 0 || size() > capacity) {
			ErrorManager::Get().set(ErrorLevel::Error, "BlockInsert", std::format("Write Out bound where size is:{}", size()));
			return { static_cast<unsigned>(number), std::move(memory) };
		}

		unsigned int overflow = 0;
		if (number > left) {
			overflow = static_cast<unsigned>(number - left);
		}

		char* mem = memory.get();

		{
			WriteLock lock(m_mutex);
			size_t size = std::min(number, left);
			size_t copy_bytes = static_cast<size_t>(l_byte) * size;
			if (mem != nullptr)
				memcpy_s(m_memory + m_pointer * l_byte, copy_bytes, mem, copy_bytes);

			// 如果有 overflow 数据需要保留，将其前移到 memory 开头
			if (overflow > 0 && overflow <= static_cast<unsigned>(number)) {
				size_t move_offset = static_cast<size_t>(l_byte) * (number - overflow);
				size_t move_bytes = static_cast<size_t>(l_byte) * overflow;
				memmove(mem, mem + move_offset, move_bytes);  // 安全地重叠区域移动
			}
			StaticFunc::WriteInfo("BlockInsert", std::format("Write all data into block memory position {} success", m_pointer));
		}
		m_pointer += number;
		m_count = -1;
		StaticFunc::WriteInfo("BlockInsert", std::format("The occupied size of block is: {}", size()));
		this->flush();
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
	double Block::percentage()
	{
		ReadLock lock(m_mutex);
		return size() / max();
	}
	ShareMutex& Block::getMutex()
	{
		return m_mutex;
	}
	std::vector<char*>& Block::getRecords()
	{
		return m_records;
	}
	Block::~Block()
	{
		deallocate();
	}
}
