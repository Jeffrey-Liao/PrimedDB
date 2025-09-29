#include "User.h"
#include "UserManager.h"
USECRPT;
USESTD;
USELIAOMATH;
namespace liao::PrimedDB
{
	void User::linkTables(const vector<string>& line)
	{
		int start = 4;
		if (line.size() > start)
		{
			for (;start<line.size(); ++start)
			{
				WriteLock lock(m_mutex);
				m_tables;
			}
		}
	}
	int User::findTable(const std::string& name) const
	{
		ReadLock lock(m_mutex);
		for (int n = 0;n<m_tables.size();++n)
		{
			if (m_tables[n]->getName() == name)
			{
				return n;
			}
		}
		return -1;
	}
	User::User(const string& fileLine)
	{
		vector<string> user_table(2);
		vector<string> userInfor(5);
		StaticFunc::Split(user_table, fileLine, '|');
		const string& userStr= user_table[0];
		const string& tableStr = user_table[1];
		StaticFunc::Split(userInfor, userStr, ' ');
		if (userInfor.size() >= 4)
		{
			int n = 0;
			m_id = userInfor[n++];
			m_name = userInfor[n++];
			m_password = userInfor[n++];
			m_level = static_cast<UserLevel>(stoi(userInfor[n]));
			linkTables(userInfor);
		}
	}
	User::User(string& name, string& password, UserLevel level)
		:m_name(std::move(name)), m_level(level),m_id(StaticFunc::GetUniqueId(Configuration::UserIDHashType))
	{
		changePassword(password);
		m_tables.reserve(5);
	}
	User::User(User&& object) noexcept
		:m_name(std::move(object.m_name)), m_password(std::move(object.m_password)), m_level(object.m_level), m_id(std::move(object.m_id)),m_tables(std::move(object.m_tables))
	{
	}
	User User::createUser(std::string name, std::string password, UserLevel level)
	{
		return User(name, password, level);
	}
	User::~User()
	{
		if (!m_tables.empty())
		{
			for (int n = 0; n < m_tables.size(); ++n)
			{
				m_tables[n].reset();
			}
		}
		m_tables.clear();
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
	const vector<TablePtr>& User::getTables()const
	{
		ReadLock lock(m_mutex);
		return m_tables;
	}
	TablePtr User::getTable(const string& name)
	{
		int index = findTable(name);
		if ( index != -1)
		{
			ReadLock lock(m_mutex);
			return m_tables[index];
		}
		return nullptr;
	}
	const TablePtr User::getTable(const string& name) const
	{
		int index = findTable(name);
		if (index != -1)
		{
			ReadLock lock(m_mutex);
			return m_tables[index];
		}
		return nullptr;
	}
	int User::tableCount() const
	{
		ReadLock lock(m_mutex);
		return m_tables.size();
	}
	//1 GREATER, 0 EQUAL, -1 LESS
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

	TablePtr User::createTable(std::string schema, std::string name, UserLevel permission)
	{
		{
			ReadLock lock(m_mutex);
			if (findTable(name) != -1||name.empty())
			{
				return nullptr;
			}
		}
		WriteLock lock(m_mutex);
		m_tables.emplace_back(new Table(*this, name,permission));
		return m_tables[m_tables.size()-1];
	}
	bool User::rename(string& name)
	{
		WriteLock lock(m_mutex);
		if (name == "null"|| UserManager::Get().exist(name))
			return false;
		m_name = std::move(name);
		return true;
	}
	void User::dropTable(const string& name)
	{
		int index = findTable(name);
		WriteLock lock(m_mutex);
		m_tables.erase(m_tables.begin() + index);

	}
	void User::renameTable(const string&  name, string&  newName)
	{
		int index = findTable(name);
		WriteLock lock(m_mutex);
		m_tables[index]->rename(newName);
	}
	void User::changePassword(const string& rawText)
	{
		WriteLock lock(m_mutex);
		m_password = PassWordHash(rawText);
	}
	void User::setPassword(string&  hash)
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
	bool User::validate(const std::string& password) const
	{
		ReadLock lock(m_mutex);
		return m_password == password;
	}
	void User::setLevel(UserLevel level)
	{
		WriteLock lock(m_mutex);
		m_level = level;
	}
	string User::toString() const
	{
		std::ostringstream oss;
		ReadLock lock(m_mutex);
		oss << format("{} {} {} {}|", m_id, m_name, m_password, static_cast<int>(m_level));
		for (int n = 0; n < m_tables.size(); ++n)
		{
			if (n + 1 <= m_tables.size()&& n != 0)
				oss << ',';
			oss << m_tables[n]->getName();
		}
		return oss.str();
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
