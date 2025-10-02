#pragma once
#include "Global.h"
#include "TableManager.h"
#include <ranges> 
namespace liao::PrimedDB {
	class Table;
	class User
	{
		mutable ShareMutex m_mutex;
		std::string m_id;
		std::string m_name;
		std::string m_password;
		UserLevel m_level;

		std::vector<TablePtr> m_tables;
		

	private:
		User(const User&) = delete;
		//unfinished, need to implement TableManager.
		void linkTables(const std::vector<std::string>&);
		int findTable(const std::string& name) const;

	public:
		User(const std::string& fileLine);
		User(std::string& name,std::string& password, UserLevel level);
		User(User&&) noexcept;
		~User();
		const std::string& getName()const;
        const std::string& getPassword()const;
        UserLevel getLevel()const;
        const std::string& getId()const;
        const std::vector<TablePtr>& getTables()const;
		TablePtr getTable(const std::string& name);
		const TablePtr getTable(const std::string& name) const;
		int tableCount() const;
		//1 GREATER, 0 EQUAL, -1 LESS
		int compare(const User& other) const;
		bool qualified(UserLevel level) const;
		bool validate(const std::string& password) const;
		static std::string PassWordHash(const std::string& rawText);
		TablePtr createTable(std::string schema,std::string name,UserLevel permission);
		bool rename(std::string& name);
        void dropTable(const std::string& name);
		void renameTable(const std::string& name, std::string& newName);
		void changePassword(const std::string& rawText);
		void setPassword(std::string& hash);
		void setLevel(UserLevel level);
		std::string toString() const;
		TablePtr operator[](const std::string& name);
		User& operator=(User&);
		bool operator==(const User&) const;
		static int size();
		static User createUser(std::string name,std::string password, UserLevel level);
	};
	using UserPtr = std::shared_ptr<User>;
}