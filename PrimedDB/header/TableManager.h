#pragma once
#include "Global.h"
#include "Schema.h"
namespace liao::PrimedDB
{
	class TableManager :public Singleton<TableManager>
	{
		std::unordered_map <std::string, Schema> m_schemas;
	public:
		void createSchema(User& creator, std::string& name);
		Schema find(const std::string& name);
		Table& find(const std::string& schemaName, const std::string& tableName);
	};
}

