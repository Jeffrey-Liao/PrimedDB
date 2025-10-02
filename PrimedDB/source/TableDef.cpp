#include "Table.h"
#include "User.h"
USESTD;
namespace liao::PrimedDB
{
	Table::Table(Table&& move)
		:m_owner(move.m_owner),m_permission(move.m_permission),m_name(std::move(move.m_name)),m_columns(std::move(move.m_columns)),m_recordNumber(move.m_recordNumber)
	{}
	Table::Table(User& owner, std::string& name, UserLevel permission)
		:m_owner(owner),m_permission(permission)
	{
		createTableDefFile();
		rename(name);
	}
	void Table::createTableDefFile()
	{
		ofstream file(m_name + ".def",ios::app);
		if (file.is_open())
		{
			file.close();
		}
	}
	bool Table::isEmpty()const
	{
		return m_recordNumber == 0;
	}
	void Table::constructFromFile()
	{
		ifstream file(getName() + ".def");
		if (file.fail())
		{
			createTableDefFile();
		}
		else
		{
			{
				WriteLock lock(m_mutex);
				file >> m_name;
			}
			vector<string> lines;
			string cache;
			cache.reserve(TABLE_NAME_LEN + USER_NAME_LEN + sizeof(UserLevel) + sizeof(size_t));
            while (getline(file, cache))
            {
                lines.emplace_back(cache);
            }
			WriteLock lock(m_mutex);
			for (auto& line:lines)
			{
				m_columns.emplace_back(new Column(*this,line));
			}
		}
		file.close();
	}

	Table::Table(User& owner, const std::string& fileLine)
		:m_owner(owner),m_permission(UserLevel::None)
	{
		constructFromFile();
	}
	const std::string& Table::getName() const
	{
		ReadLock lock(m_mutex);
		return m_name;
	}
	void Table::addColumn(std::string& name, short int byteSize)
	{
		WriteLock lock(m_mutex);
		m_columns.emplace_back(new Column(name, byteSize, *this));
	}
	void Table::rename(std::string& name)
	{

		if (name != "null"&&!name.empty())
		{
			WriteLock lock(m_mutex);
			m_name = std::move(name);
		}
	}
	auto Table::findColumn(const std::string& name)
	{
		ReadLock lock(m_mutex);
		for (auto iter = m_columns.begin(); iter != m_columns.end(); ++iter)
		{
			if ((*iter)->getName() == name)
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
			if ((*iter)->getName() == name)
			{
				return iter;
			}
		}
		return m_columns.end();
	}
	bool Table::exist(const string& name) const
	{
		auto iter = findColumn(name);
		ReadLock lock(m_mutex);
		return iter != m_columns.end();
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
		auto& column = *(iter);
		ReadLock lock(m_mutex);
		if (iter != m_columns.end())
		{
			column->resize(byteSize);
		}
	}
	const ColumnPtr Table::getColumn(const std::string& name) const
	{
		auto iter = findColumn(name);
		ReadLock lock(m_mutex);
		if (iter != m_columns.end())
		{
			return *iter;
		}
		return nullptr;
	}
	const std::vector<ColumnPtr>& Table::getColumns() const
	{
		ReadLock lock(m_mutex);
		return m_columns;
	}
	size_t Table::getRecordNumber() const
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
	void Table::updateFile()
	{
		ofstream file(m_name + ".def",ios::trunc);
		if (!file.fail()&&file.is_open())
			file << toString();
		file.close();
	}
	int Table::size()const
	{
		return m_columns.size();
	}
	string Table::toString() const
	{
		std::ostringstream oss;
        ReadLock lock(m_mutex);
		oss<<format("{}:{}:{}:{}",m_owner.getName(), m_name,static_cast<int>(m_permission),this->m_recordNumber);
		for (int n =0;n<this->size();++n)
		{
			if (n+1<size())
			{
				oss << "\n";
			}
			oss<<m_columns[n]->toString();
		}
        return oss.str();
	}
	UserLevel Table::getPermission() const
	{
		return m_permission;
	}
	Table& Table::operator=(const Table& object)
	{
        m_name = object.m_name;
        m_permission = object.m_permission;
        m_owner = object.m_owner;
        m_columns = object.m_columns;
        m_recordNumber = object.m_recordNumber;
		return *this;
	}
	Table::~Table()
	{
		clear();
	}
}
