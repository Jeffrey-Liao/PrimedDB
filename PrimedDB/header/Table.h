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
		Table(std::string name, User& owner);
		std::string getName() const;
        void addColumn(std::string name, short int byteSize);
        void rename(std::string name);
        void removeColumn(std::string name);
        void resizeColumn(std::string name, short int byteSize);
        Column& getColumn(std::string name) const;
        std::vector<Column*> getColumns() const;
        int getRecordNumber() const;
        void increaseRecordNumber();
		User& getOwner() const;
		~Table();
	};
}