#include "BlockManager.h"
#include "Log.h"
#include "Setting.h"
#include "TableManager.h"
USESTD;
namespace liao::PrimedDB
{
	int TableManager::selectFromTables(std::string target, const std::unordered_map <string,TablePtr>& applicant)
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
	void TableManager::constructFromDataDefFile()
	{
		WriteLock lock(TableFileMutex);
		std::fstream m_tableFile(Util::Setting::Get().getTableFile(), ios::in | ios::out | ios::app);
		vector<string> labels;
		labels.reserve(10);
		if (!m_tableFile.fail())
		{
			StaticFunc::WriteInfo("TableManager", std::format("Initializing TableManager from definition file"));
			string buffer;
			while (!m_tableFile.eof())
			{
				getline(m_tableFile, buffer);
				if (!buffer.empty())
				{
					WriteLock lock(m_listMutex);
					m_tables.push_back(std::make_shared<Table>(buffer, labels));
					m_tables.back()->read();
					m_nameIndex[m_tables.back()->getName()] = m_tables.back();
					StaticFunc::WriteInfo("TableManager", std::format("Table {} Loaded", m_tables.back()->getName()));
				}
			}
			m_tableFile.clear();
		}
		m_tableFile.close();
		StaticFunc::WriteInfo("TableManager", std::format("Initialization complete"));
	}
	TableManager::TableManager()
	{
		constructFromDataDefFile();
		m_isBlockTerminate = BlockManager::Get().wait();
	}

	TablePtr TableManager::add(User& operater, std::string& name, deque<Column>& columns, UserLevel level)
	{
		{
			ReadLock lock(m_listMutex);
			if (m_nameIndex.contains(name))
				return m_nameIndex.at(name);
		}
		UserLevel userLevel = operater.getLevel();
		if (level != UserLevel::None && level < userLevel)
			userLevel = level;
		auto ptr = std::make_shared<Table>(operater.getId(), name, userLevel, columns);
		{
			WriteLock lock(m_listMutex);
			m_tables.push_back(ptr);
			m_nameIndex[ptr->getName()] = m_tables.back();
			m_tables.back()->read();
		}
		update();
		StaticFunc::WriteInfo("TableManager", std::format("New table {} created successfully by User {}", m_tables.back()->getName(), operater.getName()));
		return m_tables.back();
	}
	bool TableManager::exist(const std::string& name)const
	{
		ReadLock lock(m_listMutex);
		return m_nameIndex.contains(name);
	}
	bool TableManager::existColumn(const std::string& name)const
	{
		for (auto& p : m_tables)
		{
			if (p->existColumn(name))
				return true;
		}
		return false;
	}
	void TableManager::update()
	{
		WriteLock fileLock(TableFileMutex);
		if (m_tables.empty())
			return;
		fstream m_tableFile(Util::Setting::Get().getTableFile(), ios::out);
		StaticFunc::WriteInfo("TableManger", "Updating schema file");
		ReadLock lock(m_listMutex);
		for (auto& p : m_tables)
		{
			m_tableFile<<p->toString()<<std::endl;
		}
		m_tableFile.close();
		StaticFunc::WriteInfo("TableManger", "Updating complete");
	}
	bool TableManager::drop(User& operater, const std::string& name)
	{
		bool success;
		{
			WriteLock lock(m_listMutex);
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
		ReadLock lock(m_listMutex);
		if (m_nameIndex.contains(name))
			return m_nameIndex.at(name);
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
		WriteLock lock(m_listMutex);
		m_nameIndex.clear();
		m_tables.clear();
	}
	std::string TableManager::toString() const
	{
		string str;
		for (int n = 0 ;n<m_tables.size();++n)
		{
			str+= m_tables[n]->toString() + "\n";
		}
		return str;
	}
	std::string TableManager::format() const
	{
		string str;
		for (auto& table:m_tables)
		{
			str+= table->format() + "\n";
		}
		return str;
	}
	TableManager::~TableManager()
	{
		update();
		TableDead = true;
		if (BlockDead)
			clear();
	}
}