#pragma once
#include "Column.h"
#include "DataCache.h"
#include "PrimeNumberConvert.h"

namespace liao::PrimedDB
{
	class User;
	//in "[tableName].def" file.
	class Table
	{
		std::string m_name;
		Math::GmpBigNumber m_prime;
		std::vector<ColumnPtr> m_columns;
		UserLevel m_permission;
		size_t m_recordNumber = 0;
		User& m_owner;
		mutable std::shared_mutex m_mutex;
		DataCache m_dataPool;

		void constructFromFile();
		void createTableDefFile();
	public:
		Table();
		Table(const Table&) = delete;
		Table(User& m_owner,const std::string& fileLine);
		Table& operator=(const Table&);
		Table(Table&&) = delete;
		Table(User& owner,std::string& name, UserLevel permission);
		const std::string& getName() const;
		auto findColumn(const std::string& name);
		auto findColumn(const std::string& name) const;
		bool exist(const std::string& name) const;
		UserLevel getPermission() const;
		void clear();
		int size()const;
        void addColumn(std::string& name, short int byteSize);
		void dropColumn(const std::string& name);
        void rename(std::string& name);
        void removeColumn(const std::string& name);
        void resizeColumn(const std::string& name, short int byteSize);
		void updateFile();
        const ColumnPtr getColumn(const std::string& name) const;
        const std::vector<ColumnPtr>& getColumns() const;
		size_t getRecordNumber() const;
        void increaseRecordNumber();
		const User& getOwner() const;
		std::string toString() const;
		bool isEmpty()const;
		~Table();
	};
	using TablePtr = std::shared_ptr<Table>;
}
