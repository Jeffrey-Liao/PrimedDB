#pragma once
#include <set>
#include <asio/detail/type_traits.hpp>

#include "Record.h"
#include "Transection.h"
#include "Column.h"
#include "PrimeNumberConvert.h"

namespace liao::PrimedDB
{

	class Table : public std::enable_shared_from_this<Table>
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

		void construct(std::vector<std::string>&, const std::string& fileLine);
		std::pair<int, int> convertBlockPos(unsigned position);
		void intoString(int index, std::deque<std::deque<std::shared_ptr<std::string>>>&, std::vector<char*>&,bool);

	public:
		Table(const Table&) = delete;
		void operator=(const Table&) = delete;
		Table(const std::string& fileLine);
		Table(const std::string& fileLine, std::vector<std::string>&);
		Table& operator=(Table&&) noexcept;
		Table(Table&&) noexcept;
		Table(std::string ownerId,std::string& name, UserLevel permission, std::deque<Column>&);
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
		Column& getColumn(const std::string& name);
		//remove a record at given position
		void setUnavailable(unsigned pos);
		//recover a record at given position
		void setAvailable(unsigned pos);
		void clear();
		bool read();
		unsigned incrementSize();
		unsigned decrementSize(unsigned pos);
		unsigned byte()const;
		unsigned primedByte()const;
		bool addColumn(std::string& name, unsigned byteSize,DataType);
		void dropColumn(const std::string& name);
        bool rename(std::string& name);
		unsigned columnSize() const;
		unsigned byteSize()const;
		unsigned totalByte()const;
		unsigned recordByte()const;
		size_t size() const;
		const std::string& getOwnerId() const;
		std::string toString();
		bool isEmpty()const;
		void insert(const std::string&,UCharPtr memory, unsigned size = 1);
		void update(const std::string&, unsigned position, UCharPtr memory, unsigned size = 1);
		void remove(const std::string&, unsigned position);
		void rollback(const std::string& name);
		void addBlock(unsigned pos);
		void dropBlock(unsigned pos);
		void dropBlockAt(unsigned pos);
		std::vector<bool> where(std::unordered_map<std::string,std::string>&);
		std::vector<Column>& getColumns();
		std::string format();
		void setSize(unsigned newSize);
		void setOwner(const std::string& );
		Record select(bool raw = false);
		Record select(std::deque<std::string>& list);
		ShareMutex& getMutex();
		unsigned blockSize()const;
		~Table();
	};
	using TablePtr = std::shared_ptr<Table>;
}
