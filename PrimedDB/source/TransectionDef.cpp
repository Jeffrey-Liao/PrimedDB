#include "Setting.h"
#include "Transection.h"
#include "UserManager.h"

namespace liao::PrimedDB
{
	Transection::Transection()
		:m_id(), m_table(), m_operator(""), m_operation(SQLType::None), m_blockId(-1), m_location(0), m_size(0), m_memory(nullptr), m_valid(false)
	{}
	Transection::Transection(const std::string& oprtor, const std::string& name, SQLType operation, int blockId, int location, unsigned size, UCharPtr memory)
		:m_id(StaticFunc::GetUniqueId(Math::HashType::MD5)),m_table(name), m_operator(oprtor), m_operation(operation), m_blockId(blockId), m_location(location), m_size(size), m_memory(memory.release()), m_valid(true)
	{}
	Transection::Transection(const std::string& oprtor, const std::string& name,const std::string& str, bool neg)
		:m_table(name),m_operator(oprtor)
	{
		if (neg)
			negFromString(str);
		else
            fromString(str);
	}
	Transection::Transection(const std::string& oprtor, const std::string& name, bool neg)
		:m_table(name), m_operator(oprtor)
	{
		std::string buffer;
		std::fstream transections(Util::Setting::Get().getDataDirectory()/ (m_table + ".trs"), std::ios::app);
		std::getline(transections, buffer);
		if (neg)
			negFromString(buffer);
		else
			fromString(buffer);
		transections.close();
	}
	Transection::Transection(Transection&& move) noexcept
		:m_id(std::move(move.m_id)), m_table(move.m_table), m_operator(std::move(move.m_operator)), m_operation(move.m_operation), m_blockId(move.m_blockId), m_location(move.m_location), m_size(move.m_size), m_memory(std::move(move.m_memory)), m_valid(move.m_valid)
	{}
	void Transection::operator=(Transection&& move)noexcept
	{
		m_id = std::move(move.m_id);
		m_table = move.m_table;
		m_operator = std::move(move.m_operator);
		m_operation = move.m_operation;
		m_blockId = move.m_blockId;
		m_location = move.m_location;
		m_size = move.m_size;
		m_memory = std::move(move.m_memory);
		m_valid = move.m_valid;
	}
	SQLType Transection::getType() const
	{
		return m_operation;
	}
	const std::string& Transection::getOperator()const
	{
		return m_operator;
	}
	const std::string& Transection::getTable() const
	{
		return m_table;
	}
	int Transection::getBlockId() const
	{
		return m_blockId;
	}
	void Transection::setBlockId(int newBlockId)
	{
		m_blockId = newBlockId;
	}
	int Transection::getLocation() const
	{
		return m_location;
	}
	unsigned Transection::size() const
	{
		return m_size;
	}
	bool Transection::isValid() const
	{
		return m_valid;
	}
	SCharPtr Transection::getMemory()
	{
		return m_memory;
	}
	void Transection::setValid()
	{
		m_valid = true;
	}
	void Transection::setInvalid()
	{
		m_valid = false;
	}
	std::string Transection::toString() const
	{
		if (m_valid)
			return std::format("{}:{}:{}:{}:{}:{}:{}"
			,m_id,m_operator,static_cast<int>(m_operation),m_blockId,m_location,m_size,m_memory.get());
		return "";
	}
	void Transection::fromString(const std::string& str)
	{
		std::vector<std::string> vec;
		StaticFunc::Split(vec, str, ':');
		if (vec.size() == 7)
		{
			m_id = vec[0];
			m_operator = UserManager::Get().get(vec[1])->getId();
			m_operation = static_cast<SQLType>(std::stoi(vec[2]));
			m_blockId = std::stoi(vec[3]);
			m_location = std::stoi(vec[4]);
			m_size = std::stoi(vec[5]);
			m_memory = std::shared_ptr<char[]>(new char[m_size]);
			memcpy(m_memory.get(), vec[6].c_str(), m_size);
		}
	}
	void Transection::negFromString(const std::string& str)
	{
		std::vector<std::string> vec;
		StaticFunc::Split(vec, str, ':');
		if (vec.size() == 7)
		{
			m_id = vec[0];
			m_operator = vec[1];
			m_operation = static_cast<SQLType>(std::stoi(vec[2]));
			if (m_operation == SQLType::Delete)
				m_operation = SQLType::Insert;
			else if (m_operation == SQLType::Insert)
                m_operation = SQLType::Delete;
			m_blockId = std::stoi(vec[3]);
			m_location = std::stoi(vec[4]);
			m_size = std::stoi(vec[5]);
			m_memory = std::make_unique<char[]>(m_size);
			memcpy(m_memory.get(), vec[6].c_str(), m_size);
		}
	}
	Transection::~Transection()
	{
		if (!m_valid&&m_memory!= nullptr)
		{
			auto dir = Util::Setting::Get().getDataDirectory() / (m_table + ".trs");
			std::fstream transections(dir, std::ios::out| std::ios::app);
			transections << toString() << std::endl;
			transections.close();
		}
	}
}
