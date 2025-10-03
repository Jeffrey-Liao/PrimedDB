#pragma once
#include "Schema.h"
namespace liao::PrimedDB
{
	class TableManager :public Singleton<TableManager>
	{
		INVITESINGLETON;
		std::vector<TablePtr> m_tables;
		//read all table information from tables.struct
		TableManager();
	public:
		bool exist()const;
		void createSchema(User& creator, std::string& name);
		Schema& find(const std::string& name);
		Table& find(const std::string& schemaName, const std::string& tableName);
        void removeSchema(User& executor,const std::string& name);
        void removeTable(User& executor, const std::string& schemaName, const std::string& tableName);
		int size()const;
		std::vector<TablePtr>& all();
	};
}

