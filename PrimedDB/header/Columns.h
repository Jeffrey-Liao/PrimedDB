#pragma once
#include<vector>
#include<Log.h>
#include<Column.h>
#include<mutex>
#include<unordered_map>
namespace liao::db
{
	class Columns {
		std::unordered_map<std::string,Column*> m_allColumns;
		std::mutex m_mutex;
	private:
		void clear();
	public:
		//give name, byte size and table owns this column to allocate a new column
		const Column* create(std::string name, short int byteSize, Table* owner);
		bool remove(const Column* column);
		int size() const;
		bool contains(const Column* column) const;
		bool contains(const std::string& name) const;
		const Column* get(int index) const;
		const Column* get(std::string name) const;
        Columns();
		~Columns();
	};
}
