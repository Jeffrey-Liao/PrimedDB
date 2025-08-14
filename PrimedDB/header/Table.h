#pragma once
#include "Column.h"
namespace liao::PrimedDB
{
	class User;
	class Table
	{
		std::string m_name;
		std::vector<Column*> m_columns;
		User& m_owner;
		std::shared_mutex m_mutex;
		int m_recordNumber;
	private:
        Table(const Table&) = delete;
        Table& operator=(const Table&) = delete;
        Table(Table&&) = delete;
	public:
		Table(const std::string& name, User& owner);
		std::string getName() const;
        void addColumn(const std::string& name, short int byteSize);
        void rename(const std::string& name);
        void removeColumn(const std::string& name);
        void resizeColumn(const std::string& name, short int byteSize);
        Column& getColumn(const std::string& name) const;
        std::vector<Column*> getColumns() const;
        int getRecordNumber() const;
        void increaseRecordNumber();
		User& getOwner() const;
		~Table();
		static Table NullRef;
	};
}