#include "Log.h"
#include "Setting.h"
#include "TableManager.h"
USESTD;
namespace liao::PrimedDB
{
	int TableManager::selectFromTables(std::string target, std::unordered_map <string, TablePtr>& applicant)
	{
		if (!applicant.contains(target))
			return -1;
		for (int n = 0;n<m_tables.size(); ++n)
		{
			if (m_tables[n]->getName() == target)
				return n;
		}
        return -1;
	}
	TableManager::TableManager()
	{
		m_tableFile.open(Util::Setting::Get().getTableFile(),ios::in|ios::out|ios::app);
		string buffer;
		vector<string> labels;
		labels.reserve(10);
		if (!m_tableFile.fail())
		{
			while (!m_tableFile.eof())
			{
				getline(m_tableFile, buffer);
				if (!buffer.empty())
				{
					m_tables.push_back(std::make_shared<Table>(buffer, labels));
					m_nameIndex[m_tables.back()->getName()]= m_tables.back();
				}

			}
			m_tableFile.clear();
		}
	}

	TablePtr TableManager::add(User& operater, std::string& name, set<Column>& columns, UserLevel level)
	{
		if (m_nameIndex.contains(name))
			return nullptr;
		UserLevel userLevel = operater.getLevel();
		if (level != UserLevel::None && level < userLevel)
			userLevel = level;
		auto ptr = std::make_shared<Table>(operater.getId(), name, userLevel, columns);
		WriteLock lock(m_listMutex);
		m_tables.push_back(ptr);
		m_nameIndex[ptr->getName()] = m_tables.back();
		m_tableFile<< ptr->toString() << std::endl;
		return m_tables.back();
	}
	bool TableManager::exist(const std::string& name)const
	{
		ReadLock lock(m_listMutex);
		return m_nameIndex.contains(name);
	}
	bool TableManager::drop(User& operater, const std::string& name)
	{
		bool success;
		{
			ReadLock lock(m_listMutex);
			success = m_nameIndex.contains(name) && m_nameIndex[name].use_count() == 2;
		}
		if (!success)
			Infor::Log::Get()[Infor::LogType::Info].openToFile("table_manager") << 
			std::format("{}-{} drop {} from the manager {}", operater.getId(), operater.getName(),name,success?"Success":"Failed");
		else
		{
			WriteLock lock(m_listMutex);
			m_nameIndex[name]->clear();
			m_tables.erase(m_tables.begin() + selectFromTables(name, m_nameIndex));
			m_nameIndex.erase(name);
		}
		return success;
	}

	std::optional<unsigned> TableManager::at(const std::string& name)
	{
		int result;
		{
			ReadLock lock(m_listMutex);
			result = selectFromTables(name, m_nameIndex);
		}
		if (result != -1)
			return result;
		else
			return nullopt;
	}
	TablePtr TableManager::get_noLock(const std::string& name)
	{
		if (m_nameIndex.contains(name))
			return m_nameIndex[name];
		else
			return nullptr;
	}
	ShareMutex& TableManager::getMutex()
	{
		return m_listMutex;
	}
	TablePtr TableManager::get(unsigned pos)
	{
		if (pos<m_tables.size())
		{
			return m_tables[pos];
		}
		else
			return nullptr;
	}
	void TableManager::transfer(User& owner, TablePtr table, User& receiver)
	{
		owner.dropTable(table->getName());
		table->setOwner(receiver.getId());
		receiver.addTable(table);
	}
	unsigned TableManager::size()const
	{
		return m_tables.size();
	}
	void TableManager::clear()
	{
		m_nameIndex.clear();
		m_tables.clear();
	}
	TableManager::~TableManager()
	{
		m_tableFile.close();
	}
}