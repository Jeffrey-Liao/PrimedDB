#include "Table.h"
#include "User.h"
USESTD;
namespace liao::PrimedDB
{
	Table::Table()
		:m_id("null"),m_name("null"),m_permission(UserLevel::None),m_owner(User::NullRef),m_recordNumber(-1)
	{}
	Table::Table(std::string& name, User& owner, UserLevel permission)
		:m_id(StaticFunc::GetUniqueId()),m_owner(owner),m_permission(permission)
	{
		rename(name);
	}
	const std::string& Table::getName() const
	{
		ReadLock lock(m_mutex);
		return m_name;
	}
	void Table::addColumn(std::string& name, short int byteSize)
	{
		WriteLock lock(m_mutex);
		m_columns.emplace_back(name, byteSize, *this);
	}
	void Table::rename(std::string& name)
	{
		WriteLock lock(m_mutex);
		if (name != "null")
			m_name = std::move(name);
	}
	void Table::dropColumn(const std::string& name)
	{
		auto iter = findColumn(name);
		WriteLock lock(m_mutex);
		if (iter != m_columns.end())
        {
            m_columns.erase(iter);
        }
	}
	auto Table::findColumn(const std::string& name) 
	{
		ReadLock lock(m_mutex);
		for (auto iter = m_columns.begin(); iter != m_columns.end(); ++iter)
		{
			if (iter->getName() == name)
			{
				return iter;
			}
		}
		return m_columns.end();
	}
	auto Table::findColumn(const std::string& name) const
	{
		ReadLock lock(m_mutex);
		for (auto iter = m_columns.begin(); iter != m_columns.end(); ++iter)
		{
			if (iter->getName() == name)
			{
				return iter;
			}
		}
		return m_columns.end();
	}
	const string& Table::getId() const
	{
		return m_id;
	}
	void Table::removeColumn(const std::string& name)
	{
		auto iter = findColumn(name);
		WriteLock lock(m_mutex);
		if (iter != m_columns.end())
		{
            m_columns.erase(iter);
		}
	}
	void Table::resizeColumn(const std::string& name, short int byteSize)
	{
		auto iter = findColumn(name);
		Column& column = *(iter);
		ReadLock lock(m_mutex);
		if (iter != m_columns.end())
		{
			column.resize(byteSize);
		}
	}
	const Column& Table::getColumn(const std::string& name) const
	{
		auto iter = findColumn(name);
		ReadLock lock(m_mutex);
		if (iter != m_columns.end())
		{
			return *iter;
		}
		return Column::NullRef;
	}
	const std::vector<Column>& Table::getColumns() const
	{
		ReadLock lock(m_mutex);
		return m_columns;
	}
	int Table::getRecordNumber() const
	{
		ReadLock lock(m_mutex);
		return m_recordNumber;
	}
	void Table::increaseRecordNumber()
	{
        WriteLock lock(m_mutex);
		m_recordNumber++;
	}
	const User& Table::getOwner() const
	{
		ReadLock lock(m_mutex);
		return m_owner;
	}
	void Table::clear()
	{
		WriteLock lock(m_mutex);
		m_columns.clear();
	}
	int Table::size()const
	{
		return m_columns.size();
	}
	string Table::toString() const
	{
		std::ostringstream oss;
        ReadLock lock(m_mutex);
		oss<<format("id:{},name:{},permission{},owner:{},size:{}",m_id, m_name,static_cast<int>(m_permission),m_owner.getName(), this->size());
		for (int n =0;n<this->size();++n)
		{
			if (n+1<size())
			{
				oss << ",";
			}
			oss<<m_columns[n].toString();
		}
        return oss.str();
	}
	Table::~Table()
	{
		clear();
	}
}
