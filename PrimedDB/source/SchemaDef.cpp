#include "Schema.h"
#include "User.h"

namespace liao::PrimedDB
{
	const std::string& Schema::getId() const
	{
		return m_id;
	}
	const std::string& Schema::getName() const
	{
		return m_name;
	}
	void Schema::rename(std::string& name)
	{
		m_name = std::move(name);
	}
	const std::vector<TablePtr>& Schema::getTables() const
	{
		return m_tables;
	}
	auto Schema::find(const std::string& name) const
	{
		for (auto iter = m_tables.begin(); iter != m_tables.end(); ++iter)
		{
			if ((*iter)->getName() == name)
			{
				return iter;
			}
		}
		return m_tables.end();
	}
	auto Schema::find(const std::string& name)
	{
		for (auto iter = m_tables.begin(); iter != m_tables.end(); ++iter)
		{
			if ((*iter)->getName() == name)
			{
				return iter;
			}
		}
		return m_tables.end();
	}
	const Table& Schema::getTable(const std::string& name)const
	{
		auto iter = find(name);
		if (iter != m_tables.end())
		{
			return **iter;
		}
		return Table::GetNullRef();
	}
	void Schema::addTable(std::string& name, User& owner, UserLevel permission)
	{
		m_tables.emplace_back(std::make_shared<Table>(name, owner, permission));
	}
	void Schema::removeTable(std::string& name, User& user)
	{
		auto iter = find(name);
		if (iter != m_tables.end())
		{
			UserLevel requiredLevel = (*iter)->getPermission();
			if (user.qualified(requiredLevel))
				m_tables.erase(iter);
		}
	}
	Table& Schema::getTable(const std::string& name, const User& user)
	{
		auto iter = find(name);
        if (iter != m_tables.end())
		{
			UserLevel requiredLevel = (*iter)->getPermission();
			if (user.qualified(requiredLevel))
				return **iter;
		}
		return Table::GetNullRef();
	}
}
