#include "Table.h"
#include "User.h"
USECRPT;
USESTD;
USELIAOMATH;
namespace liao::PrimedDB
{
	User::User()
		:m_name(), m_password(), m_level(UserLevel::None), m_id(StaticFunc::GetUniqueId(UserIDHashType)), m_tables()
	{
		m_password.reserve(Math::HashContainer::GetByteSize(UserIDHashType));
		m_tables.reserve(5);
	}
	void User::linkTables(const vector<string>& line)
	{
		WriteLock lock(m_mutex);
		int start = 4;
		if (line.size() > start)
		{
			for (;start<line.size();start++)
			{
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
		vector<string> userInfor(5);
		StaticFunc::Split(userInfor, fileLine, ',');
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
		:m_name(move(name)), m_password(move(password)), m_level(level),m_id(StaticFunc::GetUniqueId(UserIDHashType))
	{
		m_tables.reserve(5);
	}
	User::User(User&& object) noexcept
		:m_name(move(object.m_name)), m_password(move(object.m_password)), m_level(object.m_level), m_id(move(object.m_id)),m_tables(move(object.m_tables))
	{

	}
	User::~User()
	{
		if (!m_tables.empty())
		{
			for (Table* v : m_tables)
			{
				delete v;
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
	const vector<Table*>& User::getTables()const
	{
		ReadLock lock(m_mutex);
		return m_tables;
	}
	Table& User::getTable(const string& name)
	{
		int index = findTable(name);
		if ( index != -1)
		{
			ReadLock lock(m_mutex);
			return *m_tables[index];
		}
		return Table::NullRef;
	}
	const Table& User::getTable(const string& name) const
	{
		ReadLock lock(m_mutex);
		return getTable(name);
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

	void User::createTable(const string& name)
	{
		WriteLock lock(m_mutex);
		m_tables.push_back(new Table(name, *this));
	}
	void User::rename(string& name)
	{
		WriteLock lock(m_mutex);
		m_name = std::move(name);
	}
	void User::dropTable(const string& name)
	{
		int index = findTable(name);
		WriteLock lock(m_mutex);
		delete m_tables[index];
		remove(m_tables.begin(), m_tables.end(), index);
	}
	void User::renameTable(const string&  name, string&  newName)
	{
		int index = findTable(name);
		WriteLock lock(m_mutex);
		m_tables[index]->rename(newName);
	}
	void User::changePassword(const string& rawText)
	{
		HashContainer container;
		container.generate(rawText);
		WriteLock lock(m_mutex);
		m_password = container.moveHashHex();
	}
	void User::setPassword(string&  hash)
	{
		WriteLock lock(m_mutex);
		m_password = std::move(hash);
	}
	void User::setLevel(UserLevel level)
	{
		WriteLock lock(m_mutex);
		m_level = level;
	}

	void User::save(const string& fileName)
	{
		ofstream file(fileName,ios::out|ios::app);
		file << toString() << endl;
		file.close();
	}
	const string User::toString() const
	{
		std::ostringstream oss;
		WriteLock lock(m_mutex);
		oss << format("{},{},{},{}", m_id, m_name, m_password, static_cast<int>(m_level));
		for (int n = 0; n < m_tables.size(); ++n)
		{
			oss << m_tables[n]->getName();
		}
		return oss.str();
	}
	Table& User::operator[](const string&  name)
	{
		return getTable(name);
	}
}
