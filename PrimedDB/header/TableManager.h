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
		std::future<bool> m_isBlockTerminate;
		//read all table information from tables.struct
		void constructFromDataDefFile();
		void constructTableFromDataFile();
		int selectFromTables(std::string,const std::unordered_map<std::string, TablePtr>&);
		TableManager();
	public: 
		TablePtr add(User&, std::string& name, std::deque<Column>&,UserLevel level = UserLevel::None);
		bool exist(const std::string& name)const;
		bool drop(User&, const std::string& name);
		std::optional<unsigned> at(const std::string&);
		TablePtr get_noLock(const std::string&);
		ShareMutex& getMutex();
		TablePtr get(unsigned pos);
		bool existColumn(const std::string& name)const;
		void update();
		static void transfer(User&,TablePtr,User&);
		unsigned size()const;
		std::string toString() const;
		std::string format() const;
		void clear();
		~TableManager();
	};
}

