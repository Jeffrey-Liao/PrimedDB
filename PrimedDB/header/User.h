#pragma once
#include "Global.h"
#include "TableManager.h"
#include <ranges> 
namespace liao::PrimedDB {
	class User
	{
		mutable ShareMutex m_mutex;
		std::string m_id;
		std::string m_name;
		std::string m_password;
		UserLevel m_level;
		std::unordered_map<std::string, TablePtr> m_tables;
		std::deque<Transaction> m_pendingOperations;
	private:
		User(const User&) = delete;
		void constructFromFile(const std::string&);
	public:
		User(const std::string& fileLine);
		User(std::string& id, std::string& name, std::string& password, UserLevel level);
		User(std::string& name,std::string& password, UserLevel level);
		User(User&&) noexcept;
		~User();
		const std::string& getName()const;
        const std::string& getPassword()const;
        UserLevel getLevel()const;
        const std::string& getId()const;
		//1 GREATER, 0 EQUAL, -1 LESS
		int compare(const User& other) const;
		bool qualified(UserLevel level) const;
		bool validate(const std::string& password) const;
		bool validateWithHash(const std::string& password) const;
		bool rename(std::string& name);

        const std::unordered_map<std::string,TablePtr>& getTables()const;
		TablePtr getTable(const std::string& name)const;
		unsigned tableCount() const;
		static std::string PassWordHash(const std::string& rawText);
		TablePtr createTable(std::string name,UserLevel permission, std::deque<Column>&);
        void dropTable(const std::string& name);
		void renameTable(const std::string& name, std::string& newName);
		void addTable(TablePtr table);
		void changePassword(const std::string& rawText);
		void setPassword(std::string& hash);
		void setLevel(UserLevel level);
		void submit(Transaction&& operation);
		void commit();
		std::string toString() const;
		TablePtr operator[](const std::string& name);
		User& operator=(User&);
		bool operator==(const User&) const;
		static User createUser(std::string name,std::string password, UserLevel level, std::string id = "");
	};
	using UserPtr = std::shared_ptr<User>;
}