#include "BlockManager.h"
#include "Setting.h"
#include "User.h"
#include "UserManager.h"

USECRPT;
USESTD;
USELIAOMATH;
namespace liao::PrimedDB
{
	void User::constructFromFile(const std::string& fileLine)
	{
		vector<string> user_table;
		StaticFunc::Split(user_table, fileLine, '|');
		if (user_table.empty())
		{
			Util::ErrorManager::Get().set(Util::ErrorLevel::Fatal, "FileError", "Given file line is incorrect. The structure file might corrupted.", Infor::ClassInfor(THISFUNC, THISFILE));
		}
		string& user = user_table[0];
		vector<string> userInfor;
		vector<string> tables;
		StaticFunc::Split(userInfor, user, ':');

		m_name = userInfor[0];
		m_id = userInfor[1];
		m_password = userInfor[2];
		m_level = static_cast<UserLevel>(userInfor[3][0]);
		if (user_table.size() > 1)
		{
			StaticFunc::Split(tables, user_table[1], ':');
			for (auto& t : tables)
			{
				m_tables[t] = TableManager::Get().get_noLock(t);
			}
		}
	}
	User::User(const string& fileLine)
	{
		constructFromFile(fileLine);
	}
	User::User(string& id, string& name, string& password, UserLevel level)
		:m_name(std::move(name)), m_level(level), m_id(std::move(id)),m_password(PassWordHash(password))
	{
		m_tables.reserve(5);
	}
	User::User(string& name, string& password, UserLevel level)
		:m_name(std::move(name)), m_level(level),m_id(StaticFunc::GetUniqueId(Util::Setting::Get().getUserIDHashType())), m_password(PassWordHash(password))
	{
		m_tables.reserve(5);
	}
	User::User(User&& object) noexcept
		:m_name(std::move(object.m_name)), m_password(std::move(object.m_password)), m_level(object.m_level), m_id(std::move(object.m_id)),m_tables(std::move(object.m_tables))
	{
	}
	User::~User()
	{
		if (m_name != "system")
		{
			for (auto& table : m_tables)
			{
				TableManager::transfer(*this, table.second, UserManager::Get().GetSystemUser());
			}
		}
	}
	User User::createUser(std::string name, std::string password, UserLevel level,string id)
	{
		if (id.empty())
			return User(name, password, level);
		else
			return User(id, name, password, level);
	}
	
	const string& User::getName()const
	{
		ReadLock lock(m_mutex);
		return m_name;
	}
	const string& User::getPassword()const
	{
		ReadLock lock(m_mutex);
		return m_password;
	}

	UserLevel User::getLevel()const
	{
		ReadLock lock(m_mutex);
		return m_level;
	}
	const string& User::getId()const
	{
		ReadLock lock(m_mutex);
		return m_id;
	}
	int User::compare(const User& other) const
	{
		int value = static_cast<int>(other.getLevel());
		ReadLock lock(m_mutex);
		return static_cast<int>(m_level) - value;
	}
	bool User::qualified(UserLevel level)const
	{
		ReadLock lock(m_mutex);
		return m_level >= level;
	}
	bool User::validate(const std::string& password) const
	{
		string temp = PassWordHash(password);
		return validateWithHash(temp);
	}
	bool User::validateWithHash(const std::string& password) const
	{
		ReadLock lock(m_mutex);
		return m_password == password;
	}
	bool User::rename(string& name)
	{
		WriteLock lock(m_mutex);
		if (name == "null" || UserManager::Get().exist(name))
			return false;
		m_name = std::move(name);
		return true;
	}
	const std::unordered_map<std::string, TablePtr>& User::getTables()const
	{
		ReadLock lock(m_mutex);
		return m_tables;
	}
	TablePtr User::getTable(const string& name)const
	{
		ReadLock lock(m_mutex);
		if (m_tables.contains(name))
			return m_tables.at(name);
		else
			return nullptr;
	}
	unsigned User::tableCount() const
	{
		ReadLock lock(m_mutex);
		return m_tables.size();
	}
	//1 GREATER, 0 EQUAL, -1 LESS
	

	TablePtr User::createTable(std::string name, UserLevel permission, deque<Column>& columns)
	{
		bool contains;
		{
			ReadLock lock(m_mutex);
			contains = m_tables.contains(name);
		}
		if (!contains)
		{
			auto opt = TableManager::Get().add(*this, name, columns);
			if (opt)
			{
				WriteLock lock(m_mutex);
				m_tables[name] = opt;
				return opt;
			}
		}
		return nullptr;
	}
	
	void User::dropTable(const string& name)
	{
		WriteLock lock(m_mutex);
		m_tables.erase(name);
	}
	void User::renameTable(const string& name, string&  newName)
	{
		if (m_tables.contains(name))
		{
			m_tables[name]->rename(newName);
		}
	}
	void User::addTable(TablePtr table)
	{
		WriteLock lock(m_mutex);
		if (table)
		{
			m_tables[table->getName()] = table;
		}
	}
	void User::changePassword(const string& rawText)
	{
		WriteLock lock(m_mutex);
		m_password = std::move(PassWordHash(rawText));
	}
	void User::setPassword(string& hash)
	{
		WriteLock lock(m_mutex);
		m_password = std::move(hash);
	}
	std::string User::PassWordHash(const std::string& rawText)
	{
		HashContainer container;
		container.generate(rawText);
		return container.getHashHex();
	}
	
	void User::setLevel(UserLevel level)
	{
		WriteLock lock(m_mutex);
		m_level = level;
	}
	void User::submit(Transection&& operation)
	{
		WriteLock lock(m_mutex);
		m_pendingOperations.emplace_back(std::move(operation));
	}
	void User::commit()
	{
		WriteLock lock(m_mutex);
		while (!m_pendingOperations.empty())
		{
			BlockManager::Get().operate(m_pendingOperations.front());
			m_pendingOperations.pop_front();
		}
	}
	string User::toString() const
	{
		std::ostringstream oss;
		ReadLock lock(m_mutex);
		oss << format("{}:{}:{}:{}|",  m_name, m_id, m_password, static_cast<int>(m_level));
		for (auto&  table : m_tables)
		{
			oss << table.first<<":";
		}
		string str = std::move(oss.str());
		if (!m_tables.empty())
			str.pop_back();
		return str;
	}
	TablePtr User::operator[](const string&  name)
	{
		return getTable(name);
	}
	User& User::operator=(User& user)
	{
		WriteLock lock(m_mutex);
		m_id = user.m_id;
        m_level = user.m_level;
        m_name = user.m_name;
        m_password = user.m_password;
		m_tables = user.m_tables;
		return *this;
	}
	bool User::operator==(const User& object) const
	{
		ReadLock lock(m_mutex);
		return m_id == object.getId();
	}
}
