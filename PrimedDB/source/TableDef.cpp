#include "Table.h"
#include "User.h"

namespace liao::PrimedDB
{
	Table Table::NullRef;
	bool Table::isNullObject(const Table& object)
	{
		return User::isNullObject(object.m_owner);
	}
	Table::Table()
		:m_name("null"),m_permission(UserLevel::None),m_owner(User::NullRef),m_recordNumber(-1)
	{}
	Table::Table(std::string& name, User& owner, UserLevel permission)
		:m_owner(owner),m_permission(permission)
	{
		rename(name);
	}
	const std::string& Table::getName() const
	{
		return m_name;
	}
	void Table::addColumn(std::string& name, short int byteSize)
	{
		m_columns.emplace_back(Column(name, byteSize, *this));
	}
	void Table::rename(std::string& name)
	{
		if (name != "null")
			m_name = std::move(name);
	}
	void Table::dropColumn(const std::string& name)
	{
		auto iter = findColumn(name);
		if (iter != m_columns.end())
        {
            m_columns.erase(iter);
        }
	}
	auto Table::findColumn(const std::string& name) 
	{
		for (auto iter = m_columns.begin(); iter != m_columns.end(); ++iter)
		{
			if ((*iter).getName() == name)
			{
				return iter;
			}
		}
		return m_columns.end();
	}
	auto Table::findColumn(const std::string& name) const
	{
		for (auto iter = m_columns.begin(); iter != m_columns.end(); ++iter)
		{
			if ((*iter).getName() == name)
			{
				return iter;
			}
		}
		return m_columns.end();
	}
	void Table::removeColumn(const std::string& name)
	{
		auto iter = findColumn(name);
		if (iter != m_columns.end())
		{
            m_columns.erase(iter);
		}
	}
	void Table::resizeColumn(const std::string& name, short int byteSize)
	{
		auto iter = findColumn(name);
		Column& column = *(iter);
		if (iter != m_columns.end())
		{
			column.resize(byteSize);
		}
	}
	const Column& Table::getColumn(const std::string& name) const
	{
		auto iter = findColumn(name);
		if (iter != m_columns.end())
		{
			return *iter;
		}
	}
	const std::vector<Column>& Table::getColumns() const
	{
		return m_columns;
	}
	int Table::getRecordNumber() const
	{
		return m_recordNumber;
	}
	void Table::increaseRecordNumber()
	{
		m_recordNumber++;
	}
	User& Table::getOwner() const
	{
		return m_owner;
	}
	void Table::clear()
	{
		m_columns.clear();
	}
	Table::~Table()
	{
		clear();
	}
}
