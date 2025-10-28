#pragma once

#include <set>

#include "Singleton.h"
#include "Table.h"
#include "User.h"

namespace liao::PrimedDB
{
	class User;
	class TableManager :public Singleton<TableManager>
	{
		INVITESINGLETON;
		std::unordered_map<std::string, TablePtr> m_nameIndex;
		std::vector<TablePtr> m_tables;
		mutable ShareMutex m_listMutex;
		std::fstream m_tableFile;
		//read all table information from tables.struct
		int selectFromTables(std::string, std::unordered_map<std::string, TablePtr>&);
		TableManager();
	public: 
		TablePtr add(User&, std::string& name, std::set<Column>&,UserLevel level = UserLevel::None);
		bool exist(const std::string& name)const;
		bool drop(User&, const std::string& name);
		std::optional<unsigned> at(const std::string&);
		TablePtr get_noLock(const std::string&);
		ShareMutex& getMutex();
		TablePtr get(unsigned pos);
		static void transfer(User&,TablePtr,User&);
		unsigned size()const;
		void clear();
		~TableManager();
	};
}

