#include "Schema.h"
#include "User.h"
#include "UserManager.h"
USESTD;
namespace liao::PrimedDB
{
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
	const TablePtr Schema::getTable(const std::string& name)const
	{
		auto iter = find(name);
		if (iter != m_tables.end())
		{
			return *iter;
		}
		return nullptr;
	}
	void Schema::addTable(std::string& name, User& owner, UserLevel permission)
	{
		//m_tables.emplace_back(std::make_shared<Table>(owner, name, permission));
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
	TablePtr Schema::getTable(const std::string& name, const User& user)
	{
		auto iter = find(name);
        if (iter != m_tables.end())
		{
			UserLevel requiredLevel = (*iter)->getPermission();
			if (user.qualified(requiredLevel))
				return *iter;
		}
		return nullptr;
	}
	void Schema::constructTable(const std::string& fileLine)
	{
		vector<string> tokens;
        StaticFunc::Split(tokens, fileLine, ':');
		UserPtr ptr = UserManager::Get().get(tokens[0]);
		User& ref = *ptr;
		m_tables.emplace_back(make_shared<Table>(Table(ref, tokens[1])));
	}
	void Schema::construct(const std::string& fileLine)
	{
		vector<string> tokens;
        StaticFunc::Split(tokens, fileLine, '|');
		WriteLock lock(m_mutex);
        m_name = tokens[0];
		string tableInfor = std::move(tokens[1]);
		tokens.clear();
        StaticFunc::Split(tokens, tableInfor, ',');
		for (auto& token : tokens)
		{
			constructTable(token);
		}
	}
	std::string Schema::toString() const
	{
		std::ostringstream oss;
		ReadLock lock(m_mutex);
		oss << format("{}|", m_name);
		for (int n = 0; n < m_tables.size(); ++n)
		{
			if (n + 1 < m_tables.size())
			{
				oss << ",";
			}
			oss <<m_tables[n]->getOwner().getName()<<":"<< m_tables[n]->getName();
		}
		return oss.str();
	}

}
