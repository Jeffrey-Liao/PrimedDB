#include "Block.h"
USELIAOUTIL;
USESTD;
namespace liao::PrimedDB
{
	Block::OwnerInfo::OwnerInfo(OwnerInfo&& move)
		:m_ownerName(std::move(move.m_ownerName)),m_available(std::move(move.m_available)),m_beginLine(move.m_beginLine)
	{}
	void Block::allocate(const std::shared_ptr<char> source, unsigned size)
	{
		
		if (m_memory == nullptr)
            m_memory = new char[Util::Setting::Get().getBlockSize()];
		if (source != nullptr)
			memcpy_s(m_memory, Util::Setting::Get().getBlockSize(), source.get(), size);
	}
	void Block::deallocate()
	{
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
	void Block::assign(std::shared_ptr<std::string>& ownerName, std::shared_ptr<std::vector<bool>>& ownerAvailableSet, unsigned lineNumber)
	{
		m_owner.m_available = ownerAvailableSet;
        m_owner.m_ownerName = ownerName;
        m_owner.m_beginLine = lineNumber;
	}
	void Block::resize()
	{
		char* ptr = new char[Setting::Get().getBlockSize()];
		memcpy_s(ptr, Setting::Get().getBlockSize(), m_memory, m_size);
        deallocate();
        m_memory = ptr;
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
		return std::memcmp(m_memory,object.m_memory,cmpSize);
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
	//return success(true) fail(false)
	unsigned int Block::write(std::string& memory, unsigned byteSize)
	{
        return write(shared_ptr<char>(memory.data()), memory.size(), byteSize);
	}
	unsigned int Block::write(std::shared_ptr<char> memory, unsigned size, unsigned byteSize)
	{
		if (empty())
			allocate();
		int allowedNumber = Setting::Get().getBlockSize() / byteSize;
		int writeNumber = size / byteSize;
		int overflow = writeNumber - allowedNumber;
		overflow = overflow > 0 ? overflow : 0;
		char* ptr;
		{
			ReadLock lock(m_mutex);
			ptr = m_memory;
		}
		for (int n = 0; n < allowedNumber; ++n)
		{
			WriteLock lock(m_mutex);
			memcpy_s(ptr, byteSize, memory.get(), byteSize);
			ptr += byteSize;
		}
		{
			WriteLock lock(m_mutex);
			m_recordSize = allowedNumber;
			m_size = writeNumber * byteSize;
		}
		return overflow;
	}
	void Block::write(std::shared_ptr<std::fstream> file, unsigned byteSize)
	{
		if (empty())
			allocate();
		int allowedNumber = Setting::Get().getBlockSize() / byteSize;
		{
			WriteLock lock(m_mutex);
			file->getline(m_memory, allowedNumber * byteSize);
			m_size = file->gcount();
			m_recordSize = allowedNumber;
		}
	}
	unsigned Block::reocrdSize() const
	{
		return m_recordSize;
	}
	bool Block::empty() const
	{
		ReadLock lock(m_mutex);
		return m_memory == nullptr;
	}
	unsigned int Block::size() const
	{
		ReadLock lock(m_mutex);
		return m_size;
	}
	const char* Block::reference() const
	{
		ReadLock lock(m_mutex);
		return m_memory;
	}
	void Block::remove(unsigned index) const
	{
		(*m_owner.m_available)[m_owner.m_beginLine + index] = false;
	}
	bool Block::insert(unsigned byteSize, std::shared_ptr<char> memory)
	{
		if (m_size+byteSize > Setting::Get().getBlockSize())
			return false;
		char* ptr = m_memory + m_size;
        memcpy_s(ptr, byteSize, memory.get(), byteSize);
		return true;
	}
	void Block::modify(unsigned index, unsigned byteSize, std::shared_ptr<char>& memory, int size)
	{
		if (size < byteSize)
			ErrorManager::Get().set(ErrorLevel::Error, "InvalidArgument", "Given memory size is smaller than memory want to be get");
		WriteLock lock(m_mutex);
		char* ptr = m_memory + index * byteSize;
		memcpy_s(ptr, byteSize, memory.get(), byteSize);
	}
	const char* Block::get(unsigned index, unsigned byteSize)const
	{
		ReadLock lock(m_mutex);
		return m_memory + index * byteSize;
	}
	UCharPtr Block::release()
	{
		WriteLock lock(m_mutex);
		char* ptr= m_memory;
		m_memory = nullptr;
		return UCharPtr(ptr);
	}

	Block::~Block()
	{
		deallocate();
	}
}
