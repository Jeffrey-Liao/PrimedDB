#pragma once
#include "Column.h"
namespace liao::PrimedDB
{
	class User;
	class Table
	{
		std::string m_name;
		std::vector<Column> m_columns;
		UserLevel m_permission;
		User& m_owner;
		mutable std::shared_mutex m_mutex;
		int m_recordNumber;
	private:

		Table();
	public:
		Table(const Table&) = delete;
		Table& operator=(const Table&) = delete;
		Table(Table&&) = delete;
		Table(std::string& name, User& owner, UserLevel permission);
		const std::string& getName() const;
		auto findColumn(const std::string& name);
		auto findColumn(const std::string& name) const;
		void clear();
        void addColumn(std::string& name, short int byteSize);
		void dropColumn(const std::string& name);
        void rename(std::string& name);
        void removeColumn(const std::string& name);
        void resizeColumn(const std::string& name, short int byteSize);
        const Column& getColumn(const std::string& name) const;
        const std::vector<Column>& getColumns() const;
        int getRecordNumber() const;
        void increaseRecordNumber();
		User& getOwner() const;
		~Table();
		static Table NullRef;
		static bool isNullObject(const Table&);
	};
}