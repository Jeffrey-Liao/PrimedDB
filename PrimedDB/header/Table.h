#pragma once
#include <set>
#include "Record.h"
#include "Transection.h"
#include "Column.h"
#include "PrimeNumberConvert.h"

namespace liao::PrimedDB
{
	class Table
	{
		std::string m_name;
		std::vector<bool> m_available;
		std::deque<int> m_owned;
		UserLevel m_permission;
		unsigned m_size = 0;
		std::string m_ownerId;
		std::vector<Column> m_columns;
		mutable std::shared_mutex m_mutex;
		//byte size of record before primize
		unsigned m_byteSize;
		//byte size of record after primize
		unsigned m_primedSize;
		//the byte size of record number after all data
		unsigned m_recordByte;
		std::queue<Transection> m_pendingOperations;

		void construct(std::vector<std::string>&, const std::string& fileLine);
		std::pair<int, int> convertBlockPos(unsigned position);


	public:
		Table(const Table&) = delete;
		void operator=(const Table&) = delete;
		Table(const std::string& fileLine);
		Table(const std::string& fileLine, std::vector<std::string>&);
		Table& operator=(Table&&) noexcept;
		Table(Table&&) noexcept;
		Table(std::string ownerId,std::string& name, UserLevel permission, std::set<Column>&);
		//get_noLock name of the table
		const std::string& getName() const;
		//convert a record memory into primed format
		static void primize(UCharPtr& memory, unsigned byteSize, unsigned primedSize, unsigned recordSize);
		//convert a record memory into normal format
		static void deprimize(UCharPtr& memory, unsigned byteSize, unsigned primedSize, unsigned recordSize);
		//return the iterator of column
		auto findColumn(const std::string& name);
		auto findColumn(const std::string& name) const;
		//rename a column
		void renameColumn(const std::string& name, std::string& newName);
		//check is given name belongs to a column in the table
		bool existColumn(const std::string& name) const;
		//Get the states of all records, 1 is active, 0 is inactive
		std::vector<bool>& getAvailable();
		std::deque<int>&  getOwned();
		//return permission of the table
		UserLevel getPermission() const;
		//remove a record at given position
		void setUnavailable(unsigned pos);
		//recover a record at given position
		void setAvailable(unsigned pos);
		void clear();
		unsigned incrementSize();
		unsigned byte()const;
		unsigned primedByte()const;
		bool addColumn(std::string& name, unsigned byteSize,DataType);
		void dropColumn(const std::string& name);
        bool rename(std::string& name);
		unsigned byteSize()const;
		unsigned totalByte()const;
		size_t size() const;
		const std::string& getOwnerId() const;
		std::string toString();
		bool isEmpty()const;
		void insert(const std::string&,UCharPtr memory, unsigned size = 1);
		void update(const std::string&, unsigned position, UCharPtr memory, unsigned size = 1);
		void remove(const std::string&, unsigned position);
		void commit();
		void rollback(const std::string& name);
		void addBlock(unsigned pos);
		void dropBlock(unsigned pos);
		void dropBlockAt(unsigned pos);
		
		void setSize(unsigned newSize);
		void setOwner(const std::string& );
		Record select(const std::string& column = "all",bool raw = false);
		Record select(std::vector<std::string>&, bool raw = false);
		ShareMutex& getMutex();
		unsigned blockSize()const;
		~Table();
	};
	using TablePtr = std::shared_ptr<Table>;
}
