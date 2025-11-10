#pragma once
#include "Global.h"
#include "Setting.h"
#include "Table.h"
namespace liao::PrimedDB
{
	class Block
	{
		std::string m_id;
		TablePtr m_owner;
		//memory
		char* m_memory = nullptr;
		//size of how many record in memory
		//the position of the first record in table
		unsigned m_start;
		int m_count;
		unsigned m_pointer = 0;
		mutable ShareMutex m_mutex;
		std::vector<char*> m_records;
		void allocate(std::shared_ptr<char> source = nullptr, unsigned size = 0);
		void deallocate();
	public:
		Block();
		Block(Block&)=delete;
		Block(Block&&) noexcept;
		void assign(TablePtr, unsigned);
		//check two Blocks are same object or not
		bool same(const Block&) const;
		//compare content in memory
		bool equal(const Block&);
		unsigned max() const;
		void writeLine(UCharPtr, unsigned pos);
		void flush();
		void update(unsigned location,std::shared_ptr<char[]> memory);
		bool empty();
		unsigned int size();
		unsigned byte() const;
		TablePtr getOwner();
		//build the link of records
		void build();
		//get the memory space of entire block
		char* reference();
		void remove(unsigned index);
		std::pair<unsigned, std::shared_ptr<char[]>> insert(std::shared_ptr<char[]> memory,unsigned number);
		char* get_noLock(unsigned index);
		void drop(unsigned index);
		double percentage();
		ShareMutex& getMutex();
		std::vector<char*>& getRecords();
		//==same
		bool operator==(const Block&)  const;
		Block& operator=(Block&&) noexcept;
		~Block();
	};
}
